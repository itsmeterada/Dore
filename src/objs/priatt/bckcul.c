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
	int dor_bacfacculble_initialize ()
	dot_object * dor_bacfacculble_create (bacfacculble)
	int dor_bacfacculble_print (object)
	int dor_bacfacculble_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_bacfacculble_class_id;

static DtMethodEntry bacfacculble_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_bacfacculble_print   },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_bacfacculble_execute },
    { DcMethodPick,             (DtMethodPtr) dor_bacfacculble_execute },
};

void dor_bacfacculble_initialize (void)
{
    doe_bacfacculble_class_id = dor_class_install (DcTypeBackfaceCullable, 
		"DoBackfaceCullable", 3, bacfacculble_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_bacfacculble_create (DtSwitch bacfacculble)
{
    static DtFlag initialized = DcFalse;
    dot_object *bacfacculbleobject;

    if (!initialized) {
	dor_bacfacculble_initialize();
	initialized = DcTrue;
    }

    bacfacculbleobject = dor_object_create(doe_bacfacculble_class_id,(DtPtr)bacfacculble);

    return(bacfacculbleobject);
}
/*
 ======================================================================
 */

void dor_bacfacculble_print (dot_object *object)
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

void dor_bacfacculble_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_bacfacculble_class_id))
	    return;

    dor_global_bacfacculble_set_value((DtSwitch)(object->data));
}
