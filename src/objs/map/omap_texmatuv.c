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
	int dor_texmatuv_initialize ()
	dot_object * dor_texmatuv_create (texmatuv)
	int dor_texmatuv_destroy (object)
	int dor_texmatuv_print (object)
	int dor_texmatuv_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/matrix.h>

struct texmatuv_data {
    DtMatrix3x3 texmatuv;
};

/*
 ======================================================================
 */

DtInt doe_texmatuv_class_id = -1;
DtFlag doe_texmatuv_initialized = DcFalse;

static DtMethodEntry texmatuv_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_texmatuv_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_texmatuv_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_texmatuv_print   },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_texmatuv_execute },
};

void dor_texmatuv_initialize (void)
{
    doe_texmatuv_class_id = dor_class_add ("DoTextureMatrixUV", 4, 
		texmatuv_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_texmatuv_create (
    DtMatrix3x3 texmatuv)
{
    struct texmatuv_data *texmatuvdat;

    if (!doe_texmatuv_initialized) {
	dor_texmatuv_initialize();
	doe_texmatuv_initialized = DcTrue;
    }

    texmatuvdat = (struct texmatuv_data *)dor_space_allocate(sizeof *texmatuvdat);

    dor_matrix_load3x3 (texmatuvdat->texmatuv, texmatuv);

    return(dor_object_create(doe_texmatuv_class_id,texmatuvdat)); 

}
/*
 ======================================================================
 */

void dor_texmatuv_destroy (
    dot_object *object)
{
    struct texmatuv_data *texmatuv;

    texmatuv = (struct texmatuv_data *)(object->data);

    dor_space_deallocate(texmatuv);
}
/*
 ======================================================================
 */

void dor_texmatuv_print (
    dot_object *object)
{
    struct texmatuv_data *texmatuvdata;

    texmatuvdata = (struct texmatuv_data *)object->data;

    if (texmatuvdata == NULL) {
	dor_print_output("bad values");
	return;
    }

    dor_matrix_print3x3 (texmatuvdata->texmatuv);
}
/*
 ======================================================================
 */

void dor_texmatuv_execute (
    struct object *object)
{
    struct texmatuv_data *texmatuvdat;

    if (!dor_condex_query_execute(doe_texmatuv_class_id))
	    return;

    texmatuvdat = (struct texmatuv_data *)object->data;
    dor_global_texmatuv_set_value(texmatuvdat->texmatuv);
}
