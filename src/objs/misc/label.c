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
	int dor_label_initialize ()
	dot_object * dor_label_create (label)
	int dor_label_print (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>

/*
 ======================================================================
 */

DtInt doe_label_class_id;

static DtMethodEntry label_methods[] = {
    { DcMethodPrint, (DtMethodPtr) dor_label_print }
};

void dor_label_initialize (void)
{
    doe_label_class_id = dor_class_install (DcTypeLabel, "DoLabel", 1, 
					    label_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_label_create (DtInt label)
{
    static DtFlag initialized = DcFalse;
    dot_object *labelobject;

    if (!initialized) {
	dor_label_initialize ();
	initialized = DcTrue;
    }

    /* The following cast from the DtInt label to a stored DtPtr value
    ** assumes that a DtPtr can hold a DtInt value.  This will probably
    ** be true for most platforms, but it's still a bit dangerous. */

    labelobject = dor_object_create (doe_label_class_id, (DtPtr)label);

    return labelobject;
}
/*
 ======================================================================
 */

void dor_label_print (dot_object *object)
{
    /* Cast from the DtPtr object->data back to the original DtInt value.
    ** See comment in dor_label_create(). */

    sprintf (dor_print_get_line(), "value = %d", (DtInt)(object->data));
    dor_print_output (dor_print_get_line());
}
