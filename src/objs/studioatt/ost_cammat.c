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
	int dor_cammat_initialize ()
	int dor_cammat_initialize_pick (object)
	dot_object * dor_cammat_create (matrix)
	int dor_cammat_destroy (object)
	int dor_cammat_print (object)
	dor_cammat_execute dor_cammat_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>
#include <dore/internal/matrix.h>
#include <dore/dore_develop/private/cammat.h>

/*
 ======================================================================
 */

DtInt doe_cammat_class_id;

static DtMethodEntry cammat_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_cammat_print },
    { DcMethodDestroy,         (DtMethodPtr) dor_cammat_destroy },
    { DcMethodIniPick,         (DtMethodPtr) dor_cammat_initialize_pick },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_cammat_execute },
};

void dor_cammat_initialize (void)
{
    doe_cammat_class_id = dor_class_install (DcTypeCameraMatrix, 
			"DoCameraMatrix", 4, cammat_methods,DcNullPtr);
}
/*
 ======================================================================
 */

void dor_cammat_initialize_pick (dot_object *object)
{
    struct cammat_data *cammatdat;

    cammatdat = (struct cammat_data *)object->data;

    dor_pick_set_camera_matrix(cammatdat->matrix);
}
/*
 ======================================================================
 */

dot_object *dor_cammat_create (DtMatrix4x4 matrix)
{
    static DtFlag initialized = DcFalse;
    struct cammat_data *cammatdat;
    dot_object *cammatobj;

    if (!initialized) {
	dor_cammat_initialize();
	initialized = DcTrue;
    }

    cammatdat = (struct cammat_data *)dor_space_allocate(sizeof *cammatdat);

    dor_matrix_load(cammatdat->matrix,matrix);

    cammatobj = dor_object_create(doe_cammat_class_id,cammatdat);

    return(cammatobj);
}
/*
 ======================================================================
 */

void dor_cammat_destroy (dot_object *object)
{
    dor_space_deallocate(object->data);
}
/*
 ======================================================================
 */

void dor_cammat_print (dot_object *object)
{
    struct cammat_data *cammatdat;

    cammatdat = (struct cammat_data *)object->data;

    dor_matrix_print(cammatdat->matrix);
}
/*
 ======================================================================
 */

void dor_cammat_execute  (dot_object *object)
{
    struct cammat_data *cammatdat;

    if (!dor_condex_query_execute(doe_cammat_class_id))
	    return;

    cammatdat = (struct cammat_data *)object->data;

    dor_global_cammat_set_value(DcCameraArbitrary,
				cammatdat->matrix);
}
