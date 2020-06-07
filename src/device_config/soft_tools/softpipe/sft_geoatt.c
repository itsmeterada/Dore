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
	void ddr_softpipe_drom_get_lcstowcsmat (matrix)
	void ddr_softpipe_drom_update_lcstowcsmat_lokatfrm (at,from,up,concat)
	void ddr_softpipe_drom_update_lcstowcsmat_rotate (axis,radians,concat)
	void ddr_softpipe_drom_update_lcstowcsmat_scale (sx,sy,sz,concat)
	void ddr_softpipe_drom_update_lcstowcsmat_shear (plane,firstshearvalue, secondshearvalue,concat)
	void ddr_softpipe_drom_update_lcstowcsmat_tfmmat (tfmmat,concat)
	void ddr_softpipe_drom_update_lcstowcsmat_transl (tx,ty,tz,concat)
	void ddr_softpipe_drom_update_lcstowcsmat ()
	void ddr_softpipe_drom_update_lcstowcsmat_pop ()
	void ddr_softpipe_drom_update_lcstowcsmat_push ()
	void ddr_softpipe_drom_get_wcstofcsmat (wcstofcsmat)
	void ddr_softpipe_drom_pop_lcstofcsmat ()
	void ddr_softpipe_drom_push_lcstofcsmat (newmatrix)
	DtFlag ddr_softpipe_drom_transform_clip_z_point (point,tfmmatrix)

  ======================================================================
 */
#include "drom.h"
#include <dore/internal/matrix.h>

/*
 ======================================================================
 */
void ddr_softpipe_drom_get_lcstowcsmat (DtMatrix4x4 matrix)
{
   dor_matrix_load(matrix, dde_softpipe_drom_data.object_lcstowcsmat);
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_update_lcstowcsmat_lokatfrm (
    DtPoint3 at,
    DtPoint3 from,
    DtVector3 up,
    DtCompType concat)
{
#ifdef DEBUG
   printf("...processing dynout update lcstowcsmat lokatfrm\n");
   fflush(stdout);
#endif

   dor_matrix_look_at_from(dde_softpipe_drom_data.object_lcstowcsmat,
			   at[0],at[1],at[2],from[0],from[1],from[2],
			   up[0],up[1],up[2],
			   concat);
   ddr_softpipe_drom_update_lcstowcsmat();
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_update_lcstowcsmat_rotate (
    DtAxis axis,
    DtReal radians,
    DtCompType concat)
{
#ifdef DEBUG
   printf("...processing dynout update lcstowcsmat rotate\n");
   fflush(stdout);
#endif

   switch(axis) {
   case DcXAxis:
      dor_matrix_rotate_x(dde_softpipe_drom_data.object_lcstowcsmat,
			  radians,concat);
      break;

   case DcYAxis:
      dor_matrix_rotate_y(dde_softpipe_drom_data.object_lcstowcsmat,
			  radians,concat);
      break;

   case DcZAxis:
      dor_matrix_rotate_z(dde_softpipe_drom_data.object_lcstowcsmat,
			  radians,concat);
      break;
   }
   ddr_softpipe_drom_update_lcstowcsmat();
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_update_lcstowcsmat_scale (
    DtReal sx,
    DtReal sy,
    DtReal sz,
    DtCompType concat)
{
#ifdef DEBUG
   printf("...processing dynout update lcstowcsmat scale\n");
   fflush(stdout);
#endif

   dor_matrix_scale(dde_softpipe_drom_data.object_lcstowcsmat,
		    sx,sy,sz,concat);
   ddr_softpipe_drom_update_lcstowcsmat();
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_update_lcstowcsmat_shear (
    DtMajorPlane plane,
    DtReal       firstshearvalue,
    DtReal       secondshearvalue,
    DtCompType   concat)
{
#ifdef DEBUG
   printf("...processing dynout update lcstowcsmat shear\n");
   fflush(stdout);
#endif

   dor_matrix_shear(dde_softpipe_drom_data.object_lcstowcsmat,
		    plane,firstshearvalue,secondshearvalue,concat);
   ddr_softpipe_drom_update_lcstowcsmat();
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_update_lcstowcsmat_tfmmat (
    DtMatrix4x4 tfmmat,
    DtCompType  concat)
{
#ifdef DEBUG
   printf("...processing dynout update lcstowcsmat tfmmat\n");
   fflush(stdout);
#endif

   dor_matrix_concat(dde_softpipe_drom_data.object_lcstowcsmat,
		     tfmmat,concat);
   ddr_softpipe_drom_update_lcstowcsmat();
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_update_lcstowcsmat_transl (
    DtReal tx,
    DtReal ty,
    DtReal tz,
    DtCompType concat)
{
#ifdef DEBUG
   printf("...processing dynout update lcstowcsmat transl\n");
   fflush(stdout);
#endif

   dor_matrix_translate(dde_softpipe_drom_data.object_lcstowcsmat,
			tx,ty,tz,concat);
   ddr_softpipe_drom_update_lcstowcsmat();
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_update_lcstowcsmat (void)
{
#ifdef DEBUG
   printf("...processing dynout update lcstowcsmat \n");
   fflush(stdout);
#endif

   dor_matrix_load(dde_softpipe_drom_curcamdat->lcstofcsmat,
		   dde_softpipe_drom_curcamdat->wcstofcsmat);
   dor_matrix_concat(dde_softpipe_drom_curcamdat->lcstofcsmat,
		     dde_softpipe_drom_data.object_lcstowcsmat,DcPreConcatenate);

   if(!dde_softpipe_drom_data.draw_switch) {
      /*  World to local normal transformation matrix is the transpose of
	  the inverse of the wcstolcsmat.  Since lcstowcsmat is already the
	  inverse of wcstolcsmat, we merely need to take the transpose of this
	  matrix.  */

      dor_matrix_load(dde_softpipe_drom_data.nrmwcstolcsmat,
		      dde_softpipe_drom_data.object_lcstowcsmat);
      dor_matrix_transpose(dde_softpipe_drom_data.nrmwcstolcsmat);

      ddr_softpipe_drom_update_lights();
   }
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_update_lcstowcsmat_pop (void)
{
   struct matrix_stack *ms;

#ifdef DEBUG
   printf("...processing dynout update lcstowcsmat pop\n");
   fflush(stdout);
#endif

   ms = &(dde_softpipe_drom_data.mat_stack);

   if(ms->current_element >= 0) {
      dor_matrix_load(dde_softpipe_drom_data.object_lcstowcsmat,
		      ms->elements[ms->current_element].mat);
      ddr_softpipe_drom_update_lcstowcsmat();
      ms->current_element --;
   }
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_update_lcstowcsmat_push (void)
{
   struct matrix_stack *ms;

#ifdef DEBUG
   printf("...processing dynout update lcstowcsmat push\n");
   fflush(stdout);
#endif

   ms = &(dde_softpipe_drom_data.mat_stack);

   if(ms->current_element == ms->last_element) {
      ms->elements = (struct mat_element *)DDspace_Reallocate
	    (ms->elements, ((ms->last_element+1)+10)*(sizeof(struct mat_element)));

      ms->last_element += 10;
   }

   ms->current_element ++;
   dor_matrix_load(ms->elements[ms->current_element].mat,
		   dde_softpipe_drom_data.object_lcstowcsmat);
}

/*
 ======================================================================
 */
void ddr_softpipe_drom_get_wcstofcsmat (DtMatrix4x4 wcstofcsmat)
{
   dor_matrix_load(wcstofcsmat,dde_softpipe_drom_curcamdat->wcstofcsmat);
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_pop_lcstofcsmat (void)
{
   dor_matrix_load(dde_softpipe_drom_curcamdat->lcstofcsmat,
		   dde_softpipe_drom_curcamdat->lcstofcsmat_temp);
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_push_lcstofcsmat (DtMatrix4x4 newmatrix)
{
   dor_matrix_load(dde_softpipe_drom_curcamdat->lcstofcsmat_temp,
		   dde_softpipe_drom_curcamdat->lcstofcsmat);

   dor_matrix_load(dde_softpipe_drom_curcamdat->lcstofcsmat,newmatrix);
}
/*
 ======================================================================
 */
DtFlag ddr_softpipe_drom_transform_clip_z_point (
    DtRealTriple point,
    DtMatrix4x4 tfmmatrix)
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


   /* Transform the point. */

   for(row=0; row<4; row++) {
      fcspoint[row] =(dde_softpipe_drom_curcamdat->lcstofcsmat)[row][3];

      for(column=0; column<3; column++) {
	 fcspoint[row] +=
	       (dde_softpipe_drom_curcamdat->lcstofcsmat)[row][column] *
		     point[column];
      }
   }

   if(fcspoint[2] < -fcspoint[3])
	 return(DcTrue);
   if(fcspoint[2] > 0)
	 return(DcFalse);

   oneoverw = 1. / fcspoint[3];

   tfmpoint[0] = fcspoint[0] * oneoverw;
   tfmpoint[1] = fcspoint[1] * oneoverw;
   tfmpoint[2] = fcspoint[2] * oneoverw;

   dor_matrix_load_identity(tfmmatrix);

   dor_matrix_translate(tfmmatrix,tfmpoint[0],tfmpoint[1],tfmpoint[2],
			DcPreConcatenate);

   dor_matrix_scale(tfmmatrix,
		    dde_softpipe_drom_curdevdat->annoscale[0],
		    dde_softpipe_drom_curdevdat->annoscale[1],
		    dde_softpipe_drom_curdevdat->annoscale[2],
		    DcPreConcatenate);

   return(DcFalse);
}
