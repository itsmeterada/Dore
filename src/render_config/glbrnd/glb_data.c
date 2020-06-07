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
	struct glbrnd_device_data * drr_glbrnd_create_device_data ()
	struct glbrnd_view_data * drr_glbrnd_create_view_data ()
	int drr_glbrnd_delete_device_data (device, data)
	int drr_glbrnd_delete_view_data (view, data)

  ======================================================================
 */
#include "glbrnd.h"

/*
 ======================================================================
 */

struct glbrnd_device_data *
drr_glbrnd_create_device_data (void)
{
    struct glbrnd_device_data *devdata;

    devdata = (struct glbrnd_device_data *)DDspace_Allocate(
			      (sizeof (struct glbrnd_device_data)));

    return(devdata);
}
/*
 ======================================================================
 */

struct glbrnd_view_data *
drr_glbrnd_create_view_data (void)
{
    struct glbrnd_view_data *viewdata;

    viewdata = 
		(struct glbrnd_view_data *)DDspace_Allocate(
			     (sizeof (struct glbrnd_view_data)));

    viewdata->camera_data = 
		(struct glbrnd_camera_data *)DDspace_Allocate(
			    sizeof (struct glbrnd_camera_data));

    viewdata->light_data = 
		(struct glbrnd_light_data *)DDspace_Allocate(
			  sizeof (struct glbrnd_light_data));

    viewdata->window_devicecount = 0;
    viewdata->window_device = NULL;
    viewdata->window_data = NULL;

    return(viewdata);
}
/*
 ======================================================================
 */

void drr_glbrnd_delete_device_data (
    DtObject device,
    struct glbrnd_device_data *data)
{
    if (data != (struct glbrnd_device_data *)0) {
	DDspace_Deallocate (data);
    }
}

/*
 ======================================================================
 */

void drr_glbrnd_delete_view_data (
    DtObject view,
    struct glbrnd_view_data *data)
{
    DtInt i;

    if (data->window_devicecount > 0) {
	for (i = 0; i < data->window_devicecount; i++) {
	    DDspace_Deallocate( data->window_data[i]);
	}
	DDspace_Deallocate (data->window_data);
    }
    DDspace_Deallocate (data->camera_data);
    DDspace_Deallocate (data->light_data);

    DDspace_Deallocate (data);
}

/*
 ======================================================================
 */
