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
	int dor_glbrndraylvl_initialize ()
	dot_object * dor_glbrndraylvl_create (glbrndraylvl)
	int dor_glbrndraylvl_print (object)
	int dor_glbrndraylvl_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>

/*
 ======================================================================
 */

DtInt doe_glbrndraylvl_class_id;

static DtMethodEntry glbrndraylvl_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_glbrndraylvl_print },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_glbrndraylvl_execute },
};

void dor_glbrndraylvl_initialize (void)
{
    doe_glbrndraylvl_class_id = dor_class_install (DcTypeGlbRndRayLevel, 
		"DoGlbRendRayLevel", 2, glbrndraylvl_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_glbrndraylvl_create (DtInt glbrndraylvl)
{
    static DtFlag initialized = DcFalse;
    dot_object *glbrndraylvlobject;

    if (!initialized) {
	dor_glbrndraylvl_initialize();
	initialized = DcTrue;
    }

    glbrndraylvlobject = dor_object_create(doe_glbrndraylvl_class_id,
					   (DtPtr)glbrndraylvl);

    return(glbrndraylvlobject);
}
/*
 ======================================================================
 */

void dor_glbrndraylvl_print (dot_object *object)
{
    sprintf (dor_print_get_line(),"value = %lx", object->data);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_glbrndraylvl_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_glbrndraylvl_class_id))
	    return;

    dor_global_glbrndraylvl_set_value((DtInt)(object->data));
}
