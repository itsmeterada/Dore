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
	int dor_subdivspc_initialize ()
	dot_object * dor_subdivspc_create (type,parms)
	int dor_subdivspc_destroy (object)
	int dor_subdivspc_print (object)
	int dor_subdivspc_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct subdivspc_data {
    DtInt type;
    DtReal *parms;
    DtFlag first_traversal;
};
/*
 ======================================================================
 */

DtInt doe_subdivspc_class_id;

static DtMethodEntry subdivspc_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_subdivspc_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_subdivspc_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_subdivspc_destroy },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_subdivspc_execute },
};

void dor_subdivspc_initialize (void)
{
    doe_subdivspc_class_id = dor_class_install (DcTypeSubDivSpec, "DoSubDivSpec", 4,
						subdivspc_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_subdivspc_create (
    DtInt type,
    DtReal *parms)
{
    static DtFlag initialized = DcFalse;
    dot_object *subdivspcobject;
    struct subdivspc_data *subdivspcdata;

    if (!initialized) {
	dor_subdivspc_initialize();
	initialized = DcTrue;
    }

    subdivspcdata = (struct subdivspc_data *)dor_space_allocate
	    (sizeof *subdivspcdata);

    subdivspcdata->type = type;

    subdivspcdata->parms = (DtReal *)dor_space_allocate(sizeof (DtReal));
    subdivspcdata->parms[0] = parms[0];
    subdivspcdata->first_traversal = DcTrue;

    subdivspcobject = dor_object_create(doe_subdivspc_class_id,subdivspcdata);

    return(subdivspcobject);
}
/*
 ======================================================================
 */

void dor_subdivspc_destroy (
    dot_object *object)
{
    struct subdivspc_data *subdivspc;

    subdivspc = (struct subdivspc_data *)(object->data);

    dor_space_deallocate(subdivspc->parms);

    dor_space_deallocate(subdivspc);
}
/*
 ======================================================================
 */

void dor_subdivspc_print (
    dot_object *object)
{
    struct subdivspc_data *subdivspc;

    subdivspc = (struct subdivspc_data *)object->data;

    if (subdivspc == NULL){
	dor_print_output("bad values");
	return;
    }

    switch (subdivspc->type){
    case DcSubDivFixed:
	dor_print_output("type = DcSubDivFixed");
	break;

    case DcSubDivAbsolute:
	dor_print_output("type = DcSubDivAbsolute");
	break;

    case DcSubDivRelative:
	dor_print_output("type = DcSubDivRelative");
	break;

    case DcSubDivSegments:
	dor_print_output("type = DcSubSegments");
	break;

    default:
	dor_print_output("type = UNKNOWN");
	break;
    }

    sprintf(dor_print_get_line(),"parms[0] = %lf",subdivspc->parms[0]);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_subdivspc_execute (
    dot_object *object)
{
    struct subdivspc_data *subdivspcdata;

    if (!dor_condex_query_execute(doe_subdivspc_class_id))
	    return;

    subdivspcdata = (struct subdivspc_data *)object->data;

    dor_global_subdivspc_set_value(subdivspcdata->type,subdivspcdata->parms);
}
