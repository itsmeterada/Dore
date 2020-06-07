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
	int dor_smpfilter_initialize ()
	int dor_smpfilter_initialize_filters ()
	static dor_smpfilter_box_handler ()
	dot_object * dor_smpfilter_create (filter, xwidth, ywidth)
	int dor_smpfilter_destroy (object)
	int dor_smpfilter_print (object)
	int dor_smpfilter_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>

struct smpfilter_data {
    DtObject filter;
    DtReal xwidth;
    DtReal ywidth;
};

DtInt doe_smpfilter_class_id = -1;
DtFlag doe_smpfilter_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry smpfilter_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_smpfilter_print },
    { DcMethodDestroy,         (DtMethodPtr) dor_smpfilter_destroy },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_smpfilter_execute },
};

void dor_smpfilter_initialize (void)
{
    doe_smpfilter_class_id = dor_class_add ("DoSampleFilter", 
					    3, smpfilter_methods,DcNullPtr);
}
/*
 ======================================================================
 */

DtObject DcFilterBox;

/*  Initialize the sample filter callback objects provided with standard Dore.  */

static void dor_smpfilter_box_handler (void) { return; }

void dor_smpfilter_initialize_filters (void)
{
    dot_object *datavalobj;

    datavalobj = dor_datval_create((Dt32Bits)0);

    DcFilterBox = dor_calbak_create ((DtPFI) dor_smpfilter_box_handler,
				     datavalobj, DcFalse);

    /* Create dummy reference to this object in the event that it
     * is dereferenced such that its reference count goes to zero:
     * this keeps it from being deleted entirely.
     */

    dor_object_add_reference(DcFilterBox, DcNullObject);
}

/*
 ======================================================================
 */

dot_object *dor_smpfilter_create (
    DtObject filter,
    DtReal      xwidth,
    DtReal      ywidth)
{
    struct smpfilter_data *smpfilterdata;
    dot_object *smpfilter_object;

    if (!doe_smpfilter_initialized) {
	dor_smpfilter_initialize();
	doe_smpfilter_initialized = DcTrue;
    }

#ifdef ERRCHK
    if (!dor_object_confirm_type(filter,dor_class_inq_id("DoCallback"))){
	DDerror(ERR_INVALID_CALLBACK,
		     "DoSampleFilter",DcNullPtr);
	return ((dot_object *)dor_null_create(DcNullPtr));
    }
#endif

    if (xwidth <= 0.0) {
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		"DoSampleFilter", "xwidth <= 0.0, forced to 1.0");
	xwidth = 1.0;
    }
    if (ywidth <= 0.0) {
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		"DoSampleFilter", "ywidth <= 0.0, forced to 1.0");
	ywidth = 1.0;
    }

    smpfilterdata = (struct smpfilter_data *)dor_space_allocate(sizeof *smpfilterdata);
	
    smpfilterdata->filter = filter;
    smpfilterdata->xwidth = xwidth;
    smpfilterdata->ywidth = ywidth;

    smpfilter_object = dor_object_create(doe_smpfilter_class_id,smpfilterdata);

    return(smpfilter_object);
}
/*
 ======================================================================
 */

void dor_smpfilter_destroy (dot_object *object)
{
    struct smpfilter_data *smpfilter;

    smpfilter = (struct smpfilter_data *)(object->data);

    dor_space_deallocate(smpfilter);
}
/*
 ======================================================================
 */

void dor_smpfilter_print (dot_object *object)
{
    struct smpfilter_data *smpfilter;

    smpfilter = (struct smpfilter_data *)object->data;

    if (smpfilter == NULL){
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"filter function = [%lx]",smpfilter->filter);
    dor_print_output(dor_print_get_line());
    sprintf(dor_print_get_line(),"xwidth = %lf",smpfilter->xwidth);
    dor_print_output(dor_print_get_line());
    sprintf(dor_print_get_line(),"ywidth = %lf",smpfilter->ywidth);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_smpfilter_execute (dot_object *object)
{
    struct smpfilter_data *smpfilterdat;

    if (!dor_condex_query_execute(doe_smpfilter_class_id))
	return;

    smpfilterdat = (struct smpfilter_data *)object->data;

    dor_global_smpfilter_set_value(smpfilterdat->filter, 
				   smpfilterdat->xwidth, smpfilterdat->ywidth);
}
