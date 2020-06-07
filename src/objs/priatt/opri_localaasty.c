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
	int dor_localaasty_initialize ()
	dot_object * dor_localaasty_create (style)
	int dor_localaasty_destroy (object)
	int dor_localaasty_print (object)
	int dor_localaasty_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct localaasty_data {
    DtLocalAntiAliasStyle style;
};
 
DtInt doe_localaasty_class_id = -1;
DtFlag doe_localaasty_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry localaasty_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_localaasty_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_localaasty_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_localaasty_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_localaasty_execute },
};

void dor_localaasty_initialize (void)
{
    doe_localaasty_class_id = dor_class_add ("DoLocalAntiAliasStyle", 4,
					     localaasty_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_localaasty_create (
    DtLocalAntiAliasStyle style)
{
    struct localaasty_data *localaastydata;

    if (!doe_localaasty_initialized) {
	dor_localaasty_initialize();
	doe_localaasty_initialized = DcTrue;
    }

    localaastydata = (struct localaasty_data *)dor_space_allocate(sizeof *localaastydata);

    localaastydata->style = style;

    return (dor_object_create(doe_localaasty_class_id,localaastydata));
}
/*
 ======================================================================
 */

void dor_localaasty_destroy (
    dot_object *object)
{
    struct localaasty_data *localaasty;

    localaasty = (struct localaasty_data *)(object->data);

    dor_space_deallocate(localaasty);
}
/*
 ======================================================================
 */

void dor_localaasty_print (
    dot_object *object)
{
    struct localaasty_data *localaasty;

    localaasty = (struct localaasty_data *)object->data;

    if (localaasty == NULL) {
	dor_print_output("bad values");
	return;
    }

    switch(localaasty->style) {
    case DcLocalAntiAliasFast:
	dor_print_output ("mode = DcLocalAntiAliasFast");
	break;
    case DcLocalAntiAliasIntermediate:
	dor_print_output ("mode = DcLocalAntiAliasIntermediate");
	break;
    case DcLocalAntiAliasBest:
	dor_print_output ("mode = DcLocalAntiAliasBest");
	break;
    default:
	sprintf(dor_print_get_line(),"mode = <UNKNOWN TYPE> %d",localaasty->style);
	dor_print_output(dor_print_get_line());
	break;
    }
}
/*
 ======================================================================
 */

void dor_localaasty_execute (
    dot_object *object)
{
    struct localaasty_data *localaastydat;

    if (!dor_condex_query_execute(doe_localaasty_class_id))
	    return;

    localaastydat = (struct localaasty_data *)object->data;

    dor_global_localaasty_set_value(localaastydat->style);
}
