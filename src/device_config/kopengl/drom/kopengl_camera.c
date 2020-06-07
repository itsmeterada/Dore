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

/**************************************************************************

 Functions:
   ddr_kopengl_drom_camera 
   ddr_kopengl_drom_set_att_stereo 
   ddr_kopengl_drom_set_att_stereoswi 
   ddr_kopengl_drom_set_camera_matrix 
   ddr_kopengl_drom_set_parallel_matrix 
   ddr_kopengl_drom_set_perspective_matrix 
   ddr_kopengl_drom_set_projection_matrix 

 **************************************************************************/

#include "dore/internal/matrix.h"
#include "drom.h"

/*****************************************************************************
 *  The renderer calls this procedure each time the camera object is executed.
 *  The current values of the camera attributes are saved.  It is important to
 *  save the current local-to-world transformation matrix, which defines the
 *  position and direction of the camera by transforming the point (0,0,0) and
 *  the vector <0,0,-1>.  Other camera attributes include the camera matrix
 *  (parallel, perspective, projection or arbitrary).
 ****************************************************************************/

void 
ddr_kopengl_drom_camera (void)
{
 DtMatrix4x4 temp;

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.camera\n");
#endif

 CAMERA->stereoswitch         = DROM.stereo_switch;
 CAMERA->stereo_eyeseparation = DROM.stereo_eyeseparation;
 CAMERA->stereo_distance      = DROM.stereo_distance;
 CAMERA->camera_type          = DROM.camera_type;

 /* Set the projection matrix in the current drom device data */
 GET_GL_MATRIX (GL_PROJECTION_MATRIX, CAMERA->projection_matrix);

 /* Set the modelview matrix in the current drom device data */
 GET_GL_MATRIX (GL_MODELVIEW_MATRIX, CAMERA->world_to_cam_matrix);
 CONVERT_OPENGL_TO_DORE (temp, CAMERA->world_to_cam_matrix);
 dor_matrix_invert (temp);
 CONVERT_DORE_TO_OPENGL (CAMERA->cam_to_world_matrix, temp);

 glMatrixMode (GL_MODELVIEW);
}

/*****************************************************************************
 ****************************************************************************/

void 
ddr_kopengl_drom_set_att_stereo (DtReal eyeseparation, DtReal distance)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_stereo\n");
#endif

 DROM.stereo_eyeseparation = eyeseparation;
 DROM.stereo_distance      = distance;

 return;
}


/*****************************************************************************
 ****************************************************************************/

void 
ddr_kopengl_drom_set_att_stereoswi (DtSwitch switchvalue)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_stereoswi\n");
#endif

 DROM.stereo_switch = switchvalue;

 return;
}


/*****************************************************************************
 *  The dynamic renderer calls this procedure to set the camera projection
 *  matrix.  This function is one of several that set the camera projection
 *  matrix (which defines an arbitrary 4x4 projection matrix).  Other
 *  functions define perspective or parallel camera projection.  Each
 *  subsequent call to any of these functions replaces the previous matrix.
 *  The matrix in place at the time of the drom.camera call is the one used
 *  for the view being rendered.
 ****************************************************************************/

void 
ddr_kopengl_drom_set_camera_matrix (DtMatrix4x4 matrix)
{
 glReal   cam_mat[16];

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_camera_matrix\n");
#endif

 DROM.camera_type = DcCameraArbitrary;

 CONVERT_DORE_TO_OPENGL (cam_mat, matrix);
 glMatrixMode (GL_PROJECTION);
 LOAD_GL_MATRIX (cam_mat);
 glMatrixMode (GL_MODELVIEW);
}


/*****************************************************************************
 *  This function is one of several that set the camera projection matrix;
 *  this one defines a parallel projection.  `window_size' specifies the
 *  square size of the field of view.  Note that `hither' and `yon' are
 *  negative numbers, and specify the distance to the near and far Z clipping
 *  planes.  Other functions define perspective or parallel camera projection.
 *  Each subsequent call to any of these functions replaces the previous
 *  matrix.  The matrix in place at the time of the drom.camera call is the
 *  one used for the view being rendered.
 ****************************************************************************/

void 
ddr_kopengl_drom_set_parallel_matrix (DtReal winsize,
				      DtReal hither,
				      DtReal yon)
{
 DtReal          extentx, extenty;
 DtVolume        volume;

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_parallel_matrix\n");
#endif

 DROM.camera_type = DcCameraParallel;

 winsize *= 0.5;
 /* replace previous projection matrix */
 glMatrixMode (GL_PROJECTION);
 glLoadIdentity ();
 glOrtho (-winsize, winsize, -winsize, winsize, -hither,-yon);

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "glOrtho (%f, %f, %f, %f, %f, %f)\n", 
                   -winsize, winsize, -winsize, winsize, -hither, -yon); 
 {
  glReal tmp[16];

  GET_GL_MATRIX (GL_PROJECTION_MATRIX, tmp);
  ddr_kopengl_print_opengl_matrix ("After set_parallel_matrix", tmp);
 }
#endif

 /* Get the boundary */
 DvInqBoundary (DROM.view, &volume);
 extentx = volume.fur[0] - volume.bll[0];
 extenty = volume.fur[1] - volume.bll[1];

 if (extentx >= extenty)
   SET_GL_SCALE (extenty/extentx, 1.0, 1.0);
 else
   SET_GL_SCALE (1.0, extentx/extenty, 1.0);

 glMatrixMode (GL_MODELVIEW);
}




/*****************************************************************************
 *  This procedure is one of several that set the camera projection matrix.
 *  It defines a perspective projection matrix.  The parameters are specified
 *  relative to the camera position at the origin, looking down the negative Z
 *  axis.  `fov' is the field of view angle for the projection.  Note that
 *  `hither' and `yon' are negative numbers, and specify the distance to the
 *  near and far Z clipping planes.  Other functions define perspective or
 *  parallel camera projection.  Each subsequent call to any of these
 *  functions replaces the previous matrix.  The matrix in place at the time
 *  of the drom.camera call is the one used for the view being rendered.
 ****************************************************************************/

void 
ddr_kopengl_drom_set_perspective_matrix (DtReal fov,
					 DtReal hither,
					 DtReal yon)
{
 DtReal          extentx, extenty;
 DtVolume        volume;

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_perspective_matrix\n");
#endif

 DROM.camera_type = DcCameraPerspective;

 /* replace previous projection matrix */
 glMatrixMode (GL_PROJECTION);
 glLoadIdentity ();
 gluPerspective (fov, 1.0, -hither, -yon); 

#ifdef _DEBUG_KOPENGL_
 {
  glReal tmp[16];

  GET_GL_MATRIX (GL_PROJECTION_MATRIX, tmp);
  ddr_kopengl_print_opengl_matrix ("After set_perspective_matrix", tmp);
 }
#endif

 DvInqBoundary (DROM.view, &volume);
 extentx = volume.fur[0] - volume.bll[0];
 extenty = volume.fur[1] - volume.bll[1];

 if (extentx >= extenty)
   SET_GL_SCALE (extenty/extentx, 1.0, 1.0);
 else
   SET_GL_SCALE (1.0, extentx/extenty, 1.0);

 glMatrixMode (GL_MODELVIEW);
}



/*****************************************************************************
 *  This routine is one of several that set the camera projection.  This
 *  particular function defines either a parallel or perspective projection
 *  matrix depending on `ptype'.  The parameters are specified relative to the
 *  camera positioned at the origin, looking down the negative Z axis.  Other
 *  functions define perspective or parallel camera projection.  Each
 *  subsequent call to any of these functions replaces the previous matrix.
 *  The matrix in place at the time of the drom.camera call is the one used
 *  for the view being rendered.
 ****************************************************************************/

void 
ddr_kopengl_drom_set_projection_matrix (DtArea           *window,
					DtProjectionType  ptype,
					DtPoint3          prp,
					DtReal            view_plane,
					DtReal            hither,
					DtReal            yon)
{
 DtMatrix4x4 proj_mat;
 glReal      gl_proj_mat[16];
 
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_projection_matrix\n");
#endif

 /* Set the projection type */
 DROM.camera_type =
        (ptype == DcParallel) ? DcCameraParallel : DcCameraPerspective;

 /* Calculate projection matrix */
 ddr_kopengl_matrix_projection (proj_mat, window, ptype, prp, view_plane, 
                                hither, yon, DcReplace);
 
 /* Convert it to OpenGL matrix order */
 CONVERT_DORE_TO_OPENGL (gl_proj_mat, proj_mat);

 /* Load projection matrix */
 glMatrixMode (GL_PROJECTION);
 LOAD_GL_MATRIX (gl_proj_mat);

 /* Reset matrix mode */
 glMatrixMode (GL_MODELVIEW);
}
