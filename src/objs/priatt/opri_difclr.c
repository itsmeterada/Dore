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
	int dor_difclr_initialize ()
	dot_object * dor_difclr_create (colormodel,color)
	int dor_difclr_destroy (object)
	int dor_difclr_print (object)
	int dor_difclr_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct difclr_data {
    DtColorRGB color;
};
/*
 ======================================================================
 */

DtInt doe_difclr_class_id;

static DtMethodEntry difclr_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_difclr_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_difclr_print   },
    { DcMethodDestroy,          (DtMethodPtr) dor_difclr_destroy },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_difclr_execute },
};

void dor_difclr_initialize (void)
{
    doe_difclr_class_id = dor_class_install (DcTypeDiffuseColor, "DoDiffuseColor", 4,
					     difclr_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_difclr_create (
    DtColorModel colormodel,
    DtReal color[])
{
    static DtFlag initialized = DcFalse;
    dot_object *difclrobject;
    struct difclr_data *difclrdata;

    if (!initialized) {
	dor_difclr_initialize();
	initialized = DcTrue;
    }

    difclrdata = (struct difclr_data *)dor_space_allocate(sizeof *difclrdata);

    difclrdata->color[0] = color[0];
    difclrdata->color[1] = color[1];
    difclrdata->color[2] = color[2];

    difclrobject = dor_object_create(doe_difclr_class_id,difclrdata);

    return(difclrobject);
}
/*
 ======================================================================
 */

void dor_difclr_destroy (
    dot_object *object)
{
    struct difclr_data *difclr;

    difclr = (struct difclr_data *)(object->data);

    dor_space_deallocate(difclr);
}
/*
 ======================================================================
 */

void dor_difclr_print (
    dot_object *object)
{
    struct difclr_data *difclr;

    difclr = (struct difclr_data *)object->data;

    if (difclr == NULL){
	dor_print_output("bad values");
	return;
    }

    dor_print_output("color Model = DcRGB");

    sprintf(dor_print_get_line(),"color = (%f,%f,%f)", 
	    difclr->color[0],difclr->color[1],difclr->color[2]);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_difclr_execute (
    dot_object *object)
{
    struct difclr_data *difclrdat;

    if (!dor_condex_query_execute(doe_difclr_class_id))
	    return;

    difclrdat = (struct difclr_data *)object->data;

    dor_global_difclr_set_value(difclrdat->color);
}
