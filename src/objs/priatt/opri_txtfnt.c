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
	int dor_txtfnt_initialize ()
	dot_object * dor_txtfnt_create (txtfnt)
	int dor_txtfnt_print (object)
	int dor_txtfnt_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_txtfnt_class_id;

static DtMethodEntry txtfnt_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_txtfnt_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_txtfnt_print },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_txtfnt_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_txtfnt_execute },
};

void dor_txtfnt_initialize (void)
{
    doe_txtfnt_class_id = dor_class_install (DcTypeTextFont, "DoTextFont", 
		4, txtfnt_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_txtfnt_create (
    DtFont txtfnt)
{
    static DtFlag initialized = DcFalse;
    dot_object *txtfntobject;

    if (!initialized) {
	dor_txtfnt_initialize();
	initialized = DcTrue;
    }

    txtfntobject = dor_object_create(doe_txtfnt_class_id, (DtPtr)txtfnt);

    return(txtfntobject);
}
/*
 ======================================================================
 */

void dor_txtfnt_print (
    dot_object *object)
{
    sprintf (dor_print_get_line(),"value = %lx", object->data);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_txtfnt_execute (
    dot_object *object)
{
    if( ! dor_condex_query_execute(doe_txtfnt_class_id))
	    return;

    dor_global_txtfnt_set_value((DtFont)(object->data));
}
