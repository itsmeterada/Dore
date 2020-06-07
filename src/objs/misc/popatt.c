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
	int dor_popatt_initialize ()
	dot_object * dor_popatt_create ()
	int dor_popatt_print (popatt_object)
	int dor_popatt_execute (popatt_object)

  ======================================================================
 */
#include <dore/internal/dogen.h>

/*
 ======================================================================
 */

DtInt doe_popatt_class_id;

static DtMethodEntry popatt_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_popatt_print   },
    { DcMethodPick,             (DtMethodPtr) dor_popatt_execute },
    { DcMethodIniPick,          (DtMethodPtr) dor_popatt_execute },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_popatt_execute },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_popatt_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_popatt_execute },
};

void dor_popatt_initialize (void)
{
    doe_popatt_class_id = dor_class_install (DcTypePopAttributes, "DoPopAtts", 6,
					     popatt_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_popatt_create (void)
{
    static DtFlag initialized = DcFalse;
    dot_object *popatt_object;

    if (!initialized) {
	dor_popatt_initialize();
	initialized = DcTrue;
    }

    popatt_object = dor_object_create(doe_popatt_class_id,DcNullPtr);

    return popatt_object;
}
/*
 ======================================================================
 */

void dor_popatt_print (
    dot_object *popatt_object)
{
    dor_print_indent_down();
}
/*
 ======================================================================
 */

void dor_popatt_execute (
    dot_object *popatt_object)
{
    if (! dor_condex_query_execute(doe_popatt_class_id))
	    return;

    dor_group_pop(DcTrue);
}
