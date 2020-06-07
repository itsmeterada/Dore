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
	void ddr_vll_dcm_set_background_color (colormodel,c)
	void ddr_vll_dcm_set_foreground_color (colormodel,c)
	void ddr_vll_dcm_set_color_entries (colormodel,start, n, c)
	void ddr_vll_dcm_set_stereo_eye (eye)
	void ddr_vll_dcm_set_current_view (view)
	void ddr_vll_dcm_set_depth_buffer_enable (flag)
	void ddr_vll_dcm_set_depth_buffer_write (flag)
	void ddr_vll_dcm_set_shade_index (index)
	void ddr_vll_dcm_set_viewport (xmin, ymin, xmax, ymax)

  ======================================================================
 */
#include "dcm.h"

/*
 ======================================================================
 */
void
ddr_vll_dcm_set_background_color (colormodel,c)
	DtColorModel colormodel;
	DtReal *c;
{
    DtInt i;

    for (i=0; i<3; i++) 
	    dde_vll_current_dcm_data->background_color[i] = c[i];
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_set_foreground_color (colormodel,c)
	DtColorModel colormodel;
	DtReal *c;
{
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_set_color_entries (colormodel,start, n, c)
	DtColorModel colormodel;
	int start;
	int n;
	DtReal *c;
{
    static XColor colors[4096];
    static VLLcolor vllcolors[4096];
    int i, j;
    DtReal r;
    XWindowAttributes attrs;
    ddt_vll_data *devicedata;

    devicedata = dde_vll_current_dcm_data;

    if (start + n > devicedata->ncolors) {
	DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_vll_dcm_set_color_entries",
		    "(start + count) is outside colormap size");
	n = devicedata->ncolors - start;
    } 
    if (n < 0) {
	DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_vll_dcm_set_color_entries",
		    "count less than zero");
	return;
    }
    if (devicedata->use_X && (devicedata->ncolors == 256)) {
	XGetWindowAttributes(devicedata->display, devicedata->window, &attrs);
      
	for (i=0; i<n; i++) {
	    if (65535 < (r = 65535*c[i*3])) r = 65535; if (0> r) r = 0;
	    colors[i].red = r;
	    if (65535 < (r = 65535*c[i*3+1])) r = 65535; if (0> r) r = 0;
	    colors[i].green = r;
	    if (65535 < (r = 65535*c[i*3+2])) r = 65535; if (0> r) r = 0;
	    colors[i].blue = r;
	    colors[i].flags = DoRed|DoGreen|DoBlue;
	    j = start+i;
	    if (devicedata->visualtype == DcDirectColor)
		    colors[i].pixel = (j<<16)|(j<<8)|j;
	    else colors[i].pixel = j;
	}
	XStoreColors(devicedata->display, attrs.colormap, colors, n);
	XFlush(devicedata->display);
    } else if (!devicedata->offscreen) {
	for (i=0; i<n; i++) {
	    if (65535 < (r = 65535*c[i*3])) r = 65535; if (0> r) r = 0;
	    vllcolors[i].red = r;
	    if (65535 < (r = 65535*c[i*3+1])) r = 65535; if (0> r) r = 0;
	    vllcolors[i].green = r;
	    if (65535 < (r = 65535*c[i*3+2])) r = 65535; if (0> r) r = 0;
	    vllcolors[i].blue = r;
	    vllcolors[i].flags = VLL_DO_RED|VLL_DO_GREEN|VLL_DO_BLUE;
	    j = start+i;
	    vllcolors[i].pixel = j;
	}
	if (devicedata->visualtype == DcPseudoColor) {
	    if (devicedata->ncolors == 256) {
		VLLload_colormap(VLLquery_colormap_id(VLL_PSEUDO_COLOR),
				 vllcolors, start, n);
	    } else {
		VLLload_colormap(VLL_LUT_PSEUDO_16, vllcolors, start, n);
	    }
	} else {
	    VLLload_colormap(VLLquery_colormap_id(VLL_TRUE_COLOR),
			     vllcolors, start, n);
	}
    }
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_set_stereo_eye(eye)
	int eye;
{
    if (eye) {
	VLLset_stereo_eye (VLL_STEREO_RIGHT);
    } else {
	VLLset_stereo_eye (VLL_STEREO_LEFT);
    }
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_set_current_view (view)
	DtObject view;
{
    float x, y, width, height;

    dde_vll_current_dcm_data->previous_view =
	    dde_vll_current_dcm_data->current_view;
    dde_vll_current_dcm_data->current_view = view;

    if (dde_vll_current_dcm_data->double_buffered &&
	((dde_vll_current_dcm_data->current_view != DcNullObject) &&
	 (dde_vll_current_dcm_data->current_view !=
	  dde_vll_current_dcm_data->previous_view))) {
#ifdef DEBUG
	printf ("blting front to back before update\n");
#endif
	VLLset_3d_viewport(0, 0, 
			   dde_vll_current_dcm_data->width,
			   dde_vll_current_dcm_data->height);
	x = 0;
	y = 0;
	width = dde_vll_current_dcm_data->width;
	height = dde_vll_current_dcm_data->height;
      
	VLLcopy_buffer(x, y, width, height, x, y, VLL_DISPLAY_BUFFER,
		       VLL_NONDISPLAY_BUFFER);
    }
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_set_depth_buffer_enable (flag)
	DtFlag flag;
{
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_set_depth_buffer_write (flag)
	DtFlag flag;
{
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_set_shade_index (index)
	DtInt index;
{
    dde_vll_current_dcm_data->shade_index = index;
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_set_viewport (xmin, ymin, xmax, ymax)
	DtReal xmin;
	DtReal ymin;
	DtReal xmax;
	DtReal ymax;
{
    ddt_vll_data *data;
    /*
     * The assumption is made that everything outside of the device
     * directory uses the Dore' notion of x+ to left and y+ up
     */
    float x, y;
    float width, height;

    data = dde_vll_current_dcm_data;

    x = xmin;
    y = dde_vll_current_dcm_data->height - ymax;
    width = xmax - xmin;
    height = ymax - ymin;

    VLLset_dst_window(data->x, data->y, data->width, data->height, 
		      (int)data->clipid);

    if (data->offscreen)
        data->output_buffer = VLL_PIXMAP_BUFFER;

    VLLset_output_buffer(data->output_buffer);

    if (data->use_X) {
	VLLset_dst_clip_list(data->nrects, (VLLrect *)data->clip_rect);
    }
    VLLset_3d_viewport(x, y, width, height);
}
/*
 ======================================================================
 */
