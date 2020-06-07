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
	dor_global_cammat_initialize 
	dor_global_cammat_set_value 
	dor_global_cammat_push_value 
	dor_global_cammat_pop_value 
	dor_global_cammat_pop_group 
	dor_global_cammat_get_value 
	dor_global_cammat_get_data_pointer 
	dor_global_cammat_update_execute 
	dor_global_cammat_destroy_element 
	dor_global_cammat_print_value 

  ======================================================================
 */
#include "global.h"
#include <dore/internal/matrix.h>

struct cammat {
   DtCameraMatrixType type;
   DtMatrix4x4 matrix;
};

dot_object *doe_global_cammat_object;
DtInt doe_global_cammat_class_id;
/*
 ======================================================================
 */

static DtMethodEntry methods[] = {
   { DcMethodInqGlbAttVal, (DtMethodPtr) dor_global_cammat_get_value }
};

void dor_global_cammat_initialize (void)
{
    struct gas *gas;
    DtMatrix4x4 initialmatrix;

    doe_global_cammat_class_id = dor_class_install (DcTypeAttCammat, 
		"AttCameraMatrix", 1,methods,DcNullPtr);

    gas = dor_gas_create(dor_global_cammat_destroy_element);

    doe_global_cammat_object = dor_object_create(doe_global_cammat_class_id,gas);

    dor_matrix_load_parallel(initialmatrix, 30., -1., -100.);
    dor_global_cammat_push_value(DcCameraParallel, initialmatrix);
}
/*
 ======================================================================
 */

void dor_global_cammat_set_value (
    DtCameraMatrixType type,
    DtMatrix4x4 matrix)
{
    struct cammat *dataptr;

    /*  If setting the value of the global attribute would
	result in an implicit push (this would occur when the
	value is the first on the current group stack), then a new
	matrix is created and pushed on the stack.  Otherwise, the
	current matrix is loaded with the user specified matrix.  */

    if (dor_gas_test_set_value(doe_global_cammat_object->data)) {
	dor_global_cammat_push_value(type, matrix);
    } else {
	dataptr = dor_global_cammat_get_data_pointer();

	if (dataptr == (struct cammat *)DcNullPtr) {
	    return;
	}

	dataptr->type = type;
	dor_matrix_load (dataptr->matrix,matrix);

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_cammat_update_execute();
    }
}
/*
 ======================================================================
 */

void dor_global_cammat_push_value (
    DtCameraMatrixType type,
    DtMatrix4x4 matrix)
{
    struct cammat *dataptr;
    dot_object *currentgroup;

    /*  Create a new camera matrix structure and copy the value to
	be pushed into it.  */

    dataptr = (struct cammat *)dor_space_allocate(sizeof *dataptr);

    dataptr->type = type;
    dor_matrix_load (dataptr->matrix,matrix);

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_cammat_object->data,dataptr)) {
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,dor_global_cammat_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_cammat_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_cammat_pop_value (void)
{
    DtFlag popflag;
    DtPtr  dataptr;

    popflag = dor_gas_pop_value (doe_global_cammat_object->data,&dataptr);

    if (popflag) {
	dor_global_cammat_destroy_element(dataptr);

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_cammat_update_execute();
    }

    return(popflag);
}
/*
 ======================================================================
 */

/* Pop all of the attribute values stored for this global attribute for 
   the current group.  The gas subroutine will ask the attribute object 
   to destroy any instance data being popped.  */

void dor_global_cammat_pop_group (void)
{
    dor_gas_pop_group(doe_global_cammat_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_cammat_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_cammat_get_value (
    DtCameraMatrixType *type,
    DtMatrix4x4 matrix)
{
    DtFlag getflag;
    DtPtr  dataptr;

    getflag = dor_gas_get_value (doe_global_cammat_object->data,
				 &dataptr);

    if (!getflag) {
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_cammat_get_value", DcNullPtr);

	return(DcFalse);
    }

    *type = ((struct cammat *)(dataptr))->type;
    dor_matrix_load (matrix, ((struct cammat *)(dataptr))->matrix);

    return(DcTrue);
}
/*
 ======================================================================
 */

struct cammat *dor_global_cammat_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_cammat_object->data,&dataptr);

    if (!getflag) {
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_cammat_get_data_pointer", DcNullPtr);

	return((struct cammat *)DcNullPtr);
    }

    return((struct cammat *)dataptr);
}
/*
 ======================================================================
 */

/* The attribute (may have) changed value.  Execute the currently active 
   method (like rendering, rendering initialization, etc.).  */

void dor_global_cammat_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_cammat_object);
   
    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_cammat_destroy_element (
    struct cammat *dataptr)
{
    dor_space_deallocate(dataptr);
}
/*
 ======================================================================
 */

void dor_global_cammat_print_value (void)
{
    DtCameraMatrixType type;
    DtMatrix4x4 matrix;

    dor_print_output("AttCameraMatrix:");
    dor_print_indent_up();

    if (!dor_global_cammat_get_value(&type, matrix)) {
	dor_print_output("bad values");
    } else {

	switch (type) {
	case DcCameraArbitrary:
	    dor_print_output("type = DcCameraArbitrary");
	    break;
	case DcCameraParallel:
	    dor_print_output("type = DcCameraParallel");
	    break;
	case DcCameraPerspective:
	    dor_print_output("type = DcCameraPerspective");
	    break;
	case DcCameraProjection:
	    dor_print_output("type = DcCameraProjection");
	    break;
	default:
	    sprintf (dor_print_get_line(), "type = %d", type);
	    dor_print_output(dor_print_get_line());
	    break;
	}
	dor_matrix_print(matrix);

    }
    dor_print_indent_down();
    dor_print_output("AttCameraMatrix end");
}
