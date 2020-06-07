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
	void ddr_sampledev_dcm_inquire_color_entries (colormodel,start, n, c)
	void ddr_sampledev_dcm_inquire_auto_size (flag)
	void ddr_sampledev_dcm_inquire_clip_list (nrects, clip_list, extent, partiallyobscuredflag)
	void ddr_sampledev_dcm_inquire_device_extent (volume)
	void ddr_sampledev_dcm_inquire_ncolors (ncolors)
	DtFlag ddr_sampledev_dcm_inquire_pixel_data (requesttype, width, height, type, data, userdelete)
	void ddr_sampledev_dcm_inquire_resolution (xres, yres)
	void ddr_sampledev_dcm_inquire_stereo (stereoswitch)
	void ddr_sampledev_dcm_inquire_visual_type (visualtype)

  ======================================================================
 */
#include "dcm.h"
/*
 ======================================================================
 */
void
ddr_sampledev_dcm_inquire_color_entries (colormodel,start, n, c)
DtColorModel colormodel;
int start;
int n;
DtReal *c;
{
}
/*
 ======================================================================
 */
void
ddr_sampledev_dcm_inquire_auto_size (flag)
DtFlag *flag;
{
    *flag = dde_sampledev_current_device_data->auto_size;
}
/*
 ======================================================================
 */
void
ddr_sampledev_dcm_inquire_clip_list (nrects, clip_list, extent, partiallyobscuredflag)
DtUInt *nrects;
DtReal **clip_list;
DtReal extent[4];
DtFlag *partiallyobscuredflag;
{
    static DtReal clist[4];

    clist[0] = extent[0] = 0;
    clist[1] = extent[1] = 0;
    clist[2] = extent[2] = dde_sampledev_current_device_data->width;
    clist[3] = extent[3] = dde_sampledev_current_device_data->height;

    *nrects = 1;
    *clip_list = clist;
    *partiallyobscuredflag = DcFalse;
}
/*
 ======================================================================
 */
void
ddr_sampledev_dcm_inquire_device_extent (volume)
DtVolume *volume;
{
    volume->bll[0] = 0.0;
    volume->bll[1] = 0.0;
    volume->bll[2] = 0.0;
    volume->fur[0] = dde_sampledev_current_device_data->width;
    volume->fur[1] = dde_sampledev_current_device_data->height;
    volume->fur[2] = 1.0;
}
/*
 ======================================================================
 */
void
ddr_sampledev_dcm_inquire_ncolors (ncolors)
DtInt *ncolors;
{
    *ncolors = 256;
}
/*
 ======================================================================
 */
DtFlag
ddr_sampledev_dcm_inquire_pixel_data (requesttype, width, height, type, 
				 data, userdelete)
DtRasterType requesttype;
DtInt *width;
DtInt *height;
DtRasterType *type;
DtPtr *data;
DtFlag *userdelete;
{
    return (DcFalse);
}
/*
 ======================================================================
 */
void
ddr_sampledev_dcm_inquire_resolution (xres, yres)
DtReal *xres;
DtReal *yres;
{
    *xres = .28125;
    *yres = .28125;
}
/*
 ======================================================================
 */
void
ddr_sampledev_dcm_inquire_stereo (stereoswitch)
DtFlag *stereoswitch;
{
    *stereoswitch = DcFalse;
}
/*
 ======================================================================
 */
void
ddr_sampledev_dcm_inquire_visual_type (visualtype)
DtVisualType *visualtype;
{
    *visualtype = dde_sampledev_current_device_data->visualtype;
}
/*
 ======================================================================
 */
