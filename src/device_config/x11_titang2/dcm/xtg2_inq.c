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
	void ddr_x11_titang2_dcm_inquire_color_entries (colormodel,start, n, c)
	void ddr_x11_titang2_dcm_inquire_auto_size (flag)
	void ddr_x11_titang2_dcm_inquire_clip_list (nrects, clip_list, extent, partiallyobscuredflag)
	void ddr_x11_titang2_dcm_inquire_clear_list (nrects, clear_list)
	void ddr_x11_titang2_dcm_inquire_device_extent (volume)
	int ddr_x11_titang2_dcm_inquire_draw_buffer (draw_buffer)
	int ddr_x11_titang2_dcm_inquire_expansion_board (expansion_board)
	void ddr_x11_titang2_dcm_inquire_ncolors (ncolors)
	DtFlag ddr_x11_titang2_dcm_inquire_pixel_data (requesttype, width, height, type, data, userdelete)
	void ddr_x11_titang2_dcm_inquire_resolution (xres, yres)
	int ddr_x11_titang2_dcm_inquire_stereoscreen (stereoscreenswitch)
	void ddr_x11_titang2_dcm_inquire_stereo (stereoswitch)
	void ddr_x11_titang2_dcm_inquire_visual_type (visualtype)

  ======================================================================
 */
#include "dcm.h"
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_inquire_color_entries (colormodel,start, n, c)
     DtColorModel colormodel;
     int start;
     int n;
     DtReal *c;
{
    XColor colors[256];
    Colormap cmap ;
    int i, j;
    DtReal r;

    XGetWindowAttributes(XDISPLAY(dde_x11_titang2_current_dcm_data),
			 XWINDOW(dde_x11_titang2_current_dcm_data),
			 &XWINDOWATTRIBUTES(dde_x11_titang2_current_dcm_data));

    cmap = XWINDOWATTRIBUTES(dde_x11_titang2_current_dcm_data).colormap;

    for (i=0; i<n; i++) {
	j = start+i;
	if (dde_x11_titang2_current_dcm_data->visualtype == DcDirectColor)
		colors[i].pixel = (j<<16)|(j<<8)|j;
	else colors[i].pixel = j;
    }
    XQueryColors(XDISPLAY(dde_x11_titang2_current_dcm_data), 
		 cmap, colors, n);
		
    for (i=0; i<n; i++) {
	if ((r = (DtReal)colors[i].red / 65535.) < 0.0) 
		r = 0.0; if (r > 1.0) r = 1.0;
	c[i*3] = r;
	if ((r = (DtReal)colors[i].green / 65535.) < 0.0) 
		r = 0.0; if (r > 1.0) r = 1.0;
	c[i*3+1] = r;
	if ((r = (DtReal)colors[i].blue / 65535.) < 0.0) 
		r = 0.0; if (r > 1.0) r = 1.0;
	c[i*3+2] = r;
    }
}

/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_inquire_auto_size (flag)
     DtFlag *flag;
{
    *flag = dde_x11_titang2_current_dcm_data->auto_size;
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_inquire_clip_list (nrects, clip_list,
					 extent, partiallyobscuredflag)
     DtUInt *nrects;
     DtReal **clip_list;
     DtReal extent[4];
     DtFlag *partiallyobscuredflag;
{
   *nrects = XNRECTS(dde_x11_titang2_current_dcm_data);
   *clip_list = XClipRects(dde_x11_titang2_current_dcm_data);

   extent[0] = XExtent(dde_x11_titang2_current_dcm_data).x1;
   extent[1] = XExtent(dde_x11_titang2_current_dcm_data).y1;
   extent[2] = XExtent(dde_x11_titang2_current_dcm_data).x2;
   extent[3] = XExtent(dde_x11_titang2_current_dcm_data).y2;

   *partiallyobscuredflag = 
	 dde_x11_titang2_current_dcm_data->partially_obscure;

#ifdef DEBUG
	printf("inquire clip list:\n");
	printf("boxno = %d\n",XNRECTS(dde_x11_titang2_current_dcm_data));
	printf("sequenceno = %d\n",
	       XSEQUENCENO(dde_x11_titang2_current_dcm_data));
	printf("first clipping rectangle:  %d %d %d %d\n",
		(DtInt)((*clip_list)[0]),
		(DtInt)((*clip_list)[1]),
		(DtInt)((*clip_list)[2]),
		(DtInt)((*clip_list)[3]));
	printf("extent = %d %d %d %d\n",
		XExtent(dde_x11_titang2_current_dcm_data).x1,
		XExtent(dde_x11_titang2_current_dcm_data).y1,
		XExtent(dde_x11_titang2_current_dcm_data).x2,
		XExtent(dde_x11_titang2_current_dcm_data).y2);
	printf("partiallyobscuredflag = %d\n",*partiallyobscuredflag);
#endif
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_inquire_clear_list(nrects, clear_list)
     DtUInt *nrects;
     DtReal **clear_list;
{
    *nrects = XNRECTS(dde_x11_titang2_current_dcm_data);
    *clear_list = dde_x11_titang2_current_dcm_data->clearClipRects;
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_inquire_device_extent (volume)
     DtVolume *volume;
{
    volume->bll[0] = XExtent(dde_x11_titang2_current_dcm_data).x1;
    volume->bll[1] = 1024-XExtent(dde_x11_titang2_current_dcm_data).y2;
    volume->bll[2] = 0.;

    volume->fur[0] = XExtent(dde_x11_titang2_current_dcm_data).x2;
    volume->fur[1] = 1024-XExtent(dde_x11_titang2_current_dcm_data).y1;
    volume->fur[2] = 65535.;
}

/*
 ======================================================================
 */

ddr_x11_titang2_dcm_inquire_draw_buffer(draw_buffer)
     int *draw_buffer;

{
    *draw_buffer = XDrawBuffer(dde_x11_titang2_current_dcm_data);
}

/*
 ======================================================================
 */

ddr_x11_titang2_dcm_inquire_expansion_board(expansion_board)
     int *expansion_board;

{
    *expansion_board = ExpansionBoard(dde_x11_titang2_current_dcm_data);
}

/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_inquire_ncolors (ncolors)
     DtInt *ncolors;
{
    *ncolors = 256;
}
/*
 ======================================================================
 */

DtFlag
ddr_x11_titang2_dcm_inquire_pixel_data (requesttype, width, height, type, 
				   data, userdelete)
     DtRasterType requesttype;
     DtInt *width;
     DtInt *height;
     DtRasterType *type;
     DtPtr *data;
     DtFlag *userdelete;
{
   return (DcFalse);
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_inquire_resolution (xres, yres)
     DtReal *xres;
     DtReal *yres;
{
    if (dde_x11_titang2_current_dcm_data->stereoscreen) {
	*xres = .28125;
	*yres = .56250;
    } else {
	*xres = .28125;
	*yres = .28125;
    }      
}
/*
 ======================================================================
 */

ddr_x11_titang2_dcm_inquire_stereoscreen (stereoscreenswitch)
     DtSwitch *stereoscreenswitch;
{
    *stereoscreenswitch = dde_x11_titang2_current_dcm_data->stereoscreen;
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_inquire_stereo (stereoswitch)
     DtFlag *stereoswitch;
{
    *stereoswitch = dde_x11_titang2_current_dcm_data->stereoswitch ? DcTrue : DcFalse;
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_inquire_visual_type (visualtype)
     DtVisualType *visualtype;
{
    *visualtype = dde_x11_titang2_current_dcm_data->visualtype;
}
