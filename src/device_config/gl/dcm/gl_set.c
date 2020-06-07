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
	ddr_gl_dcm_set_background_color
	ddr_gl_dcm_set_foreground_color
	ddr_gl_dcm_set_color_entries
	ddr_gl_dcm_set_current_view
	ddr_gl_dcm_set_depth_buffer_enable
	ddr_gl_dcm_set_depth_buffer_write
	ddr_gl_dcm_set_shade_index

  ======================================================================
 */
#include "../gl_driver.h"



/* ======================================================================== */

void ddr_gl_dcm_set_background_color (
    DtColorModel colormodel,
    DtReal *c)
{
    DtInt shademin,shademax;

#   ifdef DEBUGP
	printf ("(DEVICE)Set_background_color(%f,%f,%f) called\n",
	    c[0], c[1], c[2]);
#   endif
    CURDEV->backgroundRGB[0] = c[0];
    CURDEV->backgroundRGB[1] = c[1];
    CURDEV->backgroundRGB[2] = c[2];

    if (CURDEV->visualtype != DcPseudoColor) return;

    DDdevice_InqShadeRange(CURDEV->device,CURDEV->shadeindex,
			   &shademin,&shademax);

    CURDEV->backgroundcolor = shademin+RGBshade(c[0],c[1],c[2])*
	    (shademax-shademin);

#   ifdef DEBUGP
	printf ("(DEVICE)  ... set background index to %d\n",
	    CURDEV->backgroundcolor);
#   endif
}



/* ======================================================================== */

void ddr_gl_dcm_set_foreground_color (
    DtColorModel colormodel,
    DtReal *c)
{
}



/* ======================================================================== */

void ddr_gl_dcm_set_color_entries (
    DtColorModel colormodel,
    int start,
    int n,
    DtReal *c)
{
    int i,i3;

#   ifdef DEBUGP
	printf ("(DEVICE)Setcolorentries(%d..%d) called\n", start, n+start-1);
#   endif

    for (i=0, i3=0; i<n; i++, i3+=3)
	mapcolor (start+i, (short)(c[i3]*255), (short)(c[i3+1]*255),
		           (short)(c[i3+2]*255));
}



/* ======================================================================== */

void ddr_gl_dcm_set_current_view (DtObject view)
{
#   ifdef DEBUGP
	printf ("(DEVICE)Set_current_view() UNIMPLEMENTED\n");
#   endif
}



/* ======================================================================== */

void ddr_gl_dcm_set_depth_buffer_enable (DtFlag flag)
{
#   ifdef DEBUGP
	printf ("(DEVICE)Set_depth_buffer_enable(%d) called\n", flag);
#   endif

    zbuffer (flag);
}



/* ======================================================================== */

void ddr_gl_dcm_set_depth_buffer_write (DtFlag flag)
{
#   ifdef DEBUGP
	printf ("(DEVICE)Set_depth_buffer_write(%d) called\n", flag);
#   endif

    zwritemask (flag ? 0xFFFFFFFFL : 0L);
}



/* ======================================================================== */

void ddr_gl_dcm_set_shade_index (DtInt index)
{
#   ifdef DEBUGP
	printf ("(DEVICE)Set_shade_index(%d) called\n", index);
#   endif

    CURDEV->shadeindex = index;
}
