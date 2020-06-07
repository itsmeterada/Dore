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
	ddr_x11_titang2_drom_get_lcstowcsmat ddr_x11_titang2_drom_get_lcstowcsmat (matrix)
	ddr_x11_titang2_drom_update_lcstowcsmat_lokatfrm ddr_x11_titang2_drom_update_lcstowcsmat_lokatfrm (at, from, up, concat)
	ddr_x11_titang2_drom_update_lcstowcsmat_rotate ddr_x11_titang2_drom_update_lcstowcsmat_rotate (axis, radians, concat)
	ddr_x11_titang2_drom_update_lcstowcsmat_scale ddr_x11_titang2_drom_update_lcstowcsmat_scale (sx, sy, sz, concat)
	ddr_x11_titang2_drom_update_lcstowcsmat_shear ddr_x11_titang2_drom_update_lcstowcsmat_shear (plane, firstshearvalue, secondshearvalue, concat)
	ddr_x11_titang2_drom_update_lcstowcsmat_tfmmat ddr_x11_titang2_drom_update_lcstowcsmat_tfmmat (tfmmat, concat)
	ddr_x11_titang2_drom_update_lcstowcsmat_transl ddr_x11_titang2_drom_update_lcstowcsmat_transl (tx, ty, tz, concat)
	ddr_x11_titang2_drom_update_lcstowcsmat_push ddr_x11_titang2_drom_update_lcstowcsmat_push ()
	ddr_x11_titang2_drom_update_lcstowcsmat_pop ddr_x11_titang2_drom_update_lcstowcsmat_pop ()
	int ddr_x11_titang2_drom_update_lcstowcsmat ()
	int ddr_x11_titang2_drom_get_wcstofcsmat (wcstofcsmat)
	int ddr_x11_titang2_drom_push_lcstofcsmat (newmatrix)
	int ddr_x11_titang2_drom_pop_lcstofcsmat ()
	DtFlag ddr_x11_titang2_drom_transform_clip_z_point (point,tfmmatrix)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */

ddr_x11_titang2_drom_get_lcstowcsmat (matrix)
     DtMatrix4x4 matrix;
{
    dor_matrix_load(matrix, dde_x11_titang2_drom_data.object_lcstowcsmat);
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_update_lcstowcsmat_lokatfrm (at, from, up, concat)
     DtPoint3 at;
     DtPoint3 from;
     DtVector3 up;
     DtCompType concat;
{
    dor_matrix_look_at_from(dde_x11_titang2_drom_data.object_lcstowcsmat,
			    at[0],at[1],at[2],
			    from[0],from[1],from[2],
			    up[0],up[1],up[2],
			    concat);
    ddr_x11_titang2_drom_update_lcstowcsmat();
}
   
/*
 ======================================================================
 */

ddr_x11_titang2_drom_update_lcstowcsmat_rotate (axis, radians, concat)
     DtAxis axis;
     DtReal radians;
     DtCompType concat;
{
    switch(axis)	{
    case DcXAxis:
	dor_matrix_rotate_x(dde_x11_titang2_drom_data.object_lcstowcsmat,
			    radians,concat);
	break;

    case DcYAxis:
	dor_matrix_rotate_y(dde_x11_titang2_drom_data.object_lcstowcsmat,
			    radians,concat);
	break;

    case DcZAxis:
	dor_matrix_rotate_z(dde_x11_titang2_drom_data.object_lcstowcsmat,
			    radians,concat);
	break;
    }
    ddr_x11_titang2_drom_update_lcstowcsmat();
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_update_lcstowcsmat_scale (sx, sy, sz, concat)
     DtReal sx, sy, sz;
     DtCompType concat;
{
    dor_matrix_scale (dde_x11_titang2_drom_data.object_lcstowcsmat,
		      sx, sy, sz, concat);
    ddr_x11_titang2_drom_update_lcstowcsmat();
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_update_lcstowcsmat_shear (plane, firstshearvalue, 
					       secondshearvalue, concat)
     DtMajorPlane plane;
     DtReal firstshearvalue, secondshearvalue;
     DtCompType concat;
{
    dor_matrix_shear(dde_x11_titang2_drom_data.object_lcstowcsmat,
		     plane, firstshearvalue, secondshearvalue, concat);
    ddr_x11_titang2_drom_update_lcstowcsmat();
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_update_lcstowcsmat_tfmmat (tfmmat, concat)
     DtMatrix4x4 tfmmat;
     DtCompType concat;
{
    dor_matrix_concat(dde_x11_titang2_drom_data.object_lcstowcsmat,
		      tfmmat, concat);
    ddr_x11_titang2_drom_update_lcstowcsmat();
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_update_lcstowcsmat_transl (tx, ty, tz, concat)
     DtReal tx, ty, tz;
     DtCompType concat;
{
    dor_matrix_translate(dde_x11_titang2_drom_data.object_lcstowcsmat,
			 tx, ty, tz, concat);
    ddr_x11_titang2_drom_update_lcstowcsmat();
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_update_lcstowcsmat_push ()
{
    struct matrix_stack *ms;

    ms = &(dde_x11_titang2_drom_data.mat_stack);

    if (ms->current_element == ms->last_element) {
	ms->elements = (struct mat_element *)DDspace_Reallocate
		(ms->elements,((ms->last_element+1)+10)*
		 (sizeof (struct mat_element)));

	ms->last_element += 10;
    }
		
    ms->current_element++;
    dor_matrix_load (ms->elements[ms->current_element].mat,
		     dde_x11_titang2_drom_data.object_lcstowcsmat);
}

/*
 ======================================================================
 */

ddr_x11_titang2_drom_update_lcstowcsmat_pop ()
{
    struct matrix_stack *ms;

    ms = &(dde_x11_titang2_drom_data.mat_stack);

    if (ms->current_element >= 0) {
	dor_matrix_load (dde_x11_titang2_drom_data.object_lcstowcsmat,
			 ms->elements[ms->current_element].mat);
	ddr_x11_titang2_drom_update_lcstowcsmat();
	ms->current_element--;
    }
}
/*
 ======================================================================
 */

	/*  Local to world transformation matrix.  */

ddr_x11_titang2_drom_update_lcstowcsmat()
{
    dor_matrix_load(dde_x11_titang2_drom_curcamdat->lcstofcsmat,
		    dde_x11_titang2_drom_curcamdat->wcstofcsmat);
    dor_matrix_concat(dde_x11_titang2_drom_curcamdat->lcstofcsmat,
		      dde_x11_titang2_drom_data.object_lcstowcsmat,
		      DcPreConcatenate);

    if (!dde_x11_titang2_drom_data.draw_switch) {
	/*  World to local normal transformation matrix is the transpose of
	    the inverse of the wcstolcsmat.  Since lcstowcsmat is already the
	    inverse of wcstolcsmat, we merely need to take the transpose of this
	    matrix.  */

	dor_matrix_load(dde_x11_titang2_drom_data.nrmwcstolcsmat,
			dde_x11_titang2_drom_data.object_lcstowcsmat);
	dor_matrix_transpose(dde_x11_titang2_drom_data.nrmwcstolcsmat);

	ddr_x11_titang2_drom_update_lights();
    }
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_get_wcstofcsmat(wcstofcsmat)
     DtMatrix4x4 wcstofcsmat;

{
    dor_matrix_load(wcstofcsmat,dde_x11_titang2_drom_curcamdat->wcstofcsmat);
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_push_lcstofcsmat(newmatrix)
     DtMatrix4x4 newmatrix;

{

    dor_matrix_load(dde_x11_titang2_drom_curcamdat->lcstofcsmat_temp,
		    dde_x11_titang2_drom_curcamdat->lcstofcsmat);
   
    dor_matrix_load(dde_x11_titang2_drom_curcamdat->lcstofcsmat,newmatrix);
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_pop_lcstofcsmat()

{
    dor_matrix_load(dde_x11_titang2_drom_curcamdat->lcstofcsmat,
		    dde_x11_titang2_drom_curcamdat->lcstofcsmat_temp);
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

DtFlag ddr_x11_titang2_drom_transform_clip_z_point(point,tfmmatrix)
     DtRealTriple point;
     DtMatrix4x4 tfmmatrix;

{
    DtInt row, column;
    DtReal fcspoint[4];
    DtReal oneoverw;
    DtRealTriple tfmpoint;

    /*  Transform the point.  */

PRAGMA(IVDEP)
    for (row=0;row<4;row++) {
	fcspoint[row] = (dde_x11_titang2_drom_curcamdat->lcstofcsmat)[row][3];

	for (column=0;column<3;column++) {
	    fcspoint[row] += 
		    (dde_x11_titang2_drom_curcamdat->lcstofcsmat)[row][column]
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

    dor_matrix_load_identity(tfmmatrix);

    dor_matrix_translate(tfmmatrix,tfmpoint[0],tfmpoint[1],tfmpoint[2],
			 DcPreConcatenate);

    dor_matrix_scale(tfmmatrix,
		     dde_x11_titang2_drom_curdevdat->annoscale[0],
		     dde_x11_titang2_drom_curdevdat->annoscale[1],
		     dde_x11_titang2_drom_curdevdat->annoscale[2],
		     DcPreConcatenate);

    return(DcFalse);
}
