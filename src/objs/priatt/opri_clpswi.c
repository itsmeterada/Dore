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
 
/* ======================================================================
  Functions:
	dor_clpswi_initialize
	dor_clpswi_create
	dor_clpswi_print
	dor_clpswi_execute

======================================================================== */

#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_clpswi_class_id;

static DtMethodEntry clpswi_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_clpswi_print },
    { DcMethodPick,             (DtMethodPtr) dor_clpswi_execute },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_clpswi_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_clpswi_execute },
};

void dor_clpswi_initialize (void)
{
    doe_clpswi_class_id = dor_class_install (DcTypeClipSwitch, "DoClipSwitch", 
				4, clpswi_methods,DcNullPtr);

}
/*
 ======================================================================
 */

dot_object *dor_clpswi_create (DtSwitch clpswi)
{
    static DtFlag initialized = DcFalse;
    dot_object *clpswiobject;

    if (!initialized) {
	dor_clpswi_initialize();
	initialized = DcTrue;
    }

    clpswiobject = dor_object_create(doe_clpswi_class_id,(DtPtr)clpswi);

    return(clpswiobject);
}
/*
 ======================================================================
 */

void dor_clpswi_print (dot_object *object)
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

void dor_clpswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_clpswi_class_id))
	    return;

    dor_global_clpswi_set_value((DtSwitch)(object->data));
}
