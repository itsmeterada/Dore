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
 
/* ======================================================================
  Functions:
	ddr_rasterfile_dcm_inquire_color_entries
	ddr_rasterfile_dcm_inquire_auto_size
	ddr_rasterfile_dcm_inquire_clip_list
	ddr_rasterfile_dcm_inquire_device_extent
	ddr_rasterfile_dcm_inquire_ncolors
	ddr_rasterfile_dcm_inquire_pixel_data
	ddr_rasterfile_dcm_inquire_resolution
	ddr_rasterfile_dcm_inquire_visual_type
  ====================================================================== */

#include "dcm.h"

/* ====================================================================== */

void ddr_rasterfile_dcm_inquire_color_entries (
    DtColorModel colormodel,
    int start,
    int n,
    DtReal *c)
{
}

/* ====================================================================== */

void ddr_rasterfile_dcm_inquire_auto_size (
    DtFlag *flag)
{
   *flag = DcFalse;
}

/* ====================================================================== */

void ddr_rasterfile_dcm_inquire_clip_list (
    DtUInt *nrects,
    DtReal **clip_list,
    DtReal extent[4],
    DtFlag *partiallyobscuredflag)
{
   *nrects = 1;

   extent[0] = 0.0;
   extent[1] = 0.0;
   extent[2] = (DtReal)dde_rasterfile_current_device_data->width - 1.;
   extent[3] = (DtReal)dde_rasterfile_current_device_data->height - 1.;

   *clip_list = extent;

   *partiallyobscuredflag = DcFalse;
}

/* ====================================================================== */

void ddr_rasterfile_dcm_inquire_device_extent (
    DtVolume *volume)
{
   volume->bll[0] = 0.;
   volume->bll[1] = 0.;
   volume->bll[2] = 0.;
   volume->fur[0] = (DtReal)dde_rasterfile_current_device_data->width - 1.;
   volume->fur[1] = (DtReal)dde_rasterfile_current_device_data->height - 1.;
   volume->fur[2] = 65535.;
}

/* ====================================================================== */

void ddr_rasterfile_dcm_inquire_ncolors (
    DtInt *ncolors)
{
   *ncolors = 256;
}

/* ====================================================================== */

DtFlag ddr_rasterfile_dcm_inquire_pixel_data (
    DtRasterType requesttype,
    DtInt *width,
    DtInt *height,
    DtRasterType *type,
    DtPtr *data,
    DtFlag *userdelete)
{
   return (DcFalse);
}

/* ====================================================================== */

void ddr_rasterfile_dcm_inquire_resolution (
    DtReal *xres,
    DtReal *yres)
{
   *xres = .28125;
   *yres = .28125;
}

/* ====================================================================== */

void ddr_rasterfile_dcm_inquire_visual_type (
    DtVisualType *visualtype)
{
   *visualtype = DcTrueColor;
}
