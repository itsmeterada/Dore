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
	int dor_shdidx_initialize ()
	dot_object * dor_shdidx_create (shdidx)
	int dor_shdidx_print (object)
	int dor_shdidx_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_shdidx_class_id;

static DtMethodEntry shdidx_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_shdidx_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_shdidx_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_shdidx_execute },
};

void dor_shdidx_initialize (void)
{
    doe_shdidx_class_id = dor_class_install (DcTypeShadeIndex, "DoShadeIndex", 3,
					     shdidx_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_shdidx_create (
    DtInt shdidx)
{
    static DtFlag initialized = DcFalse;
    dot_object *shdidxobject;

    if (!initialized) {
	dor_shdidx_initialize();
	initialized = DcTrue;
    }

    shdidxobject = dor_object_create(doe_shdidx_class_id,(DtPtr)shdidx);

    return(shdidxobject);
}
/*
 ======================================================================
 */

void dor_shdidx_print (
    dot_object *object)
{
    sprintf(dor_print_get_line(),"index value = %d",(DtInt)(object->data));
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_shdidx_execute (
    dot_object *object)
{
    if (!dor_condex_query_execute(doe_shdidx_class_id))
	    return;

    dor_global_shdidx_set_value((DtInt)(object->data));
}
