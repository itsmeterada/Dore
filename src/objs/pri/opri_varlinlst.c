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
	dor_varlinlst_initialize 
	dor_varlinlst_create 
	dor_varlinlst_destroy 
	dor_varlinlst_print 
	dor_varlinlst_bounding_box 
	dor_varlinlst_compute_bounding_volume 
	dor_varlinlst_pick 
	dor_varlinlst_update

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/varlinlst.h>

/*
 ======================================================================
 */

DtInt doe_varlinlst_class_id;

static DtMethodEntry varlinlst_methods[] = {
    { DcMethodCmpBndVolume,(DtMethodPtr)dor_varlinlst_compute_bounding_volume },
    { DcMethodDestroy,     (DtMethodPtr)dor_varlinlst_destroy },
    { DcMethodPick,        (DtMethodPtr)dor_varlinlst_pick },
    { DcMethodPrint,       (DtMethodPtr)dor_varlinlst_print }
};

void dor_varlinlst_initialize (void)
{
    doe_varlinlst_class_id = dor_class_install (DcTypeVarLineList, "DoVarLineList", 
					4, varlinlst_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_varlinlst_create (
    DtColorModel colormodel,
    DtInt linecount,
    DtReal *vertices,
    DtReal *vertnormal,
    DtReal *vertcolor)
{
    static DtFlag initialized = DcFalse;
    struct varlinlst *varlinlst;
    dot_object *varlinlstobject;
   
    if (!initialized) {
	dor_varlinlst_initialize();
	initialized = DcTrue;
    }

    varlinlst = (struct varlinlst *)dor_space_allocate(sizeof *varlinlst);
   
    varlinlst->colormodel = colormodel;
    varlinlst->line_count = linecount;
    varlinlst->vertex_location = (DtRealTriple *)vertices;
    varlinlst->vertex_normal = (DtRealTriple *)vertnormal;
    varlinlst->vertex_color = (DtColorRGB *)vertcolor;
    varlinlst->sequence_number = 1;

    dor_varlinlst_bounding_box(varlinlst);

    varlinlstobject = dor_object_create(doe_varlinlst_class_id,varlinlst);
    return(varlinlstobject);
}
/*
 ======================================================================
 */

void dor_varlinlst_destroy (dot_object *object)
{
    struct varlinlst *varlinlst;

    varlinlst = (struct varlinlst *)(object->data);

    dor_space_deallocate(varlinlst);
}
/*
 ======================================================================
 */

void dor_varlinlst_print (dot_object *object)
{
    struct varlinlst *varlinlst;

    varlinlst = (struct varlinlst *)(object->data);

    if (varlinlst == NULL){
	dor_print_output("bad values");
	return;
    }

    switch (varlinlst->colormodel){
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;

    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    sprintf(dor_print_get_line(),
	    "vertex locations at [%lx]", varlinlst->vertex_location);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex normals at [%lx]", varlinlst->vertex_normal);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex colors at [%lx]", varlinlst->vertex_color);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"linecount = %d",varlinlst->line_count);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

void dor_varlinlst_bounding_box (struct varlinlst *varlinlst)
{
    DtReal xmin, ymin, zmin, xmax, ymax, zmax;
    DtInt vertexno;
    DtInt vertexcount;

    xmin = varlinlst->vertex_location[0][0];
    xmax = varlinlst->vertex_location[0][0];

    ymin = varlinlst->vertex_location[0][1];
    ymax = varlinlst->vertex_location[0][1];

    zmin = varlinlst->vertex_location[0][2];
    zmax = varlinlst->vertex_location[0][2];

    vertexcount = 2 * varlinlst->line_count;

PRAGMA(IVDEP)
PRAGMA(VBEST)
	    for (vertexno=1;vertexno<vertexcount;vertexno++){
		xmin = min(xmin,varlinlst->vertex_location[vertexno][0]);
		xmax = max(xmax,varlinlst->vertex_location[vertexno][0]);

		ymin = min(ymin,varlinlst->vertex_location[vertexno][1]);
		ymax = max(ymax,varlinlst->vertex_location[vertexno][1]);

		zmin = min(zmin,varlinlst->vertex_location[vertexno][2]);
		zmax = max(zmax,varlinlst->vertex_location[vertexno][2]);
	    }

    varlinlst->bounding_box_pts[0][0] = xmin;
    varlinlst->bounding_box_pts[0][1] = ymin;
    varlinlst->bounding_box_pts[0][2] = zmin;

    varlinlst->bounding_box_pts[1][0] = xmin;
    varlinlst->bounding_box_pts[1][1] = ymax;
    varlinlst->bounding_box_pts[1][2] = zmin;

    varlinlst->bounding_box_pts[2][0] = xmax;
    varlinlst->bounding_box_pts[2][1] = ymax;
    varlinlst->bounding_box_pts[2][2] = zmin;

    varlinlst->bounding_box_pts[3][0] = xmax;
    varlinlst->bounding_box_pts[3][1] = ymin;
    varlinlst->bounding_box_pts[3][2] = zmin;

    varlinlst->bounding_box_pts[4][0] = xmin;
    varlinlst->bounding_box_pts[4][1] = ymin;
    varlinlst->bounding_box_pts[4][2] = zmax;

    varlinlst->bounding_box_pts[5][0] = xmin;
    varlinlst->bounding_box_pts[5][1] = ymax;
    varlinlst->bounding_box_pts[5][2] = zmax;

    varlinlst->bounding_box_pts[6][0] = xmax;
    varlinlst->bounding_box_pts[6][1] = ymax;
    varlinlst->bounding_box_pts[6][2] = zmax;

    varlinlst->bounding_box_pts[7][0] = xmax;
    varlinlst->bounding_box_pts[7][1] = ymin;
    varlinlst->bounding_box_pts[7][2] = zmax;
}
/*
 ======================================================================
 */

void dor_varlinlst_compute_bounding_volume (dot_object *object)
{
    struct varlinlst *varlinlst;

    varlinlst = (struct varlinlst *)(object->data);

    dor_system_report_bounding_points (8, (DtReal*)varlinlst->bounding_box_pts);
}

/*
 ======================================================================
 */

void dor_varlinlst_pick (dot_object *object)
{
    struct varlinlst *varlinlst;

    if (!dor_condex_query_pick(doe_varlinlst_class_id))
	    return;

    if (!doe_system_alternate_object_execution_flag) {
	dor_pick_initialize_object(object);
    }
   
    varlinlst = (struct varlinlst *)(object->data);

    dor_pick_line_list(varlinlst->bounding_box_pts,
		       varlinlst->line_count,
		       varlinlst->vertex_location);
}
/*
 ======================================================================
 */

void dor_varlinlst_update (
    dot_object *object,
    DtInt linecnt,
    DtReal *vlocs,
    DtReal *vnrms,
    DtReal *vclrs)
{
    struct varlinlst *varlinlst;

#ifdef ERRCHK
    DtInt type;

    if ((DcObjectValid != dor_object_validate(object)) ||
	(((type = dor_object_inq_type(object)) != doe_varlinlst_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarLineList",DcNullPtr);
	return;
    }
#endif

    varlinlst = (struct varlinlst *)(object->data);

    varlinlst->sequence_number++;

    if (linecnt != 0) {
	varlinlst->line_count = linecnt;
    }
   
    if (vlocs != NULL) {
	varlinlst->vertex_location = (DtRealTriple *)vlocs;

	dor_varlinlst_bounding_box(varlinlst);
    }


    /* If object was created without vertex normals then you cannot
       pass them in now */
	
    if (vnrms != NULL) {
	if (varlinlst->vertex_normal != NULL ) 
		varlinlst->vertex_normal = (DtRealTriple *)vnrms;
	else
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "dor_varlinlst_update", 
			    "vertex normals ignored");
    }	


    /* If object was created without colors then you cannot 
       pass them in now */

    if (vclrs != NULL) {
	if (varlinlst->vertex_color != NULL)
		varlinlst->vertex_color = (DtColorRGB *)vclrs;
	else 
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "dor_varlinlst_update", 
			    "vertex colors ignored");
    }

}
