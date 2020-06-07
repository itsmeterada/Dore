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
	dor_global_lintyp_initialize 
	dor_global_lintyp_set_value 
	dor_global_lintyp_push_value 
	dor_global_lintyp_pop_value 
	dor_global_lintyp_pop_group 
	dor_global_lintyp_get_value 
	dor_global_lintyp_get_data_pointer 
	dor_global_lintyp_update_execute 
	dor_global_lintyp_destroy_element 

  ======================================================================
 */
#include "global.h"

dot_object *doe_global_lintyp_object;
DtInt doe_global_lintyp_class_id;
/*
 ======================================================================
 */


static DtMethodEntry methods[] = {
    { DcMethodInqGlbAttVal, (DtMethodPtr)dor_global_lintyp_get_value }
};

void dor_global_lintyp_initialize (void)
{
    struct gas *gas;

    doe_global_lintyp_class_id = dor_class_install (DcTypeAttLineType, 
		"AttLineType", 1,methods, DcNullPtr);

    gas = dor_gas_create(dor_global_lintyp_destroy_element);

    doe_global_lintyp_object = dor_object_create(doe_global_lintyp_class_id,gas);

    dor_global_lintyp_push_value(DcLineTypeSolid);
}
/*
 ======================================================================
 */

void dor_global_lintyp_set_value (DtLineType lintyp)
{
    DtLineType *dataptr;

    /*  If setting the value of the global attribute would
	result in an implicit push (this would occur when the
	value is the first on the current group stack), then a new
	line type is created and pushed on the stack.  Otherwise, the
	current line type is loaded with the user specified type.  */

    if (dor_gas_test_set_value(doe_global_lintyp_object->data)){
	dor_global_lintyp_push_value(lintyp);
    }
    else{
	dataptr = dor_global_lintyp_get_data_pointer();

	if (dataptr == (DtLineType *)DcNullPtr){
	    return;
	}

	*dataptr = lintyp;

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_lintyp_update_execute();
    }
}
/*
 ======================================================================
 */

void dor_global_lintyp_push_value (DtLineType lintyp)
{
    DtLineType *newlintyp;
    struct object *currentgroup;

    /*  Create a new color structure and copy the value to
	be pushed into it.  */

    newlintyp = (DtLineType *)dor_space_allocate(sizeof *newlintyp);

    *newlintyp = lintyp;

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_lintyp_object->data,newlintyp)){
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,dor_global_lintyp_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_lintyp_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_lintyp_pop_value (void)
{
    DtFlag popflag;
    DtPtr lintyp;

    popflag = dor_gas_pop_value(doe_global_lintyp_object->data,&lintyp);

    dor_global_lintyp_destroy_element(lintyp);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_lintyp_update_execute();

    return(popflag);
}
/*
 ======================================================================
 */

/*  Pop all of the attribute values stored for this global attribute for 
    the current group.  The gas subroutine will ask the attribute object 
    to destroy any instance data being popped.  */

void dor_global_lintyp_pop_group (void)
{
    dor_gas_pop_group(doe_global_lintyp_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_lintyp_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_lintyp_get_value (DtLineType *lintyp)
{
    DtFlag getflag;
    DtPtr currentlintyp;

    getflag = dor_gas_get_value(doe_global_lintyp_object->data,&currentlintyp);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_lintyp_get_value", DcNullPtr);

	return(DcFalse);
    }

    *lintyp = *(DtLineType*)(currentlintyp);

    return(DcTrue);
}
/*
 ======================================================================
 */

DtLineType *dor_global_lintyp_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_lintyp_object->data,&dataptr);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_lintyp_get_data_pointer", DcNullPtr);

	return((DtLineType *)DcNullPtr);
    }

    return((DtLineType *)dataptr);
}
/*
 ======================================================================
 */

/*  The attribute (may have) changed value.  Execute the currently active 
    method (like rendering, rendering initialization, etc.).  */

void dor_global_lintyp_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_lintyp_object);

    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_lintyp_destroy_element (DtLineType *element)
{
    dor_space_deallocate(element);
}
