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
	int dor_transl_initialize ()
	dot_object * dor_transl_create (x,y,z)
	int dor_transl_destroy (object)
	int dor_transl_print (object)
	int dor_transl_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/geoatt.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/matrix.h>
#include <dore/dore_develop/private/transl.h>

/*
 ======================================================================
 */

DtInt doe_transl_class_id;

static DtMethodEntry transl_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_transl_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_transl_destroy },
    { DcMethodIniPick,          (DtMethodPtr) dor_transl_execute },
    { DcMethodPick,             (DtMethodPtr) dor_transl_execute },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_transl_execute },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_transl_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_transl_execute },
};

void dor_transl_initialize (void)
{
    doe_transl_class_id = dor_class_install (DcTypeTranslate, "DoTranslate", 7,
					     transl_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_transl_create (
    DtReal x,
    DtReal y,
    DtReal z)
{
    static DtFlag initialized = DcFalse;
    dot_object *translobj;
    struct transl_data *transldat;

    if (!initialized) {
	dor_transl_initialize();
	initialized = DcTrue;
    }

    transldat = dor_space_allocate(sizeof(*transldat));

    transldat->delta[0] = x;
    transldat->delta[1] = y;
    transldat->delta[2] = z;

    translobj = dor_object_create(doe_transl_class_id,transldat);

    return(translobj);
}
/*
 ======================================================================
 */

void dor_transl_destroy (dot_object *object)
{
    dor_space_deallocate(object->data);
}
/*
 ======================================================================
 */

void dor_transl_print (dot_object *object)
{
    struct transl_data *transl;

    transl = (struct transl_data *)object->data;

    if (transl == NULL){
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"translvalues = (%lf,%lf,%lf)",
	    transl->delta[0],transl->delta[1],transl->delta[2]);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_transl_execute (dot_object *object)
{
    struct transl_data *transldat;
    DtMatrix4x4 newmatrix;

    if (!dor_condex_query_execute(doe_transl_class_id))
	    return;

    transldat = (struct transl_data *)object->data;

    dor_global_lcstowcsmat_get_value(newmatrix);

    dor_matrix_translate(newmatrix,
			 transldat->delta[0],transldat->delta[1],
			 transldat->delta[2], DcPreConcatenate);

    dor_global_lcstowcsmat_set_value(newmatrix);
}
