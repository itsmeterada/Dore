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
	void ddr_vll_drom_get_lcstowcsmat (matrix)
	void ddr_vll_drom_update_lcstowcsmat_lokatfrm (at, from, up, concat)
	void ddr_vll_drom_update_lcstowcsmat_rotate (axis, radians, concat)
	void ddr_vll_drom_update_lcstowcsmat_scale (sx, sy, sz, concat)
	void ddr_vll_drom_update_lcstowcsmat_shear (plane, firstshearvalue, secondshearvalue, concat)
	void ddr_vll_drom_update_lcstowcsmat_tfmmat (tfmmat, concat)
	void ddr_vll_drom_update_lcstowcsmat_transl (tx, ty, tz, concat)
	void ddr_vll_drom_update_lcstowcsmat_push ()
	void ddr_vll_drom_update_lcstowcsmat_pop ()
	void ddr_vll_drom_update_matrices (matrix,invertmatrix,concat)
	void ddr_vll_drom_get_wcstofcsmat (wcstofcsmat)
	void ddr_vll_drom_push_lcstofcsmat (newmatrix)
	void ddr_vll_drom_pop_lcstofcsmat ()
	DtFlag ddr_vll_drom_transform_clip_z_point (point,tfmmatrix)

  ======================================================================
 */
#include "drom.h"

/*
 ======================================================================
 */
PRAGMA(OPT_LEVEL 2)
void
ddr_vll_drom_get_lcstowcsmat (matrix)
	DtMatrix4x4 matrix;
{
    if (sizeof(float) != sizeof(DtReal)) {
	float mat[4][4];
	int i, j;

	VLLquery_matrix(VLL_LIGHT_MATRIX, mat);

	for (i=0; i<4; i++) {
PRAGMA(IVDEP)
	    for (j=0; j<4; j++) {
		matrix[i][j] = mat[i][j];
	    }
	}
    } else {
	VLLquery_matrix(VLL_LIGHT_MATRIX, matrix);
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_update_lcstowcsmat_lokatfrm (at, from, up, concat)
	DtPoint3 at;
	DtPoint3 from;
	DtVector3 up;
	DtCompType concat;
{
    DtMatrix4x4 matrix, invertmatrix;

    dor_matrix_load_look_at_from(matrix,
				 at[0],at[1],at[2],
				 from[0],from[1],from[2],
				 up[0],up[1],up[2]);

    dor_matrix_load (invertmatrix, matrix);
    dor_matrix_invert (invertmatrix);

    if (sizeof(float) != sizeof(DtReal)) {
	float mat[4][4], imat[4][4];
	int i, j;

	for (i=0; i<4; i++) {
	    for (j=0; j<4; j++) {
		mat[i][j] = matrix[i][j];
		imat[i][j] = invertmatrix[i][j];
	    }
	}
	ddr_vll_drom_update_matrices (mat, imat, concat);
    } else {
	ddr_vll_drom_update_matrices (matrix, invertmatrix, concat);
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_update_lcstowcsmat_rotate (axis, radians, concat)
	DtAxis axis;
	DtReal radians;
	DtCompType concat;
{
    static float xmatrix[4][4] = { 1., 0., 0., 0., 
				   0., 1., 0., 0.,
				   0., 0., 1., 0., 
				   0., 0., 0., 1.};
    static float ymatrix[4][4] = { 1., 0., 0., 0., 
				   0., 1., 0., 0.,
				   0., 0., 1., 0., 
				   0., 0., 0., 1.};
    static float zmatrix[4][4] = { 1., 0., 0., 0., 
				   0., 1., 0., 0.,
				   0., 0., 1., 0., 
				   0., 0., 0., 1.};
    static float xinvertmatrix[4][4] = { 1., 0., 0., 0., 
					 0., 1., 0., 0.,
					 0., 0., 1., 0., 
					 0., 0., 0., 1.};
    static float yinvertmatrix[4][4] = { 1., 0., 0., 0., 
					 0., 1., 0., 0.,
					 0., 0., 1., 0., 
					 0., 0., 0., 1.};
    static float zinvertmatrix[4][4] = { 1., 0., 0., 0., 
					 0., 1., 0., 0.,
					 0., 0., 1., 0., 
					 0., 0., 0., 1.};
    DtReal dor_math_cos();
    DtReal dor_math_sin();
    float cosine, ncosine;
    float sine, nsine;

    cosine  = (float)dor_math_cos(radians);
    sine    = (float)dor_math_sin(radians);
    ncosine = (float)dor_math_cos(-radians);
    nsine   = (float)dor_math_sin(-radians);

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

	ddr_vll_drom_update_matrices (xmatrix, xinvertmatrix, concat);
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

	ddr_vll_drom_update_matrices (ymatrix, yinvertmatrix, concat);
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

	ddr_vll_drom_update_matrices (zmatrix, zinvertmatrix, concat);
	break;
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_update_lcstowcsmat_scale (sx, sy, sz, concat)
	DtReal sx;
	DtReal sy;
	DtReal sz;
	DtCompType concat;
{
    static float matrix[4][4] = { 1., 0., 0., 0., 
				  0., 1., 0., 0.,
				  0., 0., 1., 0., 
				  0., 0., 0., 1.};
    static float invertmatrix[4][4] = { 1., 0., 0., 0., 
					0., 1., 0., 0.,
					0., 0., 1., 0., 
					0., 0., 0., 1.};

    matrix[0][0] = sx;
    matrix[1][1] = sy;
    matrix[2][2] = sz;

    invertmatrix[0][0] = 1./sx;
    invertmatrix[1][1] = 1./sy;
    invertmatrix[2][2] = 1./sz;

    ddr_vll_drom_update_matrices (matrix, invertmatrix, concat);
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_update_lcstowcsmat_shear (plane, firstshearvalue, 
					 secondshearvalue, concat)
	DtMajorPlane plane;
	DtReal firstshearvalue;
	DtReal secondshearvalue;
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
      matrix[1][0] = firstshearvalue;
      matrix[2][0] = secondshearvalue;

      invertmatrix[1][0] = -firstshearvalue;
      invertmatrix[2][0] = -secondshearvalue;

      break;

   case DcXZ:
      matrix[0][1] = firstshearvalue;
      matrix[2][1] = secondshearvalue;

      invertmatrix[0][1] = -firstshearvalue;
      invertmatrix[2][1] = -secondshearvalue;

      break;

   case DcXY:
      matrix[0][2] = firstshearvalue;
      matrix[1][2] = secondshearvalue;

      invertmatrix[0][2] = -firstshearvalue;
      invertmatrix[1][2] = -secondshearvalue;

      break;
   }

   dor_matrix_load (invertmatrix, matrix);
   dor_matrix_invert (invertmatrix);

   if (sizeof(float) != sizeof(DtReal)) {
       float mat[4][4], imat[4][4];
       int i, j;

       for (i=0; i<4; i++) {
	   for (j=0; j<4; j++) {
	       mat[i][j] = matrix[i][j];
	       imat[i][j] = invertmatrix[i][j];
	   }
       }
       ddr_vll_drom_update_matrices (mat, imat, concat);
   } else {
       ddr_vll_drom_update_matrices (matrix, invertmatrix, concat);
   }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_update_lcstowcsmat_tfmmat (tfmmat, concat)
	DtMatrix4x4 tfmmat;
	DtCompType concat;
{
    static DtMatrix4x4 invertmatrix;
    static DtMatrix4x4 matrix;

    dor_matrix_load(matrix, tfmmat);

    dor_matrix_load(invertmatrix,matrix);
    dor_matrix_invert(invertmatrix);

    if (sizeof(float) != sizeof(DtReal)) {
	float mat[4][4], imat[4][4];
	int i, j;

	for (i=0; i<4; i++) {
	    for (j=0; j<4; j++) {
		mat[i][j] = matrix[i][j];
		imat[i][j] = invertmatrix[i][j];
	    }
	}
	ddr_vll_drom_update_matrices (mat, imat, concat);
    } else {
	ddr_vll_drom_update_matrices (matrix, invertmatrix, concat);
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_update_lcstowcsmat_transl (tx, ty, tz, concat)
	DtReal tx;
	DtReal ty;
	DtReal tz;
	DtCompType concat;
{
    static float matrix[4][4] = { 1., 0., 0., 0., 
				  0., 1., 0., 0.,
				  0., 0., 1., 0., 
				  0., 0., 0., 1.};
    static float invertmatrix[4][4] = { 1., 0., 0., 0., 
					0., 1., 0., 0.,
					0., 0., 1., 0., 
					0., 0., 0., 1.};

    matrix[0][3] = tx;
    matrix[1][3] = ty;
    matrix[2][3] = tz;

    invertmatrix[0][3] = -tx;
    invertmatrix[1][3] = -ty;
    invertmatrix[2][3] = -tz;

    ddr_vll_drom_update_matrices (matrix, invertmatrix, concat);
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_update_lcstowcsmat_push ()
{
    ddt_vll_drom_matrix_stack *ms;
    DtMatrix4x4 matrix;

    ms = &(dde_vll_drom_data.matrix_stack);

    if (ms->current_element == ms->last_element)	{
	ms->elements = (ddt_vll_drom_matrix_element *)DDspace_Reallocate
		(ms->elements,((ms->last_element+1)+10)*
		 (sizeof (ddt_vll_drom_matrix_element)));
      
	ms->last_element += 10;
    }
		
    ms->current_element++;

    VLLquery_matrix(VLL_OBJECT_MATRIX,
		    ms->elements[ms->current_element].object_matrix);

    VLLquery_matrix(VLL_LIGHT_MATRIX,
		    ms->elements[ms->current_element].light_matrix);

    VLLquery_matrix(VLL_NORMAL_MATRIX,
		    ms->elements[ms->current_element].normal_matrix);
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_update_lcstowcsmat_pop ()
{
    ddt_vll_drom_matrix_stack *ms;

    ms = &(dde_vll_drom_data.matrix_stack);

    if (ms->current_element >= 0) {
	VLLset_matrix(VLL_OBJECT_MATRIX,
		      ms->elements[ms->current_element].object_matrix);

	VLLset_matrix(VLL_LIGHT_MATRIX,
		      ms->elements[ms->current_element].light_matrix);

	VLLset_matrix(VLL_NORMAL_MATRIX,
		      ms->elements[ms->current_element].normal_matrix);
    }
    ms->current_element--;
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_update_matrices(matrix,invertmatrix,concat)
	float matrix[4][4];
	float invertmatrix[4][4];
	DtCompType concat;
{
    float tinverse[4][4];
    int i, j;

    for (i=0; i<4; i++) {
	for (j=0; j<4; j++) {
	    tinverse[j][i] = invertmatrix[i][j];
	}
    }

    switch (concat) {
    case DcPreConcatenate:
	VLLpre_concat_matrix(VLL_OBJECT_MATRIX, matrix);
	VLLpre_concat_matrix(VLL_LIGHT_MATRIX, matrix);
	VLLpre_concat_matrix(VLL_NORMAL_MATRIX, tinverse);
	break;

    case DcPostConcatenate:
	VLLpost_concat_matrix(VLL_LIGHT_MATRIX, matrix);
	VLLpost_concat_matrix(VLL_NORMAL_MATRIX, tinverse);
	/*
	 * For post concatenation of of the object matrix (it contains
	 * the projection matrix) the light matrix (has just the lcstowcs)
	 * is copied to the object matrix and then the world to frustum matrix
	 * is post concatenated to this matrix
	 */
	VLLcopy_matrix(VLL_OBJECT_MATRIX, VLL_LIGHT_MATRIX);
	if (sizeof(float) != sizeof(DtReal)) {
	    float mat[4][4];

	    for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
		    mat[i][j] = 
			    dde_vll_drom_curcamdat->world_to_frustum_matrix[i][j];
		}
	    }
	    VLLpost_concat_matrix(VLL_OBJECT_MATRIX, mat);
	} else {
	    VLLpost_concat_matrix(VLL_OBJECT_MATRIX, 
				  dde_vll_drom_curcamdat->world_to_frustum_matrix);
	}
	break;

    case DcReplace:
	if (sizeof(float) != sizeof(DtReal)) {
	    float mat[4][4];

	    for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
		    mat[i][j] = 
			    dde_vll_drom_curcamdat->world_to_frustum_matrix[i][j];
		}
	    }
	    VLLset_matrix(VLL_OBJECT_MATRIX, mat);
	} else {
	    VLLset_matrix(VLL_OBJECT_MATRIX,
			  dde_vll_drom_curcamdat->world_to_frustum_matrix);
	}
	VLLpre_concat_matrix(VLL_OBJECT_MATRIX, matrix);
	VLLset_matrix(VLL_LIGHT_MATRIX, matrix);
	VLLset_matrix(VLL_NORMAL_MATRIX, tinverse);
	break;
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_get_wcstofcsmat(wcstofcsmat)
	DtMatrix4x4 wcstofcsmat;
{
    dor_matrix_load (wcstofcsmat, dde_vll_drom_curcamdat->world_to_frustum_matrix);
}
/*
 ======================================================================
 */
float dde_vll_lcstofcsmat_temp[4][4];

void
ddr_vll_drom_push_lcstofcsmat(newmatrix)
	DtMatrix4x4 newmatrix;
{
    VLLquery_matrix(VLL_OBJECT_MATRIX, dde_vll_lcstofcsmat_temp);

    if (sizeof(float) != sizeof(DtReal)) {
	int i, j;
	float mat[4][4];

	for (i=0; i<4; i++) {
	    for (j=0; j<4; j++) {
		mat[i][j] = newmatrix[i][j];
	    }
	}
	VLLset_matrix(VLL_OBJECT_MATRIX, mat);
    } else {
	VLLset_matrix(VLL_OBJECT_MATRIX, newmatrix);
    }

    /* turn off light source shading */

    dde_vll_drom_data.constant_shading_switch = DcTrue;

    ddr_vll_drom_update_surface_properties();

}
/*
 ======================================================================
 */
void 
ddr_vll_drom_pop_lcstofcsmat()
{
    VLLset_matrix(VLL_OBJECT_MATRIX, dde_vll_lcstofcsmat_temp);

    /* reset light source shading to what is was before push */

    dde_vll_drom_data.constant_shading_switch = 
	    (dde_vll_drom_data.surface_shader == DcShaderConstant);

    ddr_vll_drom_update_surface_properties();
}
/*
 ======================================================================
 */
DtFlag
ddr_vll_drom_transform_clip_z_point(point,tfmmatrix)
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
    DtInt row, column;
    DtReal fcspoint[4];
    DtReal oneoverw;
    DtRealTriple tfmpoint;

    /*  Transform the point.  */

    if (sizeof(float) != sizeof(DtReal)) {
	int i, j;
	float mat[4][4];

	VLLquery_matrix(VLL_OBJECT_MATRIX, mat);
	for (i=0; i<4; i++) {
	    for (j=0; j<4; j++) {
		objectmatrix[i][j] = mat[i][j];
	    }
	}
    } else {
	VLLquery_matrix (VLL_OBJECT_MATRIX, objectmatrix);
    }

    for (row=0;row<4;row++) {
	fcspoint[row] = objectmatrix[row][3];

	for (column=0;column<3;column++) {
	    fcspoint[row] += objectmatrix[row][column] * point[column];
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
		     dde_vll_drom_curdevdat->annoscale[0],
		     dde_vll_drom_curdevdat->annoscale[1],
		     dde_vll_drom_curdevdat->annoscale[2],
		     DcPreConcatenate);

    return(DcFalse);
}
/*
 ======================================================================
 */
