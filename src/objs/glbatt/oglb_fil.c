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
	dor_global_fil_initialize 
	dor_global_fil_set_value 
	dor_global_fil_push_value 
	dor_global_fil_pop_value 
	dor_global_fil_pop_group 
	dor_global_fil_get_value 
	dor_global_fil_get_data_pointer 
	dor_global_fil_update_execute 
	dor_global_fil_destroy_element 

  ======================================================================
 */
#include "global.h"
#include <dore/internal/filters.h>

dot_object *doe_global_fil_object;
DtInt doe_global_fil_class_id;
/*
 ======================================================================
 */

static DtMethodEntry methods[] = {
    { 0, 0 }
};

void dor_global_fil_initialize (void)
{
    struct gas *gas;
    dot_filters initialfil;
    int i, j;

    doe_global_fil_class_id = dor_class_install (
				  DcTypeAttFilter, "Attribute Filter", 0,
				  methods, DcNullPtr);


    gas = dor_gas_create(dor_global_fil_destroy_element);

    doe_global_fil_object = dor_object_create(doe_global_fil_class_id,gas);

    for (i=0; i<dod_filter_bitfields; i++){
	for (j=0; j<dod_bitfield_ints; j++){
	    initialfil[i][j] = (Dt32Bits)0;
	}
    }

    dor_global_fil_push_value(initialfil);
}
/*
 ======================================================================
 */

void dor_global_fil_set_value (
    DtFilter     fil,
    dot_bitfield bitfield)
{
    dot_filters *newfil;
    dot_filters *dataptr;

    /*  If setting the value of the global attribute would
	result in an implicit push (this would occur when the
	value is the first on the current group stack), then a new
	fil is created and pushed on the stack.  Otherwise, the
	current fil is loaded with the user specified fil.  */

    dataptr = (dot_filters *)dor_global_fil_get_data_pointer();

    if (dor_gas_test_set_value(doe_global_fil_object->data)){
	newfil = dor_filters_create();

	dor_filters_copy (*newfil, *dataptr);

	dor_bitfield_copy((*newfil)[(int)fil], bitfield, dod_bitfield_ints);

	dor_global_fil_push_value (*newfil);
    }
    else{
	if (!dataptr) {
	    return;
	}

	dor_bitfield_copy((*dataptr)[(int)fil], bitfield, dod_bitfield_ints);

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_fil_update_execute();
    }
}
/*
 ======================================================================
 */

void dor_global_fil_push_value (dot_filters fil)
{
    dot_filters *newfil;
    struct object *currentgroup;

    /*  Create a new fil structure and copy the value to
	be pushed into it.  */

    newfil = dor_filters_create();

    dor_filters_copy(*newfil,fil);

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_fil_object->data,newfil)){
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(
				   currentgroup, dor_global_fil_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_fil_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_fil_pop_value (void)
{
    DtFlag popflag;
    DtPtr fil;

    popflag = dor_gas_pop_value (doe_global_fil_object->data, &fil);

    dor_global_fil_destroy_element (*(dot_filters*)(fil));

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_fil_update_execute();

    return popflag;
}
/*
 ======================================================================
 */

/* Pop all of the attribute values stored for this global attribute for 
   the current group.  The gas subroutine will ask the attribute object 
   to destroy any instance data being popped.  */

void dor_global_fil_pop_group (void)
{
    dor_gas_pop_group(doe_global_fil_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_fil_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_fil_get_value (dot_filters fil)
{
    DtFlag getflag;
    dot_filters *currentfil;

    getflag = dor_gas_get_value
		 (doe_global_fil_object->data, (DtPtr)(&currentfil));

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE, "dor_global_fil_get_value", DcNullPtr);
	return DcFalse;
    }

    dor_filters_copy (fil, *currentfil);

    return DcTrue;
}
/*
 ======================================================================
 */

DtPtr dor_global_fil_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_fil_object->data,&dataptr);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_fil_get_data_pointer", DcNullPtr);

	return(DcNullPtr);
    }

    return(dataptr);
}
/*
 ======================================================================
 */

/* The attribute (may have) changed value.  Execute the currently active 
   method (like rendering, rendering initialization, etc.).  */

void dor_global_fil_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_fil_object);

    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_fil_destroy_element (dot_filters fil)
{
    dor_filters_destroy (fil);
}
