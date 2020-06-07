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
**  execution on non-titan machines.  For titans, the file matrix_titan.c is
**  used.  This file is conditionally included in matrix.c.
*****************************************************************************/



/*****************************************************************************
**  This routine loads `matrix' with the entries from `newmat'.
*****************************************************************************/

void 
dor_matrix_load (
    register DtMatrix4x4 D,	/* Destination Matrix */
    register DtMatrix4x4 S	/* Source Matrix */
)
{
    D[0][0] = S[0][0]; D[0][1] = S[0][1]; D[0][2] = S[0][2]; D[0][3] = S[0][3];
    D[1][0] = S[1][0]; D[1][1] = S[1][1]; D[1][2] = S[1][2]; D[1][3] = S[1][3];
    D[2][0] = S[2][0]; D[2][1] = S[2][1]; D[2][2] = S[2][2]; D[2][3] = S[2][3];
    D[3][0] = S[3][0]; D[3][1] = S[3][1]; D[3][2] = S[3][2]; D[3][3] = S[3][3];
}



/*****************************************************************************
**  This funtion loads each entry of `matrix' with the value `value'.
*****************************************************************************/

void dor_matrix_load_constant (
    DtMatrix4x4 M,		/* Target Matrix */
    DtReal      value)		/* New Value for All Entries */
{
    M[0][0] = value;   M[0][1] = value;   M[0][2] = value;   M[0][3] = value;
    M[1][0] = value;   M[1][1] = value;   M[1][2] = value;   M[1][3] = value;  
    M[2][0] = value;   M[2][1] = value;   M[2][2] = value;   M[2][3] = value;  
    M[3][0] = value;   M[3][1] = value;   M[3][2] = value;   M[3][3] = value;  
}



/*****************************************************************************
**  This function sets the given matrix to the identity matrix.
*****************************************************************************/

void dor_matrix_load_identity (
    register DtMatrix4x4 M)
{
    M[0][0] = 1;   M[0][1] = 0;   M[0][2] = 0;   M[0][3] = 0;
    M[1][0] = 0;   M[1][1] = 1;   M[1][2] = 0;   M[1][3] = 0;
    M[2][0] = 0;   M[2][1] = 0;   M[2][2] = 1;   M[2][3] = 0;
    M[3][0] = 0;   M[3][1] = 0;   M[3][2] = 0;   M[3][3] = 1;
}



/*****************************************************************************
**  This procedure multiplies the first matrix by the second matrix and stores
**  the result in the matrix given by the third parameter.
*****************************************************************************/

void 
dor_matrix_multiply (
    register DtMatrix4x4 A,
    register DtMatrix4x4 B,	/*  A x B --> R  */
    register DtMatrix4x4 R
)
{
    R[0][0] =A[0][0]*B[0][0] +A[0][1]*B[1][0] +A[0][2]*B[2][0] +A[0][3]*B[3][0];
    R[0][1] =A[0][0]*B[0][1] +A[0][1]*B[1][1] +A[0][2]*B[2][1] +A[0][3]*B[3][1];
    R[0][2] =A[0][0]*B[0][2] +A[0][1]*B[1][2] +A[0][2]*B[2][2] +A[0][3]*B[3][2];
    R[0][3] =A[0][0]*B[0][3] +A[0][1]*B[1][3] +A[0][2]*B[2][3] +A[0][3]*B[3][3];

    R[1][0] =A[1][0]*B[0][0] +A[1][1]*B[1][0] +A[1][2]*B[2][0] +A[1][3]*B[3][0];
    R[1][1] =A[1][0]*B[0][1] +A[1][1]*B[1][1] +A[1][2]*B[2][1] +A[1][3]*B[3][1];
    R[1][2] =A[1][0]*B[0][2] +A[1][1]*B[1][2] +A[1][2]*B[2][2] +A[1][3]*B[3][2];
    R[1][3] =A[1][0]*B[0][3] +A[1][1]*B[1][3] +A[1][2]*B[2][3] +A[1][3]*B[3][3];

    R[2][0] =A[2][0]*B[0][0] +A[2][1]*B[1][0] +A[2][2]*B[2][0] +A[2][3]*B[3][0];
    R[2][1] =A[2][0]*B[0][1] +A[2][1]*B[1][1] +A[2][2]*B[2][1] +A[2][3]*B[3][1];
    R[2][2] =A[2][0]*B[0][2] +A[2][1]*B[1][2] +A[2][2]*B[2][2] +A[2][3]*B[3][2];
    R[2][3] =A[2][0]*B[0][3] +A[2][1]*B[1][3] +A[2][2]*B[2][3] +A[2][3]*B[3][3];

    R[3][0] =A[3][0]*B[0][0] +A[3][1]*B[1][0] +A[3][2]*B[2][0] +A[3][3]*B[3][0];
    R[3][1] =A[3][0]*B[0][1] +A[3][1]*B[1][1] +A[3][2]*B[2][1] +A[3][3]*B[3][1];
    R[3][2] =A[3][0]*B[0][2] +A[3][1]*B[1][2] +A[3][2]*B[2][2] +A[3][3]*B[3][2];
    R[3][3] =A[3][0]*B[0][3] +A[3][1]*B[1][3] +A[3][2]*B[2][3] +A[3][3]*B[3][3];
}



/*****************************************************************************
**  This routine loads each entry of a 3x3 matrix with a constant value.
*****************************************************************************/

void dor_matrix_load_constant3x3 (
    DtMatrix3x3 matrix,
    DtReal      value)
{
    matrix[0][0] = value;   matrix[0][1] = value;   matrix[0][2] = value;
    matrix[1][0] = value;   matrix[1][1] = value;   matrix[1][2] = value;
    matrix[2][0] = value;   matrix[2][1] = value;   matrix[2][2] = value;
}



/*****************************************************************************
**  This procedure loads the source matrix S into the destination matrix D.
*****************************************************************************/

void dor_matrix_load3x3 (
    register DtMatrix3x3 D,
    register DtMatrix3x3 S)
{
    D[0][0] = S[0][0];   D[0][1] = S[0][1];   D[0][2] = S[0][2];
    D[1][0] = S[1][0];   D[1][1] = S[1][1];   D[1][2] = S[1][2];
    D[2][0] = S[2][0];   D[2][1] = S[2][1];   D[2][2] = S[2][2];
}



/*****************************************************************************
**  This routine loads the given 3x3 matrix with the 3x3 identity matrix.
*****************************************************************************/

void dor_matrix_load_identity3x3 (
    register DtMatrix3x3 matrix)
{
    matrix[0][0] = 1;   matrix[0][1] = 0;   matrix[0][2] = 0;
    matrix[1][0] = 0;   matrix[1][1] = 1;   matrix[1][2] = 0;
    matrix[2][0] = 0;   matrix[2][1] = 0;   matrix[2][2] = 1;
}
