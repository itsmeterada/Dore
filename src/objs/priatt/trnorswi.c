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
	int dor_transporientswi_initialize ()
	dot_object * dor_transporientswi_create (switchvalue)
	int dor_transporientswi_print (object)
	int dor_transporientswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

DtInt doe_transporientswi_class_id = -1;
DtFlag doe_transporientswi_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry transporientswi_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_transporientswi_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_transporientswi_execute },
};

void dor_transporientswi_initialize (void)
{
    doe_transporientswi_class_id = dor_class_add("DoTranspOrientSwitch", 2,
						 transporientswi_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_transporientswi_create (DtSwitch switchvalue)
{
    if (!doe_transporientswi_initialized) {
	dor_transporientswi_initialize();
	doe_transporientswi_initialized = DcTrue;
    }

    return (dor_object_create(doe_transporientswi_class_id,(DtPtr)switchvalue));
}
/*
 ======================================================================
 */

void dor_transporientswi_print (dot_object *object)
{
    switch ((DtSwitch)(object->data)) {
    case DcOff:
	dor_print_output("switchvalue = DcOff");
	break;

    case DcOn:
	dor_print_output("switchvalue = DcOn");
	break;

    default:
	dor_print_output("switchvalue = UNKNOWN");
	break;
    }
}
/*
 ======================================================================
 */

void dor_transporientswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_transporientswi_class_id))
	    return;

    dor_global_transporientswi_set_value((DtSwitch)(object->data));
}
