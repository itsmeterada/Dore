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
	void ddr_sunview_gen_dcm_clear_rectangle_depth (xmin, ymin, xmax, ymax)
	void ddr_sunview_gen_dcm_clear_rectangle_depth_and_color (xmin, ymin, xmax, ymax)
	void ddr_sunview_gen_dcm_write_scanline_byte (xstart, ystart, length, data)
	void ddr_sunview_gen_DrawPoint_back (point,color)
	void ddr_sunview_gen_DrawLine_back (point1,point2,color1,color2)
	void ddr_sunview_gen_DrawTriangle_back (point1,point2,point3,color1,color2,color3)
	void ddr_sunview_gen_PutPixel_back (x,y,color)
	void ddr_sunview_gen_DrawPoint_front (point,color)
	void ddr_sunview_gen_DrawLine_front (point1,point2,color1,color2)
	void ddr_sunview_gen_DrawTriangle_front (point1,point2,point3,color1,color2,color3)
	void ddr_sunview_gen_PutPixel_front (x,y,color)
	void ddr_sunview_gen_Dither (x,y,color)
	void ddr_sunview_gen_DitherColor (x,y,color)

  ======================================================================
 */

#include "dcm.h"

#define DITHER_SIZE 4

static int dither_table[DITHER_SIZE][DITHER_SIZE] = {
    0,  8,  2, 10, 
   12,  4, 14,  6, 
    3, 11,  1,  9, 
   15,  7, 13,  5
};

static float dither_matrix[4][4] = {
   0.0,    0.5,    0.125,  0.625,
   0.75,   0.25,   0.875,  0.375,
   0.1875, 0.6875, 0.0625, 0.5625,
   0.9375, 0.4375, 0.8125, 0.3125
};


/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_clear_rectangle_depth (xmin, ymin, xmax, ymax)
DtShort xmin;
DtShort ymin;
DtShort xmax;
DtShort ymax;
{
   DtInt window_height;
   DtInt window_width;
   DtInt x0,y0,width,height;

   if(dde_sunview_gen_current_device_data->simulator) {
      window_height = dde_sunview_gen_current_device_data->height;
      window_width = dde_sunview_gen_current_device_data->width;
      x0 = xmin;
      if((window_height > window_width)&&
	 ((window_height%2)^(window_width%2)))
	    y0 = window_height - ymax - 2;
      else
	    y0 = window_height - ymax - 1;
      width = xmax - xmin + 1;
      height = ymax - ymin + 1;

      ddr_zbuffer_ClearRect(x0,y0,width,height);
   }
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_clear_rectangle_depth_and_color (xmin, ymin, xmax, ymax)
DtShort xmin;
DtShort xmax;
DtShort ymin;
DtShort ymax;
{
   ddt_sunview_gen_data *device_data;
   DtInt window_height;
   DtInt window_width;
   DtInt x0,y0,width,height;

   device_data = dde_sunview_gen_current_device_data;

   window_height = device_data->height;
   window_width = device_data->width;
   x0 = xmin;
   width = xmax - xmin + 1;
   height = ymax - ymin + 1;
   if((window_height > window_width)&&((window_height%2)^(window_width%2)))
	 y0 = window_height - ymax - 2;
   else
	 y0 = window_height - ymax - 1;


   if (device_data->doubleBuffered) {
      pr_rop (device_data->back_buf, x0, y0, width, height, 
	      PIX_SRC | PIX_COLOR(device_data->bg_color_index), NULL, 0, 0);
   } else {
      pw_rop (device_data->pw, x0, y0, width, height, 
	      PIX_SRC | PIX_COLOR(device_data->bg_color_index), NULL, 0, 0);
   }

   if(device_data->simulator) {
      ddr_zbuffer_ClearRect(x0,y0,width,height);
   }
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_write_scanline_byte (xstart, ystart, length, data)
DtInt xstart;
DtInt ystart;
DtInt length;
DtUChar *data;
{
   static struct pixrect *pr = NULL;
   static int pr_l = 0;
   DtInt windowheight;
   u_char *loc;
   DtInt index;
   int i;

   windowheight = dde_sunview_gen_current_device_data->height + 1;

   ystart = windowheight - ystart;

   if (pr_l == 0 || pr_l < length) {
      if (pr != NULL) pr_destroy (pr);
      pr = mem_create (length, 1, 8);
      pr_l = length;
   }

   ddr_sunview_gen_dcm_setup_shading();

   if (dde_sunview_gen_current_device_data->shade_mode == DcComponent) {
      for (i=0; i<length; i++) {
	 loc = mprd8_addr(mpr_d(pr), i, 0, 8);

	 index = ((int)(dde_sunview_gen_current_device_data->cbtselpsweight[0]*data[i*3+0]) 
		  & dde_sunview_gen_current_device_data->cbtselpsmask[0]) + 
	         ((int)(dde_sunview_gen_current_device_data->cbtselpsweight[1]*data[i*3+1]) 
		  & dde_sunview_gen_current_device_data->cbtselpsmask[1]) +
	         ((int)(dde_sunview_gen_current_device_data->cbtselpsweight[2]*data[i*3+2])
		  & dde_sunview_gen_current_device_data->cbtselpsmask[2]);
	 index = index < 0 ? 0 : index > 255 ? 255 : index;
	 *loc = (u_char)index;
      }
   } else if (dde_sunview_gen_current_device_data->shade_mode == DcRange) {
      for (i=0; i<length; i++) {
	 loc = mprd8_addr(mpr_d(pr), i, 0, 8);

	 index = dde_sunview_gen_current_device_data->cwtavgpslim[0] +
	       (dde_sunview_gen_current_device_data->cwtavgpsweight[0]*data[i*3+0] +
		dde_sunview_gen_current_device_data->cwtavgpsweight[1]*data[i*3+1] +
		dde_sunview_gen_current_device_data->cwtavgpsweight[2]*data[i*3+2])*
		      dde_sunview_gen_current_device_data->cwtavgpslim[2];
	 if (index > dde_sunview_gen_current_device_data->cwtavgpslim[1]) 
	       index = dde_sunview_gen_current_device_data->cwtavgpslim[1];
	 index = index < 0 ? 0 : index > 255 ? 255 : index;
	 *loc = (u_char)index;
      }
   }

   if (dde_sunview_gen_current_device_data->doubleBuffered) {
      pr_rop (dde_sunview_gen_current_device_data->back_buf, xstart, ystart, 
	      length, 1, PIX_SRC, pr, 0, 0);
   } else {
      pw_rop (dde_sunview_gen_current_device_data->pw, xstart, ystart, 
	      length, 1, PIX_SRC, pr, 0, 0);
   }
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_DrawPoint_back(point,color)
DtInt *point;
DtColorRGB color;
{
   point[1] = dde_sunview_gen_current_device_data->height + 1 - point[1];

   if(dde_sunview_gen_current_device_data->simulator &&
      dde_sunview_gen_current_device_data->dbf_enable &&
      dde_sunview_gen_current_device_data->dbf_write) {
      ddr_zbuffer_DrawPoint(point,color);
   } else {
      ddr_sunview_gen_dcm_set_foreground_color(DcRGB,color);

      pr_put(dde_sunview_gen_current_device_data->back_buf,
	     point[0], point[1], 
	     dde_sunview_gen_current_device_data->draw_index);
   }
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_DrawLine_back(point1,point2,color1,color2)
DtInt *point1;
DtInt *point2;
DtColorRGB color1;
DtColorRGB color2;
{
   DtColorRGB color;

   point1[1] = dde_sunview_gen_current_device_data->height + 1 - point1[1];
   point2[1] = dde_sunview_gen_current_device_data->height + 1 - point2[1];

   if(dde_sunview_gen_current_device_data->simulator &&
      dde_sunview_gen_current_device_data->dbf_enable &&
      dde_sunview_gen_current_device_data->dbf_write) {
      ddr_zbuffer_DrawLine(point1,point2,color1,color2);
   }
   else {
      color[0] = (color1[0]+color2[0])/2.;
      color[1] = (color1[1]+color2[1])/2.;
      color[2] = (color1[2]+color2[2])/2.;
      ddr_sunview_gen_dcm_set_foreground_color(DcRGB,color);

      pr_vector(dde_sunview_gen_current_device_data->back_buf,
		point1[0], point1[1], point2[0], point2[1], PIX_SRC,
		dde_sunview_gen_current_device_data->draw_index);
   }
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_DrawTriangle_back(point1,point2,point3,color1,color2,color3)
DtInt *point1;
DtInt *point2;
DtInt *point3;
DtColorRGB color1;
DtColorRGB color2;
DtColorRGB color3;
{
   struct pr_pos points[3];
   DtColorRGB color;
   int npnts[2];

   point1[1] = dde_sunview_gen_current_device_data->height + 1 - point1[1];
   point2[1] = dde_sunview_gen_current_device_data->height + 1 - point2[1];
   point3[1] = dde_sunview_gen_current_device_data->height + 1 - point3[1];

   if(dde_sunview_gen_current_device_data->simulator &&
      dde_sunview_gen_current_device_data->dbf_enable &&
      dde_sunview_gen_current_device_data->dbf_write) {
      ddr_zbuffer_DrawTriangle(point1,point2,point3, color1,color2,color3);
   }
   else {
      points[0].x = (short)point1[0];
      points[0].y = (short)point1[1];
      points[1].x = (short)point2[0];
      points[1].y = (short)point2[1];
      points[2].x = (short)point3[0];
      points[2].y = (short)point3[1];
      npnts[0] = 3;

      color[0] = (color1[0]+color2[0]+color3[0])/3.;
      color[1] = (color1[1]+color2[1]+color3[1])/3.;
      color[2] = (color1[2]+color2[2]+color3[2])/3.;
      ddr_sunview_gen_dcm_set_foreground_color(DcRGB,color);

      pr_polygon_2(dde_sunview_gen_current_device_data->back_buf,
		   0, 0, 1, npnts, points,
		   PIX_SRC | PIX_COLOR(dde_sunview_gen_current_device_data->draw_index),
		   NULL, 0, 0);
   }
}
/*
 ======================================================================
 */
void ddr_sunview_gen_PutPixel_back (x,y,color)
DtInt x;
DtInt y;
DtColorRGB color;
{
   if(dde_sunview_gen_current_device_data->visualtype == DcStaticGrey) {
      ddr_sunview_gen_Dither(x, y,color);
   } else {
      if (dde_sunview_gen_current_device_data->dither) {
	 ddr_sunview_gen_DitherColor(x, y,color);
      } else {
	 ddr_sunview_gen_dcm_set_foreground_color(DcRGB,color);
      }
   }
   pr_put(dde_sunview_gen_current_device_data->back_buf, x, y, 
	  dde_sunview_gen_current_device_data->draw_index);
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_DrawPoint_front(point,color)
DtInt *point;
DtColorRGB color;
{
   point[1] = dde_sunview_gen_current_device_data->height + 1 - point[1];

   if(dde_sunview_gen_current_device_data->simulator &&
      dde_sunview_gen_current_device_data->dbf_enable &&
      dde_sunview_gen_current_device_data->dbf_write) {
      ddr_zbuffer_DrawPoint(point,color);
   } else {
      ddr_sunview_gen_dcm_set_foreground_color(DcRGB,color);
      pw_put(dde_sunview_gen_current_device_data->pw,
	     point[0], point[1], 
	     dde_sunview_gen_current_device_data->draw_index);
   }
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_DrawLine_front(point1,point2,color1,color2)
DtInt *point1;
DtInt *point2;
DtColorRGB color1;
DtColorRGB color2;
{
   DtColorRGB color;

   point1[1] = dde_sunview_gen_current_device_data->height + 1 - point1[1];
   point2[1] = dde_sunview_gen_current_device_data->height + 1 - point2[1];

   if(dde_sunview_gen_current_device_data->simulator &&
      dde_sunview_gen_current_device_data->dbf_enable &&
      dde_sunview_gen_current_device_data->dbf_write) {
      ddr_zbuffer_DrawLine(point1,point2,color1,color2);
   }
   else {
      color[0] = (color1[0]+color2[0])/2.;
      color[1] = (color1[1]+color2[1])/2.;
      color[2] = (color1[2]+color2[2])/2.;
      ddr_sunview_gen_dcm_set_foreground_color(DcRGB,color);
      pw_vector(dde_sunview_gen_current_device_data->pw,
		point1[0], point1[1], point2[0], point2[1], PIX_SRC,
		dde_sunview_gen_current_device_data->draw_index);
   }
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_DrawTriangle_front(point1,point2,point3,color1,color2,color3)
      DtInt *point1;
DtInt *point2;
DtInt *point3;
DtColorRGB color1;
DtColorRGB color2;
DtColorRGB color3;
{
   struct pr_pos points[3];
   DtColorRGB color;
   int npnts[2];

   point1[1] = dde_sunview_gen_current_device_data->height + 1 - point1[1];
   point2[1] = dde_sunview_gen_current_device_data->height + 1 - point2[1];
   point3[1] = dde_sunview_gen_current_device_data->height + 1 - point3[1];

   if(dde_sunview_gen_current_device_data->simulator &&
      dde_sunview_gen_current_device_data->dbf_enable &&
      dde_sunview_gen_current_device_data->dbf_write) {
      ddr_zbuffer_DrawTriangle(point1,point2,point3, color1,color2,color3);
   }
   else {
      points[0].x = (short)point1[0];
      points[0].y = (short)point1[1];
      points[1].x = (short)point2[0];
      points[1].y = (short)point2[1];
      points[2].x = (short)point3[0];
      points[2].y = (short)point3[1];
      npnts[0] = 3;

      color[0] = (color1[0]+color2[0]+color3[0])/3.;
      color[1] = (color1[1]+color2[1]+color3[1])/3.;
      color[2] = (color1[2]+color2[2]+color3[2])/3.;
      ddr_sunview_gen_dcm_set_foreground_color(DcRGB,color);

      pw_polygon_2(dde_sunview_gen_current_device_data->pw,
		   0, 0, 1, npnts, points,
		   PIX_SRC | PIX_COLOR(dde_sunview_gen_current_device_data->draw_index),
		   NULL, 0, 0);
   }
}
/*
 ======================================================================
 */
void ddr_sunview_gen_PutPixel_front (x,y,color)
DtInt x;
DtInt y;
DtColorRGB color;
{
   if(dde_sunview_gen_current_device_data->visualtype == DcStaticGrey) {
      ddr_sunview_gen_Dither(x, y,color);
   } else {
      if (dde_sunview_gen_current_device_data->dither) {
	 ddr_sunview_gen_DitherColor(x, y,color);
      } else {
	 ddr_sunview_gen_dcm_set_foreground_color(DcRGB,color);
      }
   }
   pw_put(dde_sunview_gen_current_device_data->pw, x, y, 
	  dde_sunview_gen_current_device_data->draw_index);
}
/*
 ======================================================================
 */
void ddr_sunview_gen_Dither (x,y,color)
DtInt x, y;
DtReal *color;
{
   ddt_sunview_gen_data *device_data;
   int intensity, pixel;

   device_data = dde_sunview_gen_current_device_data;

   if(!(device_data->simulator))
	 return;

   intensity = (color[0]*255*3+color[1]*255*6+color[2])/10;
   intensity = (intensity>255)?255:intensity;
   pixel = intensity/(255/(DITHER_SIZE*DITHER_SIZE-1))
	 >= dither_table[x%DITHER_SIZE][y%DITHER_SIZE];
   pixel = pixel? device_data->fg_color_index: device_data->bg_color_index;

   device_data->draw_index = pixel;
}
/*
 ======================================================================
 */
void ddr_sunview_gen_DitherColor (x,y,color)
DtInt x, y;
DtReal *color;
{
   ddt_sunview_gen_data *dev;
   int pixel;
   int mred, mgreen, mblue;
   float rv, gv, bv;
   int irv, igv, ibv;
   float dith;

   dev = dde_sunview_gen_current_device_data;

   if(!(dev->simulator))
	 return;

   /* Get the maximum red, green and blue values that we can have */
   mred = dev->nred - 1;
   mgreen = dev->ngreen - 1;
   mblue = dev->nblue - 1;

   dith = dither_matrix[x%4][y%4];
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

      if (dev->fg_color_index != pixel) {
	 dev->fg_color_index = pixel;
	 dev->draw_index = pixel;
      }
   }
}
