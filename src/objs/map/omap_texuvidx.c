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
	int dor_texuvidx_initialize ()
	dot_object * dor_texuvidx_create (index)
	int dor_texuvidx_destroy (object)
	int dor_texuvidx_print (object)
	int dor_texuvidx_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>

struct texuvidx_data {
   DtInt index;
};

DtInt doe_texuvidx_class_id = -1;
DtFlag doe_texuvidx_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry texuvidx_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_texuvidx_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_texuvidx_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_texuvidx_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_texuvidx_execute },
};

void dor_texuvidx_initialize (void)
{
    doe_texuvidx_class_id = dor_class_add ("DoTextureUVIndex", 4,
					   texuvidx_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_texuvidx_create (
    DtInt index)
{
    struct texuvidx_data *texuvidxdata;

    if (!doe_texuvidx_initialized) {
	dor_texuvidx_initialize();
	doe_texuvidx_initialized = DcTrue;
    }

    texuvidxdata = (struct texuvidx_data *)dor_space_allocate(sizeof *texuvidxdata);

    if (index < 0) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		"DoTextureUVIndex",
		"index < 0 - returning Null object");
	return (dot_object *)dor_null_create(DcNullPtr);
    }

    texuvidxdata->index = index;

    return (dor_object_create(doe_texuvidx_class_id,texuvidxdata));
}
/*
 ======================================================================
 */

void dor_texuvidx_destroy (
    dot_object *object)
{
    struct texuvidx_data *texuvidx;

    texuvidx = (struct texuvidx_data *)(object->data);

    dor_space_deallocate(texuvidx);
}
/*
 ======================================================================
 */

void dor_texuvidx_print (
    dot_object *object)
{
    struct texuvidx_data *texuvidx;

    texuvidx = (struct texuvidx_data *)object->data;

    if (texuvidx == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"index = %d",texuvidx->index);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_texuvidx_execute (
    dot_object *object)
{
    struct texuvidx_data *texuvidxdat;

    if (!dor_condex_query_execute(doe_texuvidx_class_id))
	    return;

    texuvidxdat = (struct texuvidx_data *)object->data;

    dor_global_texuvidx_set_value(texuvidxdat->index);
}
