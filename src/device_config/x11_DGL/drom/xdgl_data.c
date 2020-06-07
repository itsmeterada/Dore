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
	DtPtr ddr_x11_DGL_drom_create_local_device_data (device)
	DtPtr ddr_x11_DGL_drom_create_local_view_data (view)
	DtPtr ddr_x11_DGL_drom_create_local_window_data (device, view)
	ddr_x11_DGL_drom_delete_local_device_data ddr_x11_DGL_drom_delete_local_device_data (data)
	ddr_x11_DGL_drom_delete_local_view_data ddr_x11_DGL_drom_delete_local_view_data (data)
	ddr_x11_DGL_drom_delete_local_window_data ddr_x11_DGL_drom_delete_local_window_data (data)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
DtPtr
ddr_x11_DGL_drom_create_local_device_data(device)
     DtObject device;
{
    struct drom_device_data *devicedata;

    devicedata = (struct drom_device_data *)DDspace_Allocate
	    ((sizeof (struct drom_device_data)));

    ddr_x11_DGL_dcm_inquire_pDraw(&(devicedata->pDraw));
    ddr_x11_DGL_dcm_inquire_pGC(&(devicedata->pGC));

    dde_x11_DGL_drom_data.current_device = device;
    dde_x11_DGL_drom_curdevdat = devicedata;

    return((DtPtr)devicedata);
}
/*
 ======================================================================
 */
DtPtr
ddr_x11_DGL_drom_create_local_view_data(view)
     DtObject view;
{
    struct drom_view_data *viewdata;

    viewdata = (struct drom_view_data *)DDspace_Allocate
	    ((sizeof (struct drom_view_data)));

    viewdata->camera_data = (struct drom_camera_data *)DDspace_Allocate
	    (sizeof (struct drom_camera_data));

    viewdata->light_data = (struct drom_light_data *)DDspace_Allocate
	    (sizeof (struct drom_light_data));

    viewdata->light_data->count = 0;
    viewdata->light_data->max_count = 5;
    viewdata->light_data->lights = 
	    (XdLightPtr)DDspace_Allocate(5*sizeof(XdLight));
    viewdata->light_data->light_objs = 
	    (DtObject *)DDspace_Allocate(5*sizeof(DtObject));

    dde_x11_DGL_drom_data.current_view = view;
    dde_x11_DGL_drom_curviwdat = viewdata;
    dde_x11_DGL_drom_curlgtdat = viewdata->light_data;
    dde_x11_DGL_drom_curcamdat = viewdata->camera_data;
   
    return((DtPtr)viewdata);
}
/*
 ======================================================================
 */
DtPtr
ddr_x11_DGL_drom_create_local_window_data (device, view)
     DtObject device;
     DtObject view;
{
    struct drom_window_data *windata;

    windata = (struct drom_window_data *)DDspace_Allocate
	    ((sizeof (struct drom_window_data)));

    dde_x11_DGL_drom_data.current_device = device;
    dde_x11_DGL_drom_data.current_view = view;
    dde_x11_DGL_drom_curwindat = windata;

    return ((DtPtr)windata);
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_delete_local_device_data (data)
     struct drom_device_data *data;
{
    if (data != (struct drom_device_data *)0) {
	DDspace_Deallocate (data);
    }
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_delete_local_view_data (data)
     struct drom_view_data *data;
{
    if (data != (struct drom_view_data *)0) {
	if (data->camera_data != (struct drom_camera_data *)0) {
	    DDspace_Deallocate (data->camera_data);
	}
	if (data->light_data != (struct drom_light_data *)0) {
	    if (data->light_data->max_count > 0) {
		if (data->light_data->light_objs != (DtObject *)0) {
		    DDspace_Deallocate (data->light_data->light_objs);
		}
		if (data->light_data->lights != (XdLightPtr)0) {
		    DDspace_Deallocate (data->light_data->lights);
		}
	    }
	    DDspace_Deallocate (data->light_data);
	}
	DDspace_Deallocate (data);
    }
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_delete_local_window_data (data)
     struct drom_window_data *data;
{
    if (data != (struct drom_window_data *)0) {
	DDspace_Deallocate (data);
    }
}
