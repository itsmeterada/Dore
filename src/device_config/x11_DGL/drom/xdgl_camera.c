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
	int ddr_x11_DGL_drom_camera ()
	int ddr_x11_DGL_drom_compute_wcstofcs ()
	int ddr_x11_DGL_drom_initialize_ccstowcsmat ()
	ddr_x11_DGL_drom_set_att_stereo ddr_x11_DGL_drom_set_att_stereo (eyeseparation, distance)
	ddr_x11_DGL_drom_set_att_stereoswi ddr_x11_DGL_drom_set_att_stereoswi (switchvalue)
	int ddr_x11_DGL_drom_set_camera_matrix (matrix)
	int ddr_x11_DGL_drom_set_parallel_matrix (fov,hither,yon)
	int ddr_x11_DGL_drom_set_perspective_matrix (fov,hither,yon)
	int ddr_x11_DGL_drom_set_projection_matrix (window,ptype,prp,view_plane,hither,yon)

  ======================================================================
 */
#include "drom.h"
#include <dore/internal/matrix.h>
/*
 ======================================================================
 */

ddr_x11_DGL_drom_camera()
{
    dde_x11_DGL_drom_curcamdat->stereoswitch = 
	    dde_x11_DGL_drom_data.stereo_switch;
    dde_x11_DGL_drom_curcamdat->stereo_eyeseparation =
	    dde_x11_DGL_drom_data.stereo_eyeseparation;
    dde_x11_DGL_drom_curcamdat->stereo_distance = 
	    dde_x11_DGL_drom_data.stereo_distance;

    XdMatrixLoad(dde_x11_DGL_drom_curcamdat->projmat,
		 dde_x11_DGL_drom_data.projmat);
    dde_x11_DGL_drom_curcamdat->viewtype = 
	    dde_x11_DGL_drom_data.viewtype;

    ddr_x11_DGL_drom_initialize_ccstowcsmat();
}
/*
 ======================================================================
 */

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

ddr_x11_DGL_drom_compute_wcstofcs()

{
    XdReal scale[3];
    XdReal trans[3];
    XdReal xtrans, ytrans, ztrans, wtrans;
    XdMatrix4x4 identmatrix;
    int status;
    DtVolume *clipvolume;
    DtVolume viewboundary;
    DtReal xres, yres;

    XdMatrixLoad(dde_x11_DGL_drom_curcamdat->wcstofcsmat,
		 dde_x11_DGL_drom_curcamdat->wcstoccsmat);

    /* map from VRC to canonical volume for single view */

    XdMatrixConcat(dde_x11_DGL_drom_curcamdat->wcstofcsmat,
		   dde_x11_DGL_drom_curcamdat->projmat, XdPostConcatenate);

#ifdef DEBUG
    printf("cmp:  wcstofcsmat after projmat\n");
    dor_matrix_print(dde_x11_DGL_drom_curcamdat->wcstofcsmat);
#endif

    /* map from canonical volume to projection viewport in NPC */

	DvInqBoundary(dde_x11_DGL_drom_data.current_view, &viewboundary);

	scale[0] = (viewboundary.fur[0]-viewboundary.bll[0])/2.;
	scale[1] = (viewboundary.fur[1]-viewboundary.bll[1])/2.;
	scale[2] = (viewboundary.fur[2]-viewboundary.bll[2]);

	trans[0] = (viewboundary.fur[0]+viewboundary.bll[0])/2.;
	trans[1] = (viewboundary.fur[1]+viewboundary.bll[1])/2.;
	trans[2] = viewboundary.fur[2];

	/*  If this is a stereo view, correct for the .5 x and y scaling
	    caused by the aspect ratio mismatch from the view subsystem.  */

	ddr_x11_DGL_dcm_inquire_resolution (&xres,&yres);
	if (xres != 0.0) {
	    scale[0] *= (yres / xres);
	}

	XdMatrixScale(dde_x11_DGL_drom_curcamdat->wcstofcsmat,
		      scale[0], scale[1], scale[2], XdPostConcatenate);

	XdMatrixTranslate(dde_x11_DGL_drom_curcamdat->wcstofcsmat,
			  trans[0], trans[1], trans[2], XdPostConcatenate);

#ifdef DEBUG
    printf("cmp:  wcstofcsmat after map to npc:\n");
    dor_matrix_print(dde_x11_DGL_drom_curcamdat->wcstofcsmat);
#endif

    /* map from clipping region in NPC to canonical clipping volume */

    clipvolume = DDdevice_InqClippingVolume(dde_x11_DGL_drom_data.current_device);

    scale[0] = 2./(clipvolume->fur[0]-clipvolume->bll[0]);
    scale[1] = 2./(clipvolume->fur[1]-clipvolume->bll[1]);
    scale[2] = 1./(clipvolume->fur[2]-clipvolume->bll[2]);

    trans[0] = -scale[0]*clipvolume->bll[0]-1;
    trans[1] = -scale[1]*clipvolume->bll[1]-1;
    trans[2] = -scale[2]*clipvolume->fur[2];

    XdMatrixScale(dde_x11_DGL_drom_curcamdat->wcstofcsmat,
		  scale[0], scale[1], scale[2], XdPostConcatenate);

    XdMatrixTranslate(dde_x11_DGL_drom_curcamdat->wcstofcsmat,
		      trans[0], trans[1], trans[2], XdPostConcatenate);

#ifdef DEBUG
    printf("cmp:  wcstofcsmat after map to canonical:\n");
    dor_matrix_print(dde_x11_DGL_drom_curcamdat->wcstofcsmat);
#endif

    /* compute camera type, location and normal */

    XdMatrixLoadIdentity(identmatrix);

    status = XdMatrixSolve(dde_x11_DGL_drom_curcamdat->wcstofcsmat,
			   dde_x11_DGL_drom_curcamdat->npctowcsmat,
			   identmatrix);

    if (status != 10) {
	DDerror (ERR_CANT_INVERT_MATRIX,
		    "ddr_x11_DGL_drom_compute_ctm", DcNullPtr);
	return;
    }

#ifdef DEBUG
    printf("cmp:  wcstofcsmat after solve:\n");
    dor_matrix_print(dde_x11_DGL_drom_curcamdat->wcstofcsmat);
#endif

    XdMatrixLoad(dde_x11_DGL_drom_curcamdat->tspfcstowcsmat,
		 dde_x11_DGL_drom_curcamdat->wcstofcsmat);
    XdMatrixInvert(dde_x11_DGL_drom_curcamdat->tspfcstowcsmat);
    XdMatrixTranspose(dde_x11_DGL_drom_curcamdat->tspfcstowcsmat);

    XdMathHomoTrns(0.,0.,1.,0.,dde_x11_DGL_drom_curcamdat->npctowcsmat,
		   &xtrans,&ytrans,&ztrans,&wtrans);

    /* if wtrans is close to zero, the camera is so far away that
       it doesn't make sense to do a perspective projection, so
       we switch to parallel */
    if (( abs(wtrans) > 1.e-20 ) &&
	(dde_x11_DGL_drom_curcamdat->viewtype == 
	 ddd_x11_DGL_drom_viewtype_perspective)) {
	dde_x11_DGL_drom_curcamdat->camtype = DcPerspective;
	dde_x11_DGL_drom_curcamdat->wcscamloc[0] = xtrans/wtrans;
	dde_x11_DGL_drom_curcamdat->wcscamloc[1] = ytrans/wtrans;
	dde_x11_DGL_drom_curcamdat->wcscamloc[2] = ztrans/wtrans;

#ifdef DEBUG
	printf("positional viewpoint wtrans = %f\n", wtrans);
	printf("eye point = (%f,%f,%f)\n",
	       dde_x11_DGL_drom_curcamdat->wcscamloc[0],
	       dde_x11_DGL_drom_curcamdat->wcscamloc[1],
	       dde_x11_DGL_drom_curcamdat->wcscamloc[2]);
#endif
    } else {
	dde_x11_DGL_drom_curcamdat->camtype = DcParallel;

#ifdef DEBUG
	printf("directional viewpoint wtrans = %f\n", wtrans);
	fflush(stdout);
#endif
    }

    XdMathVecTrns(0.,0.,1.,
		  dde_x11_DGL_drom_curcamdat->npctowcsmat,
		  &trans[0], &trans[1], &trans[2]);
    dde_x11_DGL_drom_curcamdat->wcscamnorm[0] = trans[0];
    dde_x11_DGL_drom_curcamdat->wcscamnorm[1] = trans[1];
    dde_x11_DGL_drom_curcamdat->wcscamnorm[2] = trans[2];

    ddr_x11_DGL_drom_renormalize(dde_x11_DGL_drom_curcamdat->wcscamnorm);

#ifdef DEBUG
    printf("camera normal = (%f,%f,%f)\n",
	   dde_x11_DGL_drom_curcamdat->wcscamnorm[0],
	   dde_x11_DGL_drom_curcamdat->wcscamnorm[1],
	   dde_x11_DGL_drom_curcamdat->wcscamnorm[2]);
#endif
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_initialize_ccstowcsmat()

{
    XdMatrixLoad(dde_x11_DGL_drom_curcamdat->ccstowcsmat,
		 dde_x11_DGL_drom_data.object_lcstowcsmat);
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_set_att_stereo (eyeseparation, distance)
     DtReal eyeseparation;
     DtReal distance;
{
    dde_x11_DGL_drom_data.stereo_eyeseparation = eyeseparation;
    dde_x11_DGL_drom_data.stereo_distance = distance;
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_set_att_stereoswi (switchvalue)
     DtSwitch switchvalue;
{
    dde_x11_DGL_drom_data.stereo_switch = switchvalue;
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

ddr_x11_DGL_drom_set_camera_matrix(matrix)
     DtMatrix4x4 matrix;
{
    DtInt i, j;

    dde_x11_DGL_drom_data.viewtype = ddd_x11_DGL_drom_viewtype_matrix;
	
PRAGMA(IVDEP)
    for (i=0; i<4; i++) {
	for (j=0; j<4; j++) {
	    dde_x11_DGL_drom_data.projmat[i][j] = matrix[i][j];
	}
    }
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_set_parallel_matrix(fov,hither,yon)
     DtReal fov;
     DtReal hither,yon;

{
    XdReal yoverxscalefactor,extentx,extenty;
    DtVolume volume;

    dde_x11_DGL_drom_data.viewtype = ddd_x11_DGL_drom_viewtype_parallel;

    DvInqBoundary(dde_x11_DGL_drom_data.current_view, &volume);

    extentx = volume.fur[0] - volume.bll[0];
    extenty = volume.fur[1] - volume.bll[1];

    yoverxscalefactor = extenty / extentx;

    if ( extentx >= extenty ) {
	XdMatrixScale(dde_x11_DGL_drom_data.projmat,
		      yoverxscalefactor, 1., 1., XdReplace);
    } else {
	XdMatrixScale(dde_x11_DGL_drom_data.projmat,
		      1., 1./yoverxscalefactor, 1., XdReplace);
    }
    XdMatrixParallel(dde_x11_DGL_drom_data.projmat,
		     (XdReal)fov,(XdReal)hither,(XdReal)yon,
		     XdPostConcatenate);
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_set_perspective_matrix(fov,hither,yon)
     DtReal fov;
     DtReal hither,yon;

{
    XdReal yoverxscalefactor,extentx,extenty;
    DtVolume volume;

    dde_x11_DGL_drom_data.viewtype = ddd_x11_DGL_drom_viewtype_perspective;

    DvInqBoundary(dde_x11_DGL_drom_data.current_view, &volume);

    extentx = volume.fur[0] - volume.bll[0];
    extenty = volume.fur[1] - volume.bll[1];

    yoverxscalefactor = extenty / extentx;

    if ( extentx >= extenty ) {
	XdMatrixScale(dde_x11_DGL_drom_data.projmat,
		      yoverxscalefactor, 1., 1., XdReplace);
    } else {
	XdMatrixScale(dde_x11_DGL_drom_data.projmat,
		      1., 1./yoverxscalefactor, 1.,
		      XdReplace);
    }
    XdMatrixPerspective(dde_x11_DGL_drom_data.projmat,
			(XdReal)fov,(XdReal)hither,(XdReal)yon,
			XdPostConcatenate);
}


/*
 ======================================================================
 */

ddr_x11_DGL_drom_set_projection_matrix(window,ptype,prp,view_plane,hither,yon)
     DtArea *window;
     DtProjectionType ptype;
     DtPoint3 prp;
     DtReal view_plane;
     DtReal hither;
     DtReal yon;

{
    XdProjectionType Xptype;

    switch (ptype) {
    case DcParallel:
	dde_x11_DGL_drom_data.viewtype = ddd_x11_DGL_drom_viewtype_parallel;
	Xptype = XdParallel;
	break;

    case DcPerspective:
	dde_x11_DGL_drom_data.viewtype = ddd_x11_DGL_drom_viewtype_perspective;
	Xptype = XdPerspective;
	break;
    }
	
    if (sizeof(DtReal) != sizeof(XdReal)) {
	XdArea Xwindow;
	XdPoint3 Xprp;

	Xwindow.ll[0] = window->ll[0];
	Xwindow.ll[1] = window->ll[1];
	Xwindow.ur[0] = window->ur[0];
	Xwindow.ur[1] = window->ur[1];

	Xprp[0] = prp[0];
	Xprp[1] = prp[1];
	Xprp[2] = prp[2];

	XdMatrixLoadProjection (dde_x11_DGL_drom_data.projmat,&Xwindow,
				Xptype,Xprp,
				(XdReal)view_plane,(XdReal)hither,(XdReal)yon);
    } else {
	XdMatrixLoadProjection (dde_x11_DGL_drom_data.projmat,window,Xptype,prp,
				(XdReal)view_plane,(XdReal)hither,(XdReal)yon);
    }
}
