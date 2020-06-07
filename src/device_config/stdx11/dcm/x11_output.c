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
	void ddr_stdx11_dcm_clear_rectangle_depth (xmin,ymin,xmax,ymax)
	void ddr_stdx11_dcm_clear_rectangle_depth_and_color (xmin,ymin,xmax,ymax)
	void ddr_stdx11_dcm_write_scanline_byte (xstart,ystart,length,data)
	void ddr_stdx11_DrawPoint (point,color)
	void ddr_stdx11_DrawLine (point1,point2,color1,color2)
	void ddr_stdx11_DrawTriangle (point1,point2,point3,color1,color2,color3)
	void ddr_stdx11_PutPixel (x,y,color)
	void ddr_stdx11_Dither (x,y,color)
	void ddr_stdx11_DitherColor (x,y,color)

  ======================================================================
 */
#include "dcm.h"

/*
 ======================================================================
 */
static int dither_table[DITHER_SIZE][DITHER_SIZE] = {
   {  0,  8,  2, 10 }, 
   { 12,  4, 14,  6 }, 
   {  3, 11,  1,  9 }, 
   { 15,  7, 13,  5 }
};

static float dither_matrix[4][4] = {
   { 0.0,    0.5,    0.125,  0.625  },
   { 0.75,   0.25,   0.875,  0.375  },
   { 0.1875, 0.6875, 0.0625, 0.5625 },
   { 0.9375, 0.4375, 0.8125, 0.3125 }
};
/*
 ======================================================================
 */
void ddr_stdx11_dcm_clear_rectangle_depth (
    DtShort xmin,
    DtShort ymin,
    DtShort xmax,
    DtShort ymax)
{
   DtInt window_height;
   DtInt window_width;
   DtInt x0,y0,width,height;

#ifdef DEBUG
   printf("... processing device clear rectangle depth ...\n");
   fflush(stdout);
#endif

   if(dde_stdx11_current_device_data->simulator) {
      window_height = dde_stdx11_current_device_data->height;
      window_width = dde_stdx11_current_device_data->width;
      x0 = xmin;
      if((window_height > window_width)&&
	 ((window_height%2)^(window_width%2)))
	    y0 = window_height - ymax - 2;
      else
	    y0 = window_height - ymax - 1;
      width = xmax - xmin + 1;
      height = ymax - ymin + 1;

#ifdef DEBUG
      printf("x(%d),y(%d),w(%d),h(%d)\n",x0,y0,width,height);
#endif

      ddr_zbuffer_ClearRect(x0,y0,width,height);
   }
}
/*
 ======================================================================
 */
void ddr_stdx11_dcm_clear_rectangle_depth_and_color (
    DtShort xmin,
    DtShort ymin,
    DtShort xmax,
    DtShort ymax)
{
   DtInt window_height;
   DtInt window_width;
   DtInt x0,y0,width,height;

#ifdef DEBUG
   printf("... processing device clear depth and color ...\n");
   fflush(stdout);
#endif

   XSetForeground(dde_stdx11_current_device_data->display,
		  dde_stdx11_current_device_data->gc,
		  dde_stdx11_current_device_data->bg_color_index);

   window_height = dde_stdx11_current_device_data->height;
   window_width = dde_stdx11_current_device_data->width;
   x0 = xmin;
   width = xmax - xmin + 1;
   height = ymax - ymin + 1;
   if((window_height > window_width)&&((window_height%2)^(window_width%2)))
	 y0 = window_height - ymax - 2;
   else
	 y0 = window_height - ymax - 1;

#ifdef DEBUG
   printf("window (%d x %d)\n",window_width,window_height);
   printf("xmin(%d),ymin(%d),xmax(%d),ymax(%d)\n",xmin,ymin,xmax,ymax);
   printf("x0(%d),y0(%d),w(%d),h(%d)\n",x0,y0,width,height);
   printf("background color = %d\n", 
	  dde_stdx11_current_device_data->bg_color_index);
#endif

   XFillRectangle(dde_stdx11_current_device_data->display,
		  dde_stdx11_current_device_data->draw_buf,
		  dde_stdx11_current_device_data->gc,
		  x0,y0,width,height);

   if(dde_stdx11_current_device_data->simulator) {
      ddr_zbuffer_ClearRect(x0,y0,width,height);
   }

   XSetForeground(dde_stdx11_current_device_data->display,
		  dde_stdx11_current_device_data->gc,
		  dde_stdx11_current_device_data->fg_color_index);
}
/*
 ======================================================================
 */
void ddr_stdx11_dcm_write_scanline_byte (
    DtInt xstart,
    DtInt ystart,
    DtInt length,
    DtUChar *data)
{
   ddt_stdx11_data *device_data;
   char *pixels;
   unsigned char *byteline;
   unsigned short *shortline;
   unsigned long *longline;
   DtInt windowheight;
   int i;
   int ind;

#ifdef DEBUG
   DtReal c[3];
   printf("...processing device write scanline byte\n");
   fflush(stdout);
#endif

   device_data = dde_stdx11_current_device_data;

   ddr_stdx11_dcm_setup_shading();

   /* 
    * Figure out what pixel format to send and send it.  This could
    * be made more efficient by breaking the guts of the convert
    * routine out and doing the tests outside the loop instead of
    * inside.
    */
   if (device_data->bytes_per_pixel == 0) {
      /* XXX Need to do something with bitmaps here! */
      return;
   }
   else if (device_data->bytes_per_pixel == 1) {
#ifdef DEBUG
       printf("1 byte / pixel\n");
#endif
      byteline = (unsigned char *)DDspace_Allocate(length);
      pixels = (char *) byteline;

      switch (device_data->visualtype) {
	  case DcPseudoColor:
	     if (device_data->shade_mode == DcComponent) {
		for (i = 0; i < length; i++) {
		    ind = (((int)((DtReal)data[i*3+0]/255.*(device_data->nred-1)+0.5))
			   * device_data->rscale + 
			   ((int)((DtReal)data[i*3+1]/255.*(device_data->ngreen-1)+0.5))
			   * device_data->gscale + 
			   ((int)((DtReal)data[i*3+2]/255.*(device_data->nblue-1)+0.5)));
		    byteline[i] = device_data->pseudomap[ind]; 
#    ifdef DEBUG
		   printf ("%d: (%d %d %d) %d %d\n", i, data[i*3+0], data[i*3+1],
			   data[i*3+2], ind, byteline[i]);
#    endif
		}
	     } else {
		for (i = 0; i < length; i++) {
		   ind = device_data->cwtavgpslim[0]
			 + (device_data->cwtavgpsweight[0]*data[i*3+0] / 255.
			    + device_data->cwtavgpsweight[1]*data[i*3+1] / 255.
			    + device_data->cwtavgpsweight[2]*data[i*3+2] / 255. )
			       * device_data->cwtavgpslim[2];
		   if(ind > device_data->cwtavgpslim[1])
			 ind = device_data->cwtavgpslim[1];
		   byteline[i] = device_data->pseudomap[ind];
		}
	     }
	     break;

	  case DcTrueColor:
	     for (i = 0; i < length; i++) {
		ind = device_data->redmap[(int)(data[i*3+0])] +
		      device_data->greenmap[(int)(data[i*3+1])] +
			    device_data->bluemap[(int)(data[i*3+2])];
		byteline[i] = ind;
	     }
	     break;

	  case DcStaticGrey:
	  case DcGreyScale:
	  case DcStaticColor:
	  case DcDirectColor:
	  default:
	      break;
      }
   }
   else if (device_data->bytes_per_pixel == 2) {
      shortline = (unsigned short *)DDspace_Allocate(2*length);
      pixels = (char *) shortline;
      switch (device_data->visualtype) {
	  case DcPseudoColor:
	     if (device_data->shade_mode == DcComponent) {
		for (i = 0; i < length; i++) {
		    ind = (((int)((DtReal)data[i*3+0]/255.*(device_data->nred-1)+0.5))
			   * device_data->rscale + 
			   ((int)((DtReal)data[i*3+1]/255.*(device_data->ngreen-1)+0.5))
			   * device_data->gscale + 
			   ((int)((DtReal)data[i*3+2]/255.*(device_data->nblue-1)+0.5)));
		   shortline[i] = device_data->pseudomap[ind]; 
		}
	     } else {
		for (i = 0; i < length; i++) {
		   ind = device_data->cwtavgpslim[0]
			 + (device_data->cwtavgpsweight[0]*data[i*3+0] / 255.
			    + device_data->cwtavgpsweight[1]*data[i*3+1] / 255.
			    + device_data->cwtavgpsweight[2]*data[i*3+2] / 255. )
			       * device_data->cwtavgpslim[2];
		   if(ind > device_data->cwtavgpslim[1])
			 ind = device_data->cwtavgpslim[1];
		   shortline[i] = device_data->pseudomap[ind];
		}
	     }
	     break;

	  case DcTrueColor:
	     for (i = 0; i < length; i++) {
		ind = device_data->redmap[(int)(data[i*3+0])] +
		      device_data->greenmap[(int)(data[i*3+1])] +
			    device_data->bluemap[(int)(data[i*3+2])];
		shortline[i] = ind;
	     }
	     break;

	  case DcStaticGrey:
	  case DcGreyScale:
	  case DcStaticColor:
	  case DcDirectColor:
	  default:
	      break;
      }
   }
   else if (device_data->bytes_per_pixel == 4) {
      longline = (unsigned long *)DDspace_Allocate(4*length);
      pixels = (char *) longline;
      switch (device_data->visualtype) {
	  case DcPseudoColor:
	     if (device_data->shade_mode == DcComponent) {
		for (i = 0; i < length; i++) {
		    ind = (((int)((DtReal)data[i*3+0]/255.*(device_data->nred-1)+0.5))
			   * device_data->rscale + 
			   ((int)((DtReal)data[i*3+1]/255.*(device_data->ngreen-1)+0.5))
			   * device_data->gscale + 
			   ((int)((DtReal)data[i*3+2]/255.*(device_data->nblue-1)+0.5)));
		   longline[i] = device_data->pseudomap[ind]; 
		}
	     } else {
		for (i = 0; i < length; i++) {
		   ind = device_data->cwtavgpslim[0]
			 + (device_data->cwtavgpsweight[0]*data[i*3+0] / 255.
			    + device_data->cwtavgpsweight[1]*data[i*3+1] / 255.
			    + device_data->cwtavgpsweight[2]*data[i*3+2] / 255. )
			       * device_data->cwtavgpslim[2];
		   if(ind > device_data->cwtavgpslim[1])
			 ind = device_data->cwtavgpslim[1];
		   longline[i] = device_data->pseudomap[ind];
		}
	     }
	     break;

	  case DcTrueColor:
	     for (i = 0; i < length; i++) {
		ind = device_data->redmap[(int)(data[i*3+0])] +
		      device_data->greenmap[(int)(data[i*3+1])] +
			    device_data->bluemap[(int)(data[i*3+2])];
		longline[i] = ind;
	     }
	     break;

	  case DcStaticGrey:
	  case DcGreyScale:
	  case DcStaticColor:
	  case DcDirectColor:
	  default:
	      break;
      }
   }
	
   windowheight = device_data->height + 1;
   ystart = windowheight - ystart;

   device_data->image->width = length;
   device_data->image->height = 1;
   device_data->image->data = (char *)pixels;

   if(dde_stdx11_current_device_data->doubleBuffered) {
       XPutImage(device_data->display,device_data->draw_buf,device_data->gc,
		 device_data->image,0,0,xstart,ystart,length,1);
   } else {
       XPutImage(device_data->display,device_data->window,device_data->gc,
		 device_data->image,0,0,xstart,ystart,length,1);
   }
       
   DDspace_Deallocate(pixels);
}
/*
 ======================================================================
 */
void ddr_stdx11_DrawPoint (
    DtInt *point,
    DtColorRGB color)
{
#ifdef DEBUG
   printf("...processing ddr_stdx11_DrawPoint\n");
   fflush(stdout);
#endif

   point[1] = dde_stdx11_current_device_data->height + 1 - point[1];

   if(dde_stdx11_current_device_data->simulator &&
      dde_stdx11_current_device_data->dbf_enable) {
      ddr_zbuffer_DrawPoint(point,color);
   } else {
      ddr_stdx11_dcm_set_foreground_color(DcRGB, color);

      XDrawPoint(dde_stdx11_current_device_data->display,
		 dde_stdx11_current_device_data->draw_buf,
		 dde_stdx11_current_device_data->gc,
		 point[0],point[1]);
   }
}
/*
 ======================================================================
 */
void ddr_stdx11_DrawLine (
    DtInt *point1,
    DtInt *point2,
    DtColorRGB color1,
    DtColorRGB color2)
{
   DtColorRGB color;

#ifdef DEBUG
   printf("...processing ddr_stdx11_DrawLine\n");
   fflush(stdout);
#endif

   point1[1] = dde_stdx11_current_device_data->height + 1 - point1[1];
   point2[1] = dde_stdx11_current_device_data->height + 1 - point2[1];

   if(dde_stdx11_current_device_data->simulator &&
      dde_stdx11_current_device_data->dbf_enable) {
      ddr_zbuffer_DrawLine(point1,point2,color1,color2);
   }
   else {
      color[0] = (color1[0]+color2[0])/2.;
      color[1] = (color1[1]+color2[1])/2.;
      color[2] = (color1[2]+color2[2])/2.;
      ddr_stdx11_dcm_set_foreground_color(DcRGB, color);

      XDrawLine(dde_stdx11_current_device_data->display,
		dde_stdx11_current_device_data->draw_buf,
		dde_stdx11_current_device_data->gc,
		point1[0],point1[1],point2[0],point2[1]);
   }
}
/*
 ======================================================================
 */
void ddr_stdx11_DrawTriangle (
    DtInt *point1,
    DtInt *point2,
    DtInt *point3,
    DtColorRGB color1,
    DtColorRGB color2,
    DtColorRGB color3)
{
   XPoint points[3];
   DtColorRGB color;

#ifdef DEBUG
   printf("...processing ddr_stdx11_DrawTriangle\n");
   fflush(stdout);
#endif

   point1[1] = dde_stdx11_current_device_data->height + 1 - point1[1];
   point2[1] = dde_stdx11_current_device_data->height + 1 - point2[1];
   point3[1] = dde_stdx11_current_device_data->height + 1 - point3[1];

   if(dde_stdx11_current_device_data->simulator &&
      dde_stdx11_current_device_data->dbf_enable) {
#ifdef DEBUG
      printf("points: (%lf,%lf,%lf)-(%lf,%lf,%lf)-(%lf,%lf,%lf)\n",
	     point1[0],point1[1],point1[2],
	     point2[0],point2[1],point2[2],
	     point3[0],point3[1],point3[2]);
      printf("color: (%lf,%lf,%lf)-(%lf,%lf,%lf)-(%lf,%lf,%lf)\n",
	     color1[0],color1[1],color1[2],
	     color2[0],color2[1],color2[2],
	     color3[0],color3[1],color3[2]);
#endif
      ddr_zbuffer_DrawTriangle(point1,point2,point3, color1,color2,color3);
   }
   else {
      points[0].x = (short)point1[0];
      points[0].y = (short)point1[1];
      points[1].x = (short)point2[0];
      points[1].y = (short)point2[1];
      points[2].x = (short)point3[0];
      points[2].y = (short)point3[1];

      color[0] = (color1[0]+color2[0]+color3[0])/3.;
      color[1] = (color1[1]+color2[1]+color3[1])/3.;
      color[2] = (color1[2]+color2[2]+color3[2])/3.;
      ddr_stdx11_dcm_set_foreground_color(DcRGB, color);

      XFillPolygon(dde_stdx11_current_device_data->display,
		   dde_stdx11_current_device_data->draw_buf,
		   dde_stdx11_current_device_data->gc,
		   points,3,Convex,CoordModeOrigin);
   }
}
/*
 ======================================================================
 */
void ddr_stdx11_PutPixel (
    DtInt x,
    DtInt y,
    DtColorRGB color)
{
#ifdef DEBUG
   printf("...processing ddr_stdx11_PutPixel\n");
   fflush(stdout);
#endif

   if(dde_stdx11_current_device_data->visualtype == DcStaticGrey) {
      ddr_stdx11_Dither(x, y,color);
   } else {
      if (dde_stdx11_current_device_data->dither) {
	 ddr_stdx11_DitherColor(x, y,color);
      } else {
	 ddr_stdx11_dcm_set_foreground_color(DcRGB, color);
      }
   }
   XDrawPoint(dde_stdx11_current_device_data->display,
	      dde_stdx11_current_device_data->draw_buf,
	      dde_stdx11_current_device_data->gc,
	      x, y);
}
/*
 ======================================================================
 */
void ddr_stdx11_Dither (
    DtInt x,
    DtInt y,
    DtReal *color)
{
   ddt_stdx11_data *device_data;
   int intensity, pixel;

#ifdef DEBUG
   printf("...processing ddr_stdx11_Dither\n");
   fflush(stdout);
#endif

   device_data = dde_stdx11_current_device_data;

   if(!(device_data->simulator))
	 return;

   intensity = (color[0]*255*3+color[1]*255*6+color[2])/10;
   intensity = (intensity>255)?255:intensity;
   pixel = intensity / (255 / (DITHER_SIZE * DITHER_SIZE - 1))
	 >= dither_table [x % DITHER_SIZE] [y % DITHER_SIZE];
   pixel = pixel? device_data->fg_color_index: device_data->bg_color_index;
   XSetForeground(device_data->display,device_data->gc, pixel);
}

/* ====================================================================== */

void ddr_stdx11_DitherColor (
   DtInt x,
   DtInt y,
   DtReal *color)
{
   ddt_stdx11_data *dev;
   int pixel;
   int mred, mgreen, mblue;
   float rv, gv, bv;
   int irv, igv, ibv;
   float dith;

#  ifdef DEBUG
      printf ("...processing ddr_stdx11_DitherColor\n");
      fflush (stdout);
#  endif

   dev = dde_stdx11_current_device_data;

   if (!(dev->simulator))
      return;

   /* Get the maximum red, green and blue values that we can have. */

   mred   = dev->nred - 1;
   mgreen = dev->ngreen - 1;
   mblue  = dev->nblue - 1;

   x = (x >= 0) ? (x % 4) : (4 - ((-x) % 4));
   y = (y >= 0) ? (y % 4) : (4 - ((-y) % 4));

   dith = dither_matrix[x][y];
   rv = color[0] * mred;
   irv = (int) rv;
   if (rv - (float)irv > dith && irv != mred) irv++;

   gv = color[1] * mgreen;
   igv = (int) gv;
   if (gv - (float)igv > dith && igv != mgreen) igv++;

   bv = color[2] * mblue;
   ibv = (int) bv;
   if (bv - (float)ibv > dith && ibv != mblue) ibv++;

   if (dev->visualtype == DcPseudoColor) {
      pixel = irv * dev->rscale + igv * dev->gscale + ibv;
      pixel = dev->pseudomap[pixel];

   } else {  /* Some true color systems may be 16, 12 or 8 bit. */
      pixel = dev->redmap  [(int)(irv * (255.0 / (float)mred  ))]
	    + dev->greenmap[(int)(igv * (255.0 / (float)mgreen))]
	    + dev->bluemap [(int)(ibv * (255.0 / (float)mblue ))];
   }

   if (dev->fg_color_index != pixel) {
      dev->fg_color_index = pixel;
      XSetForeground (dev->display,dev->gc, pixel);
   }
}
