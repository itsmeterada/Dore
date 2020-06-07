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
	int dor_frame_initialize ()
	dot_object * dor_frame_create ()
	int dor_frame_delete (frame_object)
	int dor_frame_add_reference (frame_object,reference_object)
	int dor_frame_remove_reference (frame_object,reference_object)
	int dor_frame_pick (frame_object,device_object)
	int dor_frame_print (frame_object)

  ======================================================================
 */
#include <dore/dore_develop/private/frame.h>
#include <dore/dore_develop/private/device.h>

dot_object *doe_frame_list ;
DtInt doe_frame_class_id;

/*
 ======================================================================
 */

static DtMethodEntry frame_methods[] = {
    { DcMethodAddReference,    (DtMethodPtr) dor_frame_add_reference },
    { DcMethodDestroy,         (DtMethodPtr) dor_frame_delete },
    { DcMethodPrint,           (DtMethodPtr) dor_frame_print },
    { DcMethodRemoveReference, (DtMethodPtr) dor_frame_remove_reference },
};

void dor_frame_initialize (void)
{
    doe_frame_list = DcNullPtr;

    doe_frame_class_id = dor_class_install (DcTypeFrame, "DoFrame", 
					4,frame_methods, DcNullPtr);

}
/*
 ======================================================================
 */

dot_object *dor_frame_create (void)
{
    dot_frame *frame;
    dot_object *frame_object;

    frame = ( dot_frame *)dor_space_allocate(sizeof(dot_frame));

    frame->left_justification = 0.5;
    frame->bottom_justification = 0.5;

    frame->boundary.fur[0] = 1.0;
    frame->boundary.fur[1] = 1.0;
    frame->boundary.fur[2] = 1.0;
    frame->boundary.bll[0] = 0.0;
    frame->boundary.bll[1] = 0.0;
    frame->boundary.bll[2] = 0.0;

    frame->references = (dot_object_reference *)NULL;

    frame->view_group = dor_group_create(DcFalse,DcFalse);

    frame_object = dor_object_create(doe_frame_class_id,frame);

    if (frame_object != DcNullPtr){
	frame->next = doe_frame_list;

	doe_frame_list = frame_object;
    }

    /*  The view group for this frame is referenced by the frame */

    dor_object_add_reference(frame->view_group,frame_object);

    frame->modification_sequence_number = 0;

    return(frame_object);
}
/*
 ======================================================================
 */

void dor_frame_delete (dot_object *frame_object)
{
    dot_frame *frame = (dot_frame *)frame_object->data;
    dot_object *previous_frame,*current_frame;

    /*  Remove all posted views.  */

    dor_group_empty(frame->view_group);

    /*  Detach from list of defined frames.  */

    previous_frame = (dot_object *)NULL;
    current_frame = doe_frame_list;

    while (current_frame){
	if (current_frame == frame_object){
	    if ( previous_frame ){
		((dot_frame *)(previous_frame->data))->next =
			((dot_frame *)(current_frame->data))->next;
	    } else{
		doe_frame_list = ((dot_frame *)(current_frame->data))->next;
	    }
			
	    break;
	}

	previous_frame = current_frame;
	current_frame = ((dot_frame *)(current_frame->data))->next;
    }
	
    dor_space_deallocate(frame);
}
/*
 ======================================================================
 */

void dor_frame_add_reference (
    dot_object *frame_object,
    dot_object *reference_object)
{
    dot_frame *frame;

    frame = (dot_frame *)frame_object->data;

    dor_object_reference_insert(&(frame->references),reference_object);
}
/*
 ======================================================================
 */

void dor_frame_remove_reference (
    dot_object *frame_object,
    dot_object *reference_object)
{
    dot_frame *frame = (dot_frame *)frame_object->data;

    dor_object_reference_remove(&(frame->references),reference_object);
}
/*
 ======================================================================
 */

void dor_frame_pick (
    dot_object *frame_object,
    dot_object *device_object)
{
    dot_frame *frame = (dot_frame *)frame_object->data;
    dot_object *view_group = frame->view_group;
    DtInt group_size,i;
    dot_object *element;
    DtInt view_class_id = dor_class_inq_id ("DoView");

    group_size = dor_group_inq_size(view_group);

    for (i=0;i<group_size;i++){
	element = dor_group_inquire_object_at_position(view_group,i,DcBeginning);

	if (view_class_id == dor_object_inq_type(element)){
	    dor_device_pick_view(device_object,frame_object,element);
	}
    }
}
/*
 ======================================================================
 */

void dor_frame_print (dot_object *frame_object)
{
    dot_frame *frame = (dot_frame *)frame_object->data;

    sprintf(dor_print_get_line(),
	    "justification (left,bottom) = (%f,%f)",
	    frame->left_justification,frame->bottom_justification);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "boundary (bll,fur) = ((%f,%f,%f),(%f,%f,%f))",
	    frame->boundary.bll[0],frame->boundary.bll[1],frame->boundary.bll[2],
	    frame->boundary.fur[0],frame->boundary.fur[1],frame->boundary.fur[2]);
    dor_print_output(dor_print_get_line());
}
