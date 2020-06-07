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
**  This file contains the DCM routines that the renderer uses to set
**  different DCM properties (which govern the viewport).
*****************************************************************************/

#include "../pex_driver.h"

#define DEBUG 0


/*****************************************************************************
**  This routine sets the background color of the device.  If `colormodel' is
**  DcRGB, then color[0] is the red component, color[1] is the green component
**  and color[2] is the blue component.  All values are in the range of [0,1].
*****************************************************************************/

void  ddr_pex_dcm_set_background_color  (
    DtColorModel colormodel,
    DtReal       color[])
{
    auto XColor    xcolor;		/* X Color Specifier */
    auto XGCValues xgcvals;

#   if DEBUG_CALL
	printf ("ddr_pex_dcm_set_background_color (%d, %lx)\n",
	    colormodel, color);
	fflush(stdout);
#   endif

    if (colormodel != DcRGB) return;

    DCM->bg_color[0] = color[0];
    DCM->bg_color[1] = color[1];
    DCM->bg_color[2] = color[2];

#if 0

    xcolor.red   = 65535 * color[0];
    xcolor.green = 65535 * color[1];
    xcolor.blue  = 65535 * color[2];
    xcolor.flags = DoRed | DoGreen | DoBlue;

    /* Allocate the new background color. */

    if (DCM->visualtype == DcPseudoColor)
    {   if (DdInqShadeMode(DCM->device) == DcComponent)
	{   xcolor.pixel = ((xcolor.red & 0xE000) >> 8)
		         | ((xcolor.green & 0xE000) >> 11)
		         | ((xcolor.blue & 0xC000) >> 14);
	}
	else
	{   auto DtInt shademin, shademax;	/* Shade Range Bounds */

	    DDdevice_InqShadeRange
		(DCM->device, DCM->shadeindex, &shademin, &shademax);

	    xcolor.pixel = shademin
			 + (shademax-shademin)
			   * LUMINANCE (color[0],color[1],color[2]);
	}
    }
    else /* DCM->visualtype == DcTrueColor */
    {   if (!XAllocColor (DCM->display, DCM->window_attrs.colormap, &xcolor))
	    return;
    }

    /* Now set the graphics context.  This value will be used whenever
    ** XFillRectangle() is called on the pixmap.  */

    xgcvals.foreground = xcolor.pixel;
    XChangeGC (DCM->display, DCM->gc, GCForeground, &xgcvals);

    /* Now clear the pixmap to the background color. */

    XFillRectangle
	(DCM->display, DCM->pixmap, DCM->gc, 0, 0, DCM->width, DCM->height);
#endif
}



/*****************************************************************************
**  This routine sets the foreground color of the device.  If `colormodel' is
**  DcRGB, then color[0] is the red component, color[1] is the green component
**  and color[2] is the blue component.  All values are in the range of [0,1].
*****************************************************************************/

void  ddr_pex_dcm_set_foreground_color  (
    DtColorModel colormodel,
    DtReal       color[])
{
#   if DEBUG_CALL
	printf ("ddr_pex_dcm_set_foreground_color (%d, %lx)\n",
	    colormodel, color);
	fflush(stdout);
#   endif
}



/*****************************************************************************
**  This routine sets `count' color entries in the device color table
**  beginning with entry `start'.  The parameter `colormodel' indicates
**  which color model to use.  If the color model is DcRGB, then the length of
**  `colors' is 3 * `count' and the values returned in `colors' should be in
**  the range of [0,1].  In this case, the entries colors[3*i+0],
**  colors[3*i+1], and colors[3*i+2] are the red, green and blue values of the
**  ith color starting from `start'.
*****************************************************************************/

void  ddr_pex_dcm_set_color_entries  (
    DtColorModel colormodel,	/* Color Model Use for Color Entries */
    DtInt        start,		/* Start Color Entry Index */
    DtInt        count,		/* Number of Color Entries to Set */
    DtReal       colors[])	/* Array of Color Values */
{
    register DtReal       *cptr;		/* Color Coordinate Pointer */
    register int           ii;			/* Loop Indices */
    static   XColor        xcolors[256];	/* X Color Entries */

#   if DEBUG_CALL
	printf ("ddr_pex_dcm_set_color_entries (%d, %d, %d, %lx)\n",
	    colormodel, start, count, colors);
	fflush(stdout);
#   endif

    if ((colormodel != DcRGB) || (DCM->visualtype != DcPseudoColor))
        return;

    if (start < 0) start = 0;

    if ((start + count) > DCM->ncolors)
    {   DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_pex_dcm_set_color_entries",
                 "(start + count) is outside colormap size");
        count = DCM->ncolors - start;
    }
    if (count < 0) 
    {   DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_pex_dcm_set_color_entries",
                 "Count is less than zero");
        return;
    }

    cptr = colors;

    for (ii=start;  ii < (start + count);  ++ii)
    {
	auto int red, green, blue;	/* Color Values */

	red   = *cptr++ * 65535;
	green = *cptr++ * 65535;
	blue  = *cptr++ * 65535;

	xcolors[ii].red   = CLAMP (red,   0, 65535);
	xcolors[ii].green = CLAMP (green, 0, 65535);
	xcolors[ii].blue  = CLAMP (blue,  0, 65535);
	xcolors[ii].flags = DoRed | DoGreen | DoBlue;
	xcolors[ii].pixel = ii;
    }

    XStoreColors (DCM->display, DCM->colormap, xcolors, count);
}



/*****************************************************************************
**  This procedure notifies the device driver when an update is to occur that
**  affects one view.
*****************************************************************************/

void  ddr_pex_dcm_set_current_view  (DtObject view)
{
#   if DEBUG_CALL
	printf ("ddr_pex_dcm_set_current_view (%lx)\n", view);
	fflush(stdout);
#   endif
}



/*****************************************************************************
*****************************************************************************/

void  ddr_pex_dcm_set_depth_buffer_enable  (DtFlag flag)
{
#   if DEBUG_CALL
	printf ("ddr_pex_dcm_set_depth_buffer_enable (%d)\n", flag);
	fflush(stdout);
#   endif
}



/*****************************************************************************
*****************************************************************************/

void  ddr_pex_dcm_set_depth_buffer_write  (DtFlag flag)
{
#   if DEBUG_CALL
	printf ("ddr_pex_dcm_set_depth_buffer_write (%d)\n", flag);
	fflush(stdout);
#   endif
}



/*****************************************************************************
**  This procedure sets the pseudo-color shade range index.  This value is
**  used to select a particular range of color shades for subsequent
**  rendering, and for the selected background color.
*****************************************************************************/

void  ddr_pex_dcm_set_shade_index  (DtInt index)
{
#   if DEBUG_CALL
	printf ("ddr_pex_dcm_set_shade_index (%d)\n", index);
	fflush(stdout);
#   endif
    
    DCM->shadeindex = index;
    ddr_pex_drom_update_shade_index ();
}



/*****************************************************************************
*****************************************************************************/

void  ddr_pex_dcm_set_viewport  (
    DtReal Xmin,
    DtReal Ymin,
    DtReal Xmax,
    DtReal Ymax)
{
#   if DEBUG_CALL
	printf ("ddr_pex_dcm_set_viewport (%g, %g, %g, %g)\n",
	    Xmin, Xmax, Ymin, Ymax);
	fflush(stdout);
#   endif
}
