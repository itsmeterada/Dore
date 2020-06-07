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
	dor_render_initialize
	dor_render_add_renderer
	dor_render_print
	dor_render_render

  ======================================================================
 */

#include <string.h>
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/device.h>
#include "render.h"
/*
 ======================================================================
 */
Dt32Bits		doe_render_current_renderer_id;

dot_object *		doe_render_current_device;
dot_object *		doe_render_current_view;

Dt32Bits		doe_render_next_renderer_id;
dot_render_data *	doe_render_renderer;
/*
 ======================================================================
 */
void dor_render_initialize (void)
{
    Dt32Bits device_id = dor_class_inq_id ("DoDevice");
    Dt32Bits view_id = dor_class_inq_id ("DoView");

    doe_render_current_renderer_id = -1;
    doe_render_current_device = DcNullPtr;
    doe_render_current_view = DcNullPtr;
    doe_render_renderer = (dot_render_data *)
	    dor_space_allocate(3*sizeof(dot_render_data));
    doe_render_next_renderer_id = 3;

    doe_render_renderer[0].name = DcNullPtr;
    doe_render_renderer[0].render_rtn = dor_object_method_null;
    doe_render_renderer[0].del_rtn = dor_object_method_null;
    doe_render_renderer[0].device = DcNullPtr;
    doe_render_renderer[0].view = DcNullPtr;
    doe_render_renderer[0].device_data_id = 
	    dor_class_add_object_data (device_id,
				       (DtMethodPtr) dor_render_obj_data_crt,
				       (DtMethodPtr) dor_render_obj_data_prt,
				       (DtMethodPtr) dor_render_obj_data_del);
    doe_render_renderer[0].view_data_id = 
	    dor_class_add_object_data (view_id,
				       (DtMethodPtr) dor_render_obj_data_crt,
				       (DtMethodPtr) dor_render_obj_data_prt,
				       (DtMethodPtr) dor_render_obj_data_del);

    doe_render_renderer[1].name = DcNullPtr;
    doe_render_renderer[1].render_rtn = dor_object_method_null;
    doe_render_renderer[1].del_rtn = dor_object_method_null;
    doe_render_renderer[1].device = DcNullPtr;
    doe_render_renderer[1].view = DcNullPtr;
    doe_render_renderer[1].device_data_id = 
	    dor_class_add_object_data (device_id,
				       (DtMethodPtr) dor_render_obj_data_crt,
				       (DtMethodPtr) dor_render_obj_data_prt,
				       (DtMethodPtr) dor_render_obj_data_del);
    doe_render_renderer[1].view_data_id = 
	    dor_class_add_object_data (view_id,
				       (DtMethodPtr) dor_render_obj_data_crt,
				       (DtMethodPtr) dor_render_obj_data_prt,
				       (DtMethodPtr) dor_render_obj_data_del);

    doe_render_renderer[2].name = DcNullPtr;
    doe_render_renderer[2].render_rtn = dor_object_method_null;
    doe_render_renderer[2].del_rtn = dor_object_method_null;
    doe_render_renderer[2].device = DcNullPtr;
    doe_render_renderer[2].view = DcNullPtr;
    doe_render_renderer[2].device_data_id = 
	    dor_class_add_object_data (device_id,
				       (DtMethodPtr) dor_render_obj_data_crt,
				       (DtMethodPtr) dor_render_obj_data_prt,
				       (DtMethodPtr) dor_render_obj_data_del);
    doe_render_renderer[2].view_data_id = 
	    dor_class_add_object_data (view_id,
				       (DtMethodPtr) dor_render_obj_data_crt,
				       (DtMethodPtr) dor_render_obj_data_prt,
				       (DtMethodPtr) dor_render_obj_data_del);

    dor_doresys_initialize_renderers();
}
/*
 ======================================================================
 */
DtInt dor_render_add_renderer (
    DtInt        renderer_id,
    char        *renderer_name,
    DtMethodPtr  render_rtn,
    DtMethodPtr  del_rtn,
    DtPFV        new_class_rtn,
    DtMethodPtr  inq_wcstofcs_rtn)
{
    dot_render_data *new_list;
    Dt32Bits size;

    if (renderer_id >= doe_render_next_renderer_id) {
	sprintf (DDerror_GetLine(), "id (%d) > maximum -- Adding at end",
		 renderer_id);
	DDerror (ERR_VALUE_OUT_OF_RANGE, "DDrender_AddRenderer",
		 DDerror_GetLine());
	renderer_id = -1;
    }

    if (renderer_id < 0 && renderer_id != -1) {
	sprintf (DDerror_GetLine(), "id (%d) < 0 -- Adding at end",
		 renderer_id);
	DDerror (ERR_VALUE_OUT_OF_RANGE, "DDrender_AddRenderer",
		 DDerror_GetLine());
	renderer_id = -1;
    }

    if (renderer_id != -1) {

	if (doe_render_renderer[renderer_id].name) {
	    dor_space_deallocate(doe_render_renderer[renderer_id].name);
	}
	doe_render_renderer[renderer_id].name = DcNullPtr;

    } else {

	renderer_id = doe_render_next_renderer_id;

	size = (doe_render_next_renderer_id + 1)*sizeof (dot_render_data);

	new_list = (dot_render_data *)
		dor_space_reallocate(doe_render_renderer,size);
	if (new_list == (dot_render_data *)0) {
	    DDerror (ERR_CANT_ALLOC_MEM, "DDrender_AddRenderer", DcNullPtr);
	    return -1;
	}

	doe_render_renderer = new_list;
	doe_render_next_renderer_id++;

	doe_render_renderer[renderer_id].device_data_id = 
		dor_class_add_object_data
		(   dor_class_inq_id("DoDevice"),
		    (DtMethodPtr)dor_render_obj_data_crt,
		    (DtMethodPtr)dor_render_obj_data_prt,
		    (DtMethodPtr)dor_render_obj_data_del
		);
	doe_render_renderer[renderer_id].view_data_id = 
		dor_class_add_object_data
		(   dor_class_inq_id("DoView"),
		    (DtMethodPtr) dor_render_obj_data_crt,
		    (DtMethodPtr) dor_render_obj_data_prt,
		    (DtMethodPtr) dor_render_obj_data_del
		);
    }

    if (renderer_name) {
	doe_render_renderer[renderer_id].name =
		dor_space_allocate(strlen(renderer_name)+1);
	strcpy (doe_render_renderer[renderer_id].name, renderer_name);
    } else {
	doe_render_renderer[renderer_id].name = DcNullPtr;
    }
    doe_render_renderer[renderer_id].render_rtn = render_rtn;
    doe_render_renderer[renderer_id].del_rtn = del_rtn;
    doe_render_renderer[renderer_id].inq_wcstofcs_rtn = inq_wcstofcs_rtn;
    doe_render_renderer[renderer_id].device = DcNullPtr;
    doe_render_renderer[renderer_id].view = DcNullPtr;

    if (new_class_rtn)
	dor_class_add_notify (new_class_rtn);

    return renderer_id;
}
/*
 ======================================================================
 */
void dor_render_print (void)
{
    Dt32Bits i;

    for (i=0; i<doe_render_next_renderer_id; i++) {
	if (!(doe_render_renderer[i].name)) {
	    sprintf (dor_print_get_line(), "%d: (no name)", i);
	} else {
	    sprintf (dor_print_get_line(), "%d: %s", i, 
		     doe_render_renderer[i].name);
	}
	dor_print_output(dor_print_get_line());

	dor_print_indent_up();
	sprintf (dor_print_get_line(), "render: 0x%lx",
		 doe_render_renderer[i].render_rtn);
	dor_print_output(dor_print_get_line());
	sprintf (dor_print_get_line(), "delete: 0x%lx",
		 doe_render_renderer[i].del_rtn);
	dor_print_output(dor_print_get_line());
	dor_print_indent_down();
    }
}
/*
 ======================================================================
 */
void dor_render_render (
    dot_object *device,
    dot_object *view)
{
    DtRenderStyle renderer_id;
    DtFlag device_view_changed = DcFalse;
    DtFlag studio_changed = DcFalse;
    DtUInt count;
    DtReal *rects;
    DtReal extent[4];
    DtFlag partobscured;
    DtUpdateType updatetype;
    DtInt devseqnum;
    DtInt viwseqnum;
    dot_render_obj_data *device_data;
    dot_render_obj_data *view_data;

    if (device == NULL) {
	DDerror (ERR_INVALID_DEVICE,
		    "dor_render_render", DcNullPtr);
    }
    if (view == NULL) {
	DDerror (ERR_INVALID_VIEW,
		    "dor_render_render", DcNullPtr);
	return;
    }

    renderer_id = dor_view_inq_render_style(view);

    if (renderer_id < 0 || renderer_id >= doe_render_next_renderer_id) {
	sprintf (DDerror_GetLine(), "renderstyle %d", renderer_id);
	DDerror (ERR_VALUE_OUT_OF_RANGE, "dor_render_render",DDerror_GetLine());
    }

    dor_device_inq_sequence_number(device,&devseqnum);
    dor_view_inq_sequence_number(view,&viwseqnum);

    device_data = (dot_render_obj_data*)
	    device->additional_data[doe_render_renderer[renderer_id].device_data_id];
    view_data = (dot_render_obj_data*)
	    view->additional_data[doe_render_renderer[renderer_id].view_data_id];

    if (dor_device_get_aborted_draw_flag(device)) {
      
	dor_device_update_geometry(device);

	dor_device_update_window(device,view);

	device_view_changed = DcTrue;
	studio_changed = DcTrue;
      
	dor_device_set_aborted_draw_flag (device, DcFalse);
    }

    /*  If we are updating the environment (which involves data structures
	stored computed and stored by the device and view subsystems, which
	might be changed by a pick), or we are updating a different combination of
	device and view than the last one updated, then get the (potentially)
	new device and view (and window) information.  Otherwise, check the
	modification sequence numbers of the device and view against those
	stored with the local rendering data structures for the device and
	view to make sure that the device and view have not been modified to
	be out of sync with the data stored in these data structures.  */

    updatetype = dor_view_inq_update_type(view);

    if (updatetype == DcUpdateAll) {

	dor_device_update_window(device,view);

	device_view_changed = DcTrue;
	studio_changed = DcTrue;
      
    } else if ((doe_render_renderer[renderer_id].device != device) ||
	       (doe_render_renderer[renderer_id].view != view)) {
	dor_device_update_geometry(device);

	dor_device_update_window(device,view);

	device_view_changed = DcTrue;
	studio_changed = DcTrue;

    } else {
	if ((devseqnum != device_data->seq_num) ||
	    (viwseqnum != view_data->seq_num)) {

	    dor_device_update_geometry(device);

	    dor_device_update_window(device,view);

	    device_view_changed = DcTrue;
	    studio_changed = DcTrue;
	}
    }

    dor_device_inquire_clip_list(device, &count, &rects, extent, &partobscured);
    if (count == 0) {
	/* The window that we are rendering into is completely obscured
	   (either completely covered by other windows, or iconified).
	   Before returning from rendering, we mark the geometry change flag
	   as having changed, so that the next rendering will recheck the 
	   geometry to see if the window is visible again. */
      
	dor_device_set_aborted_draw_flag (device, DcTrue);
	return;
    }

    dor_condex_set_disable_prim_exec_flag (DcFalse);

    doe_render_current_device = device;
    doe_render_current_view = view;
    doe_render_renderer[renderer_id].device = device;
    doe_render_renderer[renderer_id].view = view;
    doe_render_current_renderer_id = renderer_id;
    device_data->seq_num = devseqnum;
    view_data->seq_num = viwseqnum;

    (*(doe_render_renderer[renderer_id].render_rtn))(device, view, 
						     device_view_changed,
						     studio_changed);
}
