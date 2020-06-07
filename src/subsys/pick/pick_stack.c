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
	int dor_pick_initialize_element ()
	int dor_pick_pop_group ()
	int dor_pick_push_group (group_id)
	DtInt dor_pick_record_hit (hit_size)
	int dor_pick_terminate_element ()

  ======================================================================
 */
#include "pick.h"

/* ====================================================================== */

void dor_pick_initialize_element (DtFlag from_exec)
{
    doe_pick_data.hits_on_element = 0;

    doe_pick_data.front_z_of_hit = -1e10;	/* impossibly small */

    /* increment the element count */

    if (!from_exec) 
	doe_pick_data.stack_top[2]++;
}

/* ====================================================================== */

void dor_pick_pop_group (void)
{
    doe_pick_data.stack_top -= 3;
}

/* ====================================================================== */

void dor_pick_push_group (dot_object *group_id)
{
    DtInt stack_height;
    DtInt new_height;
    DtInt current_pick_id;

    /* Advance by one element. */

    doe_pick_data.stack_top += 3;

    stack_height = doe_pick_data.stack_top - doe_pick_data.stack_base + 3;

    /* need bigger stack */

    if (stack_height == doe_pick_data.stack_words) {
	new_height = stack_height + STACK_GROW_SIZE;

	doe_pick_data.stack_base =
	    dor_space_reallocate (doe_pick_data.stack_base,
				  new_height
				  * sizeof (*doe_pick_data.stack_base));

	doe_pick_data.stack_top = doe_pick_data.stack_base + new_height;

	doe_pick_data.stack_words = new_height;
    }
    else if (stack_height > doe_pick_data.stack_words){
	DDerror (ERR_FATAL_INTERNAL_ERROR,
		 "dor_pick_push_group", "pick stack overflow");
	return;
    }

    /* The following line of code coerces the group_id pointer into a long
    ** integer value.  This is not portable, but should work on most platforms.
    ** The previous version of this code used DtInt values, which would not
    ** work on the DEC Alpha (64-bit) platforms.  */

    doe_pick_data.stack_top[0] = (long)(group_id);

    dor_global_pckid_get_value (&current_pick_id);

    doe_pick_data.stack_top[1] = current_pick_id;
    doe_pick_data.stack_top[2] = 0;
}

/* ====================================================================== */

DtInt dor_pick_record_hit (DtInt hit_size)
{
    DtInt           space_used;
    DtInt          *hit_count;
    DtInt           ii, jj;
    DtPickPathOrder path_order;

    hit_count  = doe_pick_data.hit_count;

    if (doe_pick_data.index_size - 1 <= *hit_count){
	*doe_pick_data.error_word |= DcPickIndexOverflow;
	return 1;
    }

    space_used = doe_pick_data.next_hit - doe_pick_data.hit_list;

    if (doe_pick_data.list_size < space_used + hit_size){
	*doe_pick_data.error_word |= DcPickListOverflow;
	return 1;
    }

    /* record z value if user provided space */

    if (doe_pick_data.z_values != NULL) {
	doe_pick_data.z_values[*hit_count] = doe_pick_data.front_z_of_hit;
    }
		
    /* record wcs point if user provided space */

    if (doe_pick_data.wcs_points != NULL){
	doe_pick_data.wcs_points[3*(*hit_count)+0] = doe_pick_data.wcspoint.x;
	doe_pick_data.wcs_points[3*(*hit_count)+1] = doe_pick_data.wcspoint.y;
	doe_pick_data.wcs_points[3*(*hit_count)+2] = doe_pick_data.wcspoint.z;
    }
		
    /* record lcs point if user provided space */

    if (doe_pick_data.lcs_points != NULL){
	doe_pick_data.lcs_points[3*(*hit_count)+0] = doe_pick_data.lcspoint.x;
	doe_pick_data.lcs_points[3*(*hit_count)+1] = doe_pick_data.lcspoint.y;
	doe_pick_data.lcs_points[3*(*hit_count)+2] = doe_pick_data.lcspoint.z;
    }
		
    /* record view in which hit was found if user provided space */

    if (doe_pick_data.views != NULL){
	doe_pick_data.views[*hit_count] = doe_pick_data.view;
    }
		
    /* record index into user's hit list where this hit starts */

    doe_pick_data.index[*hit_count]
	= doe_pick_data.next_hit - doe_pick_data.hit_list;

    /* record index into user's hit list of element after end of hit */

    doe_pick_data.index[*hit_count + 1]
	= doe_pick_data.index[*hit_count] + hit_size;

    path_order = dor_device_inq_pick_path_order(doe_pick_data.device);

    if (path_order == DcTopFirst) {
	for(ii=0; ii<hit_size; ii++){
	    doe_pick_data.next_hit[ii] = doe_pick_data.stack_base[ii];
	}
    }
    else{
	for(ii=0, jj=hit_size-3; ii<hit_size; ii+=3, jj-=3){
	    doe_pick_data.next_hit[ii+0] = doe_pick_data.stack_base[jj+0];
	    doe_pick_data.next_hit[ii+1] = doe_pick_data.stack_base[jj+1];
	    doe_pick_data.next_hit[ii+2] = doe_pick_data.stack_base[jj+2];
	}
    }

    doe_pick_data.last_hit = doe_pick_data.next_hit;

    doe_pick_data.next_hit += hit_size;

    return 0;
}

/* ====================================================================== */

void dor_pick_terminate_element (void)
{
    DtInt path_size;
    DtReal near_z;
    DtHitStatus status;
    DtHitStatus (*user_function)();
    DtInt *hit_count;
    DtInt previous_hit;
    DtObject user_data;
    DtInt current_pick_id;
    DtPtr datptr;
    Dt32Bits datval;
    DtInt type;
    DtInt path_elements;
    DtInt dataptr_class_id = dor_class_inq_id ("DoDataPtr");
    DtInt dataval_class_id = dor_class_inq_id ("DoDataVal");

    if (!doe_pick_data.hits_on_element) return;

    /* else we found a hit ! */

    doe_pick_data.stack_top[3] = (long)(doe_pick_data.current_element);

    dor_global_pckid_get_value(&current_pick_id);

    doe_pick_data.stack_top[4] = current_pick_id;

    /* record the hit just once */

    doe_pick_data.hits_on_element = 0;
	
    path_size = doe_pick_data.stack_top - doe_pick_data.stack_base +6;

    path_elements = path_size / 3;

    near_z = doe_pick_data.front_z_of_hit;

    hit_count = doe_pick_data.hit_count;

    user_function = doe_pick_data.callback_function;

    user_data = doe_pick_data.callback_data;

    if (user_data == DcNullObject) {
	datval = 0;
	if (doe_pick_data.pass_by_ref) {
	    status = (*user_function)(&datval,
				      &path_elements,
				      doe_pick_data.stack_base,
				      &near_z,
				      &doe_pick_data.view,
				      hit_count);
	} else {
	    status = (*user_function)(datval,
				      path_elements,
				      doe_pick_data.stack_base,
				      near_z,
				      doe_pick_data.view,
				      *hit_count);
	}
    } else if ((DcObjectValid == dor_object_validate(user_data)) &&
	       (((type = dor_object_inq_type(user_data)) == dataptr_class_id) ||
		(type == dataval_class_id))) {
	if (type == dataptr_class_id) {
	    datptr = dor_datptr_get_value(user_data);
	    if (doe_pick_data.pass_by_ref) {
		status = (*user_function)(&datptr,
					  &path_elements,
					  doe_pick_data.stack_base,
					  &near_z,
					  &doe_pick_data.view,
					  hit_count);
	    } else {
		status = (*user_function)(datptr,
					  path_elements,
					  doe_pick_data.stack_base,
					  near_z,
					  doe_pick_data.view,
					  *hit_count);
	    }
	} else if (type == dataval_class_id) {
	    datval = dor_datval_get_value(user_data);
	    if (doe_pick_data.pass_by_ref) {
		status = (*user_function)(&datval,
					  &path_elements,
					  doe_pick_data.stack_base,
					  &near_z,
					  &doe_pick_data.view,
					  hit_count);
	    } else {
		status = (*user_function)(datval,
					  path_elements,
					  doe_pick_data.stack_base,
					  near_z,
					  doe_pick_data.view,
					  *hit_count);
	    }
	}
    } else {
	DDerror(ERR_INVALID_DATAOBJECT, 
		   "dor_pick_terminate_element",
		   DcNullPtr);
	return;
    }


    switch(status) {
    case DcHitAccept:
	if (!dor_pick_record_hit(path_size))
		(*hit_count)++;
	return;

    case DcHitReject:
	return;

    case DcHitOverwrite:
	doe_pick_data.next_hit = doe_pick_data.last_hit;
	previous_hit = DcFalse;
	if (*hit_count > 0) {
	    --(*hit_count);
	    previous_hit = DcTrue;
	}
	if (dor_pick_record_hit(path_size)) {
	    if (previous_hit)
		    (*hit_count)++;
	    dor_group_traversal_abort();
	    return;
	}
	(*hit_count)++;
	return;

    default:
	DDerror (ERR_INVALID_PCK_CB_RTN_STATUS,
		    "dor_pick_terminate_element", 
		    DcNullPtr);
	*doe_pick_data.error_word |= DcPickBadStatus;
	dor_group_traversal_abort();
	return;
    }
}
