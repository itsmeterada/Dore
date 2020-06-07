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
 
/* ======================================================================
  Functions:
	dor_pick_setup
	dor_pick_view
	dor_pick
	dor_pick_compute_wcstofcs

  ====================================================================== */

#include <math.h>
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include "pick.h"

/* ====================================================================== */

void dor_pick_setup (
    DtObject    deviceobj,
    DtInt      *hit_count,
    DtInt       index_size,
    DtInt       index[],
    DtInt       list_size,
    long        hit_list[],
    DtReal      z_values[],
    DtReal      wcs_points[],
    DtReal      lcs_points[],
    dot_object *views[],
    DtInt      *error_word)
{
    dot_object *current_callback;

    doe_pick_data.hit_count  = hit_count;
    doe_pick_data.index_size = index_size;
    doe_pick_data.index      = index;
    doe_pick_data.list_size  = list_size;
    doe_pick_data.hit_list   = hit_list;
    doe_pick_data.z_values   = z_values;
    doe_pick_data.wcs_points = wcs_points;
    doe_pick_data.lcs_points = lcs_points;
    doe_pick_data.views      = views;
    doe_pick_data.error_word = error_word;

    *doe_pick_data.hit_count = *doe_pick_data.error_word  = 0;

    /* no hits, no errors */

    doe_pick_data.next_hit = doe_pick_data.last_hit = hit_list;

    doe_pick_data.stack_top = doe_pick_data.stack_base -3;

    current_callback = dor_device_inq_pick_callback (deviceobj);

    doe_pick_data.callback_function = (DtHitStatus (*)())
	    dor_calbak_get_function(current_callback);

    doe_pick_data.callback_data =
	    dor_calbak_get_dataobject(current_callback);

    doe_pick_data.pass_by_ref =
	    dor_calbak_get_pass_by_ref (current_callback);
}

/* ====================================================================== */

void dor_pick_view (
    dot_object *device,
    dot_object *view)
{
    doe_pick_data.device = device;
    doe_pick_data.view = view;

    dor_pick(view);
}

/* ====================================================================== */

void dor_pick (dot_object *view)
{
    DtMethodPtr pickmethod;
    dot_object *displaygroup;

    if (view == NULL){
	DDerror (ERR_INVALID_VIEW, "dor_pick", DcNullPtr);
	return;
    }

    displaygroup = dor_view_inq_display_group(view);

    /*  Get data related to the camera.	*/

    dor_pick_initialize_environment();

    dor_pick_compute_wcstofcs();

    /*  Indicate to the conditional execution utility code that
	objects are not to be skipped as part of a transparent 
	rendering pass.  */

    dor_condex_set_disable_prim_exec_flag(DcFalse);

    /*  Indicate to all objects the current method being executed.  */

    doe_system_current_method = DcMethodPick;

    /*  Perform the picking.		*/

    pickmethod = dor_object_get_current_method(displaygroup);
    (*pickmethod)(displaygroup);

    /*  Indicate that no method is being executed.  */

    doe_system_current_method = DcMethodNull;
}
/*
 ======================================================================
 */

	/* Transformation pipeline:

	Dore implements a rather nice pipeline to meet both 
	simple user models of viewing and projection and the 
	PHIGS pipeline.  The following coordinate systems and 
	transformations are employed.

		Modelling (Local) Coordinates 
				|
		MODELLING TRANSFORMATIONS
				|
		World Coordinates
				|
		VIEW MAPPING TRANSFORMATION
				|
		View Reference (camera coordinates)
				|
		VIEW PROJECTION TRANSFORMATION
				|
		View Coordinates
				|
		Frame Coordinates
				|
		Clipping, w-divide, and WORKSTATION TRANSFORMATION
				|
		Device Coordinates

	The modelling transformations are specified as objects in 
	the tree.

	The view mapping matrix can be specified directly for 
	the view or using modelling matrices and a camera.

	The view projection transformation can be specified directly
	for the view or can be specified using the perspective
	structure element.  This is then combined with a 
	transformation from the cannonical clipping volume to the 
	projection viewport.

	The workstation transformation is specified by calls to set 
	the workstation window and workstation viewport.  */

void dor_pick_compute_wcstofcs (void)
{
    DtReal xscale, yscale, zscale;
    DtReal xtrans, ytrans, ztrans, wtrans;
    DtMatrix4x4 identmatrix;
    int status;
    DtVolume *clipvolume;
    DtVolume *viewboundary;
    DtReal xres, yres;

    /* map from WC to VRC */

    dor_matrix_load (doe_pick_data.wcstofcsmat, doe_pick_data.wcstoccsmat);

#ifdef debug
    printf("cmp:  wcstoccsmat:\n");
    dor_matrix_print(doe_pick_data.wcstoccsmat);
#endif

    /* map from VRC to canonical volume for single view */

    dor_matrix_post_concatenate(doe_pick_data.wcstofcsmat,
				doe_pick_data.projmat);

#ifdef debug
    printf("cmp:  wcstofcsmat after projat post_concatenate:\n");
    dor_matrix_print(doe_pick_data.wcstofcsmat);
#endif

    /* map from canonical volume to projection viewport in NPC */

    if ( doe_pick_data.viewtype != dod_pick_viewtype_matrix ){
	viewboundary = dor_view_inq_boundary_ptr(doe_pick_data.view);

	xscale = (viewboundary->fur[0]-viewboundary->bll[0])/2.;
	yscale = (viewboundary->fur[1]-viewboundary->bll[1])/2.;
	zscale = (viewboundary->fur[2]-viewboundary->bll[2]);

	xtrans = (viewboundary->fur[0]+viewboundary->bll[0])/2.;
	ytrans = (viewboundary->fur[1]+viewboundary->bll[1])/2.;
	ztrans = viewboundary->fur[2];

	/*  get the screen resolution to account for 
	    any aspect ratio mismatch from the view system
	    This is generally due to a stereo screen */

	dor_device_inq_resolution(doe_pick_data.device,&xres,&yres);
	if (xres != 0.0) {
	    xscale *= (yres / xres);
	}

	dor_matrix_scale(doe_pick_data.wcstofcsmat,
			 xscale,yscale,zscale,DcPostConcatenate);

	dor_matrix_translate(doe_pick_data.wcstofcsmat,
			     xtrans,ytrans,ztrans,DcPostConcatenate);
    }

#ifdef debug
    printf("cmp:  wcstofcsmat after map to npc:\n");
    dor_matrix_print(doe_pick_data.wcstofcsmat);
#endif

    /* map from clipping region in NPC to canonical clipping volume */

    clipvolume = dor_device_inq_clipping_volume_ptr(doe_pick_data.device);

    xscale = 2./(clipvolume->fur[0]-clipvolume->bll[0]);
    yscale = 2./(clipvolume->fur[1]-clipvolume->bll[1]);
    zscale = 1./(clipvolume->fur[2]-clipvolume->bll[2]);

    xtrans = -xscale*clipvolume->bll[0]-1;
    ytrans = -yscale*clipvolume->bll[1]-1;
    ztrans = -zscale*clipvolume->fur[2];

    dor_matrix_scale(doe_pick_data.wcstofcsmat,
		     xscale,yscale,zscale,
		     DcPostConcatenate);

    dor_matrix_translate(doe_pick_data.wcstofcsmat,
			 xtrans,ytrans,ztrans,
			 DcPostConcatenate);

#ifdef debug
    printf("cmp:  wcstofcsmat after map to canonical:\n");
    dor_matrix_print(doe_pick_data.wcstofcsmat);
#endif

    /* compute camera type, location and normal */

    dor_matrix_load_identity(identmatrix);

    status = dor_matrix_solve(doe_pick_data.wcstofcsmat,
			      doe_pick_data.npctowcsmat,identmatrix);

    if (status != 10){
	DDerror (ERR_CANT_INVERT_MATRIX,
		    "dor_pick_compute_ctm", DcNullPtr);
	return;
    }

#ifdef debug
    printf("cmp:  wcstofcsmat after solve:\n");
    dor_matrix_print(doe_pick_data.wcstofcsmat);
#endif

    dor_math_homo_trns(0.,0.,1.,0.,doe_pick_data.npctowcsmat,
		       &xtrans,&ytrans,&ztrans,&wtrans);

    /* if wtrans is close to zero, the camera is so far away that
       it doesn't make sense to do a perspective projection, so
       we switch to parallel */
    if (( abs(wtrans) > 1.e-20 ) &&
	(doe_pick_data.viewtype == dod_pick_viewtype_perspective)){
	doe_pick_data.camtype = DcPerspective;
	doe_pick_data.wcscamloc[0] = xtrans/wtrans;
	doe_pick_data.wcscamloc[1] = ytrans/wtrans;
	doe_pick_data.wcscamloc[2] = ztrans/wtrans;

#ifdef debug
	printf("positional viewpoint wtrans = %f\n", wtrans);
	printf("eye point = (%f,%f,%f)\n",
	       doe_pick_data.wcscamloc[0],
	       doe_pick_data.wcscamloc[1],
	       doe_pick_data.wcscamloc[2]);
#endif
    }
    else{
	doe_pick_data.camtype = DcParallel;

#ifdef debug
	printf("directional viewpoint wtrans = %f\n", wtrans);
	fflush(stdout);
#endif
    }

    dor_math_vectrns(0.,0.,1.,
		     doe_pick_data.npctowcsmat,
		     &doe_pick_data.wcscamnorm[0],
		     &doe_pick_data.wcscamnorm[1],
		     &doe_pick_data.wcscamnorm[2]);

    dor_math_renormalize_fast(doe_pick_data.wcscamnorm);

#ifdef debug
    printf("camera normal = (%f,%f,%f)\n",
	   doe_pick_data.wcscamnorm[0],
	   doe_pick_data.wcscamnorm[1],
	   doe_pick_data.wcscamnorm[2]);
#endif

    dor_matrix_load(doe_pick_data.fcstowcsmat,
		    doe_pick_data.wcstofcsmat);
    dor_matrix_invert (doe_pick_data.fcstowcsmat);
}
