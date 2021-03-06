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
	void ddr_sunview_gen_dcm_set_background_color (colormodel,c)
	void ddr_sunview_gen_dcm_set_foreground_color (colormodel,c)
	void ddr_sunview_gen_dcm_set_color_entries (colormodel,start, n, c)
	void ddr_sunview_gen_dcm_set_current_view (view)
	void ddr_sunview_gen_dcm_set_depth_buffer_enable (flag)
	void ddr_sunview_gen_dcm_set_depth_buffer_write (flag)
	void ddr_sunview_gen_dcm_set_shade_index (index)

  ======================================================================
 */
#include "dcm.h"

/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_set_background_color (colormodel,c)
DtColorModel colormodel;
DtReal *c;
{
   DtInt index;

   if (dde_sunview_gen_current_device_data->visualtype == DcPseudoColor) {

      ddr_sunview_gen_dcm_setup_shading();

      if (dde_sunview_gen_current_device_data->shade_mode == DcComponent) {
	 index =  ((int)(dde_sunview_gen_current_device_data->cbtselpsweight[0]*c[0]*255.)
		   & dde_sunview_gen_current_device_data->cbtselpsmask[0]) + 
			 ((int)(dde_sunview_gen_current_device_data->cbtselpsweight[1]*c[1]*255.)
			  & dde_sunview_gen_current_device_data->cbtselpsmask[1]) +
				((int)(dde_sunview_gen_current_device_data->cbtselpsweight[2]*c[2]*255.)
				 & dde_sunview_gen_current_device_data->cbtselpsmask[2]);
	 index = index < 0 ? 0 : index > 255 ? 255 : index;
      } else if (dde_sunview_gen_current_device_data->shade_mode == DcRange) {
	 index = dde_sunview_gen_current_device_data->cwtavgpslim[0] +
	       (dde_sunview_gen_current_device_data->cwtavgpsweight[0]*c[0]*255. +
		dde_sunview_gen_current_device_data->cwtavgpsweight[1]*c[1]*255. +
		dde_sunview_gen_current_device_data->cwtavgpsweight[2]*c[2]*255.)*
		      dde_sunview_gen_current_device_data->cwtavgpslim[2];
	 if (index > dde_sunview_gen_current_device_data->cwtavgpslim[1]) 
	       index = dde_sunview_gen_current_device_data->cwtavgpslim[1];
	 index = index < 0 ? 0 : index > 255 ? 255 : index;
      }

      dde_sunview_gen_current_device_data->bg_color_index = (short)index;
   }
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_set_foreground_color (colormodel,c)
DtColorModel colormodel;
DtReal *c;
{
   DtInt index;

   if (dde_sunview_gen_current_device_data->visualtype == DcPseudoColor) {
      ddr_sunview_gen_dcm_setup_shading();

      if (dde_sunview_gen_current_device_data->shade_mode == DcComponent) {
	 index =  ((int)(dde_sunview_gen_current_device_data->cbtselpsweight[0]*c[0]*255.)
		   & dde_sunview_gen_current_device_data->cbtselpsmask[0]) + 
			 ((int)(dde_sunview_gen_current_device_data->cbtselpsweight[1]*c[1]*255.)
			  & dde_sunview_gen_current_device_data->cbtselpsmask[1]) +
				((int)(dde_sunview_gen_current_device_data->cbtselpsweight[2]*c[2]*255.)
				 & dde_sunview_gen_current_device_data->cbtselpsmask[2]);
	 index = index < 0 ? 0 : index > 255 ? 255 : index;
      } else if (dde_sunview_gen_current_device_data->shade_mode == DcRange) {
	 index = dde_sunview_gen_current_device_data->cwtavgpslim[0] +
	       (dde_sunview_gen_current_device_data->cwtavgpsweight[0]*c[0]*255. +
		dde_sunview_gen_current_device_data->cwtavgpsweight[1]*c[1]*255. +
		dde_sunview_gen_current_device_data->cwtavgpsweight[2]*c[2]*255.)*
		      dde_sunview_gen_current_device_data->cwtavgpslim[2];
	 if (index > dde_sunview_gen_current_device_data->cwtavgpslim[1])
	       index = dde_sunview_gen_current_device_data->cwtavgpslim[1];
	 index = index < 0 ? 0 : index > 255 ? 255 : index;
      }
      dde_sunview_gen_current_device_data->fg_color_index = (short)index;
      dde_sunview_gen_current_device_data->draw_index = (short)index;
   } else if (dde_sunview_gen_current_device_data->visualtype == DcStaticGrey) {
      dde_sunview_gen_current_device_data->draw_index = 
	    dde_sunview_gen_current_device_data->fg_color_index;
   }
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_set_color_entries (colormodel,start, n, c)
DtColorModel colormodel;
int start;
int n;
DtReal *c;
{
   static unsigned char r[256], g[256], b[256];
   int i;
   int k;

   if (dde_sunview_gen_current_device_data->visualtype == DcPseudoColor) {

      for (i=0; i<n; i++) {
	 r[i] = (k=256 * c[i*3+0]) > 255 ? 255 : k < 0 ? 0 : k;
	 g[i] = (k=256 * c[i*3+1]) > 255 ? 255 : k < 0 ? 0 : k;
	 b[i] = (k=256 * c[i*3+2]) > 255 ? 255 : k < 0 ? 0 : k;
      }

      pw_putcolormap (dde_sunview_gen_current_device_data->pw, start, 
		      n, r, g, b);

   }
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_set_current_view (view)
     DtObject view;
{
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_set_depth_buffer_enable (flag)
DtFlag flag;
{
   dde_sunview_gen_current_device_data->dbf_enable = flag;
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_set_depth_buffer_write (flag)
DtFlag flag;
{
   dde_sunview_gen_current_device_data->dbf_write = flag;
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_set_shade_index (index)
DtInt index;
{
   dde_sunview_gen_current_device_data->shade_index = index;
}
