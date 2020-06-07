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
**  This file contains dynamic rendering functions for the PEX driver.  The
**  functions in this file govern the matrices for transformation to and from
**  world coordinates, local coordinates, normalized device coordinates,
**  viewing coordinates, and so on.  A number of these functions also control
**  the transformation matrix stack.
*****************************************************************************/

#include "../pex_driver.h"

#define DEBUG 0


    /***  Function Declarations  ***/

static void UpdateMatrices ARGS((DtMatrix4x4, DtCompType));



/*****************************************************************************
**  This procedure returns (in `matrix') the current local-to-world
**  transformation matrix currently on the top of the transformation matrix
**  stack.
*****************************************************************************/

void  ddr_pex_drom_get_lcstowcsmat  (DtMatrix4x4 matrix)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_get_lcstowcsmat (%lx)\n", matrix);
	fflush (stdout);
#   endif

    dor_matrix_load (matrix, DROM.M_local_world);
}



/*****************************************************************************
**  This function returns the current composite transformation matrix that
**  transforms data from the world coordinate system to the frustum
**  coordinate system.  The matrix is stored in the matrix given by the
**  parameter `P_world_frustum'.  The composite transformation matrix does
**  not include any transformations that occur as a result of calls to
**  ddr_pex_drom_update_lcstowcs_* functions.
*****************************************************************************/

void  ddr_pex_drom_get_wcstofcsmat  (DtMatrix4x4 P_world_frustum)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_get_wcstofcsmat (%lx)\n", P_world_frustum);
	fflush (stdout);
#   endif

    dor_matrix_load (P_world_frustum, DROM.M_world_frustum);
}



/*****************************************************************************
**  This procedure restores the local-to-frustum transformation matrix
**  previously saved by ddr_pex_drom_push_lcstofcsmat().  This is used by
**  frame-coordinate geometries, such as annotation text.
*****************************************************************************/

void ddr_pex_drom_pop_lcstofcsmat (void)
{
#   if DEBUG_CALL
	print ("ddr_pex_drom_pop_lcstofcsmat ()\n");
	fflush (stdout);
#   endif

    ddr_pex_drom_update_lcstowcsmat_pop ();

    UpdateMatrices (DROM.M_local_world, DcReplace);

    DROM.pexview = DROM.pexview_save;

    PEXSetTableEntries
    (   DCM->display,
	DROM.pexview_LUT, 0, 1, PEXLUTView,
	(PEXPointer)(&DROM.pexview)
    );

    /* Restore the shader that was saved with the pop. */

    PEXSetReflectionModel
	(DCM->display, PEX.renderer, PEXOCRender, DROM.shading_model);
}



/*****************************************************************************
**  This subroutine saves the current local-to-frustum transformation matrix
**  and replaces it with `newmatrix'.  This stack need only be one level deep.
**  This function is used for frame-coordinate geometries, such as annotation
**  text.
*****************************************************************************/

void  ddr_pex_drom_push_lcstofcsmat  (DtMatrix4x4 newmatrix)
{
#   if DEBUG_CALL
	print  ("ddr_pex_drom_push_lcstofcsmat\n");
	PrintMatrix (newmatrix);
	fflush (stdout);
#   endif

    ddr_pex_drom_update_lcstowcsmat_push ();

    dor_matrix_load (DROM.M_local_frustum, newmatrix);

    /* Set the local-to-world transformation to the identity matrix, and
    ** fake the PEX orientation and mapping matrices so that the new matrix
    ** is the full transformation from local coordinates to frustum
    ** coordinates.  */

    UpdateMatrices (Identity4x4, DcReplace);

    DROM.pexview_save = DROM.pexview;
    MATRIX_LOAD (DROM.pexview.orientation, Identity4x4);
    MATRIX_LOAD (DROM.pexview.mapping,     newmatrix);

    PEXSetTableEntries
    (   DCM->display,
	DROM.pexview_LUT, 0,1, PEXLUTView,
	(PEXPointer)(&DROM.pexview)
    );

    /* Set the shading to the Constant shader.  Since we are replacing the
    ** lcstofcsmat matrix, the lighting and normal transforms may not be
    ** valid, so turn off light source shading.  */

    PEXSetReflectionModel
	(DCM->display, PEX.renderer, PEXOCRender, PEXReflectionNone);
}



/*****************************************************************************
**  ddr_pex_drom_transform_clip_z_point() produces a transformation matrix
**  for frame coordinate geometry, such as annotation text.  First, a point
**  p' is computed as the transformation of `point' by the current local-to-
**  frustum transformation matrix.  Next, Z clipping is performed.  If p' is
**  outside the viewport and clipped away, this function returns DcTrue and
**  does not derive the matrix.  If p' is inside the viewport, this function
**  returns DcFalse and derives `tfmmatrix' by preconcatenating a translation
**  matrix with a scale matrix.  The translation matrix is computed by
**  dividing the scale factor returned from DDdevice_InqFrameScaleTrans by the
**  scale factor returned from DDdevice_InqFrustumScaleTrans.
*****************************************************************************/

DtFlag  ddr_pex_drom_transform_clip_z_point  (
    DtRealTriple point,		/* Target Point in Local Coordinates */
    DtMatrix4x4  tfmmatrix)	/* Return Storage for Transformation Matrix */
{
    auto     DtReal       fcspoint[4];	/* Point in Frustum Coordinates */
    register DtReal       oneoverw;	/* 1 / Pw */
    register DtShort      row;		/* Vector/Matrix Row Index*/
    auto     DtRealTriple tfmpoint;	/* Transformed Point */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_transform_clip_z_point (<%f,%f,%f>, %lx)\n",
	    point[0], point[1], point[2], tfmmatrix);
	fflush (stdout);
#   endif

    /*  Transform the 3D point from local coordinates to frustum-space
    **  coordintates.  Use 1.0 as the W coordinate of the 3D point.  */

    for (row=0;  row <= 3;  ++row)
    {   fcspoint[row] = (DROM.M_local_frustum[row][0] * point[0])
		      + (DROM.M_local_frustum[row][1] * point[1])
		      + (DROM.M_local_frustum[row][2] * point[2])
		      + (DROM.M_local_frustum[row][3]);
    }

    /*  Examine the Z coordinate in frustum space.  If it's out of the [0,1]
    **  range, then the point is clipped by the hither or yon plane, so return
    **  true (the point is clipped).  */

    if ((fcspoint[2] < 0.0) || (fcspoint[2] > 1.0))
	return DcTrue;

    /*  Normalize the transformed point so that the W coordinate is 1.0.  */

    oneoverw = 1 / fcspoint[3];
    tfmpoint[0] = fcspoint[0] * oneoverw;
    tfmpoint[1] = fcspoint[1] * oneoverw;
    tfmpoint[2] = fcspoint[2] * oneoverw;

#   if DEBUG
	printf ("+ Transformed & normalized point <%f %f %f>\n",
	    veclist(tfmpoint));
	fflush (stdout);
#   endif

    /*  Load the transformation matrix with a translation to the transformed
    **  (and normalized) 3D point.  */

    dor_matrix_load_translate (tfmmatrix, veclist (tfmpoint));

    /*  Now concatenate the transformation (translation) matrix with a scaling
    **  so that the frustum region is scaled by the annotation text scaling
    **  factors.  */

    dor_matrix_scale
    (   tfmmatrix, DEVICE->annoscale[0] / 2, DEVICE->annoscale[1] / 2,
	DEVICE->annoscale[2], DcPreConcatenate
    );

#   if DEBUG
	print ("+ Transformation Matrix:\n");
	PrintMatrix (tfmmatrix);
	fflush (stdout);
#   endif

    return DcFalse;
}



/*****************************************************************************
**  This routine provides an interface to the the matrix transformation stack.
**  update_lcstowcsmat_lokatfrm() is called by the renderer each time a
**  DoLookAtFrom geometric transformation object is executed.  This function
**  composes a look-at-from matrix with the current local-to-world
**  transformation matrix based on the composition type `concat'.
*****************************************************************************/

void  ddr_pex_drom_update_lcstowcsmat_lokatfrm  (
    DtPoint3   at,
    DtPoint3   from,
    DtVector3  up,
    DtCompType concat)
{
    DtMatrix4x4 matrix;

#   if DEBUG_CALL
	print  ("ddr_pex_drom_update_lcstowcsmat_lokatfrm (");
	printf ("<%f %f %f>,", at[0], at[1], at[2]);
	printf ("<%f %f %f>,", from[0], from[1], from[2]);
	printf ("<%f %f %f>,", up[0], up[1], up[2]);
	printf ("%d)\n", concat);
	fflush (stdout);
#   endif

    dor_matrix_load_look_at_from
	(matrix, veclist(at), veclist(from), veclist(up));

    UpdateMatrices (matrix, concat);
}



/*****************************************************************************
**  This function provides an interface to the matrix transformation stack,
**  which specifies the coordinate transformation from the local space of the
**  primitive objects to the world coordinate space.  The renderer calls this
**  function to pop the current local-to-world transformation matrix and to
**  restore the previous one.
*****************************************************************************/

void ddr_pex_drom_update_lcstowcsmat_pop (void)
{
    DtInt index;

#   if DEBUG_CALL
	print ("ddr_pex_drom_update_lcstowcsmat_pop ()\n");
	fflush (stdout);
#   endif

    if (DROM.matrix_stack_top > 0)
    {   index = --DROM.matrix_stack_top;

	dor_matrix_load (DROM.M_local_world,
			 DROM.matrix_stack[index].M_local_world);
	dor_matrix_load (DROM.M_local_frustum,
			 DROM.matrix_stack[index].M_local_frustum);
    }

    UpdateMatrices (DROM.M_local_world, DcReplace);
}



/*****************************************************************************
**  This routine saves the current local-to-world transformation matrix on the
**  top of the matrix stack.  The matrix will later be restored with a call to
**  _update_lcstwcsmat_pop().
*****************************************************************************/

void ddr_pex_drom_update_lcstowcsmat_push (void)
{
    DtInt index;

#   if DEBUG_CALL
	print ("ddr_pex_drom_update_lcstowcsmat_push ()\n");
	fflush (stdout);
#   endif

    index = DROM.matrix_stack_top++;

    if (DROM.matrix_stack_top > DROM.matrix_stack_size)
    {
	DROM.matrix_stack_size += MATRIX_STACK_GROW_SIZE;

	DROM.matrix_stack = DDspace_Reallocate
			    (   DROM.matrix_stack,
				DROM.matrix_stack_size
				* sizeof(ddt_pex_drom_matrix)
			    );
    }

    dor_matrix_load (DROM.matrix_stack[index].M_local_world,
		     DROM.M_local_world);
    dor_matrix_load (DROM.matrix_stack[index].M_local_frustum,
		     DROM.M_local_frustum);
}



/*****************************************************************************
**  This routine modifies the local-to-world transformation matrix that is
**  currently on top of the matrix stack.  It effects a rotation about `axis'
**  by an angle of `radians', based on the composition type `concat' (which
**  can be one of pre-multiply, post-multiply, or replace).
*****************************************************************************/

void  ddr_pex_drom_update_lcstowcsmat_rotate  (
    DtAxis     axis,
    DtReal     radians,
    DtCompType concat)
{
    static DtMatrix4x4 xmatrix = IDENTITY_4x4;
    static DtMatrix4x4 ymatrix = IDENTITY_4x4;
    static DtMatrix4x4 zmatrix = IDENTITY_4x4;

    auto   DtReal cosine, ncosine;
    auto   DtReal sine, nsine;

#   if DEBUG_CALL
	printf ("ddr_pex_drom_update_lcstowcsmat_rotate (%d, %f, %d)\n",
	    axis, radians, concat);
	fflush (stdout);
#   endif

    cosine  = cos ((double)radians);
    sine    = sin ((double)radians);
    ncosine = cos ((double)-radians);
    nsine   = sin ((double)-radians);

    switch (axis)
    {   case DcXAxis:
	{   xmatrix[1][1] = cosine;
	    xmatrix[2][1] = sine;
	    xmatrix[1][2] = -sine;
	    xmatrix[2][2] = cosine;
	    UpdateMatrices (xmatrix, concat);
	    break;
	}
	case DcYAxis:
	{   ymatrix[0][0] = cosine;
	    ymatrix[2][0] = -sine;
	    ymatrix[0][2] = sine;
	    ymatrix[2][2] = cosine;
	    UpdateMatrices (ymatrix, concat);
	    break;
	}
	case DcZAxis:
	{   zmatrix[0][0] = cosine;
	    zmatrix[1][0] = sine;
	    zmatrix[0][1] = -sine;
	    zmatrix[1][1] = cosine;
	    UpdateMatrices (zmatrix, concat);
	    break;
	}
    }
}



/*****************************************************************************
**  This procedure applies a scaling transformation to the local-to-world
**  transformation matrix currently on the top of the matrix stack.  The three
**  scale values Sx, Sy, and Sz apply a scaling in the X, Y and Z directions.
*****************************************************************************/

void  ddr_pex_drom_update_lcstowcsmat_scale  (
    DtReal     Sx,
    DtReal     Sy,
    DtReal     Sz,
    DtCompType concat)
{
    static DtMatrix4x4 matrix = IDENTITY_4x4;

#   if DEBUG_CALL
	printf ("ddr_pex_drom_update_lcstowcsmat_scale (%f, %f, %f, %d)\n",
	    Sx, Sy, Sz, concat);
	fflush (stdout);
#   endif

    matrix[0][0] = Sx;
    matrix[1][1] = Sy;
    matrix[2][2] = Sz;

    UpdateMatrices (matrix, concat);
}



/*****************************************************************************
**  This routine applies a shear transformation to the local-to-world
**  transformation matrix currently on the top of the matrix stack.  Depending
**  on the value of `concat', the matrix is pre-multiplied with,
**  post-multiplied with, or replaces the matrix on the top of the stack.  A
**  directional shear is done about the major plane specified by the parameter
**  `plane', with `shear1' and `shear2' specifying the shearing in the first
**  and second direction of `plane'.
*****************************************************************************/

void  ddr_pex_drom_update_lcstowcsmat_shear  (
    DtMajorPlane plane,
    DtReal       shear1,
    DtReal       shear2,
    DtCompType   concat)
{
    static DtMatrix4x4 matrix = IDENTITY_4x4;

#   if DEBUG_CALL
	printf ("ddr_pex_drom_update_lcstowcsmat_shear (%d, %f, %f, %d)\n",
	    plane, shear1, shear2, concat);
	fflush (stdout);
#   endif

    switch (plane)
    {   case DcYZ:  matrix[1][0] = shear1;  matrix[2][0] = shear2;   break;
	case DcXZ:  matrix[0][1] = shear1;  matrix[2][1] = shear2;   break;
	case DcXY:  matrix[0][2] = shear1;  matrix[1][2] = shear2;   break;
    }

    UpdateMatrices (matrix, concat);
}



/*****************************************************************************
**  This procedure applies an arbitrary transformation to the local-to-world
**  transformation matrix currently on the top of the transformation stack.
**  The matrix either pre-multiplies, post-multiplies, or replaces the current
**  matrix, depending on the `concat' parameter.
*****************************************************************************/

void  ddr_pex_drom_update_lcstowcsmat_tfmmat  (
    DtMatrix4x4 tfmmat,
    DtCompType  concat)
{
#   if DEBUG_CALL
	print  ("ddr_pex_drom_update_lcstowcsmat_tfmmat\n");
	print  ("    tfmmat:\n");
	PrintMatrix (tfmmat);
	printf ("    concat %d\n", concat);
	fflush (stdout);
#   endif

    UpdateMatrices (tfmmat, concat);
}



/*****************************************************************************
**  This routine applies a translation transformation to the local-to-world
**  transformation matrix currently on the top of the matrix stack.  The
**  translation either pre-multiplies, post-multiplies, or replaces the
**  current matrix, depending on the concatenation parameter `concat'.  The
**  translation values for X, Y and Z are given by Tx, Ty and Tz.
*****************************************************************************/

void  ddr_pex_drom_update_lcstowcsmat_transl  (
    DtReal     Tx,
    DtReal     Ty,
    DtReal     Tz,
    DtCompType concat)
{
    /* The following matrices are statically initialized because only
    ** three matrix entries are ever modified. */

    static DtMatrix4x4 matrix = IDENTITY_4x4;

#   if DEBUG_CALL
	printf ("ddr_pex_drom_update_lcstowcsmat_transl (%f, %f, %f, %d)\n",
	    Tx, Ty, Tz, concat);
	fflush (stdout);
#   endif

    matrix[0][3] = Tx;
    matrix[1][3] = Ty;
    matrix[2][3] = Tz;

    UpdateMatrices (matrix, concat);
}



/*****************************************************************************
**  This routine is called to update the current transformation matrices
**  whenever a transformation change has been made.
*****************************************************************************/

static void  UpdateMatrices  (
    DtMatrix4x4 matrix,		/* New Matrix Transformation */
    DtCompType  concat)		/* Matrix Concatenation Type */
{
    auto PEXMatrix PEX_local_world;	/* Local Xform */

#   if DEBUG_CALL
	print  ("UpdateMatrices\n");
	print  ("    matrix:\n");
	PrintMatrix (matrix);
	printf ("    concat %d\n", concat);
	fflush (stdout);
#   endif

#   if DEBUG
	print ("+ Local-to-world (before):\n");
	PrintMatrix (DROM.M_local_world);
	print ("+ Local-to-frustum (before):\n");
	PrintMatrix (DROM.M_local_frustum);
#   endif

    switch (concat)
    {   case DcPreConcatenate:
	    dor_matrix_concat (DROM.M_local_world, matrix, DcPreConcatenate);
	    dor_matrix_concat (DROM.M_local_frustum, matrix, DcPreConcatenate);
	    break;

	case DcPostConcatenate:
	    dor_matrix_concat (DROM.M_local_world, matrix, DcPostConcatenate);

	    /* For post concatenation of the lcswofcsmat matrix (it contains
	    ** the projection matrix) the light matrix (has just the lcstowcs)
	    ** is copied to the object matrix and then the world to frustum
	    ** matrix is post concatenated to this matrix.  */

	    dor_matrix_load   (DROM.M_local_frustum, DROM.M_local_world);
	    dor_matrix_concat (DROM.M_local_frustum, DROM.M_world_frustum,
			       DcPostConcatenate);
	    break;

	case DcReplace:
	    dor_matrix_load (DROM.M_local_world, matrix);

	    /* For replacement of the M_local_frustum matrix (it contains the
	    ** projection matrix) the world to frustum matrix is copied into
	    ** the M_local_frustum and the new matrix will be pre concatenated
	    ** to the M_world_frustum.  */

	    dor_matrix_load   (DROM.M_local_frustum, DROM.M_world_frustum);
	    dor_matrix_concat (DROM.M_local_frustum, matrix, DcPreConcatenate);
	    break;
    }

#   if DEBUG
	print ("+ Local-to-world (after):\n");
	PrintMatrix (DROM.M_local_world);
	print ("+ Local-to-frustum (after):\n");
	PrintMatrix (DROM.M_local_frustum);
#   endif

    /* Set the current PEX local transformation matrix. */

    MATRIX_LOAD (PEX_local_world, DROM.M_local_world);
    PEXSetLocalTransform
	(DCM->display, PEX.renderer, PEXOCRender, PEXReplace, PEX_local_world);
}
