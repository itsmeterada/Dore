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
	int dor_smpsuperswi_initialize ()
	dot_object * dor_smpsuperswi_create (smpsuperswi)
	int dor_smpsuperswi_destroy (object)
	int dor_smpsuperswi_print (object)
	int dor_smpsuperswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>

DtInt doe_smpsuperswi_class_id = -1;
DtFlag doe_smpsuperswi_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry smpsuperswi_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_smpsuperswi_print },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_smpsuperswi_execute },
};

void dor_smpsuperswi_initialize (void)
{

    doe_smpsuperswi_class_id = dor_class_add("DoSampleSuperSwitch", 2,
					     smpsuperswi_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_smpsuperswi_create (DtSwitch smpsuperswi)
{
    dot_object *smpsuperswi_object;

    if (!doe_smpsuperswi_initialized) {
	dor_smpsuperswi_initialize();
	doe_smpsuperswi_initialized = DcTrue;
    }

    smpsuperswi_object = dor_object_create(doe_smpsuperswi_class_id,(DtPtr)smpsuperswi);

    return(smpsuperswi_object);
}
/*
 ======================================================================
 */

void dor_smpsuperswi_destroy (dot_object *object)
{
    struct smpsuperswi_data *smpsuperswi;

    smpsuperswi = (struct smpsuperswi_data *)(object->data);

    dor_space_deallocate(smpsuperswi);
}
/*
 ======================================================================
 */

void dor_smpsuperswi_print (dot_object *object)
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

void dor_smpsuperswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_smpsuperswi_class_id))
	    return;

    dor_global_smpsuperswi_set_value((DtSwitch)(object->data));
}
