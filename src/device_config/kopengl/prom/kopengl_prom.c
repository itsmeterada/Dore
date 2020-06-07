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
 
/**************************************************************************

  Functions:
    void ddr_kopengl_prom_write_rectangle_byte_rgb (xstart, ystart, length, height, pixarr)
    DtPtr ddr_kopengl_return_PROM_fcns ()

**************************************************************************/

#include <stdio.h>
#include <dore/dore.h>
#include <dore/dore_develop/error.h>
#include <dore/dore_develop/develop.h>
#include "../dcm/dcm.h"


static DDt_PROM	*dde_kopengl_PROM_fcns = 0;


/*****************************************************************************
 *  This routine writes a rectangle of pixels to the display device.  The
 *  array 'pixarr' contains the pixel data to be written into a rectangle on
 *  the device.  The rectangle starts at 'x','y' and is 'length' wide and
 *  'height' high.  The array 'pixarr' is 3*length*height elements long, and
 *  stores pixels in scanline order.  pixarr[3*i], pixarr[3*i + 1] and
 *  pixarr[3*i + 2] are the red, green, and blue components of the i'th pixel.
 *  Each pixel value in 'pixarr' is in the range [0,255].  The rectangle is in
 *  Dore' device coordinates, that is, the origin is at the lower left hand
 *  corner of the device, with y up, x to the right, and z coming out of the
 *  screen.
 ****************************************************************************/


void ddr_kopengl_prom_write_rectangle_byte_rgb (DtInt xstart, DtInt ystart, 
                                                DtInt length, DtInt height, 
                                                DtUChar *pixarr)
 {
  DtInt    yend;
  DtInt    scanlineno;
  DtUChar *pixeladdr;

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] prom.write_rectangle_byte_rgb\n");
#endif

  pixeladdr = pixarr;
  yend = ystart + height - 1;
 
  for (scanlineno=ystart; scanlineno<=yend; scanlineno++)
    {
     ddr_kopengl_dcm_write_scanline_byte(xstart, scanlineno, length, pixeladdr);

     pixeladdr += 3*length;
    }

  return;
 }


DtPtr ddr_kopengl_return_PROM_fcns (void)
 {
  DDt_PROM *fcns;

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ddr_kopengl_return_PROM_fcns\n");
#endif

  if (dde_kopengl_PROM_fcns == (DDt_PROM *)0) 
    {
     fcns = DDdevice_CreatePROMStruct();

     if (fcns == (DDt_PROM *)0) 
       {
	DDerror (ERR_CANT_ALLOC_MEM, "dde_kopengl_return_PROM_fcns",
		   DcNullPtr);
       }

     fcns->version_number = 1;
     fcns->write_rectangle_byte_rgb = ddr_kopengl_prom_write_rectangle_byte_rgb;

     dde_kopengl_PROM_fcns = fcns;
    }

  return (DtPtr) dde_kopengl_PROM_fcns;
 }
