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
	dor_global_clpvol_initialize 
	dor_global_clpvol_set_value 
	dor_global_clpvol_push_value 
	dor_global_clpvol_pop_value 
	dor_global_clpvol_pop_group 
	dor_global_clpvol_get_value 
	dor_global_clpvol_get_data_pointer 
	dor_global_clpvol_update_execute 
	dor_global_clpvol_destroy_element 
	dor_global_clpvol_notify_change 
	dor_global_clpvol_apply_volume 
	dor_global_clpvol_push_apply_volume 

  ======================================================================
 */
#include "global.h"
#include <dore/internal/modclpvol.h>

#define dod_global_clpvol_exe_none 	-1
#define dod_global_clpvol_exe_push 	1
#define dod_global_clpvol_exe_pop 	2
#define dod_global_clpvol_exe_set 	3

dot_object *doe_global_clpvol_object;
DtInt doe_global_clpvol_class_id;

#if 0
typedef struct modclpvol {
    DtInt halfspacecount;
    DtHalfSpace *halfspaces;
    DtPoint4 *plneqns;
} dot_modclpvol;
#endif

/*
 ======================================================================
 */

static DtMethodEntry methods[] = {
    { DcMethodInqGlbAttVal, (DtMethodPtr) dor_global_clpvol_get_value }
};

void dor_global_clpvol_initialize (void)
{
    struct gas *gas;

    doe_global_clpvol_class_id = dor_class_install (DcTypeAttClipVolume, 
		"AttClipVol", 1,methods, DcNullPtr);

    gas = dor_gas_create(dor_global_clpvol_destroy_element);

    doe_global_clpvol_object = dor_object_create(doe_global_clpvol_class_id,gas);

    dor_global_clpvol_push_value(DcClipNone,0,NULL);
}
/*
 ======================================================================
 */

void dor_global_clpvol_set_value (
    DtClipOperator operator,
    DtInt halfspacecount,
    DtHalfSpace *halfspaces)
{
    dot_modclpvol *modclpvol;
    DtMatrix4x4 lcstowcs;

    dor_global_lcstowcsmat_get_value(lcstowcs);


    /*  If setting the value of the global attribute would result in 
	an implicit push (this would occur when the value is the first 
	on the current group stack), then a new model clipping volume 
	is created and pushed on the stack.  Otherwise, the current 
	volume is loaded with the user specified volume.  */

    if (dor_gas_test_set_value(doe_global_clpvol_object->data)){
	dor_global_clpvol_push_value(operator,halfspacecount,halfspaces);
    }
    else{
	modclpvol = dor_global_clpvol_get_data_pointer();

	if (modclpvol == (dot_modclpvol *)DcNullPtr){
	    return;
	}

	dor_modclpvol_set_value(modclpvol,lcstowcs,operator,halfspacecount,halfspaces);

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_clpvol_update_execute(DcGlobalCallSetValue, 0);
    }
}
/*
 ======================================================================
 */

void dor_global_clpvol_push_value (
    DtClipOperator operator,
    DtInt halfspacecount,
    DtHalfSpace *halfspaces)
{
    dot_modclpvol *modclpvol;
    struct object *currentgroup;
    DtMatrix4x4 lcstowcs;

    dor_global_lcstowcsmat_get_value (lcstowcs);

    /*  Create a new modclpvol structure and copy the value to
	be pushed into it.  */

    modclpvol = dor_modclpvol_create(lcstowcs,operator,halfspacecount,halfspaces);

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_clpvol_object->data,modclpvol)){
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,dor_global_clpvol_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_clpvol_update_execute(DcGlobalCallPush, 0);
	
}
/*
 ======================================================================
 */

DtFlag dor_global_clpvol_pop_value (void)
{
    DtFlag popflag;
    DtPtr modclpvol;

    popflag = dor_gas_pop_value(doe_global_clpvol_object->data,&modclpvol);

    dor_global_clpvol_destroy_element(modclpvol);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_clpvol_update_execute(DcGlobalCallPop, 1);

    return(popflag);
}
/*
 ======================================================================
 */

/* Pop all of the attribute values stored for this global attribute for 
   the current group.  The gas subroutine will ask the attribute object 
   to destroy any instance data being popped.  */

void dor_global_clpvol_pop_group (void)
{
    DtInt count;

    count = dor_gas_pop_group(doe_global_clpvol_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_clpvol_update_execute(DcGlobalCallPop, count);
}
/*
 ======================================================================
 */

DtFlag dor_global_clpvol_get_value (
    DtInt *count,
    DtPoint3 *point,
    DtVector3 *vector,
    DtPoint4 *plneqn)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_clpvol_object->data,&dataptr);

    if (!getflag) {
	DDerror (ERR_NO_CURRENT_VALUE, "dor_global_clpvol_get_value",DcNullPtr);
	return (DcFalse);
    }

    dor_modclpvol_get_value (dataptr, count, point, vector, plneqn);

    return (DcTrue);
}

/*
 ======================================================================
 */

dot_modclpvol *dor_global_clpvol_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_clpvol_object->data,&dataptr);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_clpvol_get_data_pointer", DcNullPtr);

	return((dot_modclpvol *)NULL);
    }

    return((dot_modclpvol *)dataptr);
}
/*
 ======================================================================
 */

/* The attribute (may have) changed value.  Execute the currently active 
   method (like rendering, rendering initialization, etc.).  */

void dor_global_clpvol_update_execute (
    DtGlobalCall exe_type,
    DtInt pop_count)
{
    static int in_count = 0;
    DtMethodPtr executemethod;

    if (in_count > 0) 
	    return;

    in_count++;
    executemethod = dor_object_get_current_method(doe_global_clpvol_object);
    (*executemethod)(exe_type, pop_count);
    in_count--;
}
/*
 ======================================================================
 */

void dor_global_clpvol_destroy_element (
    dot_modclpvol *modclpvol)
{

    dor_modclpvol_destroy(modclpvol);
}
/*
 ======================================================================
 */

void dor_global_clpvol_notify_change (void)
{
    /*  If setting the value of the global attribute would
	result in an implicit push (this would occur when the
	value is the first on the current group stack), then a dummy
	matrix is created and pushed on the stack.  Otherwise, nothing
	needs to be done */

    if (dor_gas_test_set_value(doe_global_clpvol_object->data)) {
	dor_global_clpvol_push_value(DcClipNone,0,NULL);
    }
}
/*
 ======================================================================
 */

void dor_global_clpvol_apply_volume (
    DtClipOperator operator,
    DtInt halfspacecount,
    DtHalfSpace *halfspaces)
{
    dot_modclpvol *modclpvol;
    DtMatrix4x4 lcstowcs;

    dor_global_lcstowcsmat_get_value(lcstowcs);

    /*  If setting the value of the global attribute would result 
	in an implicit push (this would occur when the value is the 
	first on the current group stack), then a new clipping volume 
	is created and pushed on the stack.  Otherwise, the
	current volume is loaded with the user specified volume.  */

    if (dor_gas_test_set_value(doe_global_clpvol_object->data)){
	dor_global_clpvol_push_apply_volume(operator,halfspacecount,halfspaces);
    }
    else{
	modclpvol = dor_global_clpvol_get_data_pointer();

	if (modclpvol == (dot_modclpvol *)DcNullPtr){
	    return;
	}

	dor_modclpvol_apply_volume(modclpvol,lcstowcs,operator,halfspacecount,halfspaces);

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_clpvol_update_execute(DcGlobalCallSetValue, 0);
    }
}
/*
 ======================================================================
 */

void dor_global_clpvol_push_apply_volume (
    DtClipOperator operator,
    DtInt halfspacecount,
    DtHalfSpace *halfspaces)
{
    dot_modclpvol *oldmodclpvol;
    dot_modclpvol *modclpvol;
    struct object *currentgroup;
    DtMatrix4x4 lcstowcs;

    dor_global_lcstowcsmat_get_value(lcstowcs);

    /*  Create a new modclpvol structure and copy the value to
	be pushed into it.  */

    oldmodclpvol = dor_global_clpvol_get_data_pointer();
    modclpvol = dor_modclpvol_copy(oldmodclpvol);
    dor_modclpvol_apply_volume(modclpvol,lcstowcs,operator,halfspacecount,halfspaces);

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_clpvol_object->data,modclpvol)){
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,dor_global_clpvol_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_clpvol_update_execute(DcGlobalCallPush, 0);
}
