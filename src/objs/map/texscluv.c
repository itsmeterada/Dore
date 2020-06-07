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
	int dor_texscluv_initialize ()
	dot_object * dor_texscluv_create (su, sv)
	int dor_texscluv_destroy (object)
	int dor_texscluv_print (object)
	int dor_texscluv_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>

struct texscluv_data {
   DtReal su;
   DtReal sv;
};

DtInt doe_texscluv_class_id = -1;
DtFlag doe_texscluv_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry texscluv_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_texscluv_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_texscluv_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_texscluv_print   },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_texscluv_execute },
};

void dor_texscluv_initialize (void)
{
    doe_texscluv_class_id = dor_class_add ("DoTextureScaleUV", 4,
					   texscluv_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_texscluv_create (
    DtReal su,
    DtReal sv)
{
    struct texscluv_data *texscluvdata;

    if (!doe_texscluv_initialized) {
	dor_texscluv_initialize();
	doe_texscluv_initialized = DcTrue;
    }

    texscluvdata = (struct texscluv_data *)dor_space_allocate(sizeof *texscluvdata);

    texscluvdata->su = su;
    texscluvdata->sv = sv;

    return (dor_object_create(doe_texscluv_class_id,texscluvdata));
}
/*
 ======================================================================
 */

void dor_texscluv_destroy (dot_object *object)
{
    struct texscluv_data *texscluv;

    texscluv = (struct texscluv_data *)(object->data);

    dor_space_deallocate(texscluv);
}
/*
 ======================================================================
 */

void dor_texscluv_print (dot_object *object)
{
    struct texscluv_data *texscluv;

    texscluv = (struct texscluv_data *)object->data;

    if (!texscluv) {
	dor_print_output("bad values");
	return;
    }

    sprintf (dor_print_get_line(),"scale = %lf %lf", texscluv->su,texscluv->sv);
    dor_print_output (dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_texscluv_execute (dot_object *object)
{
    struct texscluv_data *texscluvdat;
    DtMatrix3x3 newmatrix;

    if (!dor_condex_query_execute(doe_texscluv_class_id))
	    return;

    texscluvdat = (struct texscluv_data *)object->data;

    /* Overwrite the global UV texture matrix scale factors. */
    dor_global_texmatuv_get_value(newmatrix);
    newmatrix[0][0] = texscluvdat->su;
    newmatrix[1][1] = texscluvdat->sv;
    dor_global_texmatuv_set_value(newmatrix);
}
