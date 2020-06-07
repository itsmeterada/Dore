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
	drt_dynrnd_device_data * drr_dynrnd_create_device_data ()
	drt_dynrnd_driver_data * drr_dynrnd_create_driver_data ()
	drt_dynrnd_view_data * drr_dynrnd_create_view_data ()
	void drr_dynrnd_delete_device_data (device, data)
	void drr_dynrnd_delete_driver_data (view, data)
	void drr_dynrnd_delete_view_data (data)
	void drr_dynrnd_delete_window_data (data)

  ======================================================================
 */
#include "dynrnd.h"

/*
 ======================================================================
 */
drt_dynrnd_device_data *drr_dynrnd_create_device_data (void)

{
    drt_dynrnd_device_data *devicedata;

    devicedata = (drt_dynrnd_device_data *)DDspace_Allocate
	    ((sizeof (drt_dynrnd_device_data)));

    devicedata->modification_sequence_number = -1;
    devicedata->user_data = DcNullPtr;

    return(devicedata);
}
/*
 ======================================================================
 */
drt_dynrnd_driver_data *drr_dynrnd_create_driver_data (void)
{
    drt_dynrnd_driver_data *driver_data;

    driver_data = (drt_dynrnd_driver_data *)DDspace_Allocate
	    ((sizeof (drt_dynrnd_driver_data)));

    driver_data->driver_count = 0;
    driver_data->view_data = (drt_dynrnd_view_data **)0;

    return(driver_data);
}
/*
 ======================================================================
 */
drt_dynrnd_view_data *drr_dynrnd_create_view_data (void)
{
    drt_dynrnd_view_data *viewdata;

    viewdata = (drt_dynrnd_view_data *)DDspace_Allocate
	    ((sizeof (drt_dynrnd_view_data)));

    viewdata->window_devicecount = 0;
    viewdata->window_device = NULL;
    viewdata->window_data = NULL;

    return(viewdata);
}
/*
 ======================================================================
 */
void drr_dynrnd_delete_device_data (
    DtObject device,
    drt_dynrnd_device_data *data)
{
    if ((data->user_data != DcNullPtr) &&
	(dre_dynrnd_DROM_fcns != DcNullPtr)) {
	(*dre_dynrnd_DROM_fcns->delete_local_device_data)(data->user_data);
    }
    DDspace_Deallocate (data);
}
/*
 ======================================================================
 */
void drr_dynrnd_delete_driver_data (
    DtObject view,
    drt_dynrnd_driver_data *data)
{
    DtInt driverno;

    for (driverno=0; driverno<data->driver_count; driverno++) {
	drr_dynrnd_delete_view_data (data->view_data[driverno]);
    }

    DDspace_Deallocate (data);
}
/*
 ======================================================================
 */
void drr_dynrnd_delete_view_data (
    drt_dynrnd_view_data *data)
{
    DtInt deviceno;

    if (data->user_data != DcNullPtr) 
	    (*data->del_fcn)(data->user_data);

    for (deviceno=0; deviceno<data->window_devicecount; deviceno++) {
	drr_dynrnd_delete_window_data (data->window_data[deviceno]);
    }

    DDspace_Deallocate (data);
}
/*
 ======================================================================
 */
void drr_dynrnd_delete_window_data (
    drt_dynrnd_window_data *data)
{
    if (data->user_data != DcNullPtr)
	    (*data->del_fcn)(data->user_data);

    DDspace_Deallocate (data);
}
/*
 ======================================================================
 */
