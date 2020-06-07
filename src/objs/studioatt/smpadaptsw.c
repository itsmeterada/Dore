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
	int dor_smpadaptswi_initialize ()
	dot_object * dor_smpadaptswi_create (smpadaptswi)
	int dor_smpadaptswi_destroy (object)
	int dor_smpadaptswi_print (object)
	int dor_smpadaptswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>

DtInt doe_smpadaptswi_class_id = -1;
DtFlag doe_smpadaptswi_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry smpadaptswi_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_smpadaptswi_print },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_smpadaptswi_execute },
};

void dor_smpadaptswi_initialize (void)
{

    doe_smpadaptswi_class_id = dor_class_add("DoSampleAdaptiveSwitch", 2,
					     smpadaptswi_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_smpadaptswi_create (DtSwitch smpadaptswi)
{
    dot_object *smpadaptswi_object;

    if (!doe_smpadaptswi_initialized) {
	dor_smpadaptswi_initialize();
	doe_smpadaptswi_initialized = DcTrue;
    }

    smpadaptswi_object = dor_object_create(doe_smpadaptswi_class_id,(DtPtr)smpadaptswi);

    return(smpadaptswi_object);
}
/*
 ======================================================================
 */

void dor_smpadaptswi_destroy (dot_object *object)
{
    struct smpadaptswi_data *smpadaptswi;

    smpadaptswi = (struct smpadaptswi_data *)(object->data);

    dor_space_deallocate(smpadaptswi);
}
/*
 ======================================================================
 */

void dor_smpadaptswi_print (dot_object *object)
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

void dor_smpadaptswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_smpadaptswi_class_id))
	    return;

    dor_global_smpadaptswi_set_value ((DtSwitch)(object->data));
}
