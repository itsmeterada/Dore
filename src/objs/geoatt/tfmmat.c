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
	int dor_tfmmat_initialize ()
	dot_object * dor_tfmmat_create (tfmmat, comptype)
	int dor_tfmmat_destroy (object)
	int dor_tfmmat_print (object)
	int dor_tfmmat_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/geoatt.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/matrix.h>
#include <dore/dore_develop/private/tfmmat.h>

/*
 ======================================================================
 */

DtInt doe_tfmmat_class_id;

static DtMethodEntry tfmmat_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_tfmmat_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_tfmmat_destroy },
    { DcMethodIniPick,          (DtMethodPtr) dor_tfmmat_execute },
    { DcMethodPick,             (DtMethodPtr) dor_tfmmat_execute },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_tfmmat_execute },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_tfmmat_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_tfmmat_execute },
};

void dor_tfmmat_initialize (void)
{
    doe_tfmmat_class_id = dor_class_install (DcTypeTransformMatrix, 
		"DoTransformMatrix", 7, tfmmat_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_tfmmat_create (
    DtMatrix4x4 tfmmat,
    DtCompType comptype)
{
    static DtFlag initialized = DcFalse;
    struct tfmmat_data *tfmmatdat;
    dot_object *tfmmatobject;

    if (!initialized) {
	dor_tfmmat_initialize();
	initialized = DcTrue;
    }

    tfmmatdat = dor_space_allocate (sizeof(*tfmmatdat));

    dor_matrix_load(tfmmatdat->tfmmat, tfmmat);
    tfmmatdat->comptype = comptype;

    tfmmatobject = dor_object_create(doe_tfmmat_class_id,tfmmatdat);

    return(tfmmatobject);
}
/*
 ======================================================================
 */

void dor_tfmmat_destroy (
    dot_object *object)
{
    dor_space_deallocate(object->data);
}
/*
 ======================================================================
 */

void dor_tfmmat_print (
    dot_object *object)
{
    struct tfmmat_data *tfmmatdata;

    tfmmatdata = (struct tfmmat_data *)object->data;

    if (tfmmatdata == NULL) {
	dor_print_output("bad values");
	return;
    }

    switch(tfmmatdata->comptype) {
    case DcPreConcatenate:
	dor_print_output("comptype = DcPreConcatenate");
	break;
    case DcPostConcatenate:
	dor_print_output("comptype = DcPostConcatenate");
	break;
    case DcReplace:
	dor_print_output("comptype = DcReplace");
	break;
    }
    dor_matrix_print (tfmmatdata->tfmmat);
}
/*
 ======================================================================
 */

void dor_tfmmat_execute (
    struct object *object)
{
    struct tfmmat_data *tfmmatdat;
    DtMatrix4x4 newmatrix;

    if (!dor_condex_query_execute(doe_tfmmat_class_id))
	    return;

    tfmmatdat = (struct tfmmat_data *)object->data;
    dor_global_lcstowcsmat_get_value(newmatrix);
    dor_matrix_concat(newmatrix, tfmmatdat->tfmmat, tfmmatdat->comptype);
    dor_global_lcstowcsmat_set_value(newmatrix);
}
