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
	int dor_txthgt_initialize ()
	struct object * dor_txthgt_create (txthgt)
	int dor_txthgt_destroy (object)
	int dor_txthgt_print (object)
	int dor_txthgt_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct txthgt_data {
    DtReal txthgt;
};
/*
 ======================================================================
 */

DtInt doe_txthgt_class_id;

static DtMethodEntry txthgt_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_txthgt_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_txthgt_print },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_txthgt_execute },
    { DcMethodDestroy,          (DtMethodPtr) dor_txthgt_destroy },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_txthgt_execute },
};

void dor_txthgt_initialize (void)
{
    doe_txthgt_class_id = dor_class_install (DcTypeTextHeight, "DoTextHeight", 5,
					     txthgt_methods,DcNullPtr);
}
/*
 ======================================================================
 */

struct object *dor_txthgt_create (DtReal txthgt)
{
    static DtFlag initialized = DcFalse;
    struct txthgt_data *txthgtdata;
    struct object *txthgtobject;

    if (!initialized) {
	dor_txthgt_initialize();
	initialized = DcTrue;
    }

    txthgtdata = (struct txthgt_data *)
	    dor_space_allocate(sizeof *txthgtdata);

    txthgtdata->txthgt = txthgt;

    txthgtobject = dor_object_create(doe_txthgt_class_id,txthgtdata);

    return(txthgtobject);
}
/*
 ======================================================================
 */

void dor_txthgt_destroy (dot_object *object)
{
    struct txthgt_data *txthgt;

    txthgt = (struct txthgt_data *)(object->data);

    dor_space_deallocate(txthgt);
}
/*
 ======================================================================
 */

void dor_txthgt_print (dot_object *object)
{
    struct txthgt_data *txthgtdata;

    txthgtdata = (struct txthgt_data *)object->data;

    if (txthgtdata == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"value = %f",txthgtdata->txthgt);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_txthgt_execute (struct object *object)
{
    struct txthgt_data *txthgtdata;

    if( ! dor_condex_query_execute(doe_txthgt_class_id))
	    return;

    txthgtdata = (struct txthgt_data *)object->data;

    dor_global_txthgt_set_value(txthgtdata->txthgt);
}
