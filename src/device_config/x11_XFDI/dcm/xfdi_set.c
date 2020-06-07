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
	void ddr_x11_XFDI_dcm_set_background_color (colormodel, c)
	void ddr_x11_XFDI_dcm_set_foreground_color (colormodel, c)
	void ddr_x11_XFDI_dcm_set_color_entries (colormodel, start, n, c)
	int ddr_x11_XFDI_dcm_set_stereo_eye (eye)
	void ddr_x11_XFDI_dcm_set_current_view (view)
	void ddr_x11_XFDI_dcm_set_depth_buffer_enable (flag)
	void ddr_x11_XFDI_dcm_set_depth_buffer_write (flag)
	void ddr_x11_XFDI_dcm_set_shade_index (index)
	ddr_x11_XFDI_dcm_set_viewport ddr_x11_XFDI_dcm_set_viewport (xmin, ymin, xmax, ymax)

  ======================================================================
 */
#include "dcm.h"
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_set_background_color (colormodel,c)
     DtColorModel colormodel;
     DtReal *c;
{
    DtInt i;
    DtInt index;

    for (i=0; i<3; i++) 
	    dde_x11_XFDI_current_dcm_data->background_color[i] = c[i];

    if (dde_x11_XFDI_current_dcm_data->visualtype == DcPseudoColor) {
	ddr_x11_XFDI_dcm_setup_shading();

	if (dde_x11_XFDI_current_dcm_data->shade_mode == DcComponent) {
	    index = 
		    ((int)(dde_x11_XFDI_current_dcm_data->cbtselpsweight[0]*c[0]*255.) &
		     dde_x11_XFDI_current_dcm_data->cbtselpsmask[0]) + 
			     ((int)(dde_x11_XFDI_current_dcm_data->cbtselpsweight[1]*c[1]*255.) &
			      dde_x11_XFDI_current_dcm_data->cbtselpsmask[1]) +
				      ((int)(dde_x11_XFDI_current_dcm_data->cbtselpsweight[2]*c[2]*255.) &
				       dde_x11_XFDI_current_dcm_data->cbtselpsmask[2]);

	    index = index < 0 ? 0 : 
	    index > (dde_x11_XFDI_current_dcm_data->ncolors - 1) ?
		    (dde_x11_XFDI_current_dcm_data->ncolors - 1) : index;
	}
	else if (dde_x11_XFDI_current_dcm_data->shade_mode == DcRange) {
	    index = dde_x11_XFDI_current_dcm_data->cwtavgpslim[0] +
		    (dde_x11_XFDI_current_dcm_data->cwtavgpsweight[0]*c[0]*
		     (dde_x11_XFDI_current_dcm_data->ncolors - 1) +
		     dde_x11_XFDI_current_dcm_data->cwtavgpsweight[1]*c[1]*
		     (dde_x11_XFDI_current_dcm_data->ncolors - 1) +
		     dde_x11_XFDI_current_dcm_data->cwtavgpsweight[2]*c[2]*
		     (dde_x11_XFDI_current_dcm_data->ncolors -1))*
			     dde_x11_XFDI_current_dcm_data->cwtavgpslim[2];

	    if (index > dde_x11_XFDI_current_dcm_data->cwtavgpslim[1]) 
		    index = dde_x11_XFDI_current_dcm_data->cwtavgpslim[1];

	    index = index < 0 ? 0 : 
	    index > (dde_x11_XFDI_current_dcm_data->ncolors - 1) ?
		    (dde_x11_XFDI_current_dcm_data->ncolors - 1) : index;
	}

	dde_x11_XFDI_current_dcm_data->bg_color_index = (short)index;
    }
}
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_set_foreground_color (colormodel,c)
     DtColorModel colormodel;
     DtReal *c;
{
}
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_set_color_entries (colormodel,start, n, c)
     DtColorModel colormodel;
     int start;
     int n;
     DtReal *c;
{
    XColor colors[4096];
    Colormap cmap ;
    int i, j;
    DtReal r;

    XGetWindowAttributes(dde_x11_XFDI_current_dcm_data->display,
			 dde_x11_XFDI_current_dcm_data->window,
			 &dde_x11_XFDI_current_dcm_data->window_attributes);

    cmap = dde_x11_XFDI_current_dcm_data->window_attributes.colormap;

    if (start + n > dde_x11_XFDI_current_dcm_data->ncolors) {
	DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_x11_XFDI_dcm_set_color_entries",
		    "(start + count) is outside colormap size");
	n = dde_x11_XFDI_current_dcm_data->ncolors - start;
    } 
    if (n < 0) {
	DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_x11_XFDI_dcm_set_color_entries",
		    "count less than zero");
	return;
    }

    for (i=0; i<n; i++) {
	if (65535 < (r = 65535*c[i*3])) r = 65535; if (0> r) r = 0;
	colors[i].red = r;
	if (65535 < (r = 65535*c[i*3+1])) r = 65535; if (0> r) r = 0;
	colors[i].green = r;
	if (65535 < (r = 65535*c[i*3+2])) r = 65535; if (0> r) r = 0;
	colors[i].blue = r;
	colors[i].flags = DoRed|DoGreen|DoBlue;
	j = start+i;
	if (dde_x11_XFDI_current_dcm_data->visualtype == DcDirectColor)
		colors[i].pixel = (j<<16)|(j<<8)|j;
	else colors[i].pixel = j;
    }

    XStoreColors(dde_x11_XFDI_current_dcm_data->display, cmap, colors, n);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_dcm_set_stereo_eye(eye)
     int eye;
{
    if (eye) {
	XFDISetRenderable(dde_x11_XFDI_current_dcm_data->display,
			  dde_x11_XFDI_current_dcm_data->gc,
			  dde_x11_XFDI_current_dcm_data->Vdraw_buffer_stereo);
	if (dde_x11_XFDI_current_dcm_data->visualtype != DcPseudoColor) {
	    /*
	     * There is a bug in pseudo color that causes this call to fail
	     * so we have two z buffers in pseudocolor so no need to 
	     * associate the Z buffer
	     */
	    XFDIAssociateZbuffer(dde_x11_XFDI_current_dcm_data->display,
				 dde_x11_XFDI_current_dcm_data->Vdraw_buffer_stereo,
				 dde_x11_XFDI_current_dcm_data->z_buffer);
	}
    } else {
	XFDISetRenderable(dde_x11_XFDI_current_dcm_data->display,
			  dde_x11_XFDI_current_dcm_data->gc,
			  dde_x11_XFDI_current_dcm_data->Vdraw_buffer);
	if (dde_x11_XFDI_current_dcm_data->visualtype != DcPseudoColor) {
	    /*
	     * There is a bug in pseudo color that causes this call to fail
	     * so we have two z buffers in pseudocolor so no need to 
	     * associate the Z buffer
	     */
	    XFDIAssociateZbuffer(dde_x11_XFDI_current_dcm_data->display,
				 dde_x11_XFDI_current_dcm_data->Vdraw_buffer,
				 dde_x11_XFDI_current_dcm_data->z_buffer);
	}
    }
      
    XFDISetBuffer(dde_x11_XFDI_current_dcm_data->display, 
		  dde_x11_XFDI_current_dcm_data->gc, 
		  dde_x11_XFDI_current_dcm_data->texture_buffer, 
		  XFDIPmapTexUV);

    if (dde_x11_XFDI_current_dcm_data->multi_pass_transp) {
	XFDISetBuffer(dde_x11_XFDI_current_dcm_data->display,
		      dde_x11_XFDI_current_dcm_data->gc, 
		      dde_x11_XFDI_current_dcm_data->sort_z_buffer, 
		      XFDIPmapSortZ);
	XFDISetBuffer(dde_x11_XFDI_current_dcm_data->display,
		      dde_x11_XFDI_current_dcm_data->gc,
		      dde_x11_XFDI_current_dcm_data->sort_i_buffer, 
		      XFDIPmapSortI);
	XFDISetBuffer(dde_x11_XFDI_current_dcm_data->display,
		      dde_x11_XFDI_current_dcm_data->gc, 
		      dde_x11_XFDI_current_dcm_data->sort_a_buffer, 
		      XFDIPmapSortA);
    }
}
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_set_current_view (view)
     DtObject view;
{
    dde_x11_XFDI_current_dcm_data->previous_view =
	    dde_x11_XFDI_current_dcm_data->current_view;
    dde_x11_XFDI_current_dcm_data->current_view = view;

    if (dde_x11_XFDI_current_dcm_data->double_buffered &&
        ((dde_x11_XFDI_current_dcm_data->current_view != DcNullObject) &&
	 (dde_x11_XFDI_current_dcm_data->current_view !=
	  dde_x11_XFDI_current_dcm_data->previous_view))) {
#ifdef DEBUG
	printf ("blting front to back before update\n");
#endif
    }
}
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_set_depth_buffer_enable (flag)
     DtFlag flag;
{
}
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_set_depth_buffer_write (flag)
     DtFlag flag;
{
    if (flag) {
	XFDISetDrawMode(dde_x11_XFDI_current_dcm_data->display,
			dde_x11_XFDI_current_dcm_data->gc, XFDIDrawZ);
    } else {
	XFDISetDrawMode(dde_x11_XFDI_current_dcm_data->display,
			dde_x11_XFDI_current_dcm_data->gc, XFDIDrawNoZ);
    }
}
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_set_shade_index (index)
     DtInt index;
{
    dde_x11_XFDI_current_dcm_data->shade_index = index;
}
/*
 ======================================================================
 */
ddr_x11_XFDI_dcm_set_viewport (xmin, ymin, xmax, ymax)
     DtReal xmin;
     DtReal ymin;
     DtReal xmax;
     DtReal ymax;
{
    int x, y;
    int width, height;

    dde_x11_XFDI_current_dcm_data->viewport.x = xmin;
    dde_x11_XFDI_current_dcm_data->viewport.y = ymin;
    dde_x11_XFDI_current_dcm_data->viewport.width = xmax - xmin;
    dde_x11_XFDI_current_dcm_data->viewport.height = ymax - ymin;

    XVPChange(dde_x11_XFDI_current_dcm_data->display,
	      dde_x11_XFDI_current_dcm_data->Vdraw_buffer,
	      dde_x11_XFDI_current_dcm_data->viewport.x, 
	      dde_x11_XFDI_current_dcm_data->viewport.y,
	      dde_x11_XFDI_current_dcm_data->viewport.width, 
	      dde_x11_XFDI_current_dcm_data->viewport.height);
}
