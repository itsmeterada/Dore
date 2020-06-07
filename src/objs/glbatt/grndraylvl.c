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
	dor_global_glbrndraylvl_initialize 
	dor_global_glbrndraylvl_set_value 
	dor_global_glbrndraylvl_push_value 
	dor_global_glbrndraylvl_pop_value 
	dor_global_glbrndraylvl_pop_group 
	dor_global_glbrndraylvl_get_value 
	dor_global_glbrndraylvl_get_data_pointer 
	dor_global_glbrndraylvl_update_execute 
	dor_global_glbrndraylvl_destroy_element 

  ======================================================================
 */
#include "global.h"

dot_object *doe_global_glbrndraylvl_object;
DtInt doe_global_glbrndraylvl_class_id;
/*
 ======================================================================
 */

static DtMethodEntry methods[] = {
    { DcMethodInqGlbAttVal, (DtMethodPtr) dor_global_glbrndraylvl_get_value }
};

void dor_global_glbrndraylvl_initialize (void)
{
    struct gas *gas;

    doe_global_glbrndraylvl_class_id = dor_class_install (
					   DcTypeAttGlbRndRayLevel, 
					   "AttGlbRendRayLevel", 1,
					   methods,DcNullPtr);

    gas = dor_gas_create(dor_global_glbrndraylvl_destroy_element);

    doe_global_glbrndraylvl_object = dor_object_create
		(doe_global_glbrndraylvl_class_id, gas);

    dor_global_glbrndraylvl_push_value(3);
}
/*
 ======================================================================
 */

void dor_global_glbrndraylvl_set_value (
    DtInt glbrndraylvl)
{
    DtInt *dataptr;

    /*  If setting the value of the global attribute would result in 
	an implicit push (this would occur when the value is the first 
	on the current group stack), then a new "maximum bounces" value 
	is created and pushed on the stack.  Otherwise, the current value 
	is loaded with the user specified value.  */

    if (dor_gas_test_set_value(doe_global_glbrndraylvl_object->data)){
	dor_global_glbrndraylvl_push_value(glbrndraylvl);
    }
    else{
	dataptr = dor_global_glbrndraylvl_get_data_pointer();

	if (dataptr == (DtInt *)DcNullPtr){
	    return;
	}

	*dataptr = glbrndraylvl;

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_glbrndraylvl_update_execute();
    }
}
/*
 ======================================================================
 */

void dor_global_glbrndraylvl_push_value (
    DtInt glbrndraylvl)
{
    DtInt *newglbrndraylvl;
    struct object *currentgroup;

    /*  Create a new color structure and copy the value to
	be pushed into it.  */

    newglbrndraylvl = (DtInt *)dor_space_allocate(sizeof *newglbrndraylvl);

    *newglbrndraylvl = glbrndraylvl;

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_glbrndraylvl_object->data,newglbrndraylvl)){
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,dor_global_glbrndraylvl_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_glbrndraylvl_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_glbrndraylvl_pop_value (void)
{
    DtFlag popflag;
    DtPtr glbrndraylvl;

    popflag = dor_gas_pop_value (doe_global_glbrndraylvl_object->data,
				 &glbrndraylvl);

    dor_global_glbrndraylvl_destroy_element(glbrndraylvl);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_glbrndraylvl_update_execute();

    return(popflag);
}
/*
 ======================================================================
 */

/* Pop all of the attribute values stored for this global attribute for 
   the current group.  The gas subroutine will ask the attribute object 
   to destroy any instance data being popped.  */

void dor_global_glbrndraylvl_pop_group (void)
{
    dor_gas_pop_group(doe_global_glbrndraylvl_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_glbrndraylvl_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_glbrndraylvl_get_value (
    DtInt *glbrndraylvl)
{
    DtFlag getflag;
    DtPtr currentglbrndraylvl;

    getflag = dor_gas_get_value(doe_global_glbrndraylvl_object->data,&currentglbrndraylvl);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_glbrndraylvl_get_value", DcNullPtr);
	return(DcFalse);
    }

    *glbrndraylvl = *(DtInt*)(currentglbrndraylvl);

    return(DcTrue);
}
/*
 ======================================================================
 */

DtInt *dor_global_glbrndraylvl_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_glbrndraylvl_object->data,&dataptr);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_glbrndraylvl_get_data_pointer", DcNullPtr);

	return((DtInt *)DcNullPtr);
    }

    return((DtInt *)dataptr);
}
/*
 ======================================================================
 */

/* The attribute (may have) changed value.  Execute the currently active 
   method (like rendering, rendering initialization, etc.).  */

void dor_global_glbrndraylvl_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_glbrndraylvl_object);

    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_glbrndraylvl_destroy_element (DtInt *element)
{
    dor_space_deallocate (element);
}
