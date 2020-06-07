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
	dor_global_lgtswi_initialize 
	dor_global_lgtswi_set_value 
	dor_global_lgtswi_push_value 
	dor_global_lgtswi_pop_value 
	dor_global_lgtswi_pop_group 
	dor_global_lgtswi_get_value 
	dor_global_lgtswi_get_data_pointer 
	dor_global_lgtswi_update_execute 
	dor_global_lgtswi_destroy_element 

  ======================================================================
 */
#include "global.h"


    /***  Data Structures  ***/

/* list of light objects that are currently turned off */

struct global_lgtswi_data {
    DtInt  offcount;
    DtPtr *offlist;
};

dot_object *doe_global_lgtswi_object;
DtInt doe_global_lgtswi_class_id;

static DtMethodEntry methods[] = {
    { DcMethodInqGlbAttVal, (DtMethodPtr)dor_global_lgtswi_get_value }
};


    /***  Function Declarations  ***/

static struct global_lgtswi_data *dor_global_lgtswi_get_data_pointer (void);



/* ====================================================================== */

void dor_global_lgtswi_initialize (void)
{
    struct gas *gas;

    doe_global_lgtswi_class_id = 
	    dor_class_add ("AttLightSwitch", 1, methods, DcNullPtr);


    gas = dor_gas_create(dor_global_lgtswi_destroy_element);

    doe_global_lgtswi_object = 
	    dor_object_create(doe_global_lgtswi_class_id,gas);

    dor_global_lgtswi_push_value (0, DcNullPtr);
}
/*
 ======================================================================
 */

void dor_global_lgtswi_set_value (
    DtObject light,
    DtSwitch switchvalue)
{
    struct global_lgtswi_data *lgtswidata;
    int found;
    DtPtr pushlist[50];
    DtInt pushcount;
    int i;
   
    /*  If setting the value of the global attribute would
	result in an implicit push (this would occur when the
	value is the first on the current group stack), then a new
	switch is created and pushed on the stack.  Otherwise, the
	current switch is loaded with the user specified switch.  */

    lgtswidata = dor_global_lgtswi_get_data_pointer();
    if (lgtswidata == (struct global_lgtswi_data *)DcNullPtr) {
	return;
    }

    found = -1;
    for (i=0; i<lgtswidata->offcount; i++) {
	if (lgtswidata->offlist[i] == light) {
	    found = i;
	    break;
	}
    }

    if (dor_gas_test_set_value(doe_global_lgtswi_object->data)) {

	if ((switchvalue == DcOff) && (found < 0)) {

	    /* pushed list is the same as the current list + light */
	    for (i=0; i<lgtswidata->offcount; i++) {
		pushlist[i] = lgtswidata->offlist[i];
	    }
	    pushcount = lgtswidata->offcount + 1;
	    pushlist[pushcount-1] = light;

        } else if ((switchvalue == DcOn) && (found >= 0)) {
 
	    /* pushed list is the same as the current list - light */
	    for (i=0; i<found; i++) {
		pushlist[i] = lgtswidata->offlist[i];
	    }
	    for (i=found+1; i<lgtswidata->offcount; i++) {
		pushlist[i-1] = lgtswidata->offlist[i];
	    }
	    pushcount = lgtswidata->offcount - 1;

        } else {		/* list has not changed, but still need to push
                           to keep group mechanism happy */

		for (i=0; i<lgtswidata->offcount; i++) {
                       pushlist[i] = lgtswidata->offlist[i];
        	}
		pushcount = lgtswidata->offcount;
	}

	dor_global_lgtswi_push_value (pushcount, pushlist);

   } else {	/* don't need to push */

	if ((switchvalue == DcOff) && (found < 0)) {

		/* add light to current list */
	
		lgtswidata->offcount++;
		lgtswidata->offlist = dor_space_reallocate(
				lgtswidata->offlist,
				lgtswidata->offcount*sizeof (char*));
		lgtswidata->offlist[lgtswidata->offcount-1] = light;

	} else if ((switchvalue == DcOn) && (found >= 0)) {

		/* delete light from current list */

		lgtswidata->offcount--;
		for (i=found; i<lgtswidata->offcount; i++) {
			lgtswidata->offlist[i] = lgtswidata->offlist[i+1];
		}
	}

	/*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

	dor_global_lgtswi_update_execute();
   }

}

/* ====================================================================== */

void dor_global_lgtswi_push_value (
    DtInt  count,
    DtPtr *list)
{
    struct global_lgtswi_data *newlgtswidata;
    struct object *currentgroup;
    int i;

    /*  Create a new light switch data structure and copy the values to
	be pushed into it.  */

    newlgtswidata = (struct global_lgtswi_data *)
		dor_space_allocate(sizeof *newlgtswidata);

    newlgtswidata->offcount = count;

    if (count>0) {
	newlgtswidata->offlist = dor_space_allocate (count * sizeof(char*));
	for (i=0; i<count; i++) {
	    newlgtswidata->offlist[i] = list[i];
	}
    } else {
	newlgtswidata->offlist = DcNullPtr;
    }

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_lgtswi_object->data,newlgtswidata)) {
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,dor_global_lgtswi_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_lgtswi_update_execute();
}



/* ====================================================================== */

DtFlag dor_global_lgtswi_pop_value (void)
{
    DtFlag popflag;
    struct global_lgtswi_data *lgtswidata;

    popflag = dor_gas_pop_value (doe_global_lgtswi_object->data,
				 (DtPtr)(&lgtswidata));

    dor_global_lgtswi_destroy_element (lgtswidata);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_lgtswi_update_execute();

    return(popflag);
}



/* ======================================================================
**  Pop all of the attribute values stored for this global attribute for 
**  the current group.  The gas subroutine will ask the attribute object 
**  to destroy any instance data being popped.
** ====================================================================== */

void dor_global_lgtswi_pop_group (void)
{
    dor_gas_pop_group(doe_global_lgtswi_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_lgtswi_update_execute();
}



/* ====================================================================== */

DtFlag dor_global_lgtswi_get_value (
    DtInt *count,
    DtPtr *list,
    DtInt  listsize)	/* number of elements list can hold */
{
    DtFlag getflag;
    struct global_lgtswi_data *lgtswidata;
    int i;

    getflag = dor_gas_get_value(doe_global_lgtswi_object->data,
				(DtPtr)(&lgtswidata));

    if (!getflag) {
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_lgtswi_get_value", DcNullPtr);

	return(DcFalse);
    }

    *count = lgtswidata->offcount;

    if (listsize<lgtswidata->offcount) {
	for (i=0; i<listsize; i++)
		list[i] = lgtswidata->offlist[i];
    } else {
	for (i=0; i<lgtswidata->offcount; i++)
		list[i] = lgtswidata->offlist[i];
    }

    return(DcTrue);
}
/*
 ======================================================================
 */

static struct global_lgtswi_data *dor_global_lgtswi_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_lgtswi_object->data,&dataptr);

    if (!getflag) {
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_lgtswi_get_data_pointer", DcNullPtr);
	return DcNullPtr;
    }

    return dataptr;
}
/*
 ======================================================================
 */

/*  The attribute (may have) changed value.  Execute the currently active 
    method (like rendering, rendering initialization, etc.).  */

void dor_global_lgtswi_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_lgtswi_object);

    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_lgtswi_destroy_element (struct global_lgtswi_data *lgtswidata)
{
    dor_space_deallocate (lgtswidata->offlist);
    dor_space_deallocate (lgtswidata);
}
