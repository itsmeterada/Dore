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
	int dor_ambswi_initialize ()
	dot_object * dor_ambswi_create (ambswi)
	int dor_ambswi_print (object)
	int dor_ambswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_ambswi_class_id;

static DtMethodEntry ambswi_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_ambswi_print   },
    { DcMethodPick,             (DtMethodPtr) dor_ambswi_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_ambswi_execute },
};

void dor_ambswi_initialize (void)
{
    doe_ambswi_class_id = dor_class_install (DcTypeAmbientSwitch, "DoAmbientSwitch",
		3, ambswi_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_ambswi_create (DtSwitch ambswi)
{
    static DtFlag      initialized = DcFalse;
    auto   dot_object *ambswiobject;

    if (!initialized) {
	dor_ambswi_initialize();
	initialized = DcTrue;
    }

    ambswiobject = dor_object_create(doe_ambswi_class_id,(DtPtr)ambswi);

    return(ambswiobject);
}
/*
 ======================================================================
 */

void dor_ambswi_print (dot_object *object)
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

void dor_ambswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_ambswi_class_id))
	    return;

    dor_global_ambswi_set_value((DtSwitch)(object->data));
}
