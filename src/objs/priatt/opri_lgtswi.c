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
	int dor_lgtswi_initialize ()
	dot_object * dor_lgtswi_create (lightobject, switchvalue)
	int dor_lgtswi_destroy (object)
	int dor_lgtswi_print (object)
	int dor_lgtswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct lgtswi_data {
    DtObject lightobject;
    DtSwitch switchvalue;
};
/*
 ======================================================================
 */

DtInt doe_lgtswi_class_id;

static DtMethodEntry lgtswi_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_lgtswi_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_lgtswi_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_lgtswi_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_lgtswi_execute },
};

void dor_lgtswi_initialize (void)
{
    doe_lgtswi_class_id = 
	    dor_class_add ("DoLightSwitch", 4, lgtswi_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_lgtswi_create (
    DtObject lightobject,
    DtSwitch switchvalue)
{
    static DtFlag initialized = DcFalse;
    struct lgtswi_data *lgtswidata;
    dot_object *lgtswi_object;

    if (!initialized) {
	dor_lgtswi_initialize();
	initialized = DcTrue;
    }

    lgtswidata = (struct lgtswi_data *)dor_space_allocate(sizeof *lgtswidata);
	
#ifdef ERRCHK
    if (!dor_object_confirm_type(lightobject,dor_class_inq_id("DoLight"))) {
	DDerror(ERR_BAD_OBJECT, "DoLightSwitch", 
		     "invalid light - returning Null object");
	return ((dot_object *)dor_null_create(DcNullPtr));
    }
#endif
    lgtswidata->lightobject = lightobject;
    lgtswidata->switchvalue = switchvalue;

    lgtswi_object = dor_object_create(doe_lgtswi_class_id,lgtswidata);

    return(lgtswi_object);
}
/*
 ======================================================================
 */

void dor_lgtswi_destroy (
    dot_object *object)
{
    struct lgtswi_data *lgtswi;

    lgtswi = (struct lgtswi_data *)(object->data);

    dor_space_deallocate(lgtswi);
}
/*
 ======================================================================
 */

void dor_lgtswi_print (
    dot_object *object)
{
    struct lgtswi_data *lgtswi;

    lgtswi = (struct lgtswi_data *)object->data;

    if (lgtswi == NULL){
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"light at [%lx]",lgtswi->lightobject);
    dor_print_output(dor_print_get_line());

    switch(lgtswi->switchvalue) {
    case DcOff:
	dor_print_output("switchvalue = DcOff");
	break;
    case DcOn:
	dor_print_output("switchvalue = DcOn");
	break;
    }
}
/*
 ======================================================================
 */

void dor_lgtswi_execute (
    dot_object *object)
{
    struct lgtswi_data *lgtswidat;

    if (!dor_condex_query_execute(doe_lgtswi_class_id))
	    return;

    lgtswidat = (struct lgtswi_data *)object->data;

    dor_global_lgtswi_set_value(lgtswidat->lightobject, 
				lgtswidat->switchvalue);
}
