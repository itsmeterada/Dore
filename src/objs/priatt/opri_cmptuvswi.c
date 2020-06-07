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
	int dor_cmptexuvswi_initialize ()
	dot_object * dor_cmptexuvswi_create (switchvalue)
	int dor_cmptexuvswi_print (object)
	int dor_cmptexuvswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

DtInt doe_cmptexuvswi_class_id = -1;
DtFlag doe_cmptexuvswi_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry cmptexuvswi_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_cmptexuvswi_print   },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_cmptexuvswi_execute },
};

void dor_cmptexuvswi_initialize (void)
{
    doe_cmptexuvswi_class_id = dor_class_add("DoCompTextureUVSwitch", 
					2, cmptexuvswi_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_cmptexuvswi_create (DtSwitch switchvalue)
{
    if (!doe_cmptexuvswi_initialized) {
	dor_cmptexuvswi_initialize();
	doe_cmptexuvswi_initialized = DcTrue;
    }

    return (dor_object_create(doe_cmptexuvswi_class_id,(DtPtr)switchvalue));
}
/*
 ======================================================================
 */

void dor_cmptexuvswi_print (dot_object *object)
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

void dor_cmptexuvswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_cmptexuvswi_class_id))
	    return;

    dor_global_cmptexuvswi_set_value((DtSwitch)(object->data));
}
