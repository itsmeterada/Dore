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
	int dor_mkrfnt_initialize ()
	dot_object * dor_mkrfnt_create (mkrfnt)
	int dor_mkrfnt_print (object)
	int dor_mkrfnt_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_mkrfnt_class_id;

static DtMethodEntry mkrfnt_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_mkrfnt_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_mkrfnt_execute },
};


void dor_mkrfnt_initialize (void)
{
    doe_mkrfnt_class_id = dor_class_install
			  (   DcTypeMarkerFont, "DoMarkerFont", 2,
			      mkrfnt_methods, DcNullPtr
			  );
}
/*
 ======================================================================
 */

dot_object *dor_mkrfnt_create (
    DtFont mkrfnt)
{
    static DtFlag initialized = DcFalse;
    dot_object *mkrfntobject;

    if (!initialized) {
	dor_mkrfnt_initialize();
	initialized = DcTrue;
    }

    mkrfntobject = dor_object_create(doe_mkrfnt_class_id, (DtPtr)mkrfnt);

    return(mkrfntobject);
}
/*
 ======================================================================
 */

void dor_mkrfnt_print (
    dot_object *object)
{
    sprintf(dor_print_get_line(),"value = %d",(DtInt)(object->data));
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_mkrfnt_execute (
    dot_object *object)
{
    if( ! dor_condex_query_execute(doe_mkrfnt_class_id))
	    return;

    dor_global_mkrfnt_set_value((DtFont)(object->data));
}
