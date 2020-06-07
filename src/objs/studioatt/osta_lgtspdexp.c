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
	int dor_lgtspdexp_initialize ()
	dot_object * dor_lgtspdexp_create (exponent)
	int dor_lgtspdexp_destroy (object)
	int dor_lgtspdexp_print (object)
	int dor_lgtspdexp_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>

struct lgtspdexp_data {
    DtReal exponent;
};
/*
 ======================================================================
 */

DtInt doe_lgtspdexp_class_id;

static DtMethodEntry lgtspdexp_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_lgtspdexp_print },
    { DcMethodDestroy,         (DtMethodPtr) dor_lgtspdexp_destroy },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_lgtspdexp_execute },
};

void dor_lgtspdexp_initialize (void)
{
    doe_lgtspdexp_class_id = dor_class_add ("DoLightSpreadExp", 3, 
					    lgtspdexp_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_lgtspdexp_create (DtReal exponent)

{
    static DtFlag initialized = DcFalse;
    struct lgtspdexp_data *lgtspdexpdata;
    dot_object *lgtspdexp_object;

    if (!initialized) {
	dor_lgtspdexp_initialize();
	initialized = DcTrue;
    }

    lgtspdexpdata = (struct lgtspdexp_data *)dor_space_allocate(sizeof *lgtspdexpdata);
	
    if (exponent < 0.) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "DoLightSpreadExp", DcNullPtr);
	lgtspdexpdata->exponent = 0.;
    } else {
	lgtspdexpdata->exponent = exponent;
    }

    lgtspdexp_object = dor_object_create(doe_lgtspdexp_class_id,lgtspdexpdata);

    return(lgtspdexp_object);
}
/*
 ======================================================================
 */

void dor_lgtspdexp_destroy (dot_object *object)
{
    struct lgtspdexp_data *lgtspdexp;

    lgtspdexp = (struct lgtspdexp_data *)(object->data);

    dor_space_deallocate(lgtspdexp);
}
/*
 ======================================================================
 */

void dor_lgtspdexp_print (dot_object *object)
{
    struct lgtspdexp_data *lgtspdexp;

    lgtspdexp = (struct lgtspdexp_data *)object->data;

    if (lgtspdexp == NULL){
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"exponent = %lf",lgtspdexp->exponent);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_lgtspdexp_execute (dot_object *object)
{
    struct lgtspdexp_data *lgtspdexpdat;

    if (!dor_condex_query_execute(doe_lgtspdexp_class_id))
	    return;

    lgtspdexpdat = (struct lgtspdexp_data *)object->data;

    dor_global_lgtspdexp_set_value(lgtspdexpdat->exponent);
}
