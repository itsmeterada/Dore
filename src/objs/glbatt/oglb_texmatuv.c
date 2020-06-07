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
	dor_global_texmatuv_initialize 
	dor_global_texmatuv_set_value 
	dor_global_texmatuv_push_value 
	dor_global_texmatuv_pop_value 
	dor_global_texmatuv_pop_group 
	dor_global_texmatuv_get_value 
	dor_global_texmatuv_get_data_pointer 
	dor_global_texmatuv_update_execute 
	dor_global_texmatuv_destroy_element 
	dor_global_texmatuv_print_value 

  ======================================================================
 */
#include "global.h"
#include <dore/internal/matrix.h>

struct texmatuv {
    DtMatrix3x3	matrix;
};

dot_object *doe_global_texmatuv_object;
DtInt doe_global_texmatuv_class_id;
/*
 ======================================================================
 */


static DtMethodEntry methods[] = {
    { DcMethodInqGlbAttVal, (DtMethodPtr)dor_global_texmatuv_get_value }
};

void dor_global_texmatuv_initialize (void)
{
    struct gas *gas;
    DtMatrix3x3 initialmatrix;

    doe_global_texmatuv_class_id = dor_class_add ("AttTextureMatrixUV", 1,
		methods, DcNullPtr);

    gas = dor_gas_create(dor_global_texmatuv_destroy_element);

    doe_global_texmatuv_object = dor_object_create(
			doe_global_texmatuv_class_id, gas);

    dor_matrix_load_identity3x3 (initialmatrix);
    dor_global_texmatuv_push_value(initialmatrix);
}
/*
 ======================================================================
 */

void dor_global_texmatuv_set_value (
    DtMatrix3x3 matrix)
{
    struct texmatuv *dataptr;

    /*  If setting the value of the global attribute would result in 
	an implicit push (this would occur when the value is the first 
	on the current group stack), then a new 3x3 texture matrix is 
	created and pushed on the stack.  Otherwise, the current matrix 
	is loaded with the user specified matrix.  */

    if (dor_gas_test_set_value(doe_global_texmatuv_object->data)) {
	dor_global_texmatuv_push_value(matrix);
    } else {
	dataptr = dor_global_texmatuv_get_data_pointer();

	if (dataptr == (struct texmatuv *)DcNullPtr) {
	    return;
	}

	dor_matrix_load3x3 (dataptr->matrix,matrix);

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_texmatuv_update_execute();
    }
}
/*
 ======================================================================
 */

void dor_global_texmatuv_push_value (
    DtMatrix3x3 matrix)
{
    struct texmatuv *dataptr;
    dot_object *currentgroup;

    /*  Create a new texture 3x3 matrix structure and copy the value to
	be pushed into it.  */

    dataptr = (struct texmatuv *)dor_space_allocate(sizeof *dataptr);

    dor_matrix_load3x3 (dataptr->matrix,matrix);

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_texmatuv_object->data,dataptr)) {
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,dor_global_texmatuv_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_texmatuv_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_texmatuv_pop_value (void)
{
    DtFlag popflag;
    struct texmatuv *dataptr;

    popflag = dor_gas_pop_value (doe_global_texmatuv_object->data,
				 (DtPtr)(&dataptr));

    if (popflag) {
	dor_global_texmatuv_destroy_element(dataptr);

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_texmatuv_update_execute();
    }

    return(popflag);
}
/*
 ======================================================================
 */

/* Pop all of the attribute values stored for this global attribute for 
   the current group.  The gas subroutine will ask the attribute object 
   to destroy any instance data being popped.  */

void dor_global_texmatuv_pop_group (void)
{
    dor_gas_pop_group(doe_global_texmatuv_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_texmatuv_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_texmatuv_get_value (
    DtMatrix3x3 matrix)
{
    DtFlag getflag;
    struct texmatuv *dataptr;

    getflag = dor_gas_get_value (doe_global_texmatuv_object->data,
				 (DtPtr)(&dataptr));

    if (!getflag) {
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_texmatuv_get_value", DcNullPtr);

	return(DcFalse);
    }

    dor_matrix_load3x3 (matrix,dataptr->matrix);

    return(DcTrue);
}
/*
 ======================================================================
 */

struct texmatuv *dor_global_texmatuv_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_texmatuv_object->data,&dataptr);

    if (!getflag) {
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_texmatuv_get_data_pointer", DcNullPtr);

	return((struct texmatuv *)DcNullPtr);
    }

    return((struct texmatuv *)dataptr);
}
/*
 ======================================================================
 */

/* The attribute (may have) changed value.  Execute the currently active 
   method (like rendering, rendering initialization, etc.).  */

void dor_global_texmatuv_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_texmatuv_object);
   
    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_texmatuv_destroy_element (
    struct texmatuv *dataptr)
{
    dor_space_deallocate(dataptr);
}
/*
 ======================================================================
 */

void dor_global_texmatuv_print_value (void)
{
    DtMatrix3x3 matrix;

    dor_print_output("AttTextureMatrixUV:");
    dor_print_indent_up();

    if (!dor_global_texmatuv_get_value(matrix)) {
	dor_print_output("bad values");
    } else {
	dor_matrix_print3x3 (matrix);
    }
    dor_print_indent_down();
    dor_print_output("AttTextureMatrixUV end");
}
