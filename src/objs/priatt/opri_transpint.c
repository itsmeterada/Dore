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
	int dor_transpint_initialize ()
	dot_object * dor_transpint_create (intensity)
	int dor_transpint_destroy (object)
	int dor_transpint_print (object)
	int dor_transpint_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct transpint_data {
    DtReal intensity;
};
/*
 ======================================================================
 */

DtInt doe_transpint_class_id;

static DtMethodEntry transpint_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_transpint_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_transpint_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_transpint_destroy },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_transpint_execute },
};

void dor_transpint_initialize (void)
{
    doe_transpint_class_id = dor_class_install (DcTypeTranspIntens, 
		"DoTranspIntens", 4, transpint_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_transpint_create (DtReal intensity)
{
    static DtFlag initialized = DcFalse;
    struct transpint_data *transpintdata;
    dot_object *transpintobject;

    if (!initialized) {
	dor_transpint_initialize();
	initialized = DcTrue;
    }

    transpintdata = (struct transpint_data *)dor_space_allocate(sizeof *transpintdata);

    transpintdata->intensity = intensity;

    transpintobject = dor_object_create(doe_transpint_class_id,transpintdata);

    return(transpintobject);
}
/*
 ======================================================================
 */

void dor_transpint_destroy (dot_object *object)
{
    struct transpint_data *transpint;

    transpint = (struct transpint_data *)(object->data);

    dor_space_deallocate(transpint);
}
/*
 ======================================================================
 */

void dor_transpint_print (dot_object *object)
{
    struct transpint_data *transpint;

    transpint = (struct transpint_data *)object->data;

    if (transpint == NULL){
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"intensity = %lf",transpint->intensity);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_transpint_execute (dot_object *object)
{
    struct transpint_data *transpintdat;

    if (!dor_condex_query_execute(doe_transpint_class_id))
	    return;

    transpintdat = (struct transpint_data *)object->data;

    dor_global_transpint_set_value(transpintdat->intensity);
}
