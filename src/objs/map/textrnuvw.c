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
	int dor_textrnuvw_initialize ()
	dot_object * dor_textrnuvw_create (tu, tv, tw)
	int dor_textrnuvw_destroy (object)
	int dor_textrnuvw_print (object)
	int dor_textrnuvw_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>

struct textrnuvw_data {
   DtReal tu;
   DtReal tv;
   DtReal tw;
};

DtInt doe_textrnuvw_class_id = -1;
DtFlag doe_textrnuvw_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry textrnuvw_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_textrnuvw_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_textrnuvw_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_textrnuvw_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_textrnuvw_execute },
};

void dor_textrnuvw_initialize (void)
{
    doe_textrnuvw_class_id = dor_class_add ("DoTextureTranslateUVW", 4,
					    textrnuvw_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_textrnuvw_create (
    DtReal tu,
    DtReal tv,
    DtReal tw)
{
    struct textrnuvw_data *textrnuvwdata;

    if (!doe_textrnuvw_initialized) {
	dor_textrnuvw_initialize();
	doe_textrnuvw_initialized = DcTrue;
    }

    textrnuvwdata = (struct textrnuvw_data *)dor_space_allocate(sizeof *textrnuvwdata);

    textrnuvwdata->tu = tu;
    textrnuvwdata->tv = tv;
    textrnuvwdata->tw = tw;

    return (dor_object_create(doe_textrnuvw_class_id,textrnuvwdata));
}
/*
 ======================================================================
 */

void dor_textrnuvw_destroy (dot_object *object)
{
    struct textrnuvw_data *textrnuvw;

    textrnuvw = (struct textrnuvw_data *)(object->data);

    dor_space_deallocate(textrnuvw);
}
/*
 ======================================================================
 */

void dor_textrnuvw_print (dot_object *object)
{
    struct textrnuvw_data *textrnuvw;

    textrnuvw = (struct textrnuvw_data *)object->data;

    if (textrnuvw == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"translate = %lf %lf %lf", textrnuvw->tu,
	    textrnuvw->tv, textrnuvw->tw);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_textrnuvw_execute (dot_object *object)
{
    struct textrnuvw_data *textrnuvwdat;
    DtMatrix4x4 newmatrix;

    if (!dor_condex_query_execute(doe_textrnuvw_class_id))
	    return;

    textrnuvwdat = (struct textrnuvw_data *)object->data;

    /* Overwrite the global UVW texture matrix translate factors. */
    dor_global_texmatuvw_get_value(newmatrix);
    newmatrix[0][3] = textrnuvwdat->tu;
    newmatrix[1][3] = textrnuvwdat->tv;
    newmatrix[2][3] = textrnuvwdat->tw;
    dor_global_texmatuvw_set_value(newmatrix);
}
