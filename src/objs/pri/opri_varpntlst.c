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
	dor_varpntlst_initialize 
	dor_varpntlst_create 
	dor_varpntlst_destroy 
	dor_varpntlst_print 
	dor_varpntlst_bounding_box 
	dor_varpntlst_compute_bounding_volume 
	dor_varpntlst_pick 
	dor_varpntlst_update 

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/varpntlst.h>

/*
 ======================================================================
 */

DtInt doe_varpntlst_class_id;

static DtMethodEntry varpntlst_methods[] = {
    { DcMethodCmpBndVolume,(DtMethodPtr) dor_varpntlst_compute_bounding_volume},
    { DcMethodDestroy,     (DtMethodPtr) dor_varpntlst_destroy},
    { DcMethodPick,        (DtMethodPtr) dor_varpntlst_pick},
    { DcMethodPrint,       (DtMethodPtr) dor_varpntlst_print}
};

void dor_varpntlst_initialize (void)
{
    doe_varpntlst_class_id = dor_class_install (DcTypeVarPointList, 
			"DoVarPointList", 4, varpntlst_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_varpntlst_create (
    DtColorModel colormodel,
    DtInt pointcount,
    DtReal *vertices,
    DtReal *vertnormal,
    DtReal *vertcolor)
{
    static DtFlag initialized = DcFalse;
    struct varpntlst *varpntlst;
    dot_object *varpntlstobject;
   
    if (!initialized) {
	dor_varpntlst_initialize();
	initialized = DcTrue;
    }

    varpntlst = (struct varpntlst *)dor_space_allocate(sizeof *varpntlst);
   
    varpntlst->colormodel = colormodel;
    varpntlst->point_count = pointcount;
    varpntlst->vertex_location = (DtRealTriple *)vertices;
    varpntlst->vertex_normal = (DtRealTriple *)vertnormal;
    varpntlst->vertex_color = (DtColorRGB *)vertcolor;
    varpntlst->sequence_number = 1;

    dor_varpntlst_bounding_box(varpntlst);

    varpntlstobject = dor_object_create(doe_varpntlst_class_id,varpntlst);
    return(varpntlstobject);
}
/*
 ======================================================================
 */

void dor_varpntlst_destroy (dot_object *object)
{
    struct varpntlst *varpntlst;

    varpntlst = (struct varpntlst *)(object->data);

    dor_space_deallocate(varpntlst);
}
/*
 ======================================================================
 */

void dor_varpntlst_print (dot_object *object)
{
    struct varpntlst *varpntlst;

    varpntlst = (struct varpntlst *)(object->data);

    if (varpntlst == NULL){
	dor_print_output("bad values");
	return;
    }

    switch (varpntlst->colormodel){
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;

    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    sprintf(dor_print_get_line(),
	    "vertex locations at [%lx]", varpntlst->vertex_location);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex normals at [%lx]", varpntlst->vertex_normal);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex colors = [%lx]", varpntlst->vertex_color);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"point count = %d",varpntlst->point_count);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

void dor_varpntlst_bounding_box (struct varpntlst *varpntlst)
{
    DtReal xmin, ymin, zmin, xmax, ymax, zmax;
    DtInt vertexno;
    DtInt vertexcount;

    xmin = varpntlst->vertex_location[0][0];
    xmax = varpntlst->vertex_location[0][0];

    ymin = varpntlst->vertex_location[0][1];
    ymax = varpntlst->vertex_location[0][1];

    zmin = varpntlst->vertex_location[0][2];
    zmax = varpntlst->vertex_location[0][2];

    vertexcount = varpntlst->point_count;

PRAGMA(IVDEP)
PRAGMA(VBEST)
	    for (vertexno=1;vertexno<vertexcount;vertexno++){
		xmin = min(xmin,varpntlst->vertex_location[vertexno][0]);
		xmax = max(xmax,varpntlst->vertex_location[vertexno][0]);

		ymin = min(ymin,varpntlst->vertex_location[vertexno][1]);
		ymax = max(ymax,varpntlst->vertex_location[vertexno][1]);

		zmin = min(zmin,varpntlst->vertex_location[vertexno][2]);
		zmax = max(zmax,varpntlst->vertex_location[vertexno][2]);
	    }

    varpntlst->bounding_box_pts[0][0] = xmin;
    varpntlst->bounding_box_pts[0][1] = ymin;
    varpntlst->bounding_box_pts[0][2] = zmin;

    varpntlst->bounding_box_pts[1][0] = xmin;
    varpntlst->bounding_box_pts[1][1] = ymax;
    varpntlst->bounding_box_pts[1][2] = zmin;

    varpntlst->bounding_box_pts[2][0] = xmax;
    varpntlst->bounding_box_pts[2][1] = ymax;
    varpntlst->bounding_box_pts[2][2] = zmin;

    varpntlst->bounding_box_pts[3][0] = xmax;
    varpntlst->bounding_box_pts[3][1] = ymin;
    varpntlst->bounding_box_pts[3][2] = zmin;

    varpntlst->bounding_box_pts[4][0] = xmin;
    varpntlst->bounding_box_pts[4][1] = ymin;
    varpntlst->bounding_box_pts[4][2] = zmax;

    varpntlst->bounding_box_pts[5][0] = xmin;
    varpntlst->bounding_box_pts[5][1] = ymax;
    varpntlst->bounding_box_pts[5][2] = zmax;

    varpntlst->bounding_box_pts[6][0] = xmax;
    varpntlst->bounding_box_pts[6][1] = ymax;
    varpntlst->bounding_box_pts[6][2] = zmax;

    varpntlst->bounding_box_pts[7][0] = xmax;
    varpntlst->bounding_box_pts[7][1] = ymin;
    varpntlst->bounding_box_pts[7][2] = zmax;
}
/*
 ======================================================================
 */

void dor_varpntlst_compute_bounding_volume (dot_object *object)
{
    struct varpntlst *varpntlst;

    varpntlst = (struct varpntlst *)(object->data);

    dor_system_report_bounding_points (8, (DtReal*)varpntlst->bounding_box_pts);
}

/*
 ======================================================================
 */

void dor_varpntlst_pick (dot_object *object)
{
    struct varpntlst *varpntlst;

    if (!dor_condex_query_pick(doe_varpntlst_class_id))
	    return;

    if (!doe_system_alternate_object_execution_flag) {
	dor_pick_initialize_object(object);
    }
   
    varpntlst = (struct varpntlst *)(object->data);

    dor_pick_point_list(varpntlst->bounding_box_pts,
			varpntlst->point_count,
			varpntlst->vertex_location);
}
/*
 ======================================================================
 */

void dor_varpntlst_update (
    dot_object *object,
    DtInt pointcnt,
    DtReal *vlocs,
    DtReal *vnrms,
    DtReal *vclrs)
{
    struct varpntlst *varpntlst;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_varpntlst_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarPointList",DcNullPtr);
	return;
    }   
#endif

    varpntlst = (struct varpntlst *)(object->data);

    varpntlst->sequence_number++;

    if (pointcnt != 0) {
	varpntlst->point_count = pointcnt;
    }
   
    if (vlocs != NULL) {
	varpntlst->vertex_location = (DtRealTriple *)vlocs;

	dor_varpntlst_bounding_box(varpntlst);
    }


    /* If object was created without vertex normals then you cannot
       pass them in now */
	
    if (vnrms != NULL) {
	if (varpntlst->vertex_normal != NULL ) 
		varpntlst->vertex_normal = (DtRealTriple *)vnrms;
	else
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "dor_varpntlst_update", 
			    "vertex normals ignored");
    }	


    /* If object was created without colors then you cannot 
       pass them in now */

    if (vclrs != NULL) {
	if (varpntlst->vertex_color != NULL)
		varpntlst->vertex_color = (DtColorRGB *)vclrs;
	else 
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "dor_varpntlst_update", 
			    "vertex colors ignored");
    }

}
