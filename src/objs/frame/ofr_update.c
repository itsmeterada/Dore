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
 
/* ======================================================================
  Functions:
	dor_frame_update
	dor_frame_update_device
	dor_frame_update_view
	dor_frame_update_notify

======================================================================== */

#include <dore/internal/system.h>
#include <dore/dore_develop/private/frame.h>
#include <dore/dore_develop/private/device.h>

/* ====================================================================== */

void dor_frame_update (dot_object *frame_object)
{
    dot_frame *frame;
    dot_object_reference *frmrefs;
    static DtInt device_class_id = -1;

    if (device_class_id == -1) {
	device_class_id = dor_class_inq_id ("DoDevice");
    }

#   ifdef ERRCHK
	if (!dor_object_confirm_type(frame_object,doe_frame_class_id)) {
	    DDerror (ERR_INVALID_FRAME, "DfUpdate", DcNullPtr);
	    return;
	}
#   endif

    /*
     * The variable doe_system_active_traversals is used to indicate
     * that a function is active that requires traversals of the
     * data base.  These functions include:
     *   DdUpdate		subsys/device/upddev.c
     *   DdPick		subsys/device/pck.c
     *   DfUpdate		subsys/frame/updfrm.c
     *   DgCheck		subsys/group/chkgrp.c
     *   DsCompBoundingVol	subsys/system/compbndvol.c
     *   DvUpdate		subsys/view/updviw.c
     * Multiple active traversals are not possible because
     * the state of a traversal is maintained in the global 
     * attributes.  This state would need to be saved and 
     * restored if traversals were allowed to be re-entrant.
     * Further each of the above sub-systems also maintains state
     * during traversal and that state would likewise need to be 
     * saved and restored.
     */

    if (doe_system_active_traversals) {
	DDerror (ERR_MULTIPLE_TRAVERSAL, "DfUpdate", DcNullPtr);
	return;
    }

    /*
     * NOTE: doe_system_active_traversals is not incremented here
     * because it will be incremented by the function 
     * dor_device_update.  Incrementing it here would prevent
     * the device update from occurring.
     */

    frame = (dot_frame *)frame_object->data;

    for (frmrefs=frame->references;frmrefs!=NULL;frmrefs=frmrefs->next) {
	if (dor_object_inq_type(frmrefs->object) == device_class_id) {
	    dor_device_update(frmrefs->object);
	}
    }
}

/* ====================================================================== */

void dor_frame_update_device (
    dot_object *frame_object,
    dot_object *device_object)
{
    dot_frame *frame = (dot_frame *)frame_object->data;
    dot_object *view_group = frame->view_group;
    DtInt group_size, i;
    dot_object *element;
    DtInt view_class_id = dor_class_inq_id("DoView");

    group_size = dor_group_inq_size(view_group);

    for (i=0;i<group_size;i++){
	element = dor_group_inquire_object_at_position(view_group,i,DcBeginning);

	if (view_class_id == dor_object_inq_type(element)){
	    dor_device_update_view(device_object,frame_object,element);
	}
    }
}

/* ====================================================================== */

void dor_frame_update_view (
    dot_object *frame_object,
    dot_object *view_object)
{
    dot_frame *frame = (dot_frame *)frame_object->data;
    dot_object_reference *frmrefs;
    DtUInt count;
    DtReal *rects;
    DtReal extent[4];
    DtFlag partobscured;
    DtInt device_class_id = dor_class_inq_id("DoDevice");

    for (frmrefs=frame->references;  frmrefs != NULL;  frmrefs=frmrefs->next) {
	if (dor_object_inq_type(frmrefs->object) != device_class_id)
	    continue;

	dor_device_set_current_view(frmrefs->object, view_object);

	do {
	    dor_device_update_view (frmrefs->object, frame_object, view_object);
	    dor_device_inquire_clip_list
		(frmrefs->object, &count, &rects, extent, &partobscured);
	    dor_device_swap_buffers (frmrefs->object);
	} while (dor_device_get_aborted_draw_flag(frmrefs->object) && count);
    }
}

/* ====================================================================== */

void dor_frame_update_notify (dot_object *frame_object)
{
    dot_frame *frame;
    dot_object_reference *references;
    DtInt device_class_id = dor_class_inq_id ("DoDevice");

    frame = (dot_frame *)frame_object->data;

    references = frame->references;

    while (references) {
	if (dor_object_inq_type(references->object) == device_class_id) {
	    dor_device_update_notify(references->object);
	}
	references = references->next;
    }

    frame->modification_sequence_number++;
}
