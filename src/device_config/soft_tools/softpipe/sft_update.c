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
	void ddr_softpipe_drom_pre_initialization ()
	void ddr_softpipe_drom_post_initialization ()
	void ddr_softpipe_drom_update_local_data ()
	void ddr_softpipe_drom_start_update (device,view,device_data,view_data,window_data)

  ======================================================================
 */
#include "drom.h"
#include <dore/internal/matrix.h>

/*
 ======================================================================
 */
void ddr_softpipe_drom_pre_initialization (void)
{
#ifdef DEBUG
   printf("...processing dynout pre initialization \n");
   fflush(stdout);
#endif

   dde_softpipe_drom_pre_init = DcTrue;

   dde_softpipe_drom_data.colored_elements = NULL;
   dde_softpipe_drom_data.colored_element_count = 0;

   dde_softpipe_drom_data.color_indices = NULL;
   dde_softpipe_drom_data.color_indices_count = 0;

   dde_softpipe_drom_data.backfacing_flags = NULL;
   dde_softpipe_drom_data.backfacing_flags_count = 0;

   dde_softpipe_drom_data.xformed_vtxlocs = NULL;
   dde_softpipe_drom_data.xformed_vtxlocs_count = 0;

   dde_softpipe_drom_data.trimsh_unfold_vtx = NULL;
   dde_softpipe_drom_data.trimsh_unfold_vtx_cnt = 0;

   dde_softpipe_drom_data.trimsh_unfold_clr = NULL;
   dde_softpipe_drom_data.trimsh_unfold_clr_cnt = 0;

   dde_softpipe_drom_data.trimsh_unfold_nrm = NULL;
   dde_softpipe_drom_data.trimsh_unfold_nrm_cnt = 0;

   ddr_softpipe_drom_initialize_specular_table();

   dde_softpipe_drom_data.current_device = NULL;
   dde_softpipe_drom_data.current_view = NULL;

   dde_softpipe_drom_curdevdat = NULL;
   dde_softpipe_drom_curviwdat = NULL;
   dde_softpipe_drom_curwindat = NULL;
   dde_softpipe_drom_curcamdat = NULL;
   dde_softpipe_drom_curlgtdat = NULL;

   dde_softpipe_drom_data.mat_stack.current_element = -1;
   dde_softpipe_drom_data.mat_stack.last_element = 9;
   dde_softpipe_drom_data.mat_stack.elements = 
	 (struct mat_element *)DDspace_Allocate(10*sizeof(struct mat_element));
   dor_matrix_load_identity(dde_softpipe_drom_data.object_lcstowcsmat);
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_post_initialization (void)
{
#ifdef DEBUG
   printf("...processing dynout post initialization \n");
   fflush(stdout);
#endif

   dde_softpipe_drom_pre_init = DcFalse;

   dor_matrix_load_identity(dde_softpipe_drom_data.nrmwcstolcsmat);
   dor_matrix_transpose(dde_softpipe_drom_data.nrmwcstolcsmat);
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_update_local_data (void)
{
   DtVolume *actviewport;
   DtReal actviewxmin, actviewymin,actviewxmax,actviewymax;
   DtInt rno;
   DtInt cnt;
   DtUInt cliprectcount;
   DtReal *cliprects;
   DtReal windowxmin,windowymin,windowxmax,windowymax;
   DtInt shaderange[2];
   DtReal extent[4];
   DtFlag partiallyobscuredflag;
   DtReal ftodtrans[3];

#ifdef DEBUG
   printf("...processing dynout update local data\n");
   fflush(stdout);
#endif

   actviewport = DDdevice_InqActualViewport(dde_softpipe_drom_data.current_device);

   actviewxmin = actviewport->bll[0];
   actviewymin = actviewport->bll[1];
   actviewxmax = actviewport->fur[0];
   actviewymax = actviewport->fur[1];

   dde_softpipe_drom_curdevdat->stereoswitch = 
	   DDdevice_InqStereo(dde_softpipe_drom_data.current_device);

   DDdevice_InqClipList (dde_softpipe_drom_data.current_device,
			 &cliprectcount, &cliprects,
			 extent, &partiallyobscuredflag);

   for(rno=0,cnt=0; cnt<cliprectcount; cnt++) {
      windowxmin = (cliprects[cnt*4+0]);
      windowymin = (cliprects[cnt*4+1]);
      windowxmax = (cliprects[cnt*4+2]);
      windowymax = (cliprects[cnt*4+3]);

      if((windowxmin > actviewxmax) || (windowxmax < actviewxmin) ||
	 (windowymin > actviewymax) || (windowymax < actviewymin)) {
	 continue;
      }
      else {
	 windowxmin = max(windowxmin,actviewxmin);
	 windowymin = max(windowymin,actviewymin);
	 windowxmax = min(windowxmax,actviewxmax);
	 windowymax = min(windowymax,actviewymax);
      }

      (dde_softpipe_drom_curwindat->clipping_rectangles[rno]).xmin
	    = windowxmin;
      (dde_softpipe_drom_curwindat->clipping_rectangles[rno]).ymin
	    = windowymin;
      (dde_softpipe_drom_curwindat->clipping_rectangles[rno]).xmax
	    = windowxmax;
      (dde_softpipe_drom_curwindat->clipping_rectangles[rno]).ymax
	    = windowymax;

      rno++;
   }

   dde_softpipe_drom_curwindat->clipping_rectangle_count = rno;

   if(dde_softpipe_drom_curwindat->clipping_rectangle_count == 0) {
      dde_softpipe_drom_curwindat->visibility_state = 
	    ddd_softpipe_drom_window_completely_obscured;
   }
   else if(!partiallyobscuredflag) {
      dde_softpipe_drom_curwindat->visibility_state = 
	    ddd_softpipe_drom_window_completely_visible;
   }
   else {
      dde_softpipe_drom_curwindat->visibility_state = 
	    ddd_softpipe_drom_window_partially_obscured;
   }

   DdInqShadeRanges(dde_softpipe_drom_data.current_device,
			      dde_softpipe_drom_data.object_shade_index,
			      1,shaderange);

   dde_softpipe_drom_curdevdat->current_shade_range_min = 
	 (DtReal)shaderange[0];
   dde_softpipe_drom_curdevdat->current_shade_range_max = 
	 (DtReal)shaderange[1];

   DDdevice_InqFrustumScaleTrans(dde_softpipe_drom_data.current_device,
			      dde_softpipe_drom_curdevdat->ctodscale,
			      dde_softpipe_drom_curdevdat->ctodtranslate);

   DDdevice_InqFrameScaleTrans(dde_softpipe_drom_data.current_device,
			       dde_softpipe_drom_curdevdat->ftodscale,
			       ftodtrans);

   dde_softpipe_drom_curdevdat->annoscale[0] = 
	 dde_softpipe_drom_curdevdat->ftodscale[0]
	       / dde_softpipe_drom_curdevdat->ctodscale[0];
   dde_softpipe_drom_curdevdat->annoscale[1] = 
	 dde_softpipe_drom_curdevdat->ftodscale[1]
	       / dde_softpipe_drom_curdevdat->ctodscale[1];
   dde_softpipe_drom_curdevdat->annoscale[2] = 	
	 dde_softpipe_drom_curdevdat->ftodscale[2]
	       / dde_softpipe_drom_curdevdat->ctodscale[2];

   dde_softpipe_drom_curviwdat->clear_flag = 
	 DvInqClearFlag(dde_softpipe_drom_data.current_view);

   DvInqBackgroundColor(dde_softpipe_drom_data.current_view,
				&dde_softpipe_drom_curviwdat->background_color_model,
				dde_softpipe_drom_curviwdat->background_color);

   dde_softpipe_drom_curdevdat->shade_mode = 
	   DdInqShadeMode(dde_softpipe_drom_data.current_device);

   dde_softpipe_drom_curdevdat->visualtype = 
	 DdInqVisualType(dde_softpipe_drom_data.current_device);

   if(dde_softpipe_drom_curdevdat->visualtype == DcPseudoColor) {
      dde_softpipe_drom_data.pseudocolor_switch = DcTrue;
      dde_softpipe_drom_curviwdat->shade_index = 
	    DvInqShadeIndex(dde_softpipe_drom_data.current_view);
   }
   else if(dde_softpipe_drom_curdevdat->visualtype == DcStaticGrey) {
      dde_softpipe_drom_data.pseudocolor_switch = DcFalse;
   }
   else if(dde_softpipe_drom_curdevdat->visualtype == DcTrueColor) {
      dde_softpipe_drom_data.pseudocolor_switch = DcFalse;
   }
   else {
      DDerror(ERR_INVALID_VISUAL_TYPE,
		 "ddr_softpipe_drom_update_device_and_view",DcNullPtr);
      return;
   }
}

/* ====================================================================== */

void ddr_softpipe_drom_update_studio (DtPFI traverse_studio)
{
   DtInt lightno;
   DtReal hnrm[3];

#ifdef DEBUG
   printf("...processing dynout initialize studio environment\n");
   fflush(stdout);
#endif

   /** initialize before environment **/

   dde_softpipe_drom_curlgtdat->count = 0;

   dde_softpipe_drom_curlgtdat->ambient_reflectance[0] = 0.;
   dde_softpipe_drom_curlgtdat->ambient_reflectance[1] = 0.;
   dde_softpipe_drom_curlgtdat->ambient_reflectance[2] = 0.;

   /** typically **/

   (*traverse_studio)();

   /** initialize after environment **/

   dor_matrix_load(dde_softpipe_drom_curcamdat->wcstoccsmat,
		   dde_softpipe_drom_curcamdat->ccstowcsmat);
#ifdef DEBUG
   printf("wcstofcsmat:\n");
   dor_matrix_print(dde_softpipe_drom_curcamdat->wcstoccsmat);
   printf("ccstowcsmat:\n");
   dor_matrix_print(dde_softpipe_drom_curcamdat->ccstowcsmat);
#endif
   dor_matrix_invert(dde_softpipe_drom_curcamdat->wcstoccsmat);

#ifdef DEBUG
   printf("wcstofcsmat(inverted):\n");
   dor_matrix_print(dde_softpipe_drom_curcamdat->wcstoccsmat);
#endif
   ddr_softpipe_drom_compute_wcstofcs();

   for(lightno=0; lightno<dde_softpipe_drom_curlgtdat->count; lightno++) {
      ddr_softpipe_drom_h_normal(
				  dde_softpipe_drom_curcamdat->wcscamnorm,
				  dde_softpipe_drom_curlgtdat->cosine[lightno],
				  hnrm);
      dde_softpipe_drom_curlgtdat->h[lightno][0] = hnrm[0];
      dde_softpipe_drom_curlgtdat->h[lightno][1] = hnrm[1];
      dde_softpipe_drom_curlgtdat->h[lightno][2] = hnrm[2];
   }

   dor_matrix_load_identity(dde_softpipe_drom_data.nrmwcstolcsmat);
   dor_matrix_transpose(dde_softpipe_drom_data.nrmwcstolcsmat);

   ddr_softpipe_drom_update_lights();
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_start_update (
    DtObject device,
    DtObject view,
    DtPtr device_data,
    DtPtr view_data,
    DtPtr window_data)
{
#ifdef DEBUG
   printf("...processing dynout start update \n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.current_device = device;
   dde_softpipe_drom_data.current_view = view;

   dde_softpipe_drom_curdevdat = 
	 (struct dynout_device_data *)device_data;
   dde_softpipe_drom_curviwdat = (struct dynout_view_data *)view_data;
   dde_softpipe_drom_curwindat =
	 (struct dynout_window_data *)window_data;

   dde_softpipe_drom_curlgtdat = 
	 dde_softpipe_drom_curviwdat->light_data;
   dde_softpipe_drom_curcamdat =
	 dde_softpipe_drom_curviwdat->camera_data;

#ifdef DEBUG
   printf("...end of dynout start update \n");
   fflush(stdout);
#endif

}
/*
 ======================================================================
 */
void ddr_softpipe_drom_update_display (DtPFI traverse_display)
{
   DtInt rectangleno;

#ifdef DEBUG
   printf("...processing dynout render display group\n");
   fflush(stdout);
#endif

   if(DDdevice_InqAbortedDraw(dde_softpipe_drom_data.current_device)) {
      return;
   }

   ddr_softpipe_drom_update_state();

#ifdef DEBUG
   printf("...end of dynout update state\n");
   fflush(stdout);
#endif

   if(dde_softpipe_drom_curviwdat->clear_flag) {
      if(dde_softpipe_drom_data.pseudocolor_switch) {
	 DDdevice_SetShadeIndex(dde_softpipe_drom_data.current_device,
				   dde_softpipe_drom_curviwdat->shade_index);
      }
      DDdevice_SetBackgroundColor 
	      (dde_softpipe_drom_data.current_device, DcRGB,
	       dde_softpipe_drom_curviwdat->background_color);

      for(rectangleno=0; rectangleno<dde_softpipe_drom_curwindat->
	  clipping_rectangle_count; rectangleno++) {
	 DDdevice_ClearRectangleDepthColor
	       (dde_softpipe_drom_data.current_device,
		(DtInt)(dde_softpipe_drom_curwindat->
			clipping_rectangles[rectangleno]).xmin,
		(DtInt)(dde_softpipe_drom_curwindat->
			clipping_rectangles[rectangleno]).ymin,
		(DtInt)(dde_softpipe_drom_curwindat->
			clipping_rectangles[rectangleno]).xmax,
		(DtInt)(dde_softpipe_drom_curwindat->
			clipping_rectangles[rectangleno]).ymax);
      }
      DDdevice_Flush(dde_softpipe_drom_data.current_device);
   }

#ifdef DEBUG
   printf("...processing traverse display group\n");
   fflush(stdout);
#endif

   (*traverse_display)();
   DDdevice_Flush(dde_softpipe_drom_data.current_device);
}
