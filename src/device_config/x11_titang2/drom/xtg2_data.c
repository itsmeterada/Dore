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
	DtPtr ddr_x11_titang2_drom_create_local_device_data (device)
	DtPtr ddr_x11_titang2_drom_create_local_view_data (view)
	DtPtr ddr_x11_titang2_drom_create_local_window_data (device, view)
	ddr_x11_titang2_drom_delete_local_device_data ddr_x11_titang2_drom_delete_local_device_data (data)
	ddr_x11_titang2_drom_delete_local_view_data ddr_x11_titang2_drom_delete_local_view_data (data)
	ddr_x11_titang2_drom_delete_local_window_data ddr_x11_titang2_drom_delete_local_window_data (data)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */

DtPtr
ddr_x11_titang2_drom_create_local_device_data(device)
     DtObject device;
{
    struct drom_device_data *devicedata;
    DtPtr DDspace_Allocate();

    devicedata = (struct drom_device_data *)DDspace_Allocate
	    ((sizeof (struct drom_device_data)));

    return((DtPtr)devicedata);
}
/*
 ======================================================================
 */

DtPtr
ddr_x11_titang2_drom_create_local_view_data(view)
     DtObject view;
{
    struct drom_view_data *viewdata;
    DtPtr DDspace_Allocate();

    viewdata = (struct drom_view_data *)DDspace_Allocate
	    ((sizeof (struct drom_view_data)));

    viewdata->camera_data = (struct drom_camera_data *)DDspace_Allocate
	    (sizeof (struct drom_camera_data));

    viewdata->light_data = (struct drom_light_data *)DDspace_Allocate
	    (sizeof (struct drom_light_data));

    viewdata->light_data->count = 0;
    viewdata->light_data->max_count = 0;
    viewdata->light_data->cosine = NULL;
    viewdata->light_data->intens = NULL;
    viewdata->light_data->color = NULL;
    viewdata->light_data->h = NULL;
    viewdata->light_data->lcscosine = NULL;
    viewdata->light_data->scaledcolor = NULL;

    return((DtPtr)viewdata);
}
/*
 ======================================================================
 */

DtPtr
ddr_x11_titang2_drom_create_local_window_data (device, view)
     DtObject device;
     DtObject view;
{
    struct drom_window_data *windata;
    DtPtr DDspace_Allocate();

    windata = (struct drom_window_data *)DDspace_Allocate
	    ((sizeof (struct drom_window_data)));

    return ((DtPtr)windata);
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_delete_local_device_data (data)
     struct drom_device_data *data;
{
    if (data != (struct drom_device_data *)0) {
	DDspace_Deallocate (data);
    }
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_delete_local_view_data (data)
     struct drom_view_data *data;
{
    if (data != (struct drom_view_data *)0) {
	if (data->camera_data != (struct drom_camera_data *)0) {
	    DDspace_Deallocate (data->camera_data);
	}

	if (data->light_data != (struct drom_light_data *)0) {
	    if (data->light_data->max_count != 0) {
		if (data->light_data->cosine != (DtRealTriple *)0) {
		    DDspace_Deallocate (data->light_data->cosine);
		}
		if (data->light_data->intens != (DtReal *)0) {
		    DDspace_Deallocate (data->light_data->intens);
		}
		if (data->light_data->color != (DtColorRGB *)0) {
		    DDspace_Deallocate (data->light_data->color);
		}
		if (data->light_data->h != (DtRealTriple *)0) {
		    DDspace_Deallocate (data->light_data->h);
		}
		if (data->light_data->lcscosine != (DtRealTriple *)0) {
		    DDspace_Deallocate (data->light_data->lcscosine);
		}
		if (data->light_data->scaledcolor != (DtColorRGB *)0) {
		    DDspace_Deallocate (data->light_data->scaledcolor);
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

ddr_x11_titang2_drom_delete_local_window_data (data)
     struct drom_window_data *data;
{
    if (data != (struct drom_window_data *)0) {
	DDspace_Deallocate (data);
    }
}

