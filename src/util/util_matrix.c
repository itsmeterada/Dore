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
 
#include <dore/internal/dogen.h>
#include <dore/internal/matrix.h>

    /* In order to tailor the matrix functions so that they execute quickly,
    ** two versions of several critical matrix routines have been written and
    ** are included in the files mat_titan.c and mat_other.c.  The routines
    ** in the mat_titan source file take advantage of compiler pragmas to use
    ** parallel code to load & manipulate matrix elements.  For other
    ** platforms, the best speed is obtained when these operations are
    ** ``unrolled'' without the overhead of two for loops.  Thus, the
    ** following conditional include switches on the target machine type.  */

#ifdef titan
#   include "mat_titan.c"
#else
#   include "mat_other.c"
#endif



/*****************************************************************************
**  This routine takes two matrices and composes them according to a given
**  composition operator.
*****************************************************************************/

void dor_matrix_concat (
    DtMatrix4x4 matrix,		/* Target/Result Matrix */
    DtMatrix4x4 concatmat,	/* Matrix to Concatenate to The Target */
    DtCompType  compose_type)	/* Composition Operator */
{
    switch (compose_type) {
	case DcPostConcatenate:
	    dor_matrix_post_concatenate (matrix,concatmat);
	    break;

	case DcPreConcatenate:
	    dor_matrix_pre_concatenate (matrix,concatmat);
	    break;

	case DcReplace:
	    dor_matrix_load (matrix,concatmat);
	    break;
    }
}

/* ====================================================================== */

DtPtr dor_matrix_create (void)
{
    register DtMatrix4x4 *matrix;

    matrix = dor_space_allocate (sizeof *matrix);

    dor_matrix_load_identity (*matrix);

    return matrix;
}

/* ====================================================================== */

void dor_matrix_destroy (DtMatrix4x4 matrix)
{
    if (matrix == NULL){
	DDerror (ERR_VALUE_OUT_OF_RANGE, "dor_matrix_destroy","matrix is NULL");
	return;
    }

    dor_space_deallocate (matrix);
}

/* ====================================================================== */

void dor_matrix_invert (DtMatrix4x4 matrix)
{
    auto DtMatrix4x4 invertmatrix;
    auto DtMatrix4x4 identmatrix;
    auto DtInt status;

    dor_matrix_load_identity (identmatrix);

    status = dor_matrix_solve (matrix, invertmatrix, identmatrix);

    if (status != 10) {
	sprintf (DDerror_GetLine(), "inversion status %d", status);
	DDerror (ERR_CANT_INVERT_MATRIX, "dor_matrix_invert",DDerror_GetLine());
	return;
    }

    dor_matrix_load (matrix, invertmatrix);
}



/*****************************************************************************
**  This routine concatenates a look-at-from viewing matrix to the matrix
**  specified by `matrix'.
*****************************************************************************/

void dor_matrix_look_at_from (
    DtMatrix4x4 matrix,
    DtReal atx,   DtReal aty,   DtReal atz,
    DtReal fromx, DtReal fromy, DtReal fromz,
    DtReal upx,   DtReal upy,   DtReal upz,
    DtCompType compose_type)
{
    auto DtMatrix4x4 tempmat;

    dor_matrix_load_look_at_from (tempmat, atx,aty,atz, fromx,fromy,fromz,
				  upx,upy,upz);

    dor_matrix_concat (matrix, tempmat, compose_type);
}

/* ====================================================================== */

void dor_matrix_load_look_at_from (
    DtMatrix4x4 matrix,
    DtReal atx,   DtReal aty,   DtReal atz,
    DtReal fromx, DtReal fromy, DtReal fromz,
    DtReal upx,   DtReal upy,   DtReal upz)
{
    static char      *proc_name = "dor_matrix_load_look_at_from";
    auto   DtVector3  u, v, n;

    v[0] = upx;
    v[1] = upy;
    v[2] = upz;

    if ( !dor_math_renormalize(&v[0], &v[1], &v[2] ) ){
	dor_matrix_load_identity (matrix);
	DDerror (ERR_DEGEN_LOKATFRM, proc_name, "null up vector");
	dor_matrix_load_identity (matrix);
	return;
    }

    n[0] = fromx-atx;
    n[1] = fromy-aty;
    n[2] = fromz-atz;

    if (!dor_math_renormalize(&n[0],&n[1],&n[2])){
	DDerror (ERR_DEGEN_LOKATFRM, proc_name, "up and from are coincident");
	dor_matrix_load_identity (matrix);
	return;
    }

    dor_math_crossproduct (v[0],v[1],v[2], n[0],n[1],n[2], &u[0],&u[1],&u[2]);

    if (!dor_math_renormalize(&u[0],&u[1],&u[2])){
	DDerror (ERR_DEGEN_LOKATFRM, proc_name,
	    "view normal and up vector are parallel");
	dor_matrix_load_identity (matrix);
	return;
    }

    dor_math_crossproduct(n[0],n[1],n[2], u[0],u[1],u[2], &v[0],&v[1],&v[2]);

    if (!dor_math_renormalize(&v[0],&v[1],&v[2])){
	DDerror (ERR_DEGEN_LOKATFRM, proc_name,
	    "view normal and view u vector are parallel");
	dor_matrix_load_identity (matrix);
	return;
    }

    matrix[0][0] = u[0];
    matrix[1][0] = u[1];
    matrix[2][0] = u[2];
    matrix[3][0] = 0.0;

    matrix[0][1] = v[0];
    matrix[1][1] = v[1];
    matrix[2][1] = v[2];
    matrix[3][1] = 0.0;

    matrix[0][2] = n[0];
    matrix[1][2] = n[1];
    matrix[2][2] = n[2];
    matrix[3][2] = 0.0;

    matrix[0][3] = fromx;
    matrix[1][3] = fromy;
    matrix[2][3] = fromz;
    matrix[3][3] = 1.0;
}



/*============================================================================
**  dor_matrix_load_parallel
**
**  Construct a parallel transformation
**
**	-a <= x <= a,  -a <= y <= a,  yon <= z <= hither
**
**  where a = window size/2 into volume 
**
**	-w <= x <= w, -w <= y <= w, -w <= z <= 0 (These conditions imply w>=0.)
============================================================================*/

void dor_matrix_load_parallel (
    DtMatrix4x4 matrix,
    DtReal      windowsize,
    DtReal      hither,
    DtReal      yon)
{
    DtReal a, xyscale, zscale;

    dor_matrix_load_identity (matrix);

    dor_matrix_load_translate (matrix, 0.0,0.0,-hither);

    a = windowsize / 2;

    xyscale = 1 / a;
    zscale = 1 / (hither-yon);

    dor_matrix_scale (matrix, xyscale,xyscale,zscale, DcPostConcatenate);
}



/*============================================================================
** dor_matrix_load_perspective
**
** Construct a perspective transformation
**	fovdegree = viewing fovdegree
**	h = hither distance, oneoveryon = 1/yon	!!!! both always negative  !!!!
**
** This assumes viewing direction is in -z direction, with eye at
** z=0, right-handed eye coordinate system.  Will map valid coordinates
** (those inside viewing fovdegree and with yon <= z <= hither < 0) into the 
** ranges: -w<=x<=w  -w<=y<=w  -w<=z<=0 (These conditions imply w>=0.)
============================================================================*/

void dor_matrix_load_perspective (
    DtMatrix4x4 matrix,
    DtReal      fovdegree,
    DtReal      hither,
    DtReal      yon)
{
    DtReal ang;
    DtReal oneoveryon;
    DtReal a, b, c;

    dor_matrix_load_identity (matrix);

    ang = fovdegree * (0.5 * 3.1415926535/180.0);

    oneoveryon = 1 / yon;

    c = 1 / tan(ang);		/* cotangent half-fovdegree */
    a = 1 / (1.0-hither*oneoveryon);
    b = - hither * a;

    matrix[0][0] = c;
    matrix[1][1] = c;
    matrix[2][2] = a;
    matrix[2][3] = b;
    matrix[3][2] = -1.0;
    matrix[3][3] = 0.0;
}



/*============================================================================
 * dor_matrix_load_projection
 *
 * Construct a general perspective or parallel transformation
 * window and viewport should have the same xy aspect ratio to avoid
 * distortion.  This assumes viewing direction is in -z direction,
 * with eye at the prp and a right-handed eye coordinate system.
 * Points inside the viewing volume are mapped into the volume specified by:
 * -w<=x<=w  -w<=y<=w, -w<=z<=0 (These conditions imply w>=0.)
 *
 * Note that view_plane, hither and yon are all z coordinates in the camera
 * (view reference) coordinate system and not distances (as in PHIGS).
 * This means they are typically negative values. The projection
 * reference point (prp) is also defined in camera coordinates (so
 * a negative value for prp[2] (the z component) places the prp in front
 * of the camera coordinate system's origin).
 * For perspective transformations, the z component of the prp must be
 * numerically greater than hither (for example: prp[2]=-1 and hither=-2 is
 * ok). If hither > prp[2], the objects will be clipped.
 *
 * This code was derived from the theory described in Foley and Van Dam.
 * Note that the Dore shear function behaves dirrerently than the shear
 * function assumed in Foley and Van Dam.
 *
============================================================================*/

void dor_matrix_load_projection (
    DtMatrix4x4       matrix,
    DtArea           *window,
    DtProjectionType  ptype,
    DtPoint3          prp,
    DtReal            view_plane,
    DtReal            hither,
    DtReal            yon)
{
    DtReal furx, fury, furz;
    DtMatrix4x4 tempmat;
    DtReal vp, xshear, yshear;
    DtReal xcenter, ycenter;

    /* translate center of window to the orign */

    xcenter = 0.5*(window->ll[0]+window->ur[0]);
    ycenter = 0.5*(window->ll[1]+window->ur[1]);

    dor_matrix_load_translate(matrix,-xcenter,-ycenter,-view_plane);
    vp = prp[2]-view_plane;

    /* compute shear to map window centre to view axis */

    xshear= (xcenter-prp[0])/vp;
    yshear= (ycenter-prp[1])/vp;

    dor_matrix_shear_z(matrix,xshear,yshear,DcPostConcatenate);

    /* map projection reference point (prp) to the origin */

    dor_matrix_translate(matrix,0.0,0.0,-vp,DcPostConcatenate);

    /* compute transformed window size */

    dor_math_pnttrns(window->ur[0],window->ur[1],view_plane,
                     matrix,
                     &furx,&fury,&furz);

    dor_matrix_load_identity (tempmat);

    hither = hither-prp[2];
    yon = yon-prp[2];

    if ( ptype == DcPerspective ){
	tempmat[0][0] = vp/furx;
	tempmat[1][1] = vp/fury;
	tempmat[2][2] = 1.0 / (1.0-(hither/yon));
	tempmat[2][3] = - hither * tempmat[2][2];
	tempmat[3][2] = -1.0;	/* w' = -z divide (for z divide) */
	tempmat[3][3] = 0.0;
    }
    else {
	tempmat[0][0] = 1.0/furx;
	tempmat[1][1] = 1.0/fury;
	tempmat[2][2] = 1/(hither-yon);
	tempmat[2][3] = - hither * tempmat[2][2];
    }

    dor_matrix_post_concatenate(matrix,tempmat);

#   ifdef debug
	printf ("load projection matrix computed\n");
	dor_matrix_print (matrix);
#   endif
}

/* ====================================================================== */

void dor_matrix_load_rotate_x (DtMatrix4x4 matrix, DtReal angle)
{
    auto DtReal cos_angle, sin_angle;

    dor_matrix_load_identity(matrix);

    matrix[1][1] =  cos_angle = dor_math_cos(angle);
    matrix[2][1] =  sin_angle = dor_math_sin(angle);
    matrix[2][2] =  cos_angle;
    matrix[1][2] = -sin_angle;
}

/* ====================================================================== */

void dor_matrix_load_rotate_y (DtMatrix4x4 matrix, DtReal angle)
{
    auto DtReal cos_angle, sin_angle;

    dor_matrix_load_identity(matrix);

    matrix[0][0] =  cos_angle = dor_math_cos(angle);
    matrix[0][2] =  sin_angle = dor_math_sin(angle);
    matrix[2][0] = -sin_angle;
    matrix[2][2] =  cos_angle;
}

/* ====================================================================== */

void dor_matrix_load_rotate_z (DtMatrix4x4 matrix, DtReal angle)
{
    auto DtReal cos_angle, sin_angle;

    dor_matrix_load_identity(matrix);

    matrix[0][0] =  cos_angle = dor_math_cos(angle);
    matrix[1][0] =  sin_angle = dor_math_sin(angle);
    matrix[1][1] =  cos_angle;
    matrix[0][1] = -sin_angle;
}

/* ====================================================================== */

void dor_matrix_load_scale (
    DtMatrix4x4 matrix,
    DtReal      scalex,
    DtReal      scaley,
    DtReal      scalez)
{
    dor_matrix_load_identity (matrix);

    matrix[0][0] = scalex;
    matrix[1][1] = scaley;
    matrix[2][2] = scalez;
}

/* ====================================================================== */

void dor_matrix_load_shear (
    DtMatrix4x4  matrix,
    DtMajorPlane plane,
    DtReal       firstdirval,
    DtReal       seconddirval)
{
    dor_matrix_load_identity (matrix);

    switch (plane)
    {   case DcYZ:
	    matrix[1][0] = firstdirval;
	    matrix[2][0] = seconddirval;
	    break;
	case DcXZ:
	    matrix[0][1] = firstdirval;
	    matrix[2][1] = seconddirval;
	    break;
	case DcXY:
	    matrix[0][2] = firstdirval;
	    matrix[1][2] = seconddirval;
	    break;
    }
}

/* ====================================================================== */

void dor_matrix_load_shear_z (
    DtMatrix4x4 matrix,
    DtReal      a,
    DtReal      b)
{
    dor_matrix_load_identity (matrix);
    matrix[0][2] = a;
    matrix[1][2] = b;
}

/* ====================================================================== */

void dor_matrix_load_translate (
    DtMatrix4x4 matrix,
    DtReal      transx,
    DtReal      transy,
    DtReal      transz)
{
    dor_matrix_load_identity (matrix);

    matrix[0][3] = transx;
    matrix[1][3] = transy;
    matrix[2][3] = transz;
}

/* ====================================================================== */

void dor_matrix_new_vectormultiply (
    register DtVector4    invec,
    register DtMatrix4x4  inmat,
    register DtVector4    outvec)
{
    register DtShort row;
    register DtShort matindex;

    for (row=0;  row < 4;  row++)
    {   outvec[row] = 0.0;

	for (matindex=0;  matindex < 4;  matindex++)
	    outvec[row] += inmat[row][matindex] * invec[matindex];
    }
}

/* ====================================================================== */

void dor_matrix_parallel (
    DtMatrix4x4 matrix,
    DtReal      windowsize,
    DtReal      hither,
    DtReal      yon,
    DtCompType  compose_type)
{
    DtMatrix4x4 tempmat;

    dor_matrix_load_parallel(tempmat,windowsize,hither,yon);

    dor_matrix_concat(matrix,tempmat,compose_type);
}

/* ====================================================================== */

void dor_matrix_perspective (
    DtMatrix4x4 matrix,
    DtReal      fovdegree,
    DtReal      hither,
    DtReal      yon,
    DtCompType  compose_type)
{
    DtMatrix4x4 tempmat;

    dor_matrix_load_perspective (tempmat,fovdegree,hither,yon);
    dor_matrix_concat (matrix,tempmat,compose_type);
}



/*****************************************************************************
**  This procedure pre-concatenates two matrices, and stores the result in
**  the matrix given in the first parameter.  NOTE:  Since we are using column
**  vectors, this is done via matrix post-multiply.
*****************************************************************************/

void dor_matrix_pre_concatenate (
    DtMatrix4x4 matrix,
    DtMatrix4x4 concatmatrix)
{
    auto DtMatrix4x4 temp;

    dor_matrix_multiply (matrix, concatmatrix, temp);
    dor_matrix_load (matrix, temp);
}



/*****************************************************************************
**  This procedure post-concatenates two matrices, and stores the result in
**  the matrix given in the first parameter.  NOTE:  Since we are using column
**  vectors, this is done via matrix pre-multiply.
*****************************************************************************/

void dor_matrix_post_concatenate (
    DtMatrix4x4 matrix,
    DtMatrix4x4 concatmatrix)
{
    auto DtMatrix4x4 temp;

    dor_matrix_multiply (concatmatrix, matrix, temp);
    dor_matrix_load (matrix, temp);
}

/* ====================================================================== */

void dor_matrix_print (DtMatrix4x4 matrix)
{
    register DtShort row;

    if (!matrix) {
	dor_print_output ("Warning:  Matrix is null.");
	return;
    }

    sprintf (dor_print_get_line(), "matrix at [%lx]:", matrix);
    dor_print_output (dor_print_get_line());
    dor_print_indent_up ();

    for (row=0;  row < 4;  ++row){
	sprintf
	(   dor_print_get_line(),
	    "[ % #12.5lf % #12.5lf % #12.5lf % #12.5lf ]",
	    (double)matrix[row][0], (double)matrix[row][1],
	    (double)matrix[row][2], (double)matrix[row][3]
	);
	dor_print_output (dor_print_get_line());
    }

    dor_print_indent_down ();
    dor_print_output ("matrix end");
}

/* ====================================================================== */

void dor_matrix_rotate_x (
    DtMatrix4x4 matrix,
    DtReal      angle,
    DtCompType  compose_type)
{
    DtMatrix4x4 tempmat;

    dor_matrix_load_rotate_x(tempmat,angle);
    dor_matrix_concat (matrix,tempmat,compose_type);
}

/* ====================================================================== */

void dor_matrix_rotate_y (
    DtMatrix4x4 matrix,
    DtReal      angle,
    DtCompType  compose_type)
{
    DtMatrix4x4 tempmat;

    dor_matrix_load_rotate_y(tempmat,angle);
    dor_matrix_concat (matrix,tempmat,compose_type);
}

/* ====================================================================== */

void dor_matrix_rotate_z (
    DtMatrix4x4 matrix,
    DtReal      angle,
    DtCompType  compose_type)
{
    DtMatrix4x4 tempmat;

    dor_matrix_load_rotate_z (tempmat, angle);
    dor_matrix_concat (matrix, tempmat, compose_type);
}

/* ====================================================================== */

void dor_matrix_scale (
    DtMatrix4x4 matrix,
    DtReal      scalex,
    DtReal      scaley,
    DtReal      scalez,
    DtCompType  compose_type)
{
    DtMatrix4x4 tempmat;

    dor_matrix_load_scale (tempmat, scalex, scaley, scalez);
    dor_matrix_concat (matrix, tempmat, compose_type);
}

/* ====================================================================== */

void dor_matrix_shear (
    DtMatrix4x4  matrix,
    DtMajorPlane plane,
    DtReal       firstdirval,
    DtReal       seconddirval,
    DtCompType   compose_type)
{
    DtMatrix4x4 tempmat;

    dor_matrix_load_shear (tempmat, plane, firstdirval, seconddirval);
    dor_matrix_concat (matrix, tempmat, compose_type);
}

/* ====================================================================== */

void dor_matrix_shear_z (
    DtMatrix4x4 matrix,
    DtReal      a,
    DtReal      b,
    DtCompType  compose_type)
{
    DtMatrix4x4 tempmat;

    dor_matrix_load_shear_z (tempmat,a,b);
    dor_matrix_concat (matrix, tempmat, compose_type);
}



/*============================================================================
**  Computes an exact solution for the matrix equation:
**
**      a * c = b
**
**  where 'c' is the solution matrix, and in place operation is allowed.
**
**  Restrictions:  arnum = acnum = brnum  <= 6 = 16
**                 6 -- maximum dimension length of all matrices
============================================================================*/

#define	arnum	4
#define acnum	4
#define	brnum	4
#define	bcnum	4

DtShort dor_matrix_solve (
    DtReal a[arnum][acnum],
    DtReal c[brnum][bcnum],
    DtReal b[acnum][bcnum])
{
    int abcnum, i, ij, j, p[6], actrow, tmprow, vrtrow;

    /*
      abcnum - brnum -- number of rows and columns in the
      respective matrices
      i -- row matindex
      ij -- matrix 'ab' diagonal matindex
      j -- column matindex
      p -- "permutation" array which keeps track of row re-matindexing
      due to partial pivoting.  the i'th element of 'p'
      indicates which actual 'ab' row belongs in the i'th
      virtual (re-matindexed due to pivoting) row of 'ab'.
      actrow -- actual 'ab' row number of new pivot
      tmprow -- temporary 'ab' row number
      vrtrow -- virtual 'ab' row number of the new pivot
      
      completion status:
      10 = good matrix solution.
      1 = possibly bad solution.
      0 = routine in progress.
      -1 = bad dimensions.
      -2 = bad solution, zero pivot.
      */
    /*
      DtReal a(arnum,acnum), b(brnum,bcnum), c(acnum,bcnum)
      
      a -- left side input matrix
      b -- right side input matrix
      c -- solution to the matrix equation
      */
    DtReal ab[6][12], pivot, rnorm;
    DtReal ptemp;
    /*
      DtReal tpivot;
      */
    /*
      ab -- local scratch array containing concatenated copies
      of 'a' and 'b'
      pivot -- the new pivot value
      rnorm -- row normalization factor
      tpivot -- temporary pivot value
      */
    /*
      check dimensions of matrices
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
      perform gauss-jordan elimination with partial pivoting of the rows
      */
    for (ij=0;ij<arnum;ij++){

	/*
	  determine the pivot (the element with the largest absolute
	  value) for the current virtual column
	  */
	pivot = 0.0;
	for (i=ij;i<arnum;i++){
	    ptemp = dor_math_fabs(ab[p[i]][ij]);
	    if (ptemp <= pivot)
		    continue;
	    pivot = ptemp;
	    vrtrow = i;
	}
	/*
	  check for zero pivot
	  */
	if (pivot == 0.0)
		return(-2);
	/*
	  get the actual pivot value (not the absolute value)
	  note: the pivots are a good indicator for "bad" inversions
	  */
	pivot = ab[p[vrtrow]][ij];
	/*
	  update the permutation array by swapping the current ij'th 
	  virtual row with the virtual row of the new pivot
	  */
	tmprow = p[ij];
	p[ij] = p[vrtrow];
	p[vrtrow] = tmprow;
	/*
	  get the actual 'ab' row of the new pivot (which is now
	  at virtual row 'ij')
	  */
	actrow = p[ij];
	/*
	  divide pivot row by the pivot value (note: the lower left
	  triangle of 'ab' can be ignored)
	  */
	for (j=ij+1;j<abcnum;j++)
		ab[actrow][j] = ab[actrow][j] / pivot;
	/*
	  zero out the current column except for the actual pivot row 
	  */
	for (i=0;i<arnum;i++){
	    /*
	      check for the pivot row
	      */
	    if (i != actrow){
		/*
		  normalize the pivot row and subtract it from
		  the current row
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
      copy 'b' part of 'ab' into 'c' with re-matindexing via the
      permutation array
      */
    for (i=0;i<arnum;i++){
	actrow = p[i];		
	for (j=0;j<bcnum;j++)
		c[i][j] = ab[actrow][acnum+j];
    }
    return(10);
}

/* ====================================================================== */

void dor_matrix_translate (
    DtMatrix4x4 matrix,
    DtReal      transx,
    DtReal      transy,
    DtReal      transz,
    DtCompType  compose_type)
{
    DtMatrix4x4 tempmat;

    dor_matrix_load_translate(tempmat,transx,transy,transz);

    dor_matrix_concat (matrix,tempmat,compose_type);
}

/* ====================================================================== */

void dor_matrix_transpose (DtMatrix4x4 matrix)
{
    register DtReal temp;

    temp = matrix[1][0];   matrix[1][0] = matrix[0][1];   matrix[0][1] = temp;
    temp = matrix[2][0];   matrix[2][0] = matrix[0][2];   matrix[0][2] = temp;
    temp = matrix[3][0];   matrix[3][0] = matrix[0][3];   matrix[0][3] = temp;
    temp = matrix[2][1];   matrix[2][1] = matrix[1][2];   matrix[1][2] = temp;
    temp = matrix[3][1];   matrix[3][1] = matrix[1][3];   matrix[1][3] = temp;
    temp = matrix[3][2];   matrix[3][2] = matrix[2][3];   matrix[2][3] = temp;
}

/* ====================================================================== */

void dor_matrix_vectormultiply (
    register DtVector4    invec,
    register DtMatrix4x4  inmat,
    register DtVector4    outvec)
{
    DtShort row;
    DtShort matindex;

    for (row=0;  row < 3;  row++)
    {   outvec[row] = 0.0;

	for (matindex=0;  matindex < 4;  matindex++)
	    outvec[row] += inmat[row][matindex] * invec[matindex];
    }

    outvec[3] = invec[3];
}

/* ====================================================================== */

void dor_matrix_print3x3 (DtMatrix3x3 matrix)
{
    DtShort row;

    if (matrix == NULL){
	dor_print_output("matrix is null");
	return;
    }

    sprintf(dor_print_get_line(),"matrix at [%lx]:",matrix);
    dor_print_output(dor_print_get_line());
    dor_print_indent_up();

    for (row=0;  row < 3;  ++row) {
	sprintf(dor_print_get_line(),"%f %f %f", (double)matrix[row][0],
	    (double)matrix[row][1], (double)matrix[row][2]);
	dor_print_output (dor_print_get_line());
    }

    dor_print_indent_down ();
    dor_print_output ("matrix end");
}
