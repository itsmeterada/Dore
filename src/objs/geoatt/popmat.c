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
	int dor_popmat_initialize ()
	struct object * dor_popmat_create ()
	int dor_popmat_print (object)
	int dor_popmat_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/geoatt.h>
#include <dore/internal/glbatt.h>

/*
 ======================================================================
 */

DtInt doe_popmat_class_id;

static DtMethodEntry popmat_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_popmat_print },
    { DcMethodIniPick,          (DtMethodPtr) dor_popmat_execute },
    { DcMethodPick,             (DtMethodPtr) dor_popmat_execute },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_popmat_execute },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_popmat_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_popmat_execute },
};

void dor_popmat_initialize (void)
{
    doe_popmat_class_id = dor_class_install (DcTypePopMatrix, "DoPopMatrix", 6,
					     popmat_methods,DcNullPtr);
}
/*
 ======================================================================
 */

struct object *dor_popmat_create (void)
{
    static DtFlag initialized = DcFalse;

    if (!initialized) {
	dor_popmat_initialize();
	initialized = DcTrue;
    }

    return(dor_object_create(doe_popmat_class_id,DcNullPtr));
}
/*
 ======================================================================
 */

void dor_popmat_print (
    dot_object *object)
{
    dor_print_indent_down();
}
/*
 ======================================================================
 */

void dor_popmat_execute (
    dot_object *object)
{
    if (!dor_condex_query_execute(doe_popmat_class_id))
	    return;

    if (!dor_global_lcstowcsmat_pop_value()){
	DDerror (ERR_CANT_POP_MATRIX,
		    "dor_popmat_execute", DcNullPtr);
		
	return;
    }
}
