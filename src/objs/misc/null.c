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
	int dor_null_initialize ()
	DtObject dor_null_create (str)
	int dor_null_default_method (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>

/*
 ======================================================================
 */

DtInt doe_null_class_id;

void dor_null_initialize (void)
{
    doe_null_class_id = dor_class_install (DcTypeNull, "DoNull", 0, DcNullPtr, 
					   dor_null_default_method);
}
/*
 ======================================================================
 */

DtObject dor_null_create (DtPtr data)
{
    static DtFlag initialized = DcFalse;
    DtObject nullobject;

    if (!initialized) {
	dor_null_initialize();
	initialized = DcTrue;
    }

    nullobject = dor_object_create (doe_null_class_id, data);

    return nullobject;
}
/*
 ======================================================================
 */

void *dor_null_default_method (
    dot_object *object)
{
    char errstring[50];

    if ((object == DcNullPtr) || (object->data == DcNullPtr))
        DDerror (ERR_EXEC_NULL_OBJ, "dor_null_default_method", DcNullPtr);
    else {
        sprintf (errstring,
                 "Object handle is [%lx].", (dot_object *)(object->data));
        DDerror (ERR_EXEC_NULL_OBJ, "dor_null_default_method", errstring);
    }

    return 0;
}
