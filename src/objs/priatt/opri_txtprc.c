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
	int dor_txtprc_initialize ()
	dot_object * dor_txtprc_create (txtprc)
	int dor_txtprc_print (object)
	int dor_txtprc_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_txtprc_class_id;

static DtMethodEntry txtprc_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_txtprc_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_txtprc_print },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_txtprc_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_txtprc_execute },
};

void dor_txtprc_initialize (void)
{
    doe_txtprc_class_id = dor_class_install (DcTypeTextPrecision, 
		"DoTextPrecision", 4, txtprc_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_txtprc_create (DtTextPrecision txtprc)
{
    static DtFlag initialized = DcFalse;
    dot_object *txtprcobject;

    if (!initialized) {
	dor_txtprc_initialize();
	initialized = DcTrue;
    }

    txtprcobject = dor_object_create(doe_txtprc_class_id, (DtPtr)txtprc);

    return(txtprcobject);
}
/*
 ======================================================================
 */

void dor_txtprc_print (dot_object *object)
{
    switch((DtTextPrecision)(object->data)) {
    case DcStringPrecision:
	dor_print_output("value = DcStringPrecision");
	break;
    case DcCharacterPrecision:
	dor_print_output("value = DcCharacterPrecision");
	break;
    case DcStrokePrecision:
	dor_print_output("value = DcStrokePrecision");
	break;
    }
}
/*
 ======================================================================
 */

void dor_txtprc_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_txtprc_class_id)) return;

    dor_global_txtprc_set_value ((DtTextPrecision)(object->data));
}
