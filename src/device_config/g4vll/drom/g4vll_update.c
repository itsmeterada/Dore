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
	ddr_g4vll_drom_pre_initialization
	ddr_g4vll_drom_post_initialization
	ddr_g4vll_drom_update_local_data
	ddr_g4vll_drom_update_studio
	ddr_g4vll_drom_initialize_before_environment
	ddr_g4vll_drom_initialize_after_environment
	ddr_g4vll_drom_start_update
	ddr_g4vll_drom_update_display
	ddr_g4vll_drom_render_pass
	ddr_g4vll_drom_do_background
	ddr_g4vll_drom_set_prim_execute

  ======================================================================
 */
#include "drom.h"
#include <dore/internal/matrix.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/rstr.h>

#define IDENTITY	{ {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} }

/* ====================================================================== */

void ddr_g4vll_drom_pre_initialization (void)
{
    VLLclip_status *clip_status;
    static DtReal identmat[4][4] = IDENTITY;

#   ifdef DEBUG_2
    {   auto int i;
        for (i=0;  dde_g4vll_drom_class_data[i].name != 0;  i++) {
	    printf ("newclass %d: %s(%d) - %d\n", i,
		dde_g4vll_drom_class_data[i].name,
		dde_g4vll_drom_class_data[i].id,
		dde_g4vll_drom_class_data[i].index);
	}
    }
#   endif

    if (!dde_g4vll_drom_data.pre_init) {
    	/*
     	 * Ensure that the current VLL state matches this device's VLL state.  
	 * If it doesn't match, then install it.
     	 */
    	if (!(ddr_g4vll_dcm_inquire_if_states_match(
		dde_g4vll_drom_curdevdat->vll_state))) {
	    ddr_g4vll_dcm_set_state(dde_g4vll_drom_curdevdat->vll_state);
	}

	dde_g4vll_drom_data.pre_init = DcTrue;
	dde_g4vll_drom_data.matrix_stack.current_element = -1;
	dde_g4vll_drom_data.matrix_stack.last_element = 9;
	dde_g4vll_drom_data.matrix_stack.elements = 
		DDspace_Allocate (10*sizeof (ddt_g4vll_drom_matrix_element));

	/*
	 * Dore is responsible for allocating memory for the clip status
	 * structure and for all the clip plane sets and masks referenced
	 * by the clip status structure.  Only 1 set of modelling clip planes 
	 * (and corresponding clip plane masks) are accommodated here.
	 */
	dde_g4vll_drom_data.clip_status = 
		DDspace_Allocate(sizeof (VLLclip_status));
	clip_status = dde_g4vll_drom_data.clip_status;
	clip_status->clip_mask = 0;		/* canonical clipping */
	clip_status->num_clip_plane_sets = 1;	/* modelling clip clipping */
	clip_status->clip_plane_sets =
		DDspace_Allocate(sizeof (VLLclip_plane_set));
	clip_status->clip_plane_sets->clip_plane_mask =	0;
	clip_status->clip_plane_sets->operator = VLL_CLIP_REPLACE;
	clip_status->clip_plane_sets->num_planes = 0;
	clip_status->clip_plane_sets->planes = DDspace_Allocate
		(32 * sizeof (*(clip_status->clip_plane_sets->planes)));

	dde_g4vll_drom_data.clpvol_stack.current_element = 0;
	dde_g4vll_drom_data.clpvol_stack.last_element = 9;
	dde_g4vll_drom_data.clpvol_stack.elements = 
		DDspace_Allocate (10*sizeof (ddt_g4vll_drom_clpvol_element));
	dde_g4vll_drom_data.clpvol_stack.elements[0].modclpvol =
		dor_modclpvol_create (identmat,DcClipNone,0,NULL);
	ddr_g4vll_drom_update_clpvol(dde_g4vll_drom_data.clpvol_stack.elements[0].modclpvol);

    }

    ddr_g4vll_drom_set_vll_matrix (VLL_OBJECT_MATRIX, identmat);
    ddr_g4vll_drom_set_vll_matrix (VLL_LIGHT_MATRIX,  identmat);
    ddr_g4vll_drom_set_vll_matrix (VLL_NORMAL_MATRIX, identmat);
   
    dde_g4vll_drom_data.traversal_type = ddd_g4vll_drom_standard_pass;

    dde_g4vll_drom_data.super_sample_switch = DcFalse;

    /*
     * VLL's default transparency doesn't match Dore's, so we need to
     * set it here.
     */
    VLLset_transparency(0.5);
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_post_initialization (void)
{
    dde_g4vll_drom_data.pre_init = DcFalse;

    ddr_g4vll_drom_update_surface_properties();

    if (dde_g4vll_drom_data.backface_cullable_switch &&
	dde_g4vll_drom_data.backface_cull_switch) {
	VLLset_cull_mode(VLL_CULL_BACK);
    } else {
	VLLset_cull_mode(VLL_CULL_NONE);
    }
    VLLset_backface_eliminate_flag(VLL_BACK_CLOCKWISE);
    VLLset_backface_distinguish(VLL_DISABLE);
    VLLset_normal_invert(VLL_ENABLE);
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_update_local_data (void)		
{
    DtInt shaderange[2];
    DtVolume *actviewport;
    DtReal scale[3], trans[3];
    DtObject callback;
    DtReal ftodtrans[3];
    DtInt ncolors;

    actviewport = DDdevice_InqActualViewport (dde_g4vll_drom_data.current_device);
			   
    dde_g4vll_drom_curwindat->actviewxmin = actviewport->bll[0];
    dde_g4vll_drom_curwindat->actviewymin = actviewport->bll[1];
    dde_g4vll_drom_curwindat->actviewxmax = actviewport->fur[0];
    dde_g4vll_drom_curwindat->actviewymax = actviewport->fur[1];

    dde_g4vll_drom_curdevdat->stereoswitch = 
	    DDdevice_InqStereo (dde_g4vll_drom_data.current_device);
    dde_g4vll_drom_curdevdat->stereoscreen  = ddr_g4vll_dcm_inquire_stereo_screen();

    shaderange[0] = -10;
    shaderange[1] = -10;
    DdInqShadeRanges (dde_g4vll_drom_data.current_device,
		      dde_g4vll_drom_data.shade_index,1, shaderange);

    if (shaderange[0] != -10 && shaderange[1] != -10) {
	dde_g4vll_drom_curdevdat->pseudo_range[0] = shaderange[0];
	dde_g4vll_drom_curdevdat->pseudo_range[1] = shaderange[1];
    } else {
	dde_g4vll_drom_curdevdat->pseudo_range[0] = 0;
	dde_g4vll_drom_curdevdat->pseudo_range[1] = 255;
    }

    DDdevice_InqFrustumScaleTrans (dde_g4vll_drom_data.current_device,
					       scale,trans);

    DDdevice_InqFrameScaleTrans (dde_g4vll_drom_data.current_device,
				 dde_g4vll_drom_curdevdat->ftodscale,
				 ftodtrans);
     
    dde_g4vll_drom_curdevdat->annoscale[0] = 
	    dde_g4vll_drom_curdevdat->ftodscale[0] / scale[0];
    dde_g4vll_drom_curdevdat->annoscale[1] = 
	    dde_g4vll_drom_curdevdat->ftodscale[1] / scale[1];
    dde_g4vll_drom_curdevdat->annoscale[2] = 
	    dde_g4vll_drom_curdevdat->ftodscale[2] /
		    scale[2];

    dde_g4vll_drom_curviwdat->clear_flag = 
	    DvInqClearFlag(dde_g4vll_drom_data.current_view);

    DvInqBackgroundColor (dde_g4vll_drom_data.current_view,
			  &dde_g4vll_drom_curviwdat->background_color_model,
			  dde_g4vll_drom_curviwdat->background_color);
    DvInqBackgroundRaster(dde_g4vll_drom_data.current_view,
			  &dde_g4vll_drom_curviwdat->background_raster,
			  &callback);

    DvInqBackgroundJust(dde_g4vll_drom_data.current_view,
			&dde_g4vll_drom_curviwdat->background_just[0],
			&dde_g4vll_drom_curviwdat->background_just[1]);

    dde_g4vll_drom_curdevdat->shade_mode = DdInqShadeMode (dde_g4vll_drom_data.current_device);

    dde_g4vll_drom_curdevdat->visual_type = DdInqVisualType (dde_g4vll_drom_data.current_device);

    dde_g4vll_drom_curdevdat->use_spheres  = ddr_g4vll_dcm_inquire_spheres();
    dde_g4vll_drom_curdevdat->cylinders  = ddr_g4vll_dcm_inquire_cylinders();
    dde_g4vll_drom_curdevdat->use_hw_boundingvol  = 
				ddr_g4vll_dcm_inquire_hw_boundingvol();

    if (dde_g4vll_drom_curdevdat->visual_type == DcPseudoColor) {

	ddr_g4vll_dcm_inquire_ncolors(&ncolors);

	switch (dde_g4vll_drom_curdevdat->shade_mode) {
	case DcRange:
	    VLLset_output_color_model(VLL_PSEUDO_RANGE);
	    if (ncolors > 256) {
	    	VLLset_pseudo_range(dde_g4vll_drom_curdevdat->pseudo_range_weight,
				dde_g4vll_drom_curdevdat->pseudo_range, 
				VLL_PSEUDO_TYPE_12);
	    } else {
	    	VLLset_pseudo_range(dde_g4vll_drom_curdevdat->pseudo_range_weight,
				dde_g4vll_drom_curdevdat->pseudo_range, 
				VLL_PSEUDO_TYPE_RAMDAC);
	    }
	    break;
	case DcComponent:
	    VLLset_output_color_model(VLL_PSEUDO_SPACE);
	    if (ncolors > 256) {
	    	VLLset_pseudo_space(dde_g4vll_drom_curdevdat->pseudo_space_weight,
				dde_g4vll_drom_curdevdat->pseudo_space_mask,
				VLL_PSEUDO_TYPE_12);
	    } else {
	    	VLLset_pseudo_space(dde_g4vll_drom_curdevdat->pseudo_space_weight,
				dde_g4vll_drom_curdevdat->pseudo_space_mask,
				VLL_PSEUDO_TYPE_RAMDAC);
	    }
	    break;
	}
    }
}

/* ====================================================================== */

void ddr_g4vll_drom_update_studio (DtPFI traverse_studio)
{
    ddr_g4vll_drom_initialize_before_environment();

    (*traverse_studio)();

    ddr_g4vll_drom_initialize_after_environment();
}

/* ====================================================================== */

void ddr_g4vll_drom_initialize_before_environment (void)
{
    dde_g4vll_drom_curviwdat->total_lights = 0;

    VLLset_number_of_lights(0);
}

/* ====================================================================== */

void ddr_g4vll_drom_initialize_after_environment (void)
{

    if (dde_g4vll_drom_curcamdat->stereoswitch &&
	dde_g4vll_drom_curdevdat->stereoswitch) {
#ifdef DEBUG_STEREO
	printf ("computing stereo matrices\n");
#endif

	/*  Left eye view matrix.  */

	dor_matrix_load(
		dde_g4vll_drom_curcamdat->world_to_camera_matrix,
		dde_g4vll_drom_curcamdat->camera_to_world_matrix);

	dor_matrix_invert(dde_g4vll_drom_curcamdat->world_to_camera_matrix);

	ddr_g4vll_drom_compute_view_parameters();

	dor_matrix_load(dde_g4vll_drom_curcamdat->world_to_frustum_matrix_left,
		dde_g4vll_drom_curcamdat->world_to_frustum_matrix);

	dor_matrix_load(dde_g4vll_drom_curcamdat->tspfcstowcsmat_left,
		dde_g4vll_drom_curcamdat->world_to_frustum_matrix_left);
	dor_matrix_invert(dde_g4vll_drom_curcamdat->tspfcstowcsmat_left);
	dor_matrix_transpose(dde_g4vll_drom_curcamdat->tspfcstowcsmat_left);

	/*  Right eye view matrix.  */

	dor_matrix_load(
		dde_g4vll_drom_curcamdat->world_to_camera_matrix,
		dde_g4vll_drom_curcamdat->camera_to_world_matrix);

	dor_matrix_invert(dde_g4vll_drom_curcamdat->world_to_camera_matrix);

	ddr_g4vll_drom_compute_view_parameters();

	dor_matrix_load(dde_g4vll_drom_curcamdat->world_to_frustum_matrix_right,
			     dde_g4vll_drom_curcamdat->world_to_frustum_matrix);

	dor_matrix_load(dde_g4vll_drom_curcamdat->tspfcstowcsmat_right,
		dde_g4vll_drom_curcamdat->world_to_frustum_matrix_right);
	dor_matrix_invert(dde_g4vll_drom_curcamdat->tspfcstowcsmat_right);
	dor_matrix_transpose(dde_g4vll_drom_curcamdat->tspfcstowcsmat_right);
    } else {
#ifdef DEBUG_STEREO
	printf ("computing standard matrices\n");
#endif
	dor_matrix_load(dde_g4vll_drom_curcamdat->world_to_camera_matrix,
			dde_g4vll_drom_curcamdat->camera_to_world_matrix);
	dor_matrix_invert(dde_g4vll_drom_curcamdat->world_to_camera_matrix);

	ddr_g4vll_drom_compute_view_parameters();
    }
}

/* ====================================================================== */

void ddr_g4vll_drom_start_update (
    DtObject device,
    DtObject view,
    DtPtr    device_data,
    DtPtr    view_data,
    DtPtr    window_data)
{
    dde_g4vll_drom_data.current_device = device;
    dde_g4vll_drom_data.current_view = view;

    dde_g4vll_drom_curdevdat = (ddt_g4vll_drom_device *) device_data;
    dde_g4vll_drom_curviwdat = (ddt_g4vll_drom_view   *) view_data;
    dde_g4vll_drom_curwindat = (ddt_g4vll_drom_window *) window_data;

    dde_g4vll_drom_curcamdat
	= &(((ddt_g4vll_drom_view*)(view_data))->camera_data);
    /*
     * Ensure that the current VLL state matches this device's VLL state.  
     * If it doesn't match, then install it.
     */
    if (!(ddr_g4vll_dcm_inquire_if_states_match(
		dde_g4vll_drom_curdevdat->vll_state))) {
	ddr_g4vll_dcm_set_state(dde_g4vll_drom_curdevdat->vll_state);
    }

}

/* ====================================================================== */

void ddr_g4vll_drom_update_display (DtPFI traverse_display)
{
    float jitterS[3];
    int x,y,width,height;

    /*
     * Before doing the display update, check to see if the device
     * has changed size since the last update.  If it has, set the
     * AbortedDraw flag for the kernel to loop back on and exit.
     */

    if (ddr_g4vll_dcm_check_device_extent()) {
	DDdevice_SetAbortedDraw(dde_g4vll_drom_data.current_device, DcTrue);
	return;
    }

    ddr_g4vll_dcm_set_viewport(dde_g4vll_drom_curwindat->actviewxmin, 
			       dde_g4vll_drom_curwindat->actviewymin, 
			       dde_g4vll_drom_curwindat->actviewxmax, 
			       dde_g4vll_drom_curwindat->actviewymax);

#ifdef DEBUG_2
    printf ("SetView (%g %g %g) \n", 
	    dde_g4vll_drom_curcamdat->wcscamloc[0],
	    dde_g4vll_drom_curcamdat->wcscamloc[1],
	    dde_g4vll_drom_curcamdat->wcscamloc[2]);

    if (dde_g4vll_drom_curcamdat->camtype == DcParallel) {
	printf ("camera is Parallel\n");
    } else {
	printf ("camera is Perspective\n");
    }
#endif
    VLLset_eye(dde_g4vll_drom_curcamdat->wcscamnorm,
		   dde_g4vll_drom_curcamdat->wcscamloc, VLL_EYE_VECTOR);

    if (dde_g4vll_drom_curdevdat->stereoscreen != ddd_g4vll_mono_screen) {
	ddr_g4vll_dcm_set_stereo_eye (VLL_STEREO_LEFT);
#ifdef DEBUG_STEREO
	printf ("set stereo eye LEFT\n");
#endif
    }

    if (dde_g4vll_drom_curdevdat->stereoswitch &&
	dde_g4vll_drom_curcamdat->stereoswitch) {

	dor_matrix_load(dde_g4vll_drom_curcamdat->world_to_frustum_matrix,
		dde_g4vll_drom_curcamdat->world_to_frustum_matrix_left);

        dde_g4vll_drom_curcamdat->world_to_frustum_matrix[0][3] +=
		dde_g4vll_drom_curcamdat->stereo_view_scale * 
		dde_g4vll_drom_curcamdat->projmat[0][0] * 
		dde_g4vll_drom_curcamdat->stereo_eyeseparation * 0.5;
    }

    ddr_g4vll_drom_set_vll_matrix
	(VLL_OBJECT_MATRIX, dde_g4vll_drom_curcamdat->world_to_frustum_matrix);

    if (dde_g4vll_drom_curdevdat->stereoswitch &&
	dde_g4vll_drom_curcamdat->stereoswitch) {

    	VLLquery_draw_area(&x,&y,&width,&height);

    	if (dde_g4vll_drom_curcamdat->stereo_distance != 0.) {
    	    jitterS[0] = width * dde_g4vll_drom_curcamdat->stereo_view_scale * 
		dde_g4vll_drom_curcamdat->projmat[0][0] *
		dde_g4vll_drom_curcamdat->stereo_eyeseparation * 0.25 /
		-dde_g4vll_drom_curcamdat->stereo_distance;
	} else {
	    jitterS[0] = 0.;
	}
    	jitterS[1] = jitterS[2] = 0.0;
    } else {
    	jitterS[0] = jitterS[1] = jitterS[2] = 0.0;
    }
    VLLset_jitter(jitterS);

    /* reset traversal counter */
    doe_system_timing_display_traverse_count = 0;

    ddr_g4vll_drom_render_pass(traverse_display);

    if (dde_g4vll_drom_curcamdat->super_sample_switch) {
	int *tbl = dde_g4vll_drom_curcamdat->filter;
	DtInt *offset = &(dde_g4vll_drom_curcamdat->filter_center[0]);
	DtReal *step = &(dde_g4vll_drom_curcamdat->filter_step[0]);
	DtInt *size = &(dde_g4vll_drom_curcamdat->filter_size[0]);
	float jitter[3], jitterSS[3];
	int x, y;

	ddr_g4vll_dcm_clear_filter_buffer ();

	ddr_g4vll_dcm_blend_filter(tbl[offset[0] + size[0]*offset[1]]);

	jitter[0] = jitter[1] = jitter[2] = 0.0;

	for (y=0; y < size[1]; y++) {
	    jitter[1] = step[1] * (y - offset[1]);
	    for (x=0; x < size[0]; x++) {
		jitter[0] = step[0] * (x - offset[0]);
		if (x == offset[0] && y == offset[1]) {
		    continue;
		}
/*
 * If doing stereo, and supersampling, then we need to take both jitters
 */
		if (dde_g4vll_drom_curdevdat->stereoswitch &&
		    dde_g4vll_drom_curcamdat->stereoswitch) {
		    jitterSS[0] = jitter[0] + jitterS[0];
		    jitterSS[1] = jitter[1] + jitterS[1];
		    jitterSS[2] = jitter[2] + jitterS[2];
		    VLLset_jitter(jitterSS);
		} else {
		    VLLset_jitter(jitter);
		}

		ddr_g4vll_drom_set_vll_matrix
		(   VLL_OBJECT_MATRIX,
		    dde_g4vll_drom_curcamdat->world_to_frustum_matrix
		);

		ddr_g4vll_drom_render_pass (traverse_display);
	       
		ddr_g4vll_dcm_blend_filter (tbl[x + y*size[0]]);
	    }
	}
	VLLnormalize_filter();
	jitter[0] = jitter[1] = jitter[2] = 0.0;		      
	VLLset_jitter(jitter);
    }

    DDdevice_Flush(dde_g4vll_drom_data.current_device);

    /*
     * This second render pass is done when rendering is done to a
     * stereo screen (regardless of whether stereo is on or not). 
     * Use the mono camera matrix when stereo was not requested by
     * the application.
     */
    if (dde_g4vll_drom_curdevdat->stereoscreen != ddd_g4vll_mono_screen) {
	ddr_g4vll_dcm_set_stereo_eye(VLL_STEREO_RIGHT);
#ifdef DEBUG_STEREO
	printf ("set stereo eye RIGHT\n");
#endif

	if (dde_g4vll_drom_curdevdat->stereoswitch &&
	    dde_g4vll_drom_curcamdat->stereoswitch) {
	    dor_matrix_load(
	        dde_g4vll_drom_curcamdat->world_to_frustum_matrix,
		dde_g4vll_drom_curcamdat->world_to_frustum_matrix_right);

    	    dde_g4vll_drom_curcamdat->world_to_frustum_matrix[0][3] -=
		dde_g4vll_drom_curcamdat->stereo_view_scale * 
		dde_g4vll_drom_curcamdat->projmat[0][0] * 
		dde_g4vll_drom_curcamdat->stereo_eyeseparation * 0.5;
	}

	ddr_g4vll_drom_set_vll_matrix(VLL_OBJECT_MATRIX, 
	    	dde_g4vll_drom_curcamdat->world_to_frustum_matrix
	);

     	if (dde_g4vll_drom_curdevdat->stereoswitch &&
		dde_g4vll_drom_curcamdat->stereoswitch) {
    	    jitterS[0] = -jitterS[0];
    	    jitterS[1] = jitterS[2] = 0.0;
        } else {
    	    jitterS[0] = jitterS[1] = jitterS[2] = 0.0;
        }
        VLLset_jitter(jitterS);

	ddr_g4vll_drom_render_pass(traverse_display);

	if (dde_g4vll_drom_curcamdat->super_sample_switch) {
	    int *tbl = dde_g4vll_drom_curcamdat->filter;
	    DtInt *offset = &(dde_g4vll_drom_curcamdat->filter_center[0]);
	    DtReal *step = &(dde_g4vll_drom_curcamdat->filter_step[0]);
	    DtInt *size = &(dde_g4vll_drom_curcamdat->filter_size[0]);
	    float jitter[3], jitterSS[3];
	    int x, y;

	    ddr_g4vll_dcm_clear_filter_buffer();

	    ddr_g4vll_dcm_blend_filter(tbl[offset[0] + size[0]*offset[1]]);

	    jitter[0] = jitter[1] = jitter[2] = 0.0;

	    for (y=0; y < size[1]; y++) {
		jitter[1] = step[1] * (y - offset[1]);
		for (x=0; x < size[0]; x++) {
		    jitter[0] = step[0] * (x - offset[0]);
		    if (x == offset[0] && y == offset[1]) {
			continue;
		    }
/*
 * If doing stereo, and supersampling, then we need to take both jitters
 */
		    if (dde_g4vll_drom_curdevdat->stereoswitch &&
			dde_g4vll_drom_curcamdat->stereoswitch) {
			jitterSS[0] = jitter[0] + jitterS[0];
			jitterSS[1] = jitter[1] + jitterS[1];
			jitterSS[2] = jitter[2] + jitterS[2];
			VLLset_jitter(jitterSS);
		    } else {
		        VLLset_jitter(jitter);
		    }

		    ddr_g4vll_drom_set_vll_matrix
		    (   VLL_OBJECT_MATRIX, 
			dde_g4vll_drom_curcamdat->world_to_frustum_matrix
		    );

		    ddr_g4vll_drom_render_pass(traverse_display);
	       
		    ddr_g4vll_dcm_blend_filter(tbl[x + y*size[0]]);
		}
	    }
	    VLLnormalize_filter();
	    jitter[0] = jitter[1] = jitter[2] = 0.0;		      
	    VLLset_jitter(jitter);
	}
    }
}

/* ====================================================================== */

int ddr_g4vll_drom_render_pass (DtPFI traverse_display)
{
    auto int   transp_passes;
    auto float zmin, zmax;

    /* Pass Setup */

    dde_g4vll_drom_data.have_shadows		 = DcFalse;
    dde_g4vll_drom_data.have_transparency	 = DcFalse;
    dde_g4vll_drom_data.have_opaque_objects	 = DcFalse;
    dde_g4vll_drom_data.have_textures		 = DcFalse;
    dde_g4vll_drom_data.have_transp_textured_objects = DcFalse;
    dde_g4vll_drom_data.have_mip_mapping	 = DcFalse;
    dde_g4vll_drom_data.have_env_mapping	 = DcFalse;
    dde_g4vll_drom_data.have_no_norm_textures	 = DcFalse;
    dde_g4vll_drom_data.texpass_modes	 	 = 0;
    dde_g4vll_drom_data.map_extend_black 	 = DcFalse;
    dde_g4vll_drom_data.vll_extend_mode[0]       =
    	dde_g4vll_drom_data.vll_extend_mode[1]   =
    	dde_g4vll_drom_data.vll_extend_mode[2]   = VLL_EXTEND_NONE;
    dde_g4vll_drom_data.map_tile_aamode 	 = DcTextureAANone;
    dde_g4vll_drom_data.num_texture_passes 	 = 1;
    dde_g4vll_drom_data.cur_texture_pass   	 = -1;

    if (dde_g4vll_drom_curcamdat->viewtype == DcCameraPerspective) {
        /*
         * For proper rendering of textures with a perspective camera
         * zmax should be 1 and zmin should be hither/yon.
         * Clamp zmin at 1/2048 (0.00049) to avoid artifacts.
         */
        zmax = 1.0;
        zmin = dde_g4vll_drom_curcamdat->hither / dde_g4vll_drom_curcamdat->yon;
        if (zmin < 0.00049) zmin = 0.00049;
    } else {
        /* 
	 * The zrange must be limited to .21 to 1 to prevent artifacts
         * with 256x256 textures or firmware spheres (the 1/z table is 
	 * inaccurate below .21).
	 */
        zmin = .21;
        zmax = 1.0;
    }
    VLLset_z_extent (zmin, zmax);

   /*
    * Start the render pass, which in itself may involve many traversals.
    * The order in which traversals occur are:
    *
    *	---
    *    |   Opaque pass.  This also acts as a pre-pass that marks objects that
    *    |   	are textured OR textured & transparent.  This pass is always
    *    |      done.
    *    |
    *   ---
    *    |   Texture pass(es).  Done only if textured objects were encountered
    *    |	in the opaque pass.  This pass involves the following sub-
    *    |      passes:
    *    |
    *	 |	-Draw: generates the uv/w coordinates; for local 2D textures
    *	 |		this pass also does the texture lookup and light
    *    |              blending.  Determine whether subsequent texture passes
    *    |              are required at the point of rendering the primitive in
    *    |              this pass.  Mark the textured primitive if it should
    *    |              actually be rendered in textured-transparent pass
    *    |              instead.  This pass is tiled.
    *	 |	-Light: texture lookup and light blending for maps not completed
    *	 |		in the draw pass. This pass is done once only (no
    *    |              tiling).  (don't know exactly which maps at this point
    *    |              as we are always tiling large textures ...)
    *    |      -Vertex Colour:  interpolates vertex colours in the extend-none
    *    |              area of a textured primitive.  This pass is performed
    *    |              only on those primitives that have vertex colours.
    *    |              This pass is done once only (no tiling).
    *    |      -Specular Highlights:  adds specular highlights to a textured
    *    |              primitive.  This pass is performed only on those
    *    |              primitives with specular highlights, and is done once
    *    |              only (no tiling).
    *    |
    *   ---
    *    |   Transparency pass.  For both textured and nontextured transparent
    *    |      objects.  Do the nontextured transparent pass first, then the
    *    |      textured passes as needed.
    *    |
    *   ---
    *    |   Shadow pass.  Only if shadows are present.
    *    |
    *   ---
    */

    ddr_g4vll_drom_do_background();

    /* 
     * Opaque rendering pass.  
     */
#   ifdef DEBUG_FLOW
	printf ("Do the opaque rendering pass\n");
#   endif

    dde_g4vll_drom_data.traversal_type = ddd_g4vll_drom_standard_pass;
    VLLset_render_mode(VLL_RENDER_STANDARD);

    ddr_g4vll_drom_set_prim_execute();

    doe_system_timing_display_traverse_count++;
    (*traverse_display) ();
    VLLrendez_vous();

    if (dde_g4vll_drom_data.have_textures) {
#       ifdef DEBUG_FLOW
	    printf ("\tDo the opaque texture rendering pass\n");
#       endif
        /* 
         * If MIP-map texture lookup or environment mapping are
         * to be done, allocate the appropriate resources:
	 *     MIP mapping:  SP image Pixmap with SP Z  (currently get a DP Z)
	 *     Env mapping:  DP image Accumulator with SP Z (currently get DP Z)
         */
        if (dde_g4vll_drom_data.have_mip_mapping) {
            ddr_g4vll_dcm_allocate_offscreen_resource(ddd_g4vll_pixmap_z);
            ddr_g4vll_dcm_set_offscreen_resource(ddd_g4vll_pixmap_z);
        }
        if (dde_g4vll_drom_data.have_env_mapping) {
	    ddr_g4vll_dcm_allocate_offscreen_resource (ddd_g4vll_accumulator_z);
	    ddr_g4vll_dcm_set_offscreen_resource (ddd_g4vll_accumulator_z);
        }

        /*
         * Assume only one complete texture pass will be done -- multiple
         * VLL_RENDER_TEXTURE_DRAW and VLL_RENDER_TEXTURE_VERTEX_COLOR passes
         * will be required for "large" textures.
         */
        dde_g4vll_drom_data.cur_texture_pass   = 0;
        dde_g4vll_drom_data.traversal_type = ddd_g4vll_drom_texture_pass;

        VLLinit_texture();

        do {

            VLLset_render_mode(VLL_RENDER_TEXTURE_DRAW);

	    dde_g4vll_drom_data.texpass_modes = 
	    dde_g4vll_drom_data.current_texpass_mode = 
		ddd_g4vll_drom_texpass_draw;

	    /* Reset the texture-tiling-finished flags. */

	    dde_g4vll_drom_data.tex2D_finished = DcFalse;
	    dde_g4vll_drom_data.tex3D_finished = DcFalse;

	    doe_system_timing_display_traverse_count++;
            (*traverse_display) ();

            ddr_g4vll_dcm_blend_texture(
                    (DtShort)dde_g4vll_drom_curwindat->actviewxmin, 
                    (DtShort)dde_g4vll_drom_curwindat->actviewymin, 
                    (DtShort)dde_g4vll_drom_curwindat->actviewxmax, 
                    (DtShort)dde_g4vll_drom_curwindat->actviewymax,
                    dde_g4vll_drom_data.have_no_norm_textures);

	    VLLrendez_vous();

            (dde_g4vll_drom_data.cur_texture_pass)++;

#           ifdef DEBUG_FLOW
		printf ("\tTEXTURE_DRAW: curpass = %d   numpasses = %d\n",
		    dde_g4vll_drom_data.cur_texture_pass,
		    dde_g4vll_drom_data.num_texture_passes);
#           endif

        } while (dde_g4vll_drom_data.cur_texture_pass <
                 dde_g4vll_drom_data.num_texture_passes);

	/* Reset the current texture pass counter and the texture-tiling
	** finished flags.  */

	dde_g4vll_drom_data.cur_texture_pass = 0;

	/*
	 * The light pass bit of texpass_modes is set if a primitive 
	 * that requires a texture lighting pass was encountered during
	 * the texture draw pass.  If this pass is done, it will be done
	 * on all primitives, regardless of whether they need it.
	 *
	 * Likewise, the vertex colour and specular highlight texture
	 * passes are done only if their respective bit fields are set,
	 * but they will be executed only on those primitives who require
	 * them.
	 */
	if (dde_g4vll_drom_data.texpass_modes & ddd_g4vll_drom_texpass_light) {

#           ifdef DEBUG_FLOW
		printf ("\tdo TEXLIGHT pass check\n");
#           endif

	    dde_g4vll_drom_data.current_texpass_mode = 
		    ddd_g4vll_drom_texpass_light;

	    VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);

	    ddr_g4vll_drom_set_prim_execute();

	    doe_system_timing_display_traverse_count++;
	    (*traverse_display) ();

	    ddr_g4vll_dcm_blend_texture(
			(DtShort)dde_g4vll_drom_curwindat->actviewxmin,
			(DtShort)dde_g4vll_drom_curwindat->actviewymin,
			(DtShort)dde_g4vll_drom_curwindat->actviewxmax,
			(DtShort)dde_g4vll_drom_curwindat->actviewymax,
			dde_g4vll_drom_data.have_no_norm_textures);

	    VLLrendez_vous();
	}

	if (dde_g4vll_drom_data.texpass_modes & ddd_g4vll_drom_texpass_vcol) {

#           ifdef DEBUG_FLOW
		printf ("\tdo TEXVCOL pass check\n");
#           endif

	    dde_g4vll_drom_data.cur_texture_pass = 0;

	    dde_g4vll_drom_data.current_texpass_mode = 
		ddd_g4vll_drom_texpass_vcol;

	    VLLset_render_mode (VLL_RENDER_TEXTURE_VERTEX_COLOR);

	    ddr_g4vll_drom_set_prim_execute ();
	    ++ doe_system_timing_display_traverse_count;

	    (*traverse_display) ();

	    ddr_g4vll_dcm_blend_texture
	    (   (DtShort) dde_g4vll_drom_curwindat->actviewxmin,
		(DtShort) dde_g4vll_drom_curwindat->actviewymin,
		(DtShort) dde_g4vll_drom_curwindat->actviewxmax,
		(DtShort) dde_g4vll_drom_curwindat->actviewymax,
		dde_g4vll_drom_data.have_no_norm_textures
	    );

	    VLLrendez_vous();
	}

	if (dde_g4vll_drom_data.texpass_modes & ddd_g4vll_drom_texpass_spec) {

#           ifdef DEBUG_FLOW
		printf ("\tdo TEXSPEC pass check\n");
#           endif
	    dde_g4vll_drom_data.current_texpass_mode = 
		    ddd_g4vll_drom_texpass_spec;

	    VLLset_render_mode(VLL_RENDER_TEXTURE_SPECULAR);

	    ddr_g4vll_drom_set_prim_execute();

	    doe_system_timing_display_traverse_count++;
	    (*traverse_display) ();

	    ddr_g4vll_dcm_blend_texture(
			(DtShort)dde_g4vll_drom_curwindat->actviewxmin,
			(DtShort)dde_g4vll_drom_curwindat->actviewymin,
			(DtShort)dde_g4vll_drom_curwindat->actviewxmax,
			(DtShort)dde_g4vll_drom_curwindat->actviewymax,
			dde_g4vll_drom_data.have_no_norm_textures);

	    VLLrendez_vous();
	}
    }

    if (dde_g4vll_drom_data.have_transparency) {
	transp_passes = 0;
#       ifdef DEBUG_FLOW
	    printf ("\tDoing the TopLevel Transparency Pass\n");
#       endif

	ddr_g4vll_dcm_setup_transp(dde_g4vll_drom_data.have_opaque_objects,
			dde_g4vll_drom_data.have_transp_textured_objects);

	do {
	    /*
	     * Do the transparency pass first.
	     */
#           ifdef DEBUG_FLOW
		printf ("\tDoing the Nontextured Transparency Pass\n");
#           endif
	    dde_g4vll_drom_data.traversal_type = ddd_g4vll_drom_transp_pass;

	    VLLset_render_mode (VLL_RENDER_STANDARD);

	    ddr_g4vll_dcm_clear_rectangle_transp_buffers(
			(DtShort)dde_g4vll_drom_curwindat->actviewxmin, 
		     	(DtShort)dde_g4vll_drom_curwindat->actviewymin, 
		     	(DtShort)dde_g4vll_drom_curwindat->actviewxmax, 
		     	(DtShort)dde_g4vll_drom_curwindat->actviewymax);

	    DDdevice_Flush(dde_g4vll_drom_data.current_device);

  	    ddr_g4vll_drom_set_prim_execute();

	    doe_system_timing_display_traverse_count++;
	    (*traverse_display) ();
	 
	    /*
	     * Now do the transparent-texture pass(es).
	     */
	    if (dde_g4vll_drom_data.have_transp_textured_objects) {

#               ifdef DEBUG_FLOW
		    printf ("\tDoing the Textured Transparency Pass\n");
#               endif
	    	dde_g4vll_drom_data.traversal_type = 
			ddd_g4vll_drom_transp_texture_pass;
	       /*
                * Assume only one complete texture pass will be done -- multiple
                * VLL_RENDER_TEXTURE_DRAW and VLL_RENDER_TEXTURE_VERTEX_COLOR
                * passes will be required for "large" textures.
		*/
		dde_g4vll_drom_data.num_texture_passes = 1;
		dde_g4vll_drom_data.cur_texture_pass   = 0;

		VLLinit_texture();

		do {

		    dde_g4vll_drom_data.current_texpass_mode = 
			ddd_g4vll_drom_texpass_draw;

		    VLLset_render_mode(VLL_RENDER_TEXTURE_DRAW);

	    	    doe_system_timing_display_traverse_count++;
		    (*traverse_display) ();

#                   ifdef DEBUG_FLOW
			printf ("\t Pass 1: ddr_g4vll_dcm_blend_texture();\n");
#                   endif
		    ddr_g4vll_dcm_blend_texture(
				(DtShort)dde_g4vll_drom_curwindat->actviewxmin,
				(DtShort)dde_g4vll_drom_curwindat->actviewymin,
				(DtShort)dde_g4vll_drom_curwindat->actviewxmax,
				(DtShort)dde_g4vll_drom_curwindat->actviewymax,
				dde_g4vll_drom_data.have_no_norm_textures);

		    VLLrendez_vous();

		    (dde_g4vll_drom_data.cur_texture_pass)++;

#                   ifdef DEBUG_FLOW
			printf ("\t%s: curpass = %d   numpasses = %d\n",
			    "Transparent TEXTURE_DRAW",
			    dde_g4vll_drom_data.cur_texture_pass,
			    dde_g4vll_drom_data.num_texture_passes);
#                   endif
                } while (dde_g4vll_drom_data.cur_texture_pass <
                         dde_g4vll_drom_data.num_texture_passes);


                /*
                 * The light pass bit of texpass_modes is set if a
                 * primitive that requires a texture lighting pass was
                 * encountered during the texture draw pass.  If this
                 * pass is done, it will be done on all primitives,
                 * regardless of whether they need it.
                 *
                 * Likewise, the vertex colour and specular highlight
                 * texture passes are done only if their respective
                 * bit fields are set, but they will be executed only
                 * on those primitives who require them.
                 */

		 if (dde_g4vll_drom_data.texpass_modes &
				ddd_g4vll_drom_texpass_light) {
#                   ifdef DEBUG_FLOW
			printf ("\tdo TEXLIGHT pass check\n");
#                   endif
		    dde_g4vll_drom_data.current_texpass_mode =
				ddd_g4vll_drom_texpass_light;

		    VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);

  		    ddr_g4vll_drom_set_prim_execute();

	    	    doe_system_timing_display_traverse_count++;
		    (*traverse_display) ();

		    ddr_g4vll_dcm_blend_texture(
			(DtShort)dde_g4vll_drom_curwindat->actviewxmin,
			(DtShort)dde_g4vll_drom_curwindat->actviewymin,
			(DtShort)dde_g4vll_drom_curwindat->actviewxmax,
			(DtShort)dde_g4vll_drom_curwindat->actviewymax,
			dde_g4vll_drom_data.have_no_norm_textures);
		 }

		 VLLrendez_vous();

		 if (dde_g4vll_drom_data.texpass_modes &
				ddd_g4vll_drom_texpass_vcol) {
#                   ifdef DEBUG_FLOW
			printf ("\tdo TEXVCOL pass check\n");
#                   endif
	            dde_g4vll_drom_data.cur_texture_pass   = 0;

                    do {

		        dde_g4vll_drom_data.current_texpass_mode =
				ddd_g4vll_drom_texpass_vcol;

		        VLLset_render_mode(VLL_RENDER_TEXTURE_VERTEX_COLOR);

  		        ddr_g4vll_drom_set_prim_execute();

	    	        doe_system_timing_display_traverse_count++;
		        (*traverse_display) ();

		        ddr_g4vll_dcm_blend_texture(
			    (DtShort)dde_g4vll_drom_curwindat->actviewxmin,
			    (DtShort)dde_g4vll_drom_curwindat->actviewymin,
			    (DtShort)dde_g4vll_drom_curwindat->actviewxmax,
			    (DtShort)dde_g4vll_drom_curwindat->actviewymax,
			    dde_g4vll_drom_data.have_no_norm_textures);

			(dde_g4vll_drom_data.cur_texture_pass)++;

		 	VLLrendez_vous();

#                       ifdef DEBUG_FLOW
			    printf ("\t%s: curpass = %d   numpasses = %d\n",
				"Transparent TEXTURE_VERTEX_COLOR",
				dde_g4vll_drom_data.cur_texture_pass,
				dde_g4vll_drom_data.num_texture_passes);
#                       endif

                    } while (dde_g4vll_drom_data.cur_texture_pass <
                                dde_g4vll_drom_data.num_texture_passes);
                }

		if (dde_g4vll_drom_data.texpass_modes &
				ddd_g4vll_drom_texpass_spec) {

#                   ifdef DEBUG_FLOW
			printf ("\tdo TEXSPEC pass check\n");
#                   endif
		    dde_g4vll_drom_data.current_texpass_mode =
				ddd_g4vll_drom_texpass_spec;

		    VLLset_render_mode(VLL_RENDER_TEXTURE_SPECULAR);

  		    ddr_g4vll_drom_set_prim_execute();

	    	    doe_system_timing_display_traverse_count++;
		    (*traverse_display) ();

		    ddr_g4vll_dcm_blend_texture(
			(DtShort)dde_g4vll_drom_curwindat->actviewxmin,
			(DtShort)dde_g4vll_drom_curwindat->actviewymin,
			(DtShort)dde_g4vll_drom_curwindat->actviewxmax,
			(DtShort)dde_g4vll_drom_curwindat->actviewymax,
			dde_g4vll_drom_data.have_no_norm_textures);

		     VLLrendez_vous();
	        }
	    }
	    transp_passes++;
	} while (!ddr_g4vll_dcm_blend_transp(
			(DtShort)dde_g4vll_drom_curwindat->actviewxmin, 
		  	(DtShort)dde_g4vll_drom_curwindat->actviewymin, 
		  	(DtShort)dde_g4vll_drom_curwindat->actviewxmax, 
		  	(DtShort)dde_g4vll_drom_curwindat->actviewymax) &&
		 	transp_passes < 20);

    	ddr_g4vll_dcm_finish_transp();
    }

    DDdevice_Flush(dde_g4vll_drom_data.current_device);

    /* 
     * Shadows rendering pass, if needed
     */
    if (dde_g4vll_drom_data.have_shadows) {
	dde_g4vll_drom_data.traversal_type = ddd_g4vll_drom_shadow_pass;
	VLLset_render_mode (VLL_RENDER_SHADOWS);

	do {
	    VLLinit_shadows();

  	    ddr_g4vll_drom_set_prim_execute();

	    doe_system_timing_display_traverse_count++;
	    (*traverse_display) ();
	    VLLrendez_vous();

	} while (!VLLblend_shadows());
    }

    return 0;
}

/* ====================================================================== */

void ddr_g4vll_drom_do_background (void)
{
    static int rstr_classid = -1;
    static int rstr_dataindex = -1;
    static int filerstr_classid = -1;
    static int filerstr_dataindex = -1;
    int rasterid;
    struct rstr *rstr;
    dot_object *raster_object;
    DtInt x, y, i;
    DtInt vwidth, vheight;
    DtInt deltax, deltay;
    DtInt offsetx, offsety;
    unsigned char *data;
    DtInt rwidth, rheight;
    ddt_g4vll_drom_rstr *dynout_rstr;
    int size, type;

    if (dde_g4vll_drom_curviwdat->clear_flag) {
	DDdevice_SetBackgroundColor (dde_g4vll_drom_data.current_device,
				     DcRGB,
				     dde_g4vll_drom_curviwdat->background_color);
	DDdevice_ClearRectangleDepthColor
		(dde_g4vll_drom_data.current_device,
		 (DtShort)dde_g4vll_drom_curwindat->actviewxmin, 
		 (DtShort)dde_g4vll_drom_curwindat->actviewymin, 
		 (DtShort)dde_g4vll_drom_curwindat->actviewxmax, 
		 (DtShort)dde_g4vll_drom_curwindat->actviewymax);

	if (dde_g4vll_drom_curviwdat->background_raster != DcNullObject) {
	 
	    if (rstr_classid == -1)
		    rstr_classid = DsInqClassId("DoRaster");
	    if (filerstr_classid == -1) 
		    filerstr_classid = DsInqClassId("DoFileRaster");

	    vwidth = (dde_g4vll_drom_curwindat->actviewxmax - 
		      dde_g4vll_drom_curwindat->actviewxmin);
	    vheight = (dde_g4vll_drom_curwindat->actviewymax - 
		       dde_g4vll_drom_curwindat->actviewymin);

	    raster_object = DDview_ResizeRaster(dde_g4vll_drom_data.current_view,
						vwidth, vheight);

	    if (raster_object != (DtObjectStructure *)0) {
		rasterid = DsInqObjClassId(raster_object);
	 
		if (rasterid == filerstr_classid) {
		    if (filerstr_dataindex == -1) {
			for (i=0; dde_g4vll_drom_class_data[i].name != 0; i++) {
			    if (dde_g4vll_drom_class_data[i].id == filerstr_classid) {
				filerstr_dataindex = dde_g4vll_drom_class_data[i].index;
				break;
			    }
			}
		    }

		    ddr_g4vll_drom_filerstr_update (raster_object);

		    raster_object = (DtObjectStructure *)
			    raster_object->additional_data[filerstr_dataindex];
		    if (raster_object == (DtObjectStructure *)0) return;
		    rasterid = DsInqObjClassId(raster_object);
		}
	    }

	    if (rasterid == rstr_classid) {
		if (rstr_dataindex == -1) {
		    for (i=0; dde_g4vll_drom_class_data[i].name != 0; i++) {
			if (dde_g4vll_drom_class_data[i].id == rstr_classid) {
			    rstr_dataindex = dde_g4vll_drom_class_data[i].index;
			    break;
			}
		    }
		}

		rstr = (struct rstr *)raster_object->data;
		dynout_rstr = (ddt_g4vll_drom_rstr *)
			raster_object->additional_data[rstr_dataindex];

		ddr_g4vll_drom_rstr_update_background(raster_object);

		if (dynout_rstr->background_data == 0) return;
					       
		deltax = (vwidth - rstr->width);
		deltay = (vheight - rstr->height);
		offsetx = deltax*dde_g4vll_drom_curviwdat->background_just[0];
		offsety = deltay*(1.0 - dde_g4vll_drom_curviwdat->background_just[1]);

		if (dynout_rstr->background_mode & ddd_g4vll_drom_rstr_with_z) {
		    size = 8;
		    type = VLL_RASTER_ZABGR;
		} else {
		    size = 4;
		    type = VLL_RASTER_ABGR;
		}
		data = dynout_rstr->background_data;
		rwidth = rstr->width;
		rheight = rstr->height;

		if (deltay < 0) {
		    data -= offsety * rstr->width * size;
		    rheight += deltay;
		    offsety = 0;
		}

		if (deltax < 0) {
		    data -= offsetx * size;
		    rwidth += deltax;
		    offsetx = 0;
		}

		x = dde_g4vll_drom_curwindat->actviewxmin + offsetx;

		/*
		 * Need to convert y coordinate since only the device section
		 * knows about the flip in y.  Since we are using VLLput_image
		 * here directly we need to convert the y coordinate to the
		 * correct device space.  It is not needed in other places
		 * because we are making 3D calls.
		 */

		y = (DtInt) ddr_g4vll_dcm_convert_y
			((DtReal)(dde_g4vll_drom_curwindat->actviewymax - offsety));

		VLLset_output_buffer(VLL_NONDISPLAY_BUFFER);
		VLLput_image(x, y, rwidth, rheight, rstr->width, data, type);
    		dde_g4vll_drom_data.have_opaque_objects	 = DcTrue;
	    }
	}
    }
}

/* ====================================================================== */

void ddr_g4vll_drom_set_prim_execute (void)
{
#   ifdef DEBUG_FLOW_2
	printf ("\tExecutability is on ...\n");
#   endif

    DDcondex_SetDisablePrimExecFlag (DcFalse);

    switch (dde_g4vll_drom_data.traversal_type) {
    case ddd_g4vll_drom_texture_pass:
	if (!dde_g4vll_drom_data.map_diffuse_color_switch &&
	    !dde_g4vll_drom_data.map_env_switch) {
#           ifdef DEBUG_FLOW_2
		printf("\tExecutability is off (dd_g4vll_drom_texture_pass)\n");
#           endif
	    DDcondex_SetDisablePrimExecFlag (DcTrue);
	}
	break;
   
    case ddd_g4vll_drom_transp_pass:
	if (!dde_g4vll_drom_data.transparent_switch) {
#           ifdef DEBUG_FLOW_2
		printf ("\tExecutability is off (dd_g4vll_drom_transp_pass)\n");
#           endif
	    DDcondex_SetDisablePrimExecFlag (DcTrue);
	}
	break;

    case ddd_g4vll_drom_transp_texture_pass:
	if (!dde_g4vll_drom_data.transparent_switch &&
		!dde_g4vll_drom_data.map_diffuse_color_switch) {
#           ifdef DEBUG_FLOW_2
		printf ("\tExecutability is off (dd_g4vll_drom_transp_texture_pass)\n");
#           endif
	    DDcondex_SetDisablePrimExecFlag (DcTrue);
	}
	break;

    case ddd_g4vll_drom_shadow_pass:
	if (!dde_g4vll_drom_data.shadow_switch) {
#           ifdef DEBUG_FLOW_2
		printf ("\tExecutability is off (dd_g4vll_drom_shadow_pass)\n");
#           endif
	    DDcondex_SetDisablePrimExecFlag (DcTrue);
	}
	break;
    case ddd_g4vll_drom_standard_pass:
	/*
	 * Always do the opaque pass.
	 */
	break;
    }
}
