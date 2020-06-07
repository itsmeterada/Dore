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
	int dor_difint_initialize ()
	dot_object * dor_difint_create (intensity)
	int dor_difint_destroy (object)
	int dor_difint_print (object)
	int dor_difint_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct difint_data {
    DtReal intensity;
};
/*
 ======================================================================
 */

DtInt doe_difint_class_id;

static DtMethodEntry difint_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_difint_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_difint_print   },
    { DcMethodDestroy,          (DtMethodPtr) dor_difint_destroy },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_difint_execute },
};

void dor_difint_initialize (void)
{
    doe_difint_class_id = dor_class_install (DcTypeDiffuseIntens, "DoDiffuseIntens", 
		4, difint_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_difint_create (DtReal intensity)
{
    static DtFlag initialized = DcFalse;
    struct difint_data *difintdata;
    dot_object *difintobject;

    if (!initialized) {
	dor_difint_initialize();
	initialized = DcTrue;
    }

    difintdata = (struct difint_data *)dor_space_allocate(sizeof *difintdata);

    difintdata->intensity = intensity;

    difintobject = dor_object_create(doe_difint_class_id,difintdata);

    return(difintobject);
}
/*
 ======================================================================
 */

void dor_difint_destroy (dot_object *object)
{
    struct difint_data *difint;

    difint = (struct difint_data *)(object->data);

    dor_space_deallocate(difint);
}
/*
 ======================================================================
 */

void dor_difint_print (dot_object *object)
{
    struct difint_data *difint;

    difint = (struct difint_data *)object->data;

    if (difint == NULL){
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"intensity = %lf",difint->intensity);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_difint_execute (dot_object *object)
{
    struct difint_data *difintdat;

    if (!dor_condex_query_execute(doe_difint_class_id))
	    return;

    difintdat = (struct difint_data *)object->data;

    dor_global_difint_set_value(difintdat->intensity);
}
