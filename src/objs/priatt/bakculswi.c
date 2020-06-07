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
	int dor_bacfacculswi_initialize ()
	dot_object * dor_bacfacculswi_create (bacfacculswi)
	int dor_bacfacculswi_print (object)
	int dor_bacfacculswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_bacfacculswi_class_id;

static DtMethodEntry bacfacculswi_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_bacfacculswi_print   },
    { DcMethodPick,             (DtMethodPtr) dor_bacfacculswi_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_bacfacculswi_execute },
};

void dor_bacfacculswi_initialize (void)
{
    doe_bacfacculswi_class_id = dor_class_install (DcTypeBackfaceCullSwitch, 
		"DoBackfaceCullSwitch", 3, bacfacculswi_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_bacfacculswi_create (DtSwitch bacfacculswi)
{
    static DtFlag initialized = DcFalse;
    dot_object *bacfacculswiobject;

    if (!initialized) {
	dor_bacfacculswi_initialize();
	initialized = DcTrue;
    }

    bacfacculswiobject = dor_object_create(doe_bacfacculswi_class_id,(DtPtr)bacfacculswi);

    return(bacfacculswiobject);
}
/*
 ======================================================================
 */

void dor_bacfacculswi_print (dot_object *object)
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

void dor_bacfacculswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_bacfacculswi_class_id))
	    return;

    dor_global_bacfacculswi_set_value((DtSwitch)(object->data));
}
