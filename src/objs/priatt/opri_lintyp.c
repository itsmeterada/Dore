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
	int dor_lintyp_initialize ()
	dot_object * dor_lintyp_create (lintyp)
	int dor_lintyp_print (object)
	int dor_lintyp_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_lintyp_class_id;

static DtMethodEntry lintyp_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_lintyp_print },
    { DcMethodPick,             (DtMethodPtr) dor_lintyp_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_lintyp_execute },
};

void dor_lintyp_initialize (void)
{
    doe_lintyp_class_id = dor_class_install (DcTypeLineType, "DoLineType", 
		3,lintyp_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_lintyp_create (DtLineType lintyp)
{
    static DtFlag initialized = DcFalse;
    dot_object *lintypobject;

    if (!initialized) {
	dor_lintyp_initialize();
	initialized = DcTrue;
    }

    lintypobject = dor_object_create(doe_lintyp_class_id, (DtPtr)lintyp);

    return(lintypobject);
}
/*
 ======================================================================
 */

void dor_lintyp_print (dot_object *object)
{
    sprintf(dor_print_get_line(),"value = %d",(DtInt)(object->data));
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_lintyp_execute (dot_object *object)
{
    if( ! dor_condex_query_execute(doe_lintyp_class_id))
	    return;

    dor_global_lintyp_set_value((DtLineType)(object->data));
}
