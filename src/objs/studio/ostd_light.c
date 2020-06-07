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
	int dor_light_initialize ()
	dot_object * dor_light_create ()
	int dor_light_print (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/studio.h>

/*
 ======================================================================
 */

DtInt doe_light_class_id;

static DtMethodEntry methods[] = {
    { DcMethodPrint, (DtMethodPtr) dor_light_print },
};

void dor_light_initialize (void)
{
    doe_light_class_id = dor_class_install (DcTypeLight, "DoLight", 
					    1, methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_light_create (void)
{
    dot_object *lightobject;

    lightobject = dor_object_create(doe_light_class_id,DcNullPtr);

    return(lightobject);
}
/*
 ======================================================================
 */

void dor_light_print (dot_object *object)
{
}
