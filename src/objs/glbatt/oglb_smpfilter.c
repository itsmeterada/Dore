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
	dor_global_smpfilter_initialize 
	dor_global_smpfilter_set_value 
	dor_global_smpfilter_push_value 
	dor_global_smpfilter_pop_value 
	dor_global_smpfilter_pop_group 
	dor_global_smpfilter_get_value 
	dor_global_smpfilter_get_data_pointer 
	dor_global_smpfilter_update_execute 
	dor_global_smpfilter_destroy_element 

  ======================================================================
 */
#include "global.h"

struct global_smpfilter_data {
    DtObject filter;
    DtReal xwidth;
    DtReal ywidth;
};

dot_object *doe_global_smpfilter_object;
DtInt doe_global_smpfilter_class_id = -1;
/*
 ======================================================================
 */


static DtMethodEntry methods[] = {
    { DcMethodInqGlbAttVal, (DtMethodPtr)dor_global_smpfilter_get_value }
};

void dor_global_smpfilter_initialize (void)
{
    struct gas *gas;

    doe_global_smpfilter_class_id = 
	    dor_class_add ("AttSampleFilter", 1, methods,DcNullPtr);

    gas = dor_gas_create(dor_global_smpfilter_destroy_element);

    doe_global_smpfilter_object = 
	    dor_object_create(doe_global_smpfilter_class_id,gas);

    dor_global_smpfilter_push_value(DcFilterBox, 1., 1.);
}
/*
 ======================================================================
 */

void dor_global_smpfilter_set_value (
    dot_object *filter,
    DtReal      xwidth,
    DtReal      ywidth)
{
    struct global_smpfilter_data *smpfilterdata;

    /*  If setting the value of the global attribute would
	result in an implicit push (this would occur when the
	value is the first on the current group stack), then a new
	structure is created and pushed on the stack.   */

    if (dor_gas_test_set_value(doe_global_smpfilter_object->data)){
	dor_global_smpfilter_push_value(filter, xwidth, ywidth);
    }
    else{
	smpfilterdata = dor_global_smpfilter_get_data_pointer();

	if (smpfilterdata == (struct global_smpfilter_data *)DcNullPtr){
	    return;
	}

	smpfilterdata->filter = filter;
	smpfilterdata->xwidth = xwidth;
	smpfilterdata->ywidth = ywidth;

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_smpfilter_update_execute();
    }
}
/*
 ======================================================================
 */

void dor_global_smpfilter_push_value (
    dot_object *filter,
    DtReal      xwidth,
    DtReal      ywidth)
{
    struct global_smpfilter_data *newsmpfilter_data;
    struct object *currentgroup;

    newsmpfilter_data = (struct global_smpfilter_data *) dor_space_allocate
	    (sizeof *newsmpfilter_data);

    newsmpfilter_data->filter = filter;
    newsmpfilter_data->xwidth = xwidth;
    newsmpfilter_data->ywidth = ywidth;

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_smpfilter_object->data,
			   newsmpfilter_data)){
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,
				   dor_global_smpfilter_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_smpfilter_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_smpfilter_pop_value (void)
{
    DtFlag popflag;
    struct global_smpfilter_data *smpfilterdata;

    popflag = dor_gas_pop_value(doe_global_smpfilter_object->data,
				(DtPtr)(&smpfilterdata));

    dor_global_smpfilter_destroy_element(smpfilterdata);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_smpfilter_update_execute();

    return(popflag);
}
/*
 ======================================================================
 */

/*  Pop all of the attribute values stored for this global attribute for 
    the current group.  The gas subroutine will ask the attribute object 
    to destroy any instance data being popped.  */

void dor_global_smpfilter_pop_group (void)
{
    dor_gas_pop_group(doe_global_smpfilter_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_smpfilter_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_smpfilter_get_value (
    dot_object **filter,
    DtReal      *xwidth,
    DtReal      *ywidth)
{
    DtFlag getflag;
    struct global_smpfilter_data *smpfilterdata;

    getflag = dor_gas_get_value(doe_global_smpfilter_object->data,
				(DtPtr)(&smpfilterdata));

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_smpfilter_get_value", DcNullPtr);

	return(DcFalse);
    }

    *filter = smpfilterdata->filter;
    *xwidth = smpfilterdata->xwidth;
    *ywidth = smpfilterdata->ywidth;

    return(DcTrue);
}
/*
 ======================================================================
 */

struct global_smpfilter_data *dor_global_smpfilter_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_smpfilter_object->data,&dataptr);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_smpfilter_get_data_pointer", DcNullPtr);

	return((struct global_smpfilter_data *)DcNullPtr);
    }

    return((struct global_smpfilter_data *)dataptr);
}
/*
 ======================================================================
 */

/*  The attribute (may have) changed value.  Execute the currently active 
    method (like rendering, rendering initialization, etc.).  */

void dor_global_smpfilter_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_smpfilter_object);

    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_smpfilter_destroy_element (
    struct global_smpfilter_data *smpfilterdata)
{
    dor_space_deallocate(smpfilterdata);
}
