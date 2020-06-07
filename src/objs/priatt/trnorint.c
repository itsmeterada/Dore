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
	int dor_transporientint_initialize ()
	dot_object * dor_transporientint_create (intensity)
	int dor_transporientint_destroy (object)
	int dor_transporientint_print (object)
	int dor_transporientint_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct transporientint_data {
    DtReal intensity;
};

DtInt doe_transporientint_class_id = -1;
DtFlag doe_transporientint_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry transporientint_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_transporientint_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_transporientint_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_transporientint_destroy },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_transporientint_execute },
};

void dor_transporientint_initialize (void)
{
    doe_transporientint_class_id = dor_class_add ("DoTranspOrientIntens", 4,
						  transporientint_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_transporientint_create (DtReal intensity)
{
    struct transporientint_data *transporientint;
    dot_object *transporientintobject;

    if (!doe_transporientint_initialized) {
	dor_transporientint_initialize();
	doe_transporientint_initialized = DcTrue;
    }

    transporientint = (struct transporientint_data *)
	    dor_space_allocate(sizeof *transporientint);

    transporientint->intensity = intensity;

    transporientintobject = 
	    dor_object_create(doe_transporientint_class_id,transporientint);

    return(transporientintobject);
}
/*
 ======================================================================
 */

void dor_transporientint_destroy (dot_object *object)
{
    struct transporientint_data *transporientint;

    transporientint = (struct transporientint_data *)(object->data);

    dor_space_deallocate(transporientint);
}
/*
 ======================================================================
 */

void dor_transporientint_print (dot_object *object)
{
    struct transporientint_data *transporientint;

    transporientint = (struct transporientint_data *)object->data;

    if (transporientint == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),
	    "intensity = %lf",transporientint->intensity);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_transporientint_execute (dot_object *object)
{
    struct transporientint_data *transporientintdat;

    if (!dor_condex_query_execute(doe_transporientint_class_id))
	    return;

    transporientintdat = (struct transporientint_data *)object->data;

    dor_global_transporientint_set_value(transporientintdat->intensity);
}
