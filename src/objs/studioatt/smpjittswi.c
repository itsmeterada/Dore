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
	int dor_smpjitterswi_initialize ()
	dot_object * dor_smpjitterswi_create (smpjitterswi)
	int dor_smpjitterswi_destroy (object)
	int dor_smpjitterswi_print (object)
	int dor_smpjitterswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>

DtInt doe_smpjitterswi_class_id = -1;
DtFlag doe_smpjitterswi_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry smpjitterswi_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_smpjitterswi_print },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_smpjitterswi_execute },
};

void dor_smpjitterswi_initialize (void)
{
    doe_smpjitterswi_class_id = dor_class_add("DoSampleJitterSwitch", 2,
					      smpjitterswi_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_smpjitterswi_create (DtSwitch smpjitterswi)
{
    dot_object *smpjitterswi_object;

    if (!doe_smpjitterswi_initialized) {
	dor_smpjitterswi_initialize();
	doe_smpjitterswi_initialized = DcTrue;
    }

    smpjitterswi_object = dor_object_create(doe_smpjitterswi_class_id,(DtPtr)smpjitterswi);

    return(smpjitterswi_object);
}
/*
 ======================================================================
 */

void dor_smpjitterswi_destroy (dot_object *object)
{
    struct smpjitterswi_data *smpjitterswi;

    smpjitterswi = (struct smpjitterswi_data *)(object->data);

    dor_space_deallocate(smpjitterswi);
}
/*
 ======================================================================
 */

void dor_smpjitterswi_print (dot_object *object)
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

void dor_smpjitterswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_smpjitterswi_class_id))
	    return;

    dor_global_smpjitterswi_set_value((DtSwitch)(object->data));
}
