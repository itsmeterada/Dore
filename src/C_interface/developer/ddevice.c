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
 
#include <dore/internal/dogen.h>
#include <dore/dore_develop/develop.h>
#include <dore/dore_develop/private/device.h>

/* ====================================================================== */

void DDdevice_InstallDriver (
    char  *name,
    char  *description,
    DtPFV  get_fcns_fcn)
{
    dor_device_install_driver (name, description, get_fcns_fcn);
}

/* ====================================================================== */

DDt_DCM *DDdevice_CreateDCMStruct (void)
{
    return dor_device_create_DCM_struct();
}

/* ====================================================================== */

DDt_PROM *DDdevice_CreatePROMStruct (void)
{
    return dor_device_create_PROM_struct();
}

/* ====================================================================== */

DDt_DROM *DDdevice_CreateDROMStruct (void)
{
    return dor_device_create_DROM_struct();
}

/* ====================================================================== */

void DDdevice_ClearRectangleDepth (
    DtObject device,
    DtInt    xmin,
    DtInt    ymin,
    DtInt    xmax,
    DtInt    ymax)
{
    dor_device_clear_rectangle_depth(device, xmin, ymin, xmax, ymax);
}

/* ====================================================================== */

void DDdevice_ClearRectangleDepthColor (
    DtObject device,
    DtInt    xmin,
    DtInt    ymin,
    DtInt    xmax,
    DtInt    ymax)
{
    dor_device_clear_rectangle_depth_and_color(device, xmin, ymin, xmax, ymax);
}

/* ====================================================================== */

void DDdevice_Flush (DtObject device)
{
    dor_device_flush (device);
}

/* ====================================================================== */

DtFlag DDdevice_InqAbortedDraw (DtObject device)
{
    return dor_device_get_aborted_draw_flag (device);
}

/* ====================================================================== */

void DDdevice_InqClipList (
    DtObject  device,
    DtUInt   *nrects,
    DtReal  **clip_list,
    DtReal    extent[4],
    DtFlag   *partiallyobscured)
{
    dor_device_inquire_clip_list (device, nrects, clip_list, extent,
				  partiallyobscured);
}

/* ====================================================================== */

DtVolume *DDdevice_InqClippingVolume (DtObject device)
{
    return dor_device_inq_clipping_volume_ptr (device);
}

/* ====================================================================== */

void DDdevice_InqFrameScaleTrans (
    DtObject device,
    DtReal   ftodscale[3],
    DtReal   ftodtranslate[3])
{
    dor_device_get_ftod_parms (device, ftodscale, ftodtranslate);
}

/* ====================================================================== */

void DDdevice_InqInterfaceRoutines (
    DtObject             device,
    DDt_DeviceInterface  type,
    DtPtr               *rtns)
{
    dor_device_inquire_interface_functions (device, type, rtns);
}

/* ====================================================================== */

void DDdevice_InqFrustumScaleTrans (
    DtObject device,
    DtReal   ctodscale[3],
    DtReal   ctodtranslate[3])
{
    dor_device_get_scltrn_parms (device, ctodscale, ctodtranslate);
}

/* ====================================================================== */

void DDdevice_InqShadeRange (
    DtObject device,
    DtInt    index,
    DtInt   *range_min,
    DtInt   *range_max)
{
    dor_device_inq_shade_range(device, index, range_min, range_max);
}

/* ====================================================================== */

DtFlag DDdevice_InqStereo (DtObject device)
{
    DtFlag stereo;

    dor_device_inquire_stereo (device, &stereo);
    return stereo;
}

/* ====================================================================== */

DtVolume *DDdevice_InqActualViewport (DtObject device)
{
    return dor_device_inq_actual_viewport_ptr (device);
}

/* ====================================================================== */

void DDdevice_SetAbortedDraw (
    DtObject device,
    DtFlag   switchvalue)
{
    dor_device_set_aborted_draw_flag (device, switchvalue);
}

/* ====================================================================== */

void DDdevice_SetBackgroundColor (
    DtObject     device,
    DtColorModel model,
    DtReal       color[])
{
    dor_device_set_background_color (device, model, color);
}

/* ====================================================================== */

void DDdevice_SetDepthBufferEnable (
    DtObject device,
    DtFlag   flag)
{
    dor_device_set_depth_buffer_enable (device, flag);
}

/* ====================================================================== */

void DDdevice_SetDepthBufferWrite (
    DtObject device,
    DtFlag   flag)
{
    dor_device_set_depth_buffer_write (device, flag);
}

/* ====================================================================== */

void DDdevice_SetForegroundColor (
    DtObject     device,
    DtColorModel model,
    DtReal       color[])
{
    dor_device_set_foreground_color (device, model, color);
}

/* ====================================================================== */

void DDdevice_SetShadeIndex (
    DtObject device,
    DtInt    index)
{
    dor_device_set_shade_index (device, index);
}
