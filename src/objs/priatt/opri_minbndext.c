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
	int dor_minbndext_initialize ()
	struct object * dor_minbndext_create (minbndext)
	int dor_minbndext_destroy (object)
	int dor_minbndext_print (object)
	int dor_minbndext_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct minbndext_data {
    DtReal minbndext2;
};
/*
 ======================================================================
 */

DtInt doe_minbndext_class_id;

static DtMethodEntry minbndext_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_minbndext_destroy },
    { DcMethodPrint,            (DtMethodPtr) dor_minbndext_print },
    { DcMethodPick,             (DtMethodPtr) dor_minbndext_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_minbndext_execute },
};


void dor_minbndext_initialize (void)
{

    doe_minbndext_class_id = dor_class_install (DcTypeMinBoundingExtension, 
		"DoMinBoundingVolExt", 4, minbndext_methods, DcNullPtr);
}
/*
 ======================================================================
 */

struct object *dor_minbndext_create (DtReal minbndext)
{
    static DtFlag initialized = DcFalse;
    struct minbndext_data *minbndextdata;
    struct object *minbndextobject;

    if (!initialized) {
	dor_minbndext_initialize();
	initialized = DcTrue;
    }

    minbndextdata = (struct minbndext_data *)
	    dor_space_allocate(sizeof(*minbndextdata));

    minbndextdata->minbndext2 = minbndext * minbndext;

    minbndextobject = dor_object_create(
					doe_minbndext_class_id, minbndextdata);

    return(minbndextobject);
}
/*
 ======================================================================
 */

void dor_minbndext_destroy (dot_object *object)
{
    dor_space_deallocate(object->data);
}
/*
 ======================================================================
 */

void dor_minbndext_print (dot_object *object)
{
    struct minbndext_data *minbndext;

    minbndext = (struct minbndext_data *)object->data;

    if (minbndext == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"value = %f", 
	    dor_math_sqrt(minbndext->minbndext2));
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_minbndext_execute (struct object *object)
{
    struct minbndext_data *minbndextdata;

    if( ! dor_condex_query_execute(doe_minbndext_class_id))
	    return;

    minbndextdata = (struct minbndext_data *)object->data;

    dor_global_minbndext_set_value(minbndextdata->minbndext2);
}
