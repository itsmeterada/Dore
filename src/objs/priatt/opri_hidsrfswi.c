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
	int dor_hidsrfswi_initialize ()
	dot_object * dor_hidsrfswi_create (hidsrfswi)
	int dor_hidsrfswi_print (object)
	int dor_hidsrfswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_hidsrfswi_class_id;

static DtMethodEntry hidsrfswi_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_hidsrfswi_print },
    { DcMethodPick,             (DtMethodPtr) dor_hidsrfswi_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_hidsrfswi_execute },
};

void dor_hidsrfswi_initialize (void)
{

    doe_hidsrfswi_class_id = dor_class_install (DcTypeHiddenSurfaceSwitch, 
		"DoHiddenSurfSwitch", 3, hidsrfswi_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_hidsrfswi_create (DtSwitch hidsrfswi)
{
    static DtFlag initialized = DcFalse;
    dot_object *hidsrfswiobject;

    if (!initialized) {
	dor_hidsrfswi_initialize();
	initialized = DcTrue;
    }

    hidsrfswiobject = dor_object_create(doe_hidsrfswi_class_id,
					(DtPtr)hidsrfswi);

    return(hidsrfswiobject);
}
/*
 ======================================================================
 */

void dor_hidsrfswi_print (dot_object *object)
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

void dor_hidsrfswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_hidsrfswi_class_id))
	    return;

    dor_global_hidsrfswi_set_value((DtSwitch)(object->data));
}
