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
	int dor_pshatt_initialize ()
	dot_object * dor_pshatt_create ()
	int dor_pshatt_destroy (pshatt_object)
	int dor_pshatt_print (pshatt_object)
	int dor_pshatt_execute (pshatt_object)

  ======================================================================
 */
#include <dore/internal/dogen.h>

/* ====================================================================== */

DtInt doe_pshatt_class_id;

static DtMethodEntry pshatt_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_pshatt_print   },
    { DcMethodDestroy,          (DtMethodPtr) dor_pshatt_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_pshatt_execute },
    { DcMethodIniPick,          (DtMethodPtr) dor_pshatt_execute },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_pshatt_execute },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_pshatt_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_pshatt_execute },
};

void dor_pshatt_initialize (void)
{
    doe_pshatt_class_id = dor_class_install
			  (   DcTypePushAttributes, "DoPushAtts",
			      7, pshatt_methods, DcNullPtr
			  );
}

/* ====================================================================== */

dot_object *dor_pshatt_create (void)
{
    static DtFlag initialized = DcFalse;
    dot_object *group;
    dot_object *pshatt_object;

    if (!initialized) {
	dor_pshatt_initialize();
	initialized = DcTrue;
    }

    /* create a group to handle attribute push and pop */

    group = dor_group_create(DcFalse, DcFalse);

    pshatt_object = dor_object_create(doe_pshatt_class_id,group);

    return pshatt_object;
}

/* ====================================================================== */

void dor_pshatt_destroy (
    dot_object *pshatt_object)
{
    dot_object *group = (dot_object *)pshatt_object->data;

    dor_object_check_deletion (group);
}

/* ====================================================================== */

void dor_pshatt_print (
    dot_object *pshatt_object)
{
    dor_print_indent_up ();
}

/* ====================================================================== */

void dor_pshatt_execute (
    dot_object *pshatt_object)
{
    dot_object *group = (dot_object *)pshatt_object->data;

    if (! dor_condex_query_execute(doe_pshatt_class_id))
	return;

    dor_group_push (group, DcTrue);
}
