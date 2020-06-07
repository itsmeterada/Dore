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
	int dor_lgtint_initialize ()
	dot_object * dor_lgtint_create (intensity)
	int dor_lgtint_destroy (object)
	int dor_lgtint_print (object)
	int dor_lgtint_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>

struct lgtint_data {
    DtReal intensity;
};
/*
 ======================================================================
 */

DtInt doe_lgtint_class_id;

static DtMethodEntry lgtint_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_lgtint_print },
    { DcMethodDestroy,         (DtMethodPtr) dor_lgtint_destroy },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_lgtint_execute },
};

void dor_lgtint_initialize (void)
{
    doe_lgtint_class_id = dor_class_install (DcTypeLightIntens, "DoLightIntens", 3,
					     lgtint_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_lgtint_create (DtReal intensity)
{
    static DtFlag initialized = DcFalse;
    struct lgtint_data *lgtintdata;
    dot_object *lgtintobject;

    if (!initialized) {
	dor_lgtint_initialize();
	initialized = DcTrue;
    }

    lgtintdata = (struct lgtint_data *)dor_space_allocate(sizeof *lgtintdata);
	
    if (intensity < 0.) {
	DDerror (ERR_NEG_LIGHT_INT,
		    "DoLightIntens", DcNullPtr);
	lgtintdata->intensity = 0.;
    } else {
	lgtintdata->intensity = intensity;
    }

    lgtintobject = dor_object_create(doe_lgtint_class_id,lgtintdata);

    return(lgtintobject);
}
/*
 ======================================================================
 */

void dor_lgtint_destroy (dot_object *object)
{
    struct lgtint_data *lgtint;

    lgtint = (struct lgtint_data *)(object->data);

    dor_space_deallocate(lgtint);
}
/*
 ======================================================================
 */

void dor_lgtint_print (dot_object *object)
{
    struct lgtint_data *lgtint;

    lgtint = (struct lgtint_data *)object->data;

    if (lgtint == NULL){
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"intensity = %lf",lgtint->intensity);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_lgtint_execute (dot_object *object)
{
    struct lgtint_data *lgtintdat;

    if (!dor_condex_query_execute(doe_lgtint_class_id))
	    return;

    lgtintdat = (struct lgtint_data *)object->data;

    dor_global_lgtint_set_value(lgtintdat->intensity);
}
