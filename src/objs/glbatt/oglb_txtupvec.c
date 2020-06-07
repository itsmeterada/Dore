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
	dor_global_txtupvec_initialize 
	dor_global_txtupvec_set_value 
	dor_global_txtupvec_push_value 
	dor_global_txtupvec_pop_value 
	dor_global_txtupvec_pop_group 
	dor_global_txtupvec_get_value 
	dor_global_txtupvec_get_value2 
	dor_global_txtupvec_get_data_pointer 
	dor_global_txtupvec_update_execute 
	dor_global_txtupvec_destroy_element 

  ======================================================================
 */
#include "global.h"

dot_object *doe_global_txtupvec_object;
DtInt doe_global_txtupvec_class_id;
/*
 ======================================================================
 */

DtFlag dor_global_txtupvec_get_value2 (DtReal*, DtReal*);

static DtMethodEntry methods[] = {
    { DcMethodInqGlbAttVal, (DtMethodPtr)dor_global_txtupvec_get_value2 }
};

void dor_global_txtupvec_initialize (void)
{
    struct gas *gas;
    dot_txtupvec initialtxtupvec;

    doe_global_txtupvec_class_id = dor_class_install (DcTypeAttTextUpVector, 
		"AttTextUpVector", 1,methods,DcNullPtr);

    gas = dor_gas_create(dor_global_txtupvec_destroy_element);

    doe_global_txtupvec_object = dor_object_create(doe_global_txtupvec_class_id,gas);

    initialtxtupvec.xup = 0.;
    initialtxtupvec.yup = 1.;

    dor_global_txtupvec_push_value(&initialtxtupvec);
}
/*
 ======================================================================
 */

void dor_global_txtupvec_set_value (
    dot_txtupvec *txtupvec)
{
    dot_txtupvec *dataptr;

    /*  If setting the value of the global attribute would result in 
	an implicit push (this would occur when the value is the first 
	on the current group stack), then a new text up vector is 
	created and pushed on the stack.  Otherwise, the current 
	text up vector is loaded with the user specified up vector.  */

    if (dor_gas_test_set_value(doe_global_txtupvec_object->data)){
	dor_global_txtupvec_push_value(txtupvec);
    }
    else{
	dataptr = dor_global_txtupvec_get_data_pointer();

	if (dataptr == (dot_txtupvec *)DcNullPtr){
	    return;
	}

	dataptr->xup = txtupvec->xup ;
	dataptr->yup = txtupvec->yup ;

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_txtupvec_update_execute();
    }
}
/*
 ======================================================================
 */

void dor_global_txtupvec_push_value (
    dot_txtupvec *txtupvec)
{
    dot_txtupvec *newtxtupvec;
    struct object *currentgroup;

    /*  Create a new txtupvec structure and copy the value to
	be pushed into it.  */

    newtxtupvec = (dot_txtupvec *)dor_space_allocate(sizeof *newtxtupvec);

    newtxtupvec->xup = txtupvec->xup ;
    newtxtupvec->yup = txtupvec->yup ;

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_txtupvec_object->data,newtxtupvec)){
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,dor_global_txtupvec_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_txtupvec_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_txtupvec_pop_value (void)
{
    DtFlag popflag;
    dot_txtupvec *txtupvec;

    popflag = dor_gas_pop_value (doe_global_txtupvec_object->data,
				 (DtPtr)(&txtupvec));

    dor_global_txtupvec_destroy_element(txtupvec);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_txtupvec_update_execute();

    return(popflag);
}
/*
 ======================================================================
 */

/*  Pop all of the attribute values stored for this global attribute for 
    the current group.  The gas subroutine will ask the attribute object 
    to destroy any instance data being popped.  */

void dor_global_txtupvec_pop_group (void)
{
    dor_gas_pop_group(doe_global_txtupvec_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_txtupvec_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_txtupvec_get_value (
    dot_txtupvec *txtupvec)
{
    DtFlag getflag;
    dot_txtupvec *currenttxtupvec;

    getflag = dor_gas_get_value (doe_global_txtupvec_object->data,
				 (DtPtr)(&currenttxtupvec));

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_txtupvec_get_value", DcNullPtr);

	return(DcFalse);
    }

    txtupvec->xup = currenttxtupvec->xup ;
    txtupvec->yup = currenttxtupvec->yup ;

    return(DcTrue);
}
/*
 ======================================================================
 */

DtFlag dor_global_txtupvec_get_value2 (
    DtReal *xup,
    DtReal *yup)
{
    DtFlag getflag;
    dot_txtupvec *currenttxtupvec;

    getflag = dor_gas_get_value (doe_global_txtupvec_object->data,
				 (DtPtr)(&currenttxtupvec));

    if (!getflag) {
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_txtupvec_get_value", DcNullPtr);

	return(DcFalse);
    }

    *xup = currenttxtupvec->xup ;
    *yup = currenttxtupvec->yup ;

    return(DcTrue);
}
/*
 ======================================================================
 */

dot_txtupvec *dor_global_txtupvec_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_txtupvec_object->data,&dataptr);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_txtupvec_get_data_pointer", DcNullPtr);

	return((dot_txtupvec *)DcNullPtr);
    }

    return((dot_txtupvec *)dataptr);
}
/*
 ======================================================================
 */

/*  The attribute (may have) changed value.  Execute the currently active 
    method (like rendering, rendering initialization, etc.).  */

void dor_global_txtupvec_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_txtupvec_object);

    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_txtupvec_destroy_element (
    dot_txtupvec *txtupvec)
{
    dor_space_deallocate(txtupvec);
}
