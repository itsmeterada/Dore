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
	int dor_emisclr_initialize ()
	dot_object * dor_emisclr_create (colormodel,color)
	int dor_emisclr_destroy (object)
	int dor_emisclr_print (object)
	int dor_emisclr_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct emisclr_data {
    DtColorRGB color;
};

DtInt doe_emisclr_class_id = -1;
DtFlag doe_emisclr_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry emisclr_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_emisclr_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_emisclr_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_emisclr_destroy },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_emisclr_execute },
};

void dor_emisclr_initialize (void)
{
    doe_emisclr_class_id = dor_class_add ("DoEmissionColor", 4, 
			emisclr_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_emisclr_create (
    DtColorModel colormodel,
    DtReal       color[])
{
    dot_object *emisclr_object;
    struct emisclr_data *emisclr;

    if (!doe_emisclr_initialized) {
	dor_emisclr_initialize();
	doe_emisclr_initialized = DcTrue;
    }

    emisclr = (struct emisclr_data *)
	    dor_space_allocate(sizeof *emisclr);

    emisclr->color[0] = color[0];
    emisclr->color[1] = color[1];
    emisclr->color[2] = color[2];

    emisclr_object = 
	    dor_object_create(doe_emisclr_class_id,emisclr);

    return(emisclr_object);
}
/*
 ======================================================================
 */

void dor_emisclr_destroy (dot_object *object)
{
    struct emisclr_data *emisclr;

    emisclr = (struct emisclr_data *)(object->data);

    dor_space_deallocate(emisclr);
}
/*
 ======================================================================
 */

void dor_emisclr_print (dot_object *object)
{
    struct emisclr_data *emisclr;

    emisclr = (struct emisclr_data *)object->data;

    if (emisclr == NULL) {
	dor_print_output("bad values");
	return;
    }

    dor_print_output("color Model = DcRGB");

    sprintf(dor_print_get_line(),
	    "color = (%f,%f,%f)", 
	    emisclr->color[0],
	    emisclr->color[1],
	    emisclr->color[2]);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_emisclr_execute (dot_object *object)
{
    struct emisclr_data *emisclr;

    if (!dor_condex_query_execute(doe_emisclr_class_id))
	    return;

    emisclr = (struct emisclr_data *)object->data;

    dor_global_emisclr_set_value(emisclr->color);
}
