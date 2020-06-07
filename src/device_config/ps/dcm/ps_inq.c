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
	ddr_ps_dcm_inquire_color_entries
	ddr_ps_dcm_inquire_auto_size
	ddr_ps_dcm_inquire_clip_list
	ddr_ps_dcm_inquire_device_extent
	ddr_ps_dcm_inquire_ncolors
	ddr_ps_dcm_inquire_pixel_data
	ddr_ps_dcm_inquire_resolution
	ddr_ps_dcm_inquire_stereo
	ddr_ps_dcm_inquire_visual_type

  ====================================================================== */

#include "dcm.h"

/* ====================================================================== */

void ddr_ps_dcm_inquire_color_entries (
    DtColorModel colormodel,
    int start,
    int n,
    DtReal *c)
{
#   ifdef DEBUG
	printf("... processing device inquire color entries ...\n");
	printf("  Not yet implemented.\n");
	fflush(stdout);
#   endif
}

/* ====================================================================== */

void ddr_ps_dcm_inquire_auto_size (DtFlag *flag)
{
#   ifdef DEBUG
	printf("... processing device inquire auto size ...\n");
	fflush(stdout);
#   endif

    *flag = dde_ps_current_device_data->auto_size;
}

/* ====================================================================== */

void ddr_ps_dcm_inquire_clip_list (
    DtUInt *nrects,
    DtReal **clip_list,
    DtReal extent[4],
    DtFlag *partiallyobscuredflag)
{
    static DtReal clist[4];

#   ifdef DEBUG
	printf("... processing device inquire clip list ...\n");
	fflush(stdout);
#   endif

    clist[0] = extent[0] = 0;
    clist[1] = extent[1] = 0;
    clist[2] = extent[2] = dde_ps_current_device_data->width * 72.;
    clist[3] = extent[3] = dde_ps_current_device_data->height * 72.;

#   ifdef DEBUG
	printf ("extent = (%lf,%lf)-(%lf,%lf)\n",
	    extent[0],extent[1],extent[2],extent[3]);
#   endif

    *nrects = 1;
    *clip_list = clist;
    *partiallyobscuredflag = DcFalse;
}

/* ====================================================================== */

void ddr_ps_dcm_inquire_device_extent (DtVolume *volume)
{
#   ifdef DEBUG
	printf ("... processing device inquire device extent ...\n");
	fflush (stdout);
#   endif

    volume->bll[0] = 0.0;
    volume->bll[1] = 0.0;
    volume->bll[2] = 0.0;
    volume->fur[0] = dde_ps_current_device_data->width * 72.;
    volume->fur[1] = dde_ps_current_device_data->height * 72.;
    volume->fur[2] = 1.0;
}

/* ====================================================================== */

void ddr_ps_dcm_inquire_ncolors (DtInt *ncolors)
{
    ddt_ps_data *dev;

#   ifdef DEBUG
	printf ("... processing device inquire ncolors ...\n");
	fflush (stdout);
#   endif

    dev = dde_ps_current_device_data;

    *ncolors = 256;
}

/* ====================================================================== */

DtFlag ddr_ps_dcm_inquire_pixel_data (
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

void ddr_ps_dcm_inquire_resolution (
    DtReal *xres,
    DtReal *yres)
{
#   ifdef DEBUG
	printf ("... processing device inquire resolution ...\n");
	fflush (stdout);
#   endif

    *xres = .03528;
    *yres = .03528;
}

/* ====================================================================== */

void ddr_ps_dcm_inquire_stereo (
    DtFlag *stereoswitch)
{
#   ifdef DEBUG
	printf ("... processing device inquire stereo ...\n");
	fflush (stdout);
#   endif

    *stereoswitch = DcFalse;
}

/* ====================================================================== */

void ddr_ps_dcm_inquire_visual_type (
    DtVisualType *visualtype)
{
#   ifdef DEBUG
	printf ("... processing device inquire visual type ...\n");
	fflush (stdout);
#   endif

    *visualtype = dde_ps_current_device_data->visualtype;
}
