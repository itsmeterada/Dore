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
	void ddr_x11_XFDI_dcm_inquire_color_entries (colormodel,start, n, c)
	void ddr_x11_XFDI_dcm_inquire_auto_size (flag)
	void ddr_x11_XFDI_dcm_inquire_clip_list (nrects, clip_list, extent, partiallyobscuredflag)
	int ddr_x11_XFDI_dcm_inq_pseudocolor_weighted_average_weights (weights)
	void ddr_x11_XFDI_dcm_inquire_device_extent (volume)
	Display * ddr_x11_XFDI_dcm_inquire_display ()
	XFDIGC ddr_x11_XFDI_dcm_inquire_gc ()
	XImage * ddr_x11_XFDI_dcm_inquire_image ()
	void ddr_x11_XFDI_dcm_inquire_ncolors (ncolors)
	long ddr_x11_XFDI_dcm_get_pixel_value (color)
	DtFlag ddr_x11_XFDI_dcm_inquire_pixel_data (requesttype, width, height, type, data, userdelete)
	void ddr_x11_XFDI_dcm_inquire_resolution (xres, yres)
	DtFlag ddr_x11_XFDI_dcm_inquire_spheres ()
	void ddr_x11_XFDI_dcm_inquire_stereo (stereoswitch)
	void ddr_x11_XFDI_dcm_inquire_visual_type (visualtype)
	Window ddr_x11_XFDI_dcm_inquire_window ()
	GC ddr_x11_XFDI_dcm_inquire_xgc ()

  ======================================================================
 */
#include "dcm.h"
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_inquire_color_entries (colormodel,start, n, c)
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
	DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_x11_XFDI_dcm_inquire_color_entries",
		    "(start + count) is outside colormap size");
	n = dde_x11_XFDI_current_dcm_data->ncolors - start;
    } 
    if (n < 0) {
	DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_x11_XFDI_dcm_inquire_color_entries",
		    "count less than zero");
	return;
    }

    for (i=0; i<n; i++) {
	j = start+i;

	if (dde_x11_XFDI_current_dcm_data->visualtype == DcDirectColor)
		colors[i].pixel = (j<<16)|(j<<8)|j;
	else 
		colors[i].pixel = j;
    }

    XQueryColors(dde_x11_XFDI_current_dcm_data->display, cmap, colors, n);
		
    for (i=0; i<n; i++) {
	if ((r = (DtReal)colors[i].red / 65535.) < 0.0) 
		r = 0.0;
	if (r > 1.0)
		r = 1.0;
	c[i*3] = r;

	if ((r = (DtReal)colors[i].green / 65535.) < 0.0) 
		r = 0.0;
	if (r > 1.0)
		r = 1.0;
	c[i*3+1] = r;

	if ((r = (DtReal)colors[i].blue / 65535.) < 0.0) 
		r = 0.0;
	if (r > 1.0)
		r = 1.0;
	c[i*3+2] = r;
    }
}
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_inquire_auto_size (flag)
     DtFlag *flag;
{
    *flag = dde_x11_XFDI_current_dcm_data->auto_size;
}
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_inquire_clip_list (nrects, clip_list,
				    extent, partiallyobscuredflag)
     DtUInt *nrects;
     DtReal **clip_list;
     DtReal extent[4];
     DtFlag *partiallyobscuredflag;
{
   static DtReal clist[4];

   clist[0] = extent[0] = 0.0;
   clist[1] = extent[1] = 0.0;
   clist[2] = extent[2] = dde_x11_XFDI_current_dcm_data->window_extent.width;
   clist[3] = extent[3] = dde_x11_XFDI_current_dcm_data->window_extent.height;

   *nrects = 1;
   *clip_list = clist;
   *partiallyobscuredflag = DcFalse;
}
/*
 ======================================================================
 */
ddr_x11_XFDI_dcm_inq_pseudocolor_weighted_average_weights(weights)
     DtReal **weights;
{
    *weights = dde_x11_XFDI_current_dcm_data->cwtavgpsweight;
}

/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_inquire_device_extent (volume)
     DtVolume *volume;
{
    volume->bll[0] = 0.0;
    volume->bll[1] = 1024 - dde_x11_XFDI_current_dcm_data->window_extent.height;
    volume->bll[2] = 0.0;
    volume->fur[0] = dde_x11_XFDI_current_dcm_data->window_extent.width;
    volume->fur[1] = 1024;
    volume->fur[2] = 1.0;
}
/*
 ======================================================================
 */

Display
*ddr_x11_XFDI_dcm_inquire_display ()
{
    return(dde_x11_XFDI_current_dcm_data->display);
}
/*
 ======================================================================
 */
XFDIGC
ddr_x11_XFDI_dcm_inquire_gc ()
{
    return(dde_x11_XFDI_current_dcm_data->gc);
}
/*
 ======================================================================
 */
XImage *
ddr_x11_XFDI_dcm_inquire_image ()
{
    return(dde_x11_XFDI_current_dcm_data->image);
}
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_inquire_ncolors (ncolors)
     DtInt *ncolors;
{
    *ncolors = dde_x11_XFDI_current_dcm_data->ncolors;
}
/*
 ======================================================================
 */
unsigned long
ddr_x11_XFDI_dcm_get_pixel_value (color)
     DtColorRGB color;
{
    unsigned long c;

    if (dde_x11_XFDI_current_dcm_data->visualtype == DcPseudoColor) {
	c = dde_x11_XFDI_current_dcm_data->cwtavgpslim[0] + 
		(color[0] * dde_x11_XFDI_current_dcm_data->cwtavgpsweight[0] +
		 color[1] * dde_x11_XFDI_current_dcm_data->cwtavgpsweight[1] +
		 color[2] * dde_x11_XFDI_current_dcm_data->cwtavgpsweight[2]) *
			 dde_x11_XFDI_current_dcm_data->cwtavgpslim[2];

	c = c > dde_x11_XFDI_current_dcm_data->cwtavgpslim[1] ? 
		dde_x11_XFDI_current_dcm_data->cwtavgpslim[1] : c;
    } else {
	c = (256*256* (255 & (int)(255.*color[0]))) | 
		(256* (255 & (int)(255.*color[1]))) |
			(255 & (int)(255.*color[2]));
    }
    return c;
}
/*
 ======================================================================
 */
DtFlag
ddr_x11_XFDI_dcm_inquire_pixel_data (requesttype, width, height, type, 
				   data, userdelete)
     DtRasterType requesttype;
     DtInt *width;
     DtInt *height;
     DtRasterType *type;
     DtPtr *data;
     DtFlag *userdelete;
{
   XImage *image, *zimage;
   unsigned char *i_linep;
   unsigned char *dptr;
   unsigned char *p;
   unsigned char *z;
   unsigned char *z_linep;
   int l, w;
   Colormap cmap;
   XColor colors[4096];
   DtFlag ok;
   DtFlag needz;
   int index;
   int i;
   union {
      int i;
      char c[4];
   } z_value;


   switch (requesttype) {
   case DcRasterRGB:	needz = DcFalse;	break;
   case DcRasterRGBZ:	needz = DcTrue;		break;
   default:		return (DcFalse);
   }

   XGetWindowAttributes(dde_x11_XFDI_current_dcm_data->display,
			dde_x11_XFDI_current_dcm_data->window,
			&dde_x11_XFDI_current_dcm_data->window_attributes);

   image = XGetImage(dde_x11_XFDI_current_dcm_data->display,
		     dde_x11_XFDI_current_dcm_data->window,
		     0, 0, 
		     dde_x11_XFDI_current_dcm_data->window_extent.width,
		     dde_x11_XFDI_current_dcm_data->window_extent.height,
		     AllPlanes, ZPixmap);
   if (needz &&
       dde_x11_XFDI_current_dcm_data->window_attributes.visual->class != PseudoColor) {
      zimage = XGetImage(dde_x11_XFDI_current_dcm_data->display,
			 dde_x11_XFDI_current_dcm_data->z_buffer,
			 0, 0, 
			 dde_x11_XFDI_current_dcm_data->window_extent.width,
			 dde_x11_XFDI_current_dcm_data->window_extent.height,
			 AllPlanes, ZPixmap);
      if (zimage == NULL) {
	 needz = DcFalse;
      }
   }

#ifdef DEBUG
   printf ("\timage = 0x%x\n", image);
#endif

   if (image != (XImage *)0) {
#ifdef DEBUG
      printf ("\twidth\t\t\t%d\n", image->width);
      printf ("\theight\t\t\t%d\n", image->height);
      printf ("\tformat\t\t\t%s\n", image->format == XYBitmap ? "XYBitmap" :
	      image->format == XYPixmap ? "XYPixmap" : "ZPixmap");
      printf ("\tdata\t\t\t0x%x\n", image->data);
      printf ("\tbyte_order\t\t%s\n", image->byte_order  == LSBFirst ?
	      "LSBFirst" : "MSBFirst");
      printf ("\tbitmap_unit\t\t%d\n", image->bitmap_unit);
      printf ("\tbitmap_bit_order\t%s\n", image->bitmap_bit_order == LSBFirst ?
	      "LSBFirst" : "MSBFirst");
      printf ("\tbitmap_pad\t\t%d\n", image->bitmap_pad);
      printf ("\tdepth\t\t\t%d\n", image->depth);
      printf ("\tbytes_per_line\t\t%d\n", image->bytes_per_line);
      printf ("\tbits_per_pixel\t\t%d\n", image->bits_per_pixel);
      printf ("\tred_mask\t\t0x%x\n", image->red_mask);
      printf ("\tgreen_mask\t\t0x%x\n", image->green_mask);
      printf ("\tblue_mask\t\t0x%x\n", image->blue_mask);
#endif
      *width = image->width;
      *height = image->height;
	 *userdelete = DcTrue;
      if (needz) {
	 *type = DcRasterRGBZ;
	 *data = (DtPtr)DDspace_Allocate(sizeof(char)*7*image->width*image->height);
      } else {
	 *type = DcRasterRGB;
	 *data = (DtPtr)DDspace_Allocate(sizeof(char)*3*image->width*image->height);
      }
      switch (dde_x11_XFDI_current_dcm_data->window_attributes.visual->class) {
      case PseudoColor:
#ifdef DEBUG
	 printf ("PseudoColor\n");
	 printf ("\tmap_entries\t%d\n",
		 dde_x11_XFDI_current_dcm_data->window_attributes.visual->map_entries);
#endif
	 cmap = dde_x11_XFDI_current_dcm_data->window_attributes.colormap;

	 for (i=0; 
	      i<dde_x11_XFDI_current_dcm_data->window_attributes.visual->map_entries;
	      i++) {
	    colors[i].pixel = i;
	 }

	 XQueryColors(dde_x11_XFDI_current_dcm_data->display, cmap, colors, 
		      dde_x11_XFDI_current_dcm_data->window_attributes.visual->map_entries);

	 i_linep = (unsigned char *)image->data;
	 dptr = (unsigned char *)*data;

	 for (l=0; l<image->height; l++) {
	    p = i_linep;

	    for (w=0; w < image->width; w++) {
	       index = *p++;
	       if (image->depth > 8) {
		  index = index<<8  | *p++;
	       }
	       *dptr++ = colors[index].red >> 8;
	       *dptr++ = colors[index].green >> 8;
	       *dptr++ = colors[index].blue >> 8;
	       if (needz) {
		  index = *p++;
		  index = index<<8 | *p++;
		  z_value.i = index<<8 | *p++;
		  *dptr++ = z_value.c[0];
		  *dptr++ = z_value.c[1];
		  *dptr++ = z_value.c[2];
		  *dptr++ = z_value.c[3];
	       }
	    }
	    i_linep += image->bytes_per_line;
	 }
	 ok = DcTrue;
	 break;

      case TrueColor:
      case DirectColor:
#ifdef DEBUG
	 printf ("TrueColor\n");
#endif
	 i_linep = (unsigned char *)image->data;
	 dptr = (unsigned char *)*data;
	 if (needz) 
	       z_linep = (unsigned char *)zimage->data;
	 
	 for (l=0; l<image->height; l++) {
	    p = i_linep;
	    if (needz) 
		  z = z_linep;

	    for (w=0; w < image->width; w++) {
	       p++;
	       *dptr++ = *p++;
	       *dptr++ = *p++;
	       *dptr++ = *p++;
	       if (needz) {
		  *dptr++ = *z++;
		  *dptr++ = *z++;
		  *dptr++ = *z++;
		  *dptr++ = *z++;
	       }
	    }
	    i_linep += image->bytes_per_line;
	    if (needz) z_linep += zimage->bytes_per_line;
	 }
	 ok = DcTrue;
	 break;
	 
      default:
#ifdef DEBUG
	 printf ("Unknown %d\n",
		 dde_x11_XFDI_current_dcm_data->window_attributes.visual->class );
#endif
	 ok= DcFalse;
      }

      XDestroyImage(image);
      if (needz &&dde_x11_XFDI_current_dcm_data->window_attributes.visual->class != PseudoColor) {
	 XDestroyImage(zimage);
      }
      return (ok);
   }
   return (DcFalse);
}
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_inquire_resolution (xres, yres)
     DtReal *xres;
     DtReal *yres;
{
    *xres = .31738;
    *yres = .31738;
}
/*
 ======================================================================
 */
DtFlag
ddr_x11_XFDI_dcm_inquire_spheres()
{
    return (dde_x11_XFDI_current_dcm_data->use_spheres);
}
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_inquire_stereo (stereoswitch)
     DtFlag *stereoswitch;
{
    *stereoswitch = dde_x11_XFDI_current_dcm_data->stereoswitch ? DcTrue : DcFalse;
}
/*
 ======================================================================
 */
void
ddr_x11_XFDI_dcm_inquire_visual_type (visualtype)
     DtVisualType *visualtype;
{
    *visualtype = dde_x11_XFDI_current_dcm_data->visualtype;
}
/*
 ======================================================================
 */
Window
ddr_x11_XFDI_dcm_inquire_window ()
{
    return(dde_x11_XFDI_current_dcm_data->window);
}
/*
 ======================================================================
 */
GC
ddr_x11_XFDI_dcm_inquire_xgc ()
{
    return(dde_x11_XFDI_current_dcm_data->xgc);
}
