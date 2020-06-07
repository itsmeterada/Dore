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
	int dor_srfedgclr_initialize ()
	dot_object * dor_srfedgclr_create (colormodel,color)
	int dor_srfedgclr_destroy (object)
	int dor_srfedgclr_print (object)
	int dor_srfedgclr_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>
 
struct srfedgclr_data {
    DtColorRGB color;
};
 
DtInt doe_srfedgclr_class_id = -1;
DtFlag doe_srfedgclr_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry srfedgclr_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_srfedgclr_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_srfedgclr_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_srfedgclr_destroy },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_srfedgclr_execute },
};

void dor_srfedgclr_initialize (void)
{
    doe_srfedgclr_class_id = dor_class_add ("DoSurfaceEdgeColor", 4, 
					    srfedgclr_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_srfedgclr_create (
    DtColorModel colormodel,
    DtReal color[])
{
    dot_object *srfedgclr_object;
    struct srfedgclr_data *srfedgclr;

    if (!doe_srfedgclr_initialized) {
	dor_srfedgclr_initialize();
	doe_srfedgclr_initialized = DcTrue;
    }

    srfedgclr = (struct srfedgclr_data *)
	    dor_space_allocate(sizeof *srfedgclr);

    srfedgclr->color[0] = color[0];
    srfedgclr->color[1] = color[1];
    srfedgclr->color[2] = color[2];

    srfedgclr_object = 
	    dor_object_create(doe_srfedgclr_class_id,srfedgclr);

    return(srfedgclr_object);
}
/*
 ======================================================================
 */

void dor_srfedgclr_destroy (
    dot_object *object)
{
    struct srfedgclr_data *srfedgclr;

    srfedgclr = (struct srfedgclr_data *)(object->data);

    dor_space_deallocate(srfedgclr);
}
/*
 ======================================================================
 */

void dor_srfedgclr_print (
    dot_object *object)
{
    struct srfedgclr_data *srfedgclr;

    srfedgclr = (struct srfedgclr_data *)object->data;

    if (srfedgclr == NULL) {
	dor_print_output("bad values");
	return;
    }

    dor_print_output("color Model = DcRGB");

    sprintf(dor_print_get_line(),
	    "color = (%f,%f,%f)", 
	    srfedgclr->color[0],
	    srfedgclr->color[1],
	    srfedgclr->color[2]);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_srfedgclr_execute (
    dot_object *object)
{
    struct srfedgclr_data *srfedgclr;

    if (!dor_condex_query_execute(doe_srfedgclr_class_id))
	    return;

    srfedgclr = (struct srfedgclr_data *)object->data;

    dor_global_srfedgclr_set_value(srfedgclr->color);
}
