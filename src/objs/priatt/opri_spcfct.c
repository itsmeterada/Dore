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
	int dor_spcfct_initialize ()
	dot_object * dor_spcfct_create (factor)
	int dor_spcfct_print (object)
	int dor_spcfct_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct spcfct_data {
    DtReal factor;
};
/*
 ======================================================================
 */

DtInt doe_spcfct_class_id;

static DtMethodEntry spcfct_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_spcfct_print },
    { DcMethodPick,             (DtMethodPtr) dor_spcfct_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_spcfct_execute },
};

void dor_spcfct_initialize (void)
{
    doe_spcfct_class_id = dor_class_install (DcTypeSpecularFactor, 
		"DoSpecularFactor", 3, spcfct_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_spcfct_create (DtReal factor)
{
    static DtFlag initialized = DcFalse;
    struct spcfct_data *spcfctdata;
    dot_object *spcfctobject;

    if (!initialized) {
	dor_spcfct_initialize();
	initialized = DcTrue;
    }

    spcfctdata = (struct spcfct_data *)dor_space_allocate(sizeof *spcfctdata);

    spcfctdata->factor = factor;

    spcfctobject = dor_object_create(doe_spcfct_class_id,spcfctdata);

    return(spcfctobject);
}
/*
 ======================================================================
 */

void dor_spcfct_print (dot_object *object)
{
    struct spcfct_data *spcfct;

    spcfct = (struct spcfct_data *)object->data;

    if (spcfct == NULL){
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"factor = %lf",spcfct->factor);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_spcfct_execute (dot_object *object)
{
    struct spcfct_data *spcfctdat;

    if (!dor_condex_query_execute(doe_spcfct_class_id))
	    return;

    spcfctdat = (struct spcfct_data *)object->data;

    dor_global_spcfct_set_value(spcfctdat->factor);
}
