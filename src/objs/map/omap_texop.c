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
	int dor_texop_initialize ()
	dot_object * dor_texop_create (operator)
	int dor_texop_destroy (object)
	int dor_texop_print (object)
	int dor_texop_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>

struct texop_data {
   DtTextureOperator operator;
};

DtInt doe_texop_class_id = -1;
DtFlag doe_texop_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry texop_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_texop_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_texop_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_texop_print   },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_texop_execute },
};

void dor_texop_initialize (void)
{
    doe_texop_class_id = dor_class_add ("DoTextureOp", 4,
					texop_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_texop_create (
    DtTextureOperator operator)
{
    struct texop_data *texopdata;

    if (!doe_texop_initialized) {
	dor_texop_initialize();
	doe_texop_initialized = DcTrue;
    }

    texopdata = (struct texop_data *)dor_space_allocate(sizeof *texopdata);

    texopdata->operator = operator;

    return (dor_object_create(doe_texop_class_id,texopdata));
}
/*
 ======================================================================
 */

void dor_texop_destroy (
    dot_object *object)
{
    struct texop_data *texop;

    texop = (struct texop_data *)(object->data);

    dor_space_deallocate(texop);
}
/*
 ======================================================================
 */

void dor_texop_print (
    dot_object *object)
{
    struct texop_data *texop;

    texop = (struct texop_data *)object->data;

    if (texop == NULL) {
	dor_print_output("bad values");
	return;
    }

    switch(texop->operator) {
    case DcTextureReplace:
	dor_print_output ("operator = DcTextureReplace");
	break;
    case DcTextureMultiply:
	dor_print_output ("operator = DcTextureMultiply");
	break;
    case DcTextureBlend:
	dor_print_output ("operator = DcTextureBlend");
	break;
    case DcTextureAdd:
	dor_print_output ("operator = DcTextureAdd");
	break;
    default:
	sprintf(dor_print_get_line(),"operator = <UNKNOWN TYPE> %d",texop->operator);
	dor_print_output(dor_print_get_line());
	break;
    }
}
/*
 ======================================================================
 */

void dor_texop_execute (
    dot_object *object)
{
    struct texop_data *texopdat;

    if (!dor_condex_query_execute(doe_texop_class_id))
	    return;

    texopdat = (struct texop_data *)object->data;

    dor_global_texop_set_value(texopdat->operator);
}
