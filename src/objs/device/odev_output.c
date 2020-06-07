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
	dor_device_clear_rectangle_depth
	dor_device_clear_rectangle_depth_and_color
	dor_device_write_pixel_byte_rgb
	dor_device_write_rectangle_byte_rgb
	dor_device_write_scanline_byte_rgb
	dor_device_flush
	dor_device_update_geometry

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/device.h>
/*
 ======================================================================
 */
void dor_device_clear_rectangle_depth (
    dot_object *device_object,
    DtInt       xmin,
    DtInt       ymin,
    DtInt       xmax,
    DtInt       ymax)
{
    SET_CURRENT_DEVICE(device_object);
    OUTFUN(clear_rectangle_depth)((DtShort)xmin,(DtShort)ymin,
				  (DtShort)xmax,(DtShort)ymax);
}
/*
 ======================================================================
 */
void dor_device_clear_rectangle_depth_and_color (
    dot_object *device_object,
    DtInt       xmin,
    DtInt       ymin,
    DtInt       xmax,
    DtInt       ymax)
{
    SET_CURRENT_DEVICE(device_object);
    OUTFUN(clear_rectangle_depth_and_color)((DtShort)xmin,(DtShort)ymin,
					    (DtShort)xmax,(DtShort)ymax);
}
/*
 ======================================================================
 */
void dor_device_write_pixel_byte_rgb (
    dot_object *device_object,
    DtInt       pixelno,
    DtInt       scanline,
    DtUChar    *pixel)
{
    SET_CURRENT_DEVICE(device_object);
    OUTFUN(write_scanline_byte)(pixelno, scanline, 1, pixel) ;
}
/*
 ======================================================================
 */
void dor_device_write_rectangle_byte_rgb (
    dot_object *device_object,
    DtInt       xstart,
    DtInt       ystart,
    DtInt       length,
    DtInt       height,
    DtUChar    *pixarr)
{
    DtInt yend;
    DtInt scanlineno;
    DtUChar *pixeladdr;

    SET_CURRENT_DEVICE(device_object);

    pixeladdr = pixarr;
    yend = ystart + height - 1;

    for (scanlineno=ystart;scanlineno<=yend;scanlineno++){
	OUTFUN(write_scanline_byte)(xstart,scanlineno,length,
				    pixeladdr);

	pixeladdr += 3*length;
    }
}
/*
 ======================================================================
 */
void dor_device_write_scanline_byte_rgb (
    dot_object *device_object,
    DtInt       pixel,
    DtInt       scanline,
    DtInt       length,
    DtUChar    *pixarr)
{
    SET_CURRENT_DEVICE (device_object);
    OUTFUN(write_scanline_byte)(pixel, scanline, length, pixarr) ;
}
/*
 ======================================================================
 */
void dor_device_flush (dot_object *device_object)
{
    SET_CURRENT_DEVICE (device_object);
    OUTFUN(flush)() ;
}
/*
 ======================================================================
 */
void dor_device_update_geometry (dot_object *device_object)
{
    SET_CURRENT_DEVICE(device_object);
    OUTFUN(update_geometry)();
}
