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
	int dor_spcint_initialize ()
	dot_object * dor_spcint_create (intensity)
	int dor_spcint_destroy (object)
	int dor_spcint_print (object)
	int dor_spcint_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct spcint_data {
    DtReal intensity;
};
/*
 ======================================================================
 */

DtInt doe_spcint_class_id;

static DtMethodEntry spcint_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_spcint_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_spcint_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_spcint_destroy },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_spcint_execute },
};

void dor_spcint_initialize (void)
{
    doe_spcint_class_id = dor_class_install (DcTypeSpecularIntens, 
		"DoSpecularIntens", 4, spcint_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_spcint_create (DtReal intensity)
{
    static DtFlag initialized = DcFalse;
    struct spcint_data *spcintdata;
    dot_object *spcintobject;

    if (!initialized) {
	dor_spcint_initialize();
	initialized = DcTrue;
    }

    spcintdata = (struct spcint_data *)dor_space_allocate(sizeof *spcintdata);

    spcintdata->intensity = intensity;

    spcintobject = dor_object_create(doe_spcint_class_id,spcintdata);

    return(spcintobject);
}
/*
 ======================================================================
 */

void dor_spcint_destroy (dot_object *object)
{
    struct spcint *spcint;

    spcint = (struct spcint *)(object->data);

    dor_space_deallocate(spcint);
}
/*
 ======================================================================
 */

void dor_spcint_print (dot_object *object)
{
    struct spcint_data *spcint;

    spcint = (struct spcint_data *)object->data;

    if (spcint == NULL){
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"intensity = %lf",spcint->intensity);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_spcint_execute (dot_object *object)
{
    struct spcint_data *spcintdat;

    if (!dor_condex_query_execute(doe_spcint_class_id))
	    return;

    spcintdat = (struct spcint_data *)object->data;

    dor_global_spcint_set_value(spcintdat->intensity);
}
