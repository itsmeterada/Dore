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
 
/*****************************************************************************
**  This file contains the DCM routines that the renderer uses to inquire
**  about DCM properties.
*****************************************************************************/

#include "../pex_driver.h"



/*****************************************************************************
**  This function returns DcTrue in the `flag' parameter if the user specified
**  that Dore' should adjust aspect ratios when the device window changes,
**  otherwise `flag' is set to DcFalse.
*****************************************************************************/

void  ddr_pex_dcm_inquire_auto_size  (DtFlag *flag)
{
#   if DEBUG_CALL
	printf ("ddr_pex_dcm_inquire_auto_size (%lx)\n", flag);  fflush(stdout);
	fflush (stdout);
#   endif

    *flag = DCM -> auto_size;

#   if DEBUG_CALL
	printf ("+ returned %d.\n", *flag);  fflush(stdout);
	fflush (stdout);
#   endif
}



/*****************************************************************************
**  This returns the current clipping rectangle list.  This function returns
**  in the parameter `clip_list' a pointer to an array of 4*`nrects' DtReal
**  values that specify the visible rectangles on the device.  If the device
**  is fully visible then `clip_list' will contain one rectangle that is the
**  same as `extent'.  The elements of `clip_list' for the ith rectangle are
**  clip_list[4*i+0], clip_list[4*i+1], clip_list[4*i+2], and clip_list[4*i+3]
**  and define Xmin, Ymin, Xmax and Ymax, respectively.  The parameter
**  `extent' is the full unobscured extent of the device.  The parameter
**  `obscured' is set to DcTrue if the full device extent is not currently
**  visible, othersize it's set to DcFalse.
*****************************************************************************/

void  ddr_pex_dcm_inquire_clip_list  (
    DtUInt *nrects,
    DtReal **clip_list,
    DtReal extent[4],
    DtFlag *obscured)		/* True if Partially Obscurred */
{
    static DtReal clist[4];

#   if DEBUG_CALL
	printf ("ddr_pex_dcm_inquire_clip_list (%lx, %lx, %lx, %lx)\n",
	    nrects, clip_list, extent, obscured);
	fflush(stdout);
#   endif

    clist[0] = extent[0] = 0;
    clist[1] = extent[1] = 0;
    clist[2] = extent[2] = DCM -> width;
    clist[3] = extent[3] = DCM -> height;

    *nrects = 1;
    *clip_list = clist;
    *obscured = DcFalse;

#   if DEBUG_CALL
	printf ("+     returns %u nrects, obscured %d,\n", *nrects, *obscured);
	printf ("+             extent %g %g %g %g\n", extent[0], extent[1],
	    extent[2], extent[3]);
	fflush(stdout);
#   endif
}



/*****************************************************************************
**  This routine returns `count' color entries from the device color map
**  beginning at entry `start'.  The paramter `colormodel' indicates which
**  color model to use.  If the color model is DcRGB, the length of colors
**  is 3 * `count' and the values returned in `colors' are be in the range
**  of [0,1].  In this case, the entries colors[3*i+0], colors[3*i+1], and
**  colors[3*i+2] are the red, green and blue values of the ith color starting
**  from `start'.
*****************************************************************************/

void  ddr_pex_dcm_inquire_color_entries  (
    DtColorModel colormodel,	/* Color Format */
    DtInt        start,		/* Starting Index of Color Entries */
    DtInt        count,		/* Number of Color Entries to Return */
    DtReal       colors[])	/* Receiving Array of Color Entries */
{
    static char funcname[] = "ddr_pex_dcm_inquire_color_entries";

    register DtReal           *cptr;		/* Color Component Pointer */
    register int               ii;		/* Loop Index */
    auto     XWindowAttributes winattrs;	/* X Window Attributes */
    static   XColor            xcolors[4096];	/* X Color Array */

#   if DEBUG_CALL
	printf ("ddr_pex_dcm_inquire_color_entries (%d, %d, %d, %lx)\n",
	    colormodel, start, count, colors);
	fflush(stdout);
#   endif

    if (DCM->visualtype == DcTrueColor) return;
    if (colormodel != DcRGB) return;

    if ((start + count) > DCM->ncolors)
    {   DDerror (ERR_VALUE_OUT_OF_RANGE, funcname,
		 "(start + count) is outside colormap size");
	count = DCM->ncolors - start;
    } 

    if (count < 0)
    {   DDerror (ERR_VALUE_OUT_OF_RANGE, funcname, "count is less than zero");
	return;
    }

    /*add an XSync for bug in Xlib. See TRs Dore-0657 and 0707 */
    XSync(DCM->display, False);
    XGetWindowAttributes (DCM->display, DCM->window, &winattrs);

    for (ii=0;  ii < count;  ++ii)
	xcolors[ii].pixel = start + ii;

    XQueryColors(DCM->display, winattrs.colormap, xcolors, count);

    cptr = colors;

    for (ii=0;  ii < count;  ++ii)
    {   auto DtReal red, green, blue;

	red   = (DtReal) xcolors[ii].red   / (DtReal) 65535.0;
	green = (DtReal) xcolors[ii].green / (DtReal) 65535.0;
	blue  = (DtReal) xcolors[ii].blue  / (DtReal) 65535.0;

	*cptr++ = CLAMP (red,   0.0, 1.0);
	*cptr++ = CLAMP (green, 0.0, 1.0);
	*cptr++ = CLAMP (blue,  0.0, 1.0);
    }
}



/*****************************************************************************
**  This routine returns the 3D extent of the device volume.  The extent is
**  in the Dore' device coordinate system, which has the origin at the lower
**  left hand corner with Y up, X to the right and Z out of the screen.
*****************************************************************************/

void  ddr_pex_dcm_inquire_device_extent  (DtVolume *volume)
{
#   if DEBUG_CALL
	printf ("ddr_pex_dcm_inquire_device_extent (%lx)\n", volume);
	fflush(stdout);
#   endif

    volume->bll[0] = 0.0;
    volume->bll[1] = 0.0;
    volume->bll[2] = 0.0;

    volume->fur[0] = DCM->width;
    volume->fur[1] = DCM->height;
    volume->fur[2] = 1.0;
}



/*****************************************************************************
**  This function returns in `ncolors' the number of color table entries that
**  the current device supports.
*****************************************************************************/

void  ddr_pex_dcm_inquire_ncolors  (DtInt *ncolors)
{
#   if DEBUG_CALL
	printf ("ddr_pex_dcm_inquire_ncolors (%lx)\n", ncolors);
	fflush(stdout);
#   endif

    *ncolors = DCM->ncolors;

#   if DEBUG_CALL
	printf ("+     returned %d.\n", *ncolors);
	fflush(stdout);
#   endif
}



/*****************************************************************************
**  This function returns in `data' the raster data currently displayed on the
**  device.  If we cannot provide this information, then inquire_pixel_data()
**  returns DcFalse.
*****************************************************************************/

DtFlag  ddr_pex_dcm_inquire_pixel_data  (
    DtRasterType  requesttype,	/* Type of Image Data Requested */
    DtInt        *width,	/* Image Width */
    DtInt        *height,	/* Image Height */
    DtRasterType *type,		/* Type of Image Data Returned */
    DtPtr        *data,		/* Address of Image Data Pointer */
    DtFlag       *userdelete)	/* Caller-Delete-Image-Data Flag */
{
    auto     unsigned char *dataptr;	/* Image Data Pointer */
    auto     XImage        *image;	/* Pointer to Image */
    register int            ii, jj;	/* Loop Indices */
    auto     unsigned char *lineptr;	/* Scanline Data Pointer */
    auto     DtFlag         status;	/* Status Return Flag */

#   if DEBUG_CALL
	printf("ddr_pex_dcm_inquire_pixel_data (%d, %lx, %lx, %lx, %lx, %lx)\n",
	    requesttype, width, height, type, data, userdelete);
	fflush(stdout);
#   endif

    if (requesttype == DcRasterA || requesttype == DcRasterZ)
      /* don't know how to get alpha or depth info */
      return (DcFalse);

   /* this device always returns *type=DcRasterRGB */

   if (requesttype != DcRasterRGB &&
       requesttype != DcRasterABGR &&
       requesttype != DcRasterRGBA &&
       requesttype != DcRasterRGBZ &&
       requesttype != DcRasterRGBAZ)
      /* unknown raster type */
      return (DcFalse);

    *type   = DcRasterRGB;

#ifdef PIXMAP
    /*add an XSync for bug in Xlib. See TRs Dore-0657 and 0707 */
    XSync(DCM->display, False);
    image = XGetImage (DCM->display, DCM->pixmap, 0,0, DCM->width, DCM->height,
		       AllPlanes, ZPixmap);

    if (!image) return DcFalse;

    *width  = image->width;
    *height = image->height;
    *data   = DDspace_Allocate
		  (sizeof(*dataptr) * 3 * image->width * image->height);

    *userdelete = DcTrue;

    status = DcFalse;

    if (  (DCM->window_attrs.visual->class == TrueColor)
       || (DCM->window_attrs.visual->class == DirectColor)
       )
    {
	lineptr = (unsigned char *) image->data;
	dataptr = (unsigned char *) *data;

	for (ii=0;  ii < image->height;  ++ii)
	{   auto unsigned char *ptr;	/* Data RGB Pointer */

	    ptr = lineptr;

	    for (jj=0;  jj < image->width;  ++jj)
	    {   if (image->bits_per_pixel > 24)
		    ++ptr;
		*dataptr++ = *ptr++;
		*dataptr++ = *ptr++;
		*dataptr++ = *ptr++;
	    }
	    lineptr += image->bytes_per_line;
	}
	status = DcTrue;
    }
    else if (DCM->window_attrs.visual->class == PseudoColor)
    {
	auto XColor colors[256];	/* Colormap Entries */

	for (ii=0;  ii < DCM->window_attrs.visual->map_entries;  ++ii)
	    colors[ii].pixel = ii;

	XQueryColors (DCM->display, DCM->window_attrs.colormap, colors,
		      DCM->window_attrs.visual->map_entries);

	lineptr = (unsigned char *) image->data;
	dataptr = (unsigned char *) *data;

	for (ii=0;  ii < image->height;  ++ii)
	{   auto unsigned char *ptr;	/* Data RGB Pointer */
	    auto int            index;	/* Colormap Index */
	    
	    ptr = lineptr;

	    for (jj=0;  jj < image->width;  ++jj)
	    {   index = *ptr++;
		if (image->depth > 8)
		    index = (index << 8) | *ptr++;
		*dataptr++ = colors[index].red   >> 8;
		*dataptr++ = colors[index].green >> 8;
		*dataptr++ = colors[index].blue  >> 8;
	    }
	    lineptr += image->bytes_per_line;
	}
	status = DcTrue;
    }
    else
    {   printf ("ddr_pex_dcm_inquire_pixel_data:  Unknown visual class (%d).\n",
		DCM->window_attrs.visual->class);
	return DcFalse;
    }

    XDestroyImage (image);
    return status;
#else
    return 0;
#endif
}



/*****************************************************************************
**  This function returns the physical size of a pixel on the screen in
**  millimeters.
*****************************************************************************/

void  ddr_pex_dcm_inquire_resolution  (
    DtReal *xres,
    DtReal *yres)
{
    auto Screen *screen;	/* The Current X Screen */

#   if DEBUG_CALL
	printf ("ddr_pex_inquire_resolution (%lx, %lx)\n", xres, yres);
	fflush(stdout);
#   endif

    /* This routine find the size of a single pixel in millimeters.  This is
    ** mostly useful to determine the aspect ratio of the current display. */

    screen = ScreenOfDisplay (DCM->display, DCM->screen);

    *xres = (DtReal)(WidthMMOfScreen(screen))
	  / (DtReal)(WidthOfScreen(screen));
    *yres = (DtReal)(HeightMMOfScreen(screen))
	  / (DtReal)(HeightOfScreen(screen));

#   if DEBUG_CALL
	printf ("+     returns xres %g, yres %g\n", *xres, *yres);
	fflush(stdout);
#   endif
}



/*****************************************************************************
**  This inquiry function sets the boolean `stereoflag' parameter to true if
**  the device is a stereo device, otherwise it returns false.
*****************************************************************************/

void  ddr_pex_dcm_inquire_stereo  (DtFlag *stereoflag)
{
#   if DEBUG_CALL
	printf ("ddr_pex_dcm_inquire_stereo (%lx)\n", stereoflag);
	fflush(stdout);
#   endif

    *stereoflag = DcFalse;
}



/*****************************************************************************
**  This function returns the visual type of the current device.
*****************************************************************************/

void  ddr_pex_dcm_inquire_visual_type  (DtVisualType *visualtype)
{
#   if DEBUG_CALL
	printf ("ddr_pex_dcm_inquire_visualtype (%lx)\n", visualtype);
	fflush(stdout);
#   endif

    *visualtype = DCM->visualtype;
}
