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
	int dor_lgtspdang_initialize ()
	dot_object * dor_lgtspdang_create (total, delta)
	int dor_lgtspdang_destroy (object)
	int dor_lgtspdang_print (object)
	int dor_lgtspdang_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>
#include <math.h>

struct lgtspdang_data {
    DtReal total_angle;
    DtReal delta_angle;
};
/*
 ======================================================================
 */

DtInt doe_lgtspdang_class_id;

static DtMethodEntry lgtspdang_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_lgtspdang_print },
    { DcMethodDestroy,         (DtMethodPtr) dor_lgtspdang_destroy },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_lgtspdang_execute },
};

void dor_lgtspdang_initialize (void)
{
    doe_lgtspdang_class_id = dor_class_add ("DoLightSpreadAngles", 
					    3, lgtspdang_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_lgtspdang_create (
    DtReal total,
    DtReal delta)
{
    static DtFlag initialized = DcFalse;
    struct lgtspdang_data *lgtspdangdata;
    dot_object *lgtspdang_object;

    if (!initialized) {
	dor_lgtspdang_initialize();
	initialized = DcTrue;
    }

    lgtspdangdata = (struct lgtspdang_data *)dor_space_allocate(sizeof *lgtspdangdata);
	
    if (total < 0.) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "DoLightSpreadAngles", DcNullPtr);
	total = 0.;
    }

    if ((delta > total) || (delta < 0.)){
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "DoLightSpreadAngles", DcNullPtr);
	delta = 0.;
    }

    if (dor_system_inq_angle_units() == DcAngleDegrees) {
	lgtspdangdata->total_angle = total * M_PI / 180.0;
	lgtspdangdata->delta_angle = delta * M_PI / 180.0;
    } else {
	lgtspdangdata->total_angle = total;
	lgtspdangdata->delta_angle = delta;
    }

    lgtspdang_object = dor_object_create(doe_lgtspdang_class_id,lgtspdangdata);

    return(lgtspdang_object);
}
/*
 ======================================================================
 */

void dor_lgtspdang_destroy (dot_object *object)
{
    struct lgtspdang_data *lgtspdang;

    lgtspdang = (struct lgtspdang_data *)(object->data);

    dor_space_deallocate(lgtspdang);
}
/*
 ======================================================================
 */

void dor_lgtspdang_print (dot_object *object)
{
    struct lgtspdang_data *lgtspdang;
    DtAngleUnits angle;
    DtReal degrees;

    lgtspdang = (struct lgtspdang_data *)object->data;

    if (lgtspdang == NULL){
	dor_print_output("bad values");
	return;
    }

    angle = dor_system_inq_angle_units();
    switch (angle) {
    	case DcAngleOldStyle:
    	case DcAngleRadians:
    	    sprintf(dor_print_get_line(),
		    	"total angle = %lf radians", 
			lgtspdang->total_angle);
    	    dor_print_output(dor_print_get_line());
    	    sprintf(dor_print_get_line(),
			"delta angle = %lf radians",
			lgtspdang->delta_angle);
    	    dor_print_output(dor_print_get_line());
	    break;
    	case DcAngleDegrees:
	    degrees = lgtspdang->total_angle * 180. / M_PI;
    	    sprintf(dor_print_get_line(),
		    	"total angle = %lf degrees", degrees);
    	    dor_print_output(dor_print_get_line());
	    degrees = lgtspdang->delta_angle * 180. / M_PI;
    	    sprintf(dor_print_get_line(),
			"delta angle = %lf degrees", degrees);
    	    dor_print_output(dor_print_get_line());
	    break;
   	default:
	    break;
    }
		
}
/*
 ======================================================================
 */

void dor_lgtspdang_execute (dot_object *object)
{
    struct lgtspdang_data *lgtspdangdat;

    if (!dor_condex_query_execute(doe_lgtspdang_class_id))
	    return;

    lgtspdangdat = (struct lgtspdang_data *)object->data;

    dor_global_lgtspdang_set_value(lgtspdangdat->total_angle, 
				   lgtspdangdat->delta_angle);
}
