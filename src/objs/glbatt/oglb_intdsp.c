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
	dor_global_intermdisptype_initialize 
	dor_global_intermdisptype_set_value 
	dor_global_intermdisptype_push_value 
	dor_global_intermdisptype_pop_value 
	dor_global_intermdisptype_pop_group 
	dor_global_intermdisptype_get_value
	dor_global_intermdisptype_get_data_pointer 
	dor_global_intermdisptype_update_execute 
	dor_global_intermdisptype_destroy_element 

  ======================================================================
 */
#include "global.h"

struct global_intermdisptype_data {
    DtIntermediateDispType type;
    DtInt *parms;
    DtInt parms_size;
};

dot_object *doe_global_intermdisptype_object;
DtInt doe_global_intermdisptype_class_id;
/*
 ======================================================================
 */

static DtMethodEntry methods[] = {
    { DcMethodInqGlbAttVal, (DtMethodPtr) dor_global_intermdisptype_get_value }
};

void dor_global_intermdisptype_initialize (void)
{
    struct gas *gas;

    doe_global_intermdisptype_class_id = dor_class_add
                 ("AttIntermediateDispType", 1, methods, DcNullPtr);

    gas = dor_gas_create(dor_global_intermdisptype_destroy_element);

    doe_global_intermdisptype_object =
              dor_object_create(doe_global_intermdisptype_class_id,gas);

    dor_global_intermdisptype_push_value(DcDispIntermediateAll,DcNullPtr);
}
/*
 ======================================================================
 */

void dor_global_intermdisptype_set_value (
    DtIntermediateDispType type,
    DtInt *parms)
{
    struct global_intermdisptype_data *intermdisptypedata;
    DtInt i, new_parms_size;

    /*  If setting the value of the global attribute would result in an 
	implicit push (this would occur when the value is the first on 
	the current group stack), then a new interm disp type is 
	created and pushed on the stack.  Otherwise, the current interm disp
	type is loaded with the user specified value.  */

    if (dor_gas_test_set_value(doe_global_intermdisptype_object->data)){
	dor_global_intermdisptype_push_value(type,parms);
    }
    else{
	intermdisptypedata = dor_global_intermdisptype_get_data_pointer();

	if (intermdisptypedata ==
		 (struct global_intermdisptype_data *)DcNullPtr){
	    return;
	}

        if (type == DcDispRegularIntervals) 
            new_parms_size = 1;
        else if (type == DcDispIrregularIntervals) 
            new_parms_size = parms[0] + 1; 
        else
            new_parms_size = 0;

        if (intermdisptypedata->parms_size != new_parms_size) {
            if (new_parms_size != 0) {
                intermdisptypedata->parms =
                      (DtInt *)dor_space_reallocate(intermdisptypedata->parms,
                             new_parms_size*sizeof(DtInt));
            } else {
                dor_space_deallocate(intermdisptypedata->parms);
            }
       }

       for (i=0; i<new_parms_size; i++) 
           intermdisptypedata->parms[i] = parms[i];

       intermdisptypedata->type = type;
       intermdisptypedata->parms_size = new_parms_size;

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_intermdisptype_update_execute();
    }
}
/*
 ======================================================================
 */

void dor_global_intermdisptype_push_value (
    DtIntermediateDispType type,
    DtInt *parms)
{
    struct global_intermdisptype_data *newintermdisptypedata;
    struct object *currentgroup;
    DtInt i;

    /*  Create a new data structure and copy the values to
	be pushed into it.  */

    newintermdisptypedata = (struct global_intermdisptype_data *)
		dor_space_allocate(sizeof *newintermdisptypedata);

    newintermdisptypedata->type = type;


    if (type == DcDispRegularIntervals) 
        newintermdisptypedata->parms_size = 1;
    else if (type == DcDispIrregularIntervals) 
        newintermdisptypedata->parms_size = parms[0] + 1; 
    else
        newintermdisptypedata->parms_size = 0;

    newintermdisptypedata->parms = (DtInt *)dor_space_allocate(
          newintermdisptypedata->parms_size*(sizeof(DtInt)));

    for (i=0; i<newintermdisptypedata->parms_size; i++) 
        newintermdisptypedata->parms[i] = parms[i];

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_intermdisptype_object->data,newintermdisptypedata)){
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,dor_global_intermdisptype_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_intermdisptype_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_intermdisptype_pop_value (void)
{
    DtFlag popflag;
    DtPtr intermdisptypedata;

    popflag = dor_gas_pop_value(doe_global_intermdisptype_object->data,
				&intermdisptypedata);

    dor_global_intermdisptype_destroy_element(intermdisptypedata);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_intermdisptype_update_execute();

    return(popflag);
}
/*
 ======================================================================
 */

/*  Pop all of the attribute values stored for this global attribute for 
    the current group.  The gas subroutine will ask the attribute object 
    to destroy any instance data being popped.  */

void dor_global_intermdisptype_pop_group (void)
{
    dor_gas_pop_group(doe_global_intermdisptype_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_intermdisptype_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_intermdisptype_get_value (
    DtIntermediateDispType *type,
    DtInt *parms,
    DtInt max_size,
    DtInt *actual_size)
{
    struct global_intermdisptype_data *intermdisptypedata;
    DtFlag getflag;
    DtPtr  dataptr;
    DtInt  i, fillsize;

    getflag = dor_gas_get_value (doe_global_intermdisptype_object->data,
				 &dataptr);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_intermdisptype_get_value", DcNullPtr);

	return(DcFalse);
    }

    intermdisptypedata = dataptr;

    *type = intermdisptypedata->type;

    if (*type == DcDispRegularIntervals) 
        *actual_size = 1;
    else if (*type == DcDispIrregularIntervals) 
        *actual_size = intermdisptypedata->parms[0] + 1; 
    else {
        *actual_size = 0;
    }
    
    if (max_size < *actual_size) 
        fillsize = max_size;
    else
        fillsize = *actual_size;
       
    for (i=0; i<fillsize; i++) 
       parms[i] = intermdisptypedata->parms[i];

    return(DcTrue);
}
/*
 ======================================================================
 */

struct global_intermdisptype_data *dor_global_intermdisptype_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_intermdisptype_object->data,&dataptr);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_intermdisptype_get_data_pointer", DcNullPtr);

	return((struct global_intermdisptype_data *)DcNullPtr);
    }

    return((struct global_intermdisptype_data *)dataptr);
}
/*
 ======================================================================
 */

/*  The attribute (may have) changed value.  Execute the currently active 
    method (like rendering, rendering initialization, etc.).  */

void dor_global_intermdisptype_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_intermdisptype_object);

    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_intermdisptype_destroy_element (
    struct global_intermdisptype_data *intermdisptypedata)
{
    if (intermdisptypedata->parms != DcNullPtr)
        dor_space_deallocate(intermdisptypedata->parms);
    dor_space_deallocate(intermdisptypedata);
}
