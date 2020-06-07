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
	int dor_inttyp_initialize ()
	dot_object * dor_inttyp_create (inttyp)
	int dor_inttyp_print (object)
	int dor_inttyp_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_inttyp_class_id;

static DtMethodEntry inttyp_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_inttyp_print },
    { DcMethodPick,             (DtMethodPtr) dor_inttyp_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_inttyp_execute },
};

void dor_inttyp_initialize (void)
{
    doe_inttyp_class_id = dor_class_install (DcTypeInterpType, "DoInterpType", 3,
					     inttyp_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_inttyp_create (DtInterpType inttyp)
{
    static DtFlag initialized = DcFalse;
    dot_object *inttypobject;

    if (!initialized) {
	dor_inttyp_initialize();
	initialized = DcTrue;
    }

    inttypobject = dor_object_create(doe_inttyp_class_id,(DtPtr)inttyp);

    return(inttypobject);
}
/*
 ======================================================================
 */

void dor_inttyp_print (dot_object *object)
{
    switch ((DtInterpType)(object->data)){
    case DcConstantShade:
	dor_print_output("type = DcConstantShade");
	break;

    case DcVertexShade:
	dor_print_output("type = DcVertexShade");
	break;

    case DcSurfaceShade:
	dor_print_output("type = DcSurfaceShade");
	break;

    default:
	dor_print_output("type = UNKNOWN");
	break;
    }
}
/*
 ======================================================================
 */

void dor_inttyp_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_inttyp_class_id))
	    return;

    dor_global_inttyp_set_value((DtInterpType)(object->data));
}
