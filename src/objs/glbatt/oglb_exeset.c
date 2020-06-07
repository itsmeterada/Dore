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
	dor_global_exeset_initialize 
	dor_global_exeset_set_value 
	dor_global_exeset_push_value 
	dor_global_exeset_pop_value 
	dor_global_exeset_pop_group 
	dor_global_exeset_get_value 
	dor_global_exeset_get_data_pointer 
	dor_global_exeset_update_execute 
	dor_global_exeset_destroy_element 
	dor_global_exeset_obj_types_report 

  ======================================================================
 */
#include "global.h"
#include <dore/internal/exeset.h>

dot_object *doe_global_exeset_object;
dot_exeset *doe_global_exeset_base_exeset;
DtInt doe_global_exeset_class_id = -1;
/*
 ======================================================================
 */

static DtMethodEntry methods[] = {
    { 0, 0 }
};

void dor_global_exeset_initialize (void)
{
    struct gas *gas;
    dot_exeset *base_exeset;
    int i;

    doe_global_exeset_class_id = dor_class_install (DcTypeAttExecSet, 
		"Attribute Execset", 0, methods, DcNullPtr);

    gas = dor_gas_create(dor_global_exeset_destroy_element);

    doe_global_exeset_object = dor_object_create(doe_global_exeset_class_id,gas);

    base_exeset = dor_exeset_create_exeset(dor_object_inq_ntypes());

    for(i=0; i<(base_exeset->ints); i++){
	base_exeset->bitset[i] = ~0;
    }

    doe_global_exeset_base_exeset = dor_global_exeset_push_value(base_exeset);
}
/*
 ======================================================================
 */

void dor_global_exeset_set_value (dot_exeset *exeset)
{
    dot_exeset *dataptr;

    /*  If setting the value of the global attribute would
	result in an implicit push (this would occur when the
	value is the first on the current group stack), then a new
	exeset is created and pushed on the stack.  Otherwise, the
	current exeset is loaded with the user specified exeset.  */

    if (dor_gas_test_set_value(doe_global_exeset_object->data)){
	dor_global_exeset_push_value(exeset);
    }
    else{
	dataptr = dor_global_exeset_get_data_pointer();

	if (dataptr == (dot_exeset *)DcNullPtr){
	    return;
	}

	dor_exeset_copy_exeset(dataptr, exeset);

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_exeset_update_execute();
    }
}
/*
 ======================================================================
 */

/* Note: returns a pointer to the actual exeset pushed */

dot_exeset *dor_global_exeset_push_value (dot_exeset *exeset)
{
    dot_exeset *newexeset;
    struct object *currentgroup;

    /*  Create a new exeset structure and copy the value to
	be pushed into it.  */

    newexeset = dor_exeset_create_exeset(exeset->objs);

    dor_exeset_copy_exeset(newexeset,exeset);

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_exeset_object->data,newexeset)){
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(
				   currentgroup, dor_global_exeset_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_exeset_update_execute();

    return(newexeset);
}
/*
 ======================================================================
 */

DtFlag dor_global_exeset_pop_value (void)
{
    DtFlag popflag;
    DtPtr exeset;

    popflag = dor_gas_pop_value (doe_global_exeset_object->data, &exeset);

    dor_global_exeset_destroy_element(exeset);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_exeset_update_execute();

    return(popflag);
}
/*
 ======================================================================
 */

/* Pop all of the attribute values stored for this global attribute for 
   the current group.  The gas subroutine will ask the attribute object 
   to destroy any instance data being popped.  */

void dor_global_exeset_pop_group (void)
{
    dor_gas_pop_group(doe_global_exeset_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_exeset_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_exeset_get_value (dot_exeset *exeset)
{
    DtFlag getflag;
    DtPtr currentexeset;

    getflag = dor_gas_get_value(doe_global_exeset_object->data, &currentexeset);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE, "dor_global_exeset_get_value",DcNullPtr);
	return(DcFalse);
    }

    dor_exeset_copy_exeset (exeset, (dot_exeset*)currentexeset);

    return(DcTrue);
}
/*
 ======================================================================
 */

dot_exeset *dor_global_exeset_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_exeset_object->data,&dataptr);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_exeset_get_data_pointer", DcNullPtr);

	return((dot_exeset *)DcNullPtr);
    }

    return((dot_exeset *)dataptr);
}
/*
 ======================================================================
 */

/* The attribute (may have) changed value.  Execute the currently active 
   method (like rendering, rendering initialization, etc.).  */

void dor_global_exeset_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_exeset_object);

    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_exeset_destroy_element (dot_exeset *exeset)
{
    dor_exeset_destroy_exeset(exeset);
}
/*
 ======================================================================
 */

void dor_global_exeset_obj_types_report (DtInt new_objs)
{
    DtInt current_objs, current_ints, ints_needed;
    DtInt i;

    if (doe_global_exeset_class_id == -1) 
	    return;

    current_objs = doe_global_exeset_base_exeset->objs;
    current_ints = doe_global_exeset_base_exeset->ints;

    if(new_objs < current_objs){
	DDerror (ERR_CANT_DECR_OBJ_TYPES,
		    "dor_global_exeset_obj_types_report", DcNullPtr);
    }

    ints_needed = (new_objs / 32) + (new_objs % 32 != 0);

    if(current_ints < ints_needed){
	doe_global_exeset_base_exeset->ints = ints_needed;
	doe_global_exeset_base_exeset->bitset = 
		dor_space_reallocate(doe_global_exeset_base_exeset->bitset,
				     4*ints_needed);
	for (i=current_ints; i<ints_needed; i++) {
	    doe_global_exeset_base_exeset->bitset[i] = ~0;
	}
    }

	
    doe_global_exeset_base_exeset->objs = new_objs;
}
