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
	int ddr_x11_titang2_drom_pre_initialization ()
	int ddr_x11_titang2_drom_post_initialization ()
	int ddr_x11_titang2_drom_update_local_data ()
	int ddr_x11_titang2_drom_update_studio (traverse_studio)
	int ddr_x11_titang2_drom_initialize_before_environment ()
	int ddr_x11_titang2_drom_initialize_after_environment ()
	ddr_x11_titang2_drom_start_update ddr_x11_titang2_drom_start_update (device, view, device_data, view_data, window_data)
	int ddr_x11_titang2_drom_update_display (traverse_display)
	int ddr_x11_titang2_drom_render_pass (traverse_display)
	DtFlag ddr_x11_titang2_drom_check_direct_line ()
	int ddr_x11_titang2_drom_initialize_parms ()
	int ddr_x11_titang2_drom_setup_fixed_parms (prigeotyp,bndboxpts,elenrms,vtxnrms,vtxclrs)
	int ddr_x11_titang2_drom_setup_mesh_space (vertexcount,threadcount)
	int ddr_x11_titang2_drom_setup_variable_mesh_parms (bufferno,vertexcount,firstvertex, tclist)
	int ddr_x11_titang2_drom_setup_variable_parms (bufferno,elementcount,vtxlocs,elenrms, vtxnrms,vtxclrs)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */

ddr_x11_titang2_drom_pre_initialization()
{
    struct modclpvol *dor_modclpvol_create();

    dde_x11_titang2_drom_pre_init = DcTrue;

    ddr_x11_titang2_drom_initialize_specular_table();
   
    dde_x11_titang2_drom_data.current_device = NULL;
    dde_x11_titang2_drom_data.current_view = NULL;

    dde_x11_titang2_drom_curdevdat = NULL;
    dde_x11_titang2_drom_curviwdat = NULL;
    dde_x11_titang2_drom_curwindat = NULL;
    dde_x11_titang2_drom_curcamdat = NULL;
    dde_x11_titang2_drom_curlgtdat = NULL;

    /* the top of the stack is dde_x11_titang2_drom_data.object_lcstowcsmat */

    dde_x11_titang2_drom_data.mat_stack.current_element = -1;
    dde_x11_titang2_drom_data.mat_stack.last_element = 9;
    dde_x11_titang2_drom_data.mat_stack.elements = (struct mat_element *)
	    DDspace_Allocate (10*sizeof (struct mat_element));
    dor_matrix_load_identity(dde_x11_titang2_drom_data.object_lcstowcsmat);

    dde_x11_titang2_drom_data.clpvol_stack.current_element = 0;
    dde_x11_titang2_drom_data.clpvol_stack.last_element = 9;
    dde_x11_titang2_drom_data.clpvol_stack.elements = (struct clpvol_element *)
	    DDspace_Allocate (10*sizeof (struct clpvol_element));
    dde_x11_titang2_drom_data.clpvol_stack.elements[0].modclpvol = 
	    dor_modclpvol_create (dde_x11_titang2_drom_data.object_lcstowcsmat,
				  DcClipNone,0,NULL);
    ddr_x11_titang2_drom_update_clpvol(dde_x11_titang2_drom_data.clpvol_stack.elements[0].modclpvol);

    dde_x11_titang2_drom_data.mesh_vertex_count = 0;
    dde_x11_titang2_drom_data.mesh_vertex_space = NULL;
    dde_x11_titang2_drom_data.mesh_perspective_space = NULL;

}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_post_initialization()
{
    /*  World to local normal transformation matrix is the transpose of
	the inverse of the wcstolcsmat.  Since lcstowcsmat is already the
	inverse of wcstolcsmat, we merely need to take the transpose of this
	matrix.  */

    dde_x11_titang2_drom_pre_init = DcFalse;

    dor_matrix_load_identity(dde_x11_titang2_drom_data.nrmwcstolcsmat);
    dor_matrix_transpose(dde_x11_titang2_drom_data.nrmwcstolcsmat);

}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_update_local_data()      
{
    DtVolume *actviewport;
    DtReal actviewxmin, actviewymin, actviewxmax, actviewymax;
    DtInt rno;
    DtInt cnt;
    DtInt cliprectcount;
    DtReal *cliprects;
    DtReal *clearrects;
    DtReal windowxmin, windowymin, windowxmax, windowymax;
    DtReal clearxmin, clearymin, clearxmax, clearymax;
    DtInt shaderange[2];
    DtReal extent[4];
    DtFlag partiallyobscuredflag;
    DtReal scale[3], trans[3];
    DtReal ftodtrans[3];

    actviewport = DDdevice_InqActualViewport (dde_x11_titang2_drom_data.current_device);
			   
    actviewxmin = actviewport->bll[0];
    actviewymin = 1024. - actviewport->fur[1];
    actviewxmax = actviewport->fur[0];
    actviewymax = 1024. - actviewport->bll[1];

#ifdef DEBUG
    printf ("actviewport=(%g %g %g) (%g %g %g)\n",
	    actviewxmin, actviewymin, actviewport->bll[2],
	    actviewxmax, actviewymax, actviewport->fur[2]);
#endif

    dde_x11_titang2_drom_curdevdat->stereoswitch = DDdevice_InqStereo (dde_x11_titang2_drom_data.current_device);
    ddr_x11_titang2_dcm_inquire_stereoscreen(
					       &(dde_x11_titang2_drom_curdevdat->stereoscreen));

    DDdevice_InqClipList (dde_x11_titang2_drom_data.current_device,
			     &cliprectcount,&cliprects,
			     extent, &partiallyobscuredflag);

    ddr_x11_titang2_dcm_inquire_clear_list(&cnt, &clearrects);

    for (rno=0,cnt=0;cnt<cliprectcount;cnt++) {
	windowxmin = (cliprects[cnt*4+0]);
	windowymin = (cliprects[cnt*4+1]);
	windowxmax = (cliprects[cnt*4+2]);
	windowymax = (cliprects[cnt*4+3]);

	clearxmin = (clearrects[cnt*4+0]);
	clearymin = (clearrects[cnt*4+1]);
	clearxmax = (clearrects[cnt*4+2]);
	clearymax = (clearrects[cnt*4+3]);

	if ((windowxmin > actviewxmax) || (windowxmax < actviewxmin) ||
	    (windowymin > actviewymax) || (windowymax < actviewymin)) {
	    continue;

	}	else {
	    windowxmin = max(windowxmin,actviewxmin);
	    windowymin = max(windowymin,actviewymin);
	    windowxmax = min(windowxmax,actviewxmax);
	    windowymax = min(windowymax,actviewymax);

	    clearxmin = max(clearxmin,actviewxmin);
	    clearymin = max(clearymin,actviewymin);
	    clearxmax = min(clearxmax,actviewxmax);
	    clearymax = min(clearymax,actviewymax);
	}

#ifdef DEBUG
	printf ("window: (%g %g) (%g %g)\n",
		windowxmin, windowymin, windowxmax, windowymax);
	printf ("clear: (%g %g) (%g %g)\n",
		clearxmin, clearymin, clearxmax, clearymax);
#endif

	if (dde_x11_titang2_drom_curdevdat->stereoscreen) {
	    dde_x11_titang2_drom_curwindat->obs_leye_fbuf[rno][0] = windowxmin;
	    dde_x11_titang2_drom_curwindat->obs_leye_fbuf[rno][1] = windowymin;
	    dde_x11_titang2_drom_curwindat->obs_leye_fbuf[rno][2] = windowxmax;
	    dde_x11_titang2_drom_curwindat->obs_leye_fbuf[rno][3] = windowymax;

	    dde_x11_titang2_drom_curwindat->obs_leye_bbuf[rno][0] = windowxmin;
	    dde_x11_titang2_drom_curwindat->obs_leye_bbuf[rno][1] = 512. + windowymin;
	    dde_x11_titang2_drom_curwindat->obs_leye_bbuf[rno][2] = windowxmax;
	    dde_x11_titang2_drom_curwindat->obs_leye_bbuf[rno][3] = 512. + windowymax;

	    dde_x11_titang2_drom_curwindat->obs_reye_fbuf[rno][0] = 2048. + windowxmin;
	    dde_x11_titang2_drom_curwindat->obs_reye_fbuf[rno][1] = windowymin;
	    dde_x11_titang2_drom_curwindat->obs_reye_fbuf[rno][2] = 2048. + windowxmax;
	    dde_x11_titang2_drom_curwindat->obs_reye_fbuf[rno][3] = windowymax;

	    dde_x11_titang2_drom_curwindat->obs_reye_bbuf[rno][0] = 2048. + windowxmin;
	    dde_x11_titang2_drom_curwindat->obs_reye_bbuf[rno][1] = 512. + windowymin;
	    dde_x11_titang2_drom_curwindat->obs_reye_bbuf[rno][2] = 2048. + windowxmax;
	    dde_x11_titang2_drom_curwindat->obs_reye_bbuf[rno][3] = 512. + windowymax;

	    /*
	     * Rectangles for clearing -- not aligned to Super pixel
	     */
	    dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][0] = clearxmin;
	    dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][1] = clearymin;
	    dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][2] = clearxmax;
	    dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][3] = clearymax;

	    dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][0] = clearxmin;
	    dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][1] = 512. + clearymin;
	    dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][2] = clearxmax;
	    dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][3] = 512. + clearymax;

	    dde_x11_titang2_drom_curwindat->clr_reye_fbuf[rno][0] = 2048. + clearxmin;
	    dde_x11_titang2_drom_curwindat->clr_reye_fbuf[rno][1] = clearymin;
	    dde_x11_titang2_drom_curwindat->clr_reye_fbuf[rno][2] = 2048. + clearxmax;
	    dde_x11_titang2_drom_curwindat->clr_reye_fbuf[rno][3] = clearymax;

	    dde_x11_titang2_drom_curwindat->clr_reye_bbuf[rno][0] = 2048. + clearxmin;
	    dde_x11_titang2_drom_curwindat->clr_reye_bbuf[rno][1] = 512. + clearymin;
	    dde_x11_titang2_drom_curwindat->clr_reye_bbuf[rno][2] = 2048. + clearxmax;
	    dde_x11_titang2_drom_curwindat->clr_reye_bbuf[rno][3] = 512. + clearymax;
	}	else {
	    dde_x11_titang2_drom_curwindat->obs_leye_fbuf[rno][0] = windowxmin;
	    dde_x11_titang2_drom_curwindat->obs_leye_fbuf[rno][1] = windowymin;
	    dde_x11_titang2_drom_curwindat->obs_leye_fbuf[rno][2] = windowxmax;
	    dde_x11_titang2_drom_curwindat->obs_leye_fbuf[rno][3] = windowymax;

	    dde_x11_titang2_drom_curwindat->obs_leye_bbuf[rno][0] = 2048. + windowxmin;
	    dde_x11_titang2_drom_curwindat->obs_leye_bbuf[rno][1] = windowymin;
	    dde_x11_titang2_drom_curwindat->obs_leye_bbuf[rno][2] = 2048. + windowxmax;
	    dde_x11_titang2_drom_curwindat->obs_leye_bbuf[rno][3] = windowymax;

	    dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][0] = clearxmin;
	    dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][1] = clearymin;
	    dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][2] = clearxmax;
	    dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][3] = clearymax;

	    dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][0] = 2048. + clearxmin;
	    dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][1] = clearymin;
	    dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][2] = 2048. + clearxmax;
	    dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][3] = clearymax;
	}
	rno++;
    }

    dde_x11_titang2_drom_curwindat->clipping_rectangle_count = rno;

    if (dde_x11_titang2_drom_curwindat->clipping_rectangle_count == 0) {
	dde_x11_titang2_drom_curwindat->visibility_state =
		ddd_x11_titang2_drom_window_completely_obscured;

    } else if (!partiallyobscuredflag) {
	dde_x11_titang2_drom_curwindat->visibility_state =
		ddd_x11_titang2_drom_window_completely_visible;

	/*
	 * to fix a problem with the g2 hardware drawing lines that are
	 * too long by a pixel (ie drawing outside the window) the clip rectangle
	 * is adjusted to prevent this.
	 */

	dde_x11_titang2_drom_curwindat->obs_leye_fbuf[0][0] += 1.;
	dde_x11_titang2_drom_curwindat->obs_leye_fbuf[0][1] += 1.;
	dde_x11_titang2_drom_curwindat->obs_leye_fbuf[0][2] -= 1.;
	dde_x11_titang2_drom_curwindat->obs_leye_fbuf[0][3] -= 1.;

	dde_x11_titang2_drom_curwindat->obs_leye_bbuf[0][0] += 1.;
	dde_x11_titang2_drom_curwindat->obs_leye_bbuf[0][1] += 1.;
	dde_x11_titang2_drom_curwindat->obs_leye_bbuf[0][2] -= 1.;
	dde_x11_titang2_drom_curwindat->obs_leye_bbuf[0][3] -= 1.;

	dde_x11_titang2_drom_curwindat->obs_reye_fbuf[0][0] += 1.;
	dde_x11_titang2_drom_curwindat->obs_reye_fbuf[0][1] += 1.;
	dde_x11_titang2_drom_curwindat->obs_reye_fbuf[0][2] -= 1.;
	dde_x11_titang2_drom_curwindat->obs_reye_fbuf[0][3] -= 1.;

	dde_x11_titang2_drom_curwindat->obs_reye_bbuf[0][0] += 1.;
	dde_x11_titang2_drom_curwindat->obs_reye_bbuf[0][1] += 1.;
	dde_x11_titang2_drom_curwindat->obs_reye_bbuf[0][2] -= 1.;
	dde_x11_titang2_drom_curwindat->obs_reye_bbuf[0][3] -= 1.;

    } else {
	dde_x11_titang2_drom_curwindat->visibility_state =
		ddd_x11_titang2_drom_window_partially_obscured;
    }

    DdInqShadeRanges (dde_x11_titang2_drom_data.current_device,
				dde_x11_titang2_drom_data.object_shade_index,1,shaderange);
    dde_x11_titang2_drom_curdevdat->current_shade_range_min = (DtReal)shaderange[0];
    dde_x11_titang2_drom_curdevdat->current_shade_range_max = (DtReal)shaderange[1];
   
    DDdevice_InqFrustumScaleTrans (dde_x11_titang2_drom_data.current_device,
				scale,trans);
    trans[0] -= 1.;
    trans[1] -= 1.;

    /*
     * reduce the scale factors by 1 to try and keep the g2 hardware 
     * from drawing outside the window (ie pixel krud on the top line)
     */
    scale[0] -= 1.;
    scale[1] -= 1.;

    extent[0] += 1.;
    extent[1] += 1.;
    extent[2] -= 1.;
    extent[3] -= 1.;


    DDdevice_InqFrameScaleTrans (dde_x11_titang2_drom_data.current_device,
				 dde_x11_titang2_drom_curdevdat->ftodscale,
				 ftodtrans);

    dde_x11_titang2_drom_curdevdat->annoscale[0] = 
	    dde_x11_titang2_drom_curdevdat->ftodscale[0] / scale[0];
    dde_x11_titang2_drom_curdevdat->annoscale[1] = 
	    dde_x11_titang2_drom_curdevdat->ftodscale[1] / scale[1];
    dde_x11_titang2_drom_curdevdat->annoscale[2] = 
	    dde_x11_titang2_drom_curdevdat->ftodscale[2] /
		    scale[2];

    if (dde_x11_titang2_drom_curdevdat->stereoscreen) {
	/*  Stereo front and back buffer camera to device scale and
	    translate assignments.  */

	dde_x11_titang2_drom_curdevdat->ctodscale_leye_fbuf[0] = scale[0];
	dde_x11_titang2_drom_curdevdat->ctodscale_leye_fbuf[1] = -1. * scale[1];
	dde_x11_titang2_drom_curdevdat->ctodscale_leye_fbuf[2] = scale[2];

	dde_x11_titang2_drom_curdevdat->ctodtrans_leye_fbuf[0] = trans[0];
	dde_x11_titang2_drom_curdevdat->ctodtrans_leye_fbuf[1] = 1023. - trans[1];
	dde_x11_titang2_drom_curdevdat->ctodtrans_leye_fbuf[2] = trans[2];

	dde_x11_titang2_drom_curdevdat->ctodscale_leye_bbuf[0] = scale[0];
	dde_x11_titang2_drom_curdevdat->ctodscale_leye_bbuf[1] = -1. * scale[1];
	dde_x11_titang2_drom_curdevdat->ctodscale_leye_bbuf[2] = scale[2];

	dde_x11_titang2_drom_curdevdat->ctodtrans_leye_bbuf[0] = trans[0];
	dde_x11_titang2_drom_curdevdat->ctodtrans_leye_bbuf[1] = 1535. - trans[1];
	dde_x11_titang2_drom_curdevdat->ctodtrans_leye_bbuf[2] = trans[2];

	dde_x11_titang2_drom_curdevdat->ctodscale_reye_fbuf[0] = scale[0];
	dde_x11_titang2_drom_curdevdat->ctodscale_reye_fbuf[1] = -1. * scale[1];
	dde_x11_titang2_drom_curdevdat->ctodscale_reye_fbuf[2] = scale[2];

	dde_x11_titang2_drom_curdevdat->ctodtrans_reye_fbuf[0] = 2048. + trans[0];
	dde_x11_titang2_drom_curdevdat->ctodtrans_reye_fbuf[1] = 1023. - trans[1];
	dde_x11_titang2_drom_curdevdat->ctodtrans_reye_fbuf[2] = trans[2];

	dde_x11_titang2_drom_curdevdat->ctodscale_reye_bbuf[0] = scale[0];
	dde_x11_titang2_drom_curdevdat->ctodscale_reye_bbuf[1] = -1. * scale[1];
	dde_x11_titang2_drom_curdevdat->ctodscale_reye_bbuf[2] = scale[2];

	dde_x11_titang2_drom_curdevdat->ctodtrans_reye_bbuf[0] = 2048. + trans[0];
	dde_x11_titang2_drom_curdevdat->ctodtrans_reye_bbuf[1] = 1535. - trans[1];
	dde_x11_titang2_drom_curdevdat->ctodtrans_reye_bbuf[2] = trans[2];
    } else {
	/*  Non-stereo front and back buffer camera to device scale and
	    translate assignments.  These will flip the y-axis upside down
	    around 1023 by modifying the y scale factor and the y translate
	    factor for both buffers, and will cause back buffer drawing by
	    modifying the x translate factor for the back buffer.  */

	dde_x11_titang2_drom_curdevdat->ctodscale_leye_fbuf[0] = scale[0];
	dde_x11_titang2_drom_curdevdat->ctodscale_leye_fbuf[1] = -1. * scale[1];
	dde_x11_titang2_drom_curdevdat->ctodscale_leye_fbuf[2] = scale[2];

	dde_x11_titang2_drom_curdevdat->ctodtrans_leye_fbuf[0] = trans[0];
	dde_x11_titang2_drom_curdevdat->ctodtrans_leye_fbuf[1] = 1023. - trans[1];
	dde_x11_titang2_drom_curdevdat->ctodtrans_leye_fbuf[2] = trans[2];

	dde_x11_titang2_drom_curdevdat->ctodscale_leye_bbuf[0] = scale[0];
	dde_x11_titang2_drom_curdevdat->ctodscale_leye_bbuf[1] = -1. * scale[1];
	dde_x11_titang2_drom_curdevdat->ctodscale_leye_bbuf[2] = scale[2];

	dde_x11_titang2_drom_curdevdat->ctodtrans_leye_bbuf[0] = 2048. + trans[0];
	dde_x11_titang2_drom_curdevdat->ctodtrans_leye_bbuf[1] = 1023. - trans[1];
	dde_x11_titang2_drom_curdevdat->ctodtrans_leye_bbuf[2] = trans[2];
    }

    /*  Set up the front and back buffer extents from the original extent
	data.  */

    if (dde_x11_titang2_drom_curdevdat->stereoscreen) {
	dde_x11_titang2_drom_curdevdat->extent_leye_fbuf[0] = extent[0];
	dde_x11_titang2_drom_curdevdat->extent_leye_fbuf[1] = extent[1];
	dde_x11_titang2_drom_curdevdat->extent_leye_fbuf[2] = extent[2];
	dde_x11_titang2_drom_curdevdat->extent_leye_fbuf[3] = extent[3];

	dde_x11_titang2_drom_curdevdat->extent_leye_bbuf[0] = extent[0];
	dde_x11_titang2_drom_curdevdat->extent_leye_bbuf[1] = 512. + extent[1];
	dde_x11_titang2_drom_curdevdat->extent_leye_bbuf[2] = extent[2];
	dde_x11_titang2_drom_curdevdat->extent_leye_bbuf[3] = 512. + extent[3];

	dde_x11_titang2_drom_curdevdat->extent_reye_fbuf[0] = 2048. + extent[0];
	dde_x11_titang2_drom_curdevdat->extent_reye_fbuf[1] = extent[1];
	dde_x11_titang2_drom_curdevdat->extent_reye_fbuf[2] = 2048. + extent[2];
	dde_x11_titang2_drom_curdevdat->extent_reye_fbuf[3] = extent[3];

	dde_x11_titang2_drom_curdevdat->extent_reye_bbuf[0] = 2048. + extent[0];
	dde_x11_titang2_drom_curdevdat->extent_reye_bbuf[1] = 512. + extent[1];
	dde_x11_titang2_drom_curdevdat->extent_reye_bbuf[2] = 2048. + extent[2];
	dde_x11_titang2_drom_curdevdat->extent_reye_bbuf[3] = 512. + extent[3];
    } else {
	dde_x11_titang2_drom_curdevdat->extent_leye_fbuf[0] = extent[0];
	dde_x11_titang2_drom_curdevdat->extent_leye_fbuf[1] = extent[1];
	dde_x11_titang2_drom_curdevdat->extent_leye_fbuf[2] = extent[2];
	dde_x11_titang2_drom_curdevdat->extent_leye_fbuf[3] = extent[3];

	dde_x11_titang2_drom_curdevdat->extent_leye_bbuf[0] = 2048. + extent[0];
	dde_x11_titang2_drom_curdevdat->extent_leye_bbuf[1] = extent[1];
	dde_x11_titang2_drom_curdevdat->extent_leye_bbuf[2] = 2048. + extent[2];
	dde_x11_titang2_drom_curdevdat->extent_leye_bbuf[3] = extent[3];
    }

    ddr_x11_titang2_dcm_inquire_expansion_board(&(dde_x11_titang2_drom_curdevdat->expansion_board));

    dde_x11_titang2_drom_curviwdat->clear_flag = 
	    DvInqClearFlag(dde_x11_titang2_drom_data.current_view);

    DvInqBackgroundColor (dde_x11_titang2_drom_data.current_view,
				  &dde_x11_titang2_drom_curviwdat->background_color_model,
				  dde_x11_titang2_drom_curviwdat->background_color);

    dde_x11_titang2_drom_curdevdat->shade_mode =
	    DdInqShadeMode (dde_x11_titang2_drom_data.current_device);

    dde_x11_titang2_drom_curdevdat->visual_type =
	    DdInqVisualType (dde_x11_titang2_drom_data.current_device);

    if (dde_x11_titang2_drom_curdevdat->visual_type == DcDirectColor) {
	dde_x11_titang2_drom_data.pseudocolor_switch = DcFalse;

    } else if (dde_x11_titang2_drom_curdevdat->visual_type == DcPseudoColor) {
	dde_x11_titang2_drom_data.pseudocolor_switch = DcTrue;

	dde_x11_titang2_drom_curviwdat->shade_index = DvInqShadeIndex(dde_x11_titang2_drom_data.current_view);
    } else {
	DDerror (ERR_INVALID_VISUAL_TYPE,
		    "ddr_x11_titang2_drom_update_device_and_view",
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


ddr_x11_titang2_drom_update_studio(traverse_studio)
DtPFI traverse_studio;
{
    ddr_x11_titang2_drom_initialize_before_environment();

    (*traverse_studio)();

    ddr_x11_titang2_drom_initialize_after_environment();
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_initialize_before_environment()
{
    dde_x11_titang2_drom_curlgtdat->count = 0;

    dde_x11_titang2_drom_curlgtdat->ambient_reflectance[0] = 0.;
    dde_x11_titang2_drom_curlgtdat->ambient_reflectance[1] = 0.;
    dde_x11_titang2_drom_curlgtdat->ambient_reflectance[2] = 0.;
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_initialize_after_environment()
{
    DtInt lightno;
    DtReal hnrm[3];
    DtReal stereo_rotation;
    extern DtReal dor_math_atan();

    if (dde_x11_titang2_drom_curcamdat->stereoswitch &&
	dde_x11_titang2_drom_curdevdat->stereoswitch) {

#ifdef DEBUG
	printf ("computing stereo matrices\n");
#endif
	if (dde_x11_titang2_drom_curcamdat->stereo_distance != 0.0) {
	    stereo_rotation = dor_math_atan(
					    dde_x11_titang2_drom_curcamdat->stereo_eyeseparation / 
					    dde_x11_titang2_drom_curcamdat->stereo_distance);
	} else {
	    stereo_rotation = 0.0;
	}

	/*  Left eye view matrix.  */

	dor_matrix_load_rotate_y(dde_x11_titang2_drom_curcamdat->wcstoccsmat,
				 -stereo_rotation);
	dor_matrix_translate(dde_x11_titang2_drom_curcamdat->wcstoccsmat,
			     -dde_x11_titang2_drom_curcamdat->stereo_eyeseparation,0.,0.,
			     DcPostConcatenate);

	dor_matrix_concat (dde_x11_titang2_drom_curcamdat->wcstoccsmat,
			   dde_x11_titang2_drom_curcamdat->ccstowcsmat,
			   DcPreConcatenate);
	dor_matrix_invert(dde_x11_titang2_drom_curcamdat->wcstoccsmat);

	ddr_x11_titang2_drom_compute_wcstofcs();

	dor_matrix_load (dde_x11_titang2_drom_curcamdat->wcstofcsmat_left,
			 dde_x11_titang2_drom_curcamdat->wcstofcsmat);

	dor_matrix_load (dde_x11_titang2_drom_curcamdat->tspfcstowcsmat_left,
			 dde_x11_titang2_drom_curcamdat->wcstofcsmat_left);
	dor_matrix_invert(dde_x11_titang2_drom_curcamdat->tspfcstowcsmat_left);
	dor_matrix_transpose(dde_x11_titang2_drom_curcamdat->tspfcstowcsmat_left);

	/*  Right eye view matrix.  */

	dor_matrix_load_rotate_y(dde_x11_titang2_drom_curcamdat->wcstoccsmat,
				 stereo_rotation);
	dor_matrix_translate(dde_x11_titang2_drom_curcamdat->wcstoccsmat,
			     dde_x11_titang2_drom_curcamdat->stereo_eyeseparation,0.,0.,
			     DcPostConcatenate);

	dor_matrix_concat (dde_x11_titang2_drom_curcamdat->wcstoccsmat,
			   dde_x11_titang2_drom_curcamdat->ccstowcsmat,
			   DcPreConcatenate);
	dor_matrix_invert(dde_x11_titang2_drom_curcamdat->wcstoccsmat);

	ddr_x11_titang2_drom_compute_wcstofcs();

	dor_matrix_load (dde_x11_titang2_drom_curcamdat->wcstofcsmat_right,
			 dde_x11_titang2_drom_curcamdat->wcstofcsmat);

	dor_matrix_load (dde_x11_titang2_drom_curcamdat->tspfcstowcsmat_right,
			 dde_x11_titang2_drom_curcamdat->wcstofcsmat_right);
	dor_matrix_invert(dde_x11_titang2_drom_curcamdat->tspfcstowcsmat_right);
	dor_matrix_transpose(dde_x11_titang2_drom_curcamdat->tspfcstowcsmat_right);
   
    } else {
#ifdef DEBUG
	printf ("computing standard matrices\n");
#endif
	dor_matrix_load(dde_x11_titang2_drom_curcamdat->wcstoccsmat,
			dde_x11_titang2_drom_curcamdat->ccstowcsmat);
	dor_matrix_invert(dde_x11_titang2_drom_curcamdat->wcstoccsmat);

	ddr_x11_titang2_drom_compute_wcstofcs();
    }

    for (lightno=0;lightno<dde_x11_titang2_drom_curlgtdat->count;lightno++) {
	ddr_x11_titang2_drom_h_normal(dde_x11_titang2_drom_curcamdat->wcscamnorm,
				       dde_x11_titang2_drom_curlgtdat->cosine[lightno],hnrm);

	dde_x11_titang2_drom_curlgtdat->h[lightno][0] = hnrm[0];
	dde_x11_titang2_drom_curlgtdat->h[lightno][1] = hnrm[1];
	dde_x11_titang2_drom_curlgtdat->h[lightno][2] = hnrm[2];
    }

    /*  World to local normal transformation matrix is the transpose of
	the inverse of the wcstolcsmat.  Since lcstowcsmat is already the
	inverse of wcstolcsmat, we merely need to take the transpose of this
	matrix.  */

    dor_matrix_load_identity(dde_x11_titang2_drom_data.nrmwcstolcsmat);
    dor_matrix_transpose(dde_x11_titang2_drom_data.nrmwcstolcsmat);

    ddr_x11_titang2_drom_update_lights();
}

/*
 ======================================================================
 */

ddr_x11_titang2_drom_start_update (device, view, device_data, view_data, window_data)
DtObject device;
DtObject view;
DtPtr device_data;
DtPtr view_data;
DtPtr window_data;
{

    dde_x11_titang2_drom_data.current_device = device;
    dde_x11_titang2_drom_data.current_view = view;

    dde_x11_titang2_drom_curdevdat = (struct drom_device_data *)device_data;
    dde_x11_titang2_drom_curviwdat = (struct drom_view_data *)view_data;
    dde_x11_titang2_drom_curwindat = (struct drom_window_data *)window_data;

    dde_x11_titang2_drom_curlgtdat = dde_x11_titang2_drom_curviwdat->light_data;
    dde_x11_titang2_drom_curcamdat = dde_x11_titang2_drom_curviwdat->camera_data;

    ddr_x11_titang2_dcm_set_z_function (1);

    ddr_x11_titang2_dcm_inquire_draw_buffer (&dde_x11_titang2_drom_draw_buffer);
}


ddr_x11_titang2_drom_update_display(traverse_display)
DtPFI traverse_display;
{
    DtInt rno;

    if (dde_x11_titang2_drom_curwindat->visibility_state ==
	ddd_x11_titang2_drom_window_completely_obscured) {
	/*
	 * This view is completely obscured so do not do update
	 */
	return;
    }

    if (dde_x11_titang2_drom_curviwdat->clear_flag) {
	DDdevice_SetBackgroundColor (dde_x11_titang2_drom_data.current_device,
				     DcRGB,
				     dde_x11_titang2_drom_curviwdat->background_color);

	for (rno=0;rno<dde_x11_titang2_drom_curwindat->clipping_rectangle_count;rno++){
	    if (dde_x11_titang2_drom_draw_buffer) {
#ifdef DEBUG
		printf ("buf(BL): rect %d: clr depth & color %d %d %d %d\n",
			rno,
			(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][0],
			(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][1],
			(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][2],
			(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][3]);
#endif
		DDdevice_ClearRectangleDepthColor(dde_x11_titang2_drom_data.current_device,
						     (DtInt)dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][0],
						     (DtInt)dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][1],
						     (DtInt)dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][2],
						     (DtInt)dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][3]);

		if (dde_x11_titang2_drom_curdevdat->stereoswitch) {
#ifdef DEBUG
		    printf ("buf(BR): rect %d: clr depth & color %d %d %d %d\n",
			    rno,
			    (DtInt)dde_x11_titang2_drom_curwindat->clr_reye_bbuf[rno][0],
			    (DtInt)dde_x11_titang2_drom_curwindat->clr_reye_bbuf[rno][1],
			    (DtInt)dde_x11_titang2_drom_curwindat->clr_reye_bbuf[rno][2],
			    (DtInt)dde_x11_titang2_drom_curwindat->clr_reye_bbuf[rno][3]);
#endif
		    DDdevice_ClearRectangleDepthColor(dde_x11_titang2_drom_data.current_device,
							 (DtInt)dde_x11_titang2_drom_curwindat->clr_reye_bbuf[rno][0],
							 (DtInt)dde_x11_titang2_drom_curwindat->clr_reye_bbuf[rno][1],
							 (DtInt)dde_x11_titang2_drom_curwindat->clr_reye_bbuf[rno][2],
							 (DtInt)dde_x11_titang2_drom_curwindat->clr_reye_bbuf[rno][3]);
		}
	    } else {
#ifdef DEBUG
		printf ("buf(FL): rect %d: clr depth & color %d %d %d %d\n",
			rno,
			(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][0],
			(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][1],
			(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][2],
			(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][3]);
#endif
		DDdevice_ClearRectangleDepthColor(dde_x11_titang2_drom_data.current_device,
						     (DtInt)dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][0],
						     (DtInt)dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][1],
						     (DtInt)dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][2],
						     (DtInt)dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][3]);

		if (dde_x11_titang2_drom_curdevdat->stereoswitch) {
#ifdef DEBUG
		    printf ("buf(FR): rect %d: clr depth & color %d %d %d %d\n",
			    rno,
			    (DtInt)dde_x11_titang2_drom_curwindat->clr_reye_fbuf[rno][0],
			    (DtInt)dde_x11_titang2_drom_curwindat->clr_reye_fbuf[rno][1],
			    (DtInt)dde_x11_titang2_drom_curwindat->clr_reye_fbuf[rno][2],
			    (DtInt)dde_x11_titang2_drom_curwindat->clr_reye_fbuf[rno][3]);
#endif
		    DDdevice_ClearRectangleDepthColor(dde_x11_titang2_drom_data.current_device,
							 (DtInt)dde_x11_titang2_drom_curwindat->clr_reye_fbuf[rno][0],
							 (DtInt)dde_x11_titang2_drom_curwindat->clr_reye_fbuf[rno][1],
							 (DtInt)dde_x11_titang2_drom_curwindat->clr_reye_fbuf[rno][2],
							 (DtInt)dde_x11_titang2_drom_curwindat->clr_reye_fbuf[rno][3]);
		}
	    }
	}

	DDdevice_Flush(dde_x11_titang2_drom_data.current_device);
    }

    /*  If this is a stereo view, then the first pass is for the left eye,
	and we load the wcstofcsmat matrix for this pass from the left eye
	matrix.  */

    if (dde_x11_titang2_drom_curcamdat->stereoswitch &&
	dde_x11_titang2_drom_curdevdat->stereoswitch) {
#ifdef DEBUG
	printf ("loading left eye matrix\n");
#endif      
	dor_matrix_load (dde_x11_titang2_drom_curcamdat->wcstofcsmat,
			 dde_x11_titang2_drom_curcamdat->wcstofcsmat_left);
	dor_matrix_load (dde_x11_titang2_drom_curcamdat->tspfcstowcsmat,
			 dde_x11_titang2_drom_curcamdat->tspfcstowcsmat_left);

    }
    dde_x11_titang2_drom_data.stereo_leye_flag = DcTrue;

    ddr_x11_titang2_drom_render_pass(traverse_display);

    if (dde_x11_titang2_drom_curdevdat->stereoswitch) {
	/*  Clear out the depth buffer again, since it is shared by the
	    right eye drawing buffer.  */

	/* make sure left eye is complete before start right eye since
	   share the same z buffer */
	DDdevice_Flush(dde_x11_titang2_drom_data.current_device);

#ifdef DEBUG
	printf ("stereo -- right eye\n");
#endif
	for (rno=0;
	     rno<dde_x11_titang2_drom_curwindat->clipping_rectangle_count;
	     rno++) {
	    if (dde_x11_titang2_drom_draw_buffer) {
#ifdef DEBUG
		printf ("buf(BL): rect %d: clr depth %d %d %d %d\n",
			rno,
			(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][0],
			(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][1],
			(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][2],
			(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][3]);
#endif
		DDdevice_ClearRectangleDepth(dde_x11_titang2_drom_data.current_device,
						(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][0],
						(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][1],
						(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][2],
						(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_bbuf[rno][3]);
	    } else {
#ifdef DEBUG
		printf ("buf(FL): rect %d: clr depth %d %d %d %d\n",
			rno,
			(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][0],
			(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][1],
			(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][2],
			(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][3]);
#endif
		DDdevice_ClearRectangleDepth(dde_x11_titang2_drom_data.current_device,
						(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][0],
						(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][1],
						(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][2],
						(DtInt)dde_x11_titang2_drom_curwindat->clr_leye_fbuf[rno][3]);
	    }
	}

	DDdevice_Flush(dde_x11_titang2_drom_data.current_device);

	/*  If this is a stereo view, then the second pass is for the 
	    right eye, and we load the wcstofcsmat matrix for this pass
	    from the right eye matrix.  */

	if (dde_x11_titang2_drom_curcamdat->stereoswitch) {
#ifdef DEBUG
	    printf ("loading right eye matrix\n");
#endif
	    dor_matrix_load
		(dde_x11_titang2_drom_curcamdat->wcstofcsmat,
		 dde_x11_titang2_drom_curcamdat->wcstofcsmat_right);
	    dor_matrix_load
		(dde_x11_titang2_drom_curcamdat->tspfcstowcsmat,
		 dde_x11_titang2_drom_curcamdat->tspfcstowcsmat_right);
	}

	dde_x11_titang2_drom_data.stereo_leye_flag = DcFalse;

	ddr_x11_titang2_drom_render_pass(traverse_display);
    }
}

/*
 ======================================================================
 */

ddr_x11_titang2_drom_render_pass(traverse_display)
DtPFI traverse_display;
{
    ddr_x11_titang2_drom_update_state();

    /*  Opaque rendering pass.  */

    dde_x11_titang2_drom_data.transparent_surface_pass = DcFalse;

    /*  Initialize the data needed for handling transparency during
	this pass.  */

    ddr_x11_titang2_dcm_set_pixel_function(0);

    ddr_x11_titang2_dcm_set_depth_buffer_write(DcTrue);

    DDcondex_SetDisablePrimExecFlag (DcFalse);

    (*traverse_display)();

    /*  Transparent rendering pass, if needed.  */

    if (dde_x11_titang2_drom_data.transparent_surface_flag) {
	dde_x11_titang2_drom_data.transparent_surface_pass = DcTrue;

	/*  Initialize the data needed for handling transparency during
	    this pass.  */

	DDdevice_Flush(dde_x11_titang2_drom_data.current_device);

	ddr_x11_titang2_dcm_set_pixel_function(3);

	ddr_x11_titang2_dcm_set_pixel_clip(1);

	ddr_x11_titang2_dcm_set_depth_buffer_write(DcFalse);

	if (dde_x11_titang2_drom_data.object_transparent_switch) {
	    DDcondex_SetDisablePrimExecFlag (DcFalse);
	}	else {
	    DDcondex_SetDisablePrimExecFlag (DcTrue);
	}

	(*traverse_display)();

	ddr_x11_titang2_dcm_set_pixel_function(0);
	ddr_x11_titang2_dcm_set_depth_buffer_write(DcTrue);

	dde_x11_titang2_drom_data.transparent_surface_flag = DcFalse;
    }
}
/*
 ======================================================================
 */

DtFlag ddr_x11_titang2_drom_check_direct_line()
{

    if (dde_x11_titang2_drom_data.object_hidsurf_switch){
	return(DcFalse);
    }

    if (dde_x11_titang2_drom_cg_sparms.cg_ctl & (CG_mNORMS | CG_mVCOLS | CG_mDPCUE)){
	return(DcFalse);
    }

    return(DcTrue);
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_initialize_parms()
{
    struct cg_sparms *sparms;
    static DtReal dprange[3];
    static DtReal dpfrac[2];
    static DtReal cwtavgpsweight[] = { .33, .33, .33 };
    static DtReal cwtavgpslim[3];
    static DtReal cbtselpsweight[] = { 256./256., 32./256., 4./256. };
    static DtInt cbtselpsmask[] = { 0, 0xe0, 0, 0x1c, 0, 0x03 };
    DtInt halfspaceno;
    DtReal tempplneqn[4];
    static DtColorRGB ambwhite;
    static DtColorRGB ambblack = { 0., 0., 0. };

    sparms = &dde_x11_titang2_drom_cg_sparms;

    sparms->cg_ctl = CG_NIL;

    if (dde_x11_titang2_drom_data.object_cull_switch &&
	dde_x11_titang2_drom_data.object_cullable_switch){
	sparms->cg_ctl |= CG_BFCULL;
    }
    else{
	sparms->cg_ctl |= CG_NOCULL;
    }

    if (!dde_x11_titang2_drom_data.object_specular_switch){
	sparms->cg_ctl |= CG_NOSPEC;
    }
    else{
	sparms->cg_ctl |= CG_SOBJ;
    }

    if (dde_x11_titang2_drom_data.object_depthcue_switch){
	if ((dde_x11_titang2_drom_data.object_depthcue_zfront == 1.*65535.) &&
	    (dde_x11_titang2_drom_data.object_depthcue_zback == 0.) &&
	    (dde_x11_titang2_drom_data.object_depthcue_sfront == 1.) &&
	    (dde_x11_titang2_drom_data.object_depthcue_sback == 0.)){
	    if ((dde_x11_titang2_drom_data.object_depthcue_color[0] == 0.) &&
		(dde_x11_titang2_drom_data.object_depthcue_color[1] == 0.) &&
		(dde_x11_titang2_drom_data.object_depthcue_color[2] == 0.)){
		dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_DPCUEBB;
	    }
	    else{
		dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_DPCUECB;
	    }
	}
	else{
	    dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_DPCUEFULL;
	}
    }
    else{
	dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_NODPCUE;
    }

    if (dde_x11_titang2_drom_data.object_interpolation_type == DcConstantShade){
	sparms->cg_ctl |= CG_SHDAVG;
    }
    else{
	sparms->cg_ctl |= CG_NOSHDAVG;
    }

    if (dde_x11_titang2_drom_data.pseudocolor_switch){
	if (dde_x11_titang2_drom_curdevdat->shade_mode == DcComponent){
	    sparms->cg_ctl |= CG_PSCBTSEL;
	}
	else if (dde_x11_titang2_drom_curdevdat->shade_mode == DcRange){
	    sparms->cg_ctl |= CG_PSCWTAVG;
	}
    }
    else{
	sparms->cg_ctl |= CG_NOPSCOLOR;
    }

    if (dde_x11_titang2_drom_data.pseudocolor_switch){
	sparms->cg_ctl |= CG_PSEUDO;
    }
    else{
	if (dde_x11_titang2_drom_curdevdat->expansion_board){
	    sparms->cg_ctl |= CG_FULL;
	}
	else{
	    sparms->cg_ctl |= CG_BASE;
	}
    }

    sparms->cg_matrix = (double *)dde_x11_titang2_drom_curcamdat->lcstofcsmat;

    if (dde_x11_titang2_drom_curdevdat->stereoswitch){
	if (dde_x11_titang2_drom_data.stereo_leye_flag){
	    if (dde_x11_titang2_drom_draw_buffer){
		sparms->cg_scale = dde_x11_titang2_drom_curdevdat->ctodscale_leye_bbuf;
		sparms->cg_xlate = dde_x11_titang2_drom_curdevdat->ctodtrans_leye_bbuf;
	    }
	    else{
		sparms->cg_scale = dde_x11_titang2_drom_curdevdat->ctodscale_leye_fbuf;
		sparms->cg_xlate = dde_x11_titang2_drom_curdevdat->ctodtrans_leye_fbuf;
	    }
	}
	else{
	    if (dde_x11_titang2_drom_draw_buffer){
		sparms->cg_scale = dde_x11_titang2_drom_curdevdat->ctodscale_reye_bbuf;
		sparms->cg_xlate = dde_x11_titang2_drom_curdevdat->ctodtrans_reye_bbuf;
	    }
	    else{
		sparms->cg_scale = dde_x11_titang2_drom_curdevdat->ctodscale_reye_fbuf;
		sparms->cg_xlate = dde_x11_titang2_drom_curdevdat->ctodtrans_reye_fbuf;
	    }
	}
    }
    else{
	if (dde_x11_titang2_drom_draw_buffer){
	    sparms->cg_scale = dde_x11_titang2_drom_curdevdat->ctodscale_leye_bbuf;
	    sparms->cg_xlate = dde_x11_titang2_drom_curdevdat->ctodtrans_leye_bbuf;
	}
	else{
	    sparms->cg_scale = dde_x11_titang2_drom_curdevdat->ctodscale_leye_fbuf;
	    sparms->cg_xlate = dde_x11_titang2_drom_curdevdat->ctodtrans_leye_fbuf;
	}
    }

#ifdef notdef
    if (dde_x11_titang2_drom_curdevdat->stereoswitch){
	if (dde_x11_titang2_drom_data.stereo_leye_flag){
	    if (dde_x11_titang2_drom_draw_buffer){
		printf("stereo left eye, back buffer\n");
	    }
	    else{
		printf("stereo left eye, front buffer\n");
	    }
	}
	else{
	    if (dde_x11_titang2_drom_draw_buffer){
		printf("stereo right eye, back buffer\n");
	    }
	    else{
		printf("stereo right eye, front buffer\n");
	    }
	}
    }
#endif

    sparms->cg_ctl |= CG_XFORM;

    /*  If the window is completely visible, then the first clipping
	rectangle defines the clipping window for the frustum in
	device space, and the obscuring rectangle list contains a
	single rectangle equal to the device extent.
	
	If the window is partially obscured, then the clipping rectangles
	define the obscuring rectangle list, and the frustum clipping window
	is the device extent.
	
	If the window is completely obscured, then we do not need to
	do anything here, since no rendering will be performed.  */

    if (dde_x11_titang2_drom_curwindat->visibility_state ==
	ddd_x11_titang2_drom_window_completely_visible){
	sparms->cg_ctl |= CG_CLFRU;
	sparms->cg_ctl |= CG_CLALL;

	sparms->cg_nrects = 0;

	if (dde_x11_titang2_drom_curdevdat->stereoswitch){
	    if (dde_x11_titang2_drom_data.stereo_leye_flag){
		if (dde_x11_titang2_drom_draw_buffer){
		    sparms->cg_window =
			    (double *)dde_x11_titang2_drom_curwindat->obs_leye_bbuf[0];
		}
		else{
		    sparms->cg_window =
			    (double *)dde_x11_titang2_drom_curwindat->obs_leye_fbuf[0];
		}
	    }
	    else{
		if (dde_x11_titang2_drom_draw_buffer){
		    sparms->cg_window =
			    (double *)dde_x11_titang2_drom_curwindat->obs_reye_bbuf[0];
		}
		else{
		    sparms->cg_window =
			    (double *)dde_x11_titang2_drom_curwindat->obs_reye_fbuf[0];
		}
	    }
	}
	else{
	    if (dde_x11_titang2_drom_draw_buffer){
		sparms->cg_window =
			(double *)dde_x11_titang2_drom_curwindat->obs_leye_bbuf[0];
	    }
	    else{
		sparms->cg_window =
			(double *)dde_x11_titang2_drom_curwindat->obs_leye_fbuf[0];
	    }
	}

	sparms->cg_rects = NULL;
    }
    else if (dde_x11_titang2_drom_curwindat->visibility_state ==
	     ddd_x11_titang2_drom_window_partially_obscured){
	sparms->cg_ctl |= CG_CLFRU;
	sparms->cg_ctl |= CG_CLALL;

	sparms->cg_ctl |= CG_CLOBR;

	sparms->cg_nrects = dde_x11_titang2_drom_curwindat->clipping_rectangle_count;

	if (dde_x11_titang2_drom_curdevdat->stereoswitch){
	    if (dde_x11_titang2_drom_data.stereo_leye_flag){
		if (dde_x11_titang2_drom_draw_buffer){
		    sparms->cg_window =
			    (double *)dde_x11_titang2_drom_curdevdat->extent_leye_bbuf;

		    sparms->cg_rects =
			    (double *)dde_x11_titang2_drom_curwindat->obs_leye_bbuf;
		}
		else{
		    sparms->cg_window =
			    (double *)dde_x11_titang2_drom_curdevdat->extent_leye_fbuf;

		    sparms->cg_rects =
			    (double *)dde_x11_titang2_drom_curwindat->obs_leye_fbuf;
		}
	    }
	    else{
		if (dde_x11_titang2_drom_draw_buffer){
		    sparms->cg_window =
			    (double *)dde_x11_titang2_drom_curdevdat->extent_reye_bbuf;

		    sparms->cg_rects =
			    (double *)dde_x11_titang2_drom_curwindat->obs_reye_bbuf;
		}
		else{
		    sparms->cg_window =
			    (double *)dde_x11_titang2_drom_curdevdat->extent_reye_fbuf;

		    sparms->cg_rects =
			    (double *)dde_x11_titang2_drom_curwindat->obs_reye_fbuf;
		}
	    }
	}
	else{
	    if (dde_x11_titang2_drom_draw_buffer){
		sparms->cg_window =
			(double *)dde_x11_titang2_drom_curdevdat->extent_leye_bbuf;

		sparms->cg_rects =
			(double *)dde_x11_titang2_drom_curwindat->obs_leye_bbuf;
	    }
	    else{
		sparms->cg_window =
			(double *)dde_x11_titang2_drom_curdevdat->extent_leye_fbuf;

		sparms->cg_rects =
			(double *)dde_x11_titang2_drom_curwindat->obs_leye_fbuf;
	    }
	}
    }
    else if (dde_x11_titang2_drom_curwindat->visibility_state ==
	     ddd_x11_titang2_drom_window_completely_obscured){
    }

    for (halfspaceno=0;
	 halfspaceno<dde_x11_titang2_drom_data.modelclip_halfspacecount;
	 halfspaceno++){
	dor_math_homo_trns_fast
	(   dde_x11_titang2_drom_data.modelclip_plneqns[halfspaceno],
	    dde_x11_titang2_drom_curcamdat->tspfcstowcsmat,
	    tempplneqn
	);

	tempplneqn[0] /= dde_x11_titang2_drom_curdevdat->ftodscale[0];
	tempplneqn[1] /= dde_x11_titang2_drom_curdevdat->ftodscale[1];
	tempplneqn[2] /= dde_x11_titang2_drom_curdevdat->ftodscale[2];

	dde_x11_titang2_drom_data.modelclip_plneqns[halfspaceno][0] = tempplneqn[0];
	dde_x11_titang2_drom_data.modelclip_plneqns[halfspaceno][1] = tempplneqn[1];
	dde_x11_titang2_drom_data.modelclip_plneqns[halfspaceno][2] = tempplneqn[2];
	dde_x11_titang2_drom_data.modelclip_plneqns[halfspaceno][3] = tempplneqn[3];
    }

    sparms->cg_mplanes = (double *)dde_x11_titang2_drom_data.modelclip_plneqns;

    if (dde_x11_titang2_drom_data.object_modelclip_switch){
	sparms->cg_ctl |= CG_CLMOD;

	sparms->cg_nmplanes = dde_x11_titang2_drom_data.modelclip_halfspacecount;
    }
    else{
	sparms->cg_nmplanes = 0;
    }

    sparms->cg_drefl = dde_x11_titang2_drom_data.diffuse_reflectance;

    if (dde_x11_titang2_drom_data.object_ambient_switch){
	ambwhite[0] = 
		dde_x11_titang2_drom_data.ambient_reflectance[0]*               
			dde_x11_titang2_drom_curlgtdat->ambient_reflectance[0];         
	ambwhite[1] =
		dde_x11_titang2_drom_data.ambient_reflectance[1]*                        
			dde_x11_titang2_drom_curlgtdat->ambient_reflectance[1];         
	ambwhite[2] =
		dde_x11_titang2_drom_data.ambient_reflectance[2]*                        
			dde_x11_titang2_drom_curlgtdat->ambient_reflectance[2];
	sparms->cg_arefl = ambwhite;
    }
    else{
	sparms->cg_arefl = ambblack;
    }

    sparms->cg_srefl = dde_x11_titang2_drom_data.specular_reflectance;

    sparms->cg_nlights = dde_x11_titang2_drom_curlgtdat->count;

    sparms->cg_lnorms = (double *)dde_x11_titang2_drom_curlgtdat->lcscosine;

    sparms->cg_hnorms = (double *)dde_x11_titang2_drom_curlgtdat->h;

    sparms->cg_lcolors = (double *)dde_x11_titang2_drom_curlgtdat->scaledcolor;

    sparms->cg_sptbl = dde_x11_titang2_drom_data.specular_factor_table;

    dprange[0] = 1. / (dde_x11_titang2_drom_data.object_depthcue_zfront-
		       dde_x11_titang2_drom_data.object_depthcue_zback);
    dprange[1] = dde_x11_titang2_drom_data.object_depthcue_zfront;
    dprange[2] = dde_x11_titang2_drom_data.object_depthcue_zback;;

    sparms->cg_dprange = dprange;

    dpfrac[0] = dde_x11_titang2_drom_data.object_depthcue_sfront;
    dpfrac[1] = dde_x11_titang2_drom_data.object_depthcue_sback;

    sparms->cg_dpfracs = dpfrac;

    sparms->cg_dpcolor = dde_x11_titang2_drom_data.object_depthcue_color;

    if (dde_x11_titang2_drom_data.pseudocolor_switch){
	if (dde_x11_titang2_drom_curdevdat->shade_mode == DcComponent){
	    sparms->cg_psweight = cbtselpsweight;

	    sparms->cg_pscol.cg_psmask = cbtselpsmask;
	}
	else if (dde_x11_titang2_drom_curdevdat->shade_mode == DcRange){
	    sparms->cg_psweight = cwtavgpsweight;

	    cwtavgpslim[0] = dde_x11_titang2_drom_curdevdat->current_shade_range_min;
	    cwtavgpslim[1] = dde_x11_titang2_drom_curdevdat->current_shade_range_max;
	    cwtavgpslim[2] = (cwtavgpslim[1]-cwtavgpslim[0]) / 255.;

	    sparms->cg_pscol.cg_pslim = cwtavgpslim;
	}
    }
    else{
	sparms->cg_psweight = NULL;
	sparms->cg_pscol.cg_psmask = NULL;
    }

    dde_x11_titang2_drom_cg_tlparms[0].cg_spb = sparms;
    dde_x11_titang2_drom_cg_tlparms[1].cg_spb = sparms;
    dde_x11_titang2_drom_cg_tlparms[2].cg_spb = sparms;
    dde_x11_titang2_drom_cg_tlparms[3].cg_spb = sparms;

#ifdef DEBUG
    ddr_x11_titang2_drom_print_fixed_parms();
#endif
}
/*
 ======================================================================
 */

/*  Setup the fixed parameters which depend on the primitive object
	being drawn.  */

ddr_x11_titang2_drom_setup_fixed_parms(prigeotyp,bndboxpts,elenrms,vtxnrms,vtxclrs)
DtInt prigeotyp;
DtRealTriple bndboxpts[8];
DtRealTriple *elenrms;
DtRealTriple *vtxnrms;
DtRealTriple *vtxclrs;

{
    struct cg_sparms *sparms;
    static DtColorRGB difwhite;
    static DtColorRGB ambwhite;
    static DtColorRGB ambblack = { 0., 0., 0. };

    sparms = &dde_x11_titang2_drom_cg_sparms;

    /*  Nil out the control masks that are to be setup by this routine.  */

    sparms->cg_ctl &= ~(CG_mNORMS);
    sparms->cg_ctl &= ~(CG_mDRTYP);
    sparms->cg_ctl &= ~(CG_mVCOLS);
    sparms->cg_ctl &= ~(CG_mBDBOX);

    switch (prigeotyp) {
    case ddd_x11_titang2_drom_prigeotyp_trilst:
    case ddd_x11_titang2_drom_prigeotyp_trimsh:
	switch (dde_x11_titang2_drom_data.object_representation_type) {
	case DcPoints:
	    sparms->cg_ctl |= CG_POINTS;
	    break;

	case DcWireframe:
	case DcOutlines:
	    sparms->cg_ctl |= CG_LINES;
	    break;

	case DcSurface:
	case DcSurfaceWireframeEdges:
	case DcSurfaceOutlineEdges:
	    sparms->cg_ctl |= CG_TRIANGLES;
	    break;
	};
	break;

    case ddd_x11_titang2_drom_prigeotyp_linlst:
	switch (dde_x11_titang2_drom_data.object_representation_type) {
	case DcPoints:
	    sparms->cg_ctl |= CG_POINTS;
	    break;

	case DcWireframe:
	case DcOutlines:
	    sparms->cg_ctl |= CG_LINES;
	    break;

	case DcSurface:
	case DcSurfaceWireframeEdges:
	case DcSurfaceOutlineEdges:
	    sparms->cg_ctl |= CG_LINES;
	    break;
	};
	break;

    case ddd_x11_titang2_drom_prigeotyp_conlinlst:
	switch (dde_x11_titang2_drom_data.object_representation_type) {
	case DcPoints:
	    sparms->cg_ctl |= CG_POINTS;
	    break;

	case DcWireframe:
	case DcOutlines:
	    sparms->cg_ctl |= CG_LINES;
	    break;

	case DcSurface:
	case DcSurfaceWireframeEdges:
	case DcSurfaceOutlineEdges:
	    sparms->cg_ctl |= CG_LINES;
	    break;
	};
	break;

    case ddd_x11_titang2_drom_prigeotyp_pntlst:
	switch (dde_x11_titang2_drom_data.object_representation_type) {
	case DcPoints:
	    sparms->cg_ctl |= CG_POINTS;
	    break;

	case DcWireframe:
	case DcOutlines:
	    sparms->cg_ctl |= CG_POINTS;
	    break;

	case DcSurface:
	case DcSurfaceWireframeEdges:
	case DcSurfaceOutlineEdges:
	    sparms->cg_ctl |= CG_POINTS;
	    break;
	};
	break;
    };

    if (vtxclrs != NULL) {
	sparms->cg_ctl |= CG_VCOLS;

	if (dde_x11_titang2_drom_data.transparent_surface_pass &&
	    dde_x11_titang2_drom_data.object_transparent_switch) {
	   
	    difwhite[0] = 
		    dde_x11_titang2_drom_data.object_diffuse_intensity*
			    (1.0 - dde_x11_titang2_drom_data.object_transparent_intensity);
	    difwhite[1] = 
		    dde_x11_titang2_drom_data.object_diffuse_intensity *
			    (1.0 - dde_x11_titang2_drom_data.object_transparent_intensity);
	    difwhite[2] = 
		    dde_x11_titang2_drom_data.object_diffuse_intensity *
			    (1.0 - dde_x11_titang2_drom_data.object_transparent_intensity);
	    sparms->cg_drefl = difwhite;

	    if (dde_x11_titang2_drom_data.object_ambient_switch) {
		ambwhite[0] = 
			dde_x11_titang2_drom_data.object_ambient_intensity *
				dde_x11_titang2_drom_curlgtdat->ambient_reflectance[0]*
					(1.0 - dde_x11_titang2_drom_data.object_transparent_intensity);
		ambwhite[1] = 
			dde_x11_titang2_drom_data.object_ambient_intensity *
				dde_x11_titang2_drom_curlgtdat->ambient_reflectance[1]*
					(1.0 - dde_x11_titang2_drom_data.object_transparent_intensity);
		ambwhite[2] = 
			dde_x11_titang2_drom_data.object_ambient_intensity *
				dde_x11_titang2_drom_curlgtdat->ambient_reflectance[2]*
					(1.0 - dde_x11_titang2_drom_data.object_transparent_intensity);
		sparms->cg_arefl = ambwhite;
	    } else {
		sparms->cg_arefl = ambblack;
	    }	
	} else {
	    difwhite[0] = 
		    dde_x11_titang2_drom_data.object_diffuse_intensity;
	    difwhite[1] = 
		    dde_x11_titang2_drom_data.object_diffuse_intensity;
	    difwhite[2] = 
		    dde_x11_titang2_drom_data.object_diffuse_intensity;
	    sparms->cg_drefl = difwhite;

	    if (dde_x11_titang2_drom_data.object_ambient_switch) {
		ambwhite[0] = 
			dde_x11_titang2_drom_data.object_ambient_intensity*
				dde_x11_titang2_drom_curlgtdat->ambient_reflectance[0];
		ambwhite[1] = 
			dde_x11_titang2_drom_data.object_ambient_intensity*
				dde_x11_titang2_drom_curlgtdat->ambient_reflectance[1];
		ambwhite[2] = 
			dde_x11_titang2_drom_data.object_ambient_intensity*
				dde_x11_titang2_drom_curlgtdat->ambient_reflectance[2];
		sparms->cg_arefl = ambwhite;
	    } else {
		sparms->cg_arefl = ambblack;
	    }
	}

    } else {
	sparms->cg_ctl |= CG_NOVCOLS;

	if (dde_x11_titang2_drom_data.transparent_surface_pass &&
	    dde_x11_titang2_drom_data.object_transparent_switch) {
	    difwhite[0] = 
		    dde_x11_titang2_drom_data.diffuse_reflectance[0] *
			    (1.0 - dde_x11_titang2_drom_data.object_transparent_intensity);
	    difwhite[1] = 
		    dde_x11_titang2_drom_data.diffuse_reflectance[1] *
			    (1.0 - dde_x11_titang2_drom_data.object_transparent_intensity);
	    difwhite[2] = 
		    dde_x11_titang2_drom_data.diffuse_reflectance[2] *
			    (1.0 - dde_x11_titang2_drom_data.object_transparent_intensity);
	    sparms->cg_drefl = difwhite;

	    if (dde_x11_titang2_drom_data.object_ambient_switch) {
		ambwhite[0] = 
			dde_x11_titang2_drom_data.ambient_reflectance[0] *
				dde_x11_titang2_drom_curlgtdat->ambient_reflectance[0] *
					(1.0 - dde_x11_titang2_drom_data.object_transparent_intensity);
		ambwhite[1] = 
			dde_x11_titang2_drom_data.ambient_reflectance[1] *
				dde_x11_titang2_drom_curlgtdat->ambient_reflectance[1] *
					(1.0 - dde_x11_titang2_drom_data.object_transparent_intensity);
		ambwhite[2] = 
			dde_x11_titang2_drom_data.ambient_reflectance[2] *
				dde_x11_titang2_drom_curlgtdat->ambient_reflectance[2] *
					(1.0 - dde_x11_titang2_drom_data.object_transparent_intensity);
		sparms->cg_arefl = ambwhite;
	    } else {
		sparms->cg_arefl = ambblack;
	    }
	} else {
	    sparms->cg_drefl = dde_x11_titang2_drom_data.diffuse_reflectance;
	    if (dde_x11_titang2_drom_data.object_ambient_switch) {
		ambwhite[0] =
			dde_x11_titang2_drom_data.ambient_reflectance[0]*
				dde_x11_titang2_drom_curlgtdat->ambient_reflectance[0];
		ambwhite[1] =
			dde_x11_titang2_drom_data.ambient_reflectance[1]*
				dde_x11_titang2_drom_curlgtdat->ambient_reflectance[1];
		ambwhite[2] =
			dde_x11_titang2_drom_data.ambient_reflectance[2]*
				dde_x11_titang2_drom_curlgtdat->ambient_reflectance[2];
		sparms->cg_arefl = ambwhite;
	    } else {
		sparms->cg_arefl = ambblack;
	    }
	}
    }

    /*  If the current surface shader is "constant", then we force draw mode
	by setting the normals switch to NO NORMALS.  */

    if (dde_x11_titang2_drom_data.draw_switch) {
	sparms->cg_ctl |= CG_NONORMS;
    } else {
	switch (dde_x11_titang2_drom_data.object_interpolation_type) {

	    /*  The user has requested a single shade per element.  If the
		object can provide element normals (ie. triangle meshes and
		triangle lists), then he will have provided them.  If element
		normals were not provided, then two cases can exist.  Either
		the object can supply vertex normals (ie. line lists with vertex
		normals) and it will have done so, or the object cannot provide
		vertex normals (ie. line lists and point lists without attached
		vertex normals).  If vertex normals have been provided, then we
		will use them, otherwise we assume that no light source shading
		is to be performed, even though it was requested by the user.
		In this case, it is also assumed that if the object contained an
		object implicit normal (ie. text), then the diffuse color global
		attribute was pushed with the light source shaded value for the
		entire object, so that non light source shading will result in
		the correct shade for the object.  */

	case DcConstantShade:
	    if (elenrms != NULL) {
		sparms->cg_ctl |= CG_ENORMS;
	    } else if (vtxnrms != NULL) {
		sparms->cg_ctl |= CG_VNORMS;
	    } else {
		sparms->cg_ctl |= CG_NONORMS;
	    }
	    break;

	    /*  The user has requested a shade per vertex.  If the
		object can provide vertex normals then he will have provided
		them.  If vertex normals were not provided, then two cases can
		exist.  Either the object can supply element normals and it will
		have done so, or the object cannot provide element normals.
		If element normals have been provided, then we will use them,
		otherwise we assume that no light source shading is to be
		performed, even though it was requested by the user.
		In this case, it is also assumed that if the object contained an
		object implicit normal, then the diffuse color global
		attribute was pushed with the light source shaded value for the
		entire object, so that non light source shading will result in
		the correct shade for the object.  Note that if the user 
		requested surface shading (one shade per pixel) it is ignored 
		for this rendering style at this time, and is backed down to 
		vertex shading.  */

	case DcVertexShade:
	case DcSurfaceShade:
	    if (vtxnrms != NULL) {
		sparms->cg_ctl |= CG_VNORMS;
	    } else if (elenrms != NULL) {
		sparms->cg_ctl |= CG_ENORMS;
	    } else {
		sparms->cg_ctl |= CG_NONORMS;
	    }
	    break;
	}
    }

    if (bndboxpts != NULL) {
	sparms->cg_ctl |= CG_BDBOX;
	sparms->cg_bdbox = (double *)bndboxpts;
    } else {
	sparms->cg_bdbox = NULL;
    }
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_setup_mesh_space(vertexcount,threadcount)
DtInt vertexcount;
DtInt threadcount;
{
    DtPtr DDspace_Reallocate();

    if (vertexcount > dde_x11_titang2_drom_data.mesh_vertex_count){
	dde_x11_titang2_drom_data.mesh_vertex_space =
		(double *)DDspace_Reallocate(dde_x11_titang2_drom_data.mesh_vertex_space,
					     vertexcount*3*(sizeof (DtReal)));

	dde_x11_titang2_drom_data.mesh_perspective_space =
		(double *)DDspace_Reallocate(dde_x11_titang2_drom_data.mesh_perspective_space,
					     vertexcount*3*(sizeof (DtReal)));

	dde_x11_titang2_drom_data.mesh_vertex_count = vertexcount;

	dde_x11_titang2_drom_cg_tlparms[0].cg_xfverts =
		dde_x11_titang2_drom_data.mesh_vertex_space;
	dde_x11_titang2_drom_cg_tlparms[1].cg_xfverts =
		dde_x11_titang2_drom_data.mesh_vertex_space;
	dde_x11_titang2_drom_cg_tlparms[2].cg_xfverts =
		dde_x11_titang2_drom_data.mesh_vertex_space;
	dde_x11_titang2_drom_cg_tlparms[3].cg_xfverts =
		dde_x11_titang2_drom_data.mesh_vertex_space;

	dde_x11_titang2_drom_cg_tlparms[0].cg_xfpersp =
		dde_x11_titang2_drom_data.mesh_perspective_space;
	dde_x11_titang2_drom_cg_tlparms[1].cg_xfpersp =
		dde_x11_titang2_drom_data.mesh_perspective_space;
	dde_x11_titang2_drom_cg_tlparms[2].cg_xfpersp =
		dde_x11_titang2_drom_data.mesh_perspective_space;
	dde_x11_titang2_drom_cg_tlparms[3].cg_xfpersp =
		dde_x11_titang2_drom_data.mesh_perspective_space;
    }

    dde_x11_titang2_drom_data.mesh_sync = -threadcount;

    dde_x11_titang2_drom_cg_tlparms[0].cg_meshsync = 
	    &dde_x11_titang2_drom_data.mesh_sync;
    dde_x11_titang2_drom_cg_tlparms[1].cg_meshsync = 
	    &dde_x11_titang2_drom_data.mesh_sync;
    dde_x11_titang2_drom_cg_tlparms[2].cg_meshsync = 
	    &dde_x11_titang2_drom_data.mesh_sync;
    dde_x11_titang2_drom_cg_tlparms[3].cg_meshsync = 
	    &dde_x11_titang2_drom_data.mesh_sync;
}
/*
 ======================================================================
 */

/*  Setup the variable mesh parameters which depend on the primitive
    object being drawn.  */

ddr_x11_titang2_drom_setup_variable_mesh_parms(bufferno,vertexcount,firstvertex,
		tclist)
DtInt bufferno;
DtInt vertexcount;
DtInt tclist[][3];
{
    threadlocal struct cg_tlparms *tlparms;

    tlparms = &dde_x11_titang2_drom_cg_tlparms[bufferno];

    tlparms->cg_nverts = vertexcount;

    tlparms->cg_fstvert = firstvertex;

    tlparms->cg_tclist = (int *)tclist;
}
/*
 ======================================================================
 */

/*  Setup the variable parameters which depend on the primitive object
    being drawn.  */

ddr_x11_titang2_drom_setup_variable_parms(bufferno,elementcount,vtxlocs,elenrms,
		vtxnrms,vtxclrs)
DtInt bufferno;
DtInt elementcount;
DtRealTriple *vtxlocs;
DtRealTriple *elenrms;
DtRealTriple *vtxnrms;
DtRealTriple *vtxclrs;
{
    threadlocal struct cg_tlparms *tlparms;

    tlparms = &dde_x11_titang2_drom_cg_tlparms[bufferno];

    tlparms->cg_nelts = elementcount;

    tlparms->cg_verts = (double *)vtxlocs;

    switch (dde_x11_titang2_drom_cg_sparms.cg_ctl & CG_mNORMS){
    case CG_ENORMS:
	tlparms->cg_norms = (double *)elenrms;
	break;

    case CG_VNORMS:
	tlparms->cg_norms = (double *)vtxnrms;
	break;

    case CG_NONORMS:
	tlparms->cg_norms = NULL;
	break;
    }

    tlparms->cg_vcolors = (double *)vtxclrs;
}
