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
	int dor_camera_initialize ()
	int dor_camera_initialize_pick (object)
	dot_object * dor_camera_create ()
	int dor_camera_print (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/studio.h>

/*
 ======================================================================
 */

DtInt doe_camera_class_id;

static DtMethodEntry camera_methods[] = {
    { DcMethodPrint,   (DtMethodPtr) dor_camera_print },
    { DcMethodIniPick, (DtMethodPtr) dor_camera_initialize_pick },
};

void dor_camera_initialize (void)
{
    doe_camera_class_id = dor_class_install (DcTypeCamera, "DoCamera", 
					     2, camera_methods, DcNullPtr);
}
/*
 ======================================================================
 */

void dor_camera_initialize_pick (dot_object *object)
{
    dor_pick_initialize_camera(object);
}
/*
 ======================================================================
 */

dot_object *dor_camera_create (void)
{
    dot_object *cameraobject;

    cameraobject = dor_object_create(doe_camera_class_id,DcNullPtr);

    return(cameraobject);
}
/*
 ======================================================================
 */

void dor_camera_print (dot_object *object)
{
}
