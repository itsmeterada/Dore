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
	dor_global_texuvwidx_initialize 
	dor_global_texuvwidx_set_value 
	dor_global_texuvwidx_push_value 
	dor_global_texuvwidx_pop_value 
	dor_global_texuvwidx_pop_group 
	dor_global_texuvwidx_get_value 
	dor_global_texuvwidx_get_data_pointer 
	dor_global_texuvwidx_update_execute 
	dor_global_texuvwidx_destroy_element 

  ======================================================================
 */
#include "global.h"

dot_object *doe_global_texuvwidx_object;
DtInt doe_global_texuvwidx_class_id;
/*
 ======================================================================
 */


static DtMethodEntry methods[] = {
    { DcMethodInqGlbAttVal, (DtMethodPtr)dor_global_texuvwidx_get_value }
};

void dor_global_texuvwidx_initialize (void)
{
    struct gas *gas;

    doe_global_texuvwidx_class_id = dor_class_add ("AttTextureUVWIndex", 1,
						   methods,DcNullPtr);

    gas = dor_gas_create(dor_global_texuvwidx_destroy_element);

    doe_global_texuvwidx_object = dor_object_create(doe_global_texuvwidx_class_id,gas);

    dor_global_texuvwidx_push_value(0);
}
/*
 ======================================================================
 */

void dor_global_texuvwidx_set_value (
    DtInt index)
{
    DtInt *dataptr;

    /*  If setting the value of the global attribute would
	result in an implicit push (this would occur when the
	value is the first on the current group stack), then a new
	texture index is created and pushed on the stack.  Otherwise, the
	current texture index is loaded with the user specified index.  */

    if (dor_gas_test_set_value(doe_global_texuvwidx_object->data)) {
	dor_global_texuvwidx_push_value(index);
    } else {
	dataptr = dor_global_texuvwidx_get_data_pointer();

	if (dataptr == (DtInt *)DcNullPtr) {
	    return;
	}

	*dataptr = index;

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_texuvwidx_update_execute();
    }
}
/*
 ======================================================================
 */

void dor_global_texuvwidx_push_value (
    DtInt value)
{
    DtInt *newvalue;
    struct object *currentgroup;

    /*  Create a new color structure and copy the value to
	be pushed into it.  */

    newvalue = (DtInt *)dor_space_allocate(sizeof *newvalue);

    *newvalue = value;

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_texuvwidx_object->data,newvalue)) {
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,dor_global_texuvwidx_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_texuvwidx_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_texuvwidx_pop_value (void)
{
    DtFlag popflag;
    DtPtr value;

    popflag = dor_gas_pop_value(doe_global_texuvwidx_object->data,&value);

    dor_global_texuvwidx_destroy_element(value);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_texuvwidx_update_execute();

    return(popflag);
}
/*
 ======================================================================
 */

/*  Pop all of the attribute values stored for this global attribute for 
    the current group.  The gas subroutine will ask the attribute object 
    to destroy any instance data being popped.  */

void dor_global_texuvwidx_pop_group (void)
{
    dor_gas_pop_group(doe_global_texuvwidx_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_texuvwidx_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_texuvwidx_get_value (
    DtInt *value)
{
    DtFlag getflag;
    DtPtr currentvalue;

    getflag = dor_gas_get_value (doe_global_texuvwidx_object->data,
				 &currentvalue);

    if (!getflag) {
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_texuvwidx_get_value", DcNullPtr);

	return(DcFalse);
    }

    *value = *(DtInt*)(currentvalue);

    return(DcTrue);
}
/*
 ======================================================================
 */

DtInt *dor_global_texuvwidx_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_texuvwidx_object->data,&dataptr);

    if (!getflag) {
	DDerror(ERR_NO_CURRENT_VALUE,
		   "dor_global_texuvwidx_get_data_pointer",
		   DcNullPtr);

	return((DtInt *)DcNullPtr);
    }

    return((DtInt *)dataptr);
}
/*
 ======================================================================
 */

/*  The attribute (may have) changed value.  Execute the currently active 
    method (like rendering, rendering initialization, etc.).  */

void dor_global_texuvwidx_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_texuvwidx_object);

    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_texuvwidx_destroy_element (
    DtInt *element)
{
    dor_space_deallocate(element);
}
