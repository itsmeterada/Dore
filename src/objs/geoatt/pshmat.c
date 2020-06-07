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
	int dor_pshmat_initialize ()
	dot_object * dor_pshmat_create ()
	int dor_pshmat_print (object)
	int dor_pshmat_execute (object)
	int dor_pshmat_dynexe (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/geoatt.h>
#include <dore/internal/glbatt.h>

/*
 ======================================================================
 */

DtInt doe_pshmat_class_id;

static DtMethodEntry pshmat_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_pshmat_print },
    { DcMethodIniPick,          (DtMethodPtr) dor_pshmat_execute },
    { DcMethodPick,             (DtMethodPtr) dor_pshmat_execute },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_pshmat_execute },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_pshmat_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_pshmat_execute },
};

void dor_pshmat_initialize (void)
{
    doe_pshmat_class_id = dor_class_install (DcTypePushMatrix, "DoPushMatrix", 
				6, pshmat_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_pshmat_create (void)
{
    static DtFlag initialized = DcFalse;

    if (!initialized) {
	dor_pshmat_initialize();
	initialized = DcTrue;
    }

    return(dor_object_create(doe_pshmat_class_id,DcNullPtr));
}
/*
 ======================================================================
 */

void dor_pshmat_print (
    dot_object *object)
{
    dor_print_indent_up();
}
/*
 ======================================================================
 */

void dor_pshmat_execute (
    dot_object *object)
{
    DtMatrix4x4 newmatrix;

    if (!dor_condex_query_execute(doe_pshmat_class_id))
	    return;

    dor_global_lcstowcsmat_get_value(newmatrix);

    dor_global_lcstowcsmat_push_value(newmatrix);
}
/*
 ======================================================================
 */

/* For dynamics the lcstowcsmat stack is actually maintained in the 
   dynamic output module so the call to dor_global_lcstowcsmat_push_value
   is just the mechanism for keeping track of the pushes / pops of the stack 
   the actual values in the global attribute are not used for dynamics */

static DtMatrix4x4 identmat = { { 1, 0, 0, 0 },
			        { 0, 1, 0, 0 },
			        { 0, 0, 1, 0 },
			        { 0, 0, 0, 1 } };
				     
void dor_pshmat_dynexe (
    dot_object *object)
{
   if (!dor_condex_query_execute(doe_pshmat_class_id))
	 return;

   dor_global_lcstowcsmat_push_value(identmat);
}
