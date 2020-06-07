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
	DtPtr ddr_vll_drom_create_local_device_data (device)
	DtPtr ddr_vll_drom_create_local_view_data (view)
	DtPtr ddr_vll_drom_create_local_window_data (device, view)
	void ddr_vll_drom_delete_local_device_data (data)
	void ddr_vll_drom_delete_local_view_data (data)
	void ddr_vll_drom_delete_local_window_data (data)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
DtPtr
ddr_vll_drom_create_local_device_data(device)
     DtObject device;
{
    ddt_vll_drom_device *devicedata;
    DtInt shaderange[2];
    extern DtFlag   ddr_vll_dcm_inquire_spheres();

    devicedata = (ddt_vll_drom_device *)DDspace_Allocate
	    (sizeof(ddt_vll_drom_device));

    devicedata->visual_type  = DdInqVisualType(device);
    devicedata->shade_mode   = DdInqShadeMode(device);
    devicedata->stereoswitch = DDdevice_InqStereo(device);
    devicedata->use_spheres  = ddr_vll_dcm_inquire_spheres();
    devicedata->features     = VLLquery_features();

    shaderange[0] = -10;
    shaderange[1] = -10;
    DdInqShadeRanges (device,0,1, shaderange);
    if (shaderange[0] != -10 && shaderange[1] != -10) {
	devicedata->pseudo_range[0] = shaderange[0];
	devicedata->pseudo_range[1] = shaderange[1];
    } else {
	devicedata->pseudo_range[0] = 0;
	devicedata->pseudo_range[1] = 255;
    }
    ddr_vll_dcm_inq_pseudo_range(devicedata->pseudo_range_weight);
    ddr_vll_dcm_inq_pseudo_space(devicedata->pseudo_space_weight,
				   devicedata->pseudo_space_mask);

    dde_vll_drom_data.current_device = device;
    dde_vll_drom_curdevdat = devicedata;

    return((DtPtr)devicedata);
}
/*
 ======================================================================
 */
DtPtr
ddr_vll_drom_create_local_view_data(view)
     DtObject view;
{
    ddt_vll_drom_view *viewdata;

    viewdata = (ddt_vll_drom_view *)DDspace_Allocate
	    (sizeof (ddt_vll_drom_view));

    viewdata->total_lights = 0;
    viewdata->max_lights = 0;
    viewdata->have_textures = DcFalse;
    viewdata->have_spheres = DcFalse;
    viewdata->light = 0;
    viewdata->camera_data.filter = (int *)0;

    dde_vll_drom_data.current_view = view;
    dde_vll_drom_curviwdat = viewdata;

    return((DtPtr)viewdata);
}
/*
 ======================================================================
 */
DtPtr
ddr_vll_drom_create_local_window_data (device, view)
     DtObject device;
     DtObject view;
{
    ddt_vll_drom_window *windowdata;

    windowdata = (ddt_vll_drom_window *)DDspace_Allocate
	    (sizeof (ddt_vll_drom_window));

    dde_vll_drom_curwindat = windowdata;

    return((DtPtr)windowdata);
}
/*
 ======================================================================
 */
void
ddr_vll_drom_delete_local_device_data (data)
     DtPtr data;
{
    if (data != DcNullPtr)
	    DDspace_Deallocate(data);
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_delete_local_view_data (data)
     ddt_vll_drom_view *data;
{
   
    if (data->max_lights > 0 && data->light != (DtObject )0)
	    DDspace_Deallocate(data->light);

    if (data->camera_data.filter != (int *)0) {
	DDspace_Deallocate(data->camera_data.filter);
    }

    if (data != DcNullPtr)
	    DDspace_Deallocate(data);
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_delete_local_window_data (data)
     DtPtr data;
{
    if (data != DcNullPtr)
	    DDspace_Deallocate(data);
}
/*
 ======================================================================
 */
