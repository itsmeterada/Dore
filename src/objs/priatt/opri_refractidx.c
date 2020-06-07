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
	int dor_refractidx_initialize ()
	dot_object * dor_refractidx_create (index)
	int dor_refractidx_destroy (object)
	int dor_refractidx_print (object)
	int dor_refractidx_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct refractidx_data {
   DtReal index;
};

DtInt doe_refractidx_class_id = -1;
DtFlag doe_refractidx_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry refractidx_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_refractidx_destroy },
    { DcMethodPrint,            (DtMethodPtr) dor_refractidx_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_refractidx_execute },
};

void dor_refractidx_initialize (void)
{
    doe_refractidx_class_id = dor_class_add ("DoRefractionIndex", 3,
					     refractidx_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_refractidx_create (DtReal index)
{
    struct refractidx_data *refractidx;

    if (!doe_refractidx_initialized) {
	dor_refractidx_initialize();
	doe_refractidx_initialized = DcTrue;
    }

    refractidx= (struct refractidx_data *)dor_space_allocate(sizeof *refractidx);

    refractidx->index = index;

    return (dor_object_create(doe_refractidx_class_id,refractidx));
}
/*
 ======================================================================
 */

void dor_refractidx_destroy (dot_object *object)
{
    struct refractidx_data *refractidx;

    refractidx = (struct refractidx_data *)(object->data);

    dor_space_deallocate(refractidx);
}
/*
 ======================================================================
 */

void dor_refractidx_print (dot_object *object)
{
    struct refractidx_data *refractidx;

    refractidx = (struct refractidx_data *)object->data;

    if (refractidx == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),
	    "index of refraction = %lf", refractidx->index);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_refractidx_execute (dot_object *object)
{
    struct refractidx_data *refractidx;

    if (!dor_condex_query_execute(doe_refractidx_class_id))
	    return;

    refractidx= (struct refractidx_data *)object->data;

    dor_global_refractidx_set_value(refractidx->index);
}
