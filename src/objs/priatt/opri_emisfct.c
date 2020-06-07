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
	int dor_emisfct_initialize ()
	dot_object * dor_emisfct_create (factor)
	int dor_emisfct_print (object)
	int dor_emisfct_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct emisfct_data {
    DtReal factor;
};
/*
 ======================================================================
 */

DtInt doe_emisfct_class_id;

static DtMethodEntry emisfct_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_emisfct_print },
    { DcMethodPick,             (DtMethodPtr) dor_emisfct_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_emisfct_execute },
};

void dor_emisfct_initialize (void)
{
    doe_emisfct_class_id = dor_class_add ("DoEmissionFactor", 3,
                                          emisfct_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_emisfct_create (DtReal factor)
{
    static DtFlag initialized = DcFalse;
    struct emisfct_data *emisfctdata;
    dot_object *emisfctobject;

    if (!initialized) {
	dor_emisfct_initialize();
	initialized = DcTrue;
    }

    emisfctdata = (struct emisfct_data *)dor_space_allocate(sizeof *emisfctdata);

    emisfctdata->factor = factor;

    emisfctobject = dor_object_create(doe_emisfct_class_id,emisfctdata);

    return(emisfctobject);
}
/*
 ======================================================================
 */

void dor_emisfct_print (dot_object *object)
{
    struct emisfct_data *emisfct;

    emisfct = (struct emisfct_data *)object->data;

    if (emisfct == NULL){
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"factor = %lf",emisfct->factor);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_emisfct_execute (dot_object *object)
{
    struct emisfct_data *emisfctdat;

    if (!dor_condex_query_execute(doe_emisfct_class_id))
	    return;

    emisfctdat = (struct emisfct_data *)object->data;

    dor_global_emisfct_set_value(emisfctdat->factor);
}
