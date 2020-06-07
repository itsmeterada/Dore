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
	int dor_parallel_initialize ()
	int dor_parallel_initialize_pick (object)
	dot_object * dor_parallel_create (fovdegree,hither,yon)
	int dor_parallel_destroy (object)
	int dor_parallel_print (object)
	dor_parallel_execute dor_parallel_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>
#include <dore/internal/matrix.h>
#include <dore/dore_develop/private/parmat.h>

/*
 ======================================================================
 */

DtInt doe_parallel_class_id;

static DtMethodEntry parallel_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_parallel_print },
    { DcMethodDestroy,         (DtMethodPtr) dor_parallel_destroy },
    { DcMethodIniPick,         (DtMethodPtr) dor_parallel_initialize_pick },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_parallel_execute },
};

void dor_parallel_initialize (void)
{
    doe_parallel_class_id = dor_class_install (DcTypeParallel, "DoParallel", 4,
					       parallel_methods,DcNullPtr);
}
/*
 ======================================================================
 */

void dor_parallel_initialize_pick (dot_object *object)
{
    struct parallel_data *paralleldat;

    paralleldat = (struct parallel_data *)object->data;

    dor_pick_set_parallel_matrix(paralleldat->fov_degree, paralleldat->hither,
				 paralleldat->yon);
}
/*
 ======================================================================
 */

dot_object *dor_parallel_create (
    DtReal fovdegree,
    DtReal hither,
    DtReal yon)
{
    static DtFlag initialized = DcFalse;
    struct parallel_data *paralleldat;
    dot_object *parallelobj;

    if (!initialized) {
	dor_parallel_initialize();
	initialized = DcTrue;
    }

    if (fovdegree <= 0.) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "DoParallel",
		    "window size <= 0. - returning Null object");
	return (dot_object *)dor_null_create(DcNullPtr);
    }

    if (hither <= yon) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "DoParallel",
		    "hither <= yon - returning Null object");
	return (dot_object *)dor_null_create(DcNullPtr);
    }

    paralleldat = (struct parallel_data *)dor_space_allocate(sizeof *paralleldat);

    paralleldat->fov_degree = fovdegree;
    paralleldat->hither = hither;
    paralleldat->yon = yon;

    parallelobj = dor_object_create(doe_parallel_class_id,paralleldat);

    return(parallelobj);
}
/*
 ======================================================================
 */

void dor_parallel_destroy (dot_object *object)
{
    dor_space_deallocate(object->data);
}
/*
 ======================================================================
 */

void dor_parallel_print (dot_object *object)
{
    struct parallel_data *paralleldat;

    paralleldat = (struct parallel_data *)object->data;

    if (paralleldat == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),
	    "fov = %f, hither = %f, yon = %f",
	    paralleldat->fov_degree,paralleldat->hither,paralleldat->yon);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_parallel_execute  (dot_object *object)
{
    struct parallel_data *paralleldat;
    DtMatrix4x4 matrix;

    if (!dor_condex_query_execute(doe_parallel_class_id))
	    return;

    paralleldat = (struct parallel_data *)object->data;

    dor_matrix_load_parallel (matrix, paralleldat->fov_degree,
			      paralleldat->hither,
			      paralleldat->yon);

    dor_global_cammat_set_value(DcCameraParallel, matrix);
}
