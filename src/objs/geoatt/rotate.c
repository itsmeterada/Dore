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
	dor_rotate_initialize
	dor_rotate_create
	dor_rotate_destroy
	dor_rotate_print
	dor_rotate_execute

  ======================================================================
 */
#include <math.h>
#include <dore/internal/dogen.h>
#include <dore/internal/geoatt.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/system.h>
#include <dore/internal/matrix.h>
#include <dore/dore_develop/private/rotate.h>

/*
 ======================================================================
 */

DtInt doe_rotate_class_id;

static DtMethodEntry rotate_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_rotate_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_rotate_destroy },
    { DcMethodIniPick,          (DtMethodPtr) dor_rotate_execute },
    { DcMethodPick,             (DtMethodPtr) dor_rotate_execute },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_rotate_execute },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_rotate_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_rotate_execute },
};

void dor_rotate_initialize (void)
{
    doe_rotate_class_id = dor_class_install (DcTypeRotate, "DoRotate", 
			7,rotate_methods, DcNullPtr);
}
/*
 ======================================================================
 */

struct object *dor_rotate_create (
    DtAxis axis,
    DtReal angle)
{
    static DtFlag initialized = DcFalse;
    struct rotate_data *rotdat;
    dot_object *rotobj;

    if (!initialized) {
	dor_rotate_initialize();
	initialized = DcTrue;
    }

    rotdat = dor_space_allocate (sizeof (*rotdat));

    if (dor_system_inq_angle_units() == DcAngleDegrees) {
	rotdat->radians = angle * M_PI / 180.0;
    } else {
	rotdat->radians = angle;
    }
    rotdat->axis = axis;

    rotobj = dor_object_create(doe_rotate_class_id,rotdat);

    return(rotobj);
}
/*
 ======================================================================
 */

void dor_rotate_destroy (dot_object *object)
{
    dor_space_deallocate(object->data);
}
/*
 ======================================================================
 */

void dor_rotate_print (dot_object *object)
{
    struct rotate_data *rotate;
    DtAngleUnits angle;
    DtReal degrees;

    rotate = (struct rotate_data *)object->data;

    if (rotate == NULL){
	dor_print_output("bad values");
	return;
    }

    switch (rotate->axis){
        case DcXAxis:
	    dor_print_output("axis = DcXAxis");
	    break;

        case DcYAxis:
	    dor_print_output("axis = DcYAxis");
	    break;

        case DcZAxis:
	    dor_print_output("axis = DcZAxis");
	    break;

        default:
	    dor_print_output("axis = UNKNOWN");
	    break;
    }

    angle = dor_system_inq_angle_units(); 
    switch (angle) {
        case DcAngleOldStyle:
        case DcAngleRadians:
    	    sprintf(dor_print_get_line(),"radians = %lf",rotate->radians);
    	    dor_print_output(dor_print_get_line());
	    break;
        case DcAngleDegrees:
	    degrees = rotate->radians * 180. / M_PI; 
    	    sprintf(dor_print_get_line(),"degrees = %lf",degrees);
    	    dor_print_output(dor_print_get_line());
	    break;
        default:
            break;
    }
}
/*
 ======================================================================
 */

void dor_rotate_execute (struct object *object)
{
    struct rotate_data *rotdat;
    DtMatrix4x4 newmatrix;

    if (!dor_condex_query_execute(doe_rotate_class_id))
	    return;

    rotdat = (struct rotate_data *)object->data;

    dor_global_lcstowcsmat_get_value(newmatrix);

    switch(rotdat->axis){
    case DcXAxis:
	dor_matrix_rotate_x(newmatrix,rotdat->radians,DcPreConcatenate);
	break;

    case DcYAxis:
	dor_matrix_rotate_y(newmatrix,rotdat->radians,DcPreConcatenate);
	break;

    case DcZAxis:
	dor_matrix_rotate_z(newmatrix,rotdat->radians,DcPreConcatenate);
	break;
    }

    dor_global_lcstowcsmat_set_value(newmatrix);
}
