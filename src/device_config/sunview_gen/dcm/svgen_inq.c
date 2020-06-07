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
	void ddr_sunview_gen_dcm_inquire_color_entries (colormodel,start, n, c)
	void ddr_sunview_gen_dcm_inquire_auto_size (flag)
	void ddr_sunview_gen_dcm_inquire_clip_list (nrects, clip_list, extent, partiallyobscuredflag)
	void ddr_sunview_gen_dcm_inquire_device_extent (volume)
	void ddr_sunview_gen_dcm_inquire_ncolors (ncolors)
	DtFlag ddr_sunview_gen_dcm_inquire_pixel_data (requesttype, width, height, type, data, userdelete)
	void ddr_sunview_gen_dcm_inquire_resolution (xres, yres)
	void ddr_sunview_gen_dcm_inquire_stereo (stereoswitch)
	void ddr_sunview_gen_dcm_inquire_visual_type (visualtype)

  ======================================================================
 */
#include "dcm.h"

/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_inquire_color_entries (colormodel,start, n, c)
DtColorModel colormodel;
int start;
int n;
DtReal *c;
{
   static unsigned char r[256], g[256], b[256];
   int i;
   int k;

   if (dde_sunview_gen_current_device_data->visualtype == DcPseudoColor) {
      pw_getcolormap (dde_sunview_gen_current_device_data->pw, start, 
		      n, r, g, b);

      for (i=0; i<n; i++) {
	 c[i*3+0] = r[i] / 255.0;
	 c[i*3+1] = g[i] / 255.0;
	 c[i*3+2] = b[i] / 255.0;
      }
   }
}

/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_inquire_auto_size (flag)
DtFlag *flag;
{
   *flag = dde_sunview_gen_current_device_data->auto_size;
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_inquire_clip_list (nrects, clip_list,
					 extent, partiallyobscuredflag)
DtUInt *nrects;
DtReal **clip_list;
DtReal extent[4];
DtFlag *partiallyobscuredflag;
{
   ddt_sunview_gen_data *device_data;
   static DtReal clist[4];

   *nrects = 1;

   device_data = dde_sunview_gen_current_device_data;

   clist[0] = extent[0] = 0;
   clist[1] = extent[1] = 0;
   clist[2] = extent[2] = device_data->width - 1;
   clist[3] = extent[3] = device_data->height - 1;
   *clip_list= clist;
   *partiallyobscuredflag = DcFalse;

   if ((device_data->width !=
	(int)window_get (device_data->canvas, CANVAS_WIDTH, 0)) ||
       (device_data->height !=
	(int)window_get (device_data->canvas, CANVAS_HEIGHT, 0))) {
      device_data->update_needed = DcTrue;
      DDdevice_SetAbortedDraw (device_data->device, DcTrue);
   }
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_inquire_device_extent (volume)
DtVolume *volume;
{
   dde_sunview_gen_current_device_data->width = 
	 (int)window_get (dde_sunview_gen_current_device_data->canvas, 
			  CANVAS_WIDTH, 0);
   dde_sunview_gen_current_device_data->height = 
	 (int)window_get (dde_sunview_gen_current_device_data->canvas, 
			  CANVAS_HEIGHT, 0);

   volume->bll[0] = 0.0;
   volume->bll[1] = 0.0;
   volume->bll[2] = 0.0;
   volume->fur[0] = dde_sunview_gen_current_device_data->width - 1;
   volume->fur[1] = dde_sunview_gen_current_device_data->height - 1;
   volume->fur[2] = 65535.0;

}

/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_inquire_ncolors (ncolors)
DtInt *ncolors;
{
   if (dde_sunview_gen_current_device_data->visualtype == DcStaticGrey) {
      *ncolors = 2;
   } else {
      *ncolors = 256;
   }
}
/*
 ======================================================================
 */
DtFlag
ddr_sunview_gen_dcm_inquire_pixel_data (requesttype, width, height, type, 
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
ddr_sunview_gen_dcm_inquire_resolution (xres, yres)
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
ddr_sunview_gen_dcm_inquire_stereo (stereoswitch)
DtFlag *stereoswitch;
{
   *stereoswitch = DcFalse;
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_inquire_visual_type (visualtype)
DtVisualType *visualtype;
{
   *visualtype = dde_sunview_gen_current_device_data->visualtype;
}
