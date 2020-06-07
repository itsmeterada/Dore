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
	int dor_frame_inq_boundary (frame_object,boundary)
	DtVolume * dor_frame_inq_boundary_ptr (frame_object)
	int dor_frame_inq_justification (frame_object,left,bottom)
	dot_object * dor_frame_inq_view_group (frame_object)
	int dor_frame_set_boundary (frame_object,boundary)
	int dor_frame_set_justification (frame_object,left,bottom)

  ======================================================================
 */
#include <dore/dore_develop/private/frame.h>

/*
 ======================================================================
 */

void dor_frame_inq_boundary (
    dot_object *frame_object,
    DtVolume   *boundary)
{
    dot_frame *frame;

#ifdef ERRCHK
    if (!dor_object_confirm_type(frame_object,doe_frame_class_id)){
	DDerror(ERR_INVALID_FRAME,
		     "DfInqBoundary",DcNullPtr);
	return;
    }
#endif

    frame = (dot_frame *)frame_object->data;

    *boundary = frame->boundary;
}
/*
 ======================================================================
 */

DtVolume *dor_frame_inq_boundary_ptr (dot_object *frame_object)
{
    dot_frame *frame;

    frame = (dot_frame *)frame_object->data;

    return(&frame->boundary);
}
/*
 ======================================================================
 */

void dor_frame_inq_justification (
    dot_object *frame_object,
    DtReal     *left,
    DtReal     *bottom)
{
    dot_frame *frame;

#ifdef ERRCHK
    if (!dor_object_confirm_type(frame_object,doe_frame_class_id)){
	DDerror(ERR_INVALID_FRAME,
		     "DfInqJust",DcNullPtr);
	return;
    }
#endif

    frame = (dot_frame *)frame_object->data;

    *left = frame->left_justification;
    *bottom = frame->bottom_justification;
}
/*
 ======================================================================
 */

dot_object *dor_frame_inq_view_group (dot_object *frame_object)
{
    dot_frame *frame;

#ifdef ERRCHK
    if (!dor_object_confirm_type(frame_object,doe_frame_class_id)){
	DDerror(ERR_INVALID_FRAME,
		     "DfInqViewGroup",DcNullPtr);
	return(DcNullObject);
    }
#endif

    frame = (dot_frame *)frame_object->data;

    return(frame->view_group);
}
/*
 ======================================================================
 */

void dor_frame_set_boundary (dot_object *frame_object, DtVolume *boundary)
{
    dot_frame *frame;

#ifdef ERRCHK
    if (!dor_object_confirm_type(frame_object,doe_frame_class_id)){
	DDerror(ERR_INVALID_FRAME,
		     "DfSetBoundary",DcNullPtr);
	return;
    }
#endif
	
    frame = (dot_frame *)frame_object->data;
	
    frame->boundary = *boundary;

    dor_frame_update_notify(frame_object);
}
/*
 ======================================================================
 */

void dor_frame_set_justification (
    dot_object *frame_object,
    DtReal left,
    DtReal bottom)
{
    dot_frame *frame;

#ifdef ERRCHK
    if (!dor_object_confirm_type(frame_object,doe_frame_class_id)){
	DDerror(ERR_INVALID_FRAME,
		     "DfSetJust",DcNullPtr);
	return;
    }

    if ((0.0 > left) || (left > 1.0) || (0.0 > bottom) || (bottom > 1.0)){
	sprintf(dor_error_get_line(),"(%f,%f)",left,bottom);
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		     "DfSetJust",dor_error_get_line());
	return;
    }
#endif

    frame = (dot_frame *)frame_object->data;

    frame->left_justification = left;
    frame->bottom_justification = bottom;

    dor_frame_update_notify(frame_object);
}
