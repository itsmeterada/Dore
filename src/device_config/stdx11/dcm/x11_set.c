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
	void ddr_stdx11_dcm_set_color_entries (colormodel, start,n,c)
	void ddr_stdx11_dcm_set_background_color (colormodel,c)
	void ddr_stdx11_dcm_set_foreground_color (colormodel,c)
	void ddr_stdx11_dcm_set_current_view (view)
	void ddr_stdx11_dcm_set_depth_buffer_enable (flag)
	void ddr_stdx11_dcm_set_depth_buffer_write (flag)
	void ddr_stdx11_dcm_set_shade_index (index)

  ======================================================================
 */
#include "dcm.h"

/*
 ======================================================================
 */
void ddr_stdx11_dcm_set_color_entries (
    DtColorModel colormodel,
    int start,
    int n,
    DtReal *c)
{
   int i,j,k;
   Colormap cmap;
   XColor color;
   ddt_stdx11_data *dev;
   int maxn;

#ifdef DEBUG
   printf("... processing device set color entries ...\n");
   fflush(stdout);
#endif

   if(dde_stdx11_current_device_data->visualtype != DcPseudoColor)
	 return;
        
   dev = dde_stdx11_current_device_data;
   cmap = dev->window_attributes.colormap;

   if (start < 0) start = 0;
   maxn = dev->nred * dev->ngreen * dev->nblue;
   if (start + n > maxn) n = maxn - start;
   if (n < 0) n = 0;

   if (n) XFreeColors(dev->display, cmap, &dev->pseudomap[start], n, 0);

   for(i=0; i<n; i++) {
      j = (k=255*c[i*3+0]) > 255 ? 255 : k < 0 ? 0 : k;
      color.red = j << 8;
      j = (k=255*c[i*3+1]) > 255 ? 255 : k < 0 ? 0 : k;
      color.green = j << 8;
      j = (k=255*c[i*3+2]) > 255 ? 255 : k < 0 ? 0 : k;
      color.blue = j << 8;
      XAllocColor(dev->display,cmap, &color);
      dev->pseudomap[i+start] = color.pixel;
   }
}
/*
 ======================================================================
 */
void ddr_stdx11_dcm_set_background_color (
    DtColorModel colormodel,
    DtReal *c)
{
   ddt_stdx11_data *device_data;
   DtInt index;

#ifdef DEBUG
   printf("... processing device set background color ...\n");
   fflush(stdout);
#endif

   device_data = dde_stdx11_current_device_data;

   /* XXX Is this correct? -- perhaps choose between white and black? */
   if(device_data->visualtype == DcStaticGrey)
	 return;

   ddr_stdx11_dcm_setup_shading();

   index = ddr_stdx11_dcm_color_to_pixel(c,device_data);

   device_data->bg_color_index = index;

#ifdef DEBUG
   printf("backgound color (%lf,%lf,%lf) index(%d)\n",
	  c[0],c[1],c[2],index);
   fflush(stdout);
#endif
}
/*
 ======================================================================
 */
void ddr_stdx11_dcm_set_foreground_color (
    DtColorModel colormodel,
    DtReal *c)
{
   ddt_stdx11_data *device_data;
   DtInt index;

#ifdef DEBUG
   printf("...processing device set foreground color \n");
   fflush(stdout);
#endif
	
   device_data = dde_stdx11_current_device_data;

   /* XXX Is this correct? perhaps choose between white and black? */
   if(device_data->visualtype == DcStaticGrey) {
      return;
   }

   ddr_stdx11_dcm_setup_shading();

   index = ddr_stdx11_dcm_color_to_pixel(c,device_data);

#ifdef DEBUG
   printf("color(%lf,%lf,%lf),fg_index = %d\n",c[0],c[1],c[2],index);
#endif
   /* 
    * If we're setting the color and it is already set, don't do it.
    */
   if (index == device_data->fg_color_index) return;

   device_data->fg_color_index = index;

   XSetForeground(device_data->display,device_data->gc,
		  (unsigned long)index);
}
/*
 ======================================================================
 */
void ddr_stdx11_dcm_set_current_view (
    DtObject view)
{
   dde_stdx11_current_device_data->previous_view =
	 dde_stdx11_current_device_data->current_view;
   dde_stdx11_current_device_data->current_view = view;
}
/*
 ======================================================================
 */
void ddr_stdx11_dcm_set_depth_buffer_enable (
    DtFlag flag)
{
#ifdef DEBUG
   printf("... processing device set depth buffer enable ...\n");
   fflush(stdout);
#endif
   dde_stdx11_current_device_data->dbf_enable = flag;
}
/*
 ======================================================================
 */
void ddr_stdx11_dcm_set_depth_buffer_write (
    DtFlag flag)
{
#ifdef DEBUG
   printf("... processing device set depth buffer write ...\n");
   printf("Not Implemented.\n");
   fflush(stdout);
#endif
}
/*
 ======================================================================
 */
void ddr_stdx11_dcm_set_shade_index (
    DtInt index)
{
#ifdef DEBUG
   printf("...processing device set shade index\n");
   fflush(stdout);
#endif

   dde_stdx11_current_device_data->shade_index = index;

#ifdef DEBUG
   printf("shade index = %d\n",index);
   fflush(stdout);
#endif
}
