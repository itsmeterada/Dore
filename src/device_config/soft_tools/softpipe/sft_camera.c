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
	void ddr_softpipe_drom_camera ()
	void ddr_softpipe_drom_compute_wcstofcs ()
	void ddr_softpipe_drom_set_att_stereo (eyeseparation,distance)
	void ddr_softpipe_drom_set_att_stereoswi (switchvalue)
	void ddr_softpipe_drom_set_camera_matrix (matrix)
	void ddr_softpipe_drom_set_parallel_matrix (fov,hither,yon)
	void ddr_softpipe_drom_set_perspective_matrix (fov,hither,yon)
	void ddr_softpipe_drom_set_projection_matrix (window,ptype,prp,view_plane, hither,yon)

  ======================================================================
 */
#include "drom.h"
#include <dore/internal/matrix.h>

/*
 ======================================================================
 */
void ddr_softpipe_drom_camera (void)
{
#ifdef DEBUG
   printf("...processing dynout init camera \n");
   fflush(stdout);
#endif

   dor_matrix_load(dde_softpipe_drom_curcamdat->projmat, /* ADD */
		   dde_softpipe_drom_data.projmat);
   dde_softpipe_drom_curcamdat->viewtype = 
	 dde_softpipe_drom_data.viewtype; /* */

   dor_matrix_load(dde_softpipe_drom_curcamdat->ccstowcsmat,
		   dde_softpipe_drom_data.object_lcstowcsmat);
}
/*
 ======================================================================
 */
/* Transformation pipeline:

   Dore implements a rather nice pipeline to meet both simple user
   models of viewing and projection and the PHIGS pipeline.  The
   following coordinate systems and transformations are employed.

		Modelling ()Coordinates 
				|
		MODELLING TRANSFORMATIONS
				|
		World Coordinates
				|
		VIEW MAPPING TRANSFORMATION
				|
		View Reference (camera coordinates)
				|
		VIEW PROJECTION TRANSFORMATION
				|
		View Coordinates
				|
		Frame Coordinates
				|
		Clipping, w-divide, and WORKSTATION TRANSFORMATION
				|
		Device Coordinates

   The modelling transformations are specified as objects in the
   tree.

   The view mapping matrix can be specified directly for the view
   or using modelling matrices and a camera.

   The view projection transformation can be specified directly
   for the view or can be specified using the perspective
   structure element.  This is then combined with a transformation
   from the cannonical clipping volume to the projection
   viewport.

   The workstation transformation is specified by calls to set the
   workstation window and workstation viewport.  */

void ddr_softpipe_drom_compute_wcstofcs (void)
{
   DtReal xscale, yscale, zscale;
   DtReal xtrans, ytrans, ztrans, wtrans;
   DtMatrix4x4 identmatrix;
   int status;
   DtVolume *clipvolume;
   DtVolume viewboundary;

#ifdef DEBUG
   printf("...processing dynout compute wcstofcs \n");
   fflush(stdout);
#endif

   /* map from WC to VRC */

   dor_matrix_load(dde_softpipe_drom_curcamdat->wcstofcsmat,
		   dde_softpipe_drom_curcamdat->wcstoccsmat);

   /* map from VRC to canonical volume for single view */

   dor_matrix_post_concatenate(
			       dde_softpipe_drom_curcamdat->wcstofcsmat,
			       dde_softpipe_drom_curcamdat->projmat);

   /* map from canonical volume to projection viewport in NPC */

      DvInqBoundary(dde_softpipe_drom_data.current_view, &viewboundary);

      xscale = (viewboundary.fur[0]-viewboundary.bll[0])/2.;
      yscale = (viewboundary.fur[1]-viewboundary.bll[1])/2.;
      zscale = (viewboundary.fur[2]-viewboundary.bll[2]);

      xtrans = (viewboundary.fur[0]+viewboundary.bll[0])/2.;
      ytrans = (viewboundary.fur[1]+viewboundary.bll[1])/2.;
      ztrans = viewboundary.fur[2];

      dor_matrix_scale(dde_softpipe_drom_curcamdat->wcstofcsmat,
		       xscale,yscale,zscale,DcPostConcatenate);

      dor_matrix_translate(
			   dde_softpipe_drom_curcamdat->wcstofcsmat,
			   xtrans,ytrans,ztrans,DcPostConcatenate);

   /* map from clipping region in NPC to canonical clipping volume */

   clipvolume = DDdevice_InqClippingVolume(dde_softpipe_drom_data.current_device);

   xscale = 2./(clipvolume->fur[0]-clipvolume->bll[0]);
   yscale = 2./(clipvolume->fur[1]-clipvolume->bll[1]);
   zscale = 1./(clipvolume->fur[2]-clipvolume->bll[2]);

   xtrans = -xscale*clipvolume->bll[0]-1;
   ytrans = -yscale*clipvolume->bll[1]-1;
   ztrans = -zscale*clipvolume->fur[2];

   dor_matrix_scale(dde_softpipe_drom_curcamdat->wcstofcsmat,
		    xscale,yscale,zscale,DcPostConcatenate);

   dor_matrix_translate(dde_softpipe_drom_curcamdat->wcstofcsmat,
			xtrans,ytrans,ztrans,DcPostConcatenate);

   /* compute camera type, location and normal */

   dor_matrix_load_identity(identmatrix);

   status = dor_matrix_solve(dde_softpipe_drom_curcamdat->wcstofcsmat,
			     dde_softpipe_drom_curcamdat->npctowcsmat,identmatrix);

   if(status != 10) {
      DDerror(ERR_CANT_INVERT_MATRIX,
		 "ddr_softpipe_drom_compute_ctm", DcNullPtr);
      return;
   }

   dor_math_homo_trns(0.,0.,1.,0.,
		      dde_softpipe_drom_curcamdat->npctowcsmat,
		      &xtrans,&ytrans,&ztrans,&wtrans);

   /* if wtrans is close to zero, the camera is so far away that
       it doesn't make sense to do a perspective projection, so
       we switch to parallel */
   if((abs(wtrans) > 1.e-20) && (dde_softpipe_drom_curcamdat->viewtype 
				 == ddd_softpipe_drom_viewtype_perspective)) {
      dde_softpipe_drom_curcamdat->camtype = DcPerspective;
      dde_softpipe_drom_curcamdat->wcscamloc[0] = xtrans/wtrans;
      dde_softpipe_drom_curcamdat->wcscamloc[1] = ytrans/wtrans;
      dde_softpipe_drom_curcamdat->wcscamloc[2] = ztrans/wtrans;

   }
   else {
      dde_softpipe_drom_curcamdat->camtype = DcParallel;
   }

   dor_math_vectrns(0.,0.,1.,
		    dde_softpipe_drom_curcamdat->npctowcsmat,
		    &dde_softpipe_drom_curcamdat->wcscamnorm[0],
		    &dde_softpipe_drom_curcamdat->wcscamnorm[1],
		    &dde_softpipe_drom_curcamdat->wcscamnorm[2]);

   ddr_softpipe_drom_renormalize(
				  dde_softpipe_drom_curcamdat->wcscamnorm);
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_stereo (
    DtReal eyeseparation,
    DtReal distance)
{
#ifdef DEBUG
   printf("...processing dynout set att stereo\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.stereo_eyeseparation = eyeseparation;
   dde_softpipe_drom_data.stereo_distance = distance;
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_stereoswi (DtSwitch switchvalue)
{
#ifdef DEBUG
   printf("...processing dynout set att stereoswi\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.stereo_switch = switchvalue;
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_camera_matrix (DtMatrix4x4 matrix)
{
#ifdef DEBUG
   printf("...processing dynout set camera matrix\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.viewtype =
	 ddd_softpipe_drom_viewtype_matrix;

   dor_matrix_load(dde_softpipe_drom_data.projmat,matrix);
#ifdef DEBUG
   dor_matrix_print(matrix);
#endif
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_parallel_matrix (
    DtReal fov,
    DtReal hither,
    DtReal yon)
{
   DtReal yoverxscalefactor, extentx, extenty;
   DtVolume volume;

#ifdef DEBUG
   printf("...processing dynout set parallel matrix\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.viewtype = ddd_softpipe_drom_viewtype_parallel;

   DvInqBoundary(dde_softpipe_drom_data.current_view, &volume);

   extentx = volume.fur[0] - volume.bll[0];
   extenty = volume.fur[1] - volume.bll[1];

   yoverxscalefactor = extenty / extentx;

   if(extentx >= extenty)
	 dor_matrix_load_scale(dde_softpipe_drom_data.projmat,
			       yoverxscalefactor,1.,1.);
   else
	 dor_matrix_load_scale(dde_softpipe_drom_data.projmat,
			       1.,1./yoverxscalefactor,1.);

   dor_matrix_parallel(dde_softpipe_drom_data.projmat,
		       fov,hither,yon,DcPostConcatenate);

#ifdef DEBUG
   dor_matrix_print(dde_softpipe_drom_data.projmat);
#endif
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_perspective_matrix (
    DtReal fov,
    DtReal hither,
    DtReal yon)
{
   DtReal yoverxscalefactor, extentx, extenty;
   DtVolume volume;

#ifdef DEBUG
   printf("...processing dynout set perspective matrix\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.viewtype 
	 = ddd_softpipe_drom_viewtype_perspective;

   DvInqBoundary(dde_softpipe_drom_data.current_view, &volume);

   extentx = volume.fur[0] - volume.bll[0];
   extenty = volume.fur[1] - volume.bll[1];

   yoverxscalefactor = extenty / extentx;

   if(extentx >= extenty) {
      dor_matrix_load_scale(dde_softpipe_drom_data.projmat,
			    yoverxscalefactor,1.,1.);
   }
   else {
      dor_matrix_load_scale(dde_softpipe_drom_data.projmat,
			    1.,1./yoverxscalefactor,1.);
   }

   dor_matrix_perspective(dde_softpipe_drom_data.projmat,
			  fov,hither,yon,DcPostConcatenate);

#ifdef DEBUG
   dor_matrix_print(dde_softpipe_drom_data.projmat);
#endif
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_projection_matrix (
    DtArea           *window,
    DtProjectionType  ptype,
    DtPoint3          prp,
    DtReal            view_plane,
    DtReal            hither,
    DtReal            yon)
{
#ifdef DEBUG
   printf("...processing dynout set projection matrix\n");
   fflush(stdout);
#endif

   switch(ptype) {
   case DcParallel:
      dde_softpipe_drom_curcamdat->viewtype =
	    ddd_softpipe_drom_viewtype_parallel;
      break;

   case DcPerspective:
      dde_softpipe_drom_curcamdat->viewtype =
	    ddd_softpipe_drom_viewtype_perspective;
      break;
   }

   dor_matrix_load_projection(dde_softpipe_drom_data.projmat,
			      window,ptype,prp,view_plane,hither,yon);
#ifdef DEBUG
   printf("window = (%lf,%lf)-(%lf,%lf)\n",
	  window->ll[0],window->ll[1],window->ur[0],window->ur[1]);
   printf("prp = (%lf,%lf,%lf)\n",prp[0],prp[1],prp[2]);
   printf("viewplane = %lf\n",view_plane);
   printf("hither = %lf, yon = %lf\n",hither,yon);
   dor_matrix_print(dde_softpipe_drom_data.projmat);
#endif
}
