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
	dor_global_namset_initialize 
	dor_global_namset_set_value 
	dor_global_namset_push_value 
	dor_global_namset_pop_value 
	dor_global_namset_pop_group 
	dor_global_namset_get_value 
	dor_global_namset_get_data_pointer 
	dor_global_namset_update_execute 
	dor_global_namset_destroy_element 

  ======================================================================
 */
#include "global.h"
#include <dore/internal/bitfield.h>

dot_object *doe_global_namset_object;
DtInt doe_global_namset_class_id;
/*
 ======================================================================
 */

static DtMethodEntry methods[] = {
    { 0, 0 }
};

void dor_global_namset_initialize (void)
{
    struct gas *gas;
    dot_bitfield initialnamset;
    int i;

    doe_global_namset_class_id = dor_class_install (DcTypeAttNameSet, 
		"Attribute Name Set", 0, methods, DcNullPtr);

    gas = dor_gas_create(dor_global_namset_destroy_element);

    doe_global_namset_object = dor_object_create(doe_global_namset_class_id,gas);

    for (i=0; i<dod_bitfield_ints; i++){
	initialnamset[i] = (Dt32Bits)0;
    }

    dor_global_namset_push_value(initialnamset);
}
/*
 ======================================================================
 */

void dor_global_namset_set_value (
    dot_bitfield namset)
{
    dot_bitfield *dataptr;

    /*  If setting the value of the global attribute would
	result in an implicit push (this would occur when the
	value is the first on the current group stack), then a new
	namset is created and pushed on the stack.  Otherwise, the
	current namset is loaded with the user specified namset.  */

    if (dor_gas_test_set_value(doe_global_namset_object->data)){
	dor_global_namset_push_value(namset);
    }
    else{
	dataptr = dor_global_namset_get_data_pointer();

	if (dataptr == (dot_bitfield *)DcNullPtr){
	    return;
	}

	dor_bitfield_copy(*dataptr, namset, dod_bitfield_ints);

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_namset_update_execute();
    }
}
/*
 ======================================================================
 */

void dor_global_namset_push_value (
    dot_bitfield namset)
{
    dot_bitfield *newnamset;
    struct object *currentgroup;

    /*  Create a new namset structure and copy the value to
	be pushed into it.  */

    newnamset = dor_bitfield_create();

    dor_bitfield_copy(*newnamset, namset, dod_bitfield_ints);

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_namset_object->data,newnamset)){
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(
				   currentgroup, dor_global_namset_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_namset_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_namset_pop_value (void)
{
    DtFlag popflag;
    dot_bitfield *namset;

    popflag = dor_gas_pop_value (doe_global_namset_object->data,
				 (DtPtr)(&namset));

    dor_global_namset_destroy_element(namset);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_namset_update_execute();

    return(popflag);
}
/*
 ======================================================================
 */

/*  Pop all of the attribute values stored for this global attribute for 
    the current group.  The gas subroutine will ask the attribute object 
    to destroy any instance data being popped.  */

void dor_global_namset_pop_group (void)
{
    dor_gas_pop_group(doe_global_namset_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_namset_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_namset_get_value (
    dot_bitfield namset)
{
    DtFlag getflag;
    dot_bitfield *currentnamset;

    getflag = dor_gas_get_value (doe_global_namset_object->data,
				 (DtPtr)(&currentnamset));

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_namset_get_value", DcNullPtr);

	return(DcFalse);
    }

    dor_bitfield_copy(namset, *currentnamset, dod_bitfield_ints);

    return(DcTrue);
}
/*
 ======================================================================
 */

dot_bitfield *dor_global_namset_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_namset_object->data,&dataptr);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_namset_get_data_pointer", DcNullPtr);

	return((dot_bitfield *)DcNullPtr);
    }

    return((dot_bitfield *)dataptr);
}
/*
 ======================================================================
 */

/*  The attribute (may have) changed value.  Execute the currently active 
    method (like rendering, rendering initialization, etc.).  */

void dor_global_namset_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_namset_object);

    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_namset_destroy_element (
    dot_bitfield *namset)
{
    dor_bitfield_destroy(*namset);
}
