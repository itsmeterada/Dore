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
	dor_global_difclr_initialize 
	dor_global_difclr_set_value 
	dor_global_difclr_push_value 
	dor_global_difclr_pop_value 
	dor_global_difclr_pop_group 
	dor_global_difclr_get_value 
	dor_global_difclr_get_data_pointer 
	dor_global_difclr_update_execute 
	dor_global_difclr_destroy_element 

  ======================================================================
 */
#include "global.h"

dot_object *doe_global_difclr_object;
DtInt doe_global_difclr_class_id;
/*
 ======================================================================
 */

static DtMethodEntry methods[] = {
    { DcMethodInqGlbAttVal, (DtMethodPtr) dor_global_difclr_get_value }
};

void dor_global_difclr_initialize (void)
{
    struct gas *gas;
    DtColorRGB initialcolor;

    doe_global_difclr_class_id = dor_class_install (DcTypeAttDiffuseColor, 
		"AttDiffuseColor", 1, methods,DcNullPtr);

    gas = dor_gas_create(dor_global_difclr_destroy_element);

    doe_global_difclr_object = dor_object_create(doe_global_difclr_class_id,gas);

    initialcolor[0] = 1.;
    initialcolor[1] = 1.;
    initialcolor[2] = 1.;

    dor_global_difclr_push_value(initialcolor);
}
/*
 ======================================================================
 */

void dor_global_difclr_set_value (
    DtColorRGB color)
{
    DtColorRGB *dataptr;

    /*  If setting the value of the global attribute would result in 
	an implicit push (this would occur when the value is the first 
	on the current group stack), then a new diffuse colour is created 
	and pushed on the stack.  Otherwise, the current diffuse colour
	 is loaded with the user specified diffuse colour.  */

    if (dor_gas_test_set_value(doe_global_difclr_object->data)){
	dor_global_difclr_push_value(color);
    }
    else{
	dataptr = dor_global_difclr_get_data_pointer();

	if (dataptr == (DtColorRGB *)DcNullPtr){
	    return;
	}

	(*dataptr)[0] = color[0];
	(*dataptr)[1] = color[1];
	(*dataptr)[2] = color[2];

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_difclr_update_execute();
    }
}
/*
 ======================================================================
 */

void dor_global_difclr_push_value (
    DtColorRGB color)
{
    DtColorRGB *newcolor;
    struct object *currentgroup;

    /*  Create a new color structure and copy the value to
	be pushed into it.  */

    newcolor = (DtColorRGB *)dor_space_allocate(sizeof *newcolor);

    (*newcolor)[0] = color[0];
    (*newcolor)[1] = color[1];
    (*newcolor)[2] = color[2];

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_difclr_object->data,newcolor)){
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,dor_global_difclr_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_difclr_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_difclr_pop_value (void)
{
    DtFlag popflag;
    DtPtr color;

    popflag = dor_gas_pop_value(doe_global_difclr_object->data,&color);

    dor_global_difclr_destroy_element(color);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_difclr_update_execute();

    return(popflag);
}
/*
 ======================================================================
 */

/* Pop all of the attribute values stored for this global attribute for 
   the current group.  The gas subroutine will ask the attribute object 
   to destroy any instance data being popped. */

void dor_global_difclr_pop_group (void)
{
    dor_gas_pop_group(doe_global_difclr_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_difclr_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_difclr_get_value (
    DtColorModel *colormodel,
    DtReal color[3])
{
    DtFlag      getflag;
    DtPtr       dataptr;
    DtColorRGB *currentcolor;

    getflag = dor_gas_get_value (doe_global_difclr_object->data, &dataptr);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE, "dor_global_difclr_get_value",DcNullPtr);
	return DcFalse;
    }
    currentcolor = dataptr;

    *colormodel = DcRGB;
    color[0] = (*currentcolor)[0];
    color[1] = (*currentcolor)[1];
    color[2] = (*currentcolor)[2];

    return(DcTrue);
}
/*
 ======================================================================
 */

DtColorRGB *dor_global_difclr_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_difclr_object->data,&dataptr);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_difclr_get_data_pointer", DcNullPtr);

	return((DtColorRGB *)DcNullPtr);
    }

    return((DtColorRGB *)dataptr);
}
/*
 ======================================================================
 */

/* The attribute (may have) changed value.  Execute the currently active 
   method (like rendering, rendering initialization, etc.).  */

void dor_global_difclr_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_difclr_object);

    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_difclr_destroy_element (
    DtColorRGB *color)
{
    dor_space_deallocate(color);
}
