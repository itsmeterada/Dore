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
	ddr_gl_drom_get_lcstowcsmat
	ddr_gl_drom_get_wcstofcsmat
	ddr_gl_drom_pop_lcstofcsmat
	ddr_gl_drom_push_lcstofcsmat
	ddr_gl_drom_transform_by_matrix
	ddr_gl_drom_transform_clip_z_point
	ddr_gl_drom_update_lcstowcsmat_lokatfrm
	ddr_gl_drom_update_lcstowcsmat_pop
	ddr_gl_drom_update_lcstowcsmat_push
	ddr_gl_drom_update_lcstowcsmat_rotate
	ddr_gl_drom_update_lcstowcsmat_scale
	ddr_gl_drom_update_lcstowcsmat_tfmmat
	ddr_gl_drom_update_lcstowcsmat_transl

  ======================================================================
 */
#include "../gl_driver.h"
#include <dore/internal/matrix.h>

/* ====================================================================== */

void ddr_gl_drom_get_lcstowcsmat (DtMatrix4x4 matrix)
{
    auto Matrix tfm;

    /*  We have to remove the world_to_camera transformation at the front
    **  of the transformations so post multiply by the camera_to_world,
    **  which is the inverse of world_to_camera thus canceling out that
    **  transform.  */

    getmatrix(tfm);
    pushmatrix();
    loadmatrix(DevStatus.view_data->cam_to_world_matrix);
    multmatrix(tfm);
    getmatrix(tfm);
    popmatrix();
    ddr_iris_to_dore_matrix(tfm, matrix);
}



/* ====================================================================== */

void ddr_gl_drom_update_lcstowcsmat_lokatfrm (
    DtPoint3 at,
    DtPoint3 from,
    DtVector3 up,
    DtCompType concat)
{
    DtMatrix4x4 dm;
    Matrix im;

#   ifdef DEBUGP
	printf("Lookatfrom called\n");
#   endif

    dor_matrix_load_look_at_from
	(dm, at[0],at[1],at[2], from[0],from[1],from[2], up[0],up[1],up[2]);

    ddr_dore_to_iris_matrix(dm,im);

    multmatrix(im);
}



/* ====================================================================== */

#define RadToDegrees(rad) ((rad)*57.2958)

void ddr_gl_drom_update_lcstowcsmat_rotate (
    DtAxis axis,
    DtReal radians,
    DtCompType concat)
{
#   ifdef DEBUGP
	printf("Rotate on %d by %f degrees called...\n",
	    axis, RadToDegrees(radians));
#   endif

    switch (axis) {
	case DcXAxis: rot(RadToDegrees(radians),'x'); break;
	case DcYAxis: rot(RadToDegrees(radians),'y'); break;
	case DcZAxis: rot(RadToDegrees(radians),'z'); break;
	default:
	    printf("Bad axis to rotate!!\n");
    }
}



/* ====================================================================== */

void ddr_gl_drom_update_lcstowcsmat_scale (
    DtReal sx,
    DtReal sy,
    DtReal sz,
    DtCompType concat)
{
#   ifdef DEBUGP
	printf("Scale %f,%f,%f called...\n", sx,sy,sz);
#   endif

    scale(sx,sy,sz);
}



/* ====================================================================== */

void ddr_gl_drom_update_lcstowcsmat_shear (
    DtMajorPlane plane,
    DtReal firstshearvalue,
    DtReal secondshearvalue,
    DtCompType concat)
{
    DtMatrix4x4 dm;
    Matrix im;

#   ifdef DEBUGP
	printf("Shear() called!\n");
#   endif

    dor_matrix_load_shear(dm,plane,firstshearvalue,secondshearvalue);
    ddr_dore_to_iris_matrix(dm,im);
    multmatrix(im);
}



/* ====================================================================== */

void ddr_gl_drom_update_lcstowcsmat_tfmmat (
    DtMatrix4x4 tfmmat,
    DtCompType concat)
{
    Matrix tfm,current;

#   ifdef DEBUGP
	printf("Tfmmat called...\n");
#   endif

    ddr_dore_to_iris_matrix(tfmmat,tfm);

    switch (concat) {

    case DcPreConcatenate:
	multmatrix(tfm);
	break;

    case DcReplace:
	loadmatrix(DevStatus.view_data->world_to_cam_matrix);
	multmatrix(tfm);
	break;

    case DcPostConcatenate:
	getmatrix(current);
	loadmatrix(DevStatus.view_data->world_to_cam_matrix);
	multmatrix(tfm);
	multmatrix(DevStatus.view_data->cam_to_world_matrix);
	multmatrix(current);
	break;
    }
}



/* ====================================================================== */

void ddr_gl_drom_update_lcstowcsmat_transl (
    DtReal tx,
    DtReal ty,
    DtReal tz,
    DtCompType concat)
{
#   ifdef DEBUGP
	printf("Translate <%f,%f,%f> called\n", tx,ty,tz);
#   endif

    translate(tx,ty,tz);
}



/* ====================================================================== */

void ddr_gl_drom_update_lcstowcsmat_push (void)
{
    pushmatrix();

#   ifdef DEBUGP
	printf("Push matrix called\n");
#   endif
}



/* ====================================================================== */

void ddr_gl_drom_update_lcstowcsmat_pop (void)
{
#   ifdef DEBUGP
	printf("Popmatrix called\n");
#   endif
    popmatrix();
}



/* ====================================================================== */

void ddr_gl_drom_get_wcstofcsmat (
    DtMatrix4x4 wcstofcsmat)
{
    Matrix im;

    mmode (MPROJECTION);
    getmatrix (im);
    mmode (MVIEWING);

    /* the GL viewing model uses the z range -1 < z < 1 while the 
    ** Dore' viewing model uses the z range -1 < z < 0.  When returning
    ** the wcstofcsmat a scale and translate is applied to the 
    ** matrix to convert to the Dore' viewing model.  */

    pushmatrix ();
    ddr_iris_loadmatrix_id ();
    translate (0.,0.,-.5);
    scale (1.,1.,.5);
    multmatrix (im);
    multmatrix (DevStatus.view_data->world_to_cam_matrix);
    getmatrix (im);
    popmatrix ();

    ddr_iris_to_dore_matrix (im,wcstofcsmat);
}



/* ====================================================================== */

static Matrix viewsave;
static DtInt  lightingsave;

void ddr_gl_drom_push_lcstofcsmat (DtMatrix4x4 newmatrix)
{
    Matrix im;

    if (DevStatus.traversal_number == 1) {
	return;
    } else {
	DevStatus.frame_coord_prim = DcTrue;
    }

#   ifdef DEBUGP
	printf("Push_lcstofcsmat() called\n");
#   endif

    ddr_dore_to_iris_matrix(newmatrix,im);

    /* Since we are replacing the projection matrix, turn off lighting.  */

    lightingsave = DevStatus.lighting;
    ddr_gl_drom_set_att_srfshd(DcShaderConstant);

    mmode(MPROJECTION);
    ddr_iris_loadmatrix_id();

    mmode(MVIEWING);
    getmatrix(viewsave);

    ddr_iris_loadmatrix_id();
    loadmatrix(im);
}



/* ====================================================================== */

void ddr_gl_drom_pop_lcstofcsmat (void)
{
#   ifdef DEBUGP
	printf("Pop_lcstofcsmat() called\n");
#   endif

    if (DevStatus.traversal_number == 1) {
	return;
    } else {
	DevStatus.frame_coord_prim = DcFalse;
    }

    mmode(MPROJECTION);
    loadmatrix(DevStatus.view_data->cam_to_frustum_matrix);

    mmode(MVIEWING);
    loadmatrix(viewsave);

    /* restore lighting */

    if (lightingsave) 
	    ddr_gl_drom_set_att_srfshd (DcShaderLightSource);
}



/*****************************************************************************
**  ddr_gl_drom_transform_clip_z_point() produces a transformation matrix for
**  frame coordinate geometry, such as annotation text.  First, a point p' is
**  computed as the transformation of `point' by the current local-to-frustum
**  transformation matrix.  Next, Z clipping is performed.  If p' is outside
**  the viewport and clipped away, this function returns DcTrue and does not
**  derive the matrix.  If p' is inside the viewport, this function returns
**  DcFalse and derives `tfmmatrix' by preconcatenating a translation matrix
**  with a scale matrix.  The translation matrix is computed by dividing the
**  scale factor returned from DDdevice_InqFrameScaleTrans by the scale factor
**  returned from DDdevice_InqFrustumScaleTrans.
*****************************************************************************/

DtFlag ddr_gl_drom_transform_clip_z_point (
    DtRealTriple point,
    DtMatrix4x4 tfmmatrix)
{
    Matrix view;
    Matrix tfm;
    DtRealTriple temp;
    DtRealTriple newpoint;

#   ifdef DEBUGP
	printf("Transform_clip_Z_point(%f,%f,%f) called\n",
	    point[0], point[1], point[2]);
#   endif

    if (DevStatus.traversal_number == 1) {
	DevStatus.have_frame_coord_prim = DcTrue;
	return DcTrue;
    }

    /* Get the viewing transformation matrix.  */

    mmode (MVIEWING);
    getmatrix (view);

    /* Transform the point to eye (camera) coordinates, and then to
    ** frustum coordinates.  */

    ddr_gl_drom_transform_by_matrix (point, view, temp);
    ddr_gl_drom_transform_by_matrix
	(temp, DevStatus.view_data->cam_to_frustum_matrix, newpoint);

#   ifdef DEBUGP
	printf("     ... Transformed: <%f,%f,%f>\n",
	       newpoint[0], newpoint[1], newpoint[2]);
#   endif

    /* Examine the frustum-space Z coordinate.  If it's out of the [-1,1]
    ** range, then the point is clipped by the hither or yon plane, so
    ** return true (the point is clipped).  */

    if ((newpoint[2] < -1.0) || (newpoint[2] > 1.0))
	return DcTrue;

    ddr_iris_id_matrix (tfm);

    tfm[0][0] = DevStatus.window_data->frame_to_view_scale[0];
    tfm[1][1] = DevStatus.window_data->frame_to_view_scale[0];
    tfm[2][2] = DevStatus.window_data->frame_to_view_scale[0];

    tfm[3][0] = newpoint[0];
    tfm[3][1] = newpoint[1];
    tfm[3][2] = newpoint[2];

    /* Store the transformation matrix in the Dore' specified Dore' matrix. */

    ddr_iris_to_dore_matrix (tfm, tfmmatrix);

    return DcFalse;
}



/* ====================================================================== */

void ddr_gl_drom_transform_by_matrix(in,mat,out)
    DtRealTriple in,out;
    Matrix mat;
{
    int    i,j;
    DtReal tempin[4];
    DtReal tempout[4];
    DtReal w;

    for (i=0; i<3; i++)
	tempin[i] = in[i];

    tempin[3] = 1.;

    for (i=0; i<4; i++) {
	tempout[i] = 0.;

	for (j=0; j<4; j++)
	    tempout[i] += tempin[j]*mat[j][i];
    }

    if ((w = tempout[3]) == 0.0)
	w = 1.;

    for (i=0; i<3; i++)
	out[i] = tempout[i]/w;
}
