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
	int dor_invswi_initialize ()
	dot_object * dor_invswi_create (invswi)
	int dor_invswi_print (object)
	int dor_invswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_invswi_class_id;

static DtMethodEntry invswi_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_invswi_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_invswi_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_invswi_execute },
};

void dor_invswi_initialize (void)
{
    doe_invswi_class_id = dor_class_install (DcTypeInvisibilitySwitch, 
		"DoInvisSwitch", 3, invswi_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_invswi_create (DtSwitch invswi)
{
    static DtFlag initialized = DcFalse;
    dot_object *invswiobject;

    if (!initialized) {
	dor_invswi_initialize();
	initialized = DcTrue;
    }

    invswiobject = dor_object_create(
				     doe_invswi_class_id,(DtPtr)invswi);

    return(invswiobject);
}
/*
 ======================================================================
 */

void dor_invswi_print (dot_object *object)
{
    switch ((DtSwitch)(object->data)){
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

void dor_invswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_invswi_class_id))
	    return;

    dor_global_invswi_set_value((DtSwitch)(object->data));
}
