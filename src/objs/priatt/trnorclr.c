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
	int dor_transporientclr_initialize ()
	dot_object * dor_transporientclr_create (colormodel,color)
	int dor_transporientclr_destroy (object)
	int dor_transporientclr_print (object)
	int dor_transporientclr_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct transporientclr_data {
    DtColorRGB color;
};

DtInt doe_transporientclr_class_id = -1;
DtFlag doe_transporientclr_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry transporientclr_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_transporientclr_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_transporientclr_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_transporientclr_destroy },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_transporientclr_execute },
};

void dor_transporientclr_initialize (void)
{
    doe_transporientclr_class_id = dor_class_add ("DoTranspOrientColor", 4, 
			transporientclr_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_transporientclr_create (
    DtColorModel colormodel,
    DtReal color[])
{
    dot_object *transporientclr_object;
    struct transporientclr_data *transporientclr;

    if (!doe_transporientclr_initialized) {
	dor_transporientclr_initialize();
	doe_transporientclr_initialized = DcTrue;
    }

    transporientclr = (struct transporientclr_data *)
	    dor_space_allocate(sizeof *transporientclr);

    transporientclr->color[0] = color[0];
    transporientclr->color[1] = color[1];
    transporientclr->color[2] = color[2];

    transporientclr_object = 
	    dor_object_create(doe_transporientclr_class_id,transporientclr);

    return(transporientclr_object);
}
/*
 ======================================================================
 */

void dor_transporientclr_destroy (
    dot_object *object)
{
    struct transporientclr_data *transporientclr;

    transporientclr = (struct transporientclr_data *)(object->data);

    dor_space_deallocate(transporientclr);
}
/*
 ======================================================================
 */

void dor_transporientclr_print (
    dot_object *object)
{
    struct transporientclr_data *transporientclr;

    transporientclr = (struct transporientclr_data *)object->data;

    if (transporientclr == NULL) {
	dor_print_output("bad values");
	return;
    }

    dor_print_output("color Model = DcRGB");

    sprintf(dor_print_get_line(),
	    "color = (%f,%f,%f)", 
	    transporientclr->color[0],
	    transporientclr->color[1],
	    transporientclr->color[2]);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_transporientclr_execute (
    dot_object *object)
{
    struct transporientclr_data *transporientclr;

    if (!dor_condex_query_execute(doe_transporientclr_class_id))
	    return;

    transporientclr = (struct transporientclr_data *)object->data;

    dor_global_transporientclr_set_value(transporientclr->color);
}
