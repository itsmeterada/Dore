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
	void ddr_g4vll_dcm_set_background_color (colormodel,c)
	void ddr_g4vll_dcm_set_foreground_color (colormodel,c)
	void ddr_g4vll_dcm_set_color_entries (colormodel,start, n, c)
	void ddr_g4vll_dcm_set_stereo_eye (eye)
	void ddr_g4vll_dcm_set_current_view (view)
	void ddr_g4vll_dcm_set_depth_buffer_enable (flag)
	void ddr_g4vll_dcm_set_depth_buffer_write (flag)
	void ddr_g4vll_dcm_set_shade_index (index)
	void ddr_g4vll_dcm_set_viewport (xmin, ymin, xmax, ymax)
	void ddr_g4vll_dcm_set_state (state)

  ======================================================================
 */
#include "dcm.h"

/*
 ======================================================================
 */
void ddr_g4vll_dcm_set_background_color (
    DtColorModel colormodel,
    DtReal *color)
{
    DtInt i;

    for (i=0; i<3; i++) 
	dde_g4vll_current_dcm_data->background_color[i] = color[i];
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_set_foreground_color (
    DtColorModel colormodel,
    DtReal *color)
{
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_set_color_entries (
    DtColorModel colormodel,
    int start,
    int count,
    DtReal *entries)
{
    static XColor xcolors[4096];
    static VLLcolor vllcolors[4096];
    static int colors[4096];
    int i, j;
    int r,g,b;
    XWindowAttributes attrs;
    ddt_g4vll_data *devicedata;

    devicedata = dde_g4vll_current_dcm_data;

    if (start + count > devicedata->ncolors) {
	DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_g4vll_dcm_set_color_entries",
		    "(start + count) is outside colormap size");
	count = devicedata->ncolors - start;
    } 
    if (count < 0) {
	DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_g4vll_dcm_set_color_entries",
		    "count less than zero");
	return;
    }
    if (devicedata->use_X && (devicedata->ncolors == 256)) {

	/*add an XSync for bug in Xlib. See TRs Dore-0657 and 0707 */
	XSync(devicedata->display, False);
	XGetWindowAttributes(devicedata->display, devicedata->window, &attrs);
      
	for (i=0; i<count; i++) {
	    if (65535 < (r = 65535 * entries[i*3])) r = 65535; 
	    if (0 > r) r = 0;
	    xcolors[i].red = r;

	    if (65535 < (g = 65535 * entries[i*3+1])) g = 65535; 
	    if (0 > g) g = 0;
	    xcolors[i].green = g;

	    if (65535 < (b = 65535 * entries[i*3+2])) b = 65535; 
	    if (0 > b) b = 0;
	    xcolors[i].blue = b;

	    xcolors[i].flags = DoRed|DoGreen|DoBlue;
	    j = start+i;

	    if (devicedata->visualtype == DcDirectColor)
		    xcolors[i].pixel = (j<<16)|(j<<8)|j;
	    else xcolors[i].pixel = j;
	}
	XStoreColors(devicedata->display, attrs.colormap, xcolors, count);
	XFlush(devicedata->display);

    } else {
	if (devicedata->ncolors == 256) {

	    for (i=0; i<count; i++) {
                if (65535 < (r = 65535*entries[i*3])) r = 65535; if (0> r) r = 0;
            	vllcolors[i].red = r;

            	if (65535 < (g = 65535*entries[i*3+1])) g = 65535; if (0> g) g = 0;
            	vllcolors[i].green = g;

            	if (65535 < (b = 65535*entries[i*3+2])) b = 65535; if (0> b) b = 0;
            	vllcolors[i].blue = b;

            	vllcolors[i].flags = VLL_DO_RED|VLL_DO_GREEN|VLL_DO_BLUE;
            	j = start+i;
            	vllcolors[i].pixel = j;
        	}
	    } else /* 256 < ncolors < 4096 */ {

		for (i=0; i<count; i++) {

	    	    if (65535 < (r = 65535 * entries[i*3])) r = 65535; 
		    if (0> r) r = 0;

	    	    if (65535 < (g = 65535 * entries[i*3+1])) g = 65535; 
		    if (0> g) g = 0;

	    	    if (65535 < (b = 65535 * entries[i*3+2])) b = 65535; 
		    if (0> b) b = 0;

		    r >>= 8;
		    g >>= 8;
		    b >>= 8;

	    	    colors[i] = (b << 16) | (g << 8) | r;
		}
	    }

	    if (devicedata->visualtype == DcPseudoColor) {
	        if (devicedata->ncolors == 256) {

	    	    VLLset_colormap(devicedata->vll_cmap, vllcolors, start, count);

	        } else {
		    VLLstore_translation_table(devicedata->transl_table, 
				colors, start, count);
	        }
	    } else {
	        VLLset_colormap(devicedata->vll_cmap, vllcolors, start, count);
	    }
    }
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_set_stereo_eye (
    int eye)
{
    switch (eye) {
	case VLL_STEREO_RIGHT: 	VLLset_stereo_eye (VLL_STEREO_RIGHT);
				break;
	case VLL_STEREO_LEFT:	VLLset_stereo_eye (VLL_STEREO_LEFT);
				break;
	default:		/* render the mono image twice */
			        break;
    }
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_set_current_view (
    DtObject view)
{
    dde_g4vll_current_dcm_data->previous_view =
	    dde_g4vll_current_dcm_data->current_view;
    dde_g4vll_current_dcm_data->current_view = view;

    if (dde_g4vll_current_dcm_data->double_buffered &&
	((dde_g4vll_current_dcm_data->current_view != DcNullObject) &&
	 (dde_g4vll_current_dcm_data->current_view !=
	  dde_g4vll_current_dcm_data->previous_view))) {
#ifdef DEBUG
	printf ("need to blt front to back before update\n");
#endif

	dde_g4vll_current_dcm_data->clr_device = DcTrue;
    }
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_set_depth_buffer_enable (
    DtFlag flag)
{
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_set_depth_buffer_write (
    DtFlag flag)
{
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_set_shade_index (
    DtInt index)
{
    dde_g4vll_current_dcm_data->shade_index = index;
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_set_viewport (
    DtReal xmin,
    DtReal ymin,
    DtReal xmax,
    DtReal ymax)
{
    ddt_g4vll_data *data;
    /*
     * The assumption is made that everything outside of the device
     * directory uses the Dore' notion of x+ to left and y+ up
     */
    float x, y;
    float width, height;

    data = dde_g4vll_current_dcm_data;

    x = xmin;
    y = dde_g4vll_current_dcm_data->height - ymax;
    width = xmax - xmin;
    height = ymax - ymin;

    VLLset_window(data->vll_window);
    if (data->double_buffered) {
    	VLLset_output_buffer(VLL_NONDISPLAY_BUFFER);
    } else {
    	VLLset_output_buffer(VLL_DISPLAY_BUFFER);
    }
    VLLset_draw_area(x, y, width, height);
    VLLset_draw_offset(x, y);
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_set_state (
    VLLstate *state)
{
    VLLset_state (state);
}
/*
 ======================================================================
 */
