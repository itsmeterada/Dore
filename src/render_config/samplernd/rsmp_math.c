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
 ==========================================================================
 Functions :
	void drr_samplernd_matrix_identity(matrix)
	void drr_samplernd_matrix_load(matrixout,matrixin)
	void drr_samplernd_matrix_print(matrix, fileptr)
	void drr_samplernd_matrix_invert(matrix)
	void drr_samplernd_matrix_post_concatenate(matrix,concatmat)
	void drr_samplernd_matrix_scale(matrix,scalex,scaley,scalez)
	void drr_samplernd_matrix_translate(matrix,transx,transy,transz)
	void drr_samplernd_matrix_shear(matrix, plane, firstdirval, seconddirval)
	void drr_samplernd_matrix_vectormultiply(invec,inmat,outvec)
	DtShort drr_samplernd_matrix_solve(a,c,b)
	void drr_samplernd_math_crossproduct(px,py,pz,vx,vy,vz,resultx,resulty,
							resultz)
	void drr_samplernd_math_homo_trns(origx,origy,origz,origw,matrix,
		 					transx,transy,transz,transw)
	DtFlag drr_samplernd_vector_normalize(normal)
 ==========================================================================
*/

#include "dore.h"
#include "dore/dore_develop/error.h"
#include "math.h"

/*
 * Math utility module. Contains matrix and vector manipulation routines.
 */

/*
===============================================================================
*/

/* 
 * Returns a 4 x 4 identity matrix 
 */

void
drr_samplernd_matrix_identity(matrix)
     DtMatrix4x4 matrix;
{
   DtShort matindex;
   DtShort row, column;

   for (row=0;row<4;row++)
	 for (column=0;column<4;column++)
	       matrix[row][column] = 0.;

   for (matindex=0;matindex<4;matindex++)
	 matrix[matindex][matindex] = 1.;
}


/*
===============================================================================
*/

/* 
 * Copy matrixin to matrixout
 */

void
drr_samplernd_matrix_load(matrixout,matrixin)
     DtMatrix4x4 matrixout;
     DtMatrix4x4 matrixin;

{     
   DtInt row, column;

   for (row=0;row<4;row++)
	 for (column=0;column<4;column++)
	       matrixout[row][column] = matrixin[row][column];
}

/*
===============================================================================
*/

void
drr_samplernd_matrix_print(matrix, fileptr)
     DtMatrix4x4 matrix;
{
   DtShort row;

   if (matrix == 0) {
      fprintf (fileptr, "matrix is null\n");
      return;
   }

   for (row=0;row<4;row++) {
      fprintf(fileptr, "   %f %f %f %f\n",
	      (double)matrix[row][0],
	      (double)matrix[row][1],
	      (double)matrix[row][2],
	      (double)matrix[row][3]);
   }
   fprintf (fileptr, "}\n");
}

/*
===============================================================================
*/

void
drr_samplernd_matrix_indent_print(matrix, fileptr)
     DtMatrix4x4 matrix;
{
   DtShort row;

   if (matrix == 0) {
      fprintf (fileptr, "matrix is null\n");
      return;
   }

   DDprint_IndentUp();
   for (row=0;row<4;row++) {
      sprintf(DDprint_GetLine(),
            "%f %f %f %f",
            (double)matrix[row][0],
	    (double)matrix[row][1],
	    (double)matrix[row][2],
	    (double)matrix[row][3]);
      DDprint(DDprint_GetLine());
   }
   DDprint_IndentDown();
}

/*
===============================================================================
*/

void
drr_samplernd_matrix_invert(matrix)
     DtMatrix4x4 matrix;
{
   DtMatrix4x4 invertmatrix;
   DtMatrix4x4 identmatrix;
   DtInt status;
   DtShort drr_samplernd_matrix_solve();

   drr_samplernd_matrix_identity(identmatrix);

   status = drr_samplernd_matrix_solve(matrix,invertmatrix,identmatrix);

   if (status != 10) {
      DDerror (ERR_CANT_INVERT_MATRIX,
		  "drr_samplernd_matrix_invert", DcNullPtr);
      return;
   }

   drr_samplernd_matrix_load(matrix,invertmatrix);
}

/*
===============================================================================
*/

 /*
 * Post-concatenate transformation represented by concatmatrix
 * with transformation represented by matrix. Since we are using
 * column vectors this is a matrix pre-multiply!
 */

void
drr_samplernd_matrix_post_concatenate(matrix,concatmatrix)
     DtMatrix4x4 matrix;
     DtMatrix4x4 concatmatrix;

{
   DtMatrix4x4 tempmat;
   DtInt row, column;
   DtInt matindex;

   for (row=0;row<4;row++) {
      for (column=0;column<4;column++) {
       tempmat[row][column] = 0.;
       for (matindex=0;matindex<4;matindex++) {
          tempmat[row][column] +=
            concatmatrix[row][matindex]*matrix[matindex][column];
       }
      }
   }
  drr_samplernd_matrix_load(matrix,tempmat);
}

/*
===============================================================================
*/

void
drr_samplernd_matrix_scale(matrix,scalex,scaley,scalez)
     DtMatrix4x4 matrix;
     DtReal scalex, scaley, scalez;
{

   drr_samplernd_matrix_identity(matrix);

   matrix[0][0] = scalex;
   matrix[1][1] = scaley;
   matrix[2][2] = scalez;
}

/*
===============================================================================
*/

void
drr_samplernd_matrix_translate(matrix,transx,transy,transz)
     DtMatrix4x4 matrix;
     DtReal transx, transy, transz;
{
   drr_samplernd_matrix_identity(matrix);

   matrix[0][3] = transx;
   matrix[1][3] = transy;
   matrix[2][3] = transz;
}

/*
===============================================================================
*/

void
drr_samplernd_matrix_shear(matrix, plane, firstdirval, seconddirval)
     DtMatrix4x4 matrix;
     DtMajorPlane plane;
     DtReal firstdirval, seconddirval;
{

   drr_samplernd_matrix_identity(matrix);

   switch(plane) {
   case DcYZ:
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

/*
===============================================================================
*/

void
drr_samplernd_matrix_vectormultiply(invec,inmat,outvec)
     register DtPoint4 invec;
     register DtMatrix4x4 inmat;
     register DtPoint4 outvec;
{
   DtShort row;
   DtShort matindex;

   for (row=0;row<4;row++) {
      outvec[row] = 0.;
      for (matindex=0;matindex<4;matindex++)
	    outvec[row] = outvec[row] + inmat[row][matindex]*invec[matindex];
   }
}


/*
===============================================================================
*/


/*	computes an exact solution for the matrix equation,
 *				a * c = b
 *			where 'c' is the solution matrix
 *			in place operation is allowed
 *
 *	restrictions:	arnum = acnum = brnum  <= 6 = 16
 *
 *		6 -- maximum dimension length of all matrices
 */
#define	arnum	4
#define     acnum	4
#define	brnum	4
#define	bcnum	4

DtShort drr_samplernd_matrix_solve(a,c,b)
     DtReal a[arnum][acnum], c[brnum][bcnum], b[acnum][bcnum];
{
   int abcnum, i, ij, j, p[6], actrow, tmprow, vrtrow;

   /*
    *  abcnum - brnum -- number of rows and columns in the respective matrices
    * i -- row matindex
    * ij -- matrix 'ab' diagonal matindex
    * j -- column matindex
    * p -- "permutation" array which keeps track of row re-matindexing
    * 	   due to partial pivoting.  the i'th element of 'p'
    * 	   indicates which actual 'ab' row belongs in the i'th
    * 	   virtual (re-matindexed due to pivoting) row of 'ab'.
    * actrow -- actual 'ab' row number of new pivot
    * tmprow -- temporary 'ab' row number
    * vrtrow -- virtual 'ab' row number of the new pivot
    * 
    * completion status:
    * 	10 = good matrix solution.
    * 	1  = possibly bad solution.
    * 	0  = routine in progress.
    * 	-1 = bad dimensions.
    * 	-2 = bad solution, zero pivot.
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
    *       of 'a' and 'b'
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

   for (i=0;i<arnum;i++) {
      for (j=0;j<acnum;j++)
	    ab[i][j] = a[i][j];
      for (j=0;j<bcnum;j++)
	    ab[i][j+acnum] = b[i][j];
   }

   /*
    * perform gauss-jordan elimination with partial pivoting of the rows
    */
   for (ij=0;ij<arnum;ij++) {

      /*
       * determine the pivot (the element with the largest absolute
       * value) for the current virtual column
       */
      pivot = 0.0;
      for (i=ij;i<arnum;i++) {
	 ptemp = fabs(ab[p[i]][ij]);
	 if (ptemp <= pivot)
	       continue;
	 pivot = ptemp;
	 vrtrow = i;
      }
      /*
       * get the actual pivot value (not the absolute value)
       * note: the pivots are a good indicator for "bad" inversions
       */
      pivot = ab[p[vrtrow]][ij];
      /*
       * check for zero pivot
       */
      if (pivot == 0.0)
	    return(-2);
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
      for (i=0;i<arnum;i++) {
	 /*
	  * check for the pivot row
	  */
	 if (i != actrow) {
	    /*
	     * normalize the pivot row and subtract it from
	     * the current row
	     */
	    rnorm = ab[i][ij];
	    if (rnorm != 0.0) {
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
   for (i=0;i<arnum;i++) {
      actrow = p[i];		
      for (j=0;j<bcnum;j++)
	    c[i][j] = ab[actrow][acnum+j];
   }
   return(10);
}

/*
===============================================================================
*/

void
drr_samplernd_math_crossproduct(px,py,pz,vx,vy,vz,resultx,resulty,resultz)
     DtReal px, py, pz, vx, vy, vz;
     DtReal *resultx, *resulty, *resultz;
{
   *resultx = py*vz - pz*vy; 
   *resulty = pz*vx - px*vz;
   *resultz = px*vy - py*vx;
}

/*
===============================================================================
*/

void
drr_samplernd_math_homo_trns(origx,origy,origz,origw,matrix,
		 transx,transy,transz,transw)
     DtReal origx, origy, origz, origw;
     DtMatrix4x4 matrix;
     DtReal *transx, *transy, *transz, *transw;
{
   DtPoint4 invec, outvec;

   invec[0] = origx;
   invec[1] = origy;
   invec[2] = origz;
   invec[3] = origw;

   drr_samplernd_matrix_vectormultiply(invec,matrix,outvec);

   *transx = outvec[0];
   *transy = outvec[1];
   *transz = outvec[2];
   *transw = outvec[3];
}

/*
===============================================================================
*/

DtFlag drr_samplernd_vector_normalize(normal)
     DtReal normal[3];
{
   DtReal length;

   length = (normal[0])*(normal[0])+(normal[1])*(normal[1])+
	 (normal[2])*(normal[2]);

   if (length == 0.) {
      normal[0] = 0.;
      normal[1] = 0.;
      normal[2] = 0.;

      return(DcFalse);
   }

   if (length == 1.) {
      return(DcTrue);
   }

   length = 1. / sqrt(length);

   normal[0] *= length;
   normal[1] *= length;
   normal[2] *= length;

   return(DcTrue);
}


/*
===============================================================================
*/


