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
	int dor_texextuv_initialize ()
	dot_object * dor_texextuv_create (umode, vmode)
	int dor_texextuv_destroy (object)
	int dor_texextuv_print (object)
	int dor_texextuv_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>

struct texextuv_data {
   DtExtendMode umode;
   DtExtendMode vmode;
};

DtInt doe_texextuv_class_id = -1;
DtFlag doe_texextuv_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry texextuv_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_texextuv_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_texextuv_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_texextuv_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_texextuv_execute },
};

void dor_texextuv_initialize (void)
{
    doe_texextuv_class_id = dor_class_add ("DoTextureExtendUV", 4,
					   texextuv_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_texextuv_create (
    DtExtendMode umode,
    DtExtendMode vmode)
{
    struct texextuv_data *texextuvdata;

    if (!doe_texextuv_initialized) {
	dor_texextuv_initialize();
	doe_texextuv_initialized = DcTrue;
    }

    texextuvdata = (struct texextuv_data *)dor_space_allocate(sizeof *texextuvdata);

    texextuvdata->umode = umode;
    texextuvdata->vmode = vmode;

    return (dor_object_create(doe_texextuv_class_id,texextuvdata));
}
/*
 ======================================================================
 */

void dor_texextuv_destroy (
    dot_object *object)
{
    struct texextuv_data *texextuv;

    texextuv = (struct texextuv_data *)(object->data);

    dor_space_deallocate(texextuv);
}
/*
 ======================================================================
 */

void dor_texextuv_print (
    dot_object *object)
{
    struct texextuv_data *texextuv;

    texextuv = (struct texextuv_data *)object->data;

    if (texextuv == NULL) {
	dor_print_output("bad values");
	return;
    }

    switch(texextuv->umode) {
    case DcExtendNone:
	dor_print_output ("umode = DcExtendNone");
	break;
    case DcExtendBlack:
	dor_print_output ("umode = DcExtendBlack");
	break;
    case DcExtendClamp:
	dor_print_output ("umode = DcExtendClamp");
	break;
    case DcExtendRepeat:
	dor_print_output ("umode = DcExtendRepeat");
	break;
    default:
	sprintf(dor_print_get_line(),"umode = <UNKNOWN TYPE> %d",texextuv->umode);
	dor_print_output(dor_print_get_line());
	break;
    }
    switch(texextuv->vmode) {
    case DcExtendNone:
	dor_print_output ("vmode = DcExtendNone");
	break;
    case DcExtendBlack:
	dor_print_output ("vmode = DcExtendBlack");
	break;
    case DcExtendClamp:
	dor_print_output ("vmode = DcExtendClamp");
	break;
    case DcExtendRepeat:
	dor_print_output ("vmode = DcExtendRepeat");
	break;
    default:
	sprintf(dor_print_get_line(),"vmode = <UNKNOWN TYPE> %d",texextuv->vmode);
	dor_print_output(dor_print_get_line());
	break;
    }
}
/*
 ======================================================================
 */

void dor_texextuv_execute (
    dot_object *object)
{
    struct texextuv_data *texextuvdat;

    if (!dor_condex_query_execute(doe_texextuv_class_id))
	    return;

    texextuvdat = (struct texextuv_data *)object->data;

    dor_global_texextuv_set_value(texextuvdat->umode, texextuvdat->vmode);
}
