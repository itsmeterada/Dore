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
	DtPtr ddr_g4vll_drom_create_local_device_data (device)
	DtPtr ddr_g4vll_drom_create_local_view_data (view)
	DtPtr ddr_g4vll_drom_create_local_window_data (device, view)
	void ddr_g4vll_drom_delete_local_device_data (data)
	void ddr_g4vll_drom_delete_local_view_data (data)
	void ddr_g4vll_drom_delete_local_window_data (data)

  ======================================================================
 */
#include "drom.h"
#include <dore/internal/matrix.h>



/* ====================================================================== */

DtPtr ddr_g4vll_drom_create_local_device_data (
    DtObject device)
{
    ddt_g4vll_drom_device *devicedata;
    DtInt shaderange[2];

    devicedata = (ddt_g4vll_drom_device *)DDspace_Allocate
	    (sizeof(ddt_g4vll_drom_device));

    devicedata->visual_type  		= DdInqVisualType(device);
    devicedata->shade_mode   		= DdInqShadeMode(device);
    devicedata->stereoswitch 		= DDdevice_InqStereo(device);
    devicedata->use_spheres  		= ddr_g4vll_dcm_inquire_spheres();
    devicedata->use_hw_boundingvol  	= ddr_g4vll_dcm_inquire_hw_boundingvol();
    devicedata->features     		= VLLquery_features();
    devicedata->stereoscreen 		= ddr_g4vll_dcm_inquire_stereo_screen();
    devicedata->head = ddr_g4vll_dcm_inquire_head_number();
    ddr_g4vll_dcm_inquire_state(&(devicedata->vll_state));

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
    ddr_g4vll_dcm_inq_pseudo_range(devicedata->pseudo_range_weight);
    ddr_g4vll_dcm_inq_pseudo_space(devicedata->pseudo_space_weight,
				   devicedata->pseudo_space_mask);

    dde_g4vll_drom_data.current_device = device;
    dde_g4vll_drom_curdevdat = devicedata;

    return((DtPtr)devicedata);
}
/*
 ======================================================================
 */
DtPtr ddr_g4vll_drom_create_local_view_data (DtObject view)
{
    ddt_g4vll_drom_view *viewdata;

    viewdata = DDspace_Allocate (sizeof (ddt_g4vll_drom_view));

    viewdata->total_lights = 0;
    viewdata->max_lights = 0;
    viewdata->have_textures = DcFalse;
    viewdata->light = 0;
    viewdata->camera_data.filter = (int *)0;

    /* Initialize camera matrices to the identity matrix. */

    dor_matrix_load_identity (viewdata->camera_data.projmat);
    dor_matrix_load_identity (viewdata->camera_data.camera_to_world_matrix);
    dor_matrix_load_identity (viewdata->camera_data.world_to_camera_matrix);
    dor_matrix_load_identity (viewdata->camera_data.npctowcsmat);
    dor_matrix_load_identity (viewdata->camera_data.world_to_frustum_matrix);
    dor_matrix_load_identity (viewdata->camera_data.tspfcstowcsmat);
    dor_matrix_load_identity
	(viewdata->camera_data.world_to_frustum_matrix_left);
    dor_matrix_load_identity
	(viewdata->camera_data.world_to_frustum_matrix_right);
    dor_matrix_load_identity (viewdata->camera_data.tspfcstowcsmat_left);
    dor_matrix_load_identity (viewdata->camera_data.tspfcstowcsmat_right);

    dde_g4vll_drom_data.current_view = view;
    dde_g4vll_drom_curviwdat = viewdata;

    return (DtPtr) viewdata;
}
/*
 ======================================================================
 */
DtPtr ddr_g4vll_drom_create_local_window_data (
    DtObject device,
    DtObject view)
{
    ddt_g4vll_drom_window *windowdata;

    windowdata = (ddt_g4vll_drom_window *)DDspace_Allocate
	    (sizeof (ddt_g4vll_drom_window));

    dde_g4vll_drom_curwindat = windowdata;

    return((DtPtr)windowdata);
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_delete_local_device_data (
    DtPtr data)
{
    if (data != DcNullPtr)
	    DDspace_Deallocate(data);
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_delete_local_view_data (DtPtr data_parm)
{
#   define data ((ddt_g4vll_drom_view*)(data_parm))

    if ((data->max_lights > 0) && (data->light))
	DDspace_Deallocate (data->light);

    if (data->camera_data.filter)
	DDspace_Deallocate (data->camera_data.filter);

    if (data) DDspace_Deallocate (data);

#   undef data
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_delete_local_window_data (
    DtPtr data)
{
    if (data != DcNullPtr)
	    DDspace_Deallocate(data);
}
/*
 ======================================================================
 */
