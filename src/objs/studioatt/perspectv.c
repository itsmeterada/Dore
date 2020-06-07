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
	int dor_perspective_initialize ()
	int dor_perspective_initialize_pick (object)
	dot_object * dor_perspective_create (fov,hither,yon)
	int dor_perspective_destroy (object)
	int dor_perspective_print (object)
	dor_perspective_execute dor_perspective_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>
#include <dore/internal/matrix.h>
#include <dore/dore_develop/private/permat.h>
#include <math.h>

/*
 ======================================================================
 */

DtInt doe_perspective_class_id;

static DtMethodEntry perspective_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_perspective_print },
    { DcMethodDestroy,         (DtMethodPtr) dor_perspective_destroy },
    { DcMethodIniPick,         (DtMethodPtr) dor_perspective_initialize_pick },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_perspective_execute },
};

void dor_perspective_initialize (void)
{
    doe_perspective_class_id = dor_class_install (DcTypePerspective, 
		"DoPerspective", 4, perspective_methods,DcNullPtr);
}
/*
 ======================================================================
 */

void dor_perspective_initialize_pick (dot_object *object)
{
    struct perspective_data *perspectivedat;

    perspectivedat = (struct perspective_data *)object->data;

    dor_pick_set_perspective_matrix(perspectivedat->fov_degree,
				    perspectivedat->hither,
				    perspectivedat->yon);
}
/*
 ======================================================================
 */

dot_object *dor_perspective_create (
    DtReal fov,
    DtReal hither,
    DtReal yon)
{
    static DtFlag initialized = DcFalse;
    struct perspective_data *perspectivedat;
    dot_object *perspectiveobj;

    if (!initialized) {
	dor_perspective_initialize();
	initialized = DcTrue;
    }

    if (hither <= yon) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "DoPerspective",
		    "hither <= yon - returning Null object");
	return (dot_object *)dor_null_create(DcNullPtr);
    }

    if (hither >= 0.0) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "DoPerspective",
		    "hither >= 0.0 - returning Null object");
	return (dot_object *)dor_null_create(DcNullPtr);
    }

    if (fov <= 0.0) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "DoPerspective",
		    "field of view <= 0.0 - returning Null object");
	return (dot_object *)dor_null_create(DcNullPtr);
    }
	   
    perspectivedat = (struct perspective_data *)dor_space_allocate(
				sizeof *perspectivedat);
	   
    perspectivedat = (struct perspective_data *)dor_space_allocate(
				sizeof *perspectivedat);

    if (dor_system_inq_angle_units() == DcAngleRadians) {
	perspectivedat->fov_degree = fov * 180.0 / M_PI;
    } else {
	perspectivedat->fov_degree = fov;
    }
    perspectivedat->hither = hither;
    perspectivedat->yon = yon;

    perspectiveobj = dor_object_create(doe_perspective_class_id,perspectivedat);

    return(perspectiveobj);
}
/*
 ======================================================================
 */

void dor_perspective_destroy (dot_object *object)
{
    dor_space_deallocate(object->data);
}
/*
 ======================================================================
 */

void dor_perspective_print (dot_object *object)
{
    struct perspective_data *perspectivedat;
    DtAngleUnits angle;
    DtReal radians;

    perspectivedat = (struct perspective_data *)object->data;

    if (perspectivedat == NULL) {
	dor_print_output("bad values");
	return;
    }

    angle = dor_system_inq_angle_units();
    switch (angle) {
        case DcAngleOldStyle:
        case DcAngleDegrees:
    	    sprintf(dor_print_get_line(),
	    		"fov = %f degrees, hither = %f, yon = %f",
	    		perspectivedat->fov_degree,
	    		perspectivedat->hither,
	    		perspectivedat->yon);
    	    dor_print_output(dor_print_get_line());
	    break;
        case DcAngleRadians:
	    radians = perspectivedat->fov_degree * M_PI / 180.;
    	    sprintf(dor_print_get_line(),
	    		"fov = %f radians, hither = %f, yon = %f",
			radians,
	    		perspectivedat->hither,
	    		perspectivedat->yon);
    	    dor_print_output(dor_print_get_line());
	    break;
	default:
	    break;
    }
}
/*
 ======================================================================
 */

void dor_perspective_execute  (dot_object *object)
{
    struct perspective_data *perspectivedat;
    DtMatrix4x4 matrix;

    if (!dor_condex_query_execute(doe_perspective_class_id))
	    return;

    perspectivedat = (struct perspective_data *)object->data;

    dor_matrix_load_perspective (matrix, perspectivedat->fov_degree,
				 perspectivedat->hither,
				 perspectivedat->yon);

    dor_global_cammat_set_value(DcCameraPerspective, matrix);
}
