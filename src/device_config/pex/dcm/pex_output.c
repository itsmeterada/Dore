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
**  This file contains the DCM routines that modify the output buffer.  The
**  routines write scanlines of data, or clear the drawing region.
*****************************************************************************/

#include "../pex_driver.h"



/*****************************************************************************
**  This routine clears the depth buffer in the specified rectangle.  The
**  rectangle is specified in Dore' device coordinates.
*****************************************************************************/

void  ddr_pex_dcm_clear_rectangle_depth  (
    DtShort Xmin,
    DtShort Ymin,
    DtShort Xmax,
    DtShort Ymax)
{
#   if DEBUG_CALL
	printf ("ddr_pex_dcm_clear_rectangle_depth (%d, %d, %d,%d)\n",
	    Xmin, Xmax, Ymin, Ymax);
	fflush(stdout);
#   endif
}



/*****************************************************************************
**  This procedure clears the depth buffer and fills the specified rectangle
**  with the current background color (as set by dcm.set_background_color()).
**  The rectangle is specified in Dore' device coordinates.
*****************************************************************************/

void  ddr_pex_dcm_clear_rectangle_depth_and_color  (
    DtShort Xmin,
    DtShort Ymin,
    DtShort Xmax,
    DtShort Ymax)
{
#   if DEBUG_CALL
	printf("ddr_pex_dcm_clear_rectangle_depth_and_color (%d, %d, %d, %d)\n",
	    Xmin, Xmax, Ymin, Ymax);
	fflush(stdout);
#   endif
}



/*****************************************************************************
**  This function writes a sequence of pixels to the display device.  The
**  sequence starts at device coordinate `x',`y' for `length' pixels in the
**  positive x direction.  The RGB pixel values are found in `data', and are
**  stored as 8-bit values for each of red, green and blue.  The `x' & `y'
**  will be in the Dore' device coordinate system (same as that used by
**  dcm.inquire_device_extent).
*****************************************************************************/

void  ddr_pex_dcm_write_scanline_byte  (
    DtInt    xstart,	/* X-start of the Scanlines */
    DtInt    ystart,	/* Y-start of the Scanlines */
    DtInt    length,	/* Length of the Pixel Sequence */
    DtUChar *data)	/* Pixel 8-bit RGB Data */
{
    auto DtUChar *dataptr;	/* Data Buffer Pointer */
    auto int    ii;		/* Loop Counter */
    auto char    *pixels;	/* Pixel Output Buffer */
    auto char    *pixptr;	/* Pixel Output Buffer Pointer */

#   if DEBUG_CALL
	printf ("ddr_pex_dcm_write_scanline_byte (%d, %d, %d, %lx)\n",
	    xstart, ystart, length, data);
	fflush(stdout);
#   endif

    pixels = (char*) DDspace_Allocate (length * 4 * sizeof(*pixels));

    pixptr  = pixels;
    dataptr = data;

    for (ii=0;  ii < length;  ++ii)
    {
	pixptr[3] = 0;
	pixptr[2] = *dataptr++;
	pixptr[1] = *dataptr++;
	pixptr[0] = *dataptr++;
	pixptr += 4;
    }

    ystart = DCM->height + 1 - ystart;

    DCM->image->width  = length;
    DCM->image->height = 1;
    DCM->image->data   = pixels;

#ifdef PIXMAP
    /* I've commented this out for now, since the pixmap field doesn't exist
    ** any longer.  I think I should just slap the DCM->window in its place
    ** to get this to work. */

    XPutImage
    (   DCM->display, DCM->pixmap, DCM->gc, DCM->image,
	0,0, xstart,ystart, length, 1
    );
#else
    XPutImage
    (   DCM->display, DCM->dispbuff, DCM->gc, DCM->image,
	0,0, xstart,ystart, length, 1
    );
#endif

    DCM->image->data = 0;
    DDspace_Deallocate (pixels);
}
