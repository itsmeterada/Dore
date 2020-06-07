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
	int dor_transpswi_initialize ()
	dot_object * dor_transpswi_create (transpswi)
	int dor_transpswi_print (object)
	int dor_transpswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_transpswi_class_id;

static DtMethodEntry transpswi_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_transpswi_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_transpswi_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_transpswi_execute },
};

void dor_transpswi_initialize (void)
{
    doe_transpswi_class_id = dor_class_install (DcTypeTranspSwitch, 
		"DoTranspSwitch", 3, transpswi_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_transpswi_create (DtSwitch transpswi)
{
    static DtFlag initialized = DcFalse;
    dot_object *transpswiobject;

    if (!initialized) {
	dor_transpswi_initialize();
	initialized = DcTrue;
    }

    transpswiobject = dor_object_create(doe_transpswi_class_id,(DtPtr)transpswi);

    return(transpswiobject);
}
/*
 ======================================================================
 */

void dor_transpswi_print (dot_object *object)
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

void dor_transpswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_transpswi_class_id))
	    return;

    dor_global_transpswi_set_value((DtSwitch)(object->data));
}
