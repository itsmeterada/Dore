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
	int dor_texuvwidx_initialize ()
	dot_object * dor_texuvwidx_create (index)
	int dor_texuvwidx_destroy (object)
	int dor_texuvwidx_print (object)
	int dor_texuvwidx_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>

struct texuvwidx_data {
   DtInt index;
};

DtInt doe_texuvwidx_class_id = -1;
DtFlag doe_texuvwidx_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry texuvwidx_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_texuvwidx_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_texuvwidx_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_texuvwidx_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_texuvwidx_execute },
};

void dor_texuvwidx_initialize (void)
{
    doe_texuvwidx_class_id = dor_class_add ("DoTextureUVWIndex", 4,
					    texuvwidx_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_texuvwidx_create (
    DtInt index)
{
    struct texuvwidx_data *texuvwidxdata;

    if (!doe_texuvwidx_initialized) {
	dor_texuvwidx_initialize();
	doe_texuvwidx_initialized = DcTrue;
    }

    texuvwidxdata = (struct texuvwidx_data *)dor_space_allocate(sizeof 
				*texuvwidxdata);

    if (index < 0) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		"DoTextureUVWIndex",
		"index < 0 - returning Null object");
	return (dot_object *)dor_null_create(DcNullPtr);
    }

    texuvwidxdata->index = index;

    return (dor_object_create(doe_texuvwidx_class_id,texuvwidxdata));
}
/*
 ======================================================================
 */

void dor_texuvwidx_destroy (
    dot_object *object)
{
    struct texuvwidx_data *texuvwidx;

    texuvwidx = (struct texuvwidx_data *)(object->data);

    dor_space_deallocate(texuvwidx);
}
/*
 ======================================================================
 */

void dor_texuvwidx_print (
    dot_object *object)
{
    struct texuvwidx_data *texuvwidx;

    texuvwidx = (struct texuvwidx_data *)object->data;

    if (texuvwidx == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"index = %d",texuvwidx->index);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_texuvwidx_execute (
    dot_object *object)
{
    struct texuvwidx_data *texuvwidxdat;

    if (!dor_condex_query_execute(doe_texuvwidx_class_id))
	    return;

    texuvwidxdat = (struct texuvwidx_data *)object->data;

    dor_global_texuvwidx_set_value(texuvwidxdat->index);
}
