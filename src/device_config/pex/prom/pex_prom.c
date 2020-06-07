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

#include "../pex_driver.h"

static DDt_PROM	*dde_pex_PROM_fcns = 0;



/*****************************************************************************
**  This routine writes a rectangle of pixels to the display device.  The
**  array 'pixarr' contains the pixel data to be written into a rectangle on
**  the device.  The rectangle starts at 'x','y' and is 'length' wide and
**  'height' high.  The array 'pixarr' is 3*length*height elements long, and
**  stores pixels in scanline order.  pixarr[3*i], pixarr[3*i + 1] and
**  pixarr[3*i + 2] are the red, green, and blue components of the i'th pixel.
**  Each pixel value in 'pixarr' is in the range [0,255].  The rectangle is in
**  Dore' device coordinates, that is, the origin is at the lower left hand
**  corner of the device, with y up, x to the right, and z coming out of the
**  screen.
*****************************************************************************/

void ddr_pex_prom_write_rectangle_byte_rgb (
    DtInt xstart,
    DtInt ystart,	/* Corner of the Rectangle Region */
    DtInt length,
    DtInt height,	/* Size of the Rectangle Region */
    DtUChar *pixdata)	/* 24-bit Pixel Data Array */
{
    auto DtInt yend;
    auto DtInt scanline;

    yend = ystart + height - 1;

    for (scanline=ystart;  scanline <= yend;  ++scanline)
    {   ddr_pex_dcm_write_scanline_byte (xstart, scanline, length, pixdata);
	pixdata += 3 * length;
    }
}



/*****************************************************************************
**  This function returns a pointer to the filled-in PROM function pointer
**  structure.
*****************************************************************************/

DtPtr ddr_pex_return_PROM_fcns (void)
{
    if (!dde_pex_PROM_fcns)
    {
	auto DDt_PROM *fcns;	/* PROM Function Pointer Structure */

	if (!(fcns = DDdevice_CreatePROMStruct()))
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_pex_return_PROM_fcns", DcNullPtr);

	fcns->write_rectangle_byte_rgb 
		= ddr_pex_prom_write_rectangle_byte_rgb;

	dde_pex_PROM_fcns = fcns;
    }

    return (DtPtr) dde_pex_PROM_fcns;
}
