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
	ddr_g4vll_drom_camera
	ddr_g4vll_drom_compute_view_parameters
	ddr_g4vll_drom_set_att_stereo
	ddr_g4vll_drom_set_att_stereoswi
	ddr_g4vll_drom_set_camera_matrix
	ddr_g4vll_drom_set_parallel_matrix
	ddr_g4vll_drom_set_perspective_matrix
	ddr_g4vll_drom_set_projection_matrix
	ddr_g4vll_drom_set_att_smpadapt
	ddr_g4vll_drom_set_att_smpadaptswi
	ddr_g4vll_drom_set_att_smpfilter
	ddr_g4vll_drom_set_att_smpjitter
	ddr_g4vll_drom_set_att_smpjitterswi
	ddr_g4vll_drom_set_att_smpsuper
	ddr_g4vll_drom_set_att_smpsuperswi

  ======================================================================
*/
#include "drom.h"
#include <dore/internal/matrix.h>

/*
 ======================================================================
 */
void ddr_g4vll_drom_camera (void)
{
    int xsize, ysize;
    int xvalue, yvalue;
    int xstep, ystep;
    int xmiddle, ymiddle;
    int x, y;
    int sum;
    int *tptr;
    float fct;

    dde_g4vll_drom_curcamdat->stereoswitch = 
	    dde_g4vll_drom_data.stereo_switch;
    dde_g4vll_drom_curcamdat->stereo_eyeseparation =
	    dde_g4vll_drom_data.stereo_eyeseparation;
    dde_g4vll_drom_curcamdat->stereo_distance = 
	    dde_g4vll_drom_data.stereo_distance;

    dde_g4vll_drom_curcamdat->hither = dde_g4vll_drom_data.hither;
    dde_g4vll_drom_curcamdat->yon = dde_g4vll_drom_data.yon;

    dor_matrix_load(dde_g4vll_drom_curcamdat->projmat,
		    dde_g4vll_drom_data.projmat);
    dde_g4vll_drom_curcamdat->viewtype = dde_g4vll_drom_data.viewtype;

    ddr_g4vll_drom_get_lcstowcsmat(dde_g4vll_drom_curcamdat->camera_to_world_matrix);

    dde_g4vll_drom_curcamdat->super_sample_switch = 
	    dde_g4vll_drom_data.super_sample_switch;

    if (dde_g4vll_drom_curcamdat->super_sample_switch) {

	xsize = dde_g4vll_drom_curcamdat->filter_size[0] = 
		(DtInt) (dde_g4vll_drom_data.samples[0] * 
			 dde_g4vll_drom_data.filter_width[0] + .5);
	ysize = dde_g4vll_drom_curcamdat->filter_size[1] = 
		(DtInt) (dde_g4vll_drom_data.samples[1] *
			 dde_g4vll_drom_data.filter_width[1] + .5);

	if ((xsize % 2) == 1) {
	    xmiddle = (xsize - 1)/2;
	} else {
	    xmiddle = xsize/2 - 1;
	}

	if ((ysize % 2) == 1) {
	    ymiddle = (ysize - 1)/2;
	} else {
	    ymiddle = ysize/2 - 1;
	}

	dde_g4vll_drom_curcamdat->filter_center[0] = xmiddle;
	dde_g4vll_drom_curcamdat->filter_center[1] = ymiddle;

	dde_g4vll_drom_curcamdat->filter_step[0] = 1.0 / 
			(DtReal)dde_g4vll_drom_data.samples[0];
	dde_g4vll_drom_curcamdat->filter_step[1] = 1.0 / 
			(DtReal)dde_g4vll_drom_data.samples[1];

	if (dde_g4vll_drom_curcamdat->filter) {
	    DDspace_Deallocate(dde_g4vll_drom_curcamdat->filter);
	}
	tptr = dde_g4vll_drom_curcamdat->filter = (int *)
		DDspace_Allocate (xsize*ysize*sizeof(int));

	sum = 0;
	yvalue = 0;
	ystep = 1;
	for (y=0; y < ysize; y++) {
	    yvalue += ystep;
	    xvalue = 0;
	    xstep = 1;
	    for (x=0; x < xsize; x++) {
		xvalue += xstep;
		*tptr++ = xvalue*yvalue;
		sum += (xvalue*yvalue);
		if (x == xmiddle) {
		    if ((xsize %2) == 0) xvalue += xstep;
		    xstep *= -1;
		}
	    }
	    if (y == ymiddle) {
		if ((ysize % 2) == 0) yvalue += ystep;
		ystep *= -1;
	    }
	}

	fct = 65535. /(float)sum;

	tptr = dde_g4vll_drom_curcamdat->filter;
	for (y=0; y<ysize; y++) {
	    for (x=0; x<xsize; x++) {
		*tptr++ *= fct;
	    }
	}
    }
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

   The modeling transformations are specified as objects in the
   tree.

   The view mapping matrix can be specified directly for the view
   or using modeling matrices and a camera.

   The view projection transformation can be specified directly
   for the view or can be specified using the perspective
   structure element.  This is then combined with a transformation
   from the cannonical clipping volume to the projection
   viewport.

   The workstation transformation is specified by calls to set the
   workstation window and workstation viewport.  */
void ddr_g4vll_drom_compute_view_parameters (void)
{
    DtMatrix4x4 identmatrix;
    int status;
    DtReal camloc[4];
    DtReal camnorm[3];
    DtVolume *clipvolume;
    DtVolume viewboundary;
    DtReal xres, yres;
    DtReal scale[3];
    DtReal trans[3];

    /* map from WC to VRC */

    dor_matrix_load(dde_g4vll_drom_curcamdat->world_to_frustum_matrix,
		    dde_g4vll_drom_curcamdat->world_to_camera_matrix);

#ifdef DEBUG_2
    printf("cmp:  world to camera matrix:  \n");
    dor_matrix_print(dde_g4vll_drom_curcamdat->world_to_camera_matrix);
#endif

    /* map from VRC to canonical volume for single view */

    dor_matrix_post_concatenate(dde_g4vll_drom_curcamdat->world_to_frustum_matrix,
				dde_g4vll_drom_curcamdat->projmat);

#ifdef DEBUG_2
    printf("cmp:  world to frustum matrix after projmat pre-concatenate:  \n");
    dor_matrix_print(dde_g4vll_drom_curcamdat->world_to_frustum_matrix);
#endif


    /* map from canonical volume to projection viewport in NPC */

    if (dde_g4vll_drom_curcamdat->viewtype != DcCameraArbitrary) {
	DvInqBoundary(dde_g4vll_drom_data.current_view, &viewboundary);

	scale[0] = (viewboundary.fur[0]-viewboundary.bll[0])/2.;
	scale[1] = (viewboundary.fur[1]-viewboundary.bll[1])/2.;
	scale[2] = (viewboundary.fur[2]-viewboundary.bll[2]);

        dde_g4vll_drom_curcamdat->stereo_view_scale = scale[0];

	trans[0] = (viewboundary.fur[0]+viewboundary.bll[0])/2.;
	trans[1] = (viewboundary.fur[1]+viewboundary.bll[1])/2.;
	trans[2] = viewboundary.fur[2];

	/*  If this is a stereo view, correct for the .5 x and y scaling
	    caused by the aspect ratio mismatch from the view subsystem.  */

	ddr_g4vll_dcm_inquire_resolution (&xres,&yres);
	if (yres != 0.0) {
	    scale[1] *= (xres / yres);
	}

	dor_matrix_scale(dde_g4vll_drom_curcamdat->world_to_frustum_matrix,
			 scale[0], scale[1], scale[2], DcPostConcatenate);

	dor_matrix_translate(dde_g4vll_drom_curcamdat->world_to_frustum_matrix,
			     trans[0], trans[1], trans[2], DcPostConcatenate);
    }

    /* map from clipping region in NPC to canonical clipping volume */

    clipvolume = DDdevice_InqClippingVolume(dde_g4vll_drom_data.current_device);

    scale[0] = 2./(clipvolume->fur[0]-clipvolume->bll[0]);
    scale[1] = 2./(clipvolume->fur[1]-clipvolume->bll[1]);
    scale[2] = 1./(clipvolume->fur[2]-clipvolume->bll[2]);

    trans[0] = -scale[0]*clipvolume->bll[0]-1;
    trans[1] = -scale[1]*clipvolume->bll[1]-1;
    trans[2] = -scale[2]*clipvolume->fur[2];

    dde_g4vll_drom_curcamdat->stereo_view_scale = scale[0] *
		dde_g4vll_drom_curcamdat->stereo_view_scale;

    dor_matrix_scale(dde_g4vll_drom_curcamdat->world_to_frustum_matrix,
		     scale[0], scale[1], scale[2], DcPostConcatenate);

    dor_matrix_translate(dde_g4vll_drom_curcamdat->world_to_frustum_matrix,
			 trans[0], trans[1], trans[2], DcPostConcatenate);

    /* compute camera type, location and normal */

    dor_matrix_load_identity(identmatrix);

    status = dor_matrix_solve(dde_g4vll_drom_curcamdat->world_to_frustum_matrix,
			      dde_g4vll_drom_curcamdat->npctowcsmat,
			      identmatrix);

    if (status != 10) {
	DDerror (ERR_CANT_INVERT_MATRIX,
		    "ddr_g4vll_drom_compute_view_parameters", DcNullPtr);
	return;
    }

#ifdef DEBUG_2
    printf("cmp:  npctowcsmat:\n");
    dor_matrix_print(dde_g4vll_drom_curcamdat->npctowcsmat);
#endif

    dor_math_homo_trns(0.,0.,1.,0.,
		       dde_g4vll_drom_curcamdat->npctowcsmat,
		       &camloc[0],&camloc[1],&camloc[2],&camloc[3]);

    dde_g4vll_drom_curcamdat->camloc[0] = camloc[0];
    dde_g4vll_drom_curcamdat->camloc[1] = camloc[1];
    dde_g4vll_drom_curcamdat->camloc[2] = camloc[2];
    dde_g4vll_drom_curcamdat->camloc[3] = camloc[3];

#ifdef DEBUG_2
    printf("homogenous world camera location = %g %g %g %g\n",
	   dde_g4vll_drom_curcamdat->camloc[0],
	   dde_g4vll_drom_curcamdat->camloc[1],
	   dde_g4vll_drom_curcamdat->camloc[2],
	   dde_g4vll_drom_curcamdat->camloc[3]);
#endif

    /*
     * If camloc[3] is close to zero, the camera is so far away
     * that it doesn't make sense to do a perspective projection,
     * so we switch to parallel.
     */
    if (( abs(camloc[3]) > 1.e-20 ) &&
	(dde_g4vll_drom_curcamdat->viewtype == DcCameraPerspective)) {
	dde_g4vll_drom_curcamdat->camtype = DcPerspective;
	dde_g4vll_drom_curcamdat->wcscamloc[0] = camloc[0]/camloc[3];
	dde_g4vll_drom_curcamdat->wcscamloc[1] = camloc[1]/camloc[3];
	dde_g4vll_drom_curcamdat->wcscamloc[2] = camloc[2]/camloc[3];
    } else {
	dde_g4vll_drom_curcamdat->camtype = DcParallel;
	dor_math_renormalize_fast (camloc);
	dde_g4vll_drom_curcamdat->wcscamloc[0] = camloc[0];
	dde_g4vll_drom_curcamdat->wcscamloc[1] = camloc[1];
	dde_g4vll_drom_curcamdat->wcscamloc[2] = camloc[2];
    }

#ifdef DEBUG_2
    printf("world camera location = %g %g %g\n",
	   dde_g4vll_drom_curcamdat->wcscamloc[0],
	   dde_g4vll_drom_curcamdat->wcscamloc[1],
	   dde_g4vll_drom_curcamdat->wcscamloc[2]);
#endif   

    dor_math_vectrns
    (   0.0, 0.0, 1.0,
	dde_g4vll_drom_curcamdat->npctowcsmat,
	&camnorm[0], &camnorm[1], &camnorm[2]
    );
    dor_math_renormalize_fast (camnorm);

    dde_g4vll_drom_curcamdat->wcscamnorm[0] = camnorm[0];
    dde_g4vll_drom_curcamdat->wcscamnorm[1] = camnorm[1];
    dde_g4vll_drom_curcamdat->wcscamnorm[2] = camnorm[2];

#ifdef DEBUG_2
    printf("camera normal = (%f,%f,%f)\n",
	   dde_g4vll_drom_curcamdat->wcscamnorm[0],
	   dde_g4vll_drom_curcamdat->wcscamnorm[1],
	   dde_g4vll_drom_curcamdat->wcscamnorm[2]);
#endif
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_stereo (
    DtReal eyeseparation,
    DtReal distance)
{
    dde_g4vll_drom_data.stereo_eyeseparation = eyeseparation;
    dde_g4vll_drom_data.stereo_distance = distance;
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_stereoswi (
    DtSwitch switchvalue)
{
    dde_g4vll_drom_data.stereo_switch = switchvalue;
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_camera_matrix (
    DtMatrix4x4 matrix)
{
    dde_g4vll_drom_data.viewtype = DcCameraArbitrary;

    dor_matrix_load (dde_g4vll_drom_data.projmat, matrix);
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_parallel_matrix (
    DtReal fov,
    DtReal hither,
    DtReal yon)
{
    DtReal yoverxscalefactor,extentx,extenty;
    DtVolume volume;

    dde_g4vll_drom_data.viewtype = DcCameraParallel;
    dde_g4vll_drom_data.hither = hither;
    dde_g4vll_drom_data.yon = yon;

    DvInqBoundary(dde_g4vll_drom_data.current_view, &volume);

    extentx = volume.fur[0] - volume.bll[0];
    extenty = volume.fur[1] - volume.bll[1];

    yoverxscalefactor = extenty / extentx;

    if ( extentx >= extenty ) {
	dor_matrix_scale(dde_g4vll_drom_data.projmat,
			 yoverxscalefactor, 1., 1., DcReplace);
    } else {
	dor_matrix_scale(dde_g4vll_drom_data.projmat,
			 1., 1./yoverxscalefactor, 1.,
			 DcReplace);
    }

    dor_matrix_parallel(dde_g4vll_drom_data.projmat, 
			fov, hither, yon, DcPostConcatenate);
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_perspective_matrix (
    DtReal fov,
    DtReal hither,
    DtReal yon)
{
    DtReal yoverxscalefactor,extentx,extenty;
    DtVolume volume;

    dde_g4vll_drom_data.viewtype = DcCameraPerspective;
    dde_g4vll_drom_data.hither = hither;
    dde_g4vll_drom_data.yon = yon;

    DvInqBoundary(dde_g4vll_drom_data.current_view, &volume);

    extentx = volume.fur[0] - volume.bll[0];
    extenty = volume.fur[1] - volume.bll[1];

    yoverxscalefactor = extenty / extentx;

    if ( extentx >= extenty ) {
	dor_matrix_scale(dde_g4vll_drom_data.projmat,
			 yoverxscalefactor, 1., 1., DcReplace);
    } else {
	dor_matrix_scale(dde_g4vll_drom_data.projmat,
			 1., 1./yoverxscalefactor, 1.,
			 DcReplace);
    }


    dor_matrix_perspective(dde_g4vll_drom_data.projmat, 
			   fov, hither, yon, DcPostConcatenate);
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_projection_matrix (
    DtArea *window,
    DtProjectionType ptype,
    DtPoint3 prp,
    DtReal view_plane,
    DtReal hither,
    DtReal yon)
{
    dde_g4vll_drom_data.hither = hither;
    dde_g4vll_drom_data.yon = yon;

    switch (ptype) {
    case DcParallel:
	dde_g4vll_drom_data.viewtype = DcCameraParallel;
	break;
    case DcPerspective:
	dde_g4vll_drom_data.viewtype = DcCameraPerspective;
	break;
    }
    dor_matrix_load_projection(dde_g4vll_drom_data.projmat,
			       window,ptype,prp,view_plane,hither,yon);
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_smpadapt (
    DtReal variance)
{
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_smpadaptswi (
    DtSwitch switchvalue)
{
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_smpfilter (
    DtObject callbackobj,
    DtReal xwidth,
    DtReal ywidth)
{
    dde_g4vll_drom_data.filter = callbackobj;
    dde_g4vll_drom_data.filter_width[0] = xwidth;
    dde_g4vll_drom_data.filter_width[1] = ywidth;
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_smpjitter (
    DtReal factor)
{
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_smpjitterswi (
    DtSwitch switchvalue)
{
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_smpsuper (
    DtInt xsamples,
    DtInt ysamples)
{
    dde_g4vll_drom_data.samples[0] = xsamples;
    dde_g4vll_drom_data.samples[1] = ysamples;
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_smpsuperswi (
    DtSwitch switchvalue)
{
    if (dde_g4vll_drom_curdevdat->features->filter) {

    	dde_g4vll_drom_data.super_sample_switch = switchvalue;
    }
}
/*
 ======================================================================
 */
