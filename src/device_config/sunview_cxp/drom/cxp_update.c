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
	int ddr_sunview_cxp_drom_pre_initialization ()
	int ddr_sunview_cxp_drom_post_initialization ()
	int ddr_sunview_cxp_drom_update_local_data ()
	int ddr_sunview_cxp_drom_update_studio (traverse_studio)
	int ddr_sunview_cxp_drom_initialize_before_environment ()
	int ddr_sunview_cxp_drom_initialize_after_environment ()
	ddr_sunview_cxp_drom_start_update ddr_sunview_cxp_drom_start_update (device, view, device_data, view_data, window_data)
	int ddr_sunview_cxp_drom_update_display (traverse_display)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_pre_initialization()
{
    dde_sunview_cxp_drom_pre_init = DcTrue;

    dde_sunview_cxp_drom_data.colored_elements = NULL;
    dde_sunview_cxp_drom_data.colored_element_count = 0;

    dde_sunview_cxp_drom_data.color_indices = NULL;
    dde_sunview_cxp_drom_data.color_indices_count = 0;

    dde_sunview_cxp_drom_data.backfacing_flags = NULL;
    dde_sunview_cxp_drom_data.backfacing_flags_count = 0;

    ddr_sunview_cxp_drom_initialize_specular_table();
   
    dde_sunview_cxp_drom_data.current_device = NULL;
    dde_sunview_cxp_drom_data.current_view = NULL;

    dde_sunview_cxp_drom_curdevdat = NULL;
    dde_sunview_cxp_drom_curviwdat = NULL;
    dde_sunview_cxp_drom_curwindat = NULL;
    dde_sunview_cxp_drom_curcamdat = NULL;
    dde_sunview_cxp_drom_curlgtdat = NULL;

    /* the top of the stack is dde_sunview_cxp_drom_data.object_lcstowcsmat */
    dde_sunview_cxp_drom_data.mat_stack.current_element = -1;
    dde_sunview_cxp_drom_data.mat_stack.last_element = 9;
    dde_sunview_cxp_drom_data.mat_stack.elements = (struct mat_element *)
	    DDspace_Allocate (10*sizeof (struct mat_element));
    dor_matrix_load_identity(dde_sunview_cxp_drom_data.object_lcstowcsmat);

}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_post_initialization()
{
    /*  World to local normal transformation matrix is the transpose of
	the inverse of the wcstolcsmat.  Since lcstowcsmat is already the
	inverse of wcstolcsmat, we merely need to take the transpose of this
	matrix.  */

    dde_sunview_cxp_drom_pre_init = DcFalse;

    dor_matrix_load_identity(dde_sunview_cxp_drom_data.nrmwcstolcsmat);
    dor_matrix_transpose(dde_sunview_cxp_drom_data.nrmwcstolcsmat);

}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_update_local_data()      
{
    extern GP_data_ptr dde_sunview_cxp_current_gp;
    DtInt deviceno;
    DtVolume *actviewport;
    DtReal actviewxmin, actviewymin, actviewxmax, actviewymax;
    DtInt rno;
    DtInt cnt;
    DtInt cliprectcount;
    DtReal *cliprects;
    DtReal windowxmin, windowymin, windowxmax, windowymax;
    DtInt devicecount;
    DtInt shaderange[2];
    DtReal extent[4];
    DtFlag partiallyobscuredflag;
    DtReal ftodtrans[3];

    actviewport = DDdevice_InqActualViewport (dde_sunview_cxp_drom_data.current_device);
			   
    actviewxmin = actviewport->bll[0];
    actviewymin = 1023. - actviewport->fur[1];
    actviewxmax = actviewport->fur[0];
    actviewymax = 1023. - actviewport->bll[1];

    dde_sunview_cxp_drom_curdevdat->stereoswitch = DDdevice_InqStereo (dde_sunview_cxp_drom_data.current_device);

    DDdevice_InqClipList (dde_sunview_cxp_drom_data.current_device,
			     &cliprectcount,&cliprects,
			     extent, &partiallyobscuredflag);

    for (rno=0,cnt=0;cnt<cliprectcount;cnt++) {
	windowxmin = (cliprects[cnt*4+0]);
	windowymin = (cliprects[cnt*4+1]);
	windowxmax = (cliprects[cnt*4+2]);
	windowymax = (cliprects[cnt*4+3]);

	if ((windowxmin > actviewxmax) || (windowxmax < actviewxmin) ||
	    (windowymin > actviewymax) || (windowymax < actviewymin)) {
	    continue;

	}	else {
	    windowxmin = max(windowxmin,actviewxmin);
	    windowymin = max(windowymin,actviewymin);

	    windowxmax = min(windowxmax,actviewxmax);
	    windowymax = min(windowymax,actviewymax);
	}

	(dde_sunview_cxp_drom_curwindat->clipping_rectangles[rno]).xmin = windowxmin;
	(dde_sunview_cxp_drom_curwindat->clipping_rectangles[rno]).ymin = windowymin;
	(dde_sunview_cxp_drom_curwindat->clipping_rectangles[rno]).xmax = windowxmax;
	(dde_sunview_cxp_drom_curwindat->clipping_rectangles[rno]).ymax = windowymax;

	rno++;
    }

    dde_sunview_cxp_drom_curwindat->clipping_rectangle_count = rno;

    if (dde_sunview_cxp_drom_curwindat->clipping_rectangle_count == 0) {
	dde_sunview_cxp_drom_curwindat->visibility_state =
		ddd_sunview_cxp_drom_window_completely_obscured;

    } else if (!partiallyobscuredflag) {
	dde_sunview_cxp_drom_curwindat->visibility_state =
		ddd_sunview_cxp_drom_window_completely_visible;

    } else {
	dde_sunview_cxp_drom_curwindat->visibility_state =
		ddd_sunview_cxp_drom_window_partially_obscured;
    }

    DdInqShadeRanges (dde_sunview_cxp_drom_data.current_device,
				dde_sunview_cxp_drom_data.object_shade_index,
				1,shaderange);
    dde_sunview_cxp_drom_curdevdat->current_shade_range_min = (DtReal)shaderange[0];
    dde_sunview_cxp_drom_curdevdat->current_shade_range_max = (DtReal)shaderange[1];
   
    DDdevice_InqFrustumScaleTrans (dde_sunview_cxp_drom_data.current_device,
				dde_sunview_cxp_drom_curdevdat->ctodscale,
				dde_sunview_cxp_drom_curdevdat->ctodtranslate);

    DDdevice_InqFrameScaleTrans (dde_sunview_cxp_drom_data.current_device,
				 dde_sunview_cxp_drom_curdevdat->ftodscale,
				 ftodtrans);

    dde_sunview_cxp_drom_curdevdat->annoscale[0] = 
	    dde_sunview_cxp_drom_curdevdat->ftodscale[0] /
		    dde_sunview_cxp_drom_curdevdat->ctodscale[0];
    dde_sunview_cxp_drom_curdevdat->annoscale[1] = 
	    dde_sunview_cxp_drom_curdevdat->ftodscale[1] /
		    dde_sunview_cxp_drom_curdevdat->ctodscale[1];
    dde_sunview_cxp_drom_curdevdat->annoscale[2] = 
	    dde_sunview_cxp_drom_curdevdat->ftodscale[2] /
		    dde_sunview_cxp_drom_curdevdat->ctodscale[2];

    /*  Flip the y-axis upside down around 1023.  */

    dde_sunview_cxp_drom_curdevdat->ctodscale[1] *= -1.;
    dde_sunview_cxp_drom_curdevdat->ctodtranslate[1] = 1023. -
	    dde_sunview_cxp_drom_curdevdat->ctodtranslate[1];

    dde_sunview_cxp_drom_curviwdat->clear_flag = 
	    DvInqClearFlag(dde_sunview_cxp_drom_data.current_view);

    DvInqBackgroundColor (dde_sunview_cxp_drom_data.current_view,
				  &dde_sunview_cxp_drom_curviwdat->background_color_model,
				  dde_sunview_cxp_drom_curviwdat->background_color);

    dde_sunview_cxp_drom_curdevdat->shade_mode =
	    DdInqShadeMode (dde_sunview_cxp_drom_data.current_device);

    dde_sunview_cxp_drom_curdevdat->visual_type =
	    DdInqVisualType (dde_sunview_cxp_drom_data.current_device);

    if (dde_sunview_cxp_drom_curdevdat->visual_type == DcDirectColor) {
	dde_sunview_cxp_drom_data.pseudocolor_switch = DcFalse;

    } else if (dde_sunview_cxp_drom_curdevdat->visual_type == DcPseudoColor) {
	dde_sunview_cxp_drom_data.pseudocolor_switch = DcTrue;

	dde_sunview_cxp_drom_curviwdat->shade_index = 
		DvInqShadeIndex(dde_sunview_cxp_drom_data.current_view);
    } else {
	DDerror (ERR_INVALID_VISUAL_TYPE,
		    "ddr_sunview_cxp_drom_update_device_and_view", DcNullPtr);

	return;
    }

    /* for SUN ctodtranslate[z] = 0; 
       because of different device coordinates of SUN from 
       titan */
    dde_sunview_cxp_drom_curdevdat->ctodtranslate[2] = 0.;

    GP_set_winview(dde_sunview_cxp_current_gp, 
		   dde_sunview_cxp_drom_curdevdat->ctodscale,
		   dde_sunview_cxp_drom_curdevdat->ctodtranslate);

    GP_set_cliplist(dde_sunview_cxp_current_gp,
		    dde_sunview_cxp_drom_curwindat->clipping_rectangle_count,
		    dde_sunview_cxp_drom_curwindat->clipping_rectangles);

}
/*
 ======================================================================
 */

      /*  The purpose of environmental initialization is to collect
	  data related to the cameras and lights.  The
	  camera data always includes the camera's local to world
	  transform (which is inverted to obtain the world to camera
	  transform).  */


ddr_sunview_cxp_drom_update_studio(traverse_studio)
     DtPFI traverse_studio;
{
    ddr_sunview_cxp_drom_initialize_before_environment();

    (*traverse_studio) ();

    ddr_sunview_cxp_drom_initialize_after_environment();
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_initialize_before_environment()

{
    dde_sunview_cxp_drom_curlgtdat->count = 0;

    dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[0] = 0.;
    dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[1] = 0.;
    dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[2] = 0.;
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_initialize_after_environment()

{
    DtInt lightno;
    DtReal hnrm[3];

    dor_matrix_load(dde_sunview_cxp_drom_curcamdat->wcstoccsmat,
		     dde_sunview_cxp_drom_curcamdat->ccstowcsmat);
    dor_matrix_invert(dde_sunview_cxp_drom_curcamdat->wcstoccsmat);

    ddr_sunview_cxp_drom_compute_wcstofcs();

    for (lightno=0;lightno<dde_sunview_cxp_drom_curlgtdat->count;lightno++) {
	ddr_sunview_cxp_drom_h_normal(dde_sunview_cxp_drom_curcamdat->wcscamnorm,
				       dde_sunview_cxp_drom_curlgtdat->cosine[lightno],hnrm);

	dde_sunview_cxp_drom_curlgtdat->h[lightno][0] = hnrm[0];
	dde_sunview_cxp_drom_curlgtdat->h[lightno][1] = hnrm[1];
	dde_sunview_cxp_drom_curlgtdat->h[lightno][2] = hnrm[2];
    }

    /*  World to local normal transformation matrix is the transpose of
	the inverse of the wcstolcsmat.  Since lcstowcsmat is already the
	inverse of wcstolcsmat, we merely need to take the transpose of this
	matrix.  */

    dor_matrix_load_identity(dde_sunview_cxp_drom_data.nrmwcstolcsmat);
    dor_matrix_transpose(dde_sunview_cxp_drom_data.nrmwcstolcsmat);

    ddr_sunview_cxp_drom_update_lights();
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_start_update (device, view, device_data, view_data, window_data)
     DtObject device;
     DtObject view;
     DtPtr device_data;
     DtPtr view_data;
     DtPtr window_data;
{

    dde_sunview_cxp_drom_data.current_device = device;
    dde_sunview_cxp_drom_data.current_view = view;

    dde_sunview_cxp_drom_curdevdat = (struct drom_device_data *)device_data;
    dde_sunview_cxp_drom_curviwdat = (struct drom_view_data *)view_data;
    dde_sunview_cxp_drom_curwindat = (struct drom_window_data *)window_data;

    dde_sunview_cxp_drom_curlgtdat = 
	    dde_sunview_cxp_drom_curviwdat->light_data;
    dde_sunview_cxp_drom_curcamdat = 
	    dde_sunview_cxp_drom_curviwdat->camera_data;
}


ddr_sunview_cxp_drom_update_display(traverse_display)
     DtPFI traverse_display;
{
    DtInt rectangleno;

    ddr_sunview_cxp_drom_update_state();

    if (dde_sunview_cxp_drom_curviwdat->clear_flag) {
	if (dde_sunview_cxp_drom_data.pseudocolor_switch) {
	    DDdevice_SetShadeIndex (dde_sunview_cxp_drom_data.current_device,
				       dde_sunview_cxp_drom_curviwdat->shade_index);
	}
	DDdevice_SetBackgroundColor (dde_sunview_cxp_drom_data.current_device,
					dde_sunview_cxp_drom_curviwdat->background_color);

	for (rectangleno=0;
	     rectangleno<dde_sunview_cxp_drom_curwindat->clipping_rectangle_count;
	     rectangleno++) {
	    DDdevice_ClearRectangleDepthColor (dde_sunview_cxp_drom_data.current_device,
						       (DtInt)
						       (dde_sunview_cxp_drom_curwindat->clipping_rectangles[rectangleno]).xmin,
						       (DtInt)
						       (dde_sunview_cxp_drom_curwindat->clipping_rectangles[rectangleno]).ymin,
						       (DtInt)
						       (dde_sunview_cxp_drom_curwindat->clipping_rectangles[rectangleno]).xmax,
						       (DtInt)
						       (dde_sunview_cxp_drom_curwindat->clipping_rectangles[rectangleno]).ymax);
	}

	DDdevice_Flush (dde_sunview_cxp_drom_data.current_device);
    }

    (*traverse_display) ();
}
