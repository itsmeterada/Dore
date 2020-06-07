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
	ddr_x11_DGL_drom_get_lcstowcsmat ddr_x11_DGL_drom_get_lcstowcsmat (matrix)
	ddr_x11_DGL_drom_update_lcstowcsmat_lokatfrm ddr_x11_DGL_drom_update_lcstowcsmat_lokatfrm (at, from, up, concat)
	ddr_x11_DGL_drom_update_lcstowcsmat_rotate ddr_x11_DGL_drom_update_lcstowcsmat_rotate (axis, radians, concat)
	ddr_x11_DGL_drom_update_lcstowcsmat_scale ddr_x11_DGL_drom_update_lcstowcsmat_scale (sx, sy, sz, concat)
	ddr_x11_DGL_drom_update_lcstowcsmat_shear ddr_x11_DGL_drom_update_lcstowcsmat_shear (plane, firstshearvalue, secondshearvalue, concat)
	ddr_x11_DGL_drom_update_lcstowcsmat_tfmmat ddr_x11_DGL_drom_update_lcstowcsmat_tfmmat (tfmmat, concat)
	ddr_x11_DGL_drom_update_lcstowcsmat_transl ddr_x11_DGL_drom_update_lcstowcsmat_transl (tx, ty, tz, concat)
	ddr_x11_DGL_drom_update_lcstowcsmat_push ddr_x11_DGL_drom_update_lcstowcsmat_push ()
	ddr_x11_DGL_drom_update_lcstowcsmat_pop ddr_x11_DGL_drom_update_lcstowcsmat_pop ()
	int ddr_x11_DGL_drom_update_lcstowcsmat ()
	int ddr_x11_DGL_drom_get_wcstofcsmat (wcstofcsmat)
	int ddr_x11_DGL_drom_push_lcstofcsmat (newmatrix)
	int ddr_x11_DGL_drom_pop_lcstofcsmat ()
	DtFlag ddr_x11_DGL_drom_transform_clip_z_point (point,tfmmatrix)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
PRAGMA(OPT_LEVEL 2)

ddr_x11_DGL_drom_get_lcstowcsmat (matrix)
     DtMatrix4x4 matrix;
{
    DtInt i, j;

    for (i=0; i<4; i++) {
PRAGMA(IVDEP)
	for (j=0; j<4; j++) {
	    matrix[i][j] = dde_x11_DGL_drom_data.object_lcstowcsmat[i][j];
	}
    }
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_update_lcstowcsmat_lokatfrm (at, from, up, concat)
     DtPoint3 at;
     DtPoint3 from;
     DtVector3 up;
     DtCompType concat;
{
    XdCompType Xcomp;

    switch (concat) {
    case DcPreConcatenate:  Xcomp = XdPreConcatenate; break;
    case DcPostConcatenate: Xcomp = XdPostConcatenate; break;
    case DcReplace:         Xcomp = XdReplace;
    }

    XdMatrixLookAtFrom(dde_x11_DGL_drom_data.object_lcstowcsmat,
		       (XdReal)at[0], (XdReal)at[1], (XdReal)at[2],
		       (XdReal)from[0], (XdReal)from[1], (XdReal)from[2],
		       (XdReal)up[0], (XdReal)up[1], (XdReal)up[2],
		       Xcomp);

    ddr_x11_DGL_drom_update_lcstowcsmat();
}
   
/*
 ======================================================================
 */

ddr_x11_DGL_drom_update_lcstowcsmat_rotate (axis, radians, concat)
     DtAxis axis;
     DtReal radians;
     DtCompType concat;
{
    XdCompType Xcomp;

    switch (concat) {
    case DcPreConcatenate:  Xcomp = XdPreConcatenate; break;
    case DcPostConcatenate: Xcomp = XdPostConcatenate; break;
    case DcReplace:         Xcomp = XdReplace;
    }

    switch(axis)	{
    case DcXAxis:
	XdMatrixRotateX(dde_x11_DGL_drom_data.object_lcstowcsmat,
			(XdReal)radians,Xcomp);
	break;

    case DcYAxis:
	XdMatrixRotateY(dde_x11_DGL_drom_data.object_lcstowcsmat,
			(XdReal)radians,Xcomp);
	break;

    case DcZAxis:
	XdMatrixRotateZ(dde_x11_DGL_drom_data.object_lcstowcsmat,
			(XdReal)radians,Xcomp);
	break;
    }
    ddr_x11_DGL_drom_update_lcstowcsmat();
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_update_lcstowcsmat_scale (sx, sy, sz, concat)
     DtReal sx, sy, sz;
     DtCompType concat;
{
    XdCompType Xcomp;

    switch (concat) {
    case DcPreConcatenate:  Xcomp = XdPreConcatenate; break;
    case DcPostConcatenate: Xcomp = XdPostConcatenate; break;
    case DcReplace:         Xcomp = XdReplace;
    }

    XdMatrixScale (dde_x11_DGL_drom_data.object_lcstowcsmat,
		   (XdReal)sx, (XdReal)sy, (XdReal)sz, Xcomp);

    ddr_x11_DGL_drom_update_lcstowcsmat();
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_update_lcstowcsmat_shear (plane, firstshearvalue, 
					   secondshearvalue, concat)
     DtMajorPlane plane;
     DtReal firstshearvalue, secondshearvalue;
     DtCompType concat;
{
    XdCompType Xcomp;

    switch (concat) {
    case DcPreConcatenate:  Xcomp = XdPreConcatenate; break;
    case DcPostConcatenate: Xcomp = XdPostConcatenate; break;
    case DcReplace:         Xcomp = XdReplace;
    }

    XdMatrixShear(dde_x11_DGL_drom_data.object_lcstowcsmat,
		  plane, (XdReal)firstshearvalue, 
		  (XdReal)secondshearvalue, Xcomp);
    ddr_x11_DGL_drom_update_lcstowcsmat();
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

ddr_x11_DGL_drom_update_lcstowcsmat_tfmmat (tfmmat, concat)
     DtMatrix4x4 tfmmat;
     DtCompType concat;
{
    DtInt i, j;
    XdMatrix4x4 matrix;
    XdCompType Xcomp;

    switch (concat) {
    case DcPreConcatenate:  Xcomp = XdPreConcatenate; break;
    case DcPostConcatenate: Xcomp = XdPostConcatenate; break;
    case DcReplace:         Xcomp = XdReplace;
    }

PRAGMA(IVDEP)
    for (i=0; i<4; i++) {
	for (j=0; j<4; j++) {
	    matrix[i][j] = tfmmat[i][j];
	}
    }
    XdMatrixConcat(dde_x11_DGL_drom_data.object_lcstowcsmat,
		   matrix, Xcomp);
    ddr_x11_DGL_drom_update_lcstowcsmat();
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_update_lcstowcsmat_transl (tx, ty, tz, concat)
     DtReal tx, ty, tz;
     DtCompType concat;
{
    XdCompType Xcomp;

    switch (concat) {
    case DcPreConcatenate:  Xcomp = XdPreConcatenate; break;
    case DcPostConcatenate: Xcomp = XdPostConcatenate; break;
    case DcReplace:         Xcomp = XdReplace;
    }

    XdMatrixTranslate(dde_x11_DGL_drom_data.object_lcstowcsmat,
		      (XdReal)tx, (XdReal)ty, (XdReal)tz, Xcomp);
    ddr_x11_DGL_drom_update_lcstowcsmat();
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_update_lcstowcsmat_push ()
{
    struct matrix_stack *ms;

    ms = &(dde_x11_DGL_drom_data.mat_stack);

    if (ms->current_element == ms->last_element) {
	ms->elements = (struct mat_element *)DDspace_Reallocate
		(ms->elements,((ms->last_element+1)+10)*
		 (sizeof (struct mat_element)));

	ms->last_element += 10;
    }
		
    ms->current_element++;
    XdMatrixLoad (ms->elements[ms->current_element].mat,
		  dde_x11_DGL_drom_data.object_lcstowcsmat);
}

/*
 ======================================================================
 */

ddr_x11_DGL_drom_update_lcstowcsmat_pop ()
{
    struct matrix_stack *ms;

    ms = &(dde_x11_DGL_drom_data.mat_stack);

    if (ms->current_element >= 0) {
	XdMatrixLoad (dde_x11_DGL_drom_data.object_lcstowcsmat,
		      ms->elements[ms->current_element].mat);
	ddr_x11_DGL_drom_update_lcstowcsmat();
	ms->current_element--;
    }
}
/*
 ======================================================================
 */

	/*  Local to world transformation matrix.  */

ddr_x11_DGL_drom_update_lcstowcsmat()
{
    XdSetModelingMatrix(dde_x11_DGL_drom_curdevdat->pGC,
			dde_x11_DGL_drom_data.object_lcstowcsmat);

    XdMatrixLoad(dde_x11_DGL_drom_curcamdat->lcstofcsmat,
		 dde_x11_DGL_drom_curcamdat->wcstofcsmat);
    XdMatrixConcat(dde_x11_DGL_drom_curcamdat->lcstofcsmat,
		   dde_x11_DGL_drom_data.object_lcstowcsmat,
		   XdPreConcatenate);
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

ddr_x11_DGL_drom_get_wcstofcsmat(wcstofcsmat)
     DtMatrix4x4 wcstofcsmat;

{
    DtInt i, j;
   
    for (i=0; i<4; i++) {
PRAGMA(IVDEP)
	for (j=0; j<4; j++) {
	    wcstofcsmat[i][j] = dde_x11_DGL_drom_curcamdat->wcstofcsmat[i][j];
	}
    }
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

ddr_x11_DGL_drom_push_lcstofcsmat(newmatrix)
     DtMatrix4x4 newmatrix;

{
    XdMatrix4x4 identmatrix;
    DtInt i, j;

    XdMatrixLoadIdentity(identmatrix);

    XdMatrixLoad(dde_x11_DGL_drom_curcamdat->lcstofcsmat_temp,
		 dde_x11_DGL_drom_curcamdat->lcstofcsmat);
   
    for (i=0; i<4; i++) {
PRAGMA(IVDEP)
	for (j=0; j<4; j++) {
	    dde_x11_DGL_drom_curcamdat->lcstofcsmat[i][j] = newmatrix[i][j];
	}
    }

#ifdef DEBUG
    dor_matrix_print(dde_x11_DGL_drom_curcamdat->lcstofcsmat);
#endif

    XdSetModelingMatrix(dde_x11_DGL_drom_curdevdat->pGC,
			identmatrix);
    XdSetProjectionMatrix(dde_x11_DGL_drom_curdevdat->pGC,
			  dde_x11_DGL_drom_curcamdat->lcstofcsmat);
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_pop_lcstofcsmat()

{
    XdMatrixLoad(dde_x11_DGL_drom_curcamdat->lcstofcsmat,
		 dde_x11_DGL_drom_curcamdat->lcstofcsmat_temp);

    XdSetModelingMatrix(dde_x11_DGL_drom_curdevdat->pGC,
			dde_x11_DGL_drom_data.object_lcstowcsmat);
    XdSetProjectionMatrix(dde_x11_DGL_drom_curdevdat->pGC,
			  dde_x11_DGL_drom_curcamdat->wcstofcsmat);
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

DtFlag ddr_x11_DGL_drom_transform_clip_z_point(point,tfmmatrix)
     DtRealTriple point;
     DtMatrix4x4 tfmmatrix;

{
    DtInt row, column;
    DtReal fcspoint[4];
    DtReal oneoverw;
    XdReal tfmpoint[3];
    XdMatrix4x4 matrix;
    DtInt i, j;

    /*  Transform the point.  */

    for (row=0;row<4;row++) {
	fcspoint[row] = (dde_x11_DGL_drom_curcamdat->lcstofcsmat)[row][3];

PRAGMA(IVDEP)
	for (column=0;column<3;column++) {
	    fcspoint[row] += 
		    (dde_x11_DGL_drom_curcamdat->lcstofcsmat)[row][column]
			    * point[column];
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

    XdMatrixLoadIdentity(matrix);

    XdMatrixTranslate(matrix,tfmpoint[0], tfmpoint[1], tfmpoint[2],
		      XdPreConcatenate);

    XdMatrixScale(matrix, 
		  (XdReal)dde_x11_DGL_drom_curdevdat->annoscale[0],
		  (XdReal)dde_x11_DGL_drom_curdevdat->annoscale[1],
		  (XdReal)dde_x11_DGL_drom_curdevdat->annoscale[2],
		  XdPreConcatenate);

#ifdef DEBUG
    printf ("xform_clip_z: oneoverw: %g, tfmpnt: (%g,%g,%g)\n",
	    oneoverw, tfmpoint[0], tfmpoint[1], tfmpoint[2]);
    printf ("              scale: (%g,%g,%g)\n",
	    dde_x11_DGL_drom_curdevdat->annoscale[0],
	    dde_x11_DGL_drom_curdevdat->annoscale[1],
	    dde_x11_DGL_drom_curdevdat->annoscale[2]);
    dor_matrix_print(tfmmatrix);
#endif

    for (i=0; i<4; i++) {
PRAGMA(IVDEP)
	for (j=0; j<4; j++) {
	    tfmmatrix[i][j] = matrix[i][j];
	}
    }

    return(DcFalse);
}
