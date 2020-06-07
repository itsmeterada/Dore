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
	int dor_localaaswi_initialize ()
	dot_object * dor_localaaswi_create (localaaswi)
	int dor_localaaswi_destroy (object)
	int dor_localaaswi_print (object)
	int dor_localaaswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

DtInt doe_localaaswi_class_id = -1;
DtFlag doe_localaaswi_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry localaaswi_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_localaaswi_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_localaaswi_execute },
};

void dor_localaaswi_initialize (void)
{

    doe_localaaswi_class_id = dor_class_add("DoLocalAntiAliasSwitch", 2,
					    localaaswi_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_localaaswi_create (DtSwitch localaaswi)
{
    dot_object *localaaswi_object;

    if (!doe_localaaswi_initialized) {
	dor_localaaswi_initialize();
	doe_localaaswi_initialized = DcTrue;
    }

    localaaswi_object = dor_object_create(doe_localaaswi_class_id,(DtPtr)localaaswi);

    return(localaaswi_object);
}
/*
 ======================================================================
 */

void dor_localaaswi_destroy (dot_object *object)
{
    struct localaaswi_data *localaaswi;

    localaaswi = (struct localaaswi_data *)(object->data);

    dor_space_deallocate(localaaswi);
}
/*
 ======================================================================
 */

void dor_localaaswi_print (dot_object *object)
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

void dor_localaaswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_localaaswi_class_id))
	    return;

    dor_global_localaaswi_set_value((DtSwitch)(object->data));
}
