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
	int dor_ambint_initialize ()
	dot_object * dor_ambint_create (intensity)
	int dor_ambint_destroy (object)
	int dor_ambint_print (object)
	int dor_ambint_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct ambint_data {
    DtReal intensity;
};

/*
 ======================================================================
 */

DtInt doe_ambint_class_id;

static DtMethodEntry ambint_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_ambint_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_ambint_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_ambint_print   },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_ambint_execute },
};

void dor_ambint_initialize (void)
{
    doe_ambint_class_id = dor_class_install (
			      DcTypeAmbientIntens, "DoAmbientIntens",
			      4, ambint_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_ambint_create (DtReal intensity)
{
    static DtFlag initialized = DcFalse;
    struct ambint_data *ambintdata;
    dot_object *ambintobject;

    if (!initialized) {
	dor_ambint_initialize();
	initialized = DcTrue;
    }

    ambintdata = (struct ambint_data *)dor_space_allocate(sizeof *ambintdata);

    ambintdata->intensity = intensity;

    ambintobject = dor_object_create(doe_ambint_class_id,ambintdata);

    return(ambintobject);
}
/*
 ======================================================================
 */

void dor_ambint_destroy (
    dot_object *object)
{
    struct ambint_data *ambint;

    ambint = (struct ambint_data *)(object->data);

    dor_space_deallocate(ambint);
}
/*
 ======================================================================
 */

void dor_ambint_print (
    dot_object *object)
{
    struct ambint_data *ambint;

    ambint = (struct ambint_data *)object->data;

    if (ambint == NULL){
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"intensity = %lf",ambint->intensity);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_ambint_execute (
    dot_object *object)
{
    struct ambint_data *ambintdat;

    if (!dor_condex_query_execute(doe_ambint_class_id))
	    return;

    ambintdat = (struct ambint_data *)object->data;

    dor_global_ambint_set_value(ambintdat->intensity);
}
