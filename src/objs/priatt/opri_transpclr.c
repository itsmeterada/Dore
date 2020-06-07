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
	int dor_transpclr_initialize ()
	dot_object * dor_transpclr_create (colormodel,color)
	int dor_transpclr_destroy (object)
	int dor_transpclr_print (object)
	int dor_transpclr_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct transpclr_data {
    DtColorRGB color;
};
/*
 ======================================================================
 */

DtInt doe_transpclr_class_id;

static DtMethodEntry transpclr_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_transpclr_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_transpclr_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_transpclr_destroy },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_transpclr_execute },
};

void dor_transpclr_initialize (void)
{
    doe_transpclr_class_id = dor_class_install (DcTypeTranspColor, "DoTranspColor", 
		4, transpclr_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_transpclr_create (
    DtColorModel colormodel,
    DtReal color[])
{
    static DtFlag initialized = DcFalse;
    dot_object *transpclrobject;
    struct transpclr_data *transpclrdata;

    if (!initialized) {
	dor_transpclr_initialize();
	initialized = DcTrue;
    }

    transpclrdata = (struct transpclr_data *)
	    dor_space_allocate(sizeof *transpclrdata);

    transpclrdata->color[0] = color[0];
    transpclrdata->color[1] = color[1];
    transpclrdata->color[2] = color[2];

    transpclrobject = dor_object_create(doe_transpclr_class_id,transpclrdata);

    return(transpclrobject);
}
/*
 ======================================================================
 */

void dor_transpclr_destroy (
    dot_object *object)
{
    struct transpclr_data *transpclr;

    transpclr = (struct transpclr_data *)(object->data);

    dor_space_deallocate(transpclr);
}
/*
 ======================================================================
 */

void dor_transpclr_print (
    dot_object *object)
{
    struct transpclr_data *transpclr;

    transpclr = (struct transpclr_data *)object->data;

    if (transpclr == NULL){
	dor_print_output("bad values");
	return;
    }

    dor_print_output("color Model = DcRGB");

    sprintf(dor_print_get_line(),"color = (%f,%f,%f)", 
	    transpclr->color[0],transpclr->color[1],transpclr->color[2]);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_transpclr_execute (
    dot_object *object)
{
    struct transpclr_data *transpclrdata;

    if (!dor_condex_query_execute(doe_transpclr_class_id))
	    return;

    transpclrdata = (struct transpclr_data *)object->data;

    dor_global_transpclr_set_value(transpclrdata->color);
}
