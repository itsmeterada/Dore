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
	int dor_transporientexp_initialize ()
	dot_object * dor_transporientexp_create (exponent)
	int dor_transporientexp_destroy (object)
	int dor_transporientexp_print (object)
	int dor_transporientexp_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct transporientexp_data {
    DtReal exponent;
};

DtInt doe_transporientexp_class_id = -1;
DtFlag doe_transporientexp_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry transporientexp_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_transporientexp_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_transporientexp_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_transporientexp_destroy },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_transporientexp_execute },
};

void dor_transporientexp_initialize (void)
{
    doe_transporientexp_class_id = dor_class_add ("DoTranspOrientExp", 4,
					transporientexp_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_transporientexp_create (DtReal exponent)
{
    struct transporientexp_data *transporientexp;
    dot_object *transporientexpobject;

    if (!doe_transporientexp_initialized) {
	dor_transporientexp_initialize();
	doe_transporientexp_initialized = DcTrue;
    }

    transporientexp = (struct transporientexp_data *)
	    dor_space_allocate(sizeof *transporientexp);

    transporientexp->exponent = exponent;

    transporientexpobject = 
	    dor_object_create(doe_transporientexp_class_id,transporientexp);

    return(transporientexpobject);
}
/*
 ======================================================================
 */

void dor_transporientexp_destroy (dot_object *object)
{
    struct transporientexp_data *transporientexp;

    transporientexp = (struct transporientexp_data *)(object->data);

    dor_space_deallocate(transporientexp);
}
/*
 ======================================================================
 */

void dor_transporientexp_print (dot_object *object)
{
    struct transporientexp_data *transporientexp;

    transporientexp = (struct transporientexp_data *)object->data;

    if (transporientexp == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),
	    "exponent = %lf",transporientexp->exponent);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_transporientexp_execute (dot_object *object)
{
    struct transporientexp_data *transporientexpdat;

    if (!dor_condex_query_execute(doe_transporientexp_class_id))
	    return;

    transporientexpdat = (struct transporientexp_data *)object->data;

    dor_global_transporientexp_set_value(transporientexpdat->exponent);
}
