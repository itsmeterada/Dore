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
	ddr_gl_dcm_inquire_color_entries
	ddr_gl_dcm_inquire_auto_size
	ddr_gl_dcm_inquire_clip_list
	ddr_gl_dcm_inquire_device_extent
	ddr_gl_dcm_check_device_extent
	ddr_gl_dcm_inquire_ncolors
	ddr_gl_dcm_inquire_pixel_data
	ddr_gl_dcm_inquire_resolution
	ddr_gl_dcm_inquire_stereo
	ddr_gl_dcm_inquire_visual_type

  ======================================================================
 */
#include "../gl_driver.h"



/* ====================================================================== */

void ddr_gl_dcm_inquire_color_entries (
    DtColorModel colormodel,
    int start,
    int n,
    DtReal *c)
{
    int i;
    short r,g,b;

    if (colormodel != DcRGB) return;

#   ifdef DEBUGP
	printf("(DEVICE)inquire_color_entries(%d..%d) called\n",
	    start, start+n-1);
#   endif

    for (i=0; i<n; i++) {
	getmcolor(start+i,&r,&g,&b);
	*(c++) = r/255.;
	*(c++) = g/255.;
	*(c++) = b/255.;
    }
}



/* ====================================================================== */

void ddr_gl_dcm_inquire_auto_size (DtFlag *flag)
{
    *flag = dde_gl_current_device_data->auto_size;
}



/* ======================================================================== */

void ddr_gl_dcm_inquire_clip_list (
    DtUInt *nrects,
    DtReal **clip_list,
    DtReal extent[4],
    DtFlag *partiallyobscuredflag)
{
    static DtReal clist[4];

    clist[0] = extent[0] = 0;
    clist[1] = extent[1] = 0;
    clist[2] = extent[2] = dde_gl_current_device_data->width;
    clist[3] = extent[3] = dde_gl_current_device_data->height;

    *nrects = 1;
    *clip_list = clist;
    *partiallyobscuredflag = DcFalse;
}



/* ======================================================================== */

void ddr_gl_dcm_inquire_device_extent (DtVolume *volume)
{
    long sx,sy;

    getsize(&sx,&sy);

#   ifdef DEBUGP
	printf ("(DEVICE)Inquire_device_extent() called (size = <%d,%d>)\n",
	    sx, sy);
#   endif

    if ((sx != CURDEV->width) || (sy != CURDEV->height))
	ddr_gl_dcm_clear_both_buffers ();

    CURDEV->width  = sx;
    CURDEV->height = sy;

    volume->bll[0] = 0.0;
    volume->bll[1] = 0.0;
    volume->bll[2] = 0.0;
    volume->fur[0] = CURDEV->width;
    volume->fur[1] = CURDEV->height;
    volume->fur[2] = 1.0;
}



/* ===========================================================================
** This returns true if the geometry has changed since the last
** inquire_extent.
** ======================================================================== */

int ddr_gl_dcm_check_device_extent (void)
{
    auto long sx,sy;

    getsize (&sx,&sy);

    return ((sx != CURDEV->width) || (sy != CURDEV->height));
}



/* ======================================================================== */

void ddr_gl_dcm_inquire_ncolors (DtInt *ncolors)
{
    if (CURDEV->visualtype == DcPseudoColor)
	*ncolors = 1 << getplanes();
    else
	*ncolors = 1;

#   ifdef DEBUGP
	printf ("(DEVICE)Number_colors() inquired (num: %d)\n", *ncolors);
#   endif
}



/* ======================================================================== */

DtFlag ddr_gl_dcm_inquire_pixel_data (
    DtRasterType  requesttype,
    DtInt        *width,
    DtInt        *height,
    DtRasterType *type,
    DtPtr        *data,
    DtFlag       *userdelete)
{
    auto long   *ldata = 0;
    auto char   *ldptr;
    auto char   *rdptr;
    auto DtFlag  rtn = DcTrue;
    auto int     x, y;
    auto long   *zdata = 0;
    auto char   *zdptr;

    if (CURDEV->visualtype == DcPseudoColor)
	return (DcFalse);

    *width = CURDEV->width;
    *height = CURDEV->height;
    *userdelete = DcTrue;

    switch (requesttype) {
	case DcRasterRGB:
	case DcRasterRGBA:
	case DcRasterRGBAZ:
	case DcRasterRGBZ:
	case DcRasterA:
	{   if (!(ldata = DDspace_Allocate (4 * (*width) * (*height))))
		return DcFalse;

	    readsource (SRC_FRONT);
	    lrectread (0, 0, *width - 1, *height - 1, (unsigned long *)ldata);
	    ldptr = ((char*)(ldata)) + ((*width) * (*height - 1) * 4);
	    break;
	}

	case DcRasterZ:
	case DcRasterABGR:
	case DcRasterSpecial:
	    break;
    }

    switch (requesttype) {
	case DcRasterRGBAZ:
	case DcRasterRGBZ:
	case DcRasterZ:
	    zdata = DDspace_Allocate((*width)*(*height)*4);
	    if (!zdata) {
		if (ldata) DDspace_Deallocate (ldata);
		return (DcFalse);
	    }

	    readsource (SRC_ZBUFFER);
	    lrectread (0, 0, (*width)-1, (*height)-1, (unsigned long *)zdata);
	    zdptr = ((char*)(zdata)) + ((*width) * (*height - 1) * 4);
	    break;

	case DcRasterRGB:
	case DcRasterRGBA:
	case DcRasterA:
	case DcRasterABGR:
	case DcRasterSpecial:
	    break;
    }

	
    switch (requesttype) {
	case DcRasterRGB:
	{   *data = DDspace_Allocate((*width)*(*height)*3);
	    if (!*data) {
		rtn = DcFalse;
	    } else {
		*type = DcRasterRGB;

		rdptr = (char *)(*data);
		for (y=0; y<(*height); y++, ldptr -= 8*(*width)) {
		    for (x=0; x<*(width); x++) {
			rdptr[0] = ldptr[3];
			rdptr[1] = ldptr[2];
			rdptr[2] = ldptr[1];
			rdptr += 3;
			ldptr += 4;
		    }
		}
	    }
	    break;
	}

	case DcRasterRGBA:
	{   *data = DDspace_Allocate((*width)*(*height)*4);
	    if (!*data) {
		rtn = DcFalse;
	    } else {
		*type = DcRasterRGBA;

		rdptr = (char *)(*data);
		for (y=0; y<(*height); y++, ldptr -= 8*(*width)) {
		    for (x=0; x<*(width); x++) {
			rdptr[0] = ldptr[3];
			rdptr[1] = ldptr[2];
			rdptr[2] = ldptr[1];
			rdptr[3] = ldptr[0];
			rdptr += 4;
			ldptr += 4;
		    }
		}
	    }
	    break;
	}

	case DcRasterABGR:
	{   *data = DDspace_Allocate((*width)*(*height)*4);
	    if (!*data) {
		rtn = DcFalse;
	    } else {
		*type = DcRasterABGR;

		rdptr = (char *)(*data);
		for (y=0; y<(*height); y++, ldptr -= 8*(*width)) {
		    for (x=0; x<*(width); x++) {
			rdptr[0] = ldptr[0];
			rdptr[1] = ldptr[1];
			rdptr[2] = ldptr[2];
			rdptr[3] = ldptr[3];
			rdptr += 4;
			ldptr += 4;
		    }
		}
	    }
	    break;
	}

	case DcRasterA:
	{   *data = DDspace_Allocate((*width)*(*height));
	    if (!*data) {
		rtn = DcFalse;
	    } else {
		*type = DcRasterA;

		rdptr = (char *)(*data);
		for (y=0; y<(*height); y++, ldptr -= 8*(*width)) {
		    for (x=0; x<*(width); x++) {
			rdptr[0] = ldptr[0];
			rdptr += 1;
			ldptr += 4;
		    }
		}
	    }
	    break;
	}

	case DcRasterRGBAZ:
	{   *data = DDspace_Allocate((*width)*(*height)*8);
	    if (!*data) {
		rtn = DcFalse;
	    } else {
		*type = DcRasterRGBAZ;

		rdptr = (char *)(*data);
		for (y=0; y<(*height); y++, ldptr -= 8*(*width), zdptr -=8*(*width)) {
		    for (x=0; x<*(width); x++) {
			rdptr[0] = ldptr[3];
			rdptr[1] = ldptr[2];
			rdptr[2] = ldptr[1];
			rdptr[3] = ldptr[0];
			rdptr[4] = zdptr[0];
			rdptr[5] = zdptr[1];
			rdptr[6] = zdptr[2];
			rdptr[7] = zdptr[3];
			rdptr += 8;
			ldptr += 4;
			zdptr += 4;
		    }
		}
	    }
	    break;
	}

	case DcRasterRGBZ:
	{   *data = DDspace_Allocate((*width)*(*height)*7);
	    if (!*data) {
		rtn = DcFalse;
	    } else {
		*type = DcRasterRGBZ;

		rdptr = (char *)(*data);
		for (y=0; y<(*height); y++, ldptr -= 8*(*width), zdptr -=8*(*width)) {
		    for (x=0; x<*(width); x++) {
			rdptr[0] = ldptr[3];
			rdptr[1] = ldptr[2];
			rdptr[2] = ldptr[1];
			rdptr[3] = zdptr[0];
			rdptr[4] = zdptr[1];
			rdptr[5] = zdptr[2];
			rdptr[6] = zdptr[3];
			rdptr += 7;
			ldptr += 4;
			zdptr += 4;
		    }
		}
	    }
	    break;
	}

	case DcRasterZ:
	{   *data = DDspace_Allocate((*width)*(*height)*4);
	    if (!*data) {
		rtn = DcFalse;
	    } else {
		*type = DcRasterZ;

		rdptr = (char *)(*data);
		for (y=0; y<(*height); y++,  zdptr -=8*(*width)) {
		    for (x=0; x<*(width); x++) {
			rdptr[0] = zdptr[0];
			rdptr[1] = zdptr[1];
			rdptr[2] = zdptr[2];
			rdptr[3] = zdptr[3];
			rdptr += 4;
			zdptr += 4;
		    }
		}
	    }
	    break;
	}

	default:
	{   rtn = DcFalse;
	    break;
	}
    }

    if (ldata) DDspace_Deallocate (ldata);
    if (zdata) DDspace_Deallocate (zdata);

    readsource (SRC_AUTO);

    return rtn;
}



/* ======================================================================== */

void ddr_gl_dcm_inquire_resolution (
    DtReal *xres,
    DtReal *yres)
{
    static DtReal xmm = -1.;
    static DtReal ymm = -1.;
    DtReal sizemm;
    DtReal sizep;

    if (xmm == -1.) {
	sizemm = (DtReal) getgdesc(GD_XMMAX);
	sizep = (DtReal) getgdesc(GD_XPMAX);
	xmm = sizemm / sizep;

	sizemm = (DtReal) getgdesc(GD_YMMAX);
	sizep = (DtReal) getgdesc(GD_YPMAX);
	ymm = sizemm / sizep;
    }
    *xres = xmm;
    *yres = ymm;
}



/* ======================================================================== */

void ddr_gl_dcm_inquire_stereo (DtFlag *stereoswitch)
{
    *stereoswitch = DcFalse;
}



/* ======================================================================== */

void ddr_gl_dcm_inquire_visual_type (DtVisualType *visualtype)
{
#   ifdef DEBUGP
	printf
	(   "(DEVICE)Inquire_visual_type returns %s (%x)\n",
	    (CURDEV->visualtype == DcPseudoColor) ? "PseudoColor" : "TrueColor",
	    CURDEV->visualtype
	);
#   endif

    *visualtype = CURDEV->visualtype;
}
