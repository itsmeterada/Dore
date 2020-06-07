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
	ddr_x11_XFDI_drom_get_lcstowcsmat ddr_x11_XFDI_drom_get_lcstowcsmat (matrix)
	ddr_x11_XFDI_drom_update_lcstowcsmat_lokatfrm ddr_x11_XFDI_drom_update_lcstowcsmat_lokatfrm (at, from, up, concat)
	ddr_x11_XFDI_drom_update_lcstowcsmat_rotate ddr_x11_XFDI_drom_update_lcstowcsmat_rotate (axis, radians, concat)
	ddr_x11_XFDI_drom_update_lcstowcsmat_scale ddr_x11_XFDI_drom_update_lcstowcsmat_scale (sx, sy, sz, concat)
	ddr_x11_XFDI_drom_update_lcstowcsmat_shear ddr_x11_XFDI_drom_update_lcstowcsmat_shear (plane, firstshearvalue, secondshearvalue, concat)
	ddr_x11_XFDI_drom_update_lcstowcsmat_tfmmat ddr_x11_XFDI_drom_update_lcstowcsmat_tfmmat (tfmmat, concat)
	ddr_x11_XFDI_drom_update_lcstowcsmat_transl ddr_x11_XFDI_drom_update_lcstowcsmat_transl (tx, ty, tz, concat)
	ddr_x11_XFDI_drom_update_lcstowcsmat_push ddr_x11_XFDI_drom_update_lcstowcsmat_push ()
	ddr_x11_XFDI_drom_update_lcstowcsmat_pop ddr_x11_XFDI_drom_update_lcstowcsmat_pop ()
	int ddr_x11_XFDI_drom_update_matrices (matrix,invertmatrix,concat)
	int ddr_x11_XFDI_drom_get_wcstofcsmat (wcstofcsmat)
	int ddr_x11_XFDI_drom_push_lcstofcsmat (newmatrix)
	int ddr_x11_XFDI_drom_pop_lcstofcsmat ()
	DtFlag ddr_x11_XFDI_drom_transform_clip_z_point (point,tfmmatrix)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_get_lcstowcsmat (matrix)
     DtMatrix4x4 matrix;
{
    int matrixflag;

    XFDIQueryMatrix(dde_x11_XFDI_drom_curdevdat->display,
		    dde_x11_XFDI_drom_curdevdat->gc,
		    XFDILightMatrix,
		    matrix,
		    &matrixflag);
    dor_matrix_transpose(matrix);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_update_lcstowcsmat_lokatfrm (at, from, up, concat)
     DtPoint3 at;
     DtPoint3 from;
     DtVector3 up;
     DtCompType concat;
{
    DtMatrix4x4 matrix, invertmatrix;

    /* the XFDI model is a row vector for points while for Dore' it is
       a column vector 
       */

    dor_matrix_load_look_at_from(matrix,
				 at[0],at[1],at[2],
				 from[0],from[1],from[2],
				 up[0],up[1],up[2]);
    dor_matrix_transpose(matrix);
    dor_matrix_load (invertmatrix, matrix);
    dor_matrix_invert (invertmatrix);

    ddr_x11_XFDI_drom_update_matrices (matrix, invertmatrix, concat);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_update_lcstowcsmat_rotate (axis, radians, concat)
     DtAxis axis;
     DtReal radians;
     DtCompType concat;
{
    static DtMatrix4x4 xmatrix = { 1., 0., 0., 0., 
					   0., 1., 0., 0.,
					   0., 0., 1., 0., 
					   0., 0., 0., 1.};
    static DtMatrix4x4 ymatrix = { 1., 0., 0., 0., 
					   0., 1., 0., 0.,
					   0., 0., 1., 0., 
					   0., 0., 0., 1.};
    static DtMatrix4x4 zmatrix = { 1., 0., 0., 0., 
					   0., 1., 0., 0.,
					   0., 0., 1., 0., 
					   0., 0., 0., 1.};
    static DtMatrix4x4 xinvertmatrix = { 1., 0., 0., 0., 
						 0., 1., 0., 0.,
						 0., 0., 1., 0., 
						 0., 0., 0., 1.};
    static DtMatrix4x4 yinvertmatrix = { 1., 0., 0., 0., 
						 0., 1., 0., 0.,
						 0., 0., 1., 0., 
						 0., 0., 0., 1.};
    static DtMatrix4x4 zinvertmatrix = { 1., 0., 0., 0., 
						 0., 1., 0., 0.,
						 0., 0., 1., 0., 
						 0., 0., 0., 1.};
    DtReal dor_math_cos();
    DtReal dor_math_sin();
    DtReal cosine, ncosine;
    DtReal sine, nsine;

    cosine  = dor_math_cos(radians);
    sine    = dor_math_sin(radians);
    ncosine = dor_math_cos(-radians);
    nsine   = dor_math_sin(-radians);

    switch(axis) {
    case DcXAxis:
	xmatrix[1][1] = cosine;
	xmatrix[2][1] = -sine;
	xmatrix[1][2] = sine;
	xmatrix[2][2] = cosine;

	xinvertmatrix[1][1] = ncosine;
	xinvertmatrix[2][1] = -nsine;
	xinvertmatrix[1][2] = nsine;
	xinvertmatrix[2][2] = ncosine;

	ddr_x11_XFDI_drom_update_matrices (xmatrix, xinvertmatrix, concat);
	break;

    case DcYAxis:
	ymatrix[0][0] = cosine;
	ymatrix[2][0] = sine;
	ymatrix[0][2] = -sine;
	ymatrix[2][2] = cosine;

	yinvertmatrix[0][0] = ncosine;
	yinvertmatrix[2][0] = nsine;
	yinvertmatrix[0][2] = -nsine;
	yinvertmatrix[2][2] = ncosine;

	ddr_x11_XFDI_drom_update_matrices (ymatrix, yinvertmatrix, concat);
	break;

    case DcZAxis:
	zmatrix[0][0] = cosine;
	zmatrix[1][0] = -sine;
	zmatrix[0][1] = sine;
	zmatrix[1][1] = cosine;

	zinvertmatrix[0][0] = ncosine;
	zinvertmatrix[1][0] = -nsine;
	zinvertmatrix[0][1] = nsine;
	zinvertmatrix[1][1] = ncosine;

	ddr_x11_XFDI_drom_update_matrices (zmatrix, zinvertmatrix, concat);
	break;
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_update_lcstowcsmat_scale (sx, sy, sz, concat)
     DtReal sx, sy, sz;
     DtCompType concat;
{
    static DtMatrix4x4 matrix = { 1., 0., 0., 0., 
					  0., 1., 0., 0.,
					  0., 0., 1., 0., 
					  0., 0., 0., 1.};
    static DtMatrix4x4 invertmatrix = { 1., 0., 0., 0., 
						0., 1., 0., 0.,
						0., 0., 1., 0., 
						0., 0., 0., 1.};

    matrix[0][0] = sx;
    matrix[1][1] = sy;
    matrix[2][2] = sz;

    invertmatrix[0][0] = 1./sx;
    invertmatrix[1][1] = 1./sy;
    invertmatrix[2][2] = 1./sz;

    ddr_x11_XFDI_drom_update_matrices (matrix, invertmatrix, concat);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_update_lcstowcsmat_shear (plane, firstshearvalue, 
					     secondshearvalue, concat)
     DtMajorPlane plane;
     DtReal firstshearvalue, secondshearvalue;
     DtCompType concat;
{
   static dot_matrix_4x4 matrix = { 1., 0., 0., 0., 
				    0., 1., 0., 0.,
				    0., 0., 1., 0., 
				    0., 0., 0., 1.};
   static dot_matrix_4x4 invertmatrix = { 1., 0., 0., 0., 
					  0., 1., 0., 0.,
					  0., 0., 1., 0., 
					  0., 0., 0., 1.};

   switch(plane) {
   case DcYZ:
      matrix[0][1] = firstshearvalue;
      matrix[0][2] = secondshearvalue;

      matrix[0][1] = -firstshearvalue;
      matrix[0][2] = -secondshearvalue;

      break;

   case DcXZ:
      matrix[1][0] = firstshearvalue;
      matrix[1][2] = secondshearvalue;

      matrix[1][0] = -firstshearvalue;
      matrix[1][2] = -secondshearvalue;

      break;

   case DcXY:
      matrix[2][0] = firstshearvalue;
      matrix[2][1] = secondshearvalue;

      matrix[2][0] = -firstshearvalue;
      matrix[2][1] = -secondshearvalue;

      break;
   }

   dor_matrix_load (invertmatrix, matrix);
   dor_matrix_invert (invertmatrix);

   ddr_x11_XFDI_drom_update_matrices (matrix, invertmatrix, concat);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_update_lcstowcsmat_tfmmat (tfmmat, concat)
     DtMatrix4x4 tfmmat;
     DtCompType concat;
{
    static DtMatrix4x4 invertmatrix;
    static DtMatrix4x4 matrix;

    dor_matrix_load(matrix, tfmmat);
    dor_matrix_transpose(matrix);

    dor_matrix_load(invertmatrix,matrix);
    dor_matrix_invert(invertmatrix);

    ddr_x11_XFDI_drom_update_matrices (matrix, invertmatrix, concat);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_update_lcstowcsmat_transl (tx, ty, tz, concat)
     DtReal tx, ty, tz;
     DtCompType concat;
{
    static DtMatrix4x4 matrix = { 1., 0., 0., 0., 
					  0., 1., 0., 0.,
					  0., 0., 1., 0., 
					  0., 0., 0., 1.};
    static DtMatrix4x4 invertmatrix = { 1., 0., 0., 0., 
						0., 1., 0., 0.,
						0., 0., 1., 0., 
						0., 0., 0., 1.};

    matrix[3][0] = tx;
    matrix[3][1] = ty;
    matrix[3][2] = tz;

    invertmatrix[3][0] = -tx;
    invertmatrix[3][1] = -ty;
    invertmatrix[3][2] = -tz;

    ddr_x11_XFDI_drom_update_matrices (matrix, invertmatrix, concat);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_update_lcstowcsmat_push ()
{
    ddt_x11_XFDI_drom_matrix_stack *ms;
    DtMatrix4x4 matrix;
    int matrixflag;

    ms = &(dde_x11_XFDI_drom_data.matrix_stack);

    if (ms->current_element == ms->last_element)	{
	ms->elements = (ddt_x11_XFDI_drom_matrix_element *)DDspace_Reallocate
		(ms->elements,((ms->last_element+1)+10)*
		 (sizeof (ddt_x11_XFDI_drom_matrix_element)));
      
	ms->last_element += 10;
    }
		
    ms->current_element++;

    XFDIQueryMatrix(dde_x11_XFDI_drom_curdevdat->display,
		    dde_x11_XFDI_drom_curdevdat->gc,
		    XFDIObjectMatrix,
		    ms->elements[ms->current_element].object_matrix,
		    &matrixflag);

    XFDIQueryMatrix(dde_x11_XFDI_drom_curdevdat->display,
		    dde_x11_XFDI_drom_curdevdat->gc,
		    XFDILightMatrix,
		    ms->elements[ms->current_element].light_matrix,
		    &matrixflag);

    XFDIQueryMatrix(dde_x11_XFDI_drom_curdevdat->display,
		    dde_x11_XFDI_drom_curdevdat->gc,
		    XFDINormalMatrix,
		    ms->elements[ms->current_element].normal_matrix,
		    &matrixflag);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_update_lcstowcsmat_pop ()
{
    ddt_x11_XFDI_drom_matrix_stack *ms;

    ms = &(dde_x11_XFDI_drom_data.matrix_stack);

    if (ms->current_element >= 0) {
	XFDISetMatrix(dde_x11_XFDI_drom_curdevdat->display,
		      dde_x11_XFDI_drom_curdevdat->gc,
		      XFDIObjectMatrix,
		      ms->elements[ms->current_element].object_matrix,
		      XFDIMatrix4x4);

	XFDISetMatrix(dde_x11_XFDI_drom_curdevdat->display,
		      dde_x11_XFDI_drom_curdevdat->gc,
		      XFDILightMatrix,
		      ms->elements[ms->current_element].light_matrix,
		      XFDIMatrix4x4);

	XFDISetMatrix(dde_x11_XFDI_drom_curdevdat->display,
		      dde_x11_XFDI_drom_curdevdat->gc,
		      XFDINormalMatrix,
		      ms->elements[ms->current_element].normal_matrix,
		      XFDIMatrix4x4);
    }
    ms->current_element--;
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_update_matrices(matrix,invertmatrix,concat)
     DtMatrix4x4 matrix, invertmatrix;
     DtCompType concat;
{
    DtMatrix4x4 tmpmatrix;
    DtMatrix4x4 tinverse;

    dor_matrix_load(tinverse, invertmatrix);
    dor_matrix_transpose(tinverse);

    switch (concat) {
    case DcPreConcatenate:
	XFDIPreConcatMatrix(dde_x11_XFDI_drom_curdevdat->display,
			    dde_x11_XFDI_drom_curdevdat->gc,
			    XFDIObjectMatrix,
			    matrix,
			    XFDIMatrix4x4);

	XFDIPreConcatMatrix(dde_x11_XFDI_drom_curdevdat->display,
			    dde_x11_XFDI_drom_curdevdat->gc,
			    XFDILightMatrix,
			    matrix,
			    XFDIMatrix4x4);

	XFDIPreConcatMatrix(dde_x11_XFDI_drom_curdevdat->display,
			    dde_x11_XFDI_drom_curdevdat->gc,
			    XFDINormalMatrix,
			    tinverse,
			    XFDIMatrix4x4);
	break;

    case DcPostConcatenate:
	XFDIPostConcatMatrix(dde_x11_XFDI_drom_curdevdat->display,
			     dde_x11_XFDI_drom_curdevdat->gc,
			     XFDILightMatrix,
			     matrix,
			     XFDIMatrix4x4);

	XFDIPostConcatMatrix(dde_x11_XFDI_drom_curdevdat->display,
			     dde_x11_XFDI_drom_curdevdat->gc,
			     XFDINormalMatrix,
			     tinverse,
			     XFDIMatrix4x4);

	/*
	 * For post concatenation of of the object matrix (it contains
	 * the projection matrix) the light matrix (has just the lcstowcs)
	 * is copied to the object matrix and then the world to frustum matrix
	 * is post concatenated to this matrix
	 */

	XFDICopyMatrix(dde_x11_XFDI_drom_curdevdat->display,
		       dde_x11_XFDI_drom_curdevdat->gc,
		       XFDIObjectMatrix, XFDILightMatrix);
	dor_matrix_load(tmpmatrix, 
			dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix);
	dor_matrix_transpose(tmpmatrix);
	XFDIPostConcatMatrix(dde_x11_XFDI_drom_curdevdat->display,
			     dde_x11_XFDI_drom_curdevdat->gc,
			     XFDIObjectMatrix,
			     tmpmatrix,
			     XFDIMatrix4x4);

	break;
    case DcReplace:
	dor_matrix_load(tmpmatrix, 
			dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix);
	dor_matrix_transpose(tmpmatrix);
	XFDISetMatrix(dde_x11_XFDI_drom_curdevdat->display,
		      dde_x11_XFDI_drom_curdevdat->gc,
		      XFDIObjectMatrix,
		      matrix,
		      XFDIMatrix4x4);
	XFDIPreConcatMatrix(dde_x11_XFDI_drom_curdevdat->display,
			    dde_x11_XFDI_drom_curdevdat->gc,
			    XFDIObjectMatrix,
			    matrix,
			    XFDIMatrix4x4);

	XFDISetMatrix(dde_x11_XFDI_drom_curdevdat->display,
		      dde_x11_XFDI_drom_curdevdat->gc,
		      XFDILightMatrix,
		      matrix,
		      XFDIMatrix4x4);

	XFDISetMatrix(dde_x11_XFDI_drom_curdevdat->display,
		      dde_x11_XFDI_drom_curdevdat->gc,
		      XFDINormalMatrix,
		      tinverse,
		      XFDIMatrix4x4);
	break;
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_get_wcstofcsmat(wcstofcsmat)
     DtMatrix4x4 wcstofcsmat;
{
    dor_matrix_load (wcstofcsmat, dde_x11_XFDI_drom_curcamdat->world_to_frustum_matrix);
}
/*
 ======================================================================
 */

DtMatrix4x4 dde_x11_XFDI_lcstofcsmat_temp;

ddr_x11_XFDI_drom_push_lcstofcsmat(newmatrix)
     DtMatrix4x4 newmatrix;
{
    int matrixflag;
    DtMatrix4x4 matrix;

    XFDIQueryMatrix(dde_x11_XFDI_drom_curdevdat->display,
		    dde_x11_XFDI_drom_curdevdat->gc,
		    XFDIObjectMatrix,
		    dde_x11_XFDI_lcstofcsmat_temp,
		    &matrixflag);

    dor_matrix_load(matrix, newmatrix);
    dor_matrix_transpose(matrix);

    XFDISetMatrix(dde_x11_XFDI_drom_curdevdat->display,
		  dde_x11_XFDI_drom_curdevdat->gc,
		  XFDIObjectMatrix,
		  matrix,
		  XFDIMatrix4x4);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_pop_lcstofcsmat()

{
    XFDISetMatrix(dde_x11_XFDI_drom_curdevdat->display,
		  dde_x11_XFDI_drom_curdevdat->gc,
		  XFDIObjectMatrix,
		  dde_x11_XFDI_lcstofcsmat_temp,
		  XFDIMatrix4x4);
}
/*
 ======================================================================
 */
DtFlag ddr_x11_XFDI_drom_transform_clip_z_point(point,tfmmatrix)
     DtRealTriple point;
     DtMatrix4x4 tfmmatrix;

{
    /*
     * Transform the point from local coordinates to frustrum space coordinates
     * check for outside Z clipping range if so return DcTrue
     * set tfmmatrix to be the composite matrix of 
     * translation of the transformed point pre concatenated with a
     * scale matrix of the annotation scale for the current device
     * return DcFalse
     */

    DtMatrix4x4 objectmatrix;
    int matrixflag;
    DtInt row, column;
    DtReal fcspoint[4];
    DtReal oneoverw;
    DtRealTriple tfmpoint;

    /*  Transform the point.  */

    XFDIQueryMatrix(dde_x11_XFDI_drom_curdevdat->display,
		    dde_x11_XFDI_drom_curdevdat->gc,
		    XFDIObjectMatrix,
		    objectmatrix,
		    &matrixflag);

    for (row=0;row<4;row++) {
	fcspoint[row] = objectmatrix[3][row];

	for (column=0;column<3;column++) {
	    fcspoint[row] += point[row] * objectmatrix[row][column];
	}
    }

    if (fcspoint[2] < -fcspoint[3])
	    return(DcTrue);
    if (fcspoint[2] > 0)
	    return(DcTrue);

    oneoverw = 1. / fcspoint[3];

    tfmpoint[0] = fcspoint[0] * oneoverw;
    tfmpoint[1] = fcspoint[1] * oneoverw;
    tfmpoint[2] = fcspoint[2] * oneoverw;

    dor_matrix_translate(tfmmatrix,
			 tfmpoint[0],tfmpoint[1],tfmpoint[2],
			 DcReplace);

    dor_matrix_scale(tfmmatrix,
		     dde_x11_XFDI_drom_curdevdat->annoscale[0],
		     dde_x11_XFDI_drom_curdevdat->annoscale[1],
		     dde_x11_XFDI_drom_curdevdat->annoscale[2],
		     DcPreConcatenate);

    return(DcFalse);
}
