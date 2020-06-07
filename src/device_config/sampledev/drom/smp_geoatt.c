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
	void ddr_sampledev_drom_get_lcstowcsmat (matrix)
	void ddr_sampledev_drom_update_lcstowcsmat_lokatfrm (at, from, up, concat)
	void ddr_sampledev_drom_update_lcstowcsmat_rotate (axis, radians, concat)
	void ddr_sampledev_drom_update_lcstowcsmat_scale (sx, sy, sz, concat)
	void ddr_sampledev_drom_update_lcstowcsmat_shear (plane, firstshearvalue, secondshearvalue, concat)
	void ddr_sampledev_drom_update_lcstowcsmat_tfmmat (tfmmat, concat)
	void ddr_sampledev_drom_update_lcstowcsmat_transl (tx, ty, tz, concat)
	void ddr_sampledev_drom_update_lcstowcsmat_push ()
	void ddr_sampledev_drom_update_lcstowcsmat_pop ()
	void ddr_sampledev_drom_get_wcstofcsmat (wcstofcsmat)
	void ddr_sampledev_drom_push_lcstofcsmat (newmatrix)
	void ddr_sampledev_drom_pop_lcstofcsmat ()
	DtFlag ddr_sampledev_drom_transform_clip_z_point (point,tfmmatrix)

  ======================================================================
 */
#include "drom.h"
#include <math.h>
/*
 ======================================================================
 */

static DtMatrix4x4 dde_sampledev_save_lcstofcsmat;
static DtObject    dde_sampledev_save_shader;

void ddr_sampledev_drom_update_matrices();

/*
 ======================================================================
 */
void
ddr_sampledev_drom_get_lcstowcsmat (matrix)
DtMatrix4x4 matrix;
{
    ddr_sampledev_matrix_concat(matrix, dde_sampledev_drom_data.lcstowcsmat,
			       DcReplace);
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_update_lcstowcsmat_lokatfrm (at, from, up, concat)
DtPoint3 at;
DtPoint3 from;
DtVector3 up;
DtCompType concat;
{
    DtMatrix4x4 matrix, invertmatrix;

    ddr_sampledev_matrix_look_at_from(matrix, at, from, up);
    ddr_sampledev_matrix_concat (invertmatrix, matrix, DcReplace);
    ddr_sampledev_matrix_invert (invertmatrix);

    ddr_sampledev_drom_update_matrices (matrix, invertmatrix, concat);
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_update_lcstowcsmat_rotate (axis, radians, concat)
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
    DtReal cosine, ncosine;
    DtReal sine, nsine;

    cosine  = cos((double)radians);
    sine    = sin((double)radians);
    ncosine = cos((double)-radians);
    nsine   = sin((double)-radians);

    switch(axis) {
    case DcXAxis:
	xmatrix[1][1] = cosine;
	xmatrix[2][1] = sine;
	xmatrix[1][2] = -sine;
	xmatrix[2][2] = cosine;

	xinvertmatrix[1][1] = ncosine;
	xinvertmatrix[2][1] = nsine;
	xinvertmatrix[1][2] = -nsine;
	xinvertmatrix[2][2] = ncosine;

	ddr_sampledev_drom_update_matrices (xmatrix, xinvertmatrix, concat);
	break;

    case DcYAxis:
	ymatrix[0][0] = cosine;
	ymatrix[2][0] = -sine;
	ymatrix[0][2] = sine;
	ymatrix[2][2] = cosine;

	yinvertmatrix[0][0] = ncosine;
	yinvertmatrix[2][0] = -nsine;
	yinvertmatrix[0][2] = nsine;
	yinvertmatrix[2][2] = ncosine;

	ddr_sampledev_drom_update_matrices (ymatrix, yinvertmatrix, concat);
	break;

    case DcZAxis:
	zmatrix[0][0] = cosine;
	zmatrix[1][0] = sine;
	zmatrix[0][1] = -sine;
	zmatrix[1][1] = cosine;

	zinvertmatrix[0][0] = ncosine;
	zinvertmatrix[1][0] = nsine;
	zinvertmatrix[0][1] = -nsine;
	zinvertmatrix[1][1] = ncosine;

	ddr_sampledev_drom_update_matrices (zmatrix, zinvertmatrix, concat);
	break;
    }
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_update_lcstowcsmat_scale (sx, sy, sz, concat)
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

    ddr_sampledev_drom_update_matrices (matrix, invertmatrix, concat);
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_update_lcstowcsmat_shear (plane, firstshearvalue, 
					    secondshearvalue, concat)
DtMajorPlane plane;
DtReal firstshearvalue, secondshearvalue;
DtCompType concat;
{
   static DtMatrix4x4 matrix = { 1., 0., 0., 0., 
				 0., 1., 0., 0.,
				 0., 0., 1., 0., 
				 0., 0., 0., 1.};
   DtMatrix4x4 invertmatrix;

   switch(plane) {
   case DcYZ:
      matrix[1][0] = firstshearvalue;
      matrix[2][0] = secondshearvalue;
      break;

   case DcXZ:
      matrix[0][1] = firstshearvalue;
      matrix[2][1] = secondshearvalue;
      break;

   case DcXY:
      matrix[0][2] = firstshearvalue;
      matrix[1][2] = secondshearvalue;
      break;
   }

   ddr_sampledev_matrix_concat (invertmatrix, matrix, DcReplace);
   ddr_sampledev_matrix_invert (invertmatrix);

   ddr_sampledev_drom_update_matrices (matrix, invertmatrix, concat);
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_update_lcstowcsmat_tfmmat (tfmmat, concat)
DtMatrix4x4 tfmmat;
DtCompType concat;
{
    static DtMatrix4x4 invertmatrix;

    ddr_sampledev_matrix_concat(invertmatrix,tfmmat, DcReplace);
    ddr_sampledev_matrix_invert(invertmatrix);

    ddr_sampledev_drom_update_matrices (tfmmat, invertmatrix, concat);
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_update_lcstowcsmat_transl (tx, ty, tz, concat)
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

    matrix[0][3] = tx;
    matrix[1][3] = ty;
    matrix[2][3] = tz;

    invertmatrix[0][3] = -tx;
    invertmatrix[1][3] = -ty;
    invertmatrix[2][3] = -tz;

    ddr_sampledev_drom_update_matrices (matrix, invertmatrix, concat);
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_update_lcstowcsmat_push ()
{
    DtInt index;

    if (dde_sampledev_drom_data.num_matrices ==
	dde_sampledev_drom_data.max_matrices) {
	dde_sampledev_drom_data.matrix_stack = (ddt_sampledev_drom_matrix *)
		DDspace_Reallocate
			(dde_sampledev_drom_data.matrix_stack,
			 (dde_sampledev_drom_data.max_matrices+10)*
			 (sizeof (ddt_sampledev_drom_matrix)));
	dde_sampledev_drom_data.max_matrices += 10;
    }
		
    index = dde_sampledev_drom_data.num_matrices++;

    ddr_sampledev_matrix_concat (dde_sampledev_drom_data.matrix_stack[index].lcstowcsmat,
				dde_sampledev_drom_data.lcstowcsmat, 
				DcReplace);

    ddr_sampledev_matrix_concat (dde_sampledev_drom_data.matrix_stack[index].lcstofcsmat,
				dde_sampledev_drom_data.lcstofcsmat, 
				DcReplace);

    ddr_sampledev_matrix_concat (dde_sampledev_drom_data.matrix_stack[index].nrmlcstowcsmat,
				dde_sampledev_drom_data.nrmlcstowcsmat, 
				DcReplace);
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_update_lcstowcsmat_pop ()
{
    DtInt index;

    if (dde_sampledev_drom_data.num_matrices > 0) {
	index = --dde_sampledev_drom_data.num_matrices;
    
	ddr_sampledev_matrix_concat (dde_sampledev_drom_data.lcstowcsmat,
				    dde_sampledev_drom_data.matrix_stack[index].lcstowcsmat,
				    DcReplace);
	
	ddr_sampledev_matrix_concat (dde_sampledev_drom_data.lcstofcsmat,
				    dde_sampledev_drom_data.matrix_stack[index].lcstofcsmat,
				    DcReplace);
			 
	ddr_sampledev_matrix_concat (dde_sampledev_drom_data.nrmlcstowcsmat,
				    dde_sampledev_drom_data.matrix_stack[index].nrmlcstowcsmat,
				    DcReplace);
    }
}
/*
 ======================================================================
 */
void 
ddr_sampledev_drom_update_matrices(matrix,invertmatrix,concat)
DtMatrix4x4 matrix;
DtMatrix4x4 invertmatrix;
DtCompType concat;
{
    DtMatrix4x4 tinverse;
    int i, j;

    for (i=0; i<4; i++) {
	for (j=0; j<4; j++) {
	    tinverse[j][i] = invertmatrix[i][j];
	}
    }

    switch (concat) {
    case DcPreConcatenate:
	ddr_sampledev_matrix_concat (dde_sampledev_drom_data.lcstowcsmat, matrix,
				    DcPreConcatenate);
	ddr_sampledev_matrix_concat (dde_sampledev_drom_data.lcstofcsmat, matrix,
				    DcPreConcatenate);
	ddr_sampledev_matrix_concat (dde_sampledev_drom_data.nrmlcstowcsmat, tinverse,
				    DcPreConcatenate);
	break;

    case DcPostConcatenate:
	ddr_sampledev_matrix_concat (dde_sampledev_drom_data.lcstowcsmat, matrix,
				    DcPostConcatenate);
	ddr_sampledev_matrix_concat (dde_sampledev_drom_data.nrmlcstowcsmat, tinverse,
				    DcPostConcatenate);

	/*
	 * For post concatenation of the lcswofcsmat matrix (it contains
	 * the projection matrix) the light matrix (has just the lcstowcs)
	 * is copied to the object matrix and then the world to frustum matrix
	 * is post concatenated to this matrix
	 */

	ddr_sampledev_matrix_concat (dde_sampledev_drom_data.lcstofcsmat, 
				    dde_sampledev_drom_data.lcstowcsmat,
				    DcReplace);
	ddr_sampledev_matrix_concat (dde_sampledev_drom_data.lcstofcsmat,
				    dde_sampledev_drom_curcamdat->wcstofcsmat,
				    DcPostConcatenate);
	break;

    case DcReplace:
	ddr_sampledev_matrix_concat (dde_sampledev_drom_data.lcstowcsmat, matrix,
				    DcReplace);

	ddr_sampledev_matrix_concat (dde_sampledev_drom_data.nrmlcstowcsmat, tinverse,
				    DcReplace);

	/*
	 * For replacement of the lcstofcsmat matrix (it contains the 
	 * projection matrix) the world to frustum matrix is copied into 
	 * the lcstofcsmat and the new matrix will be pre concatenated to
	 * the wcstofcsmat.
	 */

	ddr_sampledev_matrix_concat (dde_sampledev_drom_data.lcstofcsmat, 
				    dde_sampledev_drom_curcamdat->wcstofcsmat,
				    DcReplace);
	ddr_sampledev_matrix_concat (dde_sampledev_drom_data.lcstofcsmat, matrix,
				    DcPreConcatenate);
	break;
    }
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_get_wcstofcsmat(wcstofcsmat)
DtMatrix4x4 wcstofcsmat;
{
    ddr_sampledev_matrix_concat (wcstofcsmat, 
				dde_sampledev_drom_curcamdat->wcstofcsmat,
				DcReplace);
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_push_lcstofcsmat(newmatrix)
DtMatrix4x4 newmatrix;
{
    ddr_sampledev_matrix_concat (dde_sampledev_save_lcstofcsmat, 
				dde_sampledev_drom_data.lcstofcsmat,
				DcReplace);

    ddr_sampledev_matrix_concat (dde_sampledev_drom_data.lcstofcsmat,
				newmatrix, DcReplace);

    dde_sampledev_save_shader = dde_sampledev_drom_data.srfshd;

    /*
     * Set the shading to the Constant shader.  Since we are 
     * replacing the lcstofcsmat matrix the lighting and normal transforms
     * may not be valid, so turn off light source shading
     */

    ddr_sampledev_drom_set_att_srfshd (DcShaderConstant);
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_pop_lcstofcsmat()
{
    ddr_sampledev_matrix_concat (dde_sampledev_drom_data.lcstofcsmat,
				dde_sampledev_save_lcstofcsmat,
				DcReplace);

    /*
     * restore the shader that was saved with the pop
     */

    ddr_sampledev_drom_set_att_srfshd (dde_sampledev_save_shader);
}
/*
 ======================================================================
 */
DtFlag
ddr_sampledev_drom_transform_clip_z_point(point,tfmmatrix)
DtRealTriple point;
DtMatrix4x4 tfmmatrix;
{
    DtInt row, column;
    DtReal fcspoint[4];
    DtReal oneoverw;
    DtRealTriple tfmpoint;

    /*
     * Transform the point from local coordinates to frustrum space coordinates
     * check for outside Z clipping range if so return DcTrue
     * set tfmmatrix to be the composite matrix of 
     * translation of the transformed point pre concatenated with a
     * scale matrix of the annotation scale for the current device
     * return DcFalse
     */

    /*  Transform the point.  */

    for (row=0;row<4;row++) {
	fcspoint[row] = dde_sampledev_drom_data.lcstofcsmat[row][3];

	for (column=0;column<3;column++) {
	    fcspoint[row] += dde_sampledev_drom_data.lcstofcsmat[row][column] * 
		    point[column];
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

    ddr_sampledev_matrix_translate(tfmmatrix, tfmpoint[0],tfmpoint[1],tfmpoint[2],
				  DcReplace);

    ddr_sampledev_matrix_scale(tfmmatrix,
			      dde_sampledev_drom_curdevdat->annoscale[0],
			      dde_sampledev_drom_curdevdat->annoscale[1],
			      dde_sampledev_drom_curdevdat->annoscale[2],
			      DcPreConcatenate);

    return(DcFalse);
}
/*
 ======================================================================
 */
