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
	int dor_scale_initialize ()
	dot_object * dor_scale_create (x,y,z)
	int dor_scale_destroy (object)
	int dor_scale_print (object)
	int dor_scale_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/geoatt.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/matrix.h>
#include <dore/dore_develop/private/scale.h>

/*
 ======================================================================
 */

DtInt doe_scale_class_id;

static DtMethodEntry scl_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_scale_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_scale_destroy },
    { DcMethodIniPick,          (DtMethodPtr) dor_scale_execute },
    { DcMethodPick,             (DtMethodPtr) dor_scale_execute },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_scale_execute },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_scale_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_scale_execute },
};

void dor_scale_initialize (void)
{
    doe_scale_class_id = dor_class_install (DcTypeScale, "DoScale", 
				7, scl_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_scale_create (
    DtReal x,
    DtReal y,
    DtReal z)
{
    static DtFlag initialized = DcFalse;
    struct scale_data *scldat;
    dot_object *scaleobject;

    if (!initialized) {
	dor_scale_initialize();
	initialized = DcTrue;
    }

    scldat = dor_space_allocate (sizeof(*scldat));

    scldat->scalevalue[0] = x;
    scldat->scalevalue[1] = y;
    scldat->scalevalue[2] = z;

    scaleobject = dor_object_create(doe_scale_class_id,scldat);

    return(scaleobject);
}
/*
 ======================================================================
 */

void dor_scale_destroy (dot_object *object)
{
    dor_space_deallocate(object->data);
}
/*
 ======================================================================
 */

void dor_scale_print (dot_object *object)
{
    struct scale_data *scale;

    scale = (struct scale_data *)object->data;

    if (scale == NULL){
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"scalevalues = (%lf,%lf,%lf)",
	    scale->scalevalue[0],scale->scalevalue[1],scale->scalevalue[2]);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_scale_execute (struct object *object)
{
    struct scale_data *scldat;
    DtMatrix4x4 newmatrix;

    if (!dor_condex_query_execute(doe_scale_class_id))
	    return;

    scldat = (struct scale_data *)object->data;

    dor_global_lcstowcsmat_get_value(newmatrix);

    dor_matrix_scale(newmatrix, scldat->scalevalue[0], scldat->scalevalue[1],
		     scldat->scalevalue[2], DcPreConcatenate);

    dor_global_lcstowcsmat_set_value(newmatrix);
}
