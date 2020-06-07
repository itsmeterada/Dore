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
	int dor_pckswi_initialize ()
	dot_object * dor_pckswi_create (pckswi)
	int dor_pckswi_print (object)
	int dor_pckswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_pckswi_class_id;

static DtMethodEntry pckswi_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_pckswi_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_pckswi_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_pckswi_execute },
};

void dor_pckswi_initialize (void)
{
    doe_pckswi_class_id = dor_class_install (DcTypePickabilitySwitch, 
		"DoPickSwitch", 3, pckswi_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_pckswi_create (DtSwitch pckswi)
{
    static DtFlag initialized = DcFalse;
    dot_object *pckswiobject;

    if (!initialized) {
	dor_pckswi_initialize();
	initialized = DcTrue;
    }

    pckswiobject = dor_object_create(doe_pckswi_class_id,(DtPtr)pckswi);

    return(pckswiobject);
}
/*
 ======================================================================
 */

void dor_pckswi_print (dot_object *object)
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

void dor_pckswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_pckswi_class_id))
	    return;

    dor_global_pckswi_set_value((DtSwitch)(object->data));
}
