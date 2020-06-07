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
	dor_device_update_all
	dor_device_update
	dor_device_update_view
	dor_device_update_window
	dor_device_update_notify
	dor_device_swap_buffers

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/device.h>

/* ====================================================================== */

void dor_device_update_all (void)
{
    dot_object *device_object = doe_device_list;

    while (device_object) {
	dor_device_update (device_object);
	device_object = ((dot_device *)(device_object->data))->next;
    }
}

/* ====================================================================== */

void dor_device_update (dot_object *device_object)
{
    dot_device *device;
    dot_object *frame_object;
    DtUInt count;
    DtReal *rects;
    DtReal extent[4];
    DtFlag partobscured;

#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object,doe_device_class_id)){
	DDerror (ERR_INVALID_DEVICE, "DdUpdate", DcNullPtr);
	return;
    }
#endif

    /*
     * The variable doe_system_active_traversals is used to indicate
     * that a function is active that requires traversals of the
     * data base.  These functions include:
     *   DdUpdate		
     *   DdPick		
     *   DfUpdate		
     *   DgCheck	
     *   DsCompBoundingVol	
     *   DvUpdate	
     * Multiple active traversals are not possible because
     * the state of a traversal is maintained in the global 
     * attributes.  This state would need to be saved and 
     * restored if traversals were allowed to be re-entrant.
     * Further each of the above sub-systems also maintains state
     * during traversal and that state would likewise need to be 
     * saved and restored.
     */

    if (doe_system_active_traversals){
	DDerror (ERR_MULTIPLE_TRAVERSAL, "DdUpdate", DcNullPtr);
	return;
    }

    device = (dot_device *)(device_object->data);

    frame_object = device->frame;

    if (frame_object == DcNullObject) {
	return;
    }

    doe_system_active_traversals++;

    /*  Tell the device subsystem what the current device and 
    **  device driver are.  */

    SET_CURRENT_DEVICE (device_object);

    dor_device_set_current_view (device_object, DcNullObject);

    do
    {   dor_frame_update_device (frame_object, device_object);
	OUTFUN(inquire_clip_list) (&count, &rects, extent, &partobscured);
	OUTFUN(swap_buffers) ();
    } while (device->aborted_draw_flag && count != 0);

    --doe_system_active_traversals;
}

/* ====================================================================== */

void dor_device_update_view (
    dot_object *device_object,
    dot_object *frame_object,
    dot_object *view_object)
{
    dor_render_render(device_object,view_object);
}

/* ====================================================================== */

void dor_device_update_window (
    dot_object *device_object,
    dot_object *view_object)
{
    dot_device *device = (dot_device *)(device_object->data);

    SET_CURRENT_DEVICE (device_object);

    if (device->auto_size){
	dor_device_viewport_size (device_object);
    }

    dor_device_compute_frame_trans (device);
    dor_device_compute_clip_volume (device, view_object);
    dor_device_compute_actual_viewport (device);
}

/* ====================================================================== */

void dor_device_update_notify (dot_object *device_object)
{
    dot_device *device = (dot_device *)device_object->data;

    device->modification_sequence_number++;
}

/* ====================================================================== */

void dor_device_swap_buffers (dot_object *device_object)
{
    SET_CURRENT_DEVICE (device_object);
    OUTFUN(swap_buffers) ();
}
