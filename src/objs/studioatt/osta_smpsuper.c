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
	int dor_smpsuper_initialize ()
	dot_object * dor_smpsuper_create (xsamples, ysamples)
	int dor_smpsuper_destroy (object)
	int dor_smpsuper_print (object)
	int dor_smpsuper_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>

struct smpsuper_data {
DtInt xsamples;
DtInt ysamples;
};

DtInt doe_smpsuper_class_id = -1;
DtFlag doe_smpsuper_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry smpsuper_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_smpsuper_print },
    { DcMethodDestroy,         (DtMethodPtr) dor_smpsuper_destroy },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_smpsuper_execute },
};

void dor_smpsuper_initialize (void)
{
    doe_smpsuper_class_id = dor_class_add ("DoSampleSuper", 
					   3, smpsuper_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_smpsuper_create (
    DtInt xsamples,
    DtInt ysamples)
{
    struct smpsuper_data *smpsuperdata;
    dot_object *smpsuper_object;

    if (!doe_smpsuper_initialized) {
	dor_smpsuper_initialize();
	doe_smpsuper_initialized = DcTrue;
    }

    if (xsamples < 1) {
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		   "DoSampleSuper", "xsamples < 1");
	xsamples = 1;
    }
    if (ysamples < 1) {
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		   "DoSampleSuper", "ysamples < 1");
	ysamples = 1;
    }

    smpsuperdata = (struct smpsuper_data *)dor_space_allocate(sizeof *smpsuperdata);
	
    smpsuperdata->xsamples = xsamples;
    smpsuperdata->ysamples = ysamples;

    smpsuper_object = dor_object_create(doe_smpsuper_class_id,smpsuperdata);

    return(smpsuper_object);
}
/*
 ======================================================================
 */

void dor_smpsuper_destroy (dot_object *object)
{
    struct smpsuper_data *smpsuper;

    smpsuper = (struct smpsuper_data *)(object->data);

    dor_space_deallocate(smpsuper);
}
/*
 ======================================================================
 */

void dor_smpsuper_print (dot_object *object)
{
    struct smpsuper_data *smpsuper;

    smpsuper = (struct smpsuper_data *)object->data;

    if (smpsuper == NULL){
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"xsamples = %lf",smpsuper->xsamples);
    dor_print_output(dor_print_get_line());
    sprintf(dor_print_get_line(),"ysamples = %lf",smpsuper->ysamples);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_smpsuper_execute (dot_object *object)
{
    struct smpsuper_data *smpsuperdat;

    if (!dor_condex_query_execute(doe_smpsuper_class_id))
	    return;

    smpsuperdat = (struct smpsuper_data *)object->data;

    dor_global_smpsuper_set_value(smpsuperdat->xsamples, 
				  smpsuperdat->ysamples);
}
