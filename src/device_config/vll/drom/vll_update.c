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
	void ddr_vll_drom_pre_initialization ()
	void ddr_vll_drom_post_initialization ()
	void ddr_vll_drom_update_local_data ()
	void ddr_vll_drom_update_studio (traverse_studio)
	void ddr_vll_drom_initialize_before_environment ()
	void ddr_vll_drom_initialize_after_environment ()
	void ddr_vll_drom_start_update (device, view, device_data, view_data, window_data)
	void ddr_vll_drom_update_display (traverse_display)
	int ddr_vll_drom_render_pass (traverse_display)
	int ddr_vll_drom_check_for_spheres ()
	void ddr_vll_drom_do_background ()
	void ddr_vll_drom_set_prim_execute ()

  ======================================================================
 */
#include "drom.h"
#include "dore/dore_develop/private/rstr.h"

/*
 ======================================================================
 */
void
ddr_vll_drom_pre_initialization()
{
    static DtFlag init = DcFalse;
    extern struct modclpvol *dor_modclpvol_create();
    static DtReal identmat[4][4] = { 1.,0.,0.,0.,
					    0.,1.,0.,0.,
					    0.,0.,1.,0.,
					    0.,0.,0.,1.};

    dde_vll_drom_pre_init = DcTrue;

#ifdef DEBUG
    { int i;
      for (i=0; dde_vll_drom_class_data[i].name != 0; i++) {
	  printf ("newclass %d: %s(%d) - %d\n", i,
		  dde_vll_drom_class_data[i].name,
		  dde_vll_drom_class_data[i].id,
		  dde_vll_drom_class_data[i].index);
      }}
#endif

    if (!init) {
	dde_vll_drom_data.matrix_stack.current_element = -1;
	dde_vll_drom_data.matrix_stack.last_element = 9;
	dde_vll_drom_data.matrix_stack.elements = 
		(ddt_vll_drom_matrix_element *) DDspace_Allocate
			(10*sizeof (ddt_vll_drom_matrix_element));

	dde_vll_drom_data.clpvol_stack.current_element = 0;
	dde_vll_drom_data.clpvol_stack.last_element = 9;
	dde_vll_drom_data.clpvol_stack.elements = 
		(ddt_vll_drom_clpvol_element *) DDspace_Allocate 
			(10*sizeof (ddt_vll_drom_clpvol_element));
	dde_vll_drom_data.clpvol_stack.elements[0].modclpvol =
		dor_modclpvol_create (identmat,DcClipNone,0,NULL);
	ddr_vll_drom_update_clpvol(dde_vll_drom_data.clpvol_stack.elements[0].modclpvol);
    }
    init = DcTrue;

    if (sizeof(float) != sizeof(DtReal)) {
        int i, j;
	float imat[4][4];
        for (i=0; i<4; i++) {
            for (j=0; j<4; j++) {
		imat[i][j] = identmat[i][j];
	    }
	}
        VLLset_matrix (VLL_OBJECT_MATRIX, imat);
        VLLset_matrix (VLL_LIGHT_MATRIX, imat);
        VLLset_matrix (VLL_NORMAL_MATRIX, imat);
    } else {
        VLLset_matrix (VLL_OBJECT_MATRIX, identmat);
        VLLset_matrix (VLL_LIGHT_MATRIX, identmat);
        VLLset_matrix (VLL_NORMAL_MATRIX, identmat);
    }
   
    dde_vll_drom_data.traversal_type = ddd_vll_drom_standard_pass;

}
/*
 ======================================================================
 */
void 
ddr_vll_drom_post_initialization()
{
    dde_vll_drom_pre_init = DcFalse;

    ddr_vll_drom_update_surface_properties();

    if (dde_vll_drom_data.backface_cullable_switch &&
	dde_vll_drom_data.backface_cull_switch) {
	VLLset_backface_eliminate_flag(VLL_BACK_COUNTERCLOCKWISE);
	VLLset_backface_style(VLL_BACK_CULL);
    } else {
	VLLset_backface_style(VLL_BACK_FLIP);
	VLLset_backface_eliminate_flag(VLL_BACK_COUNTERCLOCKWISE);
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_update_local_data()		
{
    DtInt shaderange[2];
    extern DtFlag   ddr_vll_dcm_inquire_spheres();
    DtVolume *actviewport;
    DtReal scale[3], trans[3];
    DtObject callback;
    DtInt ncolors;
    DtReal ftodtrans[3];

    actviewport = DDdevice_InqActualViewport (dde_vll_drom_data.current_device);
			   
    dde_vll_drom_curwindat->actviewxmin = actviewport->bll[0];
    dde_vll_drom_curwindat->actviewymin = actviewport->bll[1];
    dde_vll_drom_curwindat->actviewxmax = actviewport->fur[0];
    dde_vll_drom_curwindat->actviewymax = actviewport->fur[1];

    dde_vll_drom_curdevdat->stereoswitch = 
	    DDdevice_InqStereo (dde_vll_drom_data.current_device);

    shaderange[0] = -10;
    shaderange[1] = -10;
    DdInqShadeRanges (dde_vll_drom_data.current_device,
		      dde_vll_drom_data.shade_index,1, shaderange);
    if (shaderange[0] != -10 && shaderange[1] != -10) {
	dde_vll_drom_curdevdat->pseudo_range[0] = shaderange[0];
	dde_vll_drom_curdevdat->pseudo_range[1] = shaderange[1];
    } else {
	dde_vll_drom_curdevdat->pseudo_range[0] = 0;
	dde_vll_drom_curdevdat->pseudo_range[1] = 255;
    }

    DDdevice_InqFrustumScaleTrans (dde_vll_drom_data.current_device,
					       scale,trans);

    DDdevice_InqFrameScaleTrans (dde_vll_drom_data.current_device,
				 dde_vll_drom_curdevdat->ftodscale,
				 ftodtrans);
     
    dde_vll_drom_curdevdat->annoscale[0] = 
	    dde_vll_drom_curdevdat->ftodscale[0] / scale[0];
    dde_vll_drom_curdevdat->annoscale[1] = 
	    dde_vll_drom_curdevdat->ftodscale[1] / scale[1];
    dde_vll_drom_curdevdat->annoscale[2] = 
	    dde_vll_drom_curdevdat->ftodscale[2] /
		    scale[2];

    dde_vll_drom_curviwdat->clear_flag = 
	    DvInqClearFlag(dde_vll_drom_data.current_view);

    DvInqBackgroundColor (dde_vll_drom_data.current_view,
			  &dde_vll_drom_curviwdat->background_color_model,
			  dde_vll_drom_curviwdat->background_color);
    DvInqBackgroundRaster(dde_vll_drom_data.current_view,
			  &dde_vll_drom_curviwdat->background_raster,
			  &callback);

    DvInqBackgroundJust(dde_vll_drom_data.current_view,
			&dde_vll_drom_curviwdat->background_just[0],
			&dde_vll_drom_curviwdat->background_just[1]);

    dde_vll_drom_curdevdat->shade_mode = DdInqShadeMode (dde_vll_drom_data.current_device);

    dde_vll_drom_curdevdat->visual_type = DdInqVisualType (dde_vll_drom_data.current_device);

    dde_vll_drom_curdevdat->use_spheres  = ddr_vll_dcm_inquire_spheres();

    if (dde_vll_drom_curdevdat->visual_type == DcPseudoColor) {
	ddr_vll_dcm_inquire_ncolors(&ncolors);

	switch (dde_vll_drom_curdevdat->shade_mode) {
	case DcRange:
	    VLLset_output_color_model(VLL_PSEUDO_RANGE);
	    VLLset_pseudo_range(dde_vll_drom_curdevdat->pseudo_range_weight,
				dde_vll_drom_curdevdat->pseudo_range, 
				(unsigned)ncolors);
	    break;
	case DcComponent:
	    VLLset_output_color_model(VLL_PSEUDO_SPACE);
	    VLLset_pseudo_space(dde_vll_drom_curdevdat->pseudo_space_weight,
				dde_vll_drom_curdevdat->pseudo_space_mask,
				(unsigned)ncolors);
	    break;
	}
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_update_studio(traverse_studio)
	DtPFI traverse_studio;
{
    ddr_vll_drom_initialize_before_environment();

    (*traverse_studio)();

    ddr_vll_drom_initialize_after_environment();
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_initialize_before_environment()
{
    dde_vll_drom_curviwdat->total_lights = 0;

    VLLset_number_of_lights(0);
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_initialize_after_environment()
{
    DtReal hnrm[3];
    DtReal stereo_rotation;
    extern DtReal dor_math_atan();

    if (dde_vll_drom_curcamdat->stereoswitch &&
	dde_vll_drom_curdevdat->stereoswitch) {
#ifdef DEBUG
	printf ("computing stereo matrices\n");
#endif
	if (dde_vll_drom_curcamdat->stereo_distance != 0.0) {
	    stereo_rotation = 
		    dor_math_atan(dde_vll_drom_curcamdat->stereo_eyeseparation / 
				  dde_vll_drom_curcamdat->stereo_distance);
	} else {
	    stereo_rotation = 0.0;
	}

	/*  Left eye view matrix.  */

	dor_matrix_load_rotate_y(dde_vll_drom_curcamdat->world_to_camera_matrix,
				 -stereo_rotation);

	dor_matrix_translate(dde_vll_drom_curcamdat->world_to_camera_matrix,
			     -dde_vll_drom_curcamdat->stereo_eyeseparation,0.,0.,
			     DcPostConcatenate);

	dor_matrix_concat_fast(dde_vll_drom_curcamdat->world_to_camera_matrix,
			       dde_vll_drom_curcamdat->camera_to_world_matrix,
			       DcPreConcatenate);

	dor_matrix_invert(dde_vll_drom_curcamdat->world_to_camera_matrix);

	ddr_vll_drom_compute_view_parameters();

	dor_matrix_load_fast(dde_vll_drom_curcamdat->world_to_frustum_matrix_left,
			     dde_vll_drom_curcamdat->world_to_frustum_matrix);

	dor_matrix_load_fast(dde_vll_drom_curcamdat->tspfcstowcsmat_left,
			     dde_vll_drom_curcamdat->world_to_frustum_matrix_left);
	dor_matrix_invert(dde_vll_drom_curcamdat->tspfcstowcsmat_left);
	dor_matrix_transpose(dde_vll_drom_curcamdat->tspfcstowcsmat_left);

	/*  Right eye view matrix.  */

	dor_matrix_load_rotate_y(dde_vll_drom_curcamdat->world_to_camera_matrix,
				 stereo_rotation);
	dor_matrix_translate(dde_vll_drom_curcamdat->world_to_camera_matrix,
			     dde_vll_drom_curcamdat->stereo_eyeseparation,0.,0.,
			     DcPostConcatenate);

	dor_matrix_concat_fast(dde_vll_drom_curcamdat->world_to_camera_matrix,
			       dde_vll_drom_curcamdat->camera_to_world_matrix,
			       DcPreConcatenate);
	dor_matrix_invert(dde_vll_drom_curcamdat->world_to_camera_matrix);

	ddr_vll_drom_compute_view_parameters();

	dor_matrix_load_fast(dde_vll_drom_curcamdat->world_to_frustum_matrix_right,
			     dde_vll_drom_curcamdat->world_to_frustum_matrix);

	dor_matrix_load_fast(dde_vll_drom_curcamdat->tspfcstowcsmat_right,
			     dde_vll_drom_curcamdat->world_to_frustum_matrix_right);
	dor_matrix_invert(dde_vll_drom_curcamdat->tspfcstowcsmat_right);
	dor_matrix_transpose(dde_vll_drom_curcamdat->tspfcstowcsmat_right);
    } else {
#ifdef DEBUG
	printf ("computing standard matrices\n");
#endif
	dor_matrix_load(dde_vll_drom_curcamdat->world_to_camera_matrix,
			dde_vll_drom_curcamdat->camera_to_world_matrix);
	dor_matrix_invert(dde_vll_drom_curcamdat->world_to_camera_matrix);

	ddr_vll_drom_compute_view_parameters();
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_start_update (device, view, device_data, view_data, window_data)
	DtObject device;
	DtObject view;
	ddt_vll_drom_device *device_data;
	ddt_vll_drom_view *view_data;
	ddt_vll_drom_window *window_data;
{
    dde_vll_drom_data.current_device = device;
    dde_vll_drom_data.current_view = view;

    dde_vll_drom_curdevdat = device_data;
    dde_vll_drom_curviwdat = view_data;
    dde_vll_drom_curwindat = window_data;

    dde_vll_drom_curcamdat = &(view_data->camera_data);

}
/*
 ======================================================================
 */
void 
ddr_vll_drom_update_display(traverse_display)
	DtPFI traverse_display;
{
    float mat[4][4];
    int rtn;

    ddr_vll_dcm_set_viewport(dde_vll_drom_curwindat->actviewxmin, 
			       dde_vll_drom_curwindat->actviewymin, 
			       dde_vll_drom_curwindat->actviewxmax, 
			       dde_vll_drom_curwindat->actviewymax);

#ifdef DEBUG
    printf ("SetView (%g %g %g) \n", 
	    dde_vll_drom_curcamdat->wcscamloc[0],
	    dde_vll_drom_curcamdat->wcscamloc[1],
	    dde_vll_drom_curcamdat->wcscamloc[2]);
#endif
    if (dde_vll_drom_curcamdat->camtype == DcParallel) {
#ifdef DEBUG 
	printf ("camera is Parallel -- VectorNoHomo\n");
#endif
	VLLset_eye(dde_vll_drom_curcamdat->wcscamnorm,
		   dde_vll_drom_curcamdat->wcscamloc, VLL_EYE_VECTOR);
    } else {
#ifdef DEBUG
	printf ("camera is Perspective -- VectorPoint\n");
#endif
	VLLset_eye(dde_vll_drom_curcamdat->wcscamnorm,
		   dde_vll_drom_curcamdat->wcscamloc, VLL_EYE_POINT);
    }

    if (dde_vll_drom_curdevdat->stereoswitch) {
	ddr_vll_dcm_set_stereo_eye (0);
    }
    if (dde_vll_drom_curdevdat->stereoswitch &&
	dde_vll_drom_curcamdat->stereoswitch) {
	dor_matrix_load(dde_vll_drom_curcamdat->world_to_frustum_matrix,
			dde_vll_drom_curcamdat->world_to_frustum_matrix_left);
    }


    if (dde_vll_drom_curcamdat->super_sample_switch) 
	    VLLinit_filter();

    do {
	if (sizeof(float) != sizeof(DtReal)) {
	    int i, j;
	    for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
		    mat[i][j] = dde_vll_drom_curcamdat->world_to_frustum_matrix[i][j];
		}
	    }
	    VLLset_matrix(VLL_OBJECT_MATRIX, mat);
	} else {
	    VLLset_matrix (VLL_OBJECT_MATRIX, 
			   dde_vll_drom_curcamdat->world_to_frustum_matrix);
	}

	/*
	 * ddr_vll_drom_render_pass returns 1 if need to restart as a result
	 * of spheres
	 */
	rtn = ddr_vll_drom_render_pass(traverse_display);

    } while (rtn);

    if (dde_vll_drom_curcamdat->super_sample_switch) {
	int *tbl = dde_vll_drom_curcamdat->filter;
	DtInt *offset = &(dde_vll_drom_curcamdat->filter_center[0]);
	DtReal *step = &(dde_vll_drom_curcamdat->filter_step[0]);
	DtInt *size = &(dde_vll_drom_curcamdat->filter_size[0]);
	float jitter[3];
	int x, y;

	VLLblend_filter(tbl[offset[0] + size[0]*offset[1]]);

	jitter[0] = jitter[1] = jitter[2] = 0.0;

	for (y=0; y < size[1]; y++) {
	    jitter[1] = step[1] * (y - offset[1]);
	    for (x=0; x < size[0]; x++) {
		jitter[0] = step[0] * (x - offset[0]);
		if (x == offset[0] && y == offset[1]) {
		    continue;
		}
		VLLset_jitter(jitter);
		do {
		    if (sizeof(float) != sizeof(DtReal)) {
			int i, j;
			for (i=0; i<4; i++) {
			    for (j=0; j<4; j++) {
				mat[i][j] = dde_vll_drom_curcamdat->world_to_frustum_matrix[i][j];
			    }
			}
			VLLset_matrix(VLL_OBJECT_MATRIX, mat);
		    } else {
			VLLset_matrix (VLL_OBJECT_MATRIX, 
				       dde_vll_drom_curcamdat->world_to_frustum_matrix);
		    }

		    /*
		     * ddr_vll_drom_render_pass returns 1 if need to restart as a result
		     * of spheres
		     */
		    rtn = ddr_vll_drom_render_pass(traverse_display);
	       
		} while (rtn);
		VLLblend_filter(tbl[x + y*size[0]]);
	    }
	}
	VLLnormalize_filter();
	jitter[0] = jitter[1] = jitter[2] = 0.0;		      
	VLLset_jitter(jitter);
    }


    DDdevice_Flush(dde_vll_drom_data.current_device);

    if (dde_vll_drom_curdevdat->stereoswitch) {
	ddr_vll_dcm_set_stereo_eye(1);

	if (dde_vll_drom_curcamdat->stereoswitch) {
	    dor_matrix_load(dde_vll_drom_curcamdat->world_to_frustum_matrix,
			    dde_vll_drom_curcamdat->world_to_frustum_matrix_right);
	}
	if (dde_vll_drom_curcamdat->super_sample_switch) 
		VLLinit_filter();

	do {
	    if (sizeof(float) != sizeof(DtReal)) {
		int i, j;
		for (i=0; i<4; i++) {
		    for (j=0; j<4; j++) {
			mat[i][j] = dde_vll_drom_curcamdat->world_to_frustum_matrix[i][j];
		    }
		}
		VLLset_matrix(VLL_OBJECT_MATRIX, mat);
	    } else {
		VLLset_matrix (VLL_OBJECT_MATRIX, 
			       dde_vll_drom_curcamdat->world_to_frustum_matrix);
	    }

	    /*
	     * ddr_vll_drom_render_pass returns 1 if need to restart as a result
	     * of spheres
	     */
	    rtn = ddr_vll_drom_render_pass(traverse_display);

	} while (rtn);

	if (dde_vll_drom_curcamdat->super_sample_switch) {
	    int *tbl = dde_vll_drom_curcamdat->filter;
	    DtInt *offset = &(dde_vll_drom_curcamdat->filter_center[0]);
	    DtReal *step = &(dde_vll_drom_curcamdat->filter_step[0]);
	    DtInt *size = &(dde_vll_drom_curcamdat->filter_size[0]);
	    float jitter[3];
	    int x, y;

	    VLLblend_filter(tbl[offset[0] + size[0]*offset[1]]);

	    jitter[0] = jitter[1] = jitter[2] = 0.0;

	    for (y=0; y < size[1]; y++) {
		jitter[1] = step[1] * (y - offset[1]);
		for (x=0; x < size[0]; x++) {
		    jitter[0] = step[0] * (x - offset[0]);
		    if (x == offset[0] && y == offset[1]) {
			continue;
		    }
		    VLLset_jitter(jitter);
		    do {
			if (sizeof(float) != sizeof(DtReal)) {
			    int i, j;
			    for (i=0; i<4; i++) {
				for (j=0; j<4; j++) {
				    mat[i][j] = dde_vll_drom_curcamdat->world_to_frustum_matrix[i][j];
				}
			    }
			    VLLset_matrix(VLL_OBJECT_MATRIX, mat);
			} else {
			    VLLset_matrix (VLL_OBJECT_MATRIX, 
					   dde_vll_drom_curcamdat->world_to_frustum_matrix);
			}

			/*
			 * ddr_vll_drom_render_pass returns 1 if need to restart as a result
			 * of spheres
			 */
			rtn = ddr_vll_drom_render_pass(traverse_display);
	       
		    } while (rtn);
		    VLLblend_filter(tbl[x + y*size[0]]);
		}
	    }
	    VLLnormalize_filter();
	    jitter[0] = jitter[1] = jitter[2] = 0.0;		      
	    VLLset_jitter(jitter);
	}
    }
}
/*
 ======================================================================
 */
int
ddr_vll_drom_render_pass(traverse_display)
	DtPFI traverse_display;
{
    int passes;
    int texture_abort;
    static DtColorRGB white = {1.,1.,1.};
    float xscale, yscale, zrange;
    float zmin, zmax;

    /*
     * Pass setup ...
     */
    VLLset_transparency(0.5);

    dde_vll_drom_data.have_shadows		= DcFalse;
    dde_vll_drom_data.have_transparency		= DcFalse;
    dde_vll_drom_data.have_textures		= DcFalse;
    dde_vll_drom_data.have_spheres		= DcFalse;
    dde_vll_drom_data.have_no_norm_textures	= DcFalse;

    ddr_vll_drom_set_prim_execute();

    if (dde_vll_drom_curdevdat->use_spheres &&
	dde_vll_drom_curviwdat->have_spheres &&
	!dde_vll_drom_curviwdat->have_textures) {
	/* we can only do hardware spheres if we are not doing texture mapping
	   because the zranges are incompatible.  
	   We chose textures over HW spheres */
#ifdef DEBUG
	printf ("setting Z range for HW spheres\n");
#endif

	xscale = (dde_vll_drom_curwindat->actviewxmax -
		  dde_vll_drom_curwindat->actviewxmin);
	yscale = (dde_vll_drom_curwindat->actviewymax -
		  dde_vll_drom_curwindat->actviewymin);

	zrange = (xscale < yscale ? xscale : yscale) /
		dde_vll_drom_curdevdat->features->zsize;

	zmin = 0.0;
	zmax = zrange;
    } else {
	if (dde_vll_drom_curcamdat->viewtype == DcCameraPerspective) {
	    /*
	     * For proper rendering of textures with a perspective camera
	     * zmax should be 1 and zmin should be hither/yon.
	     * Clamp zmin at 0.05 to avoid artifacts.
	     */
	    zmax = 1.0;
	    zmin = dde_vll_drom_curcamdat->hither / dde_vll_drom_curcamdat->yon;
	    if (zmin < 0.05) zmin = 0.05;
	} else {

	    /* The zrange must be limited to .21 to 1 to prevent artifacts
	       with textures 256x256 (the 1/z table is inaccurate below .21) */
	    zmin = .21;
	    zmax = 1.0;
	}
    }
    VLLset_z_extent (zmin, zmax);

   /*
    * Start the render pass, which in itself may involve many traversals.
    * The order in which traversals occur are:
    *
    *	------> ---
    *   |	 |   texture mapping pass :  Do if view had textures (i.e.,
    *   |	 |		this is the 2nd pass); otherwise, do
    *   |	 |		standard render pass.
    *   |	 |
    *	|	---
    *   |	 |   standard pass :  Do traversal.  If texture mapping is to be
    *   |	 |		done then go to top.  Otherwise, this is either a
    *   -------  |		standard pass or texture lighting pass (the latter
    *    	 |		is established in the primitive rendering code).
    *		---
    *    	 |   transparency pass  (only if no texture mapping)
    *    	 |
    *    	 |
    *    	 |
    *		---
    *    	 |   shadow pass
    *    	 |
    *    	 |
    *    	 |
    *		---
    */
    do {
	ddr_vll_drom_do_background();

	texture_abort = 0;

	if (dde_vll_drom_curviwdat->have_textures) {
	    /* This view had textured geometry last time, so do a texture pass. */

	    dde_vll_drom_data.traversal_type = ddd_vll_drom_texture_pass;
	    VLLset_render_mode(VLL_RENDER_TEXTURE_UV);

	    VLLinit_texture();

	    (*traverse_display) ();

	    if (!dde_vll_drom_data.have_textures) {
		/* This view no longer has any textured geometry */

		dde_vll_drom_curviwdat->have_textures = DcFalse;
	    }

	    if (ddr_vll_drom_check_for_spheres()) {
		/* hardware spheres were discovered in the last traversal
		   and there are no longer textures, so we go back 
		   and do it again, this time with HW spheres */
#ifdef DEBUG
		printf ("aborting texture traversal because HW spheres and no textures\n");
#endif
		return 1;
	    }

	    if (dde_vll_drom_curviwdat->have_textures) {
		ddr_vll_dcm_blend_texture
			((DtShort)dde_vll_drom_curwindat->actviewxmin, 
			 (DtShort)dde_vll_drom_curwindat->actviewymin, 
			 (DtShort)dde_vll_drom_curwindat->actviewxmax, 
			 (DtShort)dde_vll_drom_curwindat->actviewymax,
			 dde_vll_drom_data.have_no_norm_textures);
	    }

	    dde_vll_drom_data.traversal_type = ddd_vll_drom_standard_pass;
	    VLLset_render_mode(VLL_RENDER_STANDARD);

	    /* Lighting rendering pass */

	    ddr_vll_drom_set_prim_execute();

	    (*traverse_display) ();

	    if (ddr_vll_drom_check_for_spheres()) {
#ifdef DEBUG
		printf ("aborting 2nd pass (after texture) because of HW spheres and no textures\n");
#endif
		return 1;
	    }

	    if (dde_vll_drom_curviwdat->have_textures) {
		VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);
		ddr_vll_dcm_blend_texture
			((DtShort)dde_vll_drom_curwindat->actviewxmin, 
			 (DtShort)dde_vll_drom_curwindat->actviewymin, 
			 (DtShort)dde_vll_drom_curwindat->actviewxmax, 
			 (DtShort)dde_vll_drom_curwindat->actviewymax,
			 dde_vll_drom_data.have_no_norm_textures);
	    }
	} else { 
	    /* The view did not have textured geometry last time, */
	    /* so do an opaque rendering pass.  */

	    dde_vll_drom_data.traversal_type = ddd_vll_drom_standard_pass;
	    VLLset_render_mode(VLL_RENDER_STANDARD);

	    ddr_vll_drom_set_prim_execute();

	    (*traverse_display) ();

	    if (ddr_vll_drom_check_for_spheres()) {
		/* We found HW spheres for the first time and there are not any
		   textures so restart so the zrange can be reset */
#ifdef DEBUG
		printf ("aborting traversal because of HW spheres\n");
#endif
		return 1;
	    }

	    if (dde_vll_drom_data.have_textures) {
		/* During the traversal a textured object was encountered
		 * so need to do a texture pass first.
		 * Set the state in the view so next time do the texture
		 * pass right away.
		 */
	 
		dde_vll_drom_curviwdat->have_textures = DcTrue;
		texture_abort = 1;
	    }
	}
    } while (texture_abort);

    /*  Transparent rendering pass, if needed.  */

    if (dde_vll_drom_data.have_transparency) {
	passes = 0;

	do {
	    dde_vll_drom_data.traversal_type = ddd_vll_drom_transp_pass;
	    VLLset_render_mode (VLL_RENDER_STANDARD);

	    ddr_vll_dcm_clear_rectangle_transp_buffers
		    ((DtShort)dde_vll_drom_curwindat->actviewxmin, 
		     (DtShort)dde_vll_drom_curwindat->actviewymin, 
		     (DtShort)dde_vll_drom_curwindat->actviewxmax, 
		     (DtShort)dde_vll_drom_curwindat->actviewymax);

	    /*  Initialize the data needed for handling transparency during
		this pass.  */

	    DDdevice_Flush(dde_vll_drom_data.current_device);

	    ddr_vll_drom_set_prim_execute();

	    (*traverse_display) ();
	 
	    if (ddr_vll_drom_check_for_spheres()) {
#ifdef DEBUG
		printf ("aborting transparent traversal because of HW spheres\n");
#endif
		return 1;
	    }

	    passes++;
	} while (!ddr_vll_dcm_blend_transp
		 ((DtShort)dde_vll_drom_curwindat->actviewxmin, 
		  (DtShort)dde_vll_drom_curwindat->actviewymin, 
		  (DtShort)dde_vll_drom_curwindat->actviewxmax, 
		  (DtShort)dde_vll_drom_curwindat->actviewymax) &&
		 passes < 100);
    }

    DDdevice_Flush(dde_vll_drom_data.current_device);

    if (dde_vll_drom_data.have_shadows) {
	dde_vll_drom_data.traversal_type = ddd_vll_drom_shadow_pass;
	VLLset_render_mode (VLL_RENDER_SHADOWS);

	do {
	    VLLinit_shadows();

	    ddr_vll_drom_set_prim_execute();

	    (*traverse_display) ();
	 
	    if (ddr_vll_drom_check_for_spheres()) {
#ifdef DEBUG
		printf ("aborting shadow traversal because of HW spheres\n");
#endif
		return 1;
	    }

	} while (!VLLblend_shadows());
    }

    if (!dde_vll_drom_data.have_spheres &&
	dde_vll_drom_curviwdat->have_spheres) {
	/*
	 * This view has had spheres in the past but does not
	 * any longer so change the state
	 */
	dde_vll_drom_curviwdat->have_spheres = DcFalse;
    }

    return 0;
}
/*
 ======================================================================
 */
int
ddr_vll_drom_check_for_spheres()
{
    /*
     * return 1 if this is the first time that this view has had spheres 
     * otherwise return 0
     */

    if (dde_vll_drom_curdevdat->use_spheres) {
	if (dde_vll_drom_data.have_spheres &&
	    !dde_vll_drom_curviwdat->have_spheres &&
	    !dde_vll_drom_curviwdat->have_textures) {
	    /*
	     * This is the first time that this view has had spheres
	     * we need to restart !
	     * but only if there are NO textured objects in the view
	     */
	    dde_vll_drom_curviwdat->have_spheres = DcTrue;
	    DDdevice_Flush(dde_vll_drom_data.current_device);
	    return 1;
	}
    }
    return 0;
}

/*
 ======================================================================
 */
void 
ddr_vll_drom_do_background()
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
    DtReal ddr_vll_dcm_convert_y();
    ddt_vll_drom_rstr *dynout_rstr;
    int size, type;

    if (dde_vll_drom_curviwdat->clear_flag) {
	DDdevice_SetBackgroundColor (dde_vll_drom_data.current_device,
				     DcRGB,
				     dde_vll_drom_curviwdat->background_color);
	DDdevice_ClearRectangleDepthColor
		(dde_vll_drom_data.current_device,
		 (DtShort)dde_vll_drom_curwindat->actviewxmin, 
		 (DtShort)dde_vll_drom_curwindat->actviewymin, 
		 (DtShort)dde_vll_drom_curwindat->actviewxmax, 
		 (DtShort)dde_vll_drom_curwindat->actviewymax);

	if (dde_vll_drom_curviwdat->background_raster != DcNullObject) {
	 
	    if (rstr_classid == -1)
		    rstr_classid = DsInqClassId("DoRaster");
	    if (filerstr_classid == -1) 
		    filerstr_classid = DsInqClassId("DoFileRaster");

	    vwidth = (dde_vll_drom_curwindat->actviewxmax - 
		      dde_vll_drom_curwindat->actviewxmin);
	    vheight = (dde_vll_drom_curwindat->actviewymax - 
		       dde_vll_drom_curwindat->actviewymin);

	    raster_object = DDview_ResizeRaster(dde_vll_drom_data.current_view,
						vwidth, vheight);

	    if (raster_object != (DtObjectStructure *)0) {
		rasterid = DsInqObjClassId(raster_object);
	 
		if (rasterid == filerstr_classid) {
		    if (filerstr_dataindex == -1) {
			for (i=0; dde_vll_drom_class_data[i].name != 0; i++) {
			    if (dde_vll_drom_class_data[i].id == filerstr_classid) {
				filerstr_dataindex = dde_vll_drom_class_data[i].index;
				break;
			    }
			}
		    }

		    ddr_vll_drom_filerstr_update (raster_object);

		    raster_object = (DtObjectStructure *)
			    raster_object->additional_data[filerstr_dataindex];
		    if (raster_object == (DtObjectStructure *)0) return;
		    rasterid = DsInqObjClassId(raster_object);
		}
	    }

	    if (rasterid == rstr_classid) {
		if (rstr_dataindex == -1) {
		    for (i=0; dde_vll_drom_class_data[i].name != 0; i++) {
			if (dde_vll_drom_class_data[i].id == rstr_classid) {
			    rstr_dataindex = dde_vll_drom_class_data[i].index;
			    break;
			}
		    }
		}

		rstr = (struct rstr *)raster_object->data;
		dynout_rstr = (ddt_vll_drom_rstr *)
			raster_object->additional_data[rstr_dataindex];

		ddr_vll_drom_rstr_update_background(raster_object);

		if (dynout_rstr->background_data == 0) return;
					       
		deltax = (vwidth - rstr->width);
		deltay = (vheight - rstr->height);
		offsetx = deltax*dde_vll_drom_curviwdat->background_just[0];
		offsety = deltay*(1.0 - dde_vll_drom_curviwdat->background_just[1]);

		if (dynout_rstr->background_mode & ddd_vll_drom_rstr_with_z) {
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
		    data = (unsigned char *)((int)(data) + (-offsety)*rstr->width*size);
		    rheight += deltay;
		    offsety = 0;
		}

		if (deltax < 0) {
		    data = (unsigned char *)((int)(data) + (-offsetx)*size);
		    rwidth += deltax;
		    offsetx = 0;
		}

		x = dde_vll_drom_curwindat->actviewxmin + offsetx;

		/*
		 * Need to convert y coordinate since only the device section
		 * knows about the flip in y.  Since we are using VLLput_image
		 * here directly we need to convert the y coordinate to the
		 * correct device space.  It is not need other places because we
		 * are making 3D calls.
		 */

		y = (DtInt) ddr_vll_dcm_convert_y
			((DtReal)(dde_vll_drom_curwindat->actviewymax - offsety));


/* this is probably useless
		VLLset_output_buffer(VLL_NONDISPLAY_BUFFER);
*/
		VLLput_image(x, y, rwidth, rheight, rstr->width, data, type);
	    }
	}
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_prim_execute()
{

    DDcondex_SetDisablePrimExecFlag (DcFalse);

    switch (dde_vll_drom_data.traversal_type) {
    case ddd_vll_drom_standard_pass:
	/*
	 * Only turn off executability if transparency is enabled AND
	 * texture mapping is not enabled.  Can't do transparent textures 
	 * so textures win.
	 */
	if ((dde_vll_drom_data.transparent_switch) &&
	    !(dde_vll_drom_data.map_diffuse_color || dde_vll_drom_data.map_env)) {
	    DDcondex_SetDisablePrimExecFlag (DcTrue);
	}
	break;

    case ddd_vll_drom_texture_pass:
	if (!dde_vll_drom_data.map_diffuse_color &&
	    !dde_vll_drom_data.map_env) {
	    DDcondex_SetDisablePrimExecFlag (DcTrue);
	}
	break;
   
    case ddd_vll_drom_transp_pass:
	if (!dde_vll_drom_data.transparent_switch) {
	    DDcondex_SetDisablePrimExecFlag (DcTrue);
	}
	break;

    case ddd_vll_drom_shadow_pass:
	if (!dde_vll_drom_data.shadow_switch) {
	    DDcondex_SetDisablePrimExecFlag (DcTrue);
	}
	break;
    }
}
/*
 ======================================================================
 */
