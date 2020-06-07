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
	int dor_difswi_initialize ()
	dot_object * dor_difswi_create (difswi)
	int dor_difswi_print (object)
	int dor_difswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_difswi_class_id;

static DtMethodEntry difswi_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_difswi_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_difswi_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_difswi_execute },
};

void dor_difswi_initialize (void)
{
    doe_difswi_class_id = dor_class_install (DcTypeDiffuseSwitch, "DoDiffuseSwitch", 
		3, difswi_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_difswi_create (DtSwitch difswi)
{
    static DtFlag initialized = DcFalse;
    dot_object *difswiobject;

    if (!initialized) {
	dor_difswi_initialize();
	initialized = DcTrue;
    }

    difswiobject = dor_object_create(doe_difswi_class_id,(DtPtr)difswi);

    return(difswiobject);
}
/*
 ======================================================================
 */

void dor_difswi_print (dot_object *object)
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

void dor_difswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_difswi_class_id))
	    return;

    dor_global_difswi_set_value((DtSwitch)(object->data));
}
