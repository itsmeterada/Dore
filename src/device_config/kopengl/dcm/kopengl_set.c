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
    ddr_kopengl_dcm_set_background_color 
    ddr_kopengl_dcm_set_foreground_color 
    ddr_kopengl_dcm_set_color_entries 
    ddr_kopengl_dcm_set_current_view 
    ddr_kopengl_dcm_set_depth_buffer_enable 
    ddr_kopengl_dcm_set_depth_buffer_write 
    ddr_kopengl_dcm_set_shade_index 

**************************************************************************/

#include <stdio.h>
#include <string.h>
#include "dcm.h"

/****************************************************************************
 *  This routine sets the background color of the device.  If `colormodel' is
 *  DcRGB, then color[0] is the red component, color[1] is the green component
 *  and color[2] is the blue component.  All values are in the range of [0,1].
 ****************************************************************************/

void ddr_kopengl_dcm_set_background_color (DtColorModel colormodel, DtReal c[])
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_set_background_color\n");
#endif

  if (colormodel == DcRGB) 

    {
     DCM->bg_color[0] = c[0];
     DCM->bg_color[1] = c[1];
     DCM->bg_color[2] = c[2];
    }

  return;
 }


/****************************************************************************
 *
 ****************************************************************************/

void ddr_kopengl_dcm_set_foreground_color (DtColorModel colormodel, DtReal *c)
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_set_foreground_color\n");
#endif
 }


/****************************************************************************
 *
 ****************************************************************************/

void ddr_kopengl_dcm_set_color_entries (DtColorModel colormodel, int start, 
                                        int n, DtReal *c)
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_set_color_entries\n");
#endif
 }


/****************************************************************************
 *
 ****************************************************************************/

void ddr_kopengl_dcm_set_current_view (DtObject view)
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_set_current_view\n");
#endif
 }


/****************************************************************************
 *
 ****************************************************************************/

void ddr_kopengl_dcm_set_depth_buffer_enable (DtFlag flag)
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_set_depth_buffer_enable\n");
  fprintf (stdout, "depth buffer enable flag = %d\n", flag);
#endif

  if (flag)
    glEnable (GL_DEPTH_TEST);
  else
    glDisable (GL_DEPTH_TEST);
 }


/****************************************************************************
 *
 ****************************************************************************/

void ddr_kopengl_dcm_set_depth_buffer_write (DtFlag flag)
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_set_depth_buffer_write\n");
#endif

  glDepthMask (flag); 
 }


/****************************************************************************
 *
 ****************************************************************************/

void ddr_kopengl_dcm_set_shade_index (DtInt index)
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_set_shade_index\n");
#endif

 }
