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
	DtPtr ddr_template_drom_create_local_device_data (device)
	DtPtr ddr_template_drom_create_local_view_data (view)
	DtPtr ddr_template_drom_create_local_window_data (device, view)
	void ddr_template_drom_delete_local_device_data (data)
	void ddr_template_drom_delete_local_view_data (data)
	void ddr_template_drom_delete_local_window_data (data)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
DtPtr ddr_template_drom_create_local_device_data (
    DtObject device)
{
    ddt_template_drom_device *devicedata;

    devicedata = (ddt_template_drom_device *)DDspace_Allocate
            (sizeof(ddt_template_drom_device));

    dde_template_drom_data.device = device;
    dde_template_drom_curdevdat = devicedata;

    return ((DtPtr)devicedata);
}
/*
 ======================================================================
 */
DtPtr ddr_template_drom_create_local_view_data (
    DtObject view)
{
    ddt_template_drom_view *viewdata;

    viewdata = (ddt_template_drom_view *)DDspace_Allocate
            (sizeof (ddt_template_drom_view));

    viewdata->total_lights = 0;
    viewdata->max_lights = 0;
    viewdata->light = 0;

    dde_template_drom_data.view = view;
    dde_template_drom_curviwdat = viewdata;

    return((DtPtr)viewdata);
}
/*
 ======================================================================
 */
DtPtr ddr_template_drom_create_local_window_data (
    DtObject device,
    DtObject view)
{
    ddt_template_drom_window *windowdata;

    windowdata = (ddt_template_drom_window *)DDspace_Allocate
            (sizeof (ddt_template_drom_window));

    dde_template_drom_curwindat = windowdata;

    return((DtPtr)windowdata);
}
/*
 ======================================================================
 */
void ddr_template_drom_delete_local_device_data (
    ddt_template_drom_device *data)
{
    if (data != (ddt_template_drom_device *)0)
	    DDspace_Deallocate(data);
}
/*
 ======================================================================
 */
void ddr_template_drom_delete_local_view_data (
    ddt_template_drom_view *data)
{
    if (data != (ddt_template_drom_view *)0) {
	if (data->max_lights > 0 && data->light != (ddt_template_drom_light *)0)
		DDspace_Deallocate(data->light);

	DDspace_Deallocate(data);
    }
}
/*
 ======================================================================
 */
void ddr_template_drom_delete_local_window_data (
    ddt_template_drom_window *data)
{
    if (data != (ddt_template_drom_window *)0)
	    DDspace_Deallocate(data);
}
/*
 ======================================================================
 */
