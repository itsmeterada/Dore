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
	int dor_textrnuv_initialize ()
	dot_object * dor_textrnuv_create (tu, tv)
	int dor_textrnuv_destroy (object)
	int dor_textrnuv_print (object)
	int dor_textrnuv_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>

struct textrnuv_data {
   DtReal tu;
   DtReal tv;
};

DtInt doe_textrnuv_class_id = -1;
DtFlag doe_textrnuv_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry textrnuv_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_textrnuv_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_textrnuv_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_textrnuv_print   },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_textrnuv_execute },
};

void dor_textrnuv_initialize (void)
{
    doe_textrnuv_class_id = dor_class_add ("DoTextureTranslateUV", 4,
					   textrnuv_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_textrnuv_create (
    DtReal tu,
    DtReal tv)
{
    struct textrnuv_data *textrnuvdata;

    if (!doe_textrnuv_initialized) {
	dor_textrnuv_initialize();
	doe_textrnuv_initialized = DcTrue;
    }

    textrnuvdata = (struct textrnuv_data *)dor_space_allocate(sizeof *textrnuvdata);

    textrnuvdata->tu = tu;
    textrnuvdata->tv = tv;

    return (dor_object_create(doe_textrnuv_class_id,textrnuvdata));
}
/*
 ======================================================================
 */

void dor_textrnuv_destroy  (dot_object *object)
{
    struct textrnuv_data *textrnuv;

    textrnuv = (struct textrnuv_data *)(object->data);

    dor_space_deallocate(textrnuv);
}
/*
 ======================================================================
 */

void dor_textrnuv_print  (dot_object *object)
{
    struct textrnuv_data *textrnuv;

    textrnuv = (struct textrnuv_data *)object->data;

    if (textrnuv == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"translate = %lf %lf", textrnuv->tu,
	    textrnuv->tv);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_textrnuv_execute (dot_object *object)
{
    struct textrnuv_data *textrnuvdat;
    DtMatrix3x3 newmatrix;

    if (!dor_condex_query_execute(doe_textrnuv_class_id))
	    return;

    textrnuvdat = (struct textrnuv_data *)object->data;

    /* Overwrite the global UV texture matrix translate factors. */
    dor_global_texmatuv_get_value(newmatrix);
    newmatrix[0][2] = textrnuvdat->tu;
    newmatrix[1][2] = textrnuvdat->tv;
    dor_global_texmatuv_set_value(newmatrix);
}
