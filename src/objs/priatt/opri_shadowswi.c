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
	int dor_shadowswi_initialize ()
	dot_object * dor_shadowswi_create (shadowswi)
	int dor_shadowswi_print (object)
	int dor_shadowswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_shadowswi_class_id;

static DtMethodEntry shadowswi_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_shadowswi_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_shadowswi_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_shadowswi_execute },
};

void dor_shadowswi_initialize (void)
{
    doe_shadowswi_class_id = dor_class_install (DcTypeShadowSwitch, 
		"DoShadowSwitch", 3, shadowswi_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_shadowswi_create (DtSwitch shadowswi)
{
    static DtFlag initialized = DcFalse;
    dot_object *shadowswiobject;

    if (!initialized) {
	dor_shadowswi_initialize();
	initialized = DcTrue;
    }

    shadowswiobject = dor_object_create(doe_shadowswi_class_id,(DtPtr)shadowswi);

    return(shadowswiobject);
}
/*
 ======================================================================
 */

void dor_shadowswi_print (dot_object *object)
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

void dor_shadowswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_shadowswi_class_id))
	    return;

    dor_global_shadowswi_set_value((DtSwitch)(object->data));
}
