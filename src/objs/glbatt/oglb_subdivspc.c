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
	dor_global_subdivspc_initialize 
	dor_global_subdivspc_set_value 
	dor_global_subdivspc_push_value 
	dor_global_subdivspc_pop_value 
	dor_global_subdivspc_pop_group 
	dor_global_subdivspc_get_value 
	dor_global_subdivlvl_get_value 
	dor_global_subdivspc_get_data_pointer 
	dor_global_subdivspc_update_execute 
	dor_global_subdivspc_destroy_element 

  ======================================================================
 */
#include "global.h"

struct global_subdivspc_data {
    DtInt type;
    DtReal *parms;
};

dot_object *doe_global_subdivspc_object;
DtInt doe_global_subdivspc_class_id;
/*
 ======================================================================
 */


static DtMethodEntry methods[] = {
    { DcMethodInqGlbAttVal, (DtMethodPtr)dor_global_subdivspc_get_value }
};

void dor_global_subdivspc_initialize (void)
{
    struct gas *gas;
    DtReal initiallevel;

    doe_global_subdivspc_class_id = dor_class_install (DcTypeAttSubDivSpec, 
		"AttSubDivSpec", 1,methods, DcNullPtr);

    gas = dor_gas_create(dor_global_subdivspc_destroy_element);

    doe_global_subdivspc_object = dor_object_create(doe_global_subdivspc_class_id,gas);

    initiallevel = .1;

    dor_global_subdivspc_push_value(DcSubDivRelative,&initiallevel);
}
/*
 ======================================================================
 */

void dor_global_subdivspc_set_value (
    DtInt type,
    DtReal *parms)
{
    struct global_subdivspc_data *subdivspcdata;

    /*  If setting the value of the global attribute would result in an 
	implicit push (this would occur when the value is the first on 
	the current group stack), then a new subdivision specification is 
	created and pushed on the stack.  Otherwise, the current subdivision 
	specification is loaded with the user specified value.  */

    if (dor_gas_test_set_value(doe_global_subdivspc_object->data)){
	dor_global_subdivspc_push_value(type,parms);
    }
    else{
	subdivspcdata = dor_global_subdivspc_get_data_pointer();

	if (subdivspcdata == (struct global_subdivspc_data *)DcNullPtr){
	    return;
	}

	subdivspcdata->type = type;
	subdivspcdata->parms[0] = parms[0];

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_subdivspc_update_execute();
    }
}
/*
 ======================================================================
 */

void dor_global_subdivspc_push_value (
    DtInt type,
    DtReal *parms)
{
    struct global_subdivspc_data *newsubdivspcdata;
    struct object *currentgroup;

    /*  Create a new depth cue data structure and copy the values to
	be pushed into it.  */

    newsubdivspcdata = (struct global_subdivspc_data *)
		dor_space_allocate(sizeof *newsubdivspcdata);

    newsubdivspcdata->type = type;

    newsubdivspcdata->parms = (DtReal *)dor_space_allocate(sizeof (DtReal));
    newsubdivspcdata->parms[0] = parms[0];

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_subdivspc_object->data,newsubdivspcdata)){
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,dor_global_subdivspc_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_subdivspc_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_subdivspc_pop_value (void)
{
    DtFlag popflag;
    struct global_subdivspc_data *subdivspcdata;

    popflag = dor_gas_pop_value (doe_global_subdivspc_object->data,
				 (DtPtr)(&subdivspcdata));

    dor_global_subdivspc_destroy_element(subdivspcdata);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_subdivspc_update_execute();

    return(popflag);
}
/*
 ======================================================================
 */

/*  Pop all of the attribute values stored for this global attribute for 
    the current group.  The gas subroutine will ask the attribute object 
    to destroy any instance data being popped.  */

void dor_global_subdivspc_pop_group (void)
{
    dor_gas_pop_group(doe_global_subdivspc_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_subdivspc_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_subdivspc_get_value (
    DtInt *type,
    DtReal *parms)
{
    DtFlag getflag;
    struct global_subdivspc_data *subdivspcdata;

    getflag = dor_gas_get_value (doe_global_subdivspc_object->data,
				 (DtPtr)(&subdivspcdata));

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_subdivspc_get_value", DcNullPtr);

	return(DcFalse);
    }

    *type = subdivspcdata->type;

    parms[0] = subdivspcdata->parms[0];

    return(DcTrue);
}
/*
 ======================================================================
 */

DtFlag dor_global_subdivlvl_get_value (
    DtReal *value)
{
    DtFlag getflag;
    DtInt type;
    DtReal firstparm;

    getflag = dor_global_subdivspc_get_value(&type,&firstparm);

    if (!getflag){
	return(DcFalse);
    }

    *value = firstparm;

    return(DcTrue);
}
/*
 ======================================================================
 */

struct global_subdivspc_data *dor_global_subdivspc_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_subdivspc_object->data,&dataptr);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_subdivspc_get_data_pointer", DcNullPtr);

	return((struct global_subdivspc_data *)DcNullPtr);
    }

    return((struct global_subdivspc_data *)dataptr);
}
/*
 ======================================================================
 */

/*  The attribute (may have) changed value.  Execute the currently active 
    method (like rendering, rendering initialization, etc.).  */

void dor_global_subdivspc_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_subdivspc_object);

    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_subdivspc_destroy_element (
    struct global_subdivspc_data *subdivspcdata)
{
    dor_space_deallocate(subdivspcdata->parms);
    dor_space_deallocate(subdivspcdata);
}
