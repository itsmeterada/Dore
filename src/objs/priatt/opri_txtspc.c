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
	int dor_txtspc_initialize ()
	struct object * dor_txtspc_create (txtspc)
	int dor_txtspc_print (object)
	int dor_txtspc_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct txtspc_data {
    DtReal txtspc;
};
/*
 ======================================================================
 */

DtInt doe_txtspc_class_id;

static DtMethodEntry txtspc_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_txtspc_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_txtspc_print },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_txtspc_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_txtspc_execute },
};

void dor_txtspc_initialize (void)
{
    doe_txtspc_class_id = dor_class_install (DcTypeTextSpace, "DoTextSpace", 4,
					     txtspc_methods,DcNullPtr);
}
/*
 ======================================================================
 */

struct object *dor_txtspc_create (DtReal txtspc)
{
    static DtFlag initialized = DcFalse;
    struct txtspc_data *txtspcdata;
    struct object *txtspcobject;

    if (!initialized) {
	dor_txtspc_initialize();
	initialized = DcTrue;
    }

    txtspcdata = (struct txtspc_data *)dor_space_allocate(sizeof *txtspcdata);

    txtspcdata->txtspc = txtspc;

    txtspcobject = dor_object_create(doe_txtspc_class_id,txtspcdata);

    return(txtspcobject);
}
/*
 ======================================================================
 */

void dor_txtspc_print (dot_object *object)
{
    struct txtspc_data *txtspcdata;

    txtspcdata = (struct txtspc_data *)object->data;

    if (txtspcdata == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"value = %f",txtspcdata->txtspc);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_txtspc_execute (struct object *object)
{
    struct txtspc_data *txtspcdata;

    if( ! dor_condex_query_execute(doe_txtspc_class_id))
	    return;

    txtspcdata = (struct txtspc_data *)object->data;

    dor_global_txtspc_set_value(txtspcdata->txtspc);
}
