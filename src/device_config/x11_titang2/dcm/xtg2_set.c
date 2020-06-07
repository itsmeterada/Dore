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
	void ddr_x11_titang2_dcm_set_background_color (colormodel, c)
	void ddr_x11_titang2_dcm_set_foreground_color (colormodel, c)
	void ddr_x11_titang2_dcm_set_color_entries (colormodel, start, n, c)
	void ddr_x11_titang2_dcm_set_current_view (view)
	void ddr_x11_titang2_dcm_set_depth_buffer_enable (flag)
	void ddr_x11_titang2_dcm_set_depth_buffer_write (flag)
	int ddr_x11_titang2_dcm_set_depth_buffering (depthswitch)
	void ddr_x11_titang2_dcm_set_line_pattern (linepattern)
	void ddr_x11_titang2_dcm_set_pixel_function (pixelop)
	int ddr_x11_titang2_dcm_set_pixel_clip (pixelclip)
	void ddr_x11_titang2_dcm_set_shade_index (index)
	void ddr_x11_titang2_dcm_set_z_function (z_function)

  ======================================================================
 */
#include "dcm.h"
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_set_background_color (colormodel,c)
     DtColorModel colormodel;
     DtReal *c;
{
    DtInt i;
    DtInt index;

    for (i=0; i<3; i++) 
	    dde_x11_titang2_current_dcm_data->background_color[i] = c[i];

    if (dde_x11_titang2_current_dcm_data->visualtype == DcPseudoColor) {

	ddr_x11_titang2_dcm_setup_shading();

	if (dde_x11_titang2_current_dcm_data->shade_mode == DcComponent) {
	    index =  ((int)(dde_x11_titang2_current_dcm_data->cbtselpsweight[0]*c[0]*255.)
		      & dde_x11_titang2_current_dcm_data->cbtselpsmask[0]) + 
			      ((int)(dde_x11_titang2_current_dcm_data->cbtselpsweight[1]*c[1]*255.)
			       & dde_x11_titang2_current_dcm_data->cbtselpsmask[1]) +
				       ((int)(dde_x11_titang2_current_dcm_data->cbtselpsweight[2]*c[2]*255.)
					& dde_x11_titang2_current_dcm_data->cbtselpsmask[2]);
	    index = index < 0 ? 0 : index > 255 ? 255 : index;
	} else if (dde_x11_titang2_current_dcm_data->shade_mode == DcRange) {
	    index = dde_x11_titang2_current_dcm_data->cwtavgpslim[0] +
		    (dde_x11_titang2_current_dcm_data->cwtavgpsweight[0]*c[0]*255. +
		     dde_x11_titang2_current_dcm_data->cwtavgpsweight[1]*c[1]*255. +
		     dde_x11_titang2_current_dcm_data->cwtavgpsweight[2]*c[2]*255.)*
			     dde_x11_titang2_current_dcm_data->cwtavgpslim[2];
	    if (index > dde_x11_titang2_current_dcm_data->cwtavgpslim[1]) 
		    index = dde_x11_titang2_current_dcm_data->cwtavgpslim[1];
	    index = index < 0 ? 0 : index > 255 ? 255 : index;
	}

	dde_x11_titang2_current_dcm_data->bg_color_index = (short)index;
    }
}

/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_set_foreground_color (colormodel,c)
     DtColorModel colormodel;
     DtReal *c;
{
    DtShort r, g, b;

    if (255<(r = c[0]*256)) r=255; if (0>r) r=0;
    if (255<(g = c[1]*256)) g=255; if (0>g) g=0;
    if (255<(b = c[2]*256)) b=255; if (0>b) b=0;

    dde_x11_titang2_tigr_state.fg_r = r;
    dde_x11_titang2_tigr_state.fg_g = g;
    dde_x11_titang2_tigr_state.fg_b = b;
    tg_foreground(r,g,b);
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_set_color_entries (colormodel,start, n, c)
     DtColorModel colormodel;
     int start;
     int n;
     DtReal *c;
{
    XColor colors[256];
    Colormap cmap ;
    int i, j;
    DtReal r;

    XGetWindowAttributes(XDISPLAY(dde_x11_titang2_current_dcm_data),
			 XWINDOW(dde_x11_titang2_current_dcm_data),
			 &XWINDOWATTRIBUTES(dde_x11_titang2_current_dcm_data));

    cmap = XWINDOWATTRIBUTES(dde_x11_titang2_current_dcm_data).colormap;

    for (i=0; i<n; i++) {
	if (65535 < (r = 65535*c[i*3])) r = 65535; if (0> r) r = 0;
	colors[i].red = r;
	if (65535 < (r = 65535*c[i*3+1])) r = 65535; if (0> r) r = 0;
	colors[i].green = r;
	if (65535 < (r = 65535*c[i*3+2])) r = 65535; if (0> r) r = 0;
	colors[i].blue = r;
	colors[i].flags = DoRed|DoGreen|DoBlue;
	j = start+i;
	if (dde_x11_titang2_current_dcm_data->visualtype == DcDirectColor)
		colors[i].pixel = (j<<16)|(j<<8)|j;
	else colors[i].pixel = j;
    }

    XStoreColors(XDISPLAY(dde_x11_titang2_current_dcm_data), cmap, colors, n);
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_set_current_view (view)
     DtObject view;
{
    dde_x11_titang2_current_dcm_data->previous_view = 
	    dde_x11_titang2_current_dcm_data->current_view;
    dde_x11_titang2_current_dcm_data->current_view = view;


    if (XDoubleBuffered(dde_x11_titang2_current_dcm_data) &&
	((dde_x11_titang2_current_dcm_data->current_view != DcNullObject)&&
	 (dde_x11_titang2_current_dcm_data->current_view !=
	  dde_x11_titang2_current_dcm_data->previous_view))) {
#ifdef DEBUG
	printf ("blting front to back before update\n");
#endif
	ddr_x11_titang2_dcm_blt_front_to_back();
    }
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_set_depth_buffer_enable (flag)
     DtFlag flag;
{
    tg_set_depth_buffer_enable((DtInt)flag);
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_set_depth_buffer_write (flag)
     DtFlag flag;
{
    tg_z_write_enable((int)flag);
}
/*
 ======================================================================
 */

ddr_x11_titang2_dcm_set_depth_buffering(depthswitch)
     DtSwitch depthswitch;

{
    if (depthswitch) {
	if (ExpansionBoard(dde_x11_titang2_current_dcm_data)) {
	    dde_x11_titang2_tigr_state.select = RC_SELECT_ALL;
	    tg_ras_sel(RC_SELECT_ALL);
	} else {
	    dde_x11_titang2_tigr_state.select = RC_SELECT_R|RC_SELECT_Z;
	    tg_ras_sel(RC_SELECT_R|RC_SELECT_Z);
	}
    } else {
	if (ExpansionBoard(dde_x11_titang2_current_dcm_data)) {
	    dde_x11_titang2_tigr_state.select = RC_SELECT_FULL;
	    tg_ras_sel(RC_SELECT_FULL);
	}	else {
	    dde_x11_titang2_tigr_state.select = RC_SELECT_R;
	    tg_ras_sel(RC_SELECT_R);
	}
    }
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_set_line_pattern (linepattern)
     DtUShort linepattern;
{
    dde_x11_titang2_tigr_state.vector_pattern = linepattern;
    tg_set_vector_pattern(linepattern);
}

/*
  ======================================================================
 */
void
ddr_x11_titang2_dcm_set_pixel_function (pixelop)
     DtUShort pixelop;
{
    extern int _taskno;

    dde_x11_titang2_tigr_state.pixel_function = pixelop;

    if (doe_system_main_processors > 0) {
	for (_taskno=0; _taskno<doe_system_main_processors;_taskno++) {
	    tg_set_pixel_function(pixelop);
	}
	_taskno = 0;
    } else {
	tg_set_pixel_function(pixelop);
    }
}
/*
 ======================================================================
 */
ddr_x11_titang2_dcm_set_pixel_clip(pixelclip)
     DtInt pixelclip;
{
   extern int _taskno;

   dde_x11_titang2_tigr_state.pixel_clip = pixelclip;

   if (doe_system_main_processors > 0) {
      for (_taskno=0; _taskno<doe_system_main_processors;_taskno++) {
	 tg_set_pixel_clip(pixelclip);
      }
      _taskno = 0;
   } else {
      tg_set_pixel_clip(pixelclip);
   }
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_set_shade_index (index)
     DtInt index;
{
    dde_x11_titang2_current_dcm_data->shade_index = index;
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_set_z_function (z_function)
     DtShort z_function;
{
    dde_x11_titang2_tigr_state.z_function = z_function;
    tg_set_depth_function(z_function);
}
