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
	dor_global_lcstowcsmat_initialize 
	dor_global_lcstowcsmat_set_value 
	dor_global_lcstowcsmat_push_value 
	dor_global_lcstowcsmat_pop_value 
	dor_global_lcstowcsmat_pop_group 
	dor_global_lcstowcsmat_get_value 
	dor_global_lcstowcsmat_get_data_pointer 
	dor_global_lcstowcsmat_update_execute 
	dor_global_lcstowcsmat_destroy_element 
	dor_global_lcstowcsmat_notify_change 
	dor_global_lcstowcsmat_print_value 

  ======================================================================
 */
#include "global.h"
#include <dore/internal/matrix.h>

#define dod_global_lcstowcsmat_push		1
#define dod_global_lcstowcsmat_pop		2
#define dod_global_lcstowcsmat_get_value	3
#define dod_global_lcstowcsmat_set_value	4

dot_object *doe_global_lcstowcsmat_object;
DtInt doe_global_lcstowcsmat_class_id;

static DtMatrix4x4 * dor_global_lcstowcsmat_get_data_pointer (void);

/* ====================================================================== */

static DtMethodEntry methods[] = {
    { DcMethodInqGlbAttVal, (DtMethodPtr) dor_global_lcstowcsmat_get_value }
};

void dor_global_lcstowcsmat_initialize (void)
{
    struct gas *gas;
    DtMatrix4x4 initialmatrix;

    doe_global_lcstowcsmat_class_id = dor_class_install (DcTypeAttLcstowcsmat, 
		"AttLcstowcsmat", 1,methods, DcNullPtr);

    gas = dor_gas_create(dor_global_lcstowcsmat_destroy_element);

    doe_global_lcstowcsmat_object = dor_object_create(doe_global_lcstowcsmat_class_id,gas);

    dor_matrix_load_identity(initialmatrix);
    dor_global_lcstowcsmat_push_value(initialmatrix);
}
/*
 ======================================================================
 */

void dor_global_lcstowcsmat_set_value (
    DtMatrix4x4 matrix)
{
    DtMatrix4x4 *dataptr;

    /*  If setting the value of the global attribute would
	result in an implicit push (this would occur when the
	value is the first on the current group stack), then a new
	matrix is created and pushed on the stack.  Otherwise, the
	current matrix is loaded with the user specified matrix.  */

    if (dor_gas_test_set_value(doe_global_lcstowcsmat_object->data)) {
	dor_global_lcstowcsmat_push_value(matrix);
    } else {
	dataptr = dor_global_lcstowcsmat_get_data_pointer();

	if (dataptr == (DtMatrix4x4 *)DcNullPtr) {
	    return;
	}

	dor_matrix_load (*dataptr,matrix);

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_lcstowcsmat_update_execute(matrix, DcGlobalCallSetValue, 0);
    }
}
/*
 ======================================================================
 */

void dor_global_lcstowcsmat_push_value (
    DtMatrix4x4 matrix)
{
    DtMatrix4x4 *newmatrix;
    struct object *currentgroup;

    /*  Create a new matrix structure and copy the value to
	be pushed into it.  */

    newmatrix = (DtMatrix4x4 *)dor_space_allocate(sizeof *newmatrix);

    dor_matrix_load (*newmatrix,matrix);

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_lcstowcsmat_object->data,newmatrix)) {
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,dor_global_lcstowcsmat_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_lcstowcsmat_update_execute(matrix, DcGlobalCallPush, 0);
}
/*
 ======================================================================
 */

DtFlag dor_global_lcstowcsmat_pop_value (void)
{
    DtFlag       popflag;
    DtMatrix4x4 *matrix;

    popflag = dor_gas_pop_value (doe_global_lcstowcsmat_object->data,
				 (DtPtr)(&matrix));

    if (popflag) {
	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_lcstowcsmat_update_execute(*matrix, DcGlobalCallPop, 1);

	dor_global_lcstowcsmat_destroy_element(matrix);
    }

    return(popflag);
}
/*
 ======================================================================
 */

/* Pop all of the attribute values stored for this global attribute for 
   the current group.  The gas subroutine will ask the attribute object 
   to destroy any instance data being popped.  */

void dor_global_lcstowcsmat_pop_group (void)
{
    DtInt count;
    DtMatrix4x4 matrix;

    count = dor_gas_pop_group(doe_global_lcstowcsmat_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_lcstowcsmat_update_execute(matrix, DcGlobalCallPop, count);
}
/*
 ======================================================================
 */

DtFlag dor_global_lcstowcsmat_get_value (
    DtMatrix4x4 matrix)
{
    DtFlag getflag;
    DtMatrix4x4 *currentmatrix;

    getflag = dor_gas_get_value (doe_global_lcstowcsmat_object->data,
				 (DtPtr)(&currentmatrix));

    if (!getflag) {
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_lcstowcsmat_get_value", DcNullPtr);
	return DcFalse;
    }

    dor_matrix_load (matrix,*currentmatrix);
    dor_global_lcstowcsmat_update_execute (matrix, DcGlobalCallGetValue, 0);

    return DcTrue;
}
/*
 ======================================================================
 */

static DtMatrix4x4 * dor_global_lcstowcsmat_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_lcstowcsmat_object->data,&dataptr);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_lcstowcsmat_get_data_pointer", DcNullPtr);

	return((DtMatrix4x4 *)DcNullPtr);
    }

    return((DtMatrix4x4 *)dataptr);
}
/*
 ======================================================================
 */

/* The attribute (may have) changed value.  Execute the currently active 
   method (like rendering, rendering initialization, etc.).  */

void dor_global_lcstowcsmat_update_execute (
    DtMatrix4x4 matrix,
    DtGlobalCall exe_type,
    DtInt pop_count)
{
    static int in_count = 0;
    DtMethodPtr executemethod;

    if (in_count > 0)
	    return;

    in_count++;
    executemethod = dor_object_get_current_method(doe_global_lcstowcsmat_object);
    (*executemethod)(matrix, exe_type, pop_count);
    in_count--;
}


/*
 ======================================================================
 */

void dor_global_lcstowcsmat_destroy_element (
    DtMatrix4x4 *matrix)
{
    dor_matrix_destroy(*matrix);
}
/*
 ======================================================================
 */

static DtMatrix4x4 identmat = { { 1, 0, 0, 0 },
			        { 0, 1, 0, 0 },
			        { 0, 0, 1, 0 },
			        { 0, 0, 0, 1 } };
				     
void dor_global_lcstowcsmat_notify_change (void)
{
    /*  If setting the value of the global attribute would
	result in an implicit push (this would occur when the
	value is the first on the current group stack), then a dummy
	matrix is created and pushed on the stack.  Otherwise, nothing
	needs to be done */

    if (dor_gas_test_set_value(doe_global_lcstowcsmat_object->data)) {
	dor_global_lcstowcsmat_push_value(identmat);
    }
}
/*
 ======================================================================
 */

void dor_global_lcstowcsmat_print_value (void)
{
    DtMatrix4x4 matrix;

    printf("AttLcstowcsmat:\n");

    if (!dor_global_lcstowcsmat_get_value(matrix)){
	printf("\tcannot obtain value\n");

	return;
    }

    dor_matrix_print(matrix);
}
