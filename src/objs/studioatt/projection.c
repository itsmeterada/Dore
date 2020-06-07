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
	int dor_projection_initialize ()
	int dor_projection_initialize_pick (object)
	dot_object * dor_projection_create (window, ptype, prp, view_plane, hither, yon)
	int dor_projection_destroy (object)
	int dor_projection_print (object)
	dor_projection_execute dor_projection_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>
#include <dore/internal/matrix.h>
#include <dore/dore_develop/private/prjmat.h>

/*
 ======================================================================
 */

DtInt doe_projection_class_id;

static DtMethodEntry projection_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_projection_print },
    { DcMethodDestroy,         (DtMethodPtr) dor_projection_destroy },
    { DcMethodIniPick,         (DtMethodPtr) dor_projection_initialize_pick },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_projection_execute },
};

void dor_projection_initialize (void)
{
    doe_projection_class_id = dor_class_install (DcTypeProjection, 
			"DoProjection", 4, projection_methods,DcNullPtr);
}
/*
 ======================================================================
 */

void dor_projection_initialize_pick (dot_object *object)
{
    struct projection_data *projectiondata;

    projectiondata = (struct projection_data *)object->data;

    dor_pick_set_projection_matrix(&projectiondata->window, 
				   projectiondata->ptype, 
				   projectiondata->prp,
				   projectiondata->view_plane, 
				   projectiondata->hither, 
				   projectiondata->yon) ;
}
/*
 ======================================================================
 */

dot_object *dor_projection_create (
    DtArea *window,
    DtProjectionType ptype,
    DtPoint3 prp,
    DtReal view_plane,
    DtReal hither,
    DtReal yon)
{
    static DtFlag initialized = DcFalse;
    dot_object *projectionobject;
    struct projection_data *projectiondata;

    if (!initialized) {
	dor_projection_initialize();
	initialized = DcTrue;
    }

    if (hither <= yon) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "DoProjection",
		    "hither <= yon - returning Null object");
	return (dot_object *)dor_null_create(DcNullPtr);
    }

    if (hither >= 0.0) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "DoProjection",
		    "hither >= 0.0 - returning Null object");
	return (dot_object *)dor_null_create(DcNullPtr);
    }
	   
    projectiondata = (struct projection_data *)dor_space_allocate(
				sizeof *projectiondata);
    projectiondata->window = *window ;
    projectiondata->ptype = ptype ;
    projectiondata->prp[0] = prp[0] ;
    projectiondata->prp[1] = prp[1] ;
    projectiondata->prp[2] = prp[2] ;
    projectiondata->view_plane = view_plane ;
    projectiondata->hither = hither ;
    projectiondata->yon = yon ;

    projectionobject = dor_object_create(doe_projection_class_id, projectiondata);

    return(projectionobject);
}
/*
 ======================================================================
 */

void dor_projection_destroy (dot_object *object)
{
    dor_space_deallocate(object->data);
}
/*
 ======================================================================
 */

void dor_projection_print (dot_object *object)
{
    struct projection_data *projectiondata;

    projectiondata = (struct projection_data *)object->data;

    if (projectiondata == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),
	    "window   ll = (%f, %f),  ur = (%f, %f)",
	    projectiondata->window.ll[0], projectiondata->window.ll[1],
	    projectiondata->window.ur[0], projectiondata->window.ur[1]) ;
    dor_print_output(dor_print_get_line());

    switch ( projectiondata->ptype ) {
    case DcParallel:
	dor_print_output("projection type = DcParallel") ;
	break ;
    case DcPerspective:
	dor_print_output("projection type = DcPerspective") ;
	break ;
    }

    sprintf(dor_print_get_line(),
	    "view_plane = %f, hither = %f, yon = %f", 
	    projectiondata->view_plane, 
	    projectiondata->hither, 
	    projectiondata->yon) ;
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_projection_execute (dot_object *object)
{
    struct projection_data *projectiondat;
    DtMatrix4x4 matrix;

    if (!dor_condex_query_execute(doe_projection_class_id))
	    return;

    projectiondat = (struct projection_data *)object->data;

    dor_matrix_load_projection (matrix, &(projectiondat->window),
				projectiondat->ptype,
				projectiondat->prp,
				projectiondat->view_plane,
				projectiondat->hither,
				projectiondat->yon);

    dor_global_cammat_set_value(DcCameraProjection, matrix);
}
