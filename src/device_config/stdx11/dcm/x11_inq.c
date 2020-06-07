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
	void ddr_stdx11_dcm_inquire_color_entries (colormodel, start, n, c)
	void ddr_stdx11_dcm_inquire_auto_size (flag)
	void ddr_stdx11_dcm_inquire_clip_list (nrects,clip_list, extent, partiallyobscuredflag)
	void ddr_stdx11_dcm_inquire_device_extent (volume)
	void ddr_stdx11_dcm_inquire_ncolors (ncolors)
	DtFlag ddr_stdx11_dcm_inquire_pixel_data (requesttype, width, height, type, data, userdelete)
	void ddr_stdx11_dcm_inquire_resolution (xres,yres)
	void ddr_stdx11_dcm_inquire_stereo (stereoswitch)
	void ddr_stdx11_dcm_inquire_visual_type (visualtype)

  ======================================================================
 */
#include "dcm.h"

/*
 ======================================================================
 */
void ddr_stdx11_dcm_inquire_color_entries (
    DtColorModel colormodel,
    int start,
    int n,
    DtReal *c)
{
#ifdef DEBUG
   printf("... processing device inquire color entries ...\n");
   printf("  Not yet implemented.\n");
   fflush(stdout);
#endif
}
/*
 ======================================================================
 */
void ddr_stdx11_dcm_inquire_auto_size (
    DtFlag *flag)
{
#ifdef DEBUG
   printf("... processing device inquire auto size ...\n");
   fflush(stdout);
#endif

   *flag = dde_stdx11_current_device_data->auto_size;
}
/*
 ======================================================================
 */
void ddr_stdx11_dcm_inquire_clip_list (
    DtUInt *nrects,
    DtReal **clip_list,
    DtReal extent[4],
    DtFlag *partiallyobscuredflag)
{
   static DtReal clist[4];

#ifdef DEBUG
   printf("... processing device inquire clip list ...\n");
   fflush(stdout);
#endif

   /*add an XSync for bug in Xlib. See TRs Dore-0657 and 0707 */
   XSync(dde_stdx11_current_device_data->display, False);
   XGetWindowAttributes(dde_stdx11_current_device_data->display,
			dde_stdx11_current_device_data->window,
			&(dde_stdx11_current_device_data->window_attributes));

   if((dde_stdx11_current_device_data->height !=
       dde_stdx11_current_device_data->window_attributes.height)
      || (dde_stdx11_current_device_data->width !=
	  dde_stdx11_current_device_data->window_attributes.width)) {
      DDdevice_SetAbortedDraw(dde_stdx11_current_device_data->device,DcTrue);

      XClearWindow(dde_stdx11_current_device_data->display,
		   dde_stdx11_current_device_data->window);
      dde_stdx11_current_device_data->update_needed = DcTrue;
   }

   clist[0] = extent[0] = 0;
   clist[1] = extent[1] = 0;
   clist[2] = extent[2] = dde_stdx11_current_device_data->width - 1;
   clist[3] = extent[3] = dde_stdx11_current_device_data->height - 1;

#ifdef DEBUG
   printf("extent = (%lf,%lf)-(%lf,%lf)\n",extent[0],extent[1],extent[2],extent[3]);
#endif
   *nrects = 1;
   *clip_list = clist;
   *partiallyobscuredflag = DcFalse;
}
/*
 ======================================================================
 */
void ddr_stdx11_dcm_inquire_device_extent (
    DtVolume *volume)
{
#ifdef DEBUG
   printf("... processing device inquire device extent ...\n");
   fflush(stdout);
#endif

   volume->bll[0] = 0.0;
   volume->bll[1] = 0.0;
   volume->bll[2] = 0.0;
   volume->fur[0] = dde_stdx11_current_device_data->width - 1;
   volume->fur[1] = dde_stdx11_current_device_data->height - 1;
   volume->fur[2] = 65535.0;
}
/*
 ======================================================================
 */
void ddr_stdx11_dcm_inquire_ncolors (
    DtInt *ncolors)
{
   ddt_stdx11_data *dev;
#ifdef DEBUG
   printf("... processing device inquire ncolors ...\n");
   fflush(stdout);
#endif

   dev = dde_stdx11_current_device_data;
   if(dev->visualtype == DcStaticGrey)
	 *ncolors = 2;
   else if (dev->visualtype == DcTrueColor) /* XXX ??? */
	 *ncolors = dev->nred;
   else  
	 *ncolors = dev->nred * dev->ngreen * dev->nblue;
}
/*
 ======================================================================
 */
DtFlag ddr_stdx11_dcm_inquire_pixel_data (
    DtRasterType requesttype,
    DtInt *width,
    DtInt *height,
    DtRasterType *type,
    DtPtr *data,
    DtFlag *userdelete)
{
   XImage *image;
   unsigned char *i_linep;
   unsigned char *dptr;
   unsigned char *p;
   int l, w;
   Colormap cmap;
   XColor colors[4096];
   DtFlag ok;
   int i, index;

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

   /*add an XSync for bug in Xlib. See TRs Dore-0657 and 0707 */
   XSync(dde_stdx11_current_device_data->display, False);
   XGetWindowAttributes(dde_stdx11_current_device_data->display,
			dde_stdx11_current_device_data->window,
			&dde_stdx11_current_device_data->window_attributes);

   image = XGetImage(dde_stdx11_current_device_data->display,
		     dde_stdx11_current_device_data->window,
		     0, 0, 
		     dde_stdx11_current_device_data->window_attributes.width,
		     dde_stdx11_current_device_data->window_attributes.height,
		     AllPlanes, ZPixmap);

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
      *type = DcRasterRGB;
      *data = (DtPtr)DDspace_Allocate(sizeof(char)*3*image->width*image->height);
      *userdelete = DcTrue;

      switch (dde_stdx11_current_device_data->window_attributes.visual->class) {
      case PseudoColor:
#ifdef DEBUG
	 printf ("PseudoColor\n");
	 printf ("\tmap_entries\t%d\n",
		 dde_stdx11_current_device_data->window_attributes.visual->map_entries);
#endif
	 cmap = dde_stdx11_current_device_data->window_attributes.colormap;

	 for (i=0; 
	      i<dde_stdx11_current_device_data->window_attributes.visual->map_entries;
	      i++) {
	    colors[i].pixel = i;
	 }

	 XQueryColors(dde_stdx11_current_device_data->display, cmap, colors, 
		      dde_stdx11_current_device_data->window_attributes.visual->map_entries);

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

	 for (l=0; l<image->height; l++) {
	     p = i_linep;
	     for (w=0; w < image->width; w++) {
	           p++;
                   dptr[0] = p[0];
                   dptr[1] = p[1];
                   dptr[2] = p[2];
                   dptr+=3;
                   p+=3;
	     }
	     i_linep += image->bytes_per_line;
	 }
	 ok = DcTrue;
	 break;
	 
      default:
#ifdef DEBUG
	 printf ("Unknown %d\n",
		 dde_stdx11_current_device_data->window_attributes.visual->class );
#endif
	 ok= DcFalse;
      }

      XDestroyImage(image);

      return (ok);
   }
   return (DcFalse);
}
/*
 ======================================================================
 */
void ddr_stdx11_dcm_inquire_resolution (
    DtReal *xres,
    DtReal *yres)
{
#ifdef DEBUG
   printf("... processing device inquire resolution ...\n");
   fflush(stdout);
#endif

   *xres = .28125;
   *yres = .28125;
}
/*
 ======================================================================
 */
void ddr_stdx11_dcm_inquire_stereo (
    DtFlag *stereoswitch)
{
#ifdef DEBUG
   printf("... processing device inquire stereo ...\n");
   fflush(stdout);
#endif

   *stereoswitch = DcFalse;
}
/*
 ======================================================================
 */
void ddr_stdx11_dcm_inquire_visual_type (
    DtVisualType *visualtype)
{
#ifdef DEBUG
   printf("... processing device inquire visual type ...\n");
   fflush(stdout);
#endif

   *visualtype = dde_stdx11_current_device_data->visualtype;
}
