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
	int ddr_x11_XFDI_drom_camera ()
	int ddr_x11_XFDI_drom_compute_view_parameters ()
	ddr_x11_XFDI_drom_set_att_stereo ddr_x11_XFDI_drom_set_att_stereo (eyeseparation, distance)
	ddr_x11_XFDI_drom_set_att_stereoswi ddr_x11_XFDI_drom_set_att_stereoswi (switchvalue)
	int ddr_x11_XFDI_drom_set_camera_matrix (matrix)
	int ddr_x11_XFDI_drom_set_parallel_matrix (fov,hither,yon)
	int ddr_x11_XFDI_drom_set_perspective_matrix (fov,hither,yon)
	int ddr_x11_XFDI_drom_set_projection_matrix (window,ptype,prp,view_plane,hither,yon)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_camera()
{
    DtMatrix4x4 lcstowcsmat;

    dde_x11_XFDI_drom_curcamdat->stereoswitch = 
	    dde_x11_XFDI_drom_data.stereo_switch;
    dde_x11_XFDI_drom_curcamdat->stereo_eyeseparation =
	    dde_x11_XFDI_drom_data.stereo_eyeseparation;
    dde_x11_XFDI_drom_curcamdat->stereo_distance = 
	    dde_x11_XFDI_drom_data.stereo_distance;

    dor_matrix_load(
		    dde_x11_XFDI_drom_curcamdat->projmat,
		    dde_x11_XFDI_drom_data.projmat);
    dde_x11_XFDI_drom_curcamdat->viewtype = dde_x11_XFDI_drom_data.viewtype;

    ddr_x11_XFDI_drom_get_lcstowcsmat
	    (dde_x11_XFDI_drom_curcamdat->camera_to_world_matrix);
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

ddr_x11_XFDI_drom_compute_view_parameters()
{
    DtReal xscale, yscale, zscale;
    DtMatrix4x4 identmatrix;
    int status;
    int dor_matrix_solve();
    DtReal camloc[4];
    DtVolume *viewboundary;
    DtReal xres, yres;

    /* map from WC to VRC */

    dor_matrix_load(dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix,
		    dde_x11_XFDI_drom_curcamdat->world_to_camera_matrix);

#ifdef DEBUG
    printf("cmp:  world to camera matrix:  \n");
    dor_matrix_print(dde_x11_XFDI_drom_curcamdat->world_to_camera_matrix);
#endif

    /* map from VRC to canonical volume for single view */

    dor_matrix_post_concatenate(dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix,
				dde_x11_XFDI_drom_curcamdat->projmat);

#ifdef DEBUG
    printf("cmp:  world to frustum matrix after projmat pre-concatenate:  \n");
    dor_matrix_print(dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix);
#endif

    dor_matrix_scale(dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix,
		     1.,1.,2.,DcPostConcatenate);
    dor_matrix_translate(dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix,
			 0.,0.,1., DcPostConcatenate);

    /* compute camera type, location and normal */

    dor_matrix_load_identity(identmatrix);

    status = dor_matrix_solve(dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix,
			      dde_x11_XFDI_drom_curcamdat->npctowcsmat,
			      identmatrix);

    if (status != 10) {
	DDerror (ERR_CANT_INVERT_MATRIX,
		    "ddr_x11_XFDI_drom_compute_ctm", DcNullPtr);
	return;
    }

#ifdef DEBUG
    printf("cmp:  npctowcsmat:\n");
    dor_matrix_print(dde_x11_XFDI_drom_curcamdat->npctowcsmat);
#endif

    dor_math_homo_trns(0.,0.,1.,0.,
		       dde_x11_XFDI_drom_curcamdat->npctowcsmat,
		       &camloc[0],&camloc[1],&camloc[2],&camloc[3]);

    dde_x11_XFDI_drom_curcamdat->camloc[0] = camloc[0];
    dde_x11_XFDI_drom_curcamdat->camloc[1] = camloc[1];
    dde_x11_XFDI_drom_curcamdat->camloc[2] = camloc[2];
    dde_x11_XFDI_drom_curcamdat->camloc[3] = camloc[3];

#ifdef DEBUG
    printf("homogenous world camera location = %g %g %g %g\n",
	   dde_x11_XFDI_drom_curcamdat->camloc[0],
	   dde_x11_XFDI_drom_curcamdat->camloc[1],
	   dde_x11_XFDI_drom_curcamdat->camloc[2],
	   dde_x11_XFDI_drom_curcamdat->camloc[3]);
#endif

    /* if camloc[3] is close to zero, the camera is so far away that
       it doesn't make sense to do a perspective projection, so
       we switch to parallel */
    if (( abs(camloc[3]) > 1.e-20 ) &&
	(dde_x11_XFDI_drom_curcamdat->viewtype == DcCameraPerspective)) {
	dde_x11_XFDI_drom_curcamdat->camtype = DcPerspective;
	dde_x11_XFDI_drom_curcamdat->wcscamloc[0] = camloc[0]/camloc[3];
	dde_x11_XFDI_drom_curcamdat->wcscamloc[1] = camloc[1]/camloc[3];
	dde_x11_XFDI_drom_curcamdat->wcscamloc[2] = camloc[2]/camloc[3];
    } else {
	dde_x11_XFDI_drom_curcamdat->camtype = DcParallel;
	dde_x11_XFDI_drom_curcamdat->wcscamloc[0] = camloc[0];
	dde_x11_XFDI_drom_curcamdat->wcscamloc[1] = camloc[1];
	dde_x11_XFDI_drom_curcamdat->wcscamloc[2] = camloc[2];
	dor_math_fast_renormalize(dde_x11_XFDI_drom_curcamdat->wcscamloc);
    }

#ifdef DEBUG
    printf("world camera location = %g %g %g\n",
	   dde_x11_XFDI_drom_curcamdat->wcscamloc[0],
	   dde_x11_XFDI_drom_curcamdat->wcscamloc[1],
	   dde_x11_XFDI_drom_curcamdat->wcscamloc[2]);
#endif   

    dor_math_vectrns(0.,0.,1.,
		     dde_x11_XFDI_drom_curcamdat->npctowcsmat,
		     &dde_x11_XFDI_drom_curcamdat->wcscamnorm[0],
		     &dde_x11_XFDI_drom_curcamdat->wcscamnorm[1],
		     &dde_x11_XFDI_drom_curcamdat->wcscamnorm[2]);

    dor_math_renormalize_fast(dde_x11_XFDI_drom_curcamdat->wcscamnorm);

#ifdef DEBUG
    printf("camera normal = (%f,%f,%f)\n",
	   dde_x11_XFDI_drom_curcamdat->wcscamnorm[0],
	   dde_x11_XFDI_drom_curcamdat->wcscamnorm[1],
	   dde_x11_XFDI_drom_curcamdat->wcscamnorm[2]);
#endif
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_stereo (eyeseparation, distance)
     DtReal eyeseparation;
     DtReal distance;
{
    dde_x11_XFDI_drom_data.stereo_eyeseparation = eyeseparation;
    dde_x11_XFDI_drom_data.stereo_distance = distance;
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_stereoswi (switchvalue)
     DtSwitch switchvalue;
{
    dde_x11_XFDI_drom_data.stereo_switch = switchvalue;
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_camera_matrix(matrix)
     DtMatrix4x4 matrix;
{
    dde_x11_XFDI_drom_data.viewtype = DcCameraArbitrary;

    dor_matrix_load (dde_x11_XFDI_drom_data.projmat, matrix);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_parallel_matrix(fov,hither,yon)
     DtReal fov;
     DtReal hither,yon;
{
    DtReal yoverxscalefactor,extentx,extenty;
    DtVolume volume;

    dde_x11_XFDI_drom_data.viewtype = DcCameraParallel;

    DvInqBoundary(dde_x11_XFDI_drom_data.current_view, &volume);

    extentx = volume.fur[0] - volume.bll[0];
    extenty = volume.fur[1] - volume.bll[1];

    yoverxscalefactor = extenty / extentx;

    if ( extentx >= extenty ) {
	dor_matrix_scale(dde_x11_XFDI_drom_data.projmat,
			 yoverxscalefactor, 1., 1., DcReplace);
    } else {
	dor_matrix_scale(dde_x11_XFDI_drom_data.projmat,
			 1., 1./yoverxscalefactor, 1.,
			 DcReplace);
    }

    dor_matrix_parallel(dde_x11_XFDI_drom_data.projmat, 
			fov, hither, yon, DcPostConcatenate);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_perspective_matrix(fov,hither,yon)
     DtReal fov;
     DtReal hither,yon;
{
    DtReal yoverxscalefactor,extentx,extenty;
    DtVolume volume;

    dde_x11_XFDI_drom_data.viewtype = DcCameraPerspective;

    DvInqBoundary(dde_x11_XFDI_drom_data.current_view, &volume);

    extentx = volume.fur[0] - volume.bll[0];
    extenty = volume.fur[1] - volume.bll[1];

    yoverxscalefactor = extenty / extentx;

    if ( extentx >= extenty ) {
	dor_matrix_scale(dde_x11_XFDI_drom_data.projmat,
			 yoverxscalefactor, 1., 1., DcReplace);
    } else {
	dor_matrix_scale(dde_x11_XFDI_drom_data.projmat,
			 1., 1./yoverxscalefactor, 1.,
			 DcReplace);
    }


    dor_matrix_perspective(dde_x11_XFDI_drom_data.projmat, 
			   fov, hither, yon, DcPostConcatenate);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_projection_matrix(window,ptype,prp,view_plane,hither,yon)
     DtArea *window;
     DtProjectionType ptype;
     DtPoint3 prp;
     DtReal view_plane;
     DtReal hither;
     DtReal yon;
{
    switch (ptype) {
    case DcParallel:
	dde_x11_XFDI_drom_data.viewtype = DcCameraParallel;
	break;
    case DcPerspective:
	dde_x11_XFDI_drom_data.viewtype = DcCameraPerspective;
	break;
    }
    dor_matrix_load_projection(dde_x11_XFDI_drom_data.projmat,
			       window,ptype,prp,view_plane,hither,yon);
}
