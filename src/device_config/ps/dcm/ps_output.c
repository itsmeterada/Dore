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
	ddr_ps_dcm_clear_rectangle_depth
	ddr_ps_dcm_clear_rectangle_depth_and_color
	ddr_ps_dcm_write_scanline_byte
	ddr_ps_DrawPoint
	ddr_ps_DrawLine
	ddr_ps_DrawTriangle
  ====================================================================== */

#include "dcm.h"

/* ====================================================================== */

void ddr_ps_dcm_clear_rectangle_depth (
    DtShort xmin,
    DtShort ymin,
    DtShort xmax,
    DtShort ymax)
{
#   ifdef DEBUG
	printf ("... processing device clear rectangle depth ...\n");
	fflush (stdout);
#   endif
}

/* ====================================================================== */

void ddr_ps_dcm_clear_rectangle_depth_and_color (
    DtShort xmin,
    DtShort ymin,
    DtShort xmax,
    DtShort ymax)
{
    ddt_ps_data *device_data;
    float gray;

    device_data = dde_ps_current_device_data;

#ifdef DEBUG
    printf("... processing device clear depth and color ...\n");
    fflush(stdout);
#endif

    if (device_data->begin_page) ddr_ps_dcm_start_page();

    if (device_data->border)
	    fprintf (device_data->fp, "0 %d %d %d %d BDR\n",
		     xmin, ymin, xmax, ymax);

    switch (device_data->visualtype) {
    case DcStaticGrey:
	gray = (device_data->cwtavgpsweight[0]*device_data->bg_color[0] +
		device_data->cwtavgpsweight[1]*device_data->bg_color[1] +
		device_data->cwtavgpsweight[2]*device_data->bg_color[2]);

	fprintf (device_data->fp, "%f %d %d %d %d BGgray\n", gray,
		 xmin, ymin, xmax, ymax);
	break;
    case DcTrueColor:
	fprintf (device_data->fp, "%f %f %f %d %d %d %d BGrgb\n", 
		 device_data->bg_color[0], device_data->bg_color[1],
		 device_data->bg_color[2], xmin, ymin, xmax, ymax);
	break;
    }
}

/* ====================================================================== */

void ddr_ps_dcm_write_scanline_byte (
    DtInt xstart,
    DtInt ystart,
    DtInt length,
    DtUChar *data)
{
    ddt_ps_data *device_data;

#ifdef DEBUG
    printf("...processing device write scanline byte\n");
    fflush(stdout);
#endif

    device_data = dde_ps_current_device_data;
}

/* ====================================================================== */

void ddr_ps_DrawPoint (
    DtInt *point,
    DtColorRGB color)
{
    ddt_ps_data *device_data;
    float gray;

#ifdef DEBUG
    printf("...processing ddr_ps_DrawPoint\n");
    fflush(stdout);
#endif

    device_data = dde_ps_current_device_data;

    if (device_data->begin_page) ddr_ps_dcm_start_page();

    switch (device_data->visualtype) {
    case DcStaticGrey:
	gray = (device_data->cwtavgpsweight[0]*color[0] +
		device_data->cwtavgpsweight[1]*color[1] +
		device_data->cwtavgpsweight[2]*color[2]);

	fprintf (device_data->fp, "%f %d %d Pgray\n", gray, point[0], point[1]);
	break;
    case DcTrueColor:
	fprintf (device_data->fp, "%f %f %f %d %d Prgb\n",
		 color[0], color[1], color[2], point[0], point[1]);
	break;
    }
}

/* ====================================================================== */

void ddr_ps_DrawLine (
    DtInt *point1,
    DtInt *point2,
    DtColorRGB color1,
    DtColorRGB color2)
{
    DtColorRGB color;
    ddt_ps_data *device_data;
    float gray;

#ifdef DEBUG
    printf("...processing ddr_ps_DrawLine\n");
    fflush(stdout);
#endif

    device_data = dde_ps_current_device_data;

    if (device_data->begin_page) ddr_ps_dcm_start_page();

    color[0] = (color1[0] + color2[0]) / 2.0;
    color[1] = (color1[1] + color2[1]) / 2.0;

    switch (device_data->visualtype) {
    case DcStaticGrey:
	gray = (device_data->cwtavgpsweight[0]*color[0] +
		device_data->cwtavgpsweight[1]*color[1] +
		device_data->cwtavgpsweight[2]*color[2]);

	fprintf (device_data->fp, "%f %d %d %d %d Lgray\n", gray, 
		 point1[0], point1[1], point2[0], point2[1]);
	break;
    case DcTrueColor:
	fprintf (device_data->fp, "%f %f %f %d %d %d %d Lrgb\n",
		 color[0], color[1], color[2], point1[0], point1[1],
		 point2[0], point2[1]);
	break;
    }
}

/* ====================================================================== */

void ddr_ps_DrawTriangle (
    DtInt *point1,
    DtInt *point2,
    DtInt *point3,
    DtColorRGB color1,
    DtColorRGB color2,
    DtColorRGB color3)
{
    DtColorRGB color;
    float gray;
    ddt_ps_data *device_data;

#ifdef DEBUG
    printf("...processing ddr_ps_DrawTriangle\n");
    fflush(stdout);
#endif

    device_data = dde_ps_current_device_data;

    if (device_data->begin_page) ddr_ps_dcm_start_page();

    color[0] = (color1[0] + color2[0] + color3[0]) / 3.0;
    color[1] = (color1[1] + color2[1] + color3[1]) / 3.0;
    color[2] = (color1[2] + color2[2] + color3[2]) / 3.0;

    switch (device_data->visualtype) {
    case DcStaticGrey:
	gray = (device_data->cwtavgpsweight[0]*color[0] +
		device_data->cwtavgpsweight[1]*color[1] +
		device_data->cwtavgpsweight[2]*color[2]);

	fprintf (device_data->fp, "%f %d %d %d %d %d %d Tgray\n", gray,
		 point1[0], point1[1], point2[0], point2[1], 
		 point3[0], point3[1]);
	break;
    case DcTrueColor:
	fprintf (device_data->fp, "%f %f %f %d %d %d %d %d %d Trgb\n",
		 color[0], color[1], color[2], point1[0], point1[1],
		 point2[0], point2[1], point3[0], point3[1]);
	break;
    }
}
