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
	int dor_shear_initialize ()
	dot_object * dor_shear_create (plane,firstdirectionshearvalue, seconddirectionshearvalue)
	int dor_shear_destroy (object)
	int dor_shear_print (object)
	int dor_shear_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/geoatt.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/matrix.h>
#include <dore/dore_develop/private/shear.h>

/*
 ======================================================================
 */

DtInt doe_shear_class_id;

static DtMethodEntry shr_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_shear_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_shear_destroy },
    { DcMethodIniPick,          (DtMethodPtr) dor_shear_execute },
    { DcMethodPick,             (DtMethodPtr) dor_shear_execute },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_shear_execute },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_shear_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_shear_execute },
};

void dor_shear_initialize (void)
{
    doe_shear_class_id = dor_class_install (DcTypeShear, "DoShear", 7,
				shr_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_shear_create (
    DtMajorPlane plane,
    DtReal       firstdirectionshearvalue,
    DtReal       seconddirectionshearvalue)
{
    static DtFlag initialized = DcFalse;
    struct shear_data *shrdat;
    dot_object *shearobject;

    if (!initialized) {
	dor_shear_initialize();
	initialized = DcTrue;
    }

    shrdat = dor_space_allocate(sizeof(*shrdat));

    shrdat->plane = plane;
    shrdat->firstdirectionshearvalue = firstdirectionshearvalue;
    shrdat->seconddirectionshearvalue = seconddirectionshearvalue;

    shearobject = dor_object_create(doe_shear_class_id,shrdat);
    return(shearobject);
}
/*
 ======================================================================
 */

void dor_shear_destroy (dot_object *object)
{
    dor_space_deallocate(object->data);
}
/*
 ======================================================================
 */

void dor_shear_print (dot_object *object)
{
    struct shear_data *shrdata;

    shrdata = (struct shear_data *)object->data;

    switch(shrdata->plane) {
    case DcXY:
	dor_print_output("plane = DcXY");
	break;

    case DcYZ:
	dor_print_output("plane = DcYZ");
	break;

    case DcXZ:
	dor_print_output("plane = DcXZ");
	break;
    }

    sprintf(dor_print_get_line(),
	    "firstdirectionshearvalue = %f",
	    shrdata->firstdirectionshearvalue);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "seconddirectionshearvalue = %f",
	    shrdata->seconddirectionshearvalue);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_shear_execute (struct object *object)
{
    struct shear_data *shrdat;
    DtMatrix4x4 newmatrix;

    if (!dor_condex_query_execute(doe_shear_class_id))
	    return;

    shrdat = (struct shear_data *)object->data;

    dor_global_lcstowcsmat_get_value(newmatrix);

    dor_matrix_shear(newmatrix,
		     shrdat->plane,
		     shrdat->firstdirectionshearvalue,
		     shrdat->seconddirectionshearvalue,
		     DcPreConcatenate);

    dor_global_lcstowcsmat_set_value(newmatrix);
}
