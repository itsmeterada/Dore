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
	int ddr_x11_XFDI_drom_pre_initialization ()
	int ddr_x11_XFDI_drom_post_initialization ()
	int ddr_x11_XFDI_drom_update_local_data ()
	int ddr_x11_XFDI_drom_update_studio (traverse_studio)
	int ddr_x11_XFDI_drom_initialize_before_environment ()
	int ddr_x11_XFDI_drom_initialize_after_environment ()
	ddr_x11_XFDI_drom_start_update ddr_x11_XFDI_drom_start_update (device, view, device_data, view_data, window_data)
	int ddr_x11_XFDI_drom_update_display (traverse_display)
	int ddr_x11_XFDI_drom_render_pass (traverse_display)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_pre_initialization()
{
    extern struct modclpvol *dor_modclpvol_create();
    static DtMatrix4x4 identmat = { 1.,0.,0.,0.,
					    0.,1.,0.,0.,
					    0.,0.,1.,0.,
					    0.,0.,0.,1.};

    dde_x11_XFDI_drom_pre_init = DcTrue;

#ifdef DEBUG
    { int i;
      for (i=0; dde_x11_XFDI_drom_class_data[i].name != 0; i++) {
	  printf ("newclass %d: %s(%d) - %d\n", i,
		  dde_x11_XFDI_drom_class_data[i].name,
		  dde_x11_XFDI_drom_class_data[i].id,
		  dde_x11_XFDI_drom_class_data[i].index);
      }}
#endif

    dde_x11_XFDI_drom_data.matrix_stack.current_element = -1;
    dde_x11_XFDI_drom_data.matrix_stack.last_element = 9;
    dde_x11_XFDI_drom_data.matrix_stack.elements = 
	    (ddt_x11_XFDI_drom_matrix_element *) DDspace_Allocate
		    (10*sizeof (ddt_x11_XFDI_drom_matrix_element));

    XFDISetMatrix(dde_x11_XFDI_drom_curdevdat->display,
		  dde_x11_XFDI_drom_curdevdat->gc,
		  XFDIObjectMatrix,
		  identmat,
		  XFDIMatrix4x4);
    XFDISetMatrix(dde_x11_XFDI_drom_curdevdat->display,
		  dde_x11_XFDI_drom_curdevdat->gc,
		  XFDILightMatrix,
		  identmat,
		  XFDIMatrix4x4);
    XFDISetMatrix(dde_x11_XFDI_drom_curdevdat->display,
		  dde_x11_XFDI_drom_curdevdat->gc,
		  XFDINormalMatrix,
		  identmat,
		  XFDIMatrix4x4);
   
    dde_x11_XFDI_drom_data.transparent_surface_pass = DcFalse;

    dde_x11_XFDI_drom_data.max_cvt_colors = 0;
    dde_x11_XFDI_drom_data.converted_colors = NULL;

    dde_x11_XFDI_drom_data.clpvol_stack.current_element = 0;
    dde_x11_XFDI_drom_data.clpvol_stack.last_element = 9;
    dde_x11_XFDI_drom_data.clpvol_stack.elements = 
	    (ddt_x11_XFDI_drom_clpvol_element *) DDspace_Allocate 
		    (10*sizeof (ddt_x11_XFDI_drom_clpvol_element));
    dde_x11_XFDI_drom_data.clpvol_stack.elements[0].modclpvol =
	    dor_modclpvol_create (identmat,DcClipNone,0,NULL);
    ddr_x11_XFDI_drom_update_clpvol(dde_x11_XFDI_drom_data.clpvol_stack.elements[0].modclpvol);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_post_initialization()
{
    dde_x11_XFDI_drom_pre_init = DcFalse;

    XFDISetClipLevel(dde_x11_XFDI_drom_curdevdat->display,
		     dde_x11_XFDI_drom_curdevdat->gc,
		     XFDIClipClipPlanes);

    XFDISetMarkerScale(dde_x11_XFDI_drom_curdevdat->display,
		       dde_x11_XFDI_drom_curdevdat->gc,
		       1./16.);

    XFDISetSurfaceFlipNormals(dde_x11_XFDI_drom_curdevdat->display,
			      dde_x11_XFDI_drom_curdevdat->gc,
			      1);

    XFDISetSurfaceStyle(dde_x11_XFDI_drom_curdevdat->display,
			dde_x11_XFDI_drom_curdevdat->gc, 
			XFDIShaded);

    XFDISetVectorFlag(dde_x11_XFDI_drom_curdevdat->display,
		      dde_x11_XFDI_drom_curdevdat->gc,
		      XFDIObjectMatrix,XFDIVectorPoint);

    XFDISetVectorFlag(dde_x11_XFDI_drom_curdevdat->display,
		      dde_x11_XFDI_drom_curdevdat->gc,
		      XFDILightMatrix,XFDIVectorNoHomo);

    XFDISetVectorFlag(dde_x11_XFDI_drom_curdevdat->display,
		      dde_x11_XFDI_drom_curdevdat->gc,
		      XFDINormalMatrix,XFDIVectorNormal);

    XFDISetFilter(dde_x11_XFDI_drom_curdevdat->display,
		  dde_x11_XFDI_drom_curdevdat->gc,
		  XFDIFilterNone);

    ddr_x11_XFDI_drom_update_surface_properties();

    if (dde_x11_XFDI_drom_data.backface_cullable_switch &&
	dde_x11_XFDI_drom_data.backface_cull_switch) {
	XFDISetBackfaceEliminateFlag(dde_x11_XFDI_drom_curdevdat->display,
				     dde_x11_XFDI_drom_curdevdat->gc,
				     XFDICullClockwise);
    } else {
	XFDISetBackfaceEliminateFlag(dde_x11_XFDI_drom_curdevdat->display,
				     dde_x11_XFDI_drom_curdevdat->gc,
				     XFDICullOff);
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_update_local_data()		
{
    DtInt shaderange[2];
    DtInt nclrs;
    Display *ddr_x11_XFDI_dcm_inquire_display();
    XFDIGC ddr_x11_XFDI_dcm_inquire_gc();
    DtVolume *actviewport;
    DtReal scale[3], trans[3];
    DtReal ftodtrans[3];

    actviewport = DDdevice_InqActualViewport (dde_x11_XFDI_drom_data.current_device);
			   
    dde_x11_XFDI_drom_curwindat->actviewxmin = actviewport->bll[0];
    dde_x11_XFDI_drom_curwindat->actviewymin = 1024. - actviewport->fur[1];
    dde_x11_XFDI_drom_curwindat->actviewxmax = actviewport->fur[0];
    dde_x11_XFDI_drom_curwindat->actviewymax = 1024. - actviewport->bll[1];


    dde_x11_XFDI_drom_curdevdat->stereoswitch = DDdevice_InqStereo (dde_x11_XFDI_drom_data.current_device);

    shaderange[0] = -10;
    shaderange[1] = -10;
    DdInqShadeRanges (dde_x11_XFDI_drom_data.current_device,
				dde_x11_XFDI_drom_data.shade_index,1,
				shaderange);
    if (shaderange[0] != -10 && shaderange[1] != -10) {
	dde_x11_XFDI_drom_curdevdat->current_shade_range_min = shaderange[0];
	dde_x11_XFDI_drom_curdevdat->current_shade_range_max = shaderange[1];
	dde_x11_XFDI_drom_curdevdat->shade_range_scale = 
		(shaderange[1] - shaderange[0]);
    } else {
	dde_x11_XFDI_drom_curdevdat->current_shade_range_min = 0;
	dde_x11_XFDI_drom_curdevdat->current_shade_range_max = 255;
	dde_x11_XFDI_drom_curdevdat->shade_range_scale = 256; 
    }
    if (dde_x11_XFDI_drom_curdevdat->visual_type ==DcPseudoColor) {
	XFDISetMarkerBackground (dde_x11_XFDI_drom_curdevdat->display,
				 dde_x11_XFDI_drom_curdevdat->gc,
				 dde_x11_XFDI_drom_curdevdat->current_shade_range_min);
   
	XFDISetLineBackground (dde_x11_XFDI_drom_curdevdat->display,
			       dde_x11_XFDI_drom_curdevdat->gc,
			       dde_x11_XFDI_drom_curdevdat->current_shade_range_min);

	XFDISetSurfaceBackground (dde_x11_XFDI_drom_curdevdat->display,
				  dde_x11_XFDI_drom_curdevdat->gc,
				  dde_x11_XFDI_drom_curdevdat->current_shade_range_min);
    }
    ddr_x11_XFDI_dcm_inq_pseudocolor_weighted_average_weights
	    (&(dde_x11_XFDI_drom_curdevdat->cwtavgpsweight));

    DDdevice_InqFrustumScaleTrans (dde_x11_XFDI_drom_data.current_device,scale,trans);

    DDdevice_InqFrameScaleTrans (dde_x11_XFDI_drom_data.current_device,
				 dde_x11_XFDI_drom_curdevdat->ftodscale,
				 ftodtrans);
     
    dde_x11_XFDI_drom_curdevdat->annoscale[0] = 
	    dde_x11_XFDI_drom_curdevdat->ftodscale[0] / scale[0];
    dde_x11_XFDI_drom_curdevdat->annoscale[1] = 
	    dde_x11_XFDI_drom_curdevdat->ftodscale[1] / scale[1];
    dde_x11_XFDI_drom_curdevdat->annoscale[2] = 
	    dde_x11_XFDI_drom_curdevdat->ftodscale[2] /
		    scale[2];

    dde_x11_XFDI_drom_curviwdat->clear_flag = 
	    DvInqClearFlag(dde_x11_XFDI_drom_data.current_view);

    DvInqBackgroundColor (dde_x11_XFDI_drom_data.current_view,
				  &dde_x11_XFDI_drom_curviwdat->
				  background_color_model,dde_x11_XFDI_drom_curviwdat->background_color);

    dde_x11_XFDI_drom_curdevdat->shade_mode = DdInqShadeMode (dde_x11_XFDI_drom_data.current_device);

    dde_x11_XFDI_drom_curdevdat->visual_type = DdInqVisualType (dde_x11_XFDI_drom_data.current_device);

}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_update_studio(traverse_studio)
     DtPFI traverse_studio;
{
    ddr_x11_XFDI_drom_initialize_before_environment();

    (*traverse_studio)();

    ddr_x11_XFDI_drom_initialize_after_environment();
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_initialize_before_environment()
{
    DtInt i;
    DtInt lightidx;
   
#ifdef DEBUG
    printf ("initializing all %d lights off\n",
	    dde_x11_XFDI_drom_curviwdat->total_lights);
#endif

    for (lightidx = 0; lightidx < dde_x11_XFDI_drom_curviwdat->total_lights; 
	 lightidx++) {
	XFDISetLightStatus(dde_x11_XFDI_drom_curdevdat->display,
			   dde_x11_XFDI_drom_curdevdat->gc,
			   lightidx, XFDIOff);
    }

    dde_x11_XFDI_drom_curviwdat->total_lights = 0;
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_initialize_after_environment()

{
    DtReal hnrm[3];
    DtReal stereo_rotation;
    extern DtReal dor_math_atan();

    if (dde_x11_XFDI_drom_curcamdat->stereoswitch &&
	dde_x11_XFDI_drom_curdevdat->stereoswitch) {
#ifdef DEBUG
	printf ("computing stereo matrices\n");
#endif
	if (dde_x11_XFDI_drom_curcamdat->stereo_distance != 0.0) {
	    stereo_rotation = 
		    dor_math_atan(dde_x11_XFDI_drom_curcamdat->stereo_eyeseparation / 
				  dde_x11_XFDI_drom_curcamdat->stereo_distance);
	} else {
	    stereo_rotation = 0.0;
	}

	/*  Left eye view matrix.  */

	dor_matrix_load_rotate_y(dde_x11_XFDI_drom_curcamdat->world_to_camera_matrix,
				 -stereo_rotation);

	dor_matrix_translate(dde_x11_XFDI_drom_curcamdat->world_to_camera_matrix,
			     -dde_x11_XFDI_drom_curcamdat->stereo_eyeseparation,0.,0.,
			     DcPostConcatenate);

	dor_matrix_concat_fast(dde_x11_XFDI_drom_curcamdat->world_to_camera_matrix,
			       dde_x11_XFDI_drom_curcamdat->camera_to_world_matrix,
			       DcPreConcatenate);

	dor_matrix_invert(dde_x11_XFDI_drom_curcamdat->world_to_camera_matrix);

	ddr_x11_XFDI_drom_compute_view_parameters();

	dor_matrix_load_fast(dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix_left,
			     dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix);

	dor_matrix_load_fast(dde_x11_XFDI_drom_curcamdat->tspfcstowcsmat_left,
			     dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix_left);
	dor_matrix_invert(dde_x11_XFDI_drom_curcamdat->tspfcstowcsmat_left);
	dor_matrix_transpose(dde_x11_XFDI_drom_curcamdat->tspfcstowcsmat_left);

	/*  Right eye view matrix.  */

	dor_matrix_load_rotate_y(dde_x11_XFDI_drom_curcamdat->world_to_camera_matrix,
				 stereo_rotation);
	dor_matrix_translate(dde_x11_XFDI_drom_curcamdat->world_to_camera_matrix,
			     dde_x11_XFDI_drom_curcamdat->stereo_eyeseparation,0.,0.,
			     DcPostConcatenate);

	dor_matrix_concat_fast(dde_x11_XFDI_drom_curcamdat->world_to_camera_matrix,
			       dde_x11_XFDI_drom_curcamdat->camera_to_world_matrix,
			       DcPreConcatenate);
	dor_matrix_invert(dde_x11_XFDI_drom_curcamdat->world_to_camera_matrix);

	ddr_x11_XFDI_drom_compute_view_parameters();

	dor_matrix_load_fast(dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix_right,
			     dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix);

	dor_matrix_load_fast(dde_x11_XFDI_drom_curcamdat->tspfcstowcsmat_right,
			     dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix_right);
	dor_matrix_invert(dde_x11_XFDI_drom_curcamdat->tspfcstowcsmat_right);
	dor_matrix_transpose(dde_x11_XFDI_drom_curcamdat->tspfcstowcsmat_right);
    } else {
#ifdef DEBUG
	printf ("computing standard matrices\n");
#endif
	dor_matrix_load(dde_x11_XFDI_drom_curcamdat->world_to_camera_matrix,
			dde_x11_XFDI_drom_curcamdat->camera_to_world_matrix);
	dor_matrix_invert(dde_x11_XFDI_drom_curcamdat->world_to_camera_matrix);

	ddr_x11_XFDI_drom_compute_view_parameters();
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_start_update (device, view, device_data, view_data, window_data)
     DtObject device;
     DtObject view;
     ddt_x11_XFDI_drom_device *device_data;
     ddt_x11_XFDI_drom_view *view_data;
     ddt_x11_XFDI_drom_window *window_data;
{
    dde_x11_XFDI_drom_data.current_device = device;
    dde_x11_XFDI_drom_data.current_view = view;

    dde_x11_XFDI_drom_curdevdat = device_data;
    dde_x11_XFDI_drom_curviwdat = view_data;
    dde_x11_XFDI_drom_curwindat = window_data;

    dde_x11_XFDI_drom_curcamdat = &(view_data->camera_data);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_update_display(traverse_display)
     DtPFI traverse_display;
{
    DtMatrix4x4 tmpmatrix;
    int rtn;

    ddr_x11_XFDI_dcm_set_viewport(dde_x11_XFDI_drom_curwindat->actviewxmin, 
				    dde_x11_XFDI_drom_curwindat->actviewymin, 
				    dde_x11_XFDI_drom_curwindat->actviewxmax, 
				    dde_x11_XFDI_drom_curwindat->actviewymax);

    if (dde_x11_XFDI_drom_curcamdat->camtype == DcParallel) {
#ifdef DEBUG 
	printf ("camera is Parallel -- VectorNoHomo\n");
#endif
	XFDISetVectorFlag(dde_x11_XFDI_drom_curdevdat->display,
			  dde_x11_XFDI_drom_curdevdat->gc,
			  XFDIObjectMatrix,XFDIVectorNoHomo);
    } else {
#ifdef DEBUG
	printf ("camera is Perspective -- VectorPoint\n");
#endif
	XFDISetVectorFlag(dde_x11_XFDI_drom_curdevdat->display,
			  dde_x11_XFDI_drom_curdevdat->gc,
			  XFDIObjectMatrix,XFDIVectorPoint);
    }

#ifdef DEBUG
    printf ("SetView (%g %g %g) \n", 
	    dde_x11_XFDI_drom_curcamdat->wcscamloc[0],
	    dde_x11_XFDI_drom_curcamdat->wcscamloc[1],
	    dde_x11_XFDI_drom_curcamdat->wcscamloc[2]);
#endif

    XFDISetView(dde_x11_XFDI_drom_curdevdat->display,
		dde_x11_XFDI_drom_curdevdat->gc,
		dde_x11_XFDI_drom_curcamdat->wcscamloc);

    if (dde_x11_XFDI_drom_curdevdat->stereoswitch) {
	ddr_x11_XFDI_dcm_set_stereo_eye (0);
    }
    if (dde_x11_XFDI_drom_curdevdat->stereoswitch &&
	dde_x11_XFDI_drom_curcamdat->stereoswitch) {
	dor_matrix_load(dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix,
			dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix_left);
    }

    do {
	if (dde_x11_XFDI_drom_curviwdat->clear_flag) {
	    DDdevice_SetBackgroundColor 
		    (dde_x11_XFDI_drom_data.current_device,
		     dde_x11_XFDI_drom_curviwdat->background_color);

	    DDdevice_ClearRectangleDepthColor
		    (dde_x11_XFDI_drom_data.current_device,
		     (DtShort)dde_x11_XFDI_drom_curwindat->actviewxmin, 
		     (DtShort)dde_x11_XFDI_drom_curwindat->actviewymin, 
		     (DtShort)dde_x11_XFDI_drom_curwindat->actviewxmax, 
		     (DtShort)dde_x11_XFDI_drom_curwindat->actviewymax);
	}

	dor_matrix_load(tmpmatrix, dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix);
	dor_matrix_transpose(tmpmatrix);
	XFDISetMatrix(dde_x11_XFDI_drom_curdevdat->display,
		      dde_x11_XFDI_drom_curdevdat->gc,
		      XFDIObjectMatrix,
		      tmpmatrix,
		      XFDIMatrix4x4);

	/*
	 * ddr_x11_XFDI_drom_render_pass returns -1 if need to restart as a result
	 * of spheres
	 */
	rtn = ddr_x11_XFDI_drom_render_pass(traverse_display);

    } while (rtn == -1);

    DDdevice_Flush(dde_x11_XFDI_drom_data.current_device);

    if (dde_x11_XFDI_drom_curdevdat->stereoswitch) {
	ddr_x11_XFDI_dcm_set_stereo_eye(1);

	if (dde_x11_XFDI_drom_curcamdat->stereoswitch) {
	    dor_matrix_load(dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix,
			    dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix_right);
	}
	do {
	    if (dde_x11_XFDI_drom_curviwdat->clear_flag) {
		DDdevice_SetBackgroundColor 
			(dde_x11_XFDI_drom_data.current_device,
			 dde_x11_XFDI_drom_curviwdat->background_color);

		DDdevice_ClearRectangleDepthColor
			(dde_x11_XFDI_drom_data.current_device,
			 (DtShort)dde_x11_XFDI_drom_curwindat->actviewxmin, 
			 (DtShort)dde_x11_XFDI_drom_curwindat->actviewymin, 
			 (DtShort)dde_x11_XFDI_drom_curwindat->actviewxmax, 
			 (DtShort)dde_x11_XFDI_drom_curwindat->actviewymax);
	    }

	    dor_matrix_load(tmpmatrix, dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix);
	    dor_matrix_transpose(tmpmatrix);
	    XFDISetMatrix(dde_x11_XFDI_drom_curdevdat->display,
			  dde_x11_XFDI_drom_curdevdat->gc,
			  XFDIObjectMatrix,
			  tmpmatrix,
			  XFDIMatrix4x4);

	    /*
	     * ddr_x11_XFDI_drom_render_pass returns -1 if need to restart as a result
	     * of spheres
	     */
	    rtn = ddr_x11_XFDI_drom_render_pass(traverse_display);

	} while (rtn == -1);
    }
}
/*
 ======================================================================
 */

#define THRESHOLD 5
      
int
ddr_x11_XFDI_drom_render_pass(traverse_display)
     DtPFI traverse_display;
{
    int pixels;
    int passes;
    float zrange;
    int texture_abort;
    float xscale, yscale, zscale;

    /*  Initialize the data needed for handling transparency during
	this pass.  */

    XFDISetSurfaceTransparency(dde_x11_XFDI_drom_curdevdat->display,
			       dde_x11_XFDI_drom_curdevdat->gc,
			       0.0);

    dde_x11_XFDI_drom_data.transparent_surface_flag = DcFalse;
    dde_x11_XFDI_drom_data.transparent_surface_pass = DcFalse;
    dde_x11_XFDI_drom_data.texture_pass = DcFalse;
    dde_x11_XFDI_drom_data.texture_pass_required = DcFalse;
    dde_x11_XFDI_drom_data.texture_no_norms = DcFalse;
    DDcondex_SetDisablePrimExecFlag (DcFalse);

    dde_x11_XFDI_drom_data.spheres_required = DcFalse;

    if (dde_x11_XFDI_drom_curdevdat->use_spheres &&
	dde_x11_XFDI_drom_curviwdat->have_spheres) {
	xscale = (dde_x11_XFDI_drom_curwindat->actviewxmax -
		  dde_x11_XFDI_drom_curwindat->actviewxmin) *
			  dde_x11_XFDI_drom_curcamdat->projmat[0][0];
	yscale = (dde_x11_XFDI_drom_curwindat->actviewymax -
		  dde_x11_XFDI_drom_curwindat->actviewymin) *
			  dde_x11_XFDI_drom_curcamdat->projmat[1][1];

	/* the total zsclae in projmat is adjusted by a scale and translate
	   to correct for the zrange on the GS being -1 to 1 rather than
	   0 to -1 as in the Dore' model */
	 
	zscale = (dde_x11_XFDI_drom_curdevdat->visual_type == DcPseudoColor ?
		  65535. : 4294967295.) * 
			  (2.0 * dde_x11_XFDI_drom_curcamdat->projmat[2][2] +
			   dde_x11_XFDI_drom_curcamdat->projmat[3][2]);

	zrange = 0.5 * (xscale < yscale ? xscale : yscale) / zscale;
    } else {
	zrange = 0.9;
    }
    XFDISetZRange(dde_x11_XFDI_drom_curdevdat->display,
		  dde_x11_XFDI_drom_curdevdat->gc,zrange,-zrange);


    do {
	texture_abort = 0;

	if (dde_x11_XFDI_drom_curviwdat->texture_pass_needed) {
	    /* This view last time had textured geometry */

	    dde_x11_XFDI_drom_data.texture_pass = DcTrue;

	    (*traverse_display) ();

	    if (dde_x11_XFDI_drom_curdevdat->use_spheres) {
		if (dde_x11_XFDI_drom_data.spheres_required &&
		    !dde_x11_XFDI_drom_curviwdat->have_spheres) {
		    /*
		     * This is the first time that this view has had spheres
		     * we need to restart !
		     */
		    dde_x11_XFDI_drom_curviwdat->have_spheres = DcTrue;
		    DDdevice_Flush(dde_x11_XFDI_drom_data.current_device);
		    return -1;
		}
		if (!dde_x11_XFDI_drom_data.spheres_required &&
		    dde_x11_XFDI_drom_curviwdat->have_spheres) {
		    /*
		     * This view has had spheres in the past but does not
		     * any longer so change the state
		     */
		    dde_x11_XFDI_drom_curviwdat->have_spheres = DcFalse;
		}
	    }

	    if (!dde_x11_XFDI_drom_data.texture_pass_required) {
		/* This view no longer has any textured geometry */

		dde_x11_XFDI_drom_curviwdat->texture_pass_needed = DcFalse;
	    } else {
		ddr_x11_XFDI_dcm_blend_texture
			((DtShort)dde_x11_XFDI_drom_curwindat->actviewxmin, 
			 (DtShort)dde_x11_XFDI_drom_curwindat->actviewymin, 
			 (DtShort)dde_x11_XFDI_drom_curwindat->actviewxmax, 
			 (DtShort)dde_x11_XFDI_drom_curwindat->actviewymax,
			 dde_x11_XFDI_drom_data.texture_no_norms);
	    }

	    dde_x11_XFDI_drom_data.texture_pass = DcFalse;

	    /* Opaque rendering pass */

	    DDcondex_SetDisablePrimExecFlag (DcFalse);

	    (*traverse_display) ();

	    if (dde_x11_XFDI_drom_curdevdat->use_spheres) {
		if (dde_x11_XFDI_drom_data.spheres_required &&
		    !dde_x11_XFDI_drom_curviwdat->have_spheres) {
		    /*
		     * This is the first time that this view has had spheres
		     * we need to restart !
		     */
		    dde_x11_XFDI_drom_curviwdat->have_spheres = DcTrue;
		    DDdevice_Flush(dde_x11_XFDI_drom_data.current_device);
		    return -1;
		}
		if (!dde_x11_XFDI_drom_data.spheres_required &&
		    dde_x11_XFDI_drom_curviwdat->have_spheres) {
		    /*
		     * This view has had spheres in the past but does not
		     * any longer so change the state
		     */
		    dde_x11_XFDI_drom_curviwdat->have_spheres = DcFalse;
		}
	    }

	} else { 
	    /* Ths view last time did not have textured geometry */
	    /*  Opaque rendering pass.  */

	    DDcondex_SetDisablePrimExecFlag (DcFalse);

	    (*traverse_display) ();

	    if (dde_x11_XFDI_drom_curdevdat->use_spheres) {
		if (dde_x11_XFDI_drom_data.spheres_required &&
		    !dde_x11_XFDI_drom_curviwdat->have_spheres) {
		    /*
		     * This is the first time that this view has had spheres
		     * we need to restart !
		     */
		    dde_x11_XFDI_drom_curviwdat->have_spheres = DcTrue;
		    DDdevice_Flush(dde_x11_XFDI_drom_data.current_device);
		    return -1;
		}
		if (!dde_x11_XFDI_drom_data.spheres_required &&
		    dde_x11_XFDI_drom_curviwdat->have_spheres) {
		    /*
		     * This view has had spheres in the past but does not
		     * any longer so change the state
		     */
		    dde_x11_XFDI_drom_curviwdat->have_spheres = DcFalse;
		}
	    }

	    if (dde_x11_XFDI_drom_data.texture_pass_required) {
		/* During the traversal a textured object was encountered
		 * so need to do a texture pass first.
		 * Set the state in the view so next time do the texture
		 * pass right away.
		 */
	 
		dde_x11_XFDI_drom_curviwdat->texture_pass_needed = DcTrue;
		texture_abort = 1;
	    }
	}
    } while (texture_abort);

    /*  Transparent rendering pass, if needed.  */

    if (dde_x11_XFDI_drom_data.transparent_surface_flag) {
	passes = 0;

	do {
	    dde_x11_XFDI_drom_data.transparent_surface_pass = DcTrue;

	    ddr_x11_XFDI_dcm_clear_rectangle_transp_buffers
		    ((DtShort)dde_x11_XFDI_drom_curwindat->actviewxmin, 
		     (DtShort)dde_x11_XFDI_drom_curwindat->actviewymin, 
		     (DtShort)dde_x11_XFDI_drom_curwindat->actviewxmax, 
		     (DtShort)dde_x11_XFDI_drom_curwindat->actviewymax);

	    /*  Initialize the data needed for handling transparency during
		this pass.  */

	    DDdevice_Flush(dde_x11_XFDI_drom_data.current_device);

	    if (dde_x11_XFDI_drom_data.interptype == DcConstantShade) {
		/*
		 * NOTE: Flat shading and transparency do not work together
		 * so use Gouraud shading instead.
		 */
		XFDISetSurfaceShading(dde_x11_XFDI_drom_curdevdat->display,
				      dde_x11_XFDI_drom_curdevdat->gc,
				      XFDIShadeColor);
	    }

	    if (dde_x11_XFDI_drom_data.transparent_switch)   {
		DDcondex_SetDisablePrimExecFlag (DcFalse);
	    } else {
		DDcondex_SetDisablePrimExecFlag (DcTrue);
	    }

	    (*traverse_display) ();
	 
	    if (dde_x11_XFDI_drom_data.interptype == DcConstantShade) {
		XFDISetSurfaceShading(dde_x11_XFDI_drom_curdevdat->display,
				      dde_x11_XFDI_drom_curdevdat->gc,
				      XFDIShadeNone);
	    }

	    if (dde_x11_XFDI_drom_curdevdat->use_spheres) {
		if (dde_x11_XFDI_drom_data.spheres_required &&
		    !dde_x11_XFDI_drom_curviwdat->have_spheres) {
		    /*
		     * This is the first time that this view has had spheres
		     * we need to restart !
		     */
		    dde_x11_XFDI_drom_curviwdat->have_spheres = DcTrue;
		    DDdevice_Flush(dde_x11_XFDI_drom_data.current_device);
		    return -1;
		}
		if (!dde_x11_XFDI_drom_data.spheres_required &&
		    dde_x11_XFDI_drom_curviwdat->have_spheres) {
		    /*
		     * This view has had spheres in the past but does not
		     * any longer so change the state
		     */
		    dde_x11_XFDI_drom_curviwdat->have_spheres = DcFalse;
		}
	    }

	    ddr_x11_XFDI_dcm_blend_transp
		    ((DtShort)dde_x11_XFDI_drom_curwindat->actviewxmin, 
		     (DtShort)dde_x11_XFDI_drom_curwindat->actviewymin, 
		     (DtShort)dde_x11_XFDI_drom_curwindat->actviewxmax, 
		     (DtShort)dde_x11_XFDI_drom_curwindat->actviewymax,
		     &pixels);

	    passes++;
	} while (pixels > THRESHOLD);
#ifdef DEBUG
	printf ("Multi-pass Transparency required %d passes with %d pixels unresolved\n",
		passes, pixels);
#endif
    	dde_x11_XFDI_drom_data.transparent_surface_flag = DcFalse;
    }

    DDdevice_Flush(dde_x11_XFDI_drom_data.current_device);

    return 0;
}
