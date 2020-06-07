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
	void ddr_x11_DGL_dcm_inquire_color_entries (colormodel,start, n, c)
	void ddr_x11_DGL_dcm_inquire_auto_size (flag)
	void ddr_x11_DGL_dcm_inquire_clip_list (nrects, clip_list, extent, partiallyobscuredflag)
	ddr_x11_DGL_dcm_inq_pseudocolor_bitselect_values ddr_x11_DGL_dcm_inq_pseudocolor_bitselect_values (weights, masks)
	int ddr_x11_DGL_dcm_inq_pseudocolor_weighted_average_weights (weights)
	void ddr_x11_DGL_dcm_inquire_device_extent (volume)
	void ddr_x11_DGL_dcm_inquire_ncolors (ncolors)
	int ddr_x11_DGL_dcm_inquire_pDraw (pDraw)
	int ddr_x11_DGL_dcm_inquire_pGC (pGC)
	DtFlag ddr_x11_DGL_dcm_inquire_pixel_data (requesttype, width, height, type, data, userdelete)
	void ddr_x11_DGL_dcm_inquire_resolution (xres, yres)
	void ddr_x11_DGL_dcm_inquire_stereo (stereoswitch)
	void ddr_x11_DGL_dcm_inquire_visual_type (visualtype)

  ======================================================================
 */
#include "dcm.h"
/*
 ======================================================================
 */
void
ddr_x11_DGL_dcm_inquire_color_entries (colormodel,start, n, c)
     DtColorModel colormodel;
     int start;
     int n;
     DtReal *c;
{
    XColor colors[256];
    Colormap cmap ;
    int i, j;
    DtReal r;

    XGetWindowAttributes(XDISPLAY(dde_x11_DGL_current_dcm_data),
			 XWINDOW(dde_x11_DGL_current_dcm_data),
			 &XWINDOWATTRIBUTES(dde_x11_DGL_current_dcm_data));

    cmap = XWINDOWATTRIBUTES(dde_x11_DGL_current_dcm_data).colormap;

    for (i=0; i<n; i++) {
	j = start+i;
	if (dde_x11_DGL_current_dcm_data->visualtype == DcDirectColor)
		colors[i].pixel = (j<<16)|(j<<8)|j;
	else colors[i].pixel = j;
    }
    XQueryColors(XDISPLAY(dde_x11_DGL_current_dcm_data), 
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
ddr_x11_DGL_dcm_inquire_auto_size (flag)
     DtFlag *flag;
{
    *flag = dde_x11_DGL_current_dcm_data->auto_size;
}
/*
 ======================================================================
 */

void
ddr_x11_DGL_dcm_inquire_clip_list (nrects, clip_list,
				     extent, partiallyobscuredflag)
     DtUInt *nrects;
     DtReal **clip_list;
     DtReal extent[4];
     DtFlag *partiallyobscuredflag;
{
   *nrects = XNRECTS(dde_x11_DGL_current_dcm_data);
   *clip_list = XClipRects(dde_x11_DGL_current_dcm_data);

   extent[0] = XExtent(dde_x11_DGL_current_dcm_data).x1;
   extent[1] = XExtent(dde_x11_DGL_current_dcm_data).y1;
   extent[2] = XExtent(dde_x11_DGL_current_dcm_data).x2;
   extent[3] = XExtent(dde_x11_DGL_current_dcm_data).y2;

   *partiallyobscuredflag = 
	 dde_x11_DGL_current_dcm_data->partially_obscure;

#ifdef DEBUG
   printf("inquire clip list:\n");
   printf("boxno = %d\n",XNRECTS(dde_x11_DGL_current_dcm_data));
   printf("sequenceno = %d\n",
	  XSEQUENCENO(dde_x11_DGL_current_dcm_data));
   printf("first clipping rectangle:  %d %d %d %d\n",
	  (DtInt)((*clip_list)[0]),
	  (DtInt)((*clip_list)[1]),
	  (DtInt)((*clip_list)[2]),
	  (DtInt)((*clip_list)[3]));
   printf("extent = %d %d %d %d\n",
	  XExtent(dde_x11_DGL_current_dcm_data).x1,
	  XExtent(dde_x11_DGL_current_dcm_data).y1,
	  XExtent(dde_x11_DGL_current_dcm_data).x2,
	  XExtent(dde_x11_DGL_current_dcm_data).y2);
   printf("partiallyobscuredflag = %d\n",*partiallyobscuredflag);
#endif
}





/*
 ======================================================================
 */

ddr_x11_DGL_dcm_inq_pseudocolor_bitselect_values (weights, masks)
     XdReal **weights;
     int **masks;
{
    *weights = dde_x11_DGL_current_dcm_data->cbtselpsweight;
    *masks = dde_x11_DGL_current_dcm_data->cbtselpsmask;
}
/*
 ======================================================================
 */

ddr_x11_DGL_dcm_inq_pseudocolor_weighted_average_weights(weights)
     XdReal **weights;
{
    *weights = dde_x11_DGL_current_dcm_data->cwtavgpsweight;
}
/*
 ======================================================================
 */

void
ddr_x11_DGL_dcm_inquire_device_extent (volume)
     DtVolume *volume;
{
    volume->bll[0] = XExtent(dde_x11_DGL_current_dcm_data).x1;
    volume->bll[1] = 1024-XExtent(dde_x11_DGL_current_dcm_data).y2;
    volume->bll[2] = 0.;

    volume->fur[0] = XExtent(dde_x11_DGL_current_dcm_data).x2;
    volume->fur[1] = 1024-XExtent(dde_x11_DGL_current_dcm_data).y1;
    volume->fur[2] = 1.;
}

/*
 ======================================================================
 */

void
ddr_x11_DGL_dcm_inquire_ncolors (ncolors)
     DtInt *ncolors;
{
    *ncolors = 256;
}
/*
 ======================================================================
 */

ddr_x11_DGL_dcm_inquire_pDraw(pDraw)
     XdDrawablePtr *pDraw;
{
    *pDraw = XpDRAW(dde_x11_DGL_current_dcm_data);
}

/*
 ======================================================================
 */

ddr_x11_DGL_dcm_inquire_pGC(pGC)
     XdGCPtr *pGC;

{
    *pGC = XpGC(dde_x11_DGL_current_dcm_data);
}
/*
 ======================================================================
 */

DtFlag
ddr_x11_DGL_dcm_inquire_pixel_data (requesttype, width, height, type, 
				   data, userdelete)
     DtRasterType requesttype;
     DtInt *width;
     DtInt *height;
     DtRasterType *type;
     DtPtr *data;
     DtFlag *userdelete;
{
   unsigned char *dptr;
   unsigned char *pr, *pg, *pb;
   int l, w;
   Colormap cmap;
   XColor colors[4096];
   DtFlag ok;
   int index;
   int i;
   unsigned char *tmpred = NULL;
   unsigned char *tmpgreen = NULL;
   unsigned char *tmpblue = NULL;

   if (requesttype == DcRasterA || requesttype == DcRasterZ)
      /* don't know how to get alpha or depth info */
      return (DcFalse);

   /* this device always returns *type=DcRasterRGB */

   if (requesttype != DcRasterRGB &&
       requesttype != DcRasterABGR &&
       requesttype != DcRasterRGBA &&
       requesttype != DcRasterRGBZ &&
       requesttype != DcRasterRGBAZ)
      /* unknown raster type */
      return (DcFalse);

   XGetWindowAttributes(XDISPLAY(dde_x11_DGL_current_dcm_data),
			XWINDOW(dde_x11_DGL_current_dcm_data),
			&XWINDOWATTRIBUTES(dde_x11_DGL_current_dcm_data));

   *width =  XExtent(dde_x11_DGL_current_dcm_data).x2 - 
	 XExtent(dde_x11_DGL_current_dcm_data).x1;
   *height =  XExtent(dde_x11_DGL_current_dcm_data).y2 - 
	 XExtent(dde_x11_DGL_current_dcm_data).y1;

#ifdef DEBUG
   printf ("width %d, height %d\n", *width, *height);
#endif

   tmpred = (unsigned char *)DDspace_Allocate ((*width)*(*height)*
					       sizeof(unsigned char));

   if (dde_x11_DGL_current_dcm_data->visualtype == DcDirectColor) {
      tmpgreen = (unsigned char *)DDspace_Allocate ((*width)*(*height)*
						    sizeof(unsigned char));

      tmpblue = (unsigned char *)DDspace_Allocate ((*width)*(*height)*
						   sizeof(unsigned char));
   }

   if (XDoubleBuffered(dde_x11_DGL_current_dcm_data)) {
      XdSetDrawBuffer(XpDRAW(dde_x11_DGL_current_dcm_data),
		      XDisplayBuffer(dde_x11_DGL_current_dcm_data),
		      XBLeftEye);
   }
   XdGetImage(XpDRAW(dde_x11_DGL_current_dcm_data),
	      XpGC(dde_x11_DGL_current_dcm_data),
	      0, 0, 0, 0, *width, *height, *width, 
	      tmpred, 
	      XdRedBank);
   if (dde_x11_DGL_current_dcm_data->visualtype == DcDirectColor) {
      XdGetImage(XpDRAW(dde_x11_DGL_current_dcm_data),
		 XpGC(dde_x11_DGL_current_dcm_data),
		 0, 0, 0, 0, *width, *height, *width, 
		 tmpgreen, 
		 XdGreenBank);
      XdGetImage(XpDRAW(dde_x11_DGL_current_dcm_data),
		 XpGC(dde_x11_DGL_current_dcm_data),
		 0, 0, 0, 0, *width, *height, *width, 
		 tmpblue, 
		 XdBlueBank); 
   }

   if (XDoubleBuffered(dde_x11_DGL_current_dcm_data)) {
      XdSetDrawBuffer(XpDRAW(dde_x11_DGL_current_dcm_data),
		      XDrawBuffer(dde_x11_DGL_current_dcm_data),
		      XBLeftEye);
   }
   *type = DcRasterRGB;
   *data = (DtPtr)DDspace_Allocate(sizeof(char)*3*(*width)*(*height));
   *userdelete = DcTrue;

   switch (dde_x11_DGL_current_dcm_data->visualtype) {
   case DcPseudoColor:
#ifdef DEBUG
      printf ("PseudoColor\n");
#endif
      cmap = XWINDOWATTRIBUTES(dde_x11_DGL_current_dcm_data).colormap;

      for (i=0; 
	   i<XWINDOWATTRIBUTES(dde_x11_DGL_current_dcm_data).visual->map_entries;
	   i++) {
	 colors[i].pixel = i;
      }

      XQueryColors(XDISPLAY(dde_x11_DGL_current_dcm_data), cmap, colors, 
		   XWINDOWATTRIBUTES(dde_x11_DGL_current_dcm_data).visual->map_entries);

      pr = tmpred;
      dptr = (unsigned char *)*data;

      for (l=0; l< *height; l++) {
	 for (w=0; w < *width; w++) {
	    index = *pr++;
	    *dptr++ = colors[index].red >> 8;
	    *dptr++ = colors[index].green >> 8;
	    *dptr++ = colors[index].blue >> 8;
	 }
      }
      ok = DcTrue;
      break;

   case TrueColor:
   case DirectColor:
#ifdef DEBUG
      printf ("TrueColor\n");
#endif
      pr = tmpred;
      pg = tmpgreen;
      pb = tmpblue;
      dptr = (unsigned char *)*data;

      for (l=0; l< *height; l++) {
	 for (w=0; w < *width; w++) {
	    *dptr++ = *pr++;
	    *dptr++ = *pg++;
	    *dptr++ = *pb++;
	 }
      }
      ok = DcTrue;
      break;
	 
   default:
#ifdef DEBUG
      printf ("Unknown %d\n",
	      dde_x11_DGL_current_dcm_data->window_attributes.visual->class );
#endif
      ok= DcFalse;
   }

   DDspace_Deallocate(tmpred);
   if (dde_x11_DGL_current_dcm_data->visualtype == DcDirectColor) {
      DDspace_Deallocate(tmpgreen);
      DDspace_Deallocate(tmpblue);
   }

   return (ok);
}
/*
 ======================================================================
 */

void
ddr_x11_DGL_dcm_inquire_resolution (xres, yres)
     DtReal *xres;
     DtReal *yres;
{
    if (dde_x11_DGL_current_dcm_data->stereoscreen) {
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

void
ddr_x11_DGL_dcm_inquire_stereo (stereoswitch)
     DtFlag *stereoswitch;
{
    *stereoswitch = dde_x11_DGL_current_dcm_data->stereoswitch ? DcTrue : DcFalse;
}
/*
 ======================================================================
 */

void
ddr_x11_DGL_dcm_inquire_visual_type (visualtype)
     DtVisualType *visualtype;
{
    *visualtype = dde_x11_DGL_current_dcm_data->visualtype;
}
