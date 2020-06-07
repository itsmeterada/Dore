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
	int ddr_x11_DGL_drom_pre_initialization ()
	int ddr_x11_DGL_drom_post_initialization ()
	int ddr_x11_DGL_drom_update_local_data ()
	int ddr_x11_DGL_drom_update_studio (traverse_studio)
	int ddr_x11_DGL_drom_initialize_before_environment ()
	int ddr_x11_DGL_drom_initialize_after_environment ()
	endif ddr_x11_DGL_drom_start_update (device, view, device_data, view_data, window_data)
	int ddr_x11_DGL_drom_update_display (traverse_display)
	int ddr_x11_DGL_drom_render_pass (traverse_display)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */

static int initialized = 0;


ddr_x11_DGL_drom_pre_initialization()
{
    struct modclpvol *dor_modclpvol_create();
    DtMatrix4x4 identmat;

    dde_x11_DGL_drom_pre_init = DcTrue;

    if (!initialized) {
	/* the top of the stack is dde_x11_DGL_drom_data.object_lcstowcsmat */

	dde_x11_DGL_drom_data.mat_stack.current_element = -1;
	dde_x11_DGL_drom_data.mat_stack.last_element = 9;
	dde_x11_DGL_drom_data.mat_stack.elements = (struct mat_element *)
		DDspace_Allocate (10*sizeof (struct mat_element));
	XdMatrixLoadIdentity(dde_x11_DGL_drom_data.object_lcstowcsmat);

	dde_x11_DGL_drom_data.clpvol_stack.current_element = 0;
	dde_x11_DGL_drom_data.clpvol_stack.last_element = 9;
	dde_x11_DGL_drom_data.clpvol_stack.elements = (struct clpvol_element *)
		DDspace_Allocate (10*sizeof (struct clpvol_element));
	dor_matrix_load_identity(identmat);
	dde_x11_DGL_drom_data.clpvol_stack.elements[0].modclpvol = 
		dor_modclpvol_create (identmat,DcClipNone,0,NULL);
	ddr_x11_DGL_drom_update_clpvol(dde_x11_DGL_drom_data.clpvol_stack.elements[0].modclpvol);

	initialized = 1;
    }
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_post_initialization()
{
    /*  World to local normal transformation matrix is the transpose of
	the inverse of the wcstolcsmat.  Since lcstowcsmat is already the
	inverse of wcstolcsmat, we merely need to take the transpose of this
	matrix.  */

    dde_x11_DGL_drom_pre_init = DcFalse;

    XdMatrixLoadIdentity(dde_x11_DGL_drom_data.nrmwcstolcsmat);
    XdMatrixTranspose(dde_x11_DGL_drom_data.nrmwcstolcsmat);

}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_update_local_data()      
{
    DtVolume *actviewport;
    DtReal actviewxmin, actviewymin, actviewxmax, actviewymax;
    DtInt rno;
    DtInt cnt;
    DtInt cliprectcount;
    DtReal *cliprects;
    DtReal windowxmin, windowymin, windowxmax, windowymax;
    DtInt shaderange[2];
    DtReal extent[4];
    DtFlag partiallyobscuredflag;
    DtReal scale[3], trans[3];
    XdReal *cwtavgpsweight;
    XdReal *cbtselpsweight;
    int    *cbtselpsmask;
    DtReal  ftodtrans[3];

    actviewport = DDdevice_InqActualViewport (dde_x11_DGL_drom_data.current_device);
			   
    actviewxmin = actviewport->bll[0];
    actviewymin = 1024. - actviewport->fur[1];
    actviewxmax = actviewport->fur[0];
    actviewymax = 1024. - actviewport->bll[1];

#ifdef DEBUG
    printf ("actviewport=(%g %g %g) (%g %g %g)\n",
	    actviewxmin, actviewymin, actviewport->bll[2],
	    actviewxmax, actviewymax, actviewport->fur[2]);
#endif

    dde_x11_DGL_drom_curdevdat->stereoswitch = DDdevice_InqStereo (dde_x11_DGL_drom_data.current_device);

    DDdevice_InqClipList (dde_x11_DGL_drom_data.current_device,
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

#ifdef DEBUG
	printf ("window: (%g %g) (%g %g)\n",
		windowxmin, windowymin, windowxmax, windowymax);
#endif

	dde_x11_DGL_drom_curwindat->vis_rect[rno][0] = windowxmin;
	dde_x11_DGL_drom_curwindat->vis_rect[rno][1] = windowymin;
	dde_x11_DGL_drom_curwindat->vis_rect[rno][2] = windowxmax;
	dde_x11_DGL_drom_curwindat->vis_rect[rno][3] = windowymax;

	rno++;
    }

    dde_x11_DGL_drom_curwindat->clipping_rectangle_count = rno;

    if (dde_x11_DGL_drom_curwindat->clipping_rectangle_count == 0) {
	dde_x11_DGL_drom_curwindat->visibility_state =
		ddd_x11_DGL_drom_window_completely_obscured;

    } else if (!partiallyobscuredflag) {
	dde_x11_DGL_drom_curwindat->visibility_state =
		ddd_x11_DGL_drom_window_completely_visible;
    } else {
	dde_x11_DGL_drom_curwindat->visibility_state =
		ddd_x11_DGL_drom_window_partially_obscured;
    }

#ifdef DEBUG
    printf ("shade index = %d\n", dde_x11_DGL_drom_data.object_shade_index);
#endif

    DdInqShadeRanges (dde_x11_DGL_drom_data.current_device,
				dde_x11_DGL_drom_data.object_shade_index,
				1,shaderange);
    dde_x11_DGL_drom_curdevdat->current_shade_range_min = shaderange[0];
    dde_x11_DGL_drom_curdevdat->current_shade_range_max = shaderange[1];
   
    DDdevice_InqFrustumScaleTrans (dde_x11_DGL_drom_data.current_device,
				scale,trans);
    /* trans[0] -= 1.; */
    trans[1] -= 1.;

    DDdevice_InqFrameScaleTrans (dde_x11_DGL_drom_data.current_device,
				 dde_x11_DGL_drom_curdevdat->ftodscale,
				 ftodtrans);

    dde_x11_DGL_drom_curdevdat->annoscale[0] = 
	    dde_x11_DGL_drom_curdevdat->ftodscale[0] / scale[0];
    dde_x11_DGL_drom_curdevdat->annoscale[1] = 
	    dde_x11_DGL_drom_curdevdat->ftodscale[1] / scale[1];
    dde_x11_DGL_drom_curdevdat->annoscale[2] = 
	    dde_x11_DGL_drom_curdevdat->ftodscale[2] /
		    scale[2];

    dde_x11_DGL_drom_curdevdat->ctodscale[0] = scale[0];
    dde_x11_DGL_drom_curdevdat->ctodscale[1] = -1. * scale[1];
    dde_x11_DGL_drom_curdevdat->ctodscale[2] = scale[2];

    dde_x11_DGL_drom_curdevdat->ctodtrans[0] = trans[0];
    dde_x11_DGL_drom_curdevdat->ctodtrans[1] = 1023. - trans[1];
    dde_x11_DGL_drom_curdevdat->ctodtrans[2] = trans[2];

    dde_x11_DGL_drom_curwindat->extent[0] = actviewxmin;
    dde_x11_DGL_drom_curwindat->extent[1] = actviewymin;
    dde_x11_DGL_drom_curwindat->extent[2] = actviewxmax;
    dde_x11_DGL_drom_curwindat->extent[3] = actviewymax;

    dde_x11_DGL_drom_curviwdat->clear_flag = 
	    DvInqClearFlag(dde_x11_DGL_drom_data.current_view);

    DvInqBackgroundColor (dde_x11_DGL_drom_data.current_view,
				  &dde_x11_DGL_drom_curviwdat->background_color_model,
				  dde_x11_DGL_drom_curviwdat->background_color);

    dde_x11_DGL_drom_curdevdat->shade_mode =
	    DdInqShadeMode (dde_x11_DGL_drom_data.current_device);

    dde_x11_DGL_drom_curdevdat->visual_type =
	    DdInqVisualType (dde_x11_DGL_drom_data.current_device);

    if (dde_x11_DGL_drom_curdevdat->visual_type == DcDirectColor) {
	dde_x11_DGL_drom_data.pseudocolor_switch = DcFalse;

    } else if (dde_x11_DGL_drom_curdevdat->visual_type == DcPseudoColor) {
	dde_x11_DGL_drom_data.pseudocolor_switch = DcTrue;

	dde_x11_DGL_drom_curviwdat->shade_index = 
		DvInqShadeIndex(dde_x11_DGL_drom_data.current_view);

	DDdevice_SetShadeIndex (dde_x11_DGL_drom_data.current_device,
				   dde_x11_DGL_drom_curviwdat->shade_index);

	if (dde_x11_DGL_drom_curdevdat->shade_mode == DcRange) {
	    ddr_x11_DGL_dcm_inq_pseudocolor_weighted_average_weights(&cwtavgpsweight);
	    XdSetShadeRange(dde_x11_DGL_drom_curdevdat->pGC, cwtavgpsweight, 
			    (int)dde_x11_DGL_drom_curdevdat->current_shade_range_min,
			    (int)dde_x11_DGL_drom_curdevdat->current_shade_range_max);
	} else if (dde_x11_DGL_drom_curdevdat->shade_mode == DcComponent) {
	    ddr_x11_DGL_dcm_inq_pseudocolor_bitselect_values(&cbtselpsweight,&cbtselpsmask);
	    XdSetPseudoBitfields(dde_x11_DGL_drom_curdevdat->pGC, cbtselpsweight, cbtselpsmask);
	}

    } else {
	DDerror (ERR_INVALID_VISUAL_TYPE,
		    "ddr_x11_DGL_drom_update_device_and_view",
		    DcNullPtr);

	return;
    }
}
/*
 ======================================================================
 */

      /*  The purpose of environmental initialization is to collect
	  data related to the cameras and lights.  The
	  camera data always includes the camera's local to world
	  transform (which is inverted to obtain the world to camera
	  transform).  */


ddr_x11_DGL_drom_update_studio(traverse_studio)
     DtPFI traverse_studio;
{
    ddr_x11_DGL_drom_initialize_before_environment();

    (*traverse_studio)();

    ddr_x11_DGL_drom_initialize_after_environment();
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_initialize_before_environment()

{
    DtInt i;

    for (i=0; i<dde_x11_DGL_drom_curlgtdat->count; i++) {
	if (dde_x11_DGL_drom_curlgtdat->lights[i].mask & XdLightDist) {
	    DDspace_Deallocate (dde_x11_DGL_drom_curlgtdat->lights[i].distribution);
	    dde_x11_DGL_drom_curlgtdat->lights[i].distribution = (XdReal *) 0;
	}
    }

    dde_x11_DGL_drom_curlgtdat->count = 0;
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_initialize_after_environment()

{
    DtReal hnrm[3];
    XdReal stereo_rotation;
    extern DtReal dor_math_atan();

    if (dde_x11_DGL_drom_curcamdat->stereoswitch &&
	dde_x11_DGL_drom_curdevdat->stereoswitch) {

#ifdef DEBUG
	printf ("computing stereo matrices\n");
#endif
	if (dde_x11_DGL_drom_curcamdat->stereo_distance != 0.0) {
	    stereo_rotation = dor_math_atan(
					    dde_x11_DGL_drom_curcamdat->stereo_eyeseparation / 
					    dde_x11_DGL_drom_curcamdat->stereo_distance);
	} else {
	    stereo_rotation = 0.0;
	}

	/*  Left eye view matrix.  */

	XdMatrixRotateY(dde_x11_DGL_drom_curcamdat->wcstoccsmat,
			-stereo_rotation,XdReplace);
	XdMatrixTranslate(dde_x11_DGL_drom_curcamdat->wcstoccsmat, 
			  (XdReal)-dde_x11_DGL_drom_curcamdat->stereo_eyeseparation, 0., 0.,
			  XdPostConcatenate);
	XdMatrixConcat(dde_x11_DGL_drom_curcamdat->wcstoccsmat,
		       dde_x11_DGL_drom_curcamdat->ccstowcsmat,
		       XdPreConcatenate);
	XdMatrixInvert(dde_x11_DGL_drom_curcamdat->wcstoccsmat);

	ddr_x11_DGL_drom_compute_wcstofcs();

	XdMatrixLoad(dde_x11_DGL_drom_curcamdat->wcstofcsmat_left,
		     dde_x11_DGL_drom_curcamdat->wcstofcsmat);

	XdMatrixLoad(dde_x11_DGL_drom_curcamdat->tspfcstowcsmat_left,
		     dde_x11_DGL_drom_curcamdat->tspfcstowcsmat);

	/*  Right eye view matrix.  */

	XdMatrixRotateY(dde_x11_DGL_drom_curcamdat->wcstoccsmat,
			stereo_rotation,XdReplace);
	XdMatrixTranslate(dde_x11_DGL_drom_curcamdat->wcstoccsmat,
			  (XdReal)dde_x11_DGL_drom_curcamdat->stereo_eyeseparation,0.,0.,
			  XdPostConcatenate);

	XdMatrixConcat(dde_x11_DGL_drom_curcamdat->wcstoccsmat,
		       dde_x11_DGL_drom_curcamdat->ccstowcsmat,
		       XdPreConcatenate);
	XdMatrixInvert(dde_x11_DGL_drom_curcamdat->wcstoccsmat);

	ddr_x11_DGL_drom_compute_wcstofcs();

	XdMatrixLoad(dde_x11_DGL_drom_curcamdat->wcstofcsmat_right,
		     dde_x11_DGL_drom_curcamdat->wcstofcsmat);

	XdMatrixLoad(dde_x11_DGL_drom_curcamdat->tspfcstowcsmat_right,
		     dde_x11_DGL_drom_curcamdat->tspfcstowcsmat);
    } else {
#ifdef DEBUG
	printf ("computing standard matrices\n");
#endif
	XdMatrixLoad(dde_x11_DGL_drom_curcamdat->wcstoccsmat,
		     dde_x11_DGL_drom_curcamdat->ccstowcsmat);
	XdMatrixInvert(dde_x11_DGL_drom_curcamdat->wcstoccsmat);

	ddr_x11_DGL_drom_compute_wcstofcs();
    }
}

/*
 ======================================================================
 */

#ifdef __p3__
int dde_x11_DGL_drom_shade_world = 1;
#endif


ddr_x11_DGL_drom_start_update (device, view, device_data, view_data, window_data)
     DtObject device;
     DtObject view;
     DtPtr device_data;
     DtPtr view_data;
     DtPtr window_data;
{

    dde_x11_DGL_drom_data.current_device = device;
    dde_x11_DGL_drom_data.current_view = view;

    dde_x11_DGL_drom_curdevdat = (struct drom_device_data *)device_data;
    dde_x11_DGL_drom_curviwdat = (struct drom_view_data *)view_data;
    dde_x11_DGL_drom_curwindat = (struct drom_window_data *)window_data;

    dde_x11_DGL_drom_curlgtdat = dde_x11_DGL_drom_curviwdat->light_data;
    dde_x11_DGL_drom_curcamdat = dde_x11_DGL_drom_curviwdat->camera_data;

    ddr_x11_DGL_dcm_set_z_function (ZCOMP_GT);
}

/*
 ======================================================================
 */
ddr_x11_DGL_drom_update_display(traverse_display)
     DtPFI traverse_display;
{
    if (dde_x11_DGL_drom_curwindat->visibility_state ==
	ddd_x11_DGL_drom_window_completely_obscured) {
	/*
	 * This view is completely obscured so do not do update
	 */
	return;
    }

#ifdef DEBUG
    printf ("XdSetViewport(0x%x, %d,%d,%d,%d)\n",
	    dde_x11_DGL_drom_curdevdat->pGC,
	    (int)dde_x11_DGL_drom_curwindat->extent[0],
	    (int)dde_x11_DGL_drom_curwindat->extent[1],
	    (int)dde_x11_DGL_drom_curwindat->extent[2],
	    (int)dde_x11_DGL_drom_curwindat->extent[3]);
#endif
    XdSetViewport(dde_x11_DGL_drom_curdevdat->pGC,
		  (int)dde_x11_DGL_drom_curwindat->extent[0],
		  (int)dde_x11_DGL_drom_curwindat->extent[1],
		  (int)dde_x11_DGL_drom_curwindat->extent[2],
		  (int)dde_x11_DGL_drom_curwindat->extent[3]);
		 
    XdSetViewportMapping(dde_x11_DGL_drom_curdevdat->pGC,
			 dde_x11_DGL_drom_curdevdat->ctodscale,
			 dde_x11_DGL_drom_curdevdat->ctodtrans);

    XdSetLights(dde_x11_DGL_drom_curdevdat->pGC,
		dde_x11_DGL_drom_curlgtdat->count,
		dde_x11_DGL_drom_curlgtdat->lights);

#ifdef __p3__
    if (dde_x11_DGL_drom_shade_world) {
	XdSetShadeMode(dde_x11_DGL_drom_curdevdat->pGC,
		       XdShadeWorld);
    } else {
	XdSetShadeMode(dde_x11_DGL_drom_curdevdat->pGC,
		       XdShadeModelling);
    }      
#endif

    if (dde_x11_DGL_drom_curviwdat->clear_flag) {
	DDdevice_SetBackgroundColor (dde_x11_DGL_drom_data.current_device, DcRGB,
				     dde_x11_DGL_drom_curviwdat->background_color);

	DDdevice_ClearRectangleDepthColor(dde_x11_DGL_drom_data.current_device,
					     (DtInt)dde_x11_DGL_drom_curwindat->extent[0],
					     (DtInt)dde_x11_DGL_drom_curwindat->extent[1],
					     (DtInt)dde_x11_DGL_drom_curwindat->extent[2],
					     (DtInt)dde_x11_DGL_drom_curwindat->extent[3]);
	DDdevice_Flush(dde_x11_DGL_drom_data.current_device);
    }

    /*  If this is a stereo view, then the first pass is for the left eye,
	and we load the wcstofcsmat matrix for this pass from the left eye
	matrix.  */

    if (dde_x11_DGL_drom_curcamdat->stereoswitch &&
	dde_x11_DGL_drom_curdevdat->stereoswitch) {
#ifdef DEBUG
	printf ("loading left eye matrix\n");
#endif      
	XdMatrixLoad(dde_x11_DGL_drom_curcamdat->wcstofcsmat,
		     dde_x11_DGL_drom_curcamdat->wcstofcsmat_left);
	XdMatrixLoad(dde_x11_DGL_drom_curcamdat->tspfcstowcsmat,
		     dde_x11_DGL_drom_curcamdat->tspfcstowcsmat_left);

    }
   
    dde_x11_DGL_drom_data.stereo_leye_flag = DcTrue;

    ddr_x11_DGL_drom_render_pass(traverse_display);

    if (dde_x11_DGL_drom_curdevdat->stereoswitch) {
	/* make sure left eye is complete before start right eye since
	   share the same z buffer */
	DDdevice_Flush(dde_x11_DGL_drom_data.current_device);

	ddr_x11_DGL_dcm_set_left_eye(DcFalse);

	/*  Clear out the depth buffer again, since it is shared by the
	    right eye drawing buffer.  */

	if (dde_x11_DGL_drom_curviwdat->clear_flag) {
	    DDdevice_SetBackgroundColor (dde_x11_DGL_drom_data.current_device, 
					 DcRGB,
					 dde_x11_DGL_drom_curviwdat->background_color);

	    DDdevice_ClearRectangleDepthColor(dde_x11_DGL_drom_data.current_device,
						 (DtInt)dde_x11_DGL_drom_curwindat->extent[0],
						 (DtInt)dde_x11_DGL_drom_curwindat->extent[1],
						 (DtInt)dde_x11_DGL_drom_curwindat->extent[2],
						 (DtInt)dde_x11_DGL_drom_curwindat->extent[3]);
	} else {
	    DDdevice_ClearRectangleDepth(dde_x11_DGL_drom_data.current_device,
					    (DtInt)dde_x11_DGL_drom_curwindat->extent[0],
					    (DtInt)dde_x11_DGL_drom_curwindat->extent[1],
					    (DtInt)dde_x11_DGL_drom_curwindat->extent[2],
					    (DtInt)dde_x11_DGL_drom_curwindat->extent[3]);
	}
	DDdevice_Flush(dde_x11_DGL_drom_data.current_device);

	/*  If this is a stereo view, then the second pass is for the 
	    right eye, and we load the wcstofcsmat matrix for this pass
	    from the right eye matrix.  */

	if (dde_x11_DGL_drom_curcamdat->stereoswitch) {
	    XdMatrixLoad(dde_x11_DGL_drom_curcamdat->wcstofcsmat,
			 dde_x11_DGL_drom_curcamdat->wcstofcsmat_right);
	    XdMatrixLoad(dde_x11_DGL_drom_curcamdat->tspfcstowcsmat,
			 dde_x11_DGL_drom_curcamdat->tspfcstowcsmat_right);
	}

	dde_x11_DGL_drom_data.stereo_leye_flag = DcFalse;

	ddr_x11_DGL_drom_render_pass(traverse_display);

	ddr_x11_DGL_dcm_set_left_eye(DcTrue);
    }
}

/*
 ======================================================================
 */

ddr_x11_DGL_drom_render_pass(traverse_display)
     DtPFI traverse_display;
{
    ddr_x11_DGL_drom_update_state();
    ddr_x11_DGL_drom_update_lcstowcsmat();

    /* Opaque rendering pass; initialise both opaque and transparent
     * ambient and diffuse contributions to their default values. DGL has 
     * no concept of transparency, so transparency is done in a second 
     * pass by setting the pixel write mode to "add" and altering the 
     * ambient and diffuse components as follows:
     * 	    transp_ambient_intens = ambient_intens * (1. - transp_intens)
     * 	    transp_diffuse_intens = diffuse_intens * (1. - transp_intens)
     */

    dde_x11_DGL_drom_data.transparent_surface_pass = DcFalse;
    dde_x11_DGL_drom_data.transparent_surface_flag = DcFalse;
    dde_x11_DGL_drom_data.object_transparent_diffuse_intensity = 0.5;
    dde_x11_DGL_drom_data.object_transparent_ambient_intensity = 0.15;
    XdSetDiffuseIntens(dde_x11_DGL_drom_curdevdat->pGC, 1.0);
    XdSetAmbientIntens(dde_x11_DGL_drom_curdevdat->pGC, 0.3);
	
    ddr_x11_DGL_dcm_set_pixel_function(GXcopy);
    ddr_x11_DGL_dcm_set_depth_buffer_write(DcTrue);
    DDcondex_SetDisablePrimExecFlag (DcFalse);

    (*traverse_display)();

    /*  Transparent rendering pass, if needed.  */

    if (dde_x11_DGL_drom_data.transparent_surface_flag) {
	dde_x11_DGL_drom_data.transparent_surface_pass = DcTrue;

	/* Initialize the data needed for handling transparency during
	 * this pass.
	 */

	DDdevice_Flush(dde_x11_DGL_drom_data.current_device);
	ddr_x11_DGL_dcm_set_pixel_function(GXadd);
	ddr_x11_DGL_dcm_set_depth_buffer_write(DcFalse);

	DDcondex_SetDisablePrimExecFlag (DcFalse);

	(*traverse_display)();

	DDdevice_Flush(dde_x11_DGL_drom_data.current_device);
	ddr_x11_DGL_dcm_set_pixel_function(GXcopy);
	ddr_x11_DGL_dcm_set_depth_buffer_write(DcTrue);

	dde_x11_DGL_drom_data.transparent_surface_flag = DcFalse;
    }
}
