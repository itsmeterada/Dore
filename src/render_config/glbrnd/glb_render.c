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
	int drr_glbrnd_render (device,view,update_local_data,initialize_studio)
	int drr_glbrnd_update_device_and_view (device,view)

  ======================================================================
 */
#include "glbrnd.h"

/*
 ======================================================================
 */

void drr_glbrnd_render (
    dot_object *device,
    dot_object *view,
    DtFlag      update_local_data,
    DtFlag      initialize_studio)
{
    dre_glbrnd_initialize_studio = initialize_studio;

    if (dre_glbrnd_data.current_device != device) {
	DDdevice_InqInterfaceRoutines (device, DDc_PROM,
				       (DtPtr* )&dre_glbrnd_PROM_fcns);
    }

    if ((DtPtr) dre_glbrnd_PROM_fcns == DcNullPtr) {
	DDerror (ERR_NO_OUTPUT_MODULE,
		    "drr_glbrnd_render", 
		    "Production");
	return;
    }

    if (dre_glbrnd_initialize_studio) {
	drr_glbrnd_update_device_and_view(device,view);
	drr_glbrnd_initialize_environment();
    }

    drr_rayint_initialize_object_list();
    drr_rayint_initialize_statistics();

    DDview_TraverseDisplay (dre_glbrnd_data.current_view,
			      dre_glbrnd_data.display_method);


    drr_recrnd_initialize_device_and_view();
    drr_glbshd_initialize_render(
			dre_glbrnd_curviwdat->background_color_model,
			dre_glbrnd_curviwdat->background_color);

    drr_rayint_initialize_render();
    drr_recrnd_render();
}
/*
 ======================================================================
 */

void drr_glbrnd_update_device_and_view (
    dot_object *device,
    dot_object *view)
{
    DtInt deviceno;
    DtInt devicecount;

#ifdef DEBUG
    printf("updating new device and view = %lx %lx\n",device,view);
#endif

    /*  Set the current device data structure pointer to the data 
	structure stored with this device.  If there is none, create 
	one, and attach it to the device for later reference.  */

    dre_glbrnd_curdevdat = (struct glbrnd_device_data *)
	    device->additional_data[dre_glbrnd_data.device_index];

    dre_glbrnd_data.current_device = device;

    /*  Set the current view data structure pointer to the data 
	structure stored with this view.  If there is none, create 
	one, and attach it to the view for later reference.  */

    dre_glbrnd_curviwdat = (struct glbrnd_view_data *)
	    view->additional_data[dre_glbrnd_data.view_index];

    dre_glbrnd_data.current_view = view;

    dre_glbrnd_curcamdat = dre_glbrnd_curviwdat->camera_data;
    dre_glbrnd_curlgtdat = dre_glbrnd_curviwdat->light_data;

    /*  Look up the window data structure attached to the 
	view-device combination.  If it does not exist, create 
	one, and attach it to the current view-device combination. */

    dre_glbrnd_curwindat = NULL;

    for (deviceno=0;
	 deviceno<dre_glbrnd_curviwdat->window_devicecount;
	 deviceno++) {
	if (dre_glbrnd_curviwdat->window_device[deviceno] == device) {
	    dre_glbrnd_curwindat = 
		dre_glbrnd_curviwdat->window_data[deviceno];
	}
    }

    /*  This view-device combination has not yet been updated, so create
	a new window data structure for it.  */

    if (dre_glbrnd_curwindat == NULL) {
	dre_glbrnd_curviwdat->window_devicecount++;

	devicecount = dre_glbrnd_curviwdat->window_devicecount;

	dre_glbrnd_curviwdat->window_device =
		(dot_object **)DDspace_Reallocate
			(dre_glbrnd_curviwdat->window_device,
			 devicecount*(sizeof (dot_object *)));
		
	dre_glbrnd_curviwdat->window_data =
		(struct glbrnd_window_data **)DDspace_Reallocate
			(dre_glbrnd_curviwdat->window_data,
			 devicecount*(sizeof (struct glbrnd_window_data *)));

	dre_glbrnd_curviwdat->window_device[devicecount-1] = device;

	dre_glbrnd_curviwdat->window_data[devicecount-1] =
		(struct glbrnd_window_data *)DDspace_Allocate
			(sizeof(struct glbrnd_window_data));

	dre_glbrnd_curwindat = 
		dre_glbrnd_curviwdat->window_data[devicecount-1];
    }
		
    /*  Initialize the characteristics of the device being 
	rendered to which will not change from one update to 
	another.  */

    DvInqBackgroundColor(view,
			  &dre_glbrnd_curviwdat->background_color_model,
			  dre_glbrnd_curviwdat->background_color);

    dre_glbrnd_curdevdat->shade_mode = DdInqShadeMode(device);

    /*  Update the modification sequence number of the current 
	device data to reflect the device data that it was 
	updated with.  */

    dor_device_inq_sequence_number(device,
		&(dre_glbrnd_curdevdat->modification_sequence_number));

    /*  Update the modification sequence number of the current view
	data to reflect the view data that it was updated with.  */

    dor_view_inq_sequence_number(view,
		&(dre_glbrnd_curviwdat->modification_sequence_number));
}
