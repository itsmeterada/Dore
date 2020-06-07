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
	int dor_glbrndmaxsub_initialize ()
	dot_object * dor_glbrndmaxsub_create (glbrndmaxsub)
	int dor_glbrndmaxsub_print (object)
	int dor_glbrndmaxsub_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>

/*
 ======================================================================
 */

DtInt doe_glbrndmaxsub_class_id;

static DtMethodEntry glbrndmaxsub_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_glbrndmaxsub_print },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_glbrndmaxsub_execute },
};

void dor_glbrndmaxsub_initialize (void)
{
    doe_glbrndmaxsub_class_id = dor_class_install (DcTypeGlbRndMaxSub, 
		"DoGlbRendMaxSub", 2, glbrndmaxsub_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_glbrndmaxsub_create (DtInt glbrndmaxsub)
{
    static DtFlag initialized = DcFalse;
    dot_object *glbrndmaxsubobject;

    if (!initialized) {
	dor_glbrndmaxsub_initialize();
	initialized = DcTrue;
    }

    glbrndmaxsubobject = dor_object_create(doe_glbrndmaxsub_class_id,
					   (DtPtr)glbrndmaxsub);

    return(glbrndmaxsubobject);
}
/*
 ======================================================================
 */

void dor_glbrndmaxsub_print (dot_object *object)
{
    sprintf (dor_print_get_line(),"value = %lx", object->data);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_glbrndmaxsub_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_glbrndmaxsub_class_id))
	    return;

    dor_global_glbrndmaxsub_set_value((DtInt)(object->data));
}
