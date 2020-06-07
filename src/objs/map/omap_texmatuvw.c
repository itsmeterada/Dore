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
	int dor_texmatuvw_initialize ()
	dot_object * dor_texmatuvw_create (texmatuvw)
	int dor_texmatuvw_destroy (object)
	int dor_texmatuvw_print (object)
	int dor_texmatuvw_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/matrix.h>

struct texmatuvw_data {
    DtMatrix4x4 texmatuvw;
};

/*
 ======================================================================
 */

DtInt doe_texmatuvw_class_id = -1;
DtFlag doe_texmatuvw_initialized = DcFalse;

static DtMethodEntry texmatuvw_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_texmatuvw_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_texmatuvw_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_texmatuvw_print   },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_texmatuvw_execute },
};

void dor_texmatuvw_initialize (void)
{
    doe_texmatuvw_class_id = dor_class_add ("DoTextureMatrixUVW", 4, 
		texmatuvw_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_texmatuvw_create (
    DtMatrix4x4 texmatuvw)
{
    struct texmatuvw_data *texmatuvwdat;

    if (!doe_texmatuvw_initialized) {
	dor_texmatuvw_initialize();
	doe_texmatuvw_initialized = DcTrue;
    }

    texmatuvwdat = (struct texmatuvw_data *)dor_space_allocate(sizeof *texmatuvwdat);

    dor_matrix_load(texmatuvwdat->texmatuvw, texmatuvw);

    return(dor_object_create(doe_texmatuvw_class_id,texmatuvwdat)); 

}
/*
 ======================================================================
 */

void dor_texmatuvw_destroy (
    dot_object *object)
{
    struct texmatuvw_data *texmatuvw;

    texmatuvw = (struct texmatuvw_data *)(object->data);

    dor_space_deallocate(texmatuvw);
}
/*
 ======================================================================
 */

void dor_texmatuvw_print (
    dot_object *object)
{
    struct texmatuvw_data *texmatuvwdata;

    texmatuvwdata = (struct texmatuvw_data *)object->data;

    if (texmatuvwdata == NULL) {
	dor_print_output("bad values");
	return;
    }

    dor_matrix_print (texmatuvwdata->texmatuvw);
}
/*
 ======================================================================
 */

void dor_texmatuvw_execute (
    struct object *object)
{
    struct texmatuvw_data *texmatuvwdat;

    if (!dor_condex_query_execute(doe_texmatuvw_class_id))
	    return;

    texmatuvwdat = (struct texmatuvw_data *)object->data;
    dor_global_texmatuvw_set_value(texmatuvwdat->texmatuvw);
}
