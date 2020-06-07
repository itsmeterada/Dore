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
	void ddr_x11_DGL_dcm_clear_rectangle_depth (xmin, ymin, xmax, ymax)
	void ddr_x11_DGL_dcm_clear_rectangle_depth_and_color (xmin,ymin,xmax,ymax)
	void ddr_x11_DGL_dcm_write_scanline_byte (xstart, ystart, length, data)
	int ddr_x11_DGL_dcm_blt_front_to_back ()

  ======================================================================
 */
#include "dcm.h"
/*
 ======================================================================
 */
void
ddr_x11_DGL_dcm_clear_rectangle_depth (xmin, ymin, xmax, ymax)
     DtShort xmin;
     DtShort ymin;
     DtShort xmax;
     DtShort ymax;
{
    XRectangle rect;
    ddt_x11_DGL_data *device_data = dde_x11_DGL_current_dcm_data;

    rect.x = xmin;
    rect.y = ymin;
    rect.width = xmax - xmin + 1;
    rect.height = ymax - ymin + 1;
   
    XdSetFillMode(device_data->pGC, XdFillZ);
    XdSetZValue(device_data->pGC, (XdReal)0.);
    XdFillRectangles (device_data->pDraw, device_data->pGC, &rect, 1);

    if (doe_system_main_processors > 0) {
	/*  This must happen before parallel code, else we might clear after
	    some other thread has already drawn.  */

	XdFlush(XpDRAW(device_data));
	XdSynchronize();
    }
}
/*
 ======================================================================
 */

void
ddr_x11_DGL_dcm_clear_rectangle_depth_and_color (xmin,ymin,xmax,ymax)
     DtShort xmin;
     DtShort xmax;
     DtShort ymin;
     DtShort ymax;
{
    unsigned long color;
    XRectangle rect;
    ddt_x11_DGL_data *device_data = dde_x11_DGL_current_dcm_data;

    rect.x = xmin;
    rect.y = ymin;
    rect.width = xmax - xmin + 1;
    rect.height = ymax - ymin + 1;
   
#ifdef DEBUG
    printf ("clrrdpclr (%d %d) (%d %d)\n", xmin, ymin, xmax, ymax);
#endif

    if (device_data->visualtype == DcDirectColor) {
	color = (((int)(255.* device_data->background_color[0]) << 16) +
		 ((int)(255.* device_data->background_color[1]) << 8) +
		 ((int)(255.* device_data->background_color[2])));
    } else {
	color = device_data->bg_color_index;
    }

    XdSetFillMode(device_data->pGC, XdFillZColor);
    XdSetForeground (device_data->pGC, color);
    XdSetZValue(device_data->pGC, (XdReal)0.);
    XdFillRectangles (device_data->pDraw, device_data->pGC, &rect, 1);

    if (doe_system_main_processors > 0) {
	/*  This must happen before parallel code, else we might clear after
	    some other thread has already drawn.  */

	XdFlush(XpDRAW(device_data));
	XdSynchronize();
    }
}
/*
 ======================================================================
 */

void
ddr_x11_DGL_dcm_write_scanline_byte (xstart, ystart, length, data)
     DtInt xstart;
     DtInt ystart;
     DtInt length;
     DtUChar *data;
{
    unsigned char scanline[1280*4], *p;
    DtInt windowheight;
    DtInt n;
    int i;
    DtInt index;

    p = scanline;

    n = length;

    if (dde_x11_DGL_current_dcm_data->visualtype == DcDirectColor) {
	while (n--) {
	    *p++ = *data++;
	    *p++ = *data++;
	    *p++ = *data++;
	}
    } else {
	ddr_x11_DGL_dcm_setup_shading();

	if (dde_x11_DGL_current_dcm_data->shade_mode == DcComponent) {
	    for (i=0; i<length; i++) {
		index = ((int)(dde_x11_DGL_current_dcm_data->cbtselpsweight[0]*data[i*3+0]) 
			 & dde_x11_DGL_current_dcm_data->cbtselpsmask[0]) + 
				 ((int)(dde_x11_DGL_current_dcm_data->cbtselpsweight[1]*data[i*3+1]) 
				  & dde_x11_DGL_current_dcm_data->cbtselpsmask[1]) +
					  ((int)(dde_x11_DGL_current_dcm_data->cbtselpsweight[2]*data[i*3+2])
					   & dde_x11_DGL_current_dcm_data->cbtselpsmask[2]);
		index = index < 0 ? 0 : index > 255 ? 255 : index;
		*p++ = (unsigned char)index;
	    }
	} else if (dde_x11_DGL_current_dcm_data->shade_mode == DcRange) {
	    for (i=0; i<length; i++) {
		index = dde_x11_DGL_current_dcm_data->cwtavgpslim[0] +
			(dde_x11_DGL_current_dcm_data->cwtavgpsweight[0]*data[i*3+0] +
			 dde_x11_DGL_current_dcm_data->cwtavgpsweight[1]*data[i*3+1] +
			 dde_x11_DGL_current_dcm_data->cwtavgpsweight[2]*data[i*3+2])*
				 dde_x11_DGL_current_dcm_data->cwtavgpslim[2];
		if (index > dde_x11_DGL_current_dcm_data->cwtavgpslim[1]) 
			index = dde_x11_DGL_current_dcm_data->cwtavgpslim[1];
		index = index < 0 ? 0 : index > 255 ? 255 : index;
		*p++ = (unsigned char)index;
	    }
	}
    }

    windowheight = XExtent(dde_x11_DGL_current_dcm_data).y2 - 
	    XExtent(dde_x11_DGL_current_dcm_data).y1 + 1;

    ystart = windowheight - ystart;

    XdPutImage(XpDRAW(dde_x11_DGL_current_dcm_data),
	       XpGC(dde_x11_DGL_current_dcm_data),
	       0, 0, (int)xstart, (int)ystart, (int)length, 
	       1, 1280, scanline, 
	       (dde_x11_DGL_current_dcm_data->visualtype == DcDirectColor)
	       ? XdRGB : XdRedBank); 

    if (dde_x11_DGL_current_dcm_data->stereoswitch) {
	/* draw the right eye too */
	XdSetDrawBuffer(XpDRAW(dde_x11_DGL_current_dcm_data),
			XDrawBuffer(dde_x11_DGL_current_dcm_data),
			XBRightEye);
	XdPutImage(XpDRAW(dde_x11_DGL_current_dcm_data),
		   XpGC(dde_x11_DGL_current_dcm_data),
		   0, 0, (int)xstart, (int)ystart, (int)length, 
		   1, 1280, scanline, 
		   (dde_x11_DGL_current_dcm_data->visualtype == DcDirectColor)
		   ? XdRGB : XdRedBank); 
	XdSetDrawBuffer(XpDRAW(dde_x11_DGL_current_dcm_data),
			XDrawBuffer(dde_x11_DGL_current_dcm_data),
			XBLeftEye);
    }
    if (!XDoubleBuffered(dde_x11_DGL_current_dcm_data)) {
	XdFlush(XpDRAW(dde_x11_DGL_current_dcm_data));
    }
}
/*
 ======================================================================
 */

ddr_x11_DGL_dcm_blt_front_to_back()
{
    int w, h;

#ifdef DEBUG
    printf ("XDrawBuffer = %d\n", XDrawBuffer(dde_x11_DGL_current_dcm_data));
#endif

    w = (XExtent(dde_x11_DGL_current_dcm_data).x2 -
	 XExtent(dde_x11_DGL_current_dcm_data).x1) + 1;
    h = (XExtent(dde_x11_DGL_current_dcm_data).y2 -
	 XExtent(dde_x11_DGL_current_dcm_data).y1) + 1;

    XdSetViewport(XpGC(dde_x11_DGL_current_dcm_data),
		  XExtent(dde_x11_DGL_current_dcm_data).x1,
		  XExtent(dde_x11_DGL_current_dcm_data).y1,
		  XExtent(dde_x11_DGL_current_dcm_data).x2,
		  XExtent(dde_x11_DGL_current_dcm_data).y2);

    XdCopyArea(XpDRAW(dde_x11_DGL_current_dcm_data),
	       XDisplayBuffer(dde_x11_DGL_current_dcm_data),
	       XBLeftEye, 
	       XpDRAW(dde_x11_DGL_current_dcm_data),
	       XDrawBuffer(dde_x11_DGL_current_dcm_data),
	       XBLeftEye, 
	       XpGC(dde_x11_DGL_current_dcm_data),
	       XExtent(dde_x11_DGL_current_dcm_data).x1,
	       XExtent(dde_x11_DGL_current_dcm_data).y1,
	       w, h, 
	       XExtent(dde_x11_DGL_current_dcm_data).x1,
	       XExtent(dde_x11_DGL_current_dcm_data).y1);

    if (dde_x11_DGL_current_dcm_data->stereoswitch) {
	XdCopyArea(XpDRAW(dde_x11_DGL_current_dcm_data),
		   XDisplayBuffer(dde_x11_DGL_current_dcm_data),
		   XBRightEye, 
		   XpDRAW(dde_x11_DGL_current_dcm_data),
		   XDrawBuffer(dde_x11_DGL_current_dcm_data),
		   XBRightEye, 
		   XpGC(dde_x11_DGL_current_dcm_data),
		   XExtent(dde_x11_DGL_current_dcm_data).x1,
		   XExtent(dde_x11_DGL_current_dcm_data).y1,
		   w, h, 
		   XExtent(dde_x11_DGL_current_dcm_data).x1,
		   XExtent(dde_x11_DGL_current_dcm_data).y1);
    }

    /*  Make sure all drawing commands are sent to the command list.  */
    /*  Wait for the kernel to process all of the cmds.  */

    XdFlush(XpDRAW(dde_x11_DGL_current_dcm_data));
    XdSynchronize();
}
