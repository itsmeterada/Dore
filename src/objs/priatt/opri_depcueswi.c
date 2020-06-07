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
	int dor_depcueswi_initialize ()
	dot_object * dor_depcueswi_create (depcueswi)
	int dor_depcueswi_print (object)
	int dor_depcueswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_depcueswi_class_id;

static DtMethodEntry depcueswi_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_depcueswi_print   },
    { DcMethodPick,             (DtMethodPtr) dor_depcueswi_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_depcueswi_execute },
};

void dor_depcueswi_initialize (void)
{
    doe_depcueswi_class_id = dor_class_add ("DoDepthCueSwitch", 3,
					    depcueswi_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_depcueswi_create (DtSwitch depcueswi)
{
    static DtFlag initialized = DcFalse;
    dot_object *depcueswiobject;

    if (!initialized) {
	dor_depcueswi_initialize();
	initialized = DcTrue;
    }

    depcueswiobject = dor_object_create(doe_depcueswi_class_id,(DtPtr)depcueswi);

    return(depcueswiobject);
}
/*
 ======================================================================
 */

void dor_depcueswi_print (dot_object *object)
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

void dor_depcueswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_depcueswi_class_id))
	    return;

    dor_global_depcueswi_set_value((DtSwitch)(object->data));
}
