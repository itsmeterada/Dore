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
	int dor_spcclr_initialize ()
	dot_object * dor_spcclr_create (colormodel,color)
	int dor_spcclr_destroy (object)
	int dor_spcclr_print (object)
	int dor_spcclr_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct spcclr_data {
    DtColorRGB color;
};
/*
 ======================================================================
 */

DtInt doe_spcclr_class_id;

static DtMethodEntry spcclr_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_spcclr_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_spcclr_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_spcclr_destroy },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_spcclr_execute },
};

void dor_spcclr_initialize (void)
{
    doe_spcclr_class_id = dor_class_install (DcTypeSpecularColor, "DoSpecularColor", 
		4, spcclr_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_spcclr_create (
    DtColorModel colormodel,
    DtReal color[])
{
    static DtFlag initialized = DcFalse;
    dot_object *spcclrobject;
    struct spcclr_data *spcclrdata;

    if (!initialized) {
	dor_spcclr_initialize();
	initialized = DcTrue;
    }

    spcclrdata = (struct spcclr_data *)dor_space_allocate(sizeof *spcclrdata);

    spcclrdata->color[0] = color[0];
    spcclrdata->color[1] = color[1];
    spcclrdata->color[2] = color[2];

    spcclrobject = dor_object_create(doe_spcclr_class_id,spcclrdata);

    return(spcclrobject);
}
/*
 ======================================================================
 */

void dor_spcclr_destroy (
    dot_object *object)
{
    struct spcclr *spcclr;

    spcclr = (struct spcclr *)(object->data);

    dor_space_deallocate(spcclr);
}
/*
 ======================================================================
 */

void dor_spcclr_print (
    dot_object *object)
{
    struct spcclr_data *spcclr;

    spcclr = (struct spcclr_data *)object->data;

    if (spcclr == NULL){
	dor_print_output("bad values");
	return;
    }

    dor_print_output("color Model = DcRGB");

    sprintf(dor_print_get_line(),"color = (%f,%f,%f)", 
	    spcclr->color[0],spcclr->color[1],spcclr->color[2]);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_spcclr_execute (
    dot_object *object)
{
    struct spcclr_data *spcclrdat;

    if (!dor_condex_query_execute(doe_spcclr_class_id))
	    return;

    spcclrdat = (struct spcclr_data *)object->data;

    dor_global_spcclr_set_value(spcclrdat->color);
}
