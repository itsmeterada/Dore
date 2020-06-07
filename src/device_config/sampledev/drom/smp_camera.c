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
  	void ddr_sampledev_drom_camera ()
	void ddr_sampledev_drom_set_att_stereo (eyeseparation, distance)
	void ddr_sampledev_drom_set_att_stereoswi (switchvalue)
	void ddr_sampledev_drom_set_camera_matrix (matrix)
	void ddr_sampledev_drom_set_parallel_matrix (fov,hither,yon)
	void ddr_sampledev_drom_set_perspective_matrix (fov,hither,yon)
	void ddr_sampledev_drom_set_projection_matrix (window,ptype,prp,view_plane,hither,yon)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
void
ddr_sampledev_drom_camera()
{
    dde_sampledev_drom_curcamdat->stereoswitch = 
	    dde_sampledev_drom_data.stereo_switch;
    dde_sampledev_drom_curcamdat->stereo_eyeseparation =
	    dde_sampledev_drom_data.stereo_eyeseparation;
    dde_sampledev_drom_curcamdat->stereo_distance = 
	    dde_sampledev_drom_data.stereo_distance;

    ddr_sampledev_matrix_concat(dde_sampledev_drom_curcamdat->projmat,
			       dde_sampledev_drom_data.projmat, DcReplace);
    dde_sampledev_drom_curcamdat->camera_type = dde_sampledev_drom_data.camera_type;

    ddr_sampledev_drom_get_lcstowcsmat(dde_sampledev_drom_curcamdat->camtowcsmat);
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_set_att_stereo (eyeseparation, distance)
DtReal eyeseparation;
DtReal distance;
{
    dde_sampledev_drom_data.stereo_eyeseparation = eyeseparation;
    dde_sampledev_drom_data.stereo_distance = distance;
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_set_att_stereoswi (switchvalue)
DtSwitch switchvalue;
{
    dde_sampledev_drom_data.stereo_switch = switchvalue;
}
/*
 ======================================================================
 */
ddr_sampledev_drom_set_camera_matrix(matrix)
DtMatrix4x4 matrix;
{
    dde_sampledev_drom_data.camera_type = DcCameraArbitrary;

    ddr_sampledev_matrix_concat (dde_sampledev_drom_data.projmat, matrix, 
				DcReplace);
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_set_parallel_matrix(fov,hither,yon)
DtReal fov;
DtReal hither,yon;
{
    DtReal yoverxscalefactor,extentx,extenty;
    DtVolume volume;

    dde_sampledev_drom_data.camera_type = DcCameraParallel;

    DvInqBoundary(dde_sampledev_drom_data.view, &volume);

    extentx = volume.fur[0] - volume.bll[0];
    extenty = volume.fur[1] - volume.bll[1];

    yoverxscalefactor = extenty / extentx;

    if ( extentx >= extenty ) {
	ddr_sampledev_matrix_scale(dde_sampledev_drom_data.projmat,
				  yoverxscalefactor, 1., 1., DcReplace);
    } else {
	ddr_sampledev_matrix_scale(dde_sampledev_drom_data.projmat,
				  1., 1./yoverxscalefactor, 1.,
				  DcReplace);
    }

    ddr_sampledev_matrix_parallel(dde_sampledev_drom_data.projmat, 
				 fov, hither, yon, DcPostConcatenate);
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_set_perspective_matrix(fov,hither,yon)
DtReal fov;
DtReal hither,yon;
{
    DtReal yoverxscalefactor,extentx,extenty;
    DtVolume volume;

    dde_sampledev_drom_data.camera_type = DcCameraPerspective;

    DvInqBoundary(dde_sampledev_drom_data.view, &volume);

    extentx = volume.fur[0] - volume.bll[0];
    extenty = volume.fur[1] - volume.bll[1];

    yoverxscalefactor = extenty / extentx;

    if ( extentx >= extenty ) {
	ddr_sampledev_matrix_scale(dde_sampledev_drom_data.projmat,
				  yoverxscalefactor, 1., 1., DcReplace);
    } else {
	ddr_sampledev_matrix_scale(dde_sampledev_drom_data.projmat,
				  1., 1./yoverxscalefactor, 1.,
				  DcReplace);
    }

    ddr_sampledev_matrix_perspective(dde_sampledev_drom_data.projmat, 
				    fov, hither, yon, DcPostConcatenate);
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_set_projection_matrix(window,ptype,prp,view_plane,hither,yon)
DtArea *window;
DtProjectionType ptype;
DtPoint3 prp;
DtReal view_plane;
DtReal hither;
DtReal yon;
{
    switch (ptype) {
    case DcParallel:
	dde_sampledev_drom_data.camera_type = DcCameraParallel;
	break;
    case DcPerspective:
	dde_sampledev_drom_data.camera_type = DcCameraPerspective;
	break;
    }
    ddr_sampledev_matrix_projection(dde_sampledev_drom_data.projmat,
				   window,ptype,prp,view_plane,hither,yon,
				   DcReplace);
}
/*
 ======================================================================
 */
