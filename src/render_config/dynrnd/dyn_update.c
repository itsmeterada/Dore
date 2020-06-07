 
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
	void drr_dynrnd_update_device_and_view (device,view)
	void drr_dynrnd_render (device,view,update_local_data, initialize_studio)
	void drr_dynrnd_traverse_studio_group ()
	void drr_dynrnd_traverse_display_group ()

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/device.h>
#include "dynrnd.h"


/* ====================================================================== */

void drr_dynrnd_update_device_and_view (
    dot_object *device,
    dot_object *view)
{
    DtInt deviceno;
    DtInt devicecount;
    DtInt driverno;
    DtInt drivercount;
    drt_dynrnd_driver_data *driver_data;

    /*  Set the current device data structure pointer to the 
	data structure stored with this device.  If there is none, 
	create one, and attach it to the device for 
	later reference.  */

    dre_dynrnd_curdevdat = (drt_dynrnd_device_data *)
	    device->additional_data[dre_dynrnd_data.device_index];

    if (dre_dynrnd_curdevdat->user_data == DcNullPtr) {
	dre_dynrnd_curdevdat->user_data = (DtPtr)
	    (*dre_dynrnd_DROM_fcns->create_local_device_data) (device);

	(*dre_dynrnd_DROM_fcns->pre_initialization)();
	drr_dynrnd_initialize_attributes();
	(*dre_dynrnd_DROM_fcns->post_initialization)();
    }

    dre_dynrnd_data.current_device = device;

    /*  Set the current view data structure pointer to the 
	data structure stored with this view.  If there is none, 
	create one, and attach it to the view for 
	later reference.  */

    driver_data = (drt_dynrnd_driver_data *)
	    view->additional_data[dre_dynrnd_data.view_index];

    for (driverno=0, dre_dynrnd_curviwdat = NULL;
	 driverno<driver_data->driver_count;
	 driverno++) {
	if (driver_data->view_data[driverno]->crt_fcn == 
	    (DtPFI) dre_dynrnd_DROM_fcns->create_local_view_data) {
	    dre_dynrnd_curviwdat = driver_data->view_data[driverno];
	}
    }

    if (dre_dynrnd_curviwdat == NULL) {
	driver_data->driver_count++;
	drivercount = driver_data->driver_count;

	driver_data->view_data = 
		(drt_dynrnd_view_data **)DDspace_Reallocate
			(driver_data->view_data, 
			 drivercount*sizeof(drt_dynrnd_view_data *));

	driver_data->view_data[drivercount-1] = 
			drr_dynrnd_create_view_data();

	dre_dynrnd_curviwdat = driver_data->view_data[drivercount-1];

	dre_dynrnd_curviwdat->crt_fcn =
	    (DtPFI) dre_dynrnd_DROM_fcns->create_local_view_data;
	dre_dynrnd_curviwdat->del_fcn = 
	    (DtPFI) dre_dynrnd_DROM_fcns->delete_local_view_data;
	dre_dynrnd_curviwdat->user_data = (DtPtr)
		(*dre_dynrnd_DROM_fcns->create_local_view_data) (view);
    }

    dre_dynrnd_data.current_view = view;

    /*  Look up the window data structure attached to the 
	view-device combination.  If it does not exist, create 
	one, and attach it to the current view-device 
	combination.  */

    for (deviceno=0, dre_dynrnd_curwindat = NULL;
	 deviceno<dre_dynrnd_curviwdat->window_devicecount;
	 deviceno++) {
	if (dre_dynrnd_curviwdat->window_device[deviceno] == device) {
	    dre_dynrnd_curwindat = 
			dre_dynrnd_curviwdat->window_data[deviceno];
	}
    }

    /*  This view-device combination has not yet been updated, 
	so create a new window data structure for it.  */

    if (dre_dynrnd_curwindat == NULL) {
	dre_dynrnd_curviwdat->window_devicecount++;

	devicecount = dre_dynrnd_curviwdat->window_devicecount;

	dre_dynrnd_curviwdat->window_device = 
		(dot_object **)DDspace_Reallocate
			(dre_dynrnd_curviwdat->window_device,
			 devicecount*(sizeof (dot_object *)));
		
	dre_dynrnd_curviwdat->window_data =
		(drt_dynrnd_window_data **)DDspace_Reallocate
			(dre_dynrnd_curviwdat->window_data,
			 devicecount*(sizeof (drt_dynrnd_window_data *)));

	dre_dynrnd_curviwdat->window_device[devicecount-1] = device;

	dre_dynrnd_curviwdat->window_data[devicecount-1] =
		(drt_dynrnd_window_data *)DDspace_Allocate
			(sizeof(drt_dynrnd_window_data));

	dre_dynrnd_curwindat = 
		dre_dynrnd_curviwdat->window_data[devicecount-1];

	dre_dynrnd_curwindat->del_fcn = 
	    (DtPFI) dre_dynrnd_DROM_fcns->delete_local_window_data;
	dre_dynrnd_curwindat->user_data = (DtPtr)
		(*dre_dynrnd_DROM_fcns->create_local_window_data) (device, view);
    }
		
    /*  Initialize the characteristics of the device being 
	rendered to which will not change from one update 
	to another.  */

    DDdevice_SetDepthBufferEnable(
			dre_dynrnd_data.current_device, 1);

    DDdevice_SetDepthBufferWrite(
			dre_dynrnd_data.current_device, 1);

    /*  Update the modification sequence number of the current 
	device data to reflect the device data that it was 
	updated with.  */

    dor_device_inq_sequence_number(device,
				   &(dre_dynrnd_curdevdat->
				   modification_sequence_number));

    /*  Update the modification sequence number of the current 
	view data to reflect the view data that it was updated 
	with.  */
   
    dor_view_inq_sequence_number(view,
				 &(dre_dynrnd_curviwdat->
				 modification_sequence_number));
}

/* ====================================================================== */

void drr_dynrnd_render (
    dot_object *device,
    dot_object *view,
    DtFlag update_local_data,
    DtFlag initialize_studio)
{
    DtRadiosityUpdateType radupdtype;

    radupdtype = DvInqRadiosityUpdateType(view);

    /* If radiosity is requested (and supported), let the radiosity
       package take control  */

    if (radupdtype != DcRadiosityNone) {	
           if (dpr_radiosity_render(device, view,
                     dre_dynrnd_data.display_method,
                     (DtPFI) dre_dynrnd_reset_current_display_method))
       	       return;
           /* note: if dpr_radiosity_render returns false, radiosity 
              is not supported with this device type, so continue */
    }

    if (dre_dynrnd_data.current_device != device) {
	DDdevice_InqInterfaceRoutines (device,
				       DDc_DROM,
				       (DtPtr *)&dre_dynrnd_DROM_fcns);
	DDdevice_InqInterfaceRoutines (device,
				       DDc_DROMMethods,
				       (DtPtr *)&dre_dynrnd_DROM_method);
	drr_dynrnd_update_default_methods();

	drr_dynrnd_set_object_methods();
    }

    if ((DtPtr) dre_dynrnd_DROM_fcns == DcNullPtr) {
	DDerror (ERR_NO_OUTPUT_MODULE,
		    "drr_dynrnd_render", "Dynamic");
	return;
    }

    if (update_local_data) 
	    drr_dynrnd_update_device_and_view(device,view);

    (*dre_dynrnd_DROM_fcns->start_update) (
				device, view,
			   	dre_dynrnd_curdevdat->user_data,
				dre_dynrnd_curviwdat->user_data,
				dre_dynrnd_curwindat->user_data);

    if (update_local_data) 
	    (*dre_dynrnd_DROM_fcns->update_local_data)();

    if (initialize_studio)
    {   (*dre_dynrnd_DROM_fcns->update_studio)
	    ((DtPFI) drr_dynrnd_traverse_studio_group);
    }

    (*dre_dynrnd_DROM_fcns->update_display)
	((DtPFI) drr_dynrnd_traverse_display_group);
}

/* ====================================================================== */

    /*  The purpose of environmental initialization is to collect
	data related to the cameras and lights.  The
	camera data always includes the camera's local to world
	transform (which is inverted to obtain the world to camera
	transform).  */
void drr_dynrnd_traverse_studio_group (void)
{
    /*  Set up the default camera */

    (*dre_dynrnd_DROM_fcns->set_perspective_matrix) (90., -0.01, -1.);
    (*dre_dynrnd_DROM_fcns->camera) ();

    DDview_TraverseStudio (dre_dynrnd_data.current_view,
			     dre_dynrnd_data.studio_method);
}

/* ====================================================================== */

void drr_dynrnd_traverse_display_group (void)
{
    DDview_TraverseDisplay (dre_dynrnd_data.current_view,
			      dre_dynrnd_data.current_display_method);
}

/* ====================================================================== */


/* The following function resets the display method for the dynamic
   renderer. This should be used with caution! This function is
   used by the radiosity package to temporarily modify the
   display method used for the traversal (and the initial value is
   restored after radiosity rendering is over).
*/

void dre_dynrnd_reset_current_display_method (
    DtInt new_display_method_id)
{
    dre_dynrnd_data.current_display_method = new_display_method_id;
}
