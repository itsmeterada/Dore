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
	ddr_gl_drom_camera
	ddr_gl_drom_set_att_stereo
	ddr_gl_drom_set_att_stereoswi
	ddr_gl_drom_set_camera_matrix
	ddr_gl_drom_set_parallel_matrix
	ddr_gl_drom_set_perspective_matrix
	ddr_gl_drom_set_projection_matrix

  ======================================================================
 */
#include "../gl_driver.h"
#include <dore/internal/matrix.h>



/* ======================================================================== */

void ddr_gl_drom_camera (void)
{
    DtMatrix4x4 dm;

#   ifdef DEBUGP
	printf ("Initialiing a camera...\n");
#   endif
    mmode (MPROJECTION);
    getmatrix (DevStatus.view_data->projection_matrix);
    /*
    printf ("Projection ");
    ddr_print_current_matrix ();
    */

    mmode (MVIEWING);
    getmatrix (DevStatus.view_data->cam_to_world_matrix);
    ddr_iris_to_dore_matrix (DevStatus.view_data->cam_to_world_matrix, dm);
    dor_matrix_invert (dm);
    ddr_dore_to_iris_matrix (dm, DevStatus.view_data->world_to_cam_matrix);

    /*
    printf ("Location ");
    ddr_print_current_matrix ();
    */
}



/* ======================================================================== */

void ddr_gl_drom_set_att_stereo (
    DtReal eyeseparation,
    DtReal distance)
{
}



/* ======================================================================== */

void ddr_gl_drom_set_att_stereoswi (DtSwitch switchvalue)
{
}



/* ======================================================================== */

void ddr_gl_drom_set_camera_matrix (DtMatrix4x4 dm)
{
    Matrix im;

#   ifdef DEBUGP
	printf ("Set camera matrix called\n");
#   endif

    ddr_dore_to_iris_matrix(dm,im);

    mmode(MPROJECTION);
    loadmatrix(im);
    mmode(MVIEWING);
}



/* ======================================================================== */

void ddr_gl_drom_set_parallel_matrix (
    DtReal winsize,
    DtReal hither,
    DtReal yon)
{
    DtVolume volume;
    float extentx, extenty;

#   ifdef DEBUGP
	printf("Set_parallel_matrix() called\n");
#   endif

    winsize *= 0.5;

    mmode (MPROJECTION);
    ortho (-winsize,winsize,-winsize,winsize,-hither,-yon);

    DvInqBoundary (DevStatus.dore_view, &volume);
    extentx = volume.fur[0] - volume.bll[0];
    extenty = volume.fur[1] - volume.bll[1];

    if (extentx >= extenty) 
	scale (extenty/extentx, 1., 1.);
    else 
	scale (1., extentx/extenty, 1.);
    mmode (MVIEWING);
}



/* ======================================================================== */

#define DegreesToAngle(deg) ((Angle)((deg)*10.))

void ddr_gl_drom_set_perspective_matrix (
    DtReal fov,
    DtReal hither,
    DtReal yon)
{
    DtVolume volume;
    float extentx, extenty;

#   ifdef DEBUGP
	printf ("Set_perspective_matrix() called.\n");
#   endif

    mmode (MPROJECTION);
    perspective (DegreesToAngle(fov),1.,-hither,-yon);

    DvInqBoundary (DevStatus.dore_view, &volume);
    extentx = volume.fur[0] - volume.bll[0];
    extenty = volume.fur[1] - volume.bll[1];

    if (extentx >= extenty) 
	scale (extenty/extentx, 1., 1.);
    else 
	scale (1., extentx/extenty, 1.);
    mmode (MVIEWING);
}



/* ======================================================================== */

void ddr_gl_drom_set_projection_matrix (
    DtArea *window,
    DtProjectionType ptype,
    DtPoint3 prp,
    DtReal view_plane,
    DtReal hither,
    DtReal yon)
{
    Matrix im;
    DtMatrix4x4 dm;

    dor_matrix_load_projection (dm,window,ptype,prp,view_plane,hither,yon);
    ddr_dore_to_iris_matrix (dm,im);
    mmode (MPROJECTION);
    loadmatrix (im);
    mmode (MVIEWING);
}
