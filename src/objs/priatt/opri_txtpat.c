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
	int dor_txtpat_initialize ()
	dot_object * dor_txtpat_create (txtpat)
	int dor_txtpat_print (object)
	int dor_txtpat_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_txtpat_class_id;

static DtMethodEntry txtpat_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_txtpat_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_txtpat_print },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_txtpat_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_txtpat_execute },
};

void dor_txtpat_initialize (void)
{
    doe_txtpat_class_id = dor_class_install (DcTypeTextPath, "DoTextPath", 
		4,txtpat_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_txtpat_create (DtTextPath txtpat)
{
    static DtFlag initialized = DcFalse;
    dot_object *txtpatobject;

    if (!initialized) {
	dor_txtpat_initialize();
	initialized = DcTrue;
    }

    txtpatobject = dor_object_create(doe_txtpat_class_id, (DtPtr)txtpat);

    return(txtpatobject);
}
/*
 ======================================================================
 */

void dor_txtpat_print (dot_object *object)
{
    switch((DtTextPath)(object->data)) {
    case DcTextPathRight:
	dor_print_output("value = DcTextPathRight");
	break;
    case DcTextPathLeft:
	dor_print_output("value = DcTextPathLeft");
	break;
    case DcTextPathUp:
	dor_print_output("value = DcTextPathUp");
	break;
    case DcTextPathDown:
	dor_print_output("value = DcTextPathDown");
	break;
    }
}
/*
 ======================================================================
 */

void dor_txtpat_execute (dot_object *object)
{
    if( ! dor_condex_query_execute(doe_txtpat_class_id))
	    return;

    dor_global_txtpat_set_value ((DtTextPath)(object->data));
}
