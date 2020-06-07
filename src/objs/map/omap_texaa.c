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
	int dor_texaa_initialize ()
	dot_object * dor_texaa_create (mode)
	int dor_texaa_destroy (object)
	int dor_texaa_print (object)
	int dor_texaa_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>

struct texaa_data {
   DtTextureAntiAliasMode mode;
};

DtInt doe_texaa_class_id = -1;
DtFlag doe_texaa_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry texaa_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_texaa_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_texaa_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_texaa_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_texaa_execute },
};

void dor_texaa_initialize (void)
{
    doe_texaa_class_id = dor_class_add ("DoTextureAntiAlias", 4,
					texaa_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_texaa_create (
    DtTextureAntiAliasMode mode)
{
    struct texaa_data *texaadata;

    if (!doe_texaa_initialized) {
	dor_texaa_initialize();
	doe_texaa_initialized = DcTrue;
    }

    texaadata = (struct texaa_data *)dor_space_allocate(sizeof *texaadata);

    texaadata->mode = mode;

    return (dor_object_create(doe_texaa_class_id,texaadata));
}
/*
 ======================================================================
 */

void dor_texaa_destroy (
    dot_object *object)
{
    struct texaa_data *texaa;

    texaa = (struct texaa_data *)(object->data);

    dor_space_deallocate(texaa);
}
/*
 ======================================================================
 */

void dor_texaa_print (
    dot_object *object)
{
    struct texaa_data *texaa;

    texaa = (struct texaa_data *)object->data;

    if (texaa == NULL) {
	dor_print_output("bad values");
	return;
    }

    switch(texaa->mode) {
    case DcTextureAANone:
	dor_print_output ("mode = DcTextureAANone");
	break;
    case DcTextureAAMIPPoint:
	dor_print_output ("mode = DcTextureAAMIPPoint");
	break;
    case DcTextureAAAdaptive:
	dor_print_output ("mode = DcTextureAAAdaptive");
	break;
    case DcTextureAABilinear:
	dor_print_output ("mode = DcTextureAABilinear");
	break;
    case DcTextureAASumArea:
	dor_print_output ("mode = DcTextureAASumArea");
	break;
    case DcTextureAATrilinear:
	dor_print_output ("mode = DcTextureAATrilinear");
	break;
    case DcTextureAAMIP2Point:
	dor_print_output ("mode = DcTextureAAMIP2Point");
	break;
    case DcTextureAAMIPBilinear:
	dor_print_output ("mode = DcTextureAAMIPBilinear");
	break;
    case DcTextureAAMIP2Bilinear:
	dor_print_output ("mode = DcTextureAAMIP2Bilinear");
	break;
    default:
	sprintf(dor_print_get_line(),"mode = <UNKNOWN TYPE> %d",texaa->mode);
	dor_print_output(dor_print_get_line());
	break;
    }
}
/*
 ======================================================================
 */

void dor_texaa_execute (
    dot_object *object)
{
    struct texaa_data *texaadat;

    if (!dor_condex_query_execute(doe_texaa_class_id))
	    return;

    texaadat = (struct texaa_data *)object->data;

    dor_global_texaa_set_value(texaadat->mode);
}
