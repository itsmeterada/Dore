/**************************************************************************
# Copyright (C) 1994 Kubota Graphics Corp.
# 
# Permission to use, copy, modify, and distribute this material for
# any purpose and without fee is hereby granted, provided that the
# above copyright notice and this permission notice appear in all
# copies, and that the name of Kubota Graphics not be used in
# advertising or publicity pertaining to this material.  Kubota
# Graphics Corporation MAKES NO REPRESENTATIONS ABOUT THE ACCURACY
# OR SUITABILITY OF THIS MATERIAL FOR ANY PURPOSE.  IT IS PROVIDED
# "AS IS", WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE AND KUBOTA GRAPHICS CORPORATION DISCLAIMS ALL WARRANTIES,
# EXPRESS OR IMPLIED.
**************************************************************************/
 
/*
  ======================================================================
  Functions:
	int dor_bndvol_initialize ()
	dot_object * dor_bndvol_create (volume, alternateobject)
	int dor_bndvol_destroy (object)
	int dor_bndvol_print (object)
	int dor_bndvol_execute (object)
	DtReal dor_bndvol_compute_current_extension2 (bv)
	int dor_bndvol_device_coord_compare (point1, point2, point3, clipped, data)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/internal/priatt.h>
#include <dore/internal/clip3d.h>
#include <dore/internal/matrix.h>
#include <dore/dore_develop/private/device.h>

#define LARGE_NUMBER 88888888.

struct bndvol_data {
    DtVolume volume;
    DtObject alternate_object;
};

DtReal 
    doe_bndvol_low_x, doe_bndvol_low_y, 
    doe_bndvol_high_x, doe_bndvol_high_y;

DtFlag doe_bndvol_visible;
DtFlag doe_bndvol_clipped;
DtReal doe_bndvol_ctodscale[3];
DtReal doe_bndvol_ctodtrans[3];

/*
 ======================================================================
 */

DtInt doe_bndvol_class_id;

static DtMethodEntry bndvol_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_bndvol_destroy },
    { DcMethodPrint,            (DtMethodPtr) dor_bndvol_print   },
    { DcMethodPick,             (DtMethodPtr) dor_bndvol_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_bndvol_execute },
};

void dor_bndvol_initialize (void)
{
    doe_bndvol_class_id = dor_class_install (
			      DcTypeBoundingVolume, "DoBoundingVol", 
			      4, bndvol_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_bndvol_create (
    DtVolume *volume,
    DtObject  alternateobject)
{
    static DtFlag initialized = DcFalse;
    dot_object *bndvolobject;
    struct bndvol_data *bndvoldata;

    if (!initialized) {
	dor_bndvol_initialize();
	initialized = DcTrue;
    }

    bndvoldata = (struct bndvol_data *)
	    dor_space_allocate(sizeof *bndvoldata);

    bndvoldata->volume = *volume;
    bndvoldata->alternate_object = alternateobject;

    bndvolobject = dor_object_create(doe_bndvol_class_id, bndvoldata);

    if(DcNullObject != alternateobject){
	dor_object_add_reference(alternateobject, bndvolobject);
    }

    return(bndvolobject);
}
/*
 ======================================================================
 */

void dor_bndvol_destroy (dot_object *object)
{
    struct bndvol_data *bndvoldata;

    bndvoldata = (struct bndvol_data *)object->data;

    if(DcNullObject != bndvoldata->alternate_object){
	dor_object_delete_reference(bndvoldata->alternate_object, object);
    }

    dor_space_deallocate(bndvoldata);
}
/*
 ======================================================================
 */

void dor_bndvol_print (dot_object *object)
{
    struct bndvol_data *bndvoldata;

    bndvoldata = (struct bndvol_data *)object->data;

    if (bndvoldata == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(), 
	    "bll = (%f, %f, %f)\n",
	    bndvoldata->volume.bll[0], 
	    bndvoldata->volume.bll[1], 
	    bndvoldata->volume.bll[2]);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(), 
	    "fur = (%f, %f, %f)\n",
	    bndvoldata->volume.fur[0], 
	    bndvoldata->volume.fur[1], 
	    bndvoldata->volume.fur[2]);
    dor_print_output(dor_print_get_line());

    dor_print_output("alternate object:");

    if (bndvoldata->alternate_object != DcNullObject){
	dor_object_print(bndvoldata->alternate_object);
    }
    else{
	dor_print_output ("Null");
    }
}
/*
 ======================================================================
 */

void dor_bndvol_execute (dot_object *object)
{
    struct bndvol_data *bndvoldata;
    DtSwitch curbndvolswi;
    DtReal curminextension2, curextension2;

    if( ! dor_condex_query_execute(doe_bndvol_class_id))
	    return;

    dor_global_bndvolswi_get_value(&curbndvolswi);

    if(DcOff == curbndvolswi)
	    return;

    bndvoldata = (struct bndvol_data *)object->data;

    dor_global_minbndext_get_value(&curminextension2);

#ifdef DEBUG
    printf ("MinBoundingVolExt: %g\n", curminextension2);
#endif

    curextension2 = dor_bndvol_compute_current_extension2(&bndvoldata->volume);

#ifdef DEBUG
    printf ("Current Extension: %g\n", curextension2);
#endif

    if(curextension2 < curminextension2){
	if(curextension2 > (DtReal)0.){
	    if(bndvoldata->alternate_object != DcNullObject)
		    dor_system_execute_object(bndvoldata->alternate_object);
	}
	dor_group_traversal_return();
    }
}
/*
 ======================================================================
 */

/*  Returns the square of the length in device coordinates of the diagonal of 
    the smallest right rectangle that completely encloses in x and y the 
    intersection of the given volume with the device viewport.  */

DtReal dor_bndvol_compute_current_extension2 (DtVolume *bv)
{
    DtReal return_value;
    DtPoint3 luf, lub, ruf, rub, llf, llb, rlf, rlb; 
    DtMatrix4x4 wcstofcsmat, lcstofcsmat, fcstolcsmat;
    DtReal dx, dy;
    DtObject device_object;

    /* get eight corners of the box from bll and fur */

    luf[0] = bv->bll[0];
    luf[1] = bv->fur[1];
    luf[2] = bv->fur[2];
    lub[0] = bv->bll[0];
    lub[1] = bv->fur[1];
    lub[2] = bv->bll[2];

    ruf[0] = bv->fur[0];
    ruf[1] = bv->fur[1];
    ruf[2] = bv->fur[2];
    rub[0] = bv->fur[0];
    rub[1] = bv->fur[1];
    rub[2] = bv->bll[2];

    llf[0] = bv->bll[0];
    llf[1] = bv->bll[1];
    llf[2] = bv->fur[2];
    llb[0] = bv->bll[0];
    llb[1] = bv->bll[1];
    llb[2] = bv->bll[2];

    rlf[0] = bv->fur[0];
    rlf[1] = bv->bll[1];
    rlf[2] = bv->fur[2];
    rlb[0] = bv->fur[0];
    rlb[1] = bv->bll[1];
    rlb[2] = bv->bll[2];

    doe_bndvol_visible = DcFalse;
    doe_bndvol_low_x = LARGE_NUMBER;
    doe_bndvol_high_x = -LARGE_NUMBER;
    doe_bndvol_low_y = LARGE_NUMBER;
    doe_bndvol_high_y = -LARGE_NUMBER;

    dor_render_get_wcstofcsmat(wcstofcsmat);
    dor_global_lcstowcsmat_get_value(lcstofcsmat);
    dor_matrix_post_concatenate(lcstofcsmat,wcstofcsmat);

    device_object = dor_device_return_current_device_object();
    if (dor_object_confirm_type(device_object,DsInqClassId("DoDevice"))){
	dor_device_get_scltrn_parms(device_object, 
				    doe_bndvol_ctodscale,
				    doe_bndvol_ctodtrans);
    } else {
	doe_bndvol_ctodscale[0] = doe_bndvol_ctodscale[1] = 
		doe_bndvol_ctodscale[2] = 1.;
	doe_bndvol_ctodtrans[0] = doe_bndvol_ctodtrans[1] = 
		doe_bndvol_ctodtrans[2] = 0.;
    }
    /* front */

    dor_clip3d_test_triangle_clip_depth_only(llf,luf,ruf,lcstofcsmat,
			   dor_bndvol_device_coord_compare, DcNullPtr);
    dor_clip3d_test_triangle_clip_depth_only(llf,ruf,rlf,lcstofcsmat,
			   dor_bndvol_device_coord_compare, DcNullPtr);

    /* right */

    dor_clip3d_test_triangle_clip_depth_only(rlf,ruf,rub,lcstofcsmat,
			   dor_bndvol_device_coord_compare, DcNullPtr);
    dor_clip3d_test_triangle_clip_depth_only(rlf,rub,rlb,lcstofcsmat,
			   dor_bndvol_device_coord_compare, DcNullPtr);

    /* back */

    dor_clip3d_test_triangle_clip_depth_only(rlb,rub,lub,lcstofcsmat,
			   dor_bndvol_device_coord_compare, DcNullPtr);
    dor_clip3d_test_triangle_clip_depth_only(rlb,lub,llb,lcstofcsmat,
			   dor_bndvol_device_coord_compare, DcNullPtr);

    /* left */

    dor_clip3d_test_triangle_clip_depth_only(llb,lub,luf,lcstofcsmat,
			   dor_bndvol_device_coord_compare, DcNullPtr);
    dor_clip3d_test_triangle_clip_depth_only(llb,luf,llf,lcstofcsmat,
			   dor_bndvol_device_coord_compare, DcNullPtr);

    /* top */

    dor_clip3d_test_triangle_clip_depth_only(luf,lub,rub,lcstofcsmat,
			   dor_bndvol_device_coord_compare, DcNullPtr);
    dor_clip3d_test_triangle_clip_depth_only(luf,rub,ruf,lcstofcsmat,
			   dor_bndvol_device_coord_compare, DcNullPtr);

    /* bottom */

    dor_clip3d_test_triangle_clip_depth_only(llb,llf,rlf,lcstofcsmat,
			   dor_bndvol_device_coord_compare, DcNullPtr);
    dor_clip3d_test_triangle_clip_depth_only(llb,rlf,rlb,lcstofcsmat,
			   dor_bndvol_device_coord_compare, DcNullPtr);

    if (doe_bndvol_visible){
#ifdef DEBUG
	printf ("Bounding Box Visible\n");
#endif
	dx = doe_bndvol_high_x - doe_bndvol_low_x;
	dy = doe_bndvol_high_y - doe_bndvol_low_y;

	return_value = dx*dx + dy*dy;
    }
    else {
	/*  All triangles that border the bounding volume are completely 
	    clipped. This can happen if the transformed volume is 
	    completely outside the actual device viewport (in which 
	    case it's extent is zero), or if the actual view frustum 
	    is completely contained within the bounding volume */

	DtReal invec[4], outvec[4];

#ifdef DEBUG
	printf ("Bounding Box Not Visible...\n");
#endif
	/*  Get matrix that transforms actual viewport coordinates to
	    local coords.  */

	dor_matrix_load(fcstolcsmat,lcstofcsmat);
	dor_matrix_invert(fcstolcsmat);

	/* back transform center of actual viewport into local coords */

	invec[0] = 0.;
	invec[1] = 0.;
	invec[2] = -.5;
	invec[3] = 1.;
	dor_math_homo_trns_fast(invec, fcstolcsmat, outvec);
	if (outvec[3] != 0.) {
	    outvec[0] /= outvec[3];
	    outvec[1] /= outvec[3];
	    outvec[2] /= outvec[3];
	}

	if ((bv->bll[0] > outvec[0]) || (outvec[0] > bv->fur[0]) ||
	    (bv->bll[1] > outvec[1]) || (outvec[1] > bv->fur[1]) ||
	    (bv->bll[2] > outvec[2]) || (outvec[2] > bv->fur[2])){

            /* bounding volume entirely clipped i.e. not visible */
	    return_value = (DtReal) 0.;
	}
	else{

            /* Entire view frustum is enclosed in bounding volume.
               This means that we are very close to the objects
               inside the bounding volume so we don't want to use
               any simpler alternate representation. We return
               a high number to indicate that */

            return_value = LARGE_NUMBER;
            
        }
    }
    return(return_value);
}
/*
 ======================================================================
 */

/* Transform triangle (which has just been clipped in hither and yon)
   into device coordinates; compares transformed values to current
   bounding box minima and maxima */

void dor_bndvol_device_coord_compare (
    dot_clip3d_vertex *point1,
    dot_clip3d_vertex *point2,
    dot_clip3d_vertex *point3,
    DtInt              clipped,
    DtPtr              data)
{
    DtReal tmp;
    DtPoint3 p1, p2, p3;

    p1[0] = point1->loc[0];
    p1[1] = point1->loc[1];
    p2[0] = point2->loc[0];
    p2[1] = point2->loc[1];
    p3[0] = point3->loc[0];
    p3[1] = point3->loc[1];

    if (point1->loc[3] != 0.) {
	p1[0] /= point1->loc[3];
	p1[1] /= point1->loc[3];
    }
    p1[0] = p1[0] * doe_bndvol_ctodscale[0] + doe_bndvol_ctodtrans[0];
    p1[1] = p1[1] * doe_bndvol_ctodscale[1] + doe_bndvol_ctodtrans[1];

    if (point2->loc[3] != 0.) {
	p2[0] /= point2->loc[3];
	p2[1] /= point2->loc[3];
    }
    p2[0] = p2[0] * doe_bndvol_ctodscale[0] + doe_bndvol_ctodtrans[0];
    p2[1] = p2[1] * doe_bndvol_ctodscale[1] + doe_bndvol_ctodtrans[1];

    if (point3->loc[3] != 0.) {
	p3[0] /= point3->loc[3];
	p3[1] /= point3->loc[3];
    }
    p3[0] = p3[0] * doe_bndvol_ctodscale[0] + doe_bndvol_ctodtrans[0];
    p3[1] = p3[1] * doe_bndvol_ctodscale[1] + doe_bndvol_ctodtrans[1];

    doe_bndvol_visible = DcTrue;

    if ((tmp = p1[0]) < doe_bndvol_low_x )   doe_bndvol_low_x  = tmp;
    if ((tmp = p2[0]) < doe_bndvol_low_x )   doe_bndvol_low_x  = tmp;
    if ((tmp = p3[0]) < doe_bndvol_low_x )   doe_bndvol_low_x  = tmp;
    if ((tmp = p1[0]) > doe_bndvol_high_x)   doe_bndvol_high_x = tmp;
    if ((tmp = p2[0]) > doe_bndvol_high_x)   doe_bndvol_high_x = tmp;
    if ((tmp = p3[0]) > doe_bndvol_high_x)   doe_bndvol_high_x = tmp;
    if ((tmp = p1[1]) < doe_bndvol_low_y )   doe_bndvol_low_y  = tmp;
    if ((tmp = p2[1]) < doe_bndvol_low_y )   doe_bndvol_low_y  = tmp;
    if ((tmp = p3[1]) < doe_bndvol_low_y )   doe_bndvol_low_y  = tmp;
    if ((tmp = p1[1]) > doe_bndvol_high_y)   doe_bndvol_high_y = tmp;
    if ((tmp = p2[1]) > doe_bndvol_high_y)   doe_bndvol_high_y = tmp;
    if ((tmp = p3[1]) > doe_bndvol_high_y)   doe_bndvol_high_y = tmp;
}
