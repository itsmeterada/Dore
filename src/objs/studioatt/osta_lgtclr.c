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
	int dor_lgtclr_initialize ()
	dot_object * dor_lgtclr_create (colormodel,color)
	int dor_lgtclr_destroy (object)
	int dor_lgtclr_print (object)
	int dor_lgtclr_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>

struct lgtclr_data {
    DtColorRGB color;
};
/*
 ======================================================================
 */

DtInt doe_lgtclr_class_id;

static DtMethodEntry lgtclr_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_lgtclr_print },
    { DcMethodDestroy,         (DtMethodPtr) dor_lgtclr_destroy },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_lgtclr_execute },
};

void dor_lgtclr_initialize (void)
{
    doe_lgtclr_class_id = dor_class_install
			  (   DcTypeLightColor, "DoLightColor", 3,
			      lgtclr_methods, DcNullPtr
			  );
}
/*
 ======================================================================
 */

dot_object *dor_lgtclr_create (
    DtColorModel colormodel,
    DtReal color[])
{
    static DtFlag initialized = DcFalse;
    dot_object *lgtclrobject;
    struct lgtclr_data *lgtclrdata;

    if (!initialized) {
	dor_lgtclr_initialize();
	initialized = DcTrue;
    }

    lgtclrdata = (struct lgtclr_data *)dor_space_allocate(sizeof *lgtclrdata);

    lgtclrdata->color[0] = color[0];
    lgtclrdata->color[1] = color[1];
    lgtclrdata->color[2] = color[2];

    lgtclrobject = dor_object_create(doe_lgtclr_class_id,lgtclrdata);

    return(lgtclrobject);
}
/*
 ======================================================================
 */

void dor_lgtclr_destroy (dot_object *object)
{
    struct lgtclr_data *lgtclr;

    lgtclr = (struct lgtclr_data *)(object->data);

    dor_space_deallocate(lgtclr);
}
/*
 ======================================================================
 */

void dor_lgtclr_print (dot_object *object)
{
    struct lgtclr_data *lgtclrdata;

    lgtclrdata = (struct lgtclr_data *)object->data;

    if (lgtclrdata == NULL) {
	dor_print_output("bad values");
	return;
    }

    dor_print_output("color Model = DcRGB");

    sprintf(dor_print_get_line(),"color = (%f,%f,%f)", 
	    lgtclrdata->color[0],lgtclrdata->color[1],lgtclrdata->color[2]);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_lgtclr_execute (dot_object *object)
{
    struct lgtclr_data *lgtclrdat;

    if (!dor_condex_query_execute(doe_lgtclr_class_id))
	    return;

    lgtclrdat = (struct lgtclr_data *)object->data;

    dor_global_lgtclr_set_value(lgtclrdat->color);
}
