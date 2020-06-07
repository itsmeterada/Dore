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
	void ddr_g4vll_dcm_clear_rectangle_depth (xmin, ymin, xmax, ymax)
	void ddr_g4vll_dcm_clear_rectangle_depth_and_color (xmin, ymin, xmax, ymax)
	void ddr_g4vll_dcm_write_scanline_byte (xstart, ystart, length, data)
	void ddr_g4vll_dcm_blend_texture (xmin, ymin, xmax, ymax, copy_to_main)
	int ddr_g4vll_dcm_blend_transp (xmin, ymin, xmax, ymax)
	void ddr_g4vll_dcm_clear_rectangle_transp_buffers (xmin, ymin, xmax, ymax)
	void ddr_g4vll_dcm_setup_transp (have_opaque_objects, 
		have_transparent_textured_objects)
	void ddr_g4vll_dcm_finish_transp ()
	int ddr_g4vll_dcm_blend_filter (coefficient)
	void ddr_g4vll_dcm_clear_filter_buffer ()
	void ddr_g4vll_dcm_allocate_offscreen_resource(resource);

  ======================================================================
 */
#include "dcm.h"

/*
 ======================================================================
 */
void ddr_g4vll_dcm_clear_rectangle_depth (
    DtShort xmin,
    DtShort ymin,
    DtShort xmax,
    DtShort ymax)
{
    VLLclear_draw_area(VLL_CLEAR_Z, (float *)0, 0.0, 0.0, 0, 0);
    VLLclear_output_buffer(VLL_CLEAR_Z | VLL_CLEAR_STENCIL | VLL_CLEAR_ALPHA, 
		(float *)0, 0.0, 0.0, 0, 0);
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_clear_rectangle_depth_and_color (
    DtShort xmin,
    DtShort ymin,
    DtShort xmax,
    DtShort ymax)
{
    static float black[3] = {0.,0.,0.};
    int x, y, width, height;
    int xoff, yoff;

    /*
     * DISPLAY's contents are copied into NONDISPLAY after NONDISPLAY has
     * been cleared.  This is done to retain the images of any views drawn
     * in previous updates that are not necessarily being updated this time.
     * However, VLLcopy_image is draw area-based, so we need to set the
     * draw area to the entire window to do the copy and then restore it.
     */

    if (dde_g4vll_current_dcm_data->clr_device) {

	if (dde_g4vll_current_dcm_data->copy_back_buffer) {
	    VLLclear_output_buffer(VLL_CLEAR_RGB | VLL_CLEAR_STENCIL |
		VLL_CLEAR_ALPHA, dde_g4vll_current_dcm_data->background_color, 
		0.0, 0.0, 0, 0);
	}

	if (!dde_g4vll_current_dcm_data->double_buffered) {
	    dde_g4vll_current_dcm_data->copy_back_buffer = DcFalse;
	} else if ((dde_g4vll_current_dcm_data->double_buffered) &&
	   (dde_g4vll_current_dcm_data->copy_back_buffer)) {

	    VLLquery_draw_area(&x, &y, &width, &height);
	    VLLquery_draw_offset(&xoff, &yoff);

	    VLLset_draw_area(0, 0, dde_g4vll_current_dcm_data->width,
			dde_g4vll_current_dcm_data->height);
	    VLLset_draw_offset(0, 0);

	    VLLcopy_image(0, 0, dde_g4vll_current_dcm_data->width,
		dde_g4vll_current_dcm_data->height, 0, 0,
		VLL_DISPLAY_BUFFER, VLL_NONDISPLAY_BUFFER, VLL_COPY_RGB);

	    VLLset_draw_area(x, y, width, height);
	    VLLset_draw_offset(xoff, yoff);

	    dde_g4vll_current_dcm_data->copy_back_buffer = DcFalse;
	}

	dde_g4vll_current_dcm_data->clr_device = DcFalse;
    }

    VLLclear_draw_area(VLL_CLEAR_RGB | VLL_CLEAR_Z,
		dde_g4vll_current_dcm_data->background_color,
		0.0, 0.0, 0, 0);
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_write_scanline_byte (
    DtInt xstart,
    DtInt ystart,
    DtInt length,
    DtUChar *data)
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

    if (dde_g4vll_current_dcm_data->visualtype == DcDirectColor ||
	dde_g4vll_current_dcm_data->visualtype == DcTrueColor) {
	mode = VLL_RASTER_ABGR;
    
        if (dor_util_bigendian_word_byte_order()) {
            /* we have rgb, we want abgr */
  	    while (n--) {
	        p++;
	        *p++ = data[2];
	        *p++ = data[1];
	        *p++ = data[0];
	        data += 3;
	    }
	} else {
            /* we have rgb, we want abgr and this is a little-endian machine */
	    while (n--) {
	        *p++ = data[0];
	        *p++ = data[1];
	        *p++ = data[2];
	        p++;
	        data += 3;
	    }
	}

    } else {
	ddr_g4vll_dcm_setup_shading();
	mode = VLL_RASTER_PSEUDO_8;

	if (dde_g4vll_current_dcm_data->ncolors > 256) {
	    multibyte = 1;
	    mode = VLL_RASTER_PSEUDO_16UNP;
	}

	if (dde_g4vll_current_dcm_data->shade_mode == DcComponent) {
	    for (i=0; i<length; i++) {
		index = (((DtInt)(dde_g4vll_current_dcm_data->
				cbtselpsweight[0]*data[i*3+0]/255.) &
			  dde_g4vll_current_dcm_data->cbtselpsmask[0]) + 
			 ((DtInt)(dde_g4vll_current_dcm_data->
				cbtselpsweight[1]*data[i*3+1]/255.) &
			  dde_g4vll_current_dcm_data->cbtselpsmask[1]) +
			 ((DtInt)(dde_g4vll_current_dcm_data->
				cbtselpsweight[2]*data[i*3+2]/255.) &
			  dde_g4vll_current_dcm_data->cbtselpsmask[2]));

		index = index < 0 ? 0 : 
		index > (dde_g4vll_current_dcm_data->ncolors - 1) ?
			(dde_g4vll_current_dcm_data->ncolors - 1) : index;

		if (multibyte) {
		    /* unpacked 16bit data */
		    *p++ = 0;
		    *p++ = 0;
		    *p++ = (DtUChar) (0xFF & (index>>8));
		    *p++ = (DtUChar) (0xFF & index);
		} else {
		    *p++ = (DtUChar)index;
		}
	    }
	}
	else if (dde_g4vll_current_dcm_data->shade_mode == DcRange) {
	    for (i=0; i<length; i++) {
		index = dde_g4vll_current_dcm_data->cwtavgpslim[0] +
			(dde_g4vll_current_dcm_data->cwtavgpsweight[0]*data[i*3+0] +
			 dde_g4vll_current_dcm_data->cwtavgpsweight[1]*data[i*3+1] +
			 dde_g4vll_current_dcm_data->cwtavgpsweight[2]*data[i*3+2])*
				 dde_g4vll_current_dcm_data->cwtavgpslim[2]/255.;

		if (index > dde_g4vll_current_dcm_data->cwtavgpslim[1]) 
			index = dde_g4vll_current_dcm_data->cwtavgpslim[1];

		index = index < 0 ? 0 : 
		index > (dde_g4vll_current_dcm_data->ncolors - 1) ?
			(dde_g4vll_current_dcm_data->ncolors - 1) : index;

		if (multibyte) {
		    /* unpacked 16bit data */
		    *p++ = 0;
		    *p++ = 0;
		    *p++ = (DtUChar) (0xFF & (index>>8));
		    *p++ = (DtUChar) (0xFF & index);
		} else {
		    *p++ = (DtUChar)index;
		}
	    }
	}
    }

    windowheight = dde_g4vll_current_dcm_data->height + 1;

    ystart = windowheight - ystart;

    VLLput_image(xstart, ystart, length, 1, length, scanline, mode);

}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_blend_texture (
    DtShort xmin,
    DtShort ymin,
    DtShort xmax,
    DtShort ymax,
    DtFlag copy_to_main)
{
    VLLblend_texture();
}
/*
 ======================================================================
 */
int ddr_g4vll_dcm_blend_transp (
    DtShort xmin,
    DtShort ymin,
    DtShort xmax,
    DtShort ymax)
{
    return VLLblend_transparency();
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_clear_rectangle_transp_buffers (
    DtShort xmin,
    DtShort ymin,
    DtShort xmax,
    DtShort ymax)
{
    VLLinit_transparency();
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_setup_transp (
    DtFlag have_opaque_objects,
    DtFlag have_transparent_textured_objects)
{
    int x, y, width, height;

    /*
     * Resource requirements for transparency:
     *		multipass:	SP pixmap with DP Z and SP accumulator without Z
     *		singlepass:	SP pixmap with SP Z (currently get DP Z)
     *		twopass:	none
     *
     * In single- and multi-pass transparency, the pixmap becomes the 
     * "output buffer" for the transparency pass(es).
     *
     * In multipass transparency, we optimise by letting VLL know if any
     * opaque geometry needs to be copied from the nondisplay buffer
     * for the transparency passes.
     */

    if (dde_g4vll_current_dcm_data->transp_mode == ddd_g4vll_transp_multi_pass) {
	VLLset_transparency_mode(VLL_MULTI_PASS);

	if (have_transparent_textured_objects) {
	    VLLset_texture_alpha_enable(DcTrue);
	}

	if (have_opaque_objects) {
	    if (dde_g4vll_current_dcm_data->double_buffered) {
	    	VLLset_multipass_transparency_opaque_geometry(VLL_NONDISPLAY_BUFFER);
	    } else {
		VLLset_multipass_transparency_opaque_geometry(VLL_DISPLAY_BUFFER);
	    }
	} else {
	    VLLset_multipass_transparency_opaque_geometry(VLL_NULL_BUFFER);
	}
    	ddr_g4vll_dcm_allocate_offscreen_resource(ddd_g4vll_pixmap_z);
    	ddr_g4vll_dcm_set_offscreen_resource(ddd_g4vll_pixmap_z);

    	VLLset_output_buffer(VLL_PIXMAP_BUFFER);

	ddr_g4vll_dcm_allocate_offscreen_resource(ddd_g4vll_accumulator);
	ddr_g4vll_dcm_set_offscreen_resource(ddd_g4vll_accumulator);

    } else if (dde_g4vll_current_dcm_data->transp_mode ==
		ddd_g4vll_transp_single_pass) {

	VLLset_transparency_mode(VLL_SINGLE_PASS);

    	ddr_g4vll_dcm_allocate_offscreen_resource(ddd_g4vll_pixmap);
    	ddr_g4vll_dcm_set_offscreen_resource(ddd_g4vll_pixmap);

	/*
	 * Copy that part of the nondisplay (or display) buffer that enclosed
	 * the current viewport into the pixmap.
	 */

	VLLquery_draw_area(&x, &y, &width, &height);

    	VLLset_output_buffer(VLL_PIXMAP_BUFFER);

	VLLclear_output_buffer(VLL_CLEAR_ALPHA | VLL_CLEAR_RGB | VLL_CLEAR_Z, 
	    dde_g4vll_current_dcm_data->background_color, 1.0, 0.0, 0, 0);

	if (dde_g4vll_current_dcm_data->double_buffered) {
            VLLcopy_image(0, 0, width, height, 0, 0,
		VLL_NONDISPLAY_BUFFER, VLL_PIXMAP_BUFFER,
		VLL_COPY_RGB | VLL_COPY_3D);
	} else {
            VLLcopy_image(0, 0, width, height, 0, 0,
		VLL_DISPLAY_BUFFER, VLL_PIXMAP_BUFFER,
		VLL_COPY_RGB | VLL_COPY_3D);
	}
    } else if (dde_g4vll_current_dcm_data->transp_mode ==
    		ddd_g4vll_transp_two_pass_compos) {
	    VLLset_transparency_mode(VLL_TWO_PASS_COMPOS);
    } else if (dde_g4vll_current_dcm_data->transp_mode ==
	    	ddd_g4vll_transp_two_pass_add) {
	    VLLset_transparency_mode(VLL_TWO_PASS_ADD);
    }
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_finish_transp (void)
{
    int x, y, width, height;

    /*
     * Copy the contents of the pixmap to the back display buffer.
     */

    if (dde_g4vll_current_dcm_data->transp_mode == ddd_g4vll_transp_multi_pass) {

	VLLquery_draw_area(&x, &y, &width, &height);

	if (dde_g4vll_current_dcm_data->double_buffered) {
    	    VLLcopy_image(0, 0, width, height, 0, 0,
		VLL_ACCUMULATOR_BUFFER, VLL_NONDISPLAY_BUFFER, VLL_COPY_RGB);
	} else {
    	    VLLcopy_image(0, 0, width, height, 0, 0,
		VLL_ACCUMULATOR_BUFFER, VLL_DISPLAY_BUFFER, VLL_COPY_RGB);
	}

    } else if (dde_g4vll_current_dcm_data->transp_mode == 
		ddd_g4vll_transp_single_pass) {

	VLLquery_draw_area(&x, &y, &width, &height);

	if (dde_g4vll_current_dcm_data->double_buffered) {
    	    VLLcopy_image(0, 0, width, height, 0, 0,
		VLL_PIXMAP_BUFFER, VLL_NONDISPLAY_BUFFER, VLL_COPY_RGB);
	} else {
    	    VLLcopy_image(0, 0, width, height, 0, 0,
		VLL_PIXMAP_BUFFER, VLL_DISPLAY_BUFFER, VLL_COPY_RGB);
	}
    }

    if (dde_g4vll_current_dcm_data->double_buffered) {
    	VLLset_output_buffer(VLL_NONDISPLAY_BUFFER);
    } else {
    	VLLset_output_buffer(VLL_DISPLAY_BUFFER);
    }
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_clear_filter_buffer (void)
{
    /*
     * Supersampled images are generated using the non-display buffer
     * and a double precision accumulator buffer.
     */
    ddr_g4vll_dcm_allocate_offscreen_resource(ddd_g4vll_accumulator);
    ddr_g4vll_dcm_set_offscreen_resource(ddd_g4vll_accumulator);
    VLLinit_filter();
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_blend_filter (
    int coefficient)
{
    ddr_g4vll_dcm_set_offscreen_resource(ddd_g4vll_accumulator);
    VLLblend_filter((unsigned int)coefficient >> 1);
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_allocate_offscreen_resource (
    int requested_resource)
{
    ddt_g4vll_dcm_offscreen_resource *offscreen_resource;
    int device_size_changed;
    static DtFlag accumulator_has_z;
    static DtFlag pixmap_has_z;

    /*
     * Allocate the requested resource in the following conditions:
     *    1) it has never previously been allocated
     *    2) the device has changed size since this particular resource
     * 	     was created
     *    3) a pixmap/accumulator exists, but a pixmap/accumulator with Z 
     *	     is now needed
     */

    switch (requested_resource) {
    case ddd_g4vll_pixmap:
        offscreen_resource = &(dde_g4vll_current_dcm_data->vll_pixmap);
    	if ((dde_g4vll_current_dcm_data->width != offscreen_resource->width) ||
	 	(dde_g4vll_current_dcm_data->height != offscreen_resource->height)) {
	    device_size_changed = DcTrue;
    	} else {
	    device_size_changed = DcFalse;
    	}

	if ((offscreen_resource->pixmap == NULL) || (device_size_changed)) {
	    if (offscreen_resource->pixmap != NULL) {
		VLLdestroy_pixmap(offscreen_resource->pixmap);
	    }
	    offscreen_resource->pixmap = VLLcreate_pixmap(
			/* single precision image only */  0,
			dde_g4vll_current_dcm_data->width, 
			dde_g4vll_current_dcm_data->height);
	    offscreen_resource->width  = dde_g4vll_current_dcm_data->width;
	    offscreen_resource->height = dde_g4vll_current_dcm_data->height;
	    pixmap_has_z = DcFalse;
	}
	break;
    case ddd_g4vll_pixmap_z:
        offscreen_resource = &(dde_g4vll_current_dcm_data->vll_pixmap);
    	if ((dde_g4vll_current_dcm_data->width != offscreen_resource->width) ||
	 	(dde_g4vll_current_dcm_data->height != offscreen_resource->height)) {
	    device_size_changed = DcTrue;
    	} else {
	    device_size_changed = DcFalse;
    	}

	if ((offscreen_resource->pixmap == NULL) || (device_size_changed) ||
		(!pixmap_has_z)) {
	    if (offscreen_resource->pixmap != NULL) {
		VLLdestroy_pixmap(offscreen_resource->pixmap);
	    }
	    offscreen_resource->pixmap = VLLcreate_pixmap(
			(VLL_PIXMAP_ZBUF | VLL_PIXMAP_DP_Z),
			dde_g4vll_current_dcm_data->width, 
			dde_g4vll_current_dcm_data->height);
	    offscreen_resource->width  = dde_g4vll_current_dcm_data->width;
	    offscreen_resource->height = dde_g4vll_current_dcm_data->height;
	    pixmap_has_z = DcTrue;
	}
	break;
    case ddd_g4vll_accumulator:
        offscreen_resource = &(dde_g4vll_current_dcm_data->vll_accumulator);
    	if ((dde_g4vll_current_dcm_data->width != offscreen_resource->width) ||
	 	(dde_g4vll_current_dcm_data->height != offscreen_resource->height)) {
	    device_size_changed = DcTrue;
    	} else {
	    device_size_changed = DcFalse;
    	}

	if ((offscreen_resource->pixmap == NULL) || (device_size_changed)) {
	    if (offscreen_resource->pixmap != NULL) {
		VLLdestroy_pixmap(offscreen_resource->pixmap);
	    }
	    offscreen_resource->pixmap = VLLcreate_pixmap(
			VLL_PIXMAP_DP_IMAGE,
			dde_g4vll_current_dcm_data->width, 
			dde_g4vll_current_dcm_data->height);
	    offscreen_resource->width  = dde_g4vll_current_dcm_data->width;
	    offscreen_resource->height = dde_g4vll_current_dcm_data->height;
	    accumulator_has_z = DcFalse;
	}
	break;
    case ddd_g4vll_accumulator_z:
        offscreen_resource = &(dde_g4vll_current_dcm_data->vll_accumulator);
    	if ((dde_g4vll_current_dcm_data->width != offscreen_resource->width) ||
	 	(dde_g4vll_current_dcm_data->height != offscreen_resource->height)) {
	    device_size_changed = DcTrue;
    	} else {
	    device_size_changed = DcFalse;
    	}

	if ((offscreen_resource->pixmap == NULL) || (device_size_changed) || 
		(!accumulator_has_z)) {
	    if (offscreen_resource->pixmap != NULL) {
		VLLdestroy_pixmap(offscreen_resource->pixmap);
	    }
	    offscreen_resource->pixmap = VLLcreate_pixmap(
			    (VLL_PIXMAP_ZBUF|VLL_PIXMAP_DP_IMAGE|VLL_PIXMAP_DP_Z),
			    dde_g4vll_current_dcm_data->width, 
			    dde_g4vll_current_dcm_data->height);
	    offscreen_resource->width  = dde_g4vll_current_dcm_data->width;
	    offscreen_resource->height = dde_g4vll_current_dcm_data->height;
	    accumulator_has_z = DcTrue;
	}
	break;
	default:
#ifdef DEBUG
		printf ("ddr_g4vll_dcm_allocate_offscreen_resource: unknown resource\n");
#endif
	break;
    }
}
/*
 ======================================================================
 */
void ddr_g4vll_dcm_set_offscreen_resource (
    int requested_resource)
{
    switch (requested_resource) {
	case ddd_g4vll_pixmap:
	case ddd_g4vll_pixmap_z:
    	    VLLset_pixmap(dde_g4vll_current_dcm_data->vll_pixmap.pixmap);
	    break;
	case ddd_g4vll_accumulator:
	case ddd_g4vll_accumulator_z:
	    VLLset_accumulator(dde_g4vll_current_dcm_data->vll_accumulator.pixmap);
	    break;
    }
}
/*
 ======================================================================
 */
