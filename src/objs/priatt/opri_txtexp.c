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
	int dor_txtexp_initialize ()
	struct object * dor_txtexp_create (txtexp)
	int dor_txtexp_destroy (object)
	int dor_txtexp_print (object)
	int dor_txtexp_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct txtexp_data {
    DtReal txtexp;
};
/*
 ======================================================================
 */

DtInt doe_txtexp_class_id;

static DtMethodEntry txtexp_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_txtexp_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_txtexp_print },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_txtexp_execute },
    { DcMethodDestroy,          (DtMethodPtr) dor_txtexp_destroy },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_txtexp_execute },
};

void dor_txtexp_initialize (void)
{
    doe_txtexp_class_id = dor_class_install (DcTypeTextExpansionFactor, 
		"DoTextExpFactor", 5, txtexp_methods, DcNullPtr);
}
/*
 ======================================================================
 */

struct object *dor_txtexp_create (DtReal txtexp)
{
    static DtFlag initialized = DcFalse;
    struct txtexp_data *txtexpdata;
    struct object *txtexpobject;

    if (!initialized) {
	dor_txtexp_initialize();
	initialized = DcTrue;
    }

    txtexpdata = (struct txtexp_data *)dor_space_allocate(sizeof *txtexpdata);

    if (txtexp <= 0.0)
	DDerror (ERR_VALUE_OUT_OF_RANGE, "DoTextExpFactor",
		"expansion factor <= 0.0 -- expansion results are undefined");

    txtexpdata->txtexp = txtexp;

    txtexpobject = dor_object_create(doe_txtexp_class_id,txtexpdata);

    return(txtexpobject);
}
/*
 ======================================================================
 */

void dor_txtexp_destroy (dot_object *object)
{
    struct txtexp_data *txtexp;

    txtexp = (struct txtexp_data *)(object->data);

    dor_space_deallocate(txtexp);
}
/*
 ======================================================================
 */

void dor_txtexp_print (dot_object *object)
{
    struct txtexp_data *txtexpdata;

    txtexpdata = (struct txtexp_data *)object->data;

    if (txtexpdata == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"value = %f",txtexpdata->txtexp);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_txtexp_execute (struct object *object)
{
    struct txtexp_data *txtexpdata;

    if( ! dor_condex_query_execute(doe_txtexp_class_id))
	    return;

    txtexpdata = (struct txtexp_data *)object->data;

    dor_global_txtexp_set_value(txtexpdata->txtexp);
}
