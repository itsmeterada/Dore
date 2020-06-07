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
    ddr_kopengl_dcm_clear_rectangle_depth 
    ddr_kopengl_dcm_clear_rectangle_depth_and_color 
    ddr_kopengl_dcm_write_scanline_byte 

**************************************************************************/

#include <stdio.h>
#include "dcm.h"

/****************************************************************************
 *  This routine clears the depth buffer in the specified rectangle.  The
 *  rectangle is specified in Dore' device coordinates.
 ****************************************************************************/

void ddr_kopengl_dcm_clear_rectangle_depth (DtShort xmin, 
                                            DtShort ymin, 
                                            DtShort xmax, 
                                            DtShort ymax)
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ...in clear_rectangle_depth\n");
#endif

  glScissor (xmin, ymin, xmax-xmin, ymax-ymin);
  glEnable (GL_SCISSOR_TEST);
  glClearDepth (1);
  glClear (GL_DEPTH_BUFFER_BIT);
  glDisable (GL_SCISSOR_TEST);
 }


/****************************************************************************
 *  This procedure clears the depth buffer and fills the specified rectangle
 *  with the current background color (as set by dcm.set_background_color()).
 *  The rectangle is specified in Dore' device coordinates.
 ****************************************************************************/

void ddr_kopengl_dcm_clear_rectangle_depth_and_color (DtShort xmin,
                                                      DtShort ymin, 
                                                      DtShort xmax, 
                                                      DtShort ymax)
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ...in clear_rectangle_depth_and_color\n");
#endif

  glScissor (xmin, ymin, xmax-xmin, ymax-ymin);
  glEnable (GL_SCISSOR_TEST);
  glClearColor (DCM->bg_color[0], DCM->bg_color[1], DCM->bg_color[2], 0.0);
  glClearDepth (1);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable (GL_SCISSOR_TEST);
 }



/****************************************************************************
 *  This function writes a sequence of pixels to the display device.  The
 *  sequence starts at device coordinate `x',`y' for `length' pixels in the
 *  positive x direction.  The RGB pixel values are found in `data', and are
 *  stored as 8-bit values for each of red, green and blue.  The `x' & `y'
 *  will be in the Dore' device coordinate system (same as that used by
 *  dcm.inquire_device_extent).
 ****************************************************************************/

void ddr_kopengl_dcm_write_scanline_byte (DtInt    xstart, 
                                          DtInt    ystart, 
                                          DtInt    length, 
                                          DtUChar *data)
 {
  ddt_kopengl_data *device_data;

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ...in write_scanline_byte [%d, %d]\n",
           xstart, ystart);
#endif

  /* Get a pointer to the current device data */
  device_data = DCM;

  /* Check for the boundary conditions */
  if ( (ystart < 0) || 
       (ystart >= device_data->height) || 
       (xstart >= device_data->width) )
    return;

  if (xstart < 0)
    {
     length += xstart;
     xstart = 0; 
    }

  if (length < 0)
    return;

  if (xstart + length > device_data->width)
    length = device_data->width - xstart;

  /*
   * Set raster position the draw the pixels. 
   * NOTE: glOrtho was set in dcm.initialize_device because 
   * glRasterPosxx() coords are transformed by modelview and
   * projection matrices. 
   */ 
  glRasterPos2i (xstart, ystart);
  glDrawPixels (length, 1, GL_RGB, GL_UNSIGNED_BYTE, (char *) data); 

  return;
 }
