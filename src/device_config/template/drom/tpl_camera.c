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
  	void ddr_template_drom_camera ()
	void ddr_template_drom_set_att_stereo (eyeseparation, distance)
	void ddr_template_drom_set_att_stereoswi (switchvalue)
	void ddr_template_drom_set_camera_matrix (matrix)
	void ddr_template_drom_set_parallel_matrix (fov,hither,yon)
	void ddr_template_drom_set_perspective_matrix (fov,hither,yon)
	void ddr_template_drom_set_projection_matrix (window,ptype,prp,view_plane,hither,yon)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
void ddr_template_drom_camera (void)
{
    dde_template_drom_curcamdat->stereoswitch = 
	    dde_template_drom_data.stereo_switch;
    dde_template_drom_curcamdat->stereo_eyeseparation =
	    dde_template_drom_data.stereo_eyeseparation;
    dde_template_drom_curcamdat->stereo_distance = 
	    dde_template_drom_data.stereo_distance;

    ddr_template_matrix_concat(dde_template_drom_curcamdat->projmat,
			       dde_template_drom_data.projmat, DcReplace);
    dde_template_drom_curcamdat->camera_type = dde_template_drom_data.camera_type;

    ddr_template_drom_get_lcstowcsmat(dde_template_drom_curcamdat->camtowcsmat);
}
/*
 ======================================================================
 */
void ddr_template_drom_set_att_stereo (
    DtReal eyeseparation,
    DtReal distance)
{
    dde_template_drom_data.stereo_eyeseparation = eyeseparation;
    dde_template_drom_data.stereo_distance = distance;
}
/*
 ======================================================================
 */
void ddr_template_drom_set_att_stereoswi (
    DtSwitch switchvalue)
{
    dde_template_drom_data.stereo_switch = switchvalue;
}
/*
 ======================================================================
 */
int ddr_template_drom_set_camera_matrix (
    DtMatrix4x4 matrix)
{
    dde_template_drom_data.camera_type = DcCameraArbitrary;

    ddr_template_matrix_concat (dde_template_drom_data.projmat, matrix, 
				DcReplace);
}
/*
 ======================================================================
 */
void ddr_template_drom_set_parallel_matrix (
    DtReal fov,
    DtReal hither,
    DtReal yon)
{
    DtReal yoverxscalefactor,extentx,extenty;
    DtVolume volume;

    dde_template_drom_data.camera_type = DcCameraParallel;

    DvInqBoundary(dde_template_drom_data.view, &volume);

    extentx = volume.fur[0] - volume.bll[0];
    extenty = volume.fur[1] - volume.bll[1];

    yoverxscalefactor = extenty / extentx;

    if ( extentx >= extenty ) {
	ddr_template_matrix_scale(dde_template_drom_data.projmat,
				  yoverxscalefactor, 1., 1., DcReplace);
    } else {
	ddr_template_matrix_scale(dde_template_drom_data.projmat,
				  1., 1./yoverxscalefactor, 1.,
				  DcReplace);
    }

    ddr_template_matrix_parallel(dde_template_drom_data.projmat, 
				 fov, hither, yon, DcPostConcatenate);
}
/*
 ======================================================================
 */
void ddr_template_drom_set_perspective_matrix (
    DtReal fov,
    DtReal hither,
    DtReal yon)
{
    DtReal yoverxscalefactor,extentx,extenty;
    DtVolume volume;

    dde_template_drom_data.camera_type = DcCameraPerspective;

    DvInqBoundary(dde_template_drom_data.view, &volume);

    extentx = volume.fur[0] - volume.bll[0];
    extenty = volume.fur[1] - volume.bll[1];

    yoverxscalefactor = extenty / extentx;

    if ( extentx >= extenty ) {
	ddr_template_matrix_scale(dde_template_drom_data.projmat,
				  yoverxscalefactor, 1., 1., DcReplace);
    } else {
	ddr_template_matrix_scale(dde_template_drom_data.projmat,
				  1., 1./yoverxscalefactor, 1.,
				  DcReplace);
    }

    ddr_template_matrix_perspective(dde_template_drom_data.projmat, 
				    fov, hither, yon, DcPostConcatenate);
}
/*
 ======================================================================
 */
void ddr_template_drom_set_projection_matrix (
    DtArea *window,
    DtProjectionType ptype,
    DtPoint3 prp,
    DtReal view_plane,
    DtReal hither,
    DtReal yon)
{
    switch (ptype) {
    case DcParallel:
	dde_template_drom_data.camera_type = DcCameraParallel;
	break;
    case DcPerspective:
	dde_template_drom_data.camera_type = DcCameraPerspective;
	break;
    }
    ddr_template_matrix_projection(dde_template_drom_data.projmat,
				   window,ptype,prp,view_plane,hither,yon,
				   DcReplace);
}
/*
 ======================================================================
 */
