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
	void ddr_vll_dcm_inquire_color_entries (colormodel,start, n, c)
	void ddr_vll_dcm_inquire_auto_size (flag)
	void ddr_vll_dcm_inquire_clip_list (nrects, clip_list, extent, partiallyobscuredflag)
	int ddr_vll_dcm_inq_pseudo_range (weight)
	void ddr_vll_dcm_inq_pseudo_space (weight, mask)
	void ddr_vll_dcm_inquire_device_extent (volume)
	void ddr_vll_dcm_inquire_ncolors (ncolors)
	DtFlag ddr_vll_dcm_inquire_pixel_data (requesttype, width, height, type, data, userdelete)
	void ddr_vll_dcm_inquire_resolution (xres, yres)
	DtFlag ddr_vll_dcm_inquire_spheres ()
	void ddr_vll_dcm_inquire_stereo (stereoswitch)
	void ddr_vll_dcm_inquire_visual_type (visualtype)
	DtInt ddr_vll_dcm_inq_texture_expand ()

  ======================================================================
 */
#include "dcm.h"

/*
 ======================================================================
 */
void
ddr_vll_dcm_inquire_color_entries (colormodel,start, n, c)
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
	DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_vll_dcm_inquire_color_entries",
		    "(start + count) is outside colormap size");
	n = devicedata->ncolors - start;
    } 
    if (n < 0) {
	DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_vll_dcm_inquire_color_entries",
		    "count less than zero");
	return;
    }

    if (devicedata->use_X && (devicedata->ncolors == 256)) {
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
/*
 ======================================================================
 */
void
ddr_vll_dcm_inquire_auto_size (flag)
	DtFlag *flag;
{
    *flag = dde_vll_current_dcm_data->auto_size;
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_inquire_clip_list (nrects, clip_list,
			       extent, partiallyobscuredflag)
	DtUInt *nrects;
	DtReal **clip_list;
	DtReal extent[4];
	DtFlag *partiallyobscuredflag;
{
   static DtReal clist[4];

   clist[0] = extent[0] = 0;
   clist[1] = extent[1] = 0;
   clist[2] = extent[2] = dde_vll_current_dcm_data->width;
   clist[3] = extent[3] = dde_vll_current_dcm_data->height;

   *nrects = 1;
   *clip_list = clist;
   *partiallyobscuredflag = DcFalse;
}
/*
 ======================================================================
 */
ddr_vll_dcm_inq_pseudo_range(weight)
	float weight[3];
{
    weight[0] = dde_vll_current_dcm_data->cwtavgpsweight[0];
    weight[1] = dde_vll_current_dcm_data->cwtavgpsweight[1];
    weight[2] = dde_vll_current_dcm_data->cwtavgpsweight[2];
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_inq_pseudo_space(weight, mask)
	float weight[3];
	int mask[3];
{
    weight[0] = dde_vll_current_dcm_data->cbtselpsweight[0];
    weight[1] = dde_vll_current_dcm_data->cbtselpsweight[1];
    weight[2] = dde_vll_current_dcm_data->cbtselpsweight[2];
   
    mask[0] = dde_vll_current_dcm_data->cbtselpsmask[0];
    mask[1] = dde_vll_current_dcm_data->cbtselpsmask[1];
    mask[2] = dde_vll_current_dcm_data->cbtselpsmask[2];
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_inquire_device_extent (volume)
	DtVolume *volume;
{
    volume->bll[0] = 0.0;
    volume->bll[1] = 0.0;
    volume->bll[2] = 0.0;

    volume->fur[0] = dde_vll_current_dcm_data->width;
    volume->fur[1] = dde_vll_current_dcm_data->height;
    volume->fur[2] = 1.0;
}

/*
 ======================================================================
 */
void
ddr_vll_dcm_inquire_ncolors (ncolors)
	DtInt *ncolors;
{
    *ncolors = dde_vll_current_dcm_data->ncolors;
}
/*
 ======================================================================
 */
DtFlag
ddr_vll_dcm_inquire_pixel_data (requesttype, width, height, type, 
			       data, userdelete)
	DtRasterType requesttype;
	DtInt *width;
	DtInt *height;
	DtRasterType *type;
	DtPtr *data;
	DtFlag *userdelete;
{
    DtInt w, h;
    unsigned char *tmpdata;
    unsigned char *d1;
    unsigned char *d2;

    *width = dde_vll_current_dcm_data->width;
    *height = dde_vll_current_dcm_data->height;
    *userdelete = DcTrue;
    *type = requesttype;

    if (requesttype == DcRasterRGBAZ ||
	requesttype == DcRasterRGBZ ||
	requesttype == DcRasterZ) {
	tmpdata = (unsigned char *)DDspace_Allocate((*width)*(*height)*8);

	VLLget_image(0, 0, 
		     dde_vll_current_dcm_data->width,
		     dde_vll_current_dcm_data->height,
		     dde_vll_current_dcm_data->width,
		     tmpdata, VLL_RASTER_ZABGR);
    } else if (requesttype == DcRasterRGB ||
	       requesttype == DcRasterRGBA ||
	       requesttype == DcRasterA ||
	       requesttype == DcRasterABGR) {
	tmpdata = (unsigned char *)DDspace_Allocate((*width)*(*height)*4);

	VLLget_image(0, 0, 
		     dde_vll_current_dcm_data->width,
		     dde_vll_current_dcm_data->height,
		     dde_vll_current_dcm_data->width,
		     tmpdata, VLL_RASTER_ABGR);
    } else {

        /* unlock frame buffer */
        if (dde_vll_current_dcm_data->offscreen) {
            VLLset_output_buffer(VLL_NONDISPLAY_BUFFER);
            dde_vll_current_dcm_data->output_buffer = VLL_NONDISPLAY_BUFFER;
            }

	return DcFalse;
    }

    switch (requesttype) {
    case DcRasterRGB:
	*data = (DtPtr)DDspace_Allocate((*width)*(*height)*3);
	d1 = (unsigned char *)tmpdata;
	d2 = (unsigned char *)(*data);

	h = (*height);
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
	DDspace_Deallocate(tmpdata);
	break;
      
    case DcRasterRGBA:
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
	break;

    case DcRasterRGBAZ:
	*data = (DtPtr)DDspace_Allocate((*width)*(*height)*8);
	d1 = (unsigned char *)tmpdata;
	d2 = (unsigned char *)(*data);

	h = (*height);
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
	DDspace_Deallocate(tmpdata);
	break;

    case DcRasterRGBZ:
	*data = (DtPtr)DDspace_Allocate((*width)*(*height)*7);
	d1 = (unsigned char *)tmpdata;
	d2 = (unsigned char *)(*data);

	h = (*height);
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
	DDspace_Deallocate(tmpdata);
	break;

    case DcRasterA:
	*data = (DtPtr)DDspace_Allocate((*width)*(*height)*1);
	d1 = (unsigned char *)tmpdata;
	d2 = (unsigned char *)(*data);

	h = (*height);
	while (h--) {
	    w = (*width);
	    while (w--) {
		d2[0] = d1[0];
		d1 += 4;
		d2 += 1;
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
	*data = (DtPtr) tmpdata;
	break;
    }

    /* unlock frame buffer */
    if (dde_vll_current_dcm_data->offscreen) {
        VLLset_output_buffer(VLL_NONDISPLAY_BUFFER);
        dde_vll_current_dcm_data->output_buffer = VLL_NONDISPLAY_BUFFER;
    }

    return (DcTrue);
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_inquire_resolution (xres, yres)
	DtReal *xres;
	DtReal *yres;
{
    if (dde_vll_current_dcm_data->stereoswitch) {
	*xres = .31738;
	*yres = .63476;
    } else {
	*xres = .31738;
	*yres = .31738;
    }
}
/*
 ======================================================================
 */
DtFlag
ddr_vll_dcm_inquire_spheres()
{
    return (dde_vll_current_dcm_data->use_spheres);
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_inquire_stereo (stereoswitch)
	DtFlag *stereoswitch;
{
    *stereoswitch = dde_vll_current_dcm_data->stereoswitch ? DcTrue : DcFalse;
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_inquire_visual_type (visualtype)
	DtVisualType *visualtype;
{
    *visualtype = dde_vll_current_dcm_data->visualtype;
}
/*
 ======================================================================
 */
DtInt
ddr_vll_dcm_inq_texture_expand()
{
    return dde_vll_current_dcm_data->texture_expand;
}
/*
 ======================================================================
 */
