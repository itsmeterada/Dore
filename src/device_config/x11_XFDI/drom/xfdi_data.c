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
	DtPtr ddr_x11_XFDI_drom_create_local_device_data (device)
	DtPtr ddr_x11_XFDI_drom_create_local_view_data (view)
	DtPtr ddr_x11_XFDI_drom_create_local_window_data (device, view)
	ddr_x11_XFDI_drom_delete_local_device_data ddr_x11_XFDI_drom_delete_local_device_data (data)
	ddr_x11_XFDI_drom_delete_local_view_data ddr_x11_XFDI_drom_delete_local_view_data (data)
	ddr_x11_XFDI_drom_delete_local_window_data ddr_x11_XFDI_drom_delete_local_window_data (data)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
DtPtr
ddr_x11_XFDI_drom_create_local_device_data(device)
     DtObject device;
{
    ddt_x11_XFDI_drom_device *devicedata;
    DtInt shaderange[2];
    DtInt nclrs;
    extern Display *ddr_x11_XFDI_dcm_inquire_display();
    extern XFDIGC   ddr_x11_XFDI_dcm_inquire_gc();
    extern DtFlag   ddr_x11_XFDI_dcm_inquire_spheres();

    devicedata = (ddt_x11_XFDI_drom_device *)DDspace_Allocate
	    (sizeof(ddt_x11_XFDI_drom_device));

    devicedata->visual_type  = DdInqVisualType(dde_x11_XFDI_drom_data.current_device);
    devicedata->shade_mode   = DdInqShadeMode(dde_x11_XFDI_drom_data.current_device);
    devicedata->stereoswitch = DDdevice_InqStereo(dde_x11_XFDI_drom_data.current_device);
    devicedata->stereoscreen = DcFalse;
    devicedata->display      = ddr_x11_XFDI_dcm_inquire_display();
    devicedata->gc           = ddr_x11_XFDI_dcm_inquire_gc();
    devicedata->use_spheres  = ddr_x11_XFDI_dcm_inquire_spheres();

    shaderange[0] = -10;
    shaderange[1] = -10;
    DdInqShadeRanges (dde_x11_XFDI_drom_data.current_device,
				0,1, shaderange);
    if (shaderange[0] != -10 && shaderange[1] != -10) {
	devicedata->current_shade_range_min = shaderange[0];
	devicedata->current_shade_range_max = shaderange[1];
	devicedata->shade_range_scale = (shaderange[1] - shaderange[0]);
    } else {
	devicedata->current_shade_range_min = 0;
	devicedata->current_shade_range_max = 255;
	devicedata->shade_range_scale = 256;
    }
    ddr_x11_XFDI_dcm_inq_pseudocolor_weighted_average_weights
	    (&(devicedata->cwtavgpsweight));

    if (devicedata->visual_type ==DcPseudoColor) {
	XFDISetMarkerBackground (devicedata->display,
				 devicedata->gc,
				 devicedata->current_shade_range_min);
   
	XFDISetLineBackground (devicedata->display,
			       devicedata->gc,
			       devicedata->current_shade_range_min);

	XFDISetSurfaceBackground (devicedata->display,
				  devicedata->gc,
				  devicedata->current_shade_range_min);
    }
    dde_x11_XFDI_drom_data.current_device = device;
    dde_x11_XFDI_drom_curdevdat = devicedata;

    return((DtPtr)devicedata);
}
/*
 ======================================================================
 */
DtPtr
ddr_x11_XFDI_drom_create_local_view_data(view)
     DtObject view;
{
    ddt_x11_XFDI_drom_view *viewdata;

    viewdata = (ddt_x11_XFDI_drom_view *)DDspace_Allocate
	    (sizeof (ddt_x11_XFDI_drom_view));

    viewdata->total_lights = 0;
    viewdata->texture_pass_needed = DcFalse;
    viewdata->have_spheres = DcFalse;

    dde_x11_XFDI_drom_data.current_view = view;
    dde_x11_XFDI_drom_curviwdat = viewdata;

    return((DtPtr)viewdata);
}
/*
 ======================================================================
 */
DtPtr
ddr_x11_XFDI_drom_create_local_window_data (device, view)
     DtObject device;
     DtObject view;
{
    ddt_x11_XFDI_drom_window *windowdata;

    windowdata = (ddt_x11_XFDI_drom_window *)DDspace_Allocate
	    (sizeof (ddt_x11_XFDI_drom_window));

    dde_x11_XFDI_drom_curwindat = windowdata;

    return((DtPtr)windowdata);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_delete_local_device_data (data)
     DtPtr data;
{
    if (data != DcNullPtr)
	    DDspace_Deallocate(data);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_delete_local_view_data (data)
     DtPtr data;
{
    if (data != DcNullPtr)
	    DDspace_Deallocate(data);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_delete_local_window_data (data)
     DtPtr data;
{
    if (data != DcNullPtr)
	    DDspace_Deallocate(data);
}
