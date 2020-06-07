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
	void ddr_vll_dcm_clear_rectangle_depth (xmin, ymin, xmax, ymax)
	void ddr_vll_dcm_clear_rectangle_depth_and_color (xmin, ymin, xmax, ymax)
	void ddr_vll_dcm_write_scanline_byte (xstart, ystart, length, data)
	void ddr_vll_dcm_blend_texture (xmin, ymin, xmax, ymax, copy_to_main)
	int ddr_vll_dcm_blend_transp (xmin, ymin, xmax, ymax)
	void ddr_vll_dcm_clear_rectangle_transp_buffers (xmin, ymin, xmax, ymax)

  ======================================================================
 */
#include "dcm.h"

/*
 ======================================================================
 */
void
ddr_vll_dcm_clear_rectangle_depth (xmin, ymin, xmax, ymax)
	DtShort xmin;
	DtShort ymin;
	DtShort xmax;
	DtShort ymax;
{
    VLLclear_3d_viewport(VLL_CLEAR_Z, (float *)0, 0.0, 0.0);
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_clear_rectangle_depth_and_color (xmin, ymin, xmax, ymax)
	DtShort xmin;
	DtShort xmax;
	DtShort ymin;
	DtShort ymax;
{
    static float black[3] = {0.,0.,0.};

    if (dde_vll_current_dcm_data->clr_device) {
	VLLclear_window(VLL_CLEAR_RGB, black, 0.0, 0.0, 0);
	dde_vll_current_dcm_data->clr_device = DcFalse;
    }

    VLLclear_3d_viewport(VLL_CLEAR_RGB|VLL_CLEAR_Z,
			 dde_vll_current_dcm_data->background_color,
			 0.0, 0.0);
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_write_scanline_byte (xstart, ystart, length, data)
	DtInt xstart;
	DtInt ystart;
	DtInt length;
	DtUChar *data;
{
    DtUChar scanline[1280*4], *p;
    DtInt windowheight;
    DtInt n;
    int i;
    DtInt index;
    int multibyte = 0;
    int mode;

    p = scanline;

    n = length;

    if (dde_vll_current_dcm_data->visualtype == DcDirectColor ||
	dde_vll_current_dcm_data->visualtype == DcTrueColor) {
	mode = VLL_RASTER_ABGR;
	while (n--) {
	    p++;
	    *p++ = data[2];
	    *p++ = data[1];
	    *p++ = data[0];
	    data += 3;
	}
    } else {
	ddr_vll_dcm_setup_shading();
	mode = VLL_RASTER_PSEUDO_8;

	if (dde_vll_current_dcm_data->ncolors > 256) {
	    multibyte = 1;
	    mode = VLL_RASTER_PSEUDO_16UNP;
	}

	if (dde_vll_current_dcm_data->shade_mode == DcComponent) {
	    for (i=0; i<length; i++) {
		index = (((int)(dde_vll_current_dcm_data->
				cbtselpsweight[0]*data[i*3+0]/255.) &
			  dde_vll_current_dcm_data->cbtselpsmask[0]) + 
			 ((int)(dde_vll_current_dcm_data->
				cbtselpsweight[1]*data[i*3+1]/255.) &
			  dde_vll_current_dcm_data->cbtselpsmask[1]) +
			 ((int)(dde_vll_current_dcm_data->
				cbtselpsweight[2]*data[i*3+2]/255.) &
			  dde_vll_current_dcm_data->cbtselpsmask[2]));

		index = index < 0 ? 0 : 
		index > (dde_vll_current_dcm_data->ncolors - 1) ?
			(dde_vll_current_dcm_data->ncolors - 1) : index;

		if (multibyte) {
		    p += 2;
		    *p++ = (DtUChar) (index>>8);
		    *p++ = (DtUChar) index;
		} else {
		    *p++ = (DtUChar)index;
		}
	    }
	}
	else if (dde_vll_current_dcm_data->shade_mode == DcRange) {
	    for (i=0; i<length; i++) {
		index = dde_vll_current_dcm_data->cwtavgpslim[0] +
			(dde_vll_current_dcm_data->cwtavgpsweight[0]*data[i*3+0] +
			 dde_vll_current_dcm_data->cwtavgpsweight[1]*data[i*3+1] +
			 dde_vll_current_dcm_data->cwtavgpsweight[2]*data[i*3+2])*
				 dde_vll_current_dcm_data->cwtavgpslim[2]/255.;

		if (index > dde_vll_current_dcm_data->cwtavgpslim[1]) 
			index = dde_vll_current_dcm_data->cwtavgpslim[1];

		index = index < 0 ? 0 : 
		index > (dde_vll_current_dcm_data->ncolors - 1) ?
			(dde_vll_current_dcm_data->ncolors - 1) : index;

		if (multibyte) {
		    p += 2;
		    *p++ = (DtUChar) (index>>8);
		    *p++ = (DtUChar) index;
		} else {
		    *p++ = (DtUChar)index;
		}
	    }
	}
    }

    windowheight = dde_vll_current_dcm_data->height + 1;

    ystart = windowheight - ystart;

    VLLput_image(xstart, ystart, length, 1, length, scanline, mode);

}
/*
 ======================================================================
 */
void
ddr_vll_dcm_blend_texture (xmin, ymin, xmax, ymax, copy_to_main)
	DtShort xmin;
	DtShort xmax;
	DtShort ymin;
	DtShort ymax;
	DtFlag copy_to_main;
{
    VLLblend_texture();
}
/*
 ======================================================================
 */
int
ddr_vll_dcm_blend_transp (xmin, ymin, xmax, ymax)
	DtShort xmin;
	DtShort xmax;
	DtShort ymin;
	DtShort ymax;
{
    return VLLblend_transparency();
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_clear_rectangle_transp_buffers (xmin, ymin, xmax, ymax)
	DtShort xmin;
	DtShort xmax;
	DtShort ymin;
	DtShort ymax;
{
    VLLinit_transparency();
}
/*
 ======================================================================
 */
