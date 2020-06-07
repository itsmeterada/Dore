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
	void ddr_sampledev_matrix_concat (matrix, concatmat, compose_type)
	void ddr_sampledev_matrix_at_from (matrix,originx,originy,originz,tox,toy,toz, upx,upy,upz,compose_type)
	void ddr_sampledev_matrix_parallel (matrix, windowsize, hither, yon, compose_type)
	void ddr_sampledev_matrix_perspective (matrix,fovdegree,hither,yon, compose_type)
	void ddr_sampledev_matrix_projection (matrix,window,ptype,prp,view_plane,hither,yon, compose_type)
	DtShort ddr_sampledev_matrix_solve (a,c,b)
	void ddr_sampledev_matrix_invert (matrix)
	void ddr_sampledev_matrix_shear_z(matrix, a, b, compose_type)
	void ddr_sampledev_matrix_translate(matrix, tx, ty, tz, compose_type)
	void ddr_sampledev_matrix_scale(matrix, sx, sy, sz, compose_type)
  ======================================================================
 */
#include <dore/dore.h>
#include <math.h>
#include <dore/dore_develop/error.h>
/*
 ======================================================================
 */
static DtMatrix4x4 dde_sampledev_identmatrix = {1.0, 0.0, 0.0, 0.0,
					       0.0, 1.0, 0.0, 0.0,
					       0.0, 0.0, 1.0, 0.0,
					       0.0, 0.0, 0.0, 1.0 };

void ddr_sampledev_matrix_shear_z();
void ddr_sampledev_matrix_translate();
void ddr_sampledev_matrix_scale();
/*
 ======================================================================
 */
void
ddr_sampledev_matrix_concat(matrix, concatmat, compose_type)
DtMatrix4x4 matrix;
DtMatrix4x4 concatmat;
DtFlag compose_type;
{
    DtMatrix4x4 tempmat;
    DtInt row, column;
    DtInt matindex;


    switch (compose_type){
    case DcPostConcatenate:
	/*  Pre-concatenate transformation represented by concatmatrix with 
	 *  transformation represented by matrix. Since we are using column 
	 *  vectors this is a matrix post-multiply!  
	 */

	for (row=0;row<4;row++){
	    for (column=0;column<4;column++){
		tempmat[row][column] = 0.;
		for (matindex=0;matindex<4;matindex++){
		    tempmat[row][column] = tempmat[row][column] +
			    matrix[row][matindex]*concatmat[matindex][column];
		}
	    }
	}
	for (row=0;row<4;row++)
		for (column=0;column<4;column++)
			matrix[row][column] = tempmat[row][column];
	break ;

    case DcPreConcatenate:
	/*  Pre-concatenate transformation represented by concatmatrix with 
	 *  transformation represented by matrix. Since we are using column 
	 *  vectors this is a matrix post-multiply!  
	 */

	for (row=0;row<4;row++){
	    for (column=0;column<4;column++){
		tempmat[row][column] = 0.;

		for (matindex=0;matindex<4;matindex++){
		    tempmat[row][column] = tempmat[row][column] +
			    matrix[row][matindex]*concatmat[matindex][column];
		}
	    }
	}
	for (row=0;row<4;row++)
		for (column=0;column<4;column++)
			matrix[row][column] = tempmat[row][column];
	break;

    case DcReplace:
	for (row=0;row<4;row++)
		for (column=0;column<4;column++)
			matrix[row][column] = concatmat[row][column];
	break ;
    }
}
/*
 ======================================================================
 */
void
ddr_sampledev_matrix_look_at_from(matrix,at,from,up)
DtMatrix4x4 matrix;
DtPoint3 at;
DtPoint3 from;
DtVector3 up;
{
    static char *proc_name = "ddr_sampledev_matrix_look_at_from";
    DtReal u[3], v[3], n[3];

    ddr_sampledev_matrix_concat(matrix, dde_sampledev_identmatrix, DcReplace);

    v[0] = up[0];
    v[1] = up[1];
    v[2] = up[2];

    if ( !ddr_sampledev_math_renormalize(v) ){
	DDerror (ERR_DEGEN_LOKATFRM, proc_name, "null up vector");
	return;
    }

    n[0] = from[0]-at[0];
    n[1] = from[1]-at[1];
    n[2] = from[2]-at[2];

    if (!ddr_sampledev_math_renormalize(n)){
	DDerror (ERR_DEGEN_LOKATFRM, proc_name, "up and from are coincident");
	return;
    }

    ddr_sampledev_math_crossproduct(v,n,u);

    if (!ddr_sampledev_math_renormalize(u)){
	DDerror (ERR_DEGEN_LOKATFRM, proc_name, "view normal and up vector are parallel");
	return;
    }

    ddr_sampledev_math_crossproduct(n,u,v);

    if (!ddr_sampledev_math_renormalize(v)){
	DDerror (ERR_DEGEN_LOKATFRM, proc_name, "view normal and view u vector are parallel");
	return;
    }

    matrix[0][0] = u[0];
    matrix[1][0] = u[1];
    matrix[2][0] = u[2];

    matrix[0][1] = v[0];
    matrix[1][1] = v[1];
    matrix[2][1] = v[2];

    matrix[0][2] = n[0];
    matrix[1][2] = n[1];
    matrix[2][2] = n[2];

    matrix[0][3] = from[0];
    matrix[1][3] = from[1];
    matrix[2][3] = from[2];
}
/*
 ======================================================================
  * * * *  ddr_sampledev_matrix_parallel  * * * * * 
  Construct a parallel transformation
 
 		-a <= x <= a, -a <= y <= a, yon <= z <= hither
 
  	where a = window size/2 into volume 
 
 		-w <= x <= w, -w <= y <= w, -w <= z <= 0 
 		(These conditions imply w>=0.)
 ======================================================================
 */
void
ddr_sampledev_matrix_parallel(matrix, windowsize, hither, yon, compose_type)
DtMatrix4x4 matrix;
DtReal windowsize, hither, yon;
DtFlag compose_type;
{
    DtMatrix4x4 tmpmatrix;
    DtReal a, xyscale, zscale;

    ddr_sampledev_matrix_translate(tmpmatrix, 0, 0, -hither, DcReplace);

    a = windowsize/2.0;
    xyscale = 1.0/a;
    zscale = 1.0/(hither-yon);

    ddr_sampledev_matrix_scale(tmpmatrix,xyscale,xyscale,zscale,DcPostConcatenate);

    ddr_sampledev_matrix_concat (matrix, tmpmatrix, compose_type);
}
/*
 ======================================================================
  * * * *  ddr_sampledev_matrix_perspective  * * * * *
  Construct a perspective transformation
 	fovdegree = viewing fovdegree
 	h = hither distance, oneoveryon = 1/yon	!!!! both always negative  !!!!
 
  This assumes viewing direction is in -z direction, with eye at
  z=0, right-handed eye coordinate system.  Will map valid coordinates
  (those inside viewing fovdegree and with yon <= z <= hither < 0) into the 
  ranges: -w<=x<=w  -w<=y<=w  -w<=z<=0 (These conditions imply w>=0.)
 ======================================================================
 */
void
ddr_sampledev_matrix_perspective(matrix,fovdegree,hither,yon, compose_type)
DtMatrix4x4 matrix;
DtReal fovdegree, hither, yon;
DtFlag compose_type;
{
    DtMatrix4x4 tmpmatrix;
    DtReal ang;
    DtReal oneoveryon;
    DtReal a, b, c;

    ddr_sampledev_matrix_concat(tmpmatrix, dde_sampledev_identmatrix, 
			       DcReplace);

    ang = fovdegree * (0.5 * M_PI/180.0);

    oneoveryon = 1. / yon;

    c = 1.0 / tan(ang);		/* cotangent half-fovdegree */
    a = 1.0 / (1.0-hither*oneoveryon);
    b = - hither * a;

    tmpmatrix[0][0] = c;
    tmpmatrix[1][1] = c;
    tmpmatrix[2][2] = a;
    tmpmatrix[2][3] = b;

    tmpmatrix[3][2] = -1.0;
    tmpmatrix[3][3] = 0.0;

    ddr_sampledev_matrix_concat(matrix, tmpmatrix, compose_type);
}
/*
 ======================================================================
  * * * *  ddr_sampledev_matrix_projection  * * * * * 
  Construct a general perspective or parallel transformation
  window and viewport should have the same xy aspect ratio to avoid 
  distortion.  This assumes viewing direction is in -z direction, 
  with eye at the prp and a right-handed eye coordinate system.  
  Points inside the viewing volume are mapped into the volume specified by: 
  -w<=x<=w  -w<=y<=w, -w<=z<=0 (These conditions imply w>=0.)
  For perspective transformations the z component of the prp must be 
  greater than view_plane: Note that view_plane, hither and yon are all 
  z coordinates in the camera (view reference) coordinate system and not 
  distances (as in PHIGS).  This means they are typically negative values
 ======================================================================
 */
void
ddr_sampledev_matrix_projection(matrix,window,ptype,prp,view_plane,hither,yon, 
			       compose_type) 
DtMatrix4x4 matrix;
DtArea *window;
DtProjectionType ptype; 
DtPoint3 prp;
DtReal view_plane;
DtReal hither;
DtReal yon;
DtFlag compose_type;
{
    DtReal fur[4];
    DtMatrix4x4 tempmat, matrix1;
    DtReal vp, xshear, yshear;
    DtReal vec[4];

    /* translate projection reference point to the orign */

    ddr_sampledev_matrix_translate (matrix1, -prp[0], -prp[1], -prp[2], DcReplace);

    vp = prp[2]-view_plane;

    /* compute shear to map window centre to N axis */

    xshear= -(0.5*(window->ll[0]+window->ur[0])-prp[0])/vp;
    yshear = -(0.5*(window->ll[1]+window->ur[1])-prp[1])/vp;

    ddr_sampledev_matrix_shear_z(matrix1,xshear,yshear,DcPostConcatenate);

    /* compute transformed window size */

    vec[0] = window->ur[0];
    vec[1] = window->ur[1];
    vec[2] = view_plane;
    vec[3] = 1.;
    ddr_sampledev_math_pnttrns (vec, matrix1, fur);
    if (fur[3] != 0.0 && fur[3] != 1.0) {
	fur[0] /= fur[3];
	fur[1] /= fur[3];
	fur[2] /= fur[3];
    }

    ddr_sampledev_matrix_concat(tempmat, dde_sampledev_identmatrix, DcReplace);

    hither = hither-prp[2];
    yon = yon-prp[2];

    if ( ptype == DcPerspective ){
	tempmat[0][0] = vp/fur[0];
	tempmat[1][1] = vp/fur[1];
	tempmat[2][2] = 1.0 / (1.0-(hither/yon));
	tempmat[2][3] = - hither * tempmat[2][2];
	tempmat[3][2] = -1.0;	/* w' = -z divide (for z divide) */
	tempmat[3][3] = 0.0;
    }
    else {
	tempmat[0][0] = 1.0/fur[0];
	tempmat[1][1] = 1.0/fur[1];
	tempmat[2][2] = 1/(hither-yon);
	tempmat[2][3] = - hither * tempmat[2][2];
    }

    ddr_sampledev_matrix_concat (matrix1,tempmat, DcPostConcatenate);

    ddr_sampledev_matrix_concat (matrix, matrix1, compose_type);
}
/*
 ======================================================================
    Computes an exact solution for the matrix equation:

	a * c = b

    where 'c' is the solution matrix
			in place operation is allowed
    Restrictions:	arnum = acnum = brnum  <= 6 = 16
			6 -- maximum dimension length of all matrices
 ======================================================================
 */
#define	arnum	4
#define acnum	4
#define	brnum	4
#define	bcnum	4

DtShort
ddr_sampledev_matrix_solve(a,c,b)
DtReal a[arnum][acnum];
DtReal c[brnum][bcnum];
DtReal b[acnum][bcnum];
{
    int abcnum, i, ij, j, p[6], actrow, tmprow, vrtrow;

    /*
     * abcnum - brnum -- number of rows and columns in the
     * respective matrices
     * i -- row matindex
     * ij -- matrix 'ab' diagonal matindex
     * j -- column matindex
     * p -- "permutation" array which keeps track of row re-matindexing
     * due to partial pivoting.  the i'th element of 'p'
     * indicates which actual 'ab' row belongs in the i'th
     * virtual (re-matindexed due to pivoting) row of 'ab'.
     * actrow -- actual 'ab' row number of new pivot
     * tmprow -- temporary 'ab' row number
     * vrtrow -- virtual 'ab' row number of the new pivot
     * 
     * completion status:
     * 10 = good matrix solution.
     *  1 = possibly bad solution.
     *  0 = routine in progress.
     * -1 = bad dimensions.
     * -2 = bad solution, zero pivot.
     * 
     * 
     * DtReal a(arnum,acnum), b(brnum,bcnum), c(acnum,bcnum)
     * 
     * a -- left side input matrix
     * b -- right side input matrix
     * c -- solution to the matrix equation
     */
    DtReal ab[6][12], pivot, rnorm;
    DtReal ptemp;
    /*
     * ab -- local scratch array containing concatenated copies
     * of 'a' and 'b'
     * pivot -- the new pivot value
     * rnorm -- row normalization factor
     * tpivot -- temporary pivot value
     */
    /*
     * check dimensions of matrices
     */
    if (!((arnum == acnum) && (arnum == brnum) && (arnum <= 6)))
	    return(-1);

    abcnum = acnum + bcnum;
    for (i=0;i<arnum;i++)
	    p[i] = i;

    for (i=0;i<arnum;i++){
	for (j=0;j<acnum;j++)
		ab[i][j] = a[i][j];
	for (j=0;j<bcnum;j++)
		ab[i][j+acnum] = b[i][j];
    }

    /*
     * perform gauss-jordan elimination with partial pivoting of the rows
     */
    for (ij=0;ij<arnum;ij++){

	/*
	 * determine the pivot (the element with the largest absolute
	 * value) for the current virtual column
	 */
	pivot = 0.0;
	for (i=ij;i<arnum;i++){
	    ptemp = fabs(ab[p[i]][ij]);
	    if (ptemp <= pivot)
		    continue;
	    pivot = ptemp;
	    vrtrow = i;
	}
	/*
	 * check for zero pivot
	 */
	if (pivot == 0.0)
		return(-2);
	/*
	 * get the actual pivot value (not the absolute value)
	 * note: the pivots are a good indicator for "bad" inversions
	 */
	pivot = ab[p[vrtrow]][ij];
	/*
	 * update the permutation array by swapping the current ij'th 
	 * virtual row with the virtual row of the new pivot
	 */
	tmprow = p[ij];
	p[ij] = p[vrtrow];
	p[vrtrow] = tmprow;
	/*
	 * get the actual 'ab' row of the new pivot (which is now
	 * at virtual row 'ij')
	 */
	actrow = p[ij];
	/*
	 * divide pivot row by the pivot value (note: the lower left
	 * triangle of 'ab' can be ignored)
	 */
	for (j=ij+1;j<abcnum;j++)
		ab[actrow][j] = ab[actrow][j] / pivot;
	/*
	 * zero out the current column except for the actual pivot row 
	 */
	for (i=0;i<arnum;i++){
	    /*
	     * check for the pivot row
	     */
	    if (i != actrow){
		/*
		 * normalize the pivot row and subtract it from
		 * the current row
		 */
		rnorm = ab[i][ij];
		if (rnorm != 0.0){
		    for (j=ij+1;j<abcnum;j++)
			    ab[i][j] = ab[i][j] -
				    rnorm*ab[actrow][j];
		}
	    }
	}
    }
    /*
     * copy 'b' part of 'ab' into 'c' with re-matindexing via the
     * permutation array
     */
    for (i=0;i<arnum;i++){
	actrow = p[i];		
	for (j=0;j<bcnum;j++)
		c[i][j] = ab[actrow][acnum+j];
    }
    return(10);
}
/*
 ======================================================================
 */
void
ddr_sampledev_matrix_invert(matrix)
DtMatrix4x4 matrix;
{
    DtMatrix4x4 invertmatrix;
    DtMatrix4x4 identmatrix;
    DtInt status;

    ddr_sampledev_matrix_concat (identmatrix, dde_sampledev_identmatrix, DcReplace);

    status = ddr_sampledev_matrix_solve(matrix,invertmatrix,identmatrix);

    if (status != 10){
	sprintf(DDerror_GetLine(),
		"inversion status %d",
		status);
	DDerror (ERR_CANT_INVERT_MATRIX,
		    "ddr_sampledev_matrix_invert", DDerror_GetLine());
	return;
    }

    ddr_sampledev_matrix_concat(matrix, invertmatrix, DcReplace);
}
/*
 ======================================================================
 */
void
ddr_sampledev_matrix_shear_z(matrix, a, b, compose_type)
DtMatrix4x4 matrix;
DtReal a, b ;
DtFlag compose_type;
{
    static DtMatrix4x4 tmpmatrix = {1.0, 0.0, 0.0, 0.0,
				    0.0, 1.0, 0.0, 0.0,
				    0.0, 0.0, 1.0, 0.0,
				    0.0, 0.0, 0.0, 1.0 };

    tmpmatrix[0][2] = a;
    tmpmatrix[1][2] = b;
    ddr_sampledev_matrix_concat (matrix, tmpmatrix, compose_type);
}
/*
 ======================================================================
 */
void
ddr_sampledev_matrix_translate(matrix,transx,transy,transz, compose_type)
DtMatrix4x4 matrix;
DtReal transx, transy, transz;
DtFlag compose_type;
{
    static DtMatrix4x4 tmpmatrix = {1.0, 0.0, 0.0, 0.0,
				    0.0, 1.0, 0.0, 0.0,
				    0.0, 0.0, 1.0, 0.0,
				    0.0, 0.0, 0.0, 1.0 };

    tmpmatrix[0][3] = transx;
    tmpmatrix[1][3] = transy;
    tmpmatrix[2][3] = transz;
    ddr_sampledev_matrix_concat (matrix, tmpmatrix, compose_type);
}
/*
 ======================================================================
 */
void
ddr_sampledev_matrix_scale(matrix, scalex, scaley, scalez, compose_type)
DtMatrix4x4 matrix;
DtReal scalex, scaley, scalez;
DtFlag compose_type;
{
    static DtMatrix4x4 tmpmatrix = {1.0, 0.0, 0.0, 0.0,
				    0.0, 1.0, 0.0, 0.0,
				    0.0, 0.0, 1.0, 0.0,
				    0.0, 0.0, 0.0, 1.0 };

    tmpmatrix[0][0] = scalex;
    tmpmatrix[1][1] = scaley;
    tmpmatrix[2][2] = scalez;
    ddr_sampledev_matrix_concat (matrix, tmpmatrix, compose_type);
}
/*
 ======================================================================
 */
