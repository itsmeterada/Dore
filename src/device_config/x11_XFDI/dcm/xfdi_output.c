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
	void ddr_x11_XFDI_dcm_clear_rectangle_depth (xmin, ymin, xmax, ymax)
	void ddr_x11_XFDI_dcm_clear_rectangle_depth_and_color (xmin, ymin, xmax, ymax)
	void ddr_x11_XFDI_dcm_write_scanline_byte (xstart, ystart, length, data)
	void ddr_x11_XFDI_dcm_blend_texture (xmin, ymin, xmax, ymax, copy_to_main)
	void ddr_x11_XFDI_dcm_blend_transp (xmin, ymin, xmax, ymax, pixels)
	void ddr_x11_XFDI_dcm_clear_rectangle_transp_buffers (xmin, ymin, xmax, ymax)

  ======================================================================
 */
#include "dcm.h"
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_clear_rectangle_depth (xmin, ymin, xmax, ymax)
     DtShort xmin;
     DtShort ymin;
     DtShort xmax;
     DtShort ymax;
{
}
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_clear_rectangle_depth_and_color (xmin, ymin, xmax, ymax)
     DtShort xmin;
     DtShort ymin;
     DtShort xmax;
     DtShort ymax;
{
    XRectangle rect;
    unsigned long pixel_value;
    extern unsigned long ddr_x11_XFDI_dcm_get_pixel_value();

    rect.x = (int)(xmin - (dde_x11_XFDI_current_dcm_data->viewport.x));
    rect.y = (int)(ymin - (dde_x11_XFDI_current_dcm_data->viewport.y));
    rect.width = (int)(xmax - xmin);
    rect.height = (int)(ymax - ymin);

    pixel_value = ddr_x11_XFDI_dcm_get_pixel_value
	    (dde_x11_XFDI_current_dcm_data->background_color);

    XFDIClear (dde_x11_XFDI_current_dcm_data->display,
	       dde_x11_XFDI_current_dcm_data->gc,
	       pixel_value,
	       0,
	       &rect);
    XFDIClearTexture(dde_x11_XFDI_current_dcm_data->display,
		     dde_x11_XFDI_current_dcm_data->gc,
		     pixel_value);
}
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_write_scanline_byte (xstart, ystart, length, data)
     DtInt xstart;
     DtInt ystart;
     DtInt length;
     DtUChar *data;
{
    XdbStereoAttributes sattrs;
    DtUChar scanline[1280*4], *p;
    DtInt windowheight;
    DtInt n;
    int i;
    DtInt index;
    int multibyte = 0;

    p = scanline;

    n = length;

    if (dde_x11_XFDI_current_dcm_data->visualtype == DcDirectColor ||
	dde_x11_XFDI_current_dcm_data->visualtype == DcTrueColor) {
	while (n--) {
	    p++;
	    *p++ = *data++;
	    *p++ = *data++;
	    *p++ = *data++;
	}
    } else {
	ddr_x11_XFDI_dcm_setup_shading();

	if (dde_x11_XFDI_current_dcm_data->ncolors > 256)
		multibyte = 1;

	if (dde_x11_XFDI_current_dcm_data->shade_mode == DcComponent) {
	    for (i=0; i<length; i++) {
		index = ((int)
			 (dde_x11_XFDI_current_dcm_data->cbtselpsweight[0]*data[i*3+0]) &
			 dde_x11_XFDI_current_dcm_data->cbtselpsmask[0]) + 
				 ((int)(dde_x11_XFDI_current_dcm_data->
					cbtselpsweight[1]*data[i*3+1]) &
				  dde_x11_XFDI_current_dcm_data->cbtselpsmask[1]) +
					  ((int)(dde_x11_XFDI_current_dcm_data->
						 cbtselpsweight[2]*data[i*3+2]) &
					   dde_x11_XFDI_current_dcm_data->cbtselpsmask[2]);

		index = index < 0 ? 0 : 
		index > (dde_x11_XFDI_current_dcm_data->ncolors - 1) ?
			(dde_x11_XFDI_current_dcm_data->ncolors - 1) : index;

		if (multibyte) {
		    *p++ = (DtUChar) (index>>8);
		    *p++ = (DtUChar) index;
		} else {
		    *p++ = (DtUChar)index;
		}
	    }
	}
	else if (dde_x11_XFDI_current_dcm_data->shade_mode == DcRange) {
	    for (i=0; i<length; i++) {
		index = dde_x11_XFDI_current_dcm_data->cwtavgpslim[0] +
			(dde_x11_XFDI_current_dcm_data->cwtavgpsweight[0]*data[i*3+0] +
			 dde_x11_XFDI_current_dcm_data->cwtavgpsweight[1]*data[i*3+1] +
			 dde_x11_XFDI_current_dcm_data->cwtavgpsweight[2]*data[i*3+2])*
				 dde_x11_XFDI_current_dcm_data->cwtavgpslim[2]/255.;

		if (index > dde_x11_XFDI_current_dcm_data->cwtavgpslim[1]) 
			index = dde_x11_XFDI_current_dcm_data->cwtavgpslim[1];

		index = index < 0 ? 0 : 
		index > (dde_x11_XFDI_current_dcm_data->ncolors - 1) ?
			(dde_x11_XFDI_current_dcm_data->ncolors - 1) : index;

		if (multibyte) {
		    *p++ = (DtUChar) (index>>8);
		    *p++ = (DtUChar) index;
		} else {
		    *p++ = (DtUChar)index;
		}
	    }
	}
    }

    windowheight = dde_x11_XFDI_current_dcm_data->window_extent.height + 1;

    ystart = windowheight - ystart;

    dde_x11_XFDI_current_dcm_data->image->width = length;
    dde_x11_XFDI_current_dcm_data->image->height = 1;
    dde_x11_XFDI_current_dcm_data->image->data = (char *)scanline;

    XPutImage(dde_x11_XFDI_current_dcm_data->display,
	      dde_x11_XFDI_current_dcm_data->draw_buffer,
	      dde_x11_XFDI_current_dcm_data->xgc,
	      dde_x11_XFDI_current_dcm_data->image,
	      0,0,xstart,ystart,length,1);

    if (dde_x11_XFDI_current_dcm_data->stereoswitch) {
	XPutImage(dde_x11_XFDI_current_dcm_data->display,
		  dde_x11_XFDI_current_dcm_data->draw_buffer_stereo,
		  dde_x11_XFDI_current_dcm_data->xgc,
		  dde_x11_XFDI_current_dcm_data->image,
		  0,0,xstart,ystart,length,1);
    }

    if (!dde_x11_XFDI_current_dcm_data->double_buffered) {
	if (dde_x11_XFDI_current_dcm_data->stereoswitch) {
	    sattrs.eyestate = 1; /* left eye */
	    XdbChangeStereo(dde_x11_XFDI_current_dcm_data->display, 
			    dde_x11_XFDI_current_dcm_data->window,
			    &sattrs, DBStereoEyeState);
	    XdbUpdateWindows(dde_x11_XFDI_current_dcm_data->display,
			     &dde_x11_XFDI_current_dcm_data->draw_buffer,
			     NULL, 1);
      
	    sattrs.eyestate = 2; /* right eye */
	    XdbChangeStereo(dde_x11_XFDI_current_dcm_data->display, 
			    dde_x11_XFDI_current_dcm_data->window,
			    &sattrs, DBStereoEyeState);
	    XdbUpdateWindows(dde_x11_XFDI_current_dcm_data->display,
			     &dde_x11_XFDI_current_dcm_data->draw_buffer_stereo,
			     NULL, 1);
	} else {
	    XdbUpdateWindows(dde_x11_XFDI_current_dcm_data->display,
			     &dde_x11_XFDI_current_dcm_data->draw_buffer,
			     NULL, 1);
	}
	XFlush (dde_x11_XFDI_current_dcm_data->display);
    }
}
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_blend_texture (xmin, ymin, xmax, ymax, copy_to_main)
     DtShort xmin;
     DtShort xmax;
     DtShort ymin;
     DtShort ymax;
     DtFlag copy_to_main;
{
    XRectangle rect;

    rect.x = (int)(xmin - (dde_x11_XFDI_current_dcm_data->viewport.x));
    rect.y = (int)(ymin - (dde_x11_XFDI_current_dcm_data->viewport.y));
    rect.width = (int)(xmax - xmin);
    rect.height = (int)(ymax - ymin);

    XFDITextureBlend(dde_x11_XFDI_current_dcm_data->display,
		     dde_x11_XFDI_current_dcm_data->gc,
		     rect.x, rect.y, rect.width, rect.height);

    if (copy_to_main) {
	XCopyArea(dde_x11_XFDI_current_dcm_data->display,
		  dde_x11_XFDI_current_dcm_data->texture_buffer,
		  dde_x11_XFDI_current_dcm_data->Vdraw_buffer,
		  dde_x11_XFDI_current_dcm_data->xgc,
		  rect.x, rect.y, rect.height, rect.width, 0, 0);
    }
}
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_blend_transp (xmin, ymin, xmax, ymax, pixels)
     DtShort xmin;
     DtShort xmax;
     DtShort ymin;
     DtShort ymax;
     int *pixels;
{
    XRectangle rect;

    if (dde_x11_XFDI_current_dcm_data->multi_pass_transp) {
	rect.x = (int)(xmin - (dde_x11_XFDI_current_dcm_data->viewport.x));
	rect.y = (int)(ymin - (dde_x11_XFDI_current_dcm_data->viewport.y));
	rect.width = (int)(xmax - xmin);
	rect.height = (int)(ymax - ymin);

	XFDITransBlend(dde_x11_XFDI_current_dcm_data->display,
		       dde_x11_XFDI_current_dcm_data->gc,
		       rect.x, rect.y, rect.width, rect.height, pixels);
    } else {
	*pixels = 0;
    }
}
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_clear_rectangle_transp_buffers (xmin, ymin, xmax, ymax)
     DtShort xmin;
     DtShort xmax;
     DtShort ymin;
     DtShort ymax;
{
    XRectangle rect;

    if (dde_x11_XFDI_current_dcm_data->multi_pass_transp) {
	rect.x = (int)(xmin - (dde_x11_XFDI_current_dcm_data->viewport.x));
	rect.y = (int)(ymin - (dde_x11_XFDI_current_dcm_data->viewport.y));
	rect.width = (int)(xmax - xmin);
	rect.height = (int)(ymax - ymin);

	XSetForeground(dde_x11_XFDI_current_dcm_data->display,
		       dde_x11_XFDI_current_dcm_data->xgc, 0xffffffff);
	XFillRectangle(dde_x11_XFDI_current_dcm_data->display,
		       dde_x11_XFDI_current_dcm_data->sort_z_buffer, 
		       dde_x11_XFDI_current_dcm_data->xgc,
		       rect.x, rect.y, rect.width, rect.height);
	XSetForeground(dde_x11_XFDI_current_dcm_data->display,
		       dde_x11_XFDI_current_dcm_data->xgc, 0x0);
	XFillRectangle(dde_x11_XFDI_current_dcm_data->display,
		       dde_x11_XFDI_current_dcm_data->sort_i_buffer, 
		       dde_x11_XFDI_current_dcm_data->xgc,
		       rect.x, rect.y, rect.width, rect.height);
	XFillRectangle(dde_x11_XFDI_current_dcm_data->display,
		       dde_x11_XFDI_current_dcm_data->sort_a_buffer, 
		       dde_x11_XFDI_current_dcm_data->xgc,
		       rect.x, rect.y, rect.width, rect.height);
    }
}
