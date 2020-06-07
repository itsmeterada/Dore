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
	dor_global_depcue_initialize 
	dor_global_depcue_set_value 
	dor_global_depcue_push_value 
	dor_global_depcue_pop_value 
	dor_global_depcue_pop_group 
	dor_global_depcue_get_value 
	dor_global_depcue_get_data_pointer 
	dor_global_depcue_update_execute 
	dor_global_depcue_destroy_element 

  ======================================================================
 */
#include "global.h"

struct global_depcue_data {
    DtReal zfront;
    DtReal zback;
    DtReal sfront;
    DtReal sback;
    DtColorModel background_colormodel;
    DtColorRGB background_color;
};

dot_object *doe_global_depcue_object;
DtInt doe_global_depcue_class_id;
/*
 ======================================================================
 */

static DtMethodEntry methods[] = {
    { DcMethodInqGlbAttVal, (DtMethodPtr) dor_global_depcue_get_value }
};

void dor_global_depcue_initialize (void)
{
    struct gas *gas;
    DtColorRGB initialbackgroundcolor;

    doe_global_depcue_class_id = dor_class_install (DcTypeAttDepthCue, 
		"AttDepthCue", 1,methods, DcNullPtr);

    gas = dor_gas_create(dor_global_depcue_destroy_element);

    doe_global_depcue_object = dor_object_create(doe_global_depcue_class_id,gas);

    initialbackgroundcolor[0] = 0.;
    initialbackgroundcolor[1] = 0.;
    initialbackgroundcolor[2] = 0.;

    dor_global_depcue_push_value(1.,0.,1.,0.,DcRGB,initialbackgroundcolor);
}
/*
 ======================================================================
 */

void dor_global_depcue_set_value (
    DtReal       zfront,
    DtReal       zback,
    DtReal       sfront,
    DtReal       sback,
    DtColorModel colormodel,
    DtReal       color[])
{
    struct global_depcue_data *depcuedata;

    /*  If setting the value of the global attribute would result in 
	an implicit push (this would occur when the value is the first 
	on the current group stack), then new depth cueing is created 
	and pushed on the stack.  Otherwise, the current depth cueing
	 is loaded with the user specified depth cueing.  */

    if (dor_gas_test_set_value(doe_global_depcue_object->data)){
	dor_global_depcue_push_value(zfront,zback,sfront,sback,colormodel,color);
    }
    else{
	depcuedata = dor_global_depcue_get_data_pointer();

	if (depcuedata == (struct global_depcue_data *)DcNullPtr){
	    return;
	}

	depcuedata->zfront = zfront;
	depcuedata->zback = zback;
	depcuedata->sfront = sfront;
	depcuedata->sback = sback;

	depcuedata->background_colormodel = colormodel;

	depcuedata->background_color[0] = color[0];
	depcuedata->background_color[1] = color[1];
	depcuedata->background_color[2] = color[2];

	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_depcue_update_execute();
    }
}
/*
 ======================================================================
 */

void dor_global_depcue_push_value (
    DtReal       zfront,
    DtReal       zback,
    DtReal       sfront,
    DtReal       sback,
    DtColorModel colormodel,
    DtReal       color[])
{
    struct global_depcue_data *newdepcuedata;
    struct object *currentgroup;

    /*  Create a new depth cue data structure and copy the values to
	be pushed into it.  */

    newdepcuedata = (struct global_depcue_data *)dor_space_allocate(sizeof *newdepcuedata);

    newdepcuedata->zfront = zfront;
    newdepcuedata->zback = zback;
    newdepcuedata->sfront = sfront;
    newdepcuedata->sback = sback;

    newdepcuedata->background_colormodel = colormodel;

    newdepcuedata->background_color[0] = color[0];
    newdepcuedata->background_color[1] = color[1];
    newdepcuedata->background_color[2] = color[2];

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_depcue_object->data,newdepcuedata)){
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,dor_global_depcue_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_depcue_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_depcue_pop_value (void)
{
    DtFlag popflag;
    DtPtr depcuedata;

    popflag = dor_gas_pop_value(doe_global_depcue_object->data,&depcuedata);

    dor_global_depcue_destroy_element(depcuedata);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_depcue_update_execute();

    return(popflag);
}
/*
 ======================================================================
 */

/* Pop all of the attribute values stored for this global attribute for 
   the current group.  The gas subroutine will ask the attribute object 
   to destroy any instance data being popped.  */

void dor_global_depcue_pop_group (void)
{
    dor_gas_pop_group(doe_global_depcue_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_depcue_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_depcue_get_value (
    DtReal       *zfront,
    DtReal       *zback,
    DtReal       *sfront,
    DtReal       *sback,
    DtColorModel *colormodel,
    DtReal        color[3])
{
    DtFlag getflag;
    DtPtr  dataptr;
    struct global_depcue_data *depcuedata;

    getflag = dor_gas_get_value (doe_global_depcue_object->data, &dataptr);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE, "dor_global_depcue_get_value",DcNullPtr);
	return DcFalse;
    }

    depcuedata = dataptr;

    *zfront = depcuedata->zfront;
    *zback  = depcuedata->zback;
    *sfront = depcuedata->sfront;
    *sback  = depcuedata->sback;

    *colormodel = depcuedata->background_colormodel;

    color[0] = depcuedata->background_color[0];
    color[1] = depcuedata->background_color[1];
    color[2] = depcuedata->background_color[2];

    return DcTrue;
}
/*
 ======================================================================
 */

struct global_depcue_data *dor_global_depcue_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_depcue_object->data,&dataptr);

    if (!getflag){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_depcue_get_data_pointer", DcNullPtr);

	return((struct global_depcue_data *)DcNullPtr);
    }

    return((struct global_depcue_data *)dataptr);
}
/*
 ======================================================================
 */

/* The attribute (may have) changed value.  Execute the currently active 
   method (like rendering, rendering initialization, etc.).  */

void dor_global_depcue_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_depcue_object);

    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_depcue_destroy_element (struct global_depcue_data *depcuedata)
{
    dor_space_deallocate(depcuedata);
}
