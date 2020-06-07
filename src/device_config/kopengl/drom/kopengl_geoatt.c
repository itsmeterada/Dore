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
   ddr_kopengl_drom_get_lcstowcsmat 
   ddr_kopengl_drom_update_lcstowcsmat_lokatfrm
   ddr_kopengl_drom_update_lcstowcsmat_rotate 
   ddr_kopengl_drom_update_lcstowcsmat_scale 
   ddr_kopengl_drom_update_lcstowcsmat_shear
   ddr_kopengl_drom_update_lcstowcsmat_tfmmat 
   ddr_kopengl_drom_update_lcstowcsmat_transl
   ddr_kopengl_drom_update_lcstowcsmat_push 
   ddr_kopengl_drom_update_lcstowcsmat_pop 
   ddr_kopengl_drom_get_wcstofcsmat 
   ddr_kopengl_drom_push_lcstofcsmat 
   ddr_kopengl_drom_pop_lcstofcsmat 
   ddr_kopengl_drom_transform_clip_z_point

 **************************************************************************/

#include "drom.h"
#include "dore/internal/matrix.h"
#include <math.h>


static DtObject     dde_kopengl_save_shader;
static glReal       save_viewing_matrix[16];
static DtInt        save_lighting_state;


/*****************************************************************************
 *  This procedure returns (in `matrix') the current local-to-world
 *  transformation matrix currently on the top of the transformation matrix
 *  stack.
 ****************************************************************************/

void 
ddr_kopengl_drom_get_lcstowcsmat (DtMatrix4x4 matrix)
{
 glReal          xform[16];

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.get_lcstowcsmat\n");
#endif

 /*  
  * We have to remove the world_to_camera transformation at the front
  * of the transformations so post multiply by the camera_to_world,
  * which is the inverse of world_to_camera thus canceling out that
  * transform.  
  */

 /* Get current model view transfrom and save it */
 GET_GL_MATRIX (GL_MODELVIEW_MATRIX, xform);
 glPushMatrix ();
   LOAD_GL_MATRIX (CAMERA->cam_to_world_matrix);
   GL_MULT_MATRIX (xform);
   GET_GL_MATRIX (GL_MODELVIEW_MATRIX, xform);
 glPopMatrix ();
 CONVERT_OPENGL_TO_DORE (matrix, xform);

}



/*****************************************************************************
 *  This routine modifies the local-to-world transformation matrix that is
 *  currently on top of the matrix stack.  It effects a rotation about `axis'
 *  by an angle of `radians', based on the composition type `concat' (which
 *  can be one of pre-multiply, post-multiply, or replace).
 ****************************************************************************/

#define RadToDegrees(rad) ((rad)*57.2958)

void 
ddr_kopengl_drom_update_lcstowcsmat_rotate (DtAxis     axis,
					    DtReal     radians,
					    DtCompType concat)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.update_lcstowcsmat_rotate\n");
 fprintf (stdout, "  Axis of rotation: %d\n", axis);
 fprintf (stdout, "   Angle (degrees): %f\n", RadToDegrees (radians));
 fprintf (stdout, "     Concatenation: %d\n", concat);
#endif


 /* Note - Only pre-concatenation has been handled here */

 glMatrixMode (GL_MODELVIEW);
 switch (axis)
    {
     case DcXAxis:
      SET_GL_ROTATE (RadToDegrees (radians), 1.0, 0.0, 0.0);
      break;

     case DcYAxis:
      SET_GL_ROTATE (RadToDegrees (radians), 0.0, 1.0, 0.0);
      break;

     case DcZAxis:
      SET_GL_ROTATE (RadToDegrees (radians), 0.0, 0.0, 1.0);
      break;

     default:
      fprintf (stderr, "Invalid axis to rotate about!!\n");
    }
}



/*****************************************************************************
 *  This procedure applies a scaling transformation to the local-to-world
 *  transformation matrix currently on the top of the matrix stack.  The three
 *  scale values Sx, Sy, and Sz apply a scaling in the X, Y and Z directions.
 ****************************************************************************/

void 
ddr_kopengl_drom_update_lcstowcsmat_scale (DtReal     sx,
					   DtReal     sy,
					   DtReal     sz,
					   DtCompType concat)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.update_lcstowcsmat_scale\n");
 fprintf (stdout, "   Scale (x, y, z): %f %f %f\n", sx, sy, sz);
 fprintf (stdout, "     Concatenation: %d\n", concat);
#endif

 /* Note - Only pre-concatenation has been handled here */
 glMatrixMode (GL_MODELVIEW);
 SET_GL_SCALE (sx, sy, sz);
}



void 
ddr_kopengl_drom_update_lcstowcsmat_shear (DtMajorPlane plane,
					   DtReal       firstshearvalue,
					   DtReal       secondshearvalue,
					   DtCompType   concat)
{
 DtMatrix4x4     shmat;
 glReal          gl_shmat[16];


#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.update_lcstowcsmat_shear\n");
#endif

 /* Note - Only pre-concatenation has been handled here */
 dor_matrix_load_shear (shmat, plane, firstshearvalue, secondshearvalue);
 CONVERT_DORE_TO_OPENGL (gl_shmat, shmat);
 glMatrixMode (GL_MODELVIEW);
 GL_MULT_MATRIX (gl_shmat);
}


/*****************************************************************************
 *  This procedure applies an arbitrary transformation to the local-to-world
 *  transformation matrix currently on the top of the transformation stack.
 *  The matrix either pre-multiplies, post-multiplies, or replaces the current
 *  matrix, depending on the `concat' parameter.
 ****************************************************************************/

void 
ddr_kopengl_drom_update_lcstowcsmat_tfmmat (DtMatrix4x4 tfmmat,
					    DtCompType  concat)
{
 glReal          gl_tfmmat[16];
 glReal          gl_current[16];

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.update_lcstowcsmat_tfmmat\n");
#endif

 CONVERT_DORE_TO_OPENGL (gl_tfmmat, tfmmat);
 glMatrixMode (GL_MODELVIEW);

 switch (concat)
    {
     case DcPreConcatenate:
      GL_MULT_MATRIX (gl_tfmmat);
      break;

     case DcReplace:
      LOAD_GL_MATRIX (CAMERA->world_to_cam_matrix);
      GL_MULT_MATRIX (gl_tfmmat);
      break;

     case DcPostConcatenate:
      GET_GL_MATRIX (GL_MODELVIEW_MATRIX, gl_current);
      LOAD_GL_MATRIX (CAMERA->world_to_cam_matrix);
      GL_MULT_MATRIX (gl_tfmmat);
      GL_MULT_MATRIX (CAMERA->cam_to_world_matrix);
      GL_MULT_MATRIX (gl_current);
      break;
    }
}


/*****************************************************************************
 *  This routine applies a translation transformation to the local-to-world
 *  transformation matrix currently on the top of the matrix stack.  The
 *  translation either pre-multiplies, post-multiplies, or replaces the
 *  current matrix, depending on the concatenation parameter `concat'.  The
 *  translation values for X, Y and Z are given by tx, ty and tz.
 ****************************************************************************/

void 
ddr_kopengl_drom_update_lcstowcsmat_transl (DtReal     tx,
					    DtReal     ty,
					    DtReal     tz,
					    DtCompType concat)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.update_lcstowcsmat_transl\n");
#endif

 /* Note - Only pre-concatenation has been handled here */
 glMatrixMode (GL_MODELVIEW);
 SET_GL_TRANSLATE (tx, ty, tz);

}


/*****************************************************************************
 *  This routine saves the current local-to-world transformation matrix on the
 *  top of the matrix stack.  The matrix will later be restored with a call to
 * drom.update_lcstwcsmat_pop ().
 ****************************************************************************/

void 
ddr_kopengl_drom_update_lcstowcsmat_push (void)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.update_lcstowcsmat_push\n");
#endif

 glPushMatrix ();
}


/*****************************************************************************
 *  This function provides an interface to the matrix transformation stack,
 *  which specifies the coordinate transformation from the local space of the
 *  primitive objects to the world coordinate space.  The renderer calls this
 *  function to pop the current local-to-world transformation matrix and to
 *  restore the previous one.
 ****************************************************************************/

void 
ddr_kopengl_drom_update_lcstowcsmat_pop (void)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.update_lcstowcsmat_pop\n");
#endif

 glPopMatrix ();
}


void 
ddr_kopengl_drom_get_wcstofcsmat (DtMatrix4x4 wcstofcsmat)
{
 glReal tmp[16];

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.get_wcstofcsmat\n");
#endif

 GET_GL_MATRIX (GL_PROJECTION_MATRIX, tmp);
 glMatrixMode (GL_MODELVIEW);

 /* the OpenGL viewing model uses the z range -1 < z < 1 while the
  * Dore' viewing model uses the z range -1 < z < 0.  When returning
  * the wcstofcsmat a scale and translate is applied to the
  * matrix to convert to the Dore' viewing model.  
  */

 glPushMatrix ();
 LOAD_OPENGL_ID_MATRIX (tmp);
 SET_GL_TRANSLATE (0.,0.,-.5);
 SET_GL_SCALE (1.,1.,.5);
 GL_MULT_MATRIX (tmp);
 GL_MULT_MATRIX (CAMERA->world_to_cam_matrix);
 GET_GL_MATRIX (GL_MODELVIEW_MATRIX, tmp);
 glPopMatrix ();

 CONVERT_OPENGL_TO_DORE (wcstofcsmat, tmp);
}



/*****************************************************************************
 *  This subroutine saves the current local-to-frustum transformation matrix
 *  and replaces it with `newmatrix'.  This stack need only be one level deep.
 *  This function is used for frame-coordinate geometries, such as annotation
 *  text.
 ****************************************************************************/


void 
ddr_kopengl_drom_push_lcstofcsmat (DtMatrix4x4 newmatrix)
{
 glReal mat[16];

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.push_lcstofcsmat\n");
#endif

 CONVERT_DORE_TO_OPENGL (mat, newmatrix); 

 /*
  * Set the shading to the Constant shader.  Since we are 
  * replacing the lcstofcsmat matrix the lighting and normal transforms
  * may not be valid, so turn off light source shading
  */

 dde_kopengl_save_shader = dde_kopengl_drom_data.srfshd;
 save_lighting_state = DROM.lighting;  
 ddr_kopengl_drom_set_att_srfshd (DcShaderConstant);

 glMatrixMode (GL_PROJECTION);
 glLoadIdentity ();

 GET_GL_MATRIX (GL_MODELVIEW_MATRIX, save_viewing_matrix);
 glMatrixMode (GL_MODELVIEW);
 LOAD_GL_MATRIX (mat);
}


/*****************************************************************************
 *  This procedure restores the local-to-frustum transformation matrix
 *  previously saved by ddr_pex_drom_push_lcstofcsmat().  This is used by
 *  frame-coordinate geometries, such as annotation text.
 ****************************************************************************/

void 
ddr_kopengl_drom_pop_lcstofcsmat (void)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.pop_lcstofcsmat\n");
#endif

 /*
  * restore the shader that was saved with the pop
  */

 glMatrixMode (GL_PROJECTION);
 LOAD_GL_MATRIX (CAMERA->cam_to_frustum_matrix);

 glMatrixMode (GL_MODELVIEW);
 LOAD_GL_MATRIX (save_viewing_matrix);

 if (save_lighting_state)
   ddr_kopengl_drom_set_att_srfshd (dde_kopengl_save_shader);
}



/*****************************************************************************
 *  ddr_kopengl_drom_transform_clip_z_point() produces a transformation matrix
 *  for frame coordinate geometry, such as annotation text.  First, a point
 *  p' is computed as the transformation of `point' by the current local-to-
 *  frustum transformation matrix.  Next, Z clipping is performed.  If p' is
 *  outside the viewport and clipped away, this function returns DcTrue and
 *  does not derive the matrix.  If p' is inside the viewport, this function
 *  returns DcFalse and derives `tfmmatrix' by preconcatenating a translation
 *  matrix with a scale matrix.  The translation matrix is computed by
 *  dividing the scale factor returned from DDdevice_InqFrameScaleTrans by the
 *  SCaLe factor returned from DDdevice_InqFrustumScaleTrans.
 ****************************************************************************/

DtFlag 
ddr_kopengl_drom_transform_clip_z_point (DtRealTriple point,
					 DtMatrix4x4 tfmmatrix)
{
 glReal          view[16];
 glReal          tfm[16];
 DtRealTriple    temp;
 DtRealTriple    newpoint;


#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.transform_clip_z_point\n");
#endif

 /* Get view transformation matrix */
 GET_GL_MATRIX (GL_MODELVIEW_MATRIX, view); 
 
 /* 
  * Transform the point to eye (camera) coordinates, and 
  * then to frustum coordinates.  
  */

 ddr_kopengl_drom_transform_by_matrix (point, view, temp);
 ddr_kopengl_drom_transform_by_matrix
        (temp, CAMERA->cam_to_frustum_matrix, newpoint);

 /* Examine the frustum-space Z coordinate.  If it's out of the [-1,1]
  * range, then the point is clipped by the hither or yon plane, so
  * return true (the point is clipped).  
  */

 if ((newpoint[2] < -1.0) || (newpoint[2] > 1.0))
   return DcTrue;

 LOAD_OPENGL_ID_MATRIX (tfm);

 tfm[0]  = DEVICE->annoscale[0];
 tfm[5]  = DEVICE->annoscale[0];
 tfm[10] = DEVICE->annoscale[0];

 tfm[3]  = newpoint[0];
 tfm[7]  = newpoint[1];
 tfm[11] = newpoint[2];

 /* Store the transformation matrix in the Dore' specified Dore' matrix. */
 CONVERT_OPENGL_TO_DORE (tfmmatrix, tfm);

 return (DcFalse);
}




void 
ddr_kopengl_drom_transform_by_matrix (DtRealTriple in, 
                                      glReal       mat[16],
                                      DtRealTriple out)
{
 int    i,j;
 DtReal tempin[4];
 DtReal tempout[4];
 DtReal w;

 for (i=0; i<3; i++)
   tempin[i] = in[i];

 tempin[3] = 1.0;

 for (i=0; i<4; i++) 
   {
    tempout[i] = 0.0;

    for (j=0; j<4; j++)
      tempout[i] += tempin[j]*mat[j*4+i];
   }

 if ((w = tempout[3]) == 0.0)
   w = 1.;

 for (i=0; i<3; i++)
   out[i] = tempout[i]/w;
}



/*****************************************************************************
 *  This routine provides an interface to the the matrix transformation stack.
 *  update_lcstowcsmat_lokatfrm() is called by the renderer each time a
 *  DoLookAtFrom geometric transformation object is executed.  This function
 *  composes a look-at-from matrix with the current local-to-world
 *  transformation matrix based on the composition type `concat'.
 ****************************************************************************/

void 
ddr_kopengl_drom_update_lcstowcsmat_lokatfrm (DtPoint3 at,
					      DtPoint3 from,
					      DtVector3 up,
					      DtCompType concat)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.update_lcstowcsmat_lokatfrm\n");
#endif

 glMatrixMode (GL_MODELVIEW);
 gluLookAt (from[0], from[1], from[2], 
            at[0],   at[1],   at[2], 
            up[0],   up[1],   up[2]);
}

