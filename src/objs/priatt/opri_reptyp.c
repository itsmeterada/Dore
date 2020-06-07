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
	int dor_reptyp_initialize ()
	dot_object * dor_reptyp_create (reptyp)
	int dor_reptyp_print (object)
	int dor_reptyp_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_reptyp_class_id;

static DtMethodEntry reptyp_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_reptyp_print },
    { DcMethodPick,             (DtMethodPtr) dor_reptyp_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_reptyp_execute },
};

void dor_reptyp_initialize (void)
{
    doe_reptyp_class_id = dor_class_install (DcTypeRepType, "DoRepType", 3,
					     reptyp_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_reptyp_create (DtRepType reptyp)
{
    static DtFlag initialized = DcFalse;
    dot_object *reptypobject;

    if (!initialized) {
	dor_reptyp_initialize();
	initialized = DcTrue;
    }

    reptypobject = dor_object_create(doe_reptyp_class_id,(DtPtr)reptyp);

    return(reptypobject);
}
/*
 ======================================================================
 */

void dor_reptyp_print (dot_object *object)
{
    switch ((DtRepType)(object->data)){
    case DcPoints:
	dor_print_output("type = DcPoints");
	break;

    case DcWireframe:
	dor_print_output("type = DcWireframe");
	break;

    case DcSurface:
	dor_print_output("type = DcSurface");
	break;

    case DcOutlines:
	dor_print_output("type = DcOutlines");
	break;

    case DcSurfaceWireframeEdges:
	dor_print_output("type = DcSurfaceWireframeEdges");
	break;

    case DcSurfaceOutlineEdges:
	dor_print_output("type = DcSurfaceOutlineEdges");
	break;

    default:
	dor_print_output("type = UNKNOWN");
	break;
    }
}
/*
 ======================================================================
 */

void dor_reptyp_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_reptyp_class_id))
	    return;

    dor_global_reptyp_set_value((DtRepType)(object->data));
}
