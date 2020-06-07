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
	int dor_pckid_initialize ()
	dot_object * dor_pckid_create (pckid)
	int dor_pckid_print (object)
	int dor_pckid_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>

/*
 ======================================================================
 */

DtInt doe_pckid_class_id;

static DtMethodEntry pckid_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_pckid_print   },
    { DcMethodPick,             (DtMethodPtr) dor_pckid_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_pckid_execute },
};

void dor_pckid_initialize (void)
{
    doe_pckid_class_id = dor_class_install (DcTypePickID, "DoPickID", 3,
					    pckid_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_pckid_create (DtInt pckid)
{
    static DtFlag initialized = DcFalse;
    dot_object *pckidobject;

    if (!initialized) {
	dor_pckid_initialize();
	initialized = DcTrue;
    }

    /* The following cast from the DtInt pckid to a stored DtPtr value
    ** assumes that a DtPtr can hold a DtInt value.  This will probably
    ** be true for most platforms, but it's still a bit dangerous. */

    pckidobject = dor_object_create (doe_pckid_class_id, (DtPtr)pckid);

    return(pckidobject);
}
/*
 ======================================================================
 */

void dor_pckid_print (dot_object *object)
{
    /* Cast from the DtPtr object->data back to the original DtInt value.
    ** See comment in dor_label_create(). */

    sprintf (dor_print_get_line(), "value = %d", (DtInt)(object->data));
    dor_print_output (dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_pckid_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_pckid_class_id))
	return;

    /* Cast from the DtPtr object->data back to the original DtInt value.
    ** See comment in dor_label_create(). */

    dor_global_pckid_set_value ((DtInt)(object->data));
}
