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
 
/*****************************************************************************
**  This file contains the Dynamic Renderer Output Module functions that
**  govern the camera projection matrices.
*****************************************************************************/

#include "../pex_driver.h"

#define DEBUG 0


/*****************************************************************************
**  The renderer calls this procedure each time the camera object is executed.
**  The current values of the camera attributes are saved.  It is important to
**  save the current local-to-world transformation matrix, which defines the
**  position and direction of the camera by transforming the point (0,0,0) and
**  the vector <0,0,-1>.  Other camera attributes include the camera matrix
**  (parallel, perspective, projection or arbitrary).
*****************************************************************************/

void ddr_pex_drom_camera (void)
{
#   if DEBUG_CALL
	print ("ddr_pex_drom_camera ()\n");
	fflush (stdout);
#   endif

    dor_matrix_load (DROM.M_camera_world, DROM.M_local_world);
}



/*****************************************************************************
**  The dynamic renderer calls this procedure to set the camera projection
**  matrix.  This function is one of several that set the camera projection
**  matrix (which defines an arbitrary 4x4 projection matrix).  Other
**  functions define perspective or parallel camera projection.  Each
**  subsequent call to any of these functions replaces the previous matrix.
**  The matrix in place at the time of the drom.camera call is the one used
**  for the view being rendered.
*****************************************************************************/

void  ddr_pex_drom_set_camera_matrix  (DtMatrix4x4 matrix)
{
#   if DEBUG_CALL
	print  ("ddr_pex_drom_set_camera_matrix\n");
	PrintMatrix (matrix);
	fflush (stdout);
#   endif

    dor_matrix_load (DROM.M_projection, matrix);
    DROM.camera_type = DcCameraArbitrary;
}



/*****************************************************************************
**  This function is one of several that set the camera projection matrix;
**  this one defines a parallel projection.  `window_size' specifies the
**  square size of the field of view.  Note that `hither' and `yon' are
**  negative numbers, and specify the distance to the near and far Z clipping
**  planes.  Other functions define perspective or parallel camera projection.
**  Each subsequent call to any of these functions replaces the previous
**  matrix.  The matrix in place at the time of the drom.camera call is the
**  one used for the view being rendered.
*****************************************************************************/

void  ddr_pex_drom_set_parallel_matrix  (
    DtReal fov,
    DtReal hither,
    DtReal yon)
{
    auto DtReal aspect;		/* Display Window Aspect Ratio */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_parallel_matrix (%f, %f, %f)\n",
	    fov, hither, yon);
	fflush (stdout);
#   endif

    DROM.camera_type = DcCameraParallel;

#if 0
    DROM.pex_view.clipLimits.minval.x = - (DCM->width  / 2);
    DROM.pex_view.clipLimits.minval.y = - (DCM->height / 2);
    DROM.pex_view.clipLimits.maxval.x =   (DCM->width  / 2);
    DROM.pex_view.clipLimits.maxval.y =   (DCM->height / 2);
#endif

    if (DCM->width >= DCM->height)
    {   aspect = (DtReal)(DCM->height) / (DtReal)(DCM->width);
	dor_matrix_load_scale (DROM.M_projection, aspect, 1.0, 1.0);
    }
    else
    {   aspect = (DtReal)(DCM->width) / (DtReal)(DCM->height);
	dor_matrix_load_scale (DROM.M_projection, 1.0, aspect, 1.0);
    }

    dor_matrix_parallel (DROM.M_projection, fov, hither, yon, DcPostConcatenate);
}



/*****************************************************************************
**  This procedure is one of several that set the camera projection matrix.
**  It defines a perspective projection matrix.  The parameters are specified
**  relative to the camera position at the origin, looking down the negative Z
**  axis.  `fov' is the field of view angle for the projection.  Note that
**  `hither' and `yon' are negative numbers, and specify the distance to the
**  near and far Z clipping planes.  Other functions define perspective or
**  parallel camera projection.  Each subsequent call to any of these
**  functions replaces the previous matrix.  The matrix in place at the time
**  of the drom.camera call is the one used for the view being rendered.
*****************************************************************************/

void  ddr_pex_drom_set_perspective_matrix  (
    DtReal fov,
    DtReal hither,
    DtReal yon)
{
    auto DtReal aspect;		/* Display Window Aspect Ratio */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_perspective_matrix (%f, %f, %f)\n",
	    fov, hither, yon);
	fflush (stdout);
#   endif

    DROM.camera_type = DcCameraPerspective;

#if 0
    DROM.pex_view.clipLimits.minval.x = - (DCM->width  / 2);
    DROM.pex_view.clipLimits.minval.y = - (DCM->height / 2);
    DROM.pex_view.clipLimits.maxval.x =   (DCM->width  / 2);
    DROM.pex_view.clipLimits.maxval.y =   (DCM->height / 2);
#endif

    if (DCM->width >= DCM->height)
    {   aspect = (DtReal)(DCM->height) / (DtReal)(DCM->width);
	dor_matrix_load_scale (DROM.M_projection, aspect, 1.0, 1.0);
    }
    else
    {   aspect = (DtReal)(DCM->width) / (DtReal)(DCM->height);
	dor_matrix_load_scale (DROM.M_projection, 1.0, aspect, 1.0);
    }

#   if DEBUG
	printf ("\nWidth %d, Height %d\n", DCM->width, DCM->height);
	printf ("\nPerspective Scaling Matrix:\n");
	PrintMatrix (DROM.M_projection);
#   endif

    dor_matrix_perspective (DROM.M_projection, fov, hither, yon, DcPostConcatenate);

#   if DEBUG
	printf ("\nPerspective After Scaling Matrix:\n");
	PrintMatrix (DROM.M_projection);
#   endif
}



/*****************************************************************************
**  This routine is one of several that set the camera projection.  This
**  particular function defines either a parallel or perspective projection
**  matrix depending on `ptype'.  The parameters are specified relative to the
**  camera positioned at the origin, looking down the negative Z axis.  Other
**  functions define perspective or parallel camera projection.  Each
**  subsequent call to any of these functions replaces the previous matrix.
**  The matrix in place at the time of the drom.camera call is the one used
**  for the view being rendered.
*****************************************************************************/

void  ddr_pex_drom_set_projection_matrix  (
    DtArea           *window,
    DtProjectionType  ptype,
    DtPoint3          prp,
    DtReal            view_plane,
    DtReal            hither,
    DtReal            yon)
{
#   if DEBUG_CALL
	print  ("ddr_pex_drom_set_projection_matrix");
	printf ("%lx, %d, <%f,%f,%f>, %f, %f, %f)\n",
	    window, ptype, prp[0], prp[1], prp[2],
	    view_plane, hither, yon);
	fflush (stdout);
#   endif

    DROM.camera_type =
	(ptype == DcParallel) ? DcCameraParallel : DcCameraPerspective;

    dor_matrix_load_projection (DROM.M_projection, window, ptype, prp,
				view_plane, hither, yon);
}
