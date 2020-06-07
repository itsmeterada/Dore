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
	int ddr_sunview_cxp_drom_camera ()
	int ddr_sunview_cxp_drom_compute_wcstofcs ()
	int ddr_sunview_cxp_drom_initialize_ccstowcsmat ()
	ddr_sunview_cxp_drom_set_att_stereo ddr_sunview_cxp_drom_set_att_stereo (eyeseparation, distance)
	ddr_sunview_cxp_drom_set_att_stereoswi ddr_sunview_cxp_drom_set_att_stereoswi (switchvalue)
	int ddr_sunview_cxp_drom_set_camera_matrix (matrix)
	int ddr_sunview_cxp_drom_set_parallel_matrix (fov,hither,yon)
	int ddr_sunview_cxp_drom_set_perspective_matrix (fov,hither,yon)
	int ddr_sunview_cxp_drom_set_projection_matrix (window,ptype,prp,view_plane,hither,yon)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_camera()
{
    dor_matrix_load(dde_sunview_cxp_drom_curcamdat->projmat,
		     dde_sunview_cxp_drom_data.projmat);
    dde_sunview_cxp_drom_curcamdat->viewtype = 
	    dde_sunview_cxp_drom_data.viewtype;

    ddr_sunview_cxp_drom_initialize_ccstowcsmat();
}
/*
 ======================================================================
 */

      extern double frexp();

/* Transformation pipeline:

   Dore implements a rather nice pipeline to meet both simple user
   models of viewing and projection and the PHIGS pipeline.  The
   following coordinate systems and transformations are employed.

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

   The modelling transformations are specified as objects in the
   tree.

   The view mapping matrix can be specified directly for the view
   or using modelling matrices and a camera.

   The view projection transformation can be specified directly
   for the view or can be specified using the perspective
   structure element.  This is then combined with a transformation
   from the cannonical clipping volume to the projection
   viewport.

   The workstation transformation is specified by calls to set the
   workstation window and workstation viewport.  */

ddr_sunview_cxp_drom_compute_wcstofcs()

{
    DtReal xscale, yscale, zscale;
    DtReal xtrans, ytrans, ztrans, wtrans;
    int xexp, yexp, zexp, wexp;
    double wsig;
    DtMatrix4x4 identmatrix;
    int status;
    int dor_matrix_solve();
    DtVolume *clipvolume;
    DtVolume viewboundary;
    DtReal xres, yres;

    /* map from WC to VRC */

    dor_matrix_load(dde_sunview_cxp_drom_curcamdat->wcstofcsmat,
		     dde_sunview_cxp_drom_curcamdat->wcstoccsmat);

#ifdef debug
    printf("cmp:  wcstoccsmat:\n");
    dor_matrix_print(dde_sunview_cxp_drom_curcamdat->wcstoccsmat);
#endif

    /* map from VRC to canonical volume for single view */

    dor_matrix_post_concatenate(dde_sunview_cxp_drom_curcamdat->wcstofcsmat,
				 dde_sunview_cxp_drom_curcamdat->projmat);

#ifdef debug
    printf("cmp:  wcstofcsmat after projat post_concatenate:\n");
    dor_matrix_print(dde_sunview_cxp_drom_curcamdat->wcstofcsmat);
#endif

    /* map from canonical volume to projection viewport in NPC */

    if (dde_sunview_cxp_drom_curcamdat->viewtype != ddd_sunview_cxp_drom_viewtype_matrix) {
	DvInqBoundary(dde_sunview_cxp_drom_data.current_view, &viewboundary);

	xscale = (viewboundary.fur[0]-viewboundary.bll[0])/2.;
	yscale = (viewboundary.fur[1]-viewboundary.bll[1])/2.;
	zscale = (viewboundary.fur[2]-viewboundary.bll[2]);

	xtrans = (viewboundary.fur[0]+viewboundary.bll[0])/2.;
	ytrans = (viewboundary.fur[1]+viewboundary.bll[1])/2.;
	ztrans = viewboundary.fur[2];

	/* Correct for aspect ratio mismatch from view subsystem.
	   This is ususally caused by a stereo device
	   NOTE: This is for completeness since the sun port does not
	   currently support stereo.  It is intended to show how this
	   could be handled */

	ddr_sunview_cxp_dcm_inquire_resolution (&xres,&yres);
	if (xres != 0.0) {
	    xscale *= (yres / xres);
	}

	dor_matrix_scale(dde_sunview_cxp_drom_curcamdat->wcstofcsmat,
			  xscale,yscale,zscale,DcPostConcatenate);

	dor_matrix_translate(dde_sunview_cxp_drom_curcamdat->wcstofcsmat,
			      xtrans,ytrans,ztrans,DcPostConcatenate);
    }

#ifdef debug
    printf("cmp:  wcstofcsmat after map to npc:\n");
    dor_matrix_print(dde_sunview_cxp_drom_curcamdat->wcstofcsmat);
#endif

    /* map from clipping region in NPC to canonical clipping volume */

    clipvolume = DDdevice_InqClippingVolume(dde_sunview_cxp_drom_data.current_device);

    xscale = 2./(clipvolume->fur[0]-clipvolume->bll[0]);
    yscale = 2./(clipvolume->fur[1]-clipvolume->bll[1]);
    zscale = 1./(clipvolume->fur[2]-clipvolume->bll[2]);

    xtrans = -xscale*clipvolume->bll[0]-1;
    ytrans = -yscale*clipvolume->bll[1]-1;
    ztrans = -zscale*clipvolume->fur[2];

    dor_matrix_scale(dde_sunview_cxp_drom_curcamdat->wcstofcsmat,xscale,yscale,zscale,
		      DcPostConcatenate);

    dor_matrix_translate(dde_sunview_cxp_drom_curcamdat->wcstofcsmat,xtrans,ytrans,ztrans,
			  DcPostConcatenate);

#ifdef debug
    printf("cmp:  wcstofcsmat after map to canonical:\n");
    dor_matrix_print(dde_sunview_cxp_drom_curcamdat->wcstofcsmat);
#endif

    /* compute camera type, location and normal */

    dor_matrix_load_identity(identmatrix);

    status = dor_matrix_solve(dde_sunview_cxp_drom_curcamdat->wcstofcsmat,
			       dde_sunview_cxp_drom_curcamdat->npctowcsmat,identmatrix);

    if (status != 10) {
	DDerror (ERR_CANT_INVERT_MATRIX,
		    "ddr_sunview_cxp_drom_compute_ctm", DcNullPtr);
	return;
    }

#ifdef debug
    printf("cmp:  wcstofcsmat after solve:\n");
    dor_matrix_print(dde_sunview_cxp_drom_curcamdat->wcstofcsmat);
#endif

    dor_math_homo_trns(0.,0.,1.,0.,dde_sunview_cxp_drom_curcamdat->npctowcsmat,
			&xtrans,&ytrans,&ztrans,&wtrans);

    /* if wtrans is close to zero, the camera is so far away that
       it doesn't make sense to do a perspective projection, so
       we switch to parallel */
    if (( abs(wtrans) > 1.e-20 ) &&
	(dde_sunview_cxp_drom_curcamdat->viewtype == ddd_sunview_cxp_drom_viewtype_perspective)){
	dde_sunview_cxp_drom_curcamdat->camtype = DcPerspective;
	dde_sunview_cxp_drom_curcamdat->wcscamloc[0] = xtrans/wtrans;
	dde_sunview_cxp_drom_curcamdat->wcscamloc[1] = ytrans/wtrans;
	dde_sunview_cxp_drom_curcamdat->wcscamloc[2] = ztrans/wtrans;

#ifdef debug
	printf("positional viewpoint wtrans = %f\n", wtrans);
	printf("eye point = (%f,%f,%f)\n",
	       dde_sunview_cxp_drom_curcamdat->wcscamloc[0],
	       dde_sunview_cxp_drom_curcamdat->wcscamloc[1],
	       dde_sunview_cxp_drom_curcamdat->wcscamloc[2]);
#endif
    }
    else{
	dde_sunview_cxp_drom_curcamdat->camtype = DcParallel;

#ifdef debug
	printf("directional viewpoint wtrans = %f\n", wtrans);
	fflush(stdout);
#endif
    }

    dor_math_vectrns(0.,0.,1.,
		      dde_sunview_cxp_drom_curcamdat->npctowcsmat,
		      &dde_sunview_cxp_drom_curcamdat->wcscamnorm[0],
		      &dde_sunview_cxp_drom_curcamdat->wcscamnorm[1],
		      &dde_sunview_cxp_drom_curcamdat->wcscamnorm[2]);

    ddr_sunview_cxp_drom_renormalize(dde_sunview_cxp_drom_curcamdat->wcscamnorm);

#ifdef debug
    printf("camera normal = (%f,%f,%f)\n",
	   dde_sunview_cxp_drom_curcamdat->wcscamnorm[0],
	   dde_sunview_cxp_drom_curcamdat->wcscamnorm[1],
	   dde_sunview_cxp_drom_curcamdat->wcscamnorm[2]);
#endif
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_initialize_ccstowcsmat()

{
    dor_matrix_load(dde_sunview_cxp_drom_curcamdat->ccstowcsmat,
		     dde_sunview_cxp_drom_data.object_lcstowcsmat);
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_set_att_stereo (eyeseparation, distance)
     DtReal eyeseparation;
     DtReal distance;
{
    dde_sunview_cxp_drom_data.stereo_eyeseparation = eyeseparation;
    dde_sunview_cxp_drom_data.stereo_distance = distance;
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_set_att_stereoswi (switchvalue)
     DtSwitch switchvalue;
{
    dde_sunview_cxp_drom_data.stereo_switch = switchvalue;
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_set_camera_matrix(matrix)
     DtMatrix4x4 matrix;
{

    dde_sunview_cxp_drom_data.viewtype = ddd_sunview_cxp_drom_viewtype_matrix;
	
    dor_matrix_load(dde_sunview_cxp_drom_data.projmat,matrix);
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_set_parallel_matrix(fov,hither,yon)
     DtReal fov;
     DtReal hither,yon;

{
    DtReal yoverxscalefactor,extentx,extenty;
    DtVolume volume;

    dde_sunview_cxp_drom_data.viewtype = ddd_sunview_cxp_drom_viewtype_parallel;

    DvInqBoundary(dde_sunview_cxp_drom_data.current_view, &volume);

    extentx = volume.fur[0] - volume.bll[0];
    extenty = volume.fur[1] - volume.bll[1];

    yoverxscalefactor = extenty / extentx;

    if ( extentx >= extenty )
	    dor_matrix_load_scale(dde_sunview_cxp_drom_data.projmat,yoverxscalefactor,1.,1.);
    else
	    dor_matrix_load_scale(dde_sunview_cxp_drom_data.projmat,1.,1./yoverxscalefactor,1.);

    dor_matrix_parallel(dde_sunview_cxp_drom_data.projmat,fov,hither,yon,
			 DcPostConcatenate);
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_set_perspective_matrix(fov,hither,yon)
     DtReal fov;
     DtReal hither,yon;

{
    DtReal yoverxscalefactor,extentx,extenty;
    DtVolume volume;

    dde_sunview_cxp_drom_data.viewtype = ddd_sunview_cxp_drom_viewtype_perspective;

    DvInqBoundary(dde_sunview_cxp_drom_data.current_view, &volume);

    extentx = volume.fur[0] - volume.bll[0];
    extenty = volume.fur[1] - volume.bll[1];

    yoverxscalefactor = extenty / extentx;

    if (extentx >= extenty) {
	dor_matrix_load_scale(dde_sunview_cxp_drom_data.projmat,yoverxscalefactor,1.,1.);

    } else {
	dor_matrix_load_scale(dde_sunview_cxp_drom_data.projmat,1.,1./yoverxscalefactor,1.);
    }

    dor_matrix_perspective(dde_sunview_cxp_drom_data.projmat,fov,hither,yon,
			    DcPostConcatenate);
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_set_projection_matrix(window,ptype,prp,view_plane,hither,yon)
     DtArea *window;
     DtProjectionType ptype;
     DtPoint3 prp;
     DtReal view_plane;
     DtReal hither;
     DtReal yon;

{
    switch (ptype) {
    case DcParallel:
	dde_sunview_cxp_drom_data.viewtype = ddd_sunview_cxp_drom_viewtype_parallel;
	break;

    case DcPerspective:
	dde_sunview_cxp_drom_data.viewtype = ddd_sunview_cxp_drom_viewtype_perspective;
	break;
    }
	
    dor_matrix_load_projection(dde_sunview_cxp_drom_data.projmat,window,ptype,prp,
				view_plane,hither,yon);
}
