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
	int dor_cmptexuvwswi_initialize ()
	dot_object * dor_cmptexuvwswi_create (switchvalue)
	int dor_cmptexuvwswi_print (object)
	int dor_cmptexuvwswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

DtInt doe_cmptexuvwswi_class_id = -1;
DtFlag doe_cmptexuvwswi_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry cmptexuvwswi_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_cmptexuvwswi_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_cmptexuvwswi_execute },
};

void dor_cmptexuvwswi_initialize (void)
{
    doe_cmptexuvwswi_class_id = dor_class_add("DoCompTextureUVWSwitch", 
					2, cmptexuvwswi_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_cmptexuvwswi_create (DtSwitch switchvalue)
{
    if (!doe_cmptexuvwswi_initialized) {
	dor_cmptexuvwswi_initialize();
	doe_cmptexuvwswi_initialized = DcTrue;
    }

    return (dor_object_create(doe_cmptexuvwswi_class_id,(DtPtr)switchvalue));
}
/*
 ======================================================================
 */

void dor_cmptexuvwswi_print (dot_object *object)
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

void dor_cmptexuvwswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_cmptexuvwswi_class_id))
	    return;

    dor_global_cmptexuvwswi_set_value((DtSwitch)(object->data));
}
