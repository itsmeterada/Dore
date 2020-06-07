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
	void ddr_sampledev_math_crossproduct (p,v,result)
	DtFlag ddr_sampledev_math_renormalize (normal)
	void ddr_sampledev_math_pnttrns (invec, matrix, outvec)
  ======================================================================
 */
#include <dore/dore.h>
#include <math.h>
/*
 ======================================================================
 */
void
ddr_sampledev_math_crossproduct(p,v,result)
DtReal p[3], v[3];
DtReal result[3];
{
    result[0] = p[1]*v[2] - p[2]*v[1]; 
    result[1] = p[2]*v[0] - p[0]*v[2];
    result[2] = p[0]*v[1] - p[1]*v[0];
}
/*
 ======================================================================
 */
DtFlag
ddr_sampledev_math_renormalize(normal)
DtReal normal[3];
{
    DtReal length;

    length = (normal[0])*(normal[0])+(normal[1])*(normal[1])+
	    (normal[2])*(normal[2]);

    if (length == 0.){
	normal[0] = 0.;
	normal[1] = 0.;
	normal[2] = 0.;

	return(DcFalse);
    }

    if (length == 1.){
	return(DcTrue);
    }

    length = 1. / sqrt((double)length);

    normal[0] *= length;
    normal[1] *= length;
    normal[2] *= length;

    return(DcTrue);
}
/*
 ======================================================================
 */
void
ddr_sampledev_math_pnttrns(invec,matrix,outvec)
DtReal invec[4];
DtMatrix4x4 matrix;
DtReal outvec[4];
{
    DtReal tmpvec[4];
    DtShort row;
    DtShort matindex;

    for (row=0;row<4;row++){
	tmpvec[row] = 0.;

	for (matindex=0;matindex<4;matindex++)
	    tmpvec[row] += matrix[row][matindex]*invec[matindex];
    }

    outvec[0] = tmpvec[0];
    outvec[1] = tmpvec[1];
    outvec[2] = tmpvec[2];
    outvec[3] = tmpvec[3];
}
/*
 ======================================================================
 */
