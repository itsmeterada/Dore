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
	int dor_mkrgly_initialize ()
	dot_object * dor_mkrgly_create (mkrgly)
	int dor_mkrgly_print (object)
	int dor_mkrgly_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_mkrgly_class_id;

static DtMethodEntry mkrgly_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_mkrgly_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_mkrgly_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_mkrgly_execute },
};

void dor_mkrgly_initialize (void)
{
    doe_mkrgly_class_id = dor_class_install (DcTypeMarkerGlyph, "DoMarkerGlyph",
					     3, mkrgly_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_mkrgly_create (
    DtInt mkrgly)
{
    static DtFlag initialized = DcFalse;
    dot_object *mkrglyobject;

    if (!initialized) {
	dor_mkrgly_initialize();
	initialized = DcTrue;
    }

    mkrglyobject = dor_object_create(doe_mkrgly_class_id, (DtPtr)mkrgly);

    return(mkrglyobject);
}
/*
 ======================================================================
 */

void dor_mkrgly_print (
    dot_object *object)
{
    sprintf(dor_print_get_line(),"value = %d",(DtInt)(object->data));
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_mkrgly_execute (
    dot_object *object)
{
    if( ! dor_condex_query_execute(doe_mkrgly_class_id))
	    return;

    dor_global_mkrgly_set_value((DtInt)(object->data));
}
