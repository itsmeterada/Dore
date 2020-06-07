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
	void ddr_x11_titang2_dcm_clear_rectangle_depth (xmin, ymin, xmax, ymax)
	void ddr_x11_titang2_dcm_clear_rectangle_depth_and_color (xmin,ymin,xmax,ymax)
	void ddr_x11_titang2_dcm_write_scanline_byte (xstart, ystart, length, data)
	int ddr_x11_titang2_dcm_blt_front_to_back ()

  ======================================================================
 */
#include "dcm.h"
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_clear_rectangle_depth (xmin, ymin, xmax, ymax)
     DtShort xmin;
     DtShort ymin;
     DtShort xmax;
     DtShort ymax;
{
    tg_clear_z(0,xmin,ymin,xmax-xmin+1,ymax-ymin+1);

    if (doe_system_main_processors > 0) {
	/*  This must happen before parallel code, else we might clear after
	    some other thread has already drawn.  */

	tgi_cswap();
	tg_drain();
    }
}

			     
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_clear_rectangle_depth_and_color (xmin,ymin,xmax,ymax)
     DtShort xmin;
     DtShort xmax;
     DtShort ymin;
     DtShort ymax;
{
    int r, g, b;

#ifdef DEBUG
    printf ("clrrdpclr (%d %d) (%d %d)\n", xmin, ymin, xmax, ymax);
#endif

#ifdef notdef
    tg_ras_sel(RC_SELECT_R);
    tg_reg_write(RC_REG_PX_FUNC, 0);
    tg_reg_write(RC_REG_IMG_MSK, 0xffff);

    if (ExpansionBoard(dde_x11_titang2_current_dcm_data)) {
	tg_ras_sel(RC_SELECT_G);
	tg_reg_write(RC_REG_PX_FUNC, 0);
	tg_reg_write(RC_REG_IMG_MSK, 0xffff);
	tg_ras_sel(RC_SELECT_B);
	tg_reg_write(RC_REG_PX_FUNC, 0);
	tg_reg_write(RC_REG_IMG_MSK, 0xffff);
    }

    if (ExpansionBoard(dde_x11_titang2_current_dcm_data)) {
	tg_ras_sel(RC_SELECT_ALL);
    } else {
	tg_ras_sel(RC_SELECT_R|RC_SELECT_Z);
    }
    tg_set_mod(0);
#endif

    if (dde_x11_titang2_current_dcm_data->visualtype == DcDirectColor) {

	r = 255.* dde_x11_titang2_current_dcm_data->background_color[0];
	g = 255.* dde_x11_titang2_current_dcm_data->background_color[1];
	b = 255.* dde_x11_titang2_current_dcm_data->background_color[2];

	tg_clear_z_color(0,r,g,b,xmin,ymin,xmax-xmin+1,ymax-ymin+1);
    } else {
	tg_clear_z(0,xmin,ymin,xmax-xmin+1,ymax-ymin+1);

	tg_reg_write (RC_REG_F0, 
		      (int)dde_x11_titang2_current_dcm_data->bg_color_index);
	dde_x11_titang2_tigr_state.color_index = 
		(int)dde_x11_titang2_current_dcm_data->bg_color_index;

	tg_rect(xmin,ymin, xmax-xmin+1,ymax-ymin+1);
    }

    if (doe_system_main_processors > 0) {
	/*  This must happen before parallel code, else we might clear after
	    some other thread has already drawn.  */

	tgi_cswap();
	tg_drain();
    }
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_write_scanline_byte (xstart, ystart, length, data)
     DtInt xstart;
     DtInt ystart;
     DtInt length;
     DtUChar *data;
{
    DtUChar scanline[1280*4], *p;
    DtInt windowheight;
    DtInt n;
    int i;
    DtInt index;

    p = scanline;

    n = length;

    if (dde_x11_titang2_current_dcm_data->visualtype == DcDirectColor) {
	while (n--) {
	    p++;
	    *p++ = *data++;
	    *p++ = *data++;
	    *p++ = *data++;
	}
    } else {
	ddr_x11_titang2_dcm_setup_shading();

	if (dde_x11_titang2_current_dcm_data->shade_mode == DcComponent) {
	    for (i=0; i<length; i++) {
		index = ((int)(dde_x11_titang2_current_dcm_data->cbtselpsweight[0]*data[i*3+0]) 
			 & dde_x11_titang2_current_dcm_data->cbtselpsmask[0]) + 
				 ((int)(dde_x11_titang2_current_dcm_data->cbtselpsweight[1]*data[i*3+1]) 
				  & dde_x11_titang2_current_dcm_data->cbtselpsmask[1]) +
					  ((int)(dde_x11_titang2_current_dcm_data->cbtselpsweight[2]*data[i*3+2])
					   & dde_x11_titang2_current_dcm_data->cbtselpsmask[2]);
		index = index < 0 ? 0 : index > 255 ? 255 : index;
		*p++ = (DtUChar)index;
	    }
	} else if (dde_x11_titang2_current_dcm_data->shade_mode == DcRange) {
	    for (i=0; i<length; i++) {
		index = dde_x11_titang2_current_dcm_data->cwtavgpslim[0] +
			(dde_x11_titang2_current_dcm_data->cwtavgpsweight[0]*data[i*3+0] +
			 dde_x11_titang2_current_dcm_data->cwtavgpsweight[1]*data[i*3+1] +
			 dde_x11_titang2_current_dcm_data->cwtavgpsweight[2]*data[i*3+2])*
				 dde_x11_titang2_current_dcm_data->cwtavgpslim[2];
		if (index > dde_x11_titang2_current_dcm_data->cwtavgpslim[1]) 
			index = dde_x11_titang2_current_dcm_data->cwtavgpslim[1];
		index = index < 0 ? 0 : index > 255 ? 255 : index;
		*p++ = (DtUChar)index;
	    }
	}
    }

    windowheight = XExtent(dde_x11_titang2_current_dcm_data).y2 - 
	    XExtent(dde_x11_titang2_current_dcm_data).y1 + 1;

    ystart = windowheight - ystart;

    XIMAGE(dde_x11_titang2_current_dcm_data)->width = length;
    XIMAGE(dde_x11_titang2_current_dcm_data)->height = 1;
    XIMAGE(dde_x11_titang2_current_dcm_data)->data = (char *)scanline;

    XPutImage(XDISPLAY(dde_x11_titang2_current_dcm_data),
	      XWINDOW(dde_x11_titang2_current_dcm_data),
	      XGC(dde_x11_titang2_current_dcm_data),
	      XIMAGE(dde_x11_titang2_current_dcm_data),
	      0,0,xstart,ystart,length,
	      1);

    if (dde_x11_titang2_current_dcm_data->stereoswitch) {
	/* draw the right eye too */
	XBSetDrawBuffer(XDISPLAY(dde_x11_titang2_current_dcm_data),
			XWINDOW(dde_x11_titang2_current_dcm_data),
			XDrawBuffer(dde_x11_titang2_current_dcm_data),
			XBRightEye);
	XPutImage(XDISPLAY(dde_x11_titang2_current_dcm_data),
		  XWINDOW(dde_x11_titang2_current_dcm_data),
		  XGC(dde_x11_titang2_current_dcm_data),
		  XIMAGE(dde_x11_titang2_current_dcm_data),
		  0,0,xstart,ystart,length,
		  1);
	XBSetDrawBuffer(XDISPLAY(dde_x11_titang2_current_dcm_data),
			XWINDOW(dde_x11_titang2_current_dcm_data),
			XDrawBuffer(dde_x11_titang2_current_dcm_data),
			XBLeftEye);
    }
}
/*
 ======================================================================
 */

ddr_x11_titang2_dcm_blt_front_to_back()
{
    int w, h;
    int xfront, yfront;
    int xback, yback;
    int i;
    DtReal *p;


#ifdef DEBUG
    printf ("rectangles = %d\n", XNRECTS(dde_x11_titang2_current_dcm_data));
    printf ("XDrawBuffer = %d\n", XDrawBuffer(dde_x11_titang2_current_dcm_data));
#endif

    if (dde_x11_titang2_current_dcm_data->stereoscreen) {
	if (dde_x11_titang2_current_dcm_data->stereoswitch) {
#ifdef DEBUG
	    printf ("stereo screen / stereo window\n");
#endif
	    for (i=0, p = dde_x11_titang2_current_dcm_data->clearClipRects; 
		 i<XNRECTS(dde_x11_titang2_current_dcm_data);
		 i++, p += 4) {
		w = (int)(p[2] - p[0] + 1);
		h = (int)(p[3] - p[1] + 1);
		xfront = (int)p[0];
		yfront = (int)p[1];
		xback = (int)p[0];
		yback = (int)(p[1] + 512);
		if (XDrawBuffer(dde_x11_titang2_current_dcm_data)) {
#ifdef DEBUG
		    printf ("(%d %d) (%d %d) -> (%d %d)\n", xfront, yfront,
			    w, h, xback, yback);
#endif
		    tg_blt(0, xfront, yfront, w, h, xback, yback);
		} else {
#ifdef DEBUG
		    printf ("(%d %d) (%d %d) -> (%d %d)\n", xback, yback,
			    w, h, xfront, yfront);
#endif
		    tg_blt(0, xback, yback, w, h, xfront, yfront);
		}

		xfront = (int)(p[0] + 2048);
		yfront = (int)p[1];
		xback = (int)(p[0] + 2048);
		yback = (int)(p[1] + 512);
		if (XDrawBuffer(dde_x11_titang2_current_dcm_data)) {
#ifdef DEBUG
		    printf ("(%d %d) (%d %d) -> (%d %d)\n", xfront, yfront,
			    w, h, xback, yback);
#endif
		    tg_blt(0, xfront, yfront, w, h, xback, yback);
		} else {
#ifdef DEBUG
		    printf ("(%d %d) (%d %d) -> (%d %d)\n", xback, yback,
			    w, h, xfront, yfront);
#endif
		    tg_blt(0, xback, yback, w, h, xfront, yfront);
		}
	    }
	} else			/* !dde_x11_titang2_current_dcm_data->stereoswitch */ {
#ifdef DEBUG
	    printf ("stereo screen / NO stereo window\n");
#endif
	    for (i=0, p = dde_x11_titang2_current_dcm_data->clearClipRects; 
		 i<XNRECTS(dde_x11_titang2_current_dcm_data);
		 i++, p += 4) {
		w = (int)(p[2] - p[0] + 1);
		h = (int)(p[3] - p[1] + 1);
		xfront = (int)p[0];
		yfront = (int)p[1];
		xback = (int)p[0];
		yback = (int)(p[1] + 512);
		if (XDrawBuffer(dde_x11_titang2_current_dcm_data)) {
#ifdef DEBUG
		    printf ("(%d %d) (%d %d) -> (%d %d)\n", xfront, yfront,
			    w, h, xback, yback);
#endif
		    tg_blt(0, xfront, yfront, w, h, xback, yback);
		} else {
#ifdef DEBUG
		    printf ("(%d %d) (%d %d) -> (%d %d)\n", xback, yback,
			    w, h, xfront, yfront);
#endif
		    tg_blt(0, xback, yback, w, h, xfront, yfront);
		}
	    }
	}
    } else			/* !dde_x11_titang2_current_dcm_data->stereoscreen */ {
#ifdef DEBUG
	printf ("standard  screen / standard window\n");
#endif
	for (i=0, p = dde_x11_titang2_current_dcm_data->clearClipRects; 
	     i<XNRECTS(dde_x11_titang2_current_dcm_data);
	     i++, p += 4) {
	    w = (int)(p[2] - p[0] + 1);
	    h = (int)(p[3] - p[1] + 1);
	    xfront = (int)p[0];
	    yfront = (int)p[1];
	    xback = (int)(p[0] + 2048);
	    yback = (int)p[1];
	    if (XDrawBuffer(dde_x11_titang2_current_dcm_data)) {
#ifdef DEBUG
		printf ("(%d %d) (%d %d) -> (%d %d)\n", xfront, yfront,
			w, h, xback, yback);
#endif
		tg_blt(0, xfront, yfront, w, h, xback, yback);
	    } else {
#ifdef DEBUG
		printf ("(%d %d) (%d %d) -> (%d %d)\n", xback, yback,
			w, h, xfront, yfront);
#endif
		tg_blt(0, xback, yback, w, h, xfront, yfront);
	    }
	}
    }
    if (doe_system_main_processors > 0) {
	extern int _taskno;

	for (_taskno=0;_taskno<doe_system_main_processors;_taskno++)	{
	    /*  Make sure all drawing commands are sent to the command list.  */

	    tgi_cswap();	/*tg_flush();*/

	    /*  Wait for the kernel to process all of the cmds for this thread.  */

	    tg_drain();
	}
	_taskno = 0;
    } else {
	tgi_cswap();

	tg_drain();
    }
}
