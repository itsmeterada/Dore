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
	void ddr_x11_DGL_dcm_set_background_color (colormodel, c)
	void ddr_x11_DGL_dcm_set_foreground_color (colormodel, c)
	void ddr_x11_DGL_dcm_set_color_entries (colormodel,start, n, c)
	void ddr_x11_DGL_dcm_set_current_view (view)
	void ddr_x11_DGL_dcm_set_depth_buffer_enable (flag)
	void ddr_x11_DGL_dcm_set_depth_buffer_write (flag)
	int ddr_x11_DGL_dcm_set_left_eye (flag)
	void ddr_x11_DGL_dcm_set_line_pattern (linepattern)
	void ddr_x11_DGL_dcm_set_pixel_function (pixelop)
	void ddr_x11_DGL_dcm_set_shade_index (index)
	void ddr_x11_DGL_dcm_set_z_function (z_function)

  ======================================================================
 */
#include "dcm.h"
/*
 ======================================================================
 */
void
ddr_x11_DGL_dcm_set_background_color (colormodel,c)
     DtColorModel colormodel;
     DtReal *c;
{
    DtInt i;
    DtInt index;

    for (i=0; i<3; i++) 
	    dde_x11_DGL_current_dcm_data->background_color[i] = c[i];

    if (dde_x11_DGL_current_dcm_data->visualtype == DcPseudoColor) {

	ddr_x11_DGL_dcm_setup_shading();

	if (dde_x11_DGL_current_dcm_data->shade_mode == DcComponent) {
	    index =  ((int)(dde_x11_DGL_current_dcm_data->cbtselpsweight[0]*c[0]*255.)
		      & dde_x11_DGL_current_dcm_data->cbtselpsmask[0]) + 
			      ((int)(dde_x11_DGL_current_dcm_data->cbtselpsweight[1]*c[1]*255.)
			       & dde_x11_DGL_current_dcm_data->cbtselpsmask[1]) +
				       ((int)(dde_x11_DGL_current_dcm_data->cbtselpsweight[2]*c[2]*255.)
					& dde_x11_DGL_current_dcm_data->cbtselpsmask[2]);
	    index = index < 0 ? 0 : index > 255 ? 255 : index;
	} else if (dde_x11_DGL_current_dcm_data->shade_mode == DcRange) {
	    index = dde_x11_DGL_current_dcm_data->cwtavgpslim[0] +
		    (dde_x11_DGL_current_dcm_data->cwtavgpsweight[0]*c[0]*255. +
		     dde_x11_DGL_current_dcm_data->cwtavgpsweight[1]*c[1]*255. +
		     dde_x11_DGL_current_dcm_data->cwtavgpsweight[2]*c[2]*255.)*
			     dde_x11_DGL_current_dcm_data->cwtavgpslim[2];
	    if (index > dde_x11_DGL_current_dcm_data->cwtavgpslim[1]) 
		    index = dde_x11_DGL_current_dcm_data->cwtavgpslim[1];
	    index = index < 0 ? 0 : index > 255 ? 255 : index;
	}

	dde_x11_DGL_current_dcm_data->bg_color_index = (short)index;
    }
}
/*
 ======================================================================
 */
void
ddr_x11_DGL_dcm_set_foreground_color (colormodel,c)
     DtColorModel colormodel;
     DtReal *c;
{
    unsigned long color;

    color = (((int)(255.*c[0]) << 16) +
	     ((int)(255.*c[1]) << 8) +
	     ((int)(255.*c[2])));

    XdSetForeground (dde_x11_DGL_current_dcm_data->pGC, color);
}
/*
 ======================================================================
 */
void
ddr_x11_DGL_dcm_set_color_entries (colormodel,start, n, c)
     DtColorModel colormodel;
     int start;
     int n;
     DtReal *c;
{
    XColor colors[256];
    Colormap cmap ;
    int i, j;
    DtReal r;

    XGetWindowAttributes(XDISPLAY(dde_x11_DGL_current_dcm_data),
			 XWINDOW(dde_x11_DGL_current_dcm_data),
			 &XWINDOWATTRIBUTES(dde_x11_DGL_current_dcm_data));

    cmap = XWINDOWATTRIBUTES(dde_x11_DGL_current_dcm_data).colormap;

    for (i=0; i<n; i++) {
	if (65535 < (r = 65535*c[i*3])) r = 65535; if (0> r) r = 0;
	colors[i].red = r;
	if (65535 < (r = 65535*c[i*3+1])) r = 65535; if (0> r) r = 0;
	colors[i].green = r;
	if (65535 < (r = 65535*c[i*3+2])) r = 65535; if (0> r) r = 0;
	colors[i].blue = r;
	colors[i].flags = DoRed|DoGreen|DoBlue;
	j = start+i;
	if (dde_x11_DGL_current_dcm_data->visualtype == DcDirectColor)
		colors[i].pixel = (j<<16)|(j<<8)|j;
	else colors[i].pixel = j;
    }

    XStoreColors(XDISPLAY(dde_x11_DGL_current_dcm_data), cmap, colors, n);
}
/*
 ======================================================================
 */
void
ddr_x11_DGL_dcm_set_current_view (view)
     DtObject view;
{
    dde_x11_DGL_current_dcm_data->previous_view = 
	    dde_x11_DGL_current_dcm_data->current_view;
    dde_x11_DGL_current_dcm_data->current_view = view;


    if (XDoubleBuffered(dde_x11_DGL_current_dcm_data) &&
	((dde_x11_DGL_current_dcm_data->current_view != DcNullObject)&&
	 (dde_x11_DGL_current_dcm_data->current_view !=
	  dde_x11_DGL_current_dcm_data->previous_view))) {
#ifdef DEBUG
	printf ("blting front to back before update\n");
#endif
	ddr_x11_DGL_dcm_blt_front_to_back();
    }
}
/*
 ======================================================================
 */
void
ddr_x11_DGL_dcm_set_depth_buffer_enable (flag)
     DtFlag flag;
{
    XdSetZEnable(XpGC(dde_x11_DGL_current_dcm_data), (XdSwitch)flag);
}
/*
 ======================================================================
 */
void
ddr_x11_DGL_dcm_set_depth_buffer_write (flag)
     DtFlag flag;
{
    if (flag) {
	XdSetZFunction(XpGC(dde_x11_DGL_current_dcm_data),
		       ZFUNC_WRITE_RGBZ);
    } else {
	XdSetZFunction(XpGC(dde_x11_DGL_current_dcm_data),
		       ZFUNC_WRITE_RGB);
    }
}
/*
 ======================================================================
 */
ddr_x11_DGL_dcm_set_left_eye(flag)
     DtFlag flag;
{
    if (flag) {
	dde_x11_DGL_current_dcm_data->eye = XBLeftEye;
    } else {
	dde_x11_DGL_current_dcm_data->eye = XBRightEye;
    }

    XdSetDrawBuffer(XpDRAW(dde_x11_DGL_current_dcm_data),
		    XDrawBuffer(dde_x11_DGL_current_dcm_data),
		    dde_x11_DGL_current_dcm_data->eye);
}
/*
 ======================================================================
 */
void
ddr_x11_DGL_dcm_set_line_pattern (linepattern)
     DtUShort linepattern;
{
}
/*
 ======================================================================
 */

void
ddr_x11_DGL_dcm_set_pixel_function (pixelop)
     DtUShort pixelop;
{
    XdSetFunction (XpGC(dde_x11_DGL_current_dcm_data),
		   (int)pixelop);
}
/*
 ======================================================================
 */
void
ddr_x11_DGL_dcm_set_shade_index (index)
     DtInt index;
{
    dde_x11_DGL_current_dcm_data->shade_index = index;
}
/*
 ======================================================================
 */
void
ddr_x11_DGL_dcm_set_z_function (z_function)
     DtShort z_function;
{
    XdSetZCompare (XpGC(dde_x11_DGL_current_dcm_data), (XdInt)z_function);
}
