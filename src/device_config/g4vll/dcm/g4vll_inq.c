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
	ddr_g4vll_dcm_inquire_color_entries
	ddr_g4vll_dcm_inquire_auto_size
	ddr_g4vll_dcm_inquire_clip_list
	ddr_g4vll_dcm_inq_pseudo_range
	ddr_g4vll_dcm_inq_pseudo_space
	ddr_g4vll_dcm_inquire_device_extent
	ddr_g4vll_dcm_check_device_extent
	ddr_g4vll_dcm_inquire_ncolors
	ddr_g4vll_dcm_inquire_pixel_data
	ddr_g4vll_dcm_inquire_resolution
	ddr_g4vll_dcm_inquire_spheres
	ddr_g4vll_dcm_inquire_cylinders
	ddr_g4vll_dcm_inquire_stereo
	ddr_g4vll_dcm_inquire_stereo_screen
	ddr_g4vll_dcm_inquire_head_number
	ddr_g4vll_dcm_inquire_if_states_match
	ddr_g4vll_dcm_inquire_state
	ddr_g4vll_dcm_inquire_visual_type
	ddr_g4vll_dcm_inq_transp_mode
	static Bool XPeerUpdateState

  ======================================================================
 */
#include "dcm.h"


/* ====================================================================== */

void ddr_g4vll_dcm_inquire_color_entries (
    DtColorModel colormodel,
    int start,
    int n,
    DtReal *c)
{
    static XColor colors[4096];
    int i, j;
    DtReal r;
    XWindowAttributes attrs;
    ddt_g4vll_data *devicedata;

    devicedata = dde_g4vll_current_dcm_data;

    if (start + n > devicedata->ncolors) {
	DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_g4vll_dcm_inquire_color_entries",
		    "(start + count) is outside colormap size");
	n = devicedata->ncolors - start;
    } 
    if (n < 0) {
	DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_g4vll_dcm_inquire_color_entries",
		    "count less than zero");
	return;
    }

    if (devicedata->use_X && (devicedata->ncolors == 256)) {
	/*add an XSync for bug in Xlib. See TRs Dore-0657 and 0707 */
	XSync(devicedata->display, False);
	XGetWindowAttributes(devicedata->display, devicedata->window, &attrs);

	for (i=0; i<n; i++) {
	    j = start+i;
	 
	    if (devicedata->visualtype == DcDirectColor)
		colors[i].pixel = (j<<16)|(j<<8)|j;
	    else 
		colors[i].pixel = j;
	}

	XQueryColors(devicedata->display, attrs.colormap, colors, n);

		
	for (i=0; i<n; i++) {
	    if ((r = (DtReal)colors[i].red / 65535.) < 0.0) r = 0.0;
	    if (r > 1.0) r = 1.0;
	    c[i*3] = r;

	    if ((r = (DtReal)colors[i].green / 65535.) < 0.0) r = 0.0;
	    if (r > 1.0) r = 1.0;
	    c[i*3+1] = r;

	    if ((r = (DtReal)colors[i].blue / 65535.) < 0.0) r = 0.0;
	    if (r > 1.0) r = 1.0;
	    c[i*3+2] = r;
	}
    }
       
}

/* ====================================================================== */

void ddr_g4vll_dcm_inquire_auto_size (
    DtFlag *flag)
{
    *flag = dde_g4vll_current_dcm_data->auto_size;
}

/* ====================================================================== */

void ddr_g4vll_dcm_inquire_clip_list (
    DtUInt *nrects,
    DtReal **clip_list,
    DtReal extent[4],
    DtFlag *partiallyobscuredflag)
{
   static DtReal clist[4];

   clist[0] = extent[0] = 0;
   clist[1] = extent[1] = 0;
   clist[2] = extent[2] = dde_g4vll_current_dcm_data->width;
   clist[3] = extent[3] = dde_g4vll_current_dcm_data->height;

   *nrects = 1;
   *clip_list = clist;
   *partiallyobscuredflag = DcFalse;
}

/* ====================================================================== */

void ddr_g4vll_dcm_inq_pseudo_range (
    float weight[3])
{
    weight[0] = dde_g4vll_current_dcm_data->cwtavgpsweight[0];
    weight[1] = dde_g4vll_current_dcm_data->cwtavgpsweight[1];
    weight[2] = dde_g4vll_current_dcm_data->cwtavgpsweight[2];
}

/* ====================================================================== */

void ddr_g4vll_dcm_inq_pseudo_space (
    float weight[3],
    int mask[3])
{
    weight[0] = dde_g4vll_current_dcm_data->cbtselpsweight[0];
    weight[1] = dde_g4vll_current_dcm_data->cbtselpsweight[1];
    weight[2] = dde_g4vll_current_dcm_data->cbtselpsweight[2];
   
    mask[0] = dde_g4vll_current_dcm_data->cbtselpsmask[0];
    mask[1] = dde_g4vll_current_dcm_data->cbtselpsmask[1];
    mask[2] = dde_g4vll_current_dcm_data->cbtselpsmask[2];
}

/* ====================================================================== */

void ddr_g4vll_dcm_inquire_device_extent (
    DtVolume *volume)
{
    volume->bll[0] = 0.0;
    volume->bll[1] = 0.0;
    volume->bll[2] = 0.0;

    volume->fur[0] = dde_g4vll_current_dcm_data->width;
    volume->fur[1] = dde_g4vll_current_dcm_data->height;
    volume->fur[2] = 1.0;
}

/* ====================================================================== */

DtFlag ddr_g4vll_dcm_check_device_extent (void)
{
    /* 
     * Return true if the window's geometry has changed since
     * the last call to ddr_g4vll_dcm_inquire_device_extent.
     */

    if (dde_g4vll_current_dcm_data->use_X) {
	if (XPeerUpdateState(dde_g4vll_current_dcm_data->window)) {
	    dde_g4vll_current_dcm_data->bad_geom = DcTrue;
	    return DcTrue;
	} else {
	    return DcFalse;
	}
    } else {
	dde_g4vll_current_dcm_data->bad_geom = DcFalse;
	return DcFalse;
    }
}

/* ====================================================================== */

void ddr_g4vll_dcm_inquire_ncolors (
    DtInt *ncolors)
{
    *ncolors = dde_g4vll_current_dcm_data->ncolors;
}

/* ====================================================================== */

DtFlag ddr_g4vll_dcm_inquire_pixel_data (
    DtRasterType requesttype,
    DtInt *width,
    DtInt *height,
    DtRasterType *type,
    DtPtr *data,
    DtFlag *userdelete)
{
    DtInt w, h;
    unsigned char *tmpdata;
    unsigned char *d1;
    unsigned char *d2;
    unsigned int   current_outbuf;

    *width = dde_g4vll_current_dcm_data->width;
    *height = dde_g4vll_current_dcm_data->height;
    *userdelete = DcTrue;
    *type = requesttype;

    /* we always want to get the pixels from the display buffer;
       we save the current output buffer and restore its value before exiting */

    current_outbuf = VLLquery_output_buffer();
    VLLset_output_buffer(VLL_DISPLAY_BUFFER);

    if (requesttype == DcRasterRGBAZ ||
	requesttype == DcRasterRGBZ ||
	requesttype == DcRasterZ) {
	tmpdata = (unsigned char *)DDspace_Allocate((*width)*(*height)*8);

	VLLget_image(0, 0, 
		     dde_g4vll_current_dcm_data->width,
		     dde_g4vll_current_dcm_data->height,
		     dde_g4vll_current_dcm_data->width,
		     tmpdata, VLL_RASTER_ZABGR);
    } else if (requesttype == DcRasterRGB ||
	       requesttype == DcRasterRGBA ||
	       requesttype == DcRasterA ||
	       requesttype == DcRasterABGR) {
	tmpdata = (unsigned char *)DDspace_Allocate((*width)*(*height)*4);

	VLLget_image(0, 0, 
		     dde_g4vll_current_dcm_data->width,
		     dde_g4vll_current_dcm_data->height,
		     dde_g4vll_current_dcm_data->width,
		     tmpdata, VLL_RASTER_ABGR);
    } else {
	VLLset_output_buffer (current_outbuf);
	return DcFalse;
    }

    VLLset_output_buffer (current_outbuf);

    switch (requesttype) {
    case DcRasterRGB:
	*data = (DtPtr)DDspace_Allocate((*width)*(*height)*3);
	d1 = (unsigned char *)tmpdata;
	d2 = (unsigned char *)(*data);

	h = (*height);
        if (dor_util_bigendian_word_byte_order()) {
	    while (h--) {
	        w = (*width);
	        while (w--) {
		    d2[0] = d1[3];
		    d2[1] = d1[2];
		    d2[2] = d1[1];
		    d1 += 4;
		    d2 += 3;
	        }
	    }
	} else {
	    while (h--) {
	        w = (*width);
	        while (w--) {
		    d2[0] = d1[0];
		    d2[1] = d1[1];
		    d2[2] = d1[2];
		    d1 += 4;
		    d2 += 3;
	        }
	    }
	}
	DDspace_Deallocate(tmpdata);
	break;
      
    case DcRasterRGBA:
	if (dor_util_bigendian_word_byte_order()) {
	    *data = (DtPtr)DDspace_Allocate((*width)*(*height)*4);
	    d1 = (unsigned char *)tmpdata;
	    d2 = (unsigned char *)(*data);

	    h = (*height);
	    while (h--) {
	        w = (*width);
	        while (w--) {
		    d2[0] = d1[3];
		    d2[1] = d1[2];
		    d2[2] = d1[1];
		    d2[3] = d1[0];
		    d1 += 4;
		    d2 += 4;
	        }
	    }
	    DDspace_Deallocate(tmpdata);
	} else { 
	    *data = (DtPtr) tmpdata;
	}
	break;

    case DcRasterRGBAZ:
	*data = (DtPtr)DDspace_Allocate((*width)*(*height)*8);
	d1 = (unsigned char *)tmpdata;
	d2 = (unsigned char *)(*data);

	h = (*height);
	if (dor_util_bigendian_word_byte_order()) {
	    while (h--) {
	        w = (*width);
	        while (w--) {
		    d2[0] = d1[7];
		    d2[1] = d1[6];
		    d2[2] = d1[5];
		    d2[3] = d1[4];
		    d2[4] = d1[0];
		    d2[5] = d1[1];
		    d2[6] = d1[2];
		    d2[7] = d1[3];
		    d1 += 8;
		    d2 += 8;
	        }
	    }
	} else {
	    while (h--) {
	        w = (*width);
	        while (w--) {
		    d2[0] = d1[4];
		    d2[1] = d1[5];
		    d2[2] = d1[6];
		    d2[3] = d1[7];
		    d2[4] = d1[0];
		    d2[5] = d1[1];
		    d2[6] = d1[2];
		    d2[7] = d1[3];
		    d1 += 8;
		    d2 += 8;
	        }
	    }
	}
	DDspace_Deallocate(tmpdata);
	break;

    case DcRasterRGBZ:
	*data = (DtPtr)DDspace_Allocate((*width)*(*height)*7);
	d1 = (unsigned char *)tmpdata;
	d2 = (unsigned char *)(*data);

	h = (*height);
	if (dor_util_bigendian_word_byte_order()) {
	    while (h--) {
	        w = (*width);
	        while (w--) {
		    d2[0] = d1[7];
		    d2[1] = d1[6];
		    d2[2] = d1[5];
		    d2[3] = d1[0];
		    d2[4] = d1[1];
		    d2[5] = d1[2];
		    d2[6] = d1[3];
		    d1 += 8;
		    d2 += 7;
	        }
	    }
	} else {
	    while (h--) {
	        w = (*width);
	        while (w--) {
		    d2[0] = d1[4];
		    d2[1] = d1[5];
		    d2[2] = d1[6];
		    d2[3] = d1[0];
		    d2[4] = d1[1];
		    d2[5] = d1[2];
		    d2[6] = d1[3];
		    d1 += 8;
		    d2 += 7;
	        }
	    }
	}
	DDspace_Deallocate(tmpdata);
	break;

    case DcRasterA:
	*data = (DtPtr)DDspace_Allocate((*width)*(*height)*1);
	d1 = (unsigned char *)tmpdata;
	d2 = (unsigned char *)(*data);

	h = (*height);
	if (dor_util_bigendian_word_byte_order()) {
	    while (h--) {
	        w = (*width);
	        while (w--) {
		    d2[0] = d1[0];
		    d1 += 4;
		    d2 += 1;
	        }
	    }
	} else {
	    while (h--) {
	        w = (*width);
	        while (w--) {
		    d2[0] = d1[3];
		    d1 += 4;
		    d2 += 1;
	        }
	    }
	}
	DDspace_Deallocate(tmpdata);
	break;

    case DcRasterZ:
	*data = (DtPtr)DDspace_Allocate((*width)*(*height)*4);
	d1 = (unsigned char *)tmpdata;
	d2 = (unsigned char *)(*data);

	h = (*height);
	while (h--) {
	    w = (*width);
	    while (w--) {
	        d2[0] = d1[0];
	        d2[1] = d1[1];
	        d2[2] = d1[2];
	        d2[3] = d1[3];
	        d1 += 8;
	        d2 += 4;
	    }
	}
	DDspace_Deallocate(tmpdata);
	break;

    case DcRasterABGR:
	if (dor_util_bigendian_word_byte_order()) {
	    *data = (DtPtr) tmpdata;
	} else {
	    *data = (DtPtr)DDspace_Allocate((*width)*(*height)*4);
	    d1 = (unsigned char *)tmpdata;
	    d2 = (unsigned char *)(*data);

	    h = (*height);
	    while (h--) {
	        w = (*width);
	        while (w--) {
		    d2[0] = d1[3];
		    d2[1] = d1[2];
		    d2[2] = d1[1];
		    d2[3] = d1[0];
		    d1 += 4;
		    d2 += 4;
	        }
	    }
	    DDspace_Deallocate(tmpdata);
	}
	break;

    case DcRasterSpecial:
	break;
    }
    return (DcTrue);
}

/* ====================================================================== */

void ddr_g4vll_dcm_inquire_resolution (
    DtReal *xres,
    DtReal *yres)
{
    if ((dde_g4vll_current_dcm_data->stereoscreen == VLL_STEREO_PLATE) ||
	(dde_g4vll_current_dcm_data->stereoscreen == VLL_STEREO_GLASSES)) {
	*xres = .31738;
	*yres = .63476;
    } else {
	*xres = .31738;
	*yres = .31738;
    }
}

/* ====================================================================== */

DtFlag ddr_g4vll_dcm_inquire_spheres (void)
{
    return (dde_g4vll_current_dcm_data->use_spheres);
}

/* ====================================================================== */

int ddr_g4vll_dcm_inquire_cylinders (void)
{
    return (dde_g4vll_current_dcm_data->cylinders);
}

/* ====================================================================== */

DtFlag ddr_g4vll_dcm_inquire_hw_boundingvol (void)
{
    return (dde_g4vll_current_dcm_data->use_hw_boundingvol);
}

/* ====================================================================== */

void ddr_g4vll_dcm_inquire_stereo (
    DtFlag *stereoswitch)
{
    *stereoswitch = dde_g4vll_current_dcm_data->stereoswitch ? DcTrue : DcFalse;
}

/* ====================================================================== */

DtInt ddr_g4vll_dcm_inquire_stereo_screen (void)
{
    return(dde_g4vll_current_dcm_data->stereoscreen);
}

/* ====================================================================== */

int ddr_g4vll_dcm_inquire_head_number ()
{
    return(dde_g4vll_current_dcm_data->head);
}

/* ====================================================================== */

DtFlag ddr_g4vll_dcm_inquire_if_states_match (
    VLLstate *state)
{
    /*
     * Determine whether the application's current VLL state matches
     * that of this device.
     */
    VLLstate *curstate;

    curstate = VLLquery_state();

    if (curstate == state) {
	return (DcTrue);
    } else {
	return (DcFalse);
    }
}

/* ====================================================================== */

void ddr_g4vll_dcm_inquire_state (
    VLLstate **state)
{
    *state = dde_g4vll_current_dcm_data->vll_state;
}

/* ====================================================================== */

void ddr_g4vll_dcm_inquire_visual_type (
    DtVisualType *visualtype)
{
    *visualtype = dde_g4vll_current_dcm_data->visualtype;
}

/* ====================================================================== */

DtInt ddr_g4vll_dcm_inq_transp_mode (void)
{
    return dde_g4vll_current_dcm_data->transp_mode;
}

/* ====================================================================== */

/*
 * The XPeer extension changed such that it no longer directly accesses
 * the hardware (i.e. it no longer makes VLL calls).  For continuity, the
 * original XPeer functions called by Dore are here as function wrappers 
 * that call the new XPeer interface.
 */

static Bool XPeerUpdateState (
    Window window)
{
    ddt_g4vll_data *devicedata = dde_g4vll_current_dcm_data;
    unsigned int vll_window_status;
    XPeerDeviceData peer_device_data;

    vll_window_status = VLLquery_window_status(devicedata->vll_window);
    if (devicedata->vll_window_status != vll_window_status) {
	devicedata->vll_window_status = vll_window_status;

	peer_device_data = XPeerGetWindowDeviceData(devicedata->display,
						    devicedata->window);
	if (peer_device_data != NULL) {
	    VLLset_window_properties(devicedata->vll_window,
				     peer_device_data[0],
				     &peer_device_data[1]);
	}

	return True;
    }

    return False;
}

/* ====================================================================== */
