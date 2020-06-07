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
**  This file contains the matrix utility functions that are tailored to fast
**  execution on the titan (parallel-processor) machines.  For other machines,
**  the file matrix_other.c is used.  This file is conditionally included in
**  matrix.c.
*****************************************************************************/



/*****************************************************************************
**  This routine loads the matrix `matrix' with the entries from `newmat'.
*****************************************************************************/

void 
dor_matrix_load (
    DtMatrix4x4 matrix,		/* Destination Matrix */
    DtMatrix4x4 newmat		/* Source Matrix */
)
{
    register DtInt row, column;

PRAGMA(IVDEP);

    for (row=0;  row < 4;  row++)
	for (column=0;  column < 4;  column++)
	    matrix[row][column] = newmat[row][column];
}



/*****************************************************************************
**  This function loads each entry of `matrix' with the value `value'.
*****************************************************************************/

void 
dor_matrix_load_constant (
    DtMatrix4x4 matrix,		/* Target Matrix */
    DtReal value		/* New Value for Each Matrix Entry */
)
{
    register DtShort row, column;	/* Matrix Indices */

PRAGMA(IVDEP)
    for (row=0;  row < 4;  row++)
        for (column=0;  column < 4;  column++)
	    matrix[row][column] = value;
}



/*****************************************************************************
**  This routine sets the given matrix to the identity matrix.
*****************************************************************************/

void dor_matrix_load_identity (
    DtMatrix4x4 matrix)
{
    register DtShort matindex;

    dor_matrix_load_constant (matrix, 0.0);

PRAGMA(IVDEP)
    for (matindex=0;  matindex < 4;  matindex++)
        matrix[matindex][matindex] = 1.0;
}



/*****************************************************************************
**  This procedure multiplies the first matrix by the second matrix and stores
**  the result in the matrix given by the third parameter.
*****************************************************************************/

void 
dor_matrix_multiply (
    DtMatrix4x4 A,
    DtMatrix4x4 B,	/*  A x B --> R  */
    DtMatrix4x4 R
)
{
    register DtInt row, column;
    register DtInt matindex;

PRAGMA(IVDEP)
    for (row=0;  row < 4;  row++){
	for (column=0;  column < 4;  column++){
	    R[row][column] = 0.0;

	    for (matindex=0;  matindex < 4;  matindex++){
		R[row][column] += A[row][matindex] * B[matindex][column];
	    }
	}
    }
}



/*****************************************************************************
**  This routine loads each entry of a 3x3 matrix with a constant value.
*****************************************************************************/

void dor_matrix_load_constant3x3 (
    register DtMatrix3x3 matrix,
    register DtReal value)
{
    register DtShort row, column;

PRAGMA(IVDEP)
    for (row=0;  row < 3;  row++)
        for (column=0;  column < 3;  column++)
	    matrix[row][column] = value;
}



/*****************************************************************************
**  This procedure loads the source matrix S into the destination matrix D.
*****************************************************************************/

void 
dor_matrix_load3x3 (
    register DtMatrix3x3 D,	/* D <-- S */
    register DtMatrix3x3 S
)
{
    register DtInt row, column;

PRAGMA(IVDEP)
    for (row=0;  row < 3;  row++)
        for (column=0;  column < 3;  column++)
            D[row][column] = S[row][column];
}



/*****************************************************************************
**  This routine loads the given 3x3 matrix with the 3x3 identity matrix.
*****************************************************************************/

void dor_matrix_load_identity3x3 (
    register DtMatrix3x3 matrix)
{
    register DtShort matindex;

    dor_matrix_load_constant3x3 (matrix, 0.0);

PRAGMA(IVDEP)
    for (matindex=0;  matindex < 3;  matindex++)
        matrix[matindex][matindex] = 1.0;
}
