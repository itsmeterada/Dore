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
	int dor_spcswi_initialize ()
	dot_object * dor_spcswi_create (spcswi)
	int dor_spcswi_print (object)
	int dor_spcswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_spcswi_class_id;

static DtMethodEntry spcswi_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_spcswi_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_spcswi_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_spcswi_execute },
};

void dor_spcswi_initialize (void)
{
    doe_spcswi_class_id = dor_class_install (DcTypeSpecularSwitch, 
		"DoSpecularSwitch", 3, spcswi_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_spcswi_create (DtSwitch spcswi)
{
    static DtFlag initialized = DcFalse;
    dot_object *spcswiobject;

    if (!initialized) {
	dor_spcswi_initialize();
	initialized = DcTrue;
    }

    spcswiobject = dor_object_create(doe_spcswi_class_id,(DtPtr)spcswi);

    return(spcswiobject);
}
/*
 ======================================================================
 */

void dor_spcswi_print (dot_object *object)
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

void dor_spcswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_spcswi_class_id))
	    return;

    dor_global_spcswi_set_value((DtSwitch)(object->data));
}
