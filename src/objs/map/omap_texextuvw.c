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
	int dor_texextuvw_initialize ()
	dot_object * dor_texextuvw_create (umode, vmode, wmode)
	int dor_texextuvw_destroy (object)
	int dor_texextuvw_print (object)
	int dor_texextuvw_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>

struct texextuvw_data {
   DtExtendMode umode;
   DtExtendMode vmode;
   DtExtendMode wmode;
};

DtInt doe_texextuvw_class_id = -1;
DtFlag doe_texextuvw_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry texextuvw_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_texextuvw_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_texextuvw_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_texextuvw_print   },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_texextuvw_execute },
};

void dor_texextuvw_initialize (void)
{
    doe_texextuvw_class_id = dor_class_add ("DoTextureExtendUVW", 4,
					    texextuvw_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_texextuvw_create (
    DtExtendMode umode,
    DtExtendMode vmode,
    DtExtendMode wmode)
{
    struct texextuvw_data *texextuvwdata;

    if (!doe_texextuvw_initialized) {
	dor_texextuvw_initialize();
	doe_texextuvw_initialized = DcTrue;
    }

    texextuvwdata = (struct texextuvw_data *)dor_space_allocate(sizeof *texextuvwdata);

    texextuvwdata->umode = umode;
    texextuvwdata->vmode = vmode;
    texextuvwdata->wmode = wmode;

    return (dor_object_create(doe_texextuvw_class_id,texextuvwdata));
}
/*
 ======================================================================
 */

void dor_texextuvw_destroy (
    dot_object *object)
{
    struct texextuvw_data *texextuvw;

    texextuvw = (struct texextuvw_data *)(object->data);

    dor_space_deallocate(texextuvw);
}
/*
 ======================================================================
 */

void dor_texextuvw_print (
    dot_object *object)
{
    struct texextuvw_data *texextuvw;

    texextuvw = (struct texextuvw_data *)object->data;

    if (texextuvw == NULL) {
	dor_print_output("bad values");
	return;
    }

    switch(texextuvw->umode) {
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
	sprintf(dor_print_get_line(),"umode = <UNKNOWN TYPE> %d",texextuvw->umode);
	dor_print_output(dor_print_get_line());
	break;
    }
    switch(texextuvw->vmode) {
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
	sprintf(dor_print_get_line(),"vmode = <UNKNOWN TYPE> %d",texextuvw->vmode);
	dor_print_output(dor_print_get_line());
	break;
    }
    switch(texextuvw->wmode) {
    case DcExtendNone:
	dor_print_output ("wmode = DcExtendNone");
	break;
    case DcExtendBlack:
	dor_print_output ("wmode = DcExtendBlack");
	break;
    case DcExtendClamp:
	dor_print_output ("wmode = DcExtendClamp");
	break;
    case DcExtendRepeat:
	dor_print_output ("wmode = DcExtendRepeat");
	break;
    default:
	sprintf(dor_print_get_line(),"wmode = <UNKNOWN TYPE> %d",texextuvw->wmode);
	dor_print_output(dor_print_get_line());
	break;
    }
}
/*
 ======================================================================
 */

void dor_texextuvw_execute (
    dot_object *object)
{
    struct texextuvw_data *texextuvwdat;

    if (!dor_condex_query_execute(doe_texextuvw_class_id))
	    return;

    texextuvwdat = (struct texextuvw_data *)object->data;

    dor_global_texextuvw_set_value(texextuvwdat->umode, texextuvwdat->vmode,
				   texextuvwdat->wmode);
}
