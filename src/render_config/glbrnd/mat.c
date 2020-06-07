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
	int drr_glbrnd_compute_wcstofcs ()
	int drr_glbrnd_get_wcstofcsmat (wcstofcsmat)
	int drr_glbrnd_set_camera_matrix (matrix)
	int drr_glbrnd_set_parallel_matrix (fov,hither,yon)
	int drr_glbrnd_set_perspective_matrix (fov,hither,yon)
	int drr_glbrnd_set_projection_matrix (window,ptype,prp,view_plane,hither,yon)

  ======================================================================
 */
#include "glbrnd.h"
#include <dore/internal/matrix.h>

/*
 ======================================================================
 */

     /* Transformation pipeline:

	Dore implements a rather nice pipeline to meet both simple 
	user models of viewing and projection and the PHIGS pipeline.  
	The following coordinate systems and transformations are 
	employed.

		Modelling (Local) Coordinates 
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

void 
drr_glbrnd_compute_wcstofcs (void)
{
    DtReal xscale, yscale, zscale;
    DtReal xtrans, ytrans, ztrans, wtrans;
    DtMatrix4x4 identmatrix;
    int status;
    DtVolume *clipvolume;
    DtVolume *viewboundary;
    DtReal xres, yres;

    /* map from WC to VRC */

    dor_matrix_load(dre_glbrnd_curcamdat->wcstofcsmat,
			dre_glbrnd_curcamdat->wcstoccsmat);

#ifdef debug
    printf("cmp:  wcstoccsmat:\n");
    dor_matrix_print(dre_glbrnd_curcamdat->wcstoccsmat);
#endif

    /* map from VRC to canonical volume for single view */

    dor_matrix_post_concatenate(dre_glbrnd_curcamdat->wcstofcsmat,
				dre_glbrnd_curcamdat->projmat);

#ifdef debug
    printf("cmp:  wcstofcsmat after projat post_concatenate:\n");
    dor_matrix_print(dre_glbrnd_curcamdat->wcstofcsmat);
#endif

    /* map from canonical volume to projection viewport in NPC */

    if (dre_glbrnd_curcamdat->viewtype != dod_glbrnd_viewtype_matrix){
	viewboundary = 
		dor_view_inq_boundary_ptr(dre_glbrnd_data.current_view);

	xscale = (viewboundary->fur[0]-viewboundary->bll[0])/2.;
	yscale = (viewboundary->fur[1]-viewboundary->bll[1])/2.;
	zscale = (viewboundary->fur[2]-viewboundary->bll[2]);

	xtrans = (viewboundary->fur[0]+viewboundary->bll[0])/2.;
	ytrans = (viewboundary->fur[1]+viewboundary->bll[1])/2.;
	ztrans = viewboundary->fur[2];

	/*  get the screen resolution to account for 
	    any aspect ratio mismatch from the view system
	    This is generally due to a stereo screen */

	DdInqResolution(dre_glbrnd_data.current_device,
			&xres,&yres);
	if (xres != 0.0) {
	    xscale *= (yres / xres);
	}

	dor_matrix_scale(dre_glbrnd_curcamdat->wcstofcsmat,
			 xscale,yscale,zscale,DcPostConcatenate);

	dor_matrix_translate(dre_glbrnd_curcamdat->wcstofcsmat,
			     xtrans,ytrans,ztrans,DcPostConcatenate);
    }

#ifdef debug
    printf("cmp:  wcstofcsmat after map to npc:\n");
    dor_matrix_print(dre_glbrnd_curcamdat->wcstofcsmat);
#endif

    /* map from clipping region in NPC to canonical clipping volume */

    clipvolume = dor_device_inq_clipping_volume_ptr(dre_glbrnd_data.current_device);

    xscale = 2./(clipvolume->fur[0]-clipvolume->bll[0]);
    yscale = 2./(clipvolume->fur[1]-clipvolume->bll[1]);
    zscale = 1./(clipvolume->fur[2]-clipvolume->bll[2]);

    xtrans = -xscale*clipvolume->bll[0]-1;
    ytrans = -yscale*clipvolume->bll[1]-1;
    ztrans = -zscale*clipvolume->fur[2];

    dor_matrix_scale(dre_glbrnd_curcamdat->wcstofcsmat,
			xscale,yscale,zscale,
		     	DcPostConcatenate);

    dor_matrix_translate(dre_glbrnd_curcamdat->wcstofcsmat,
			xtrans,ytrans,ztrans,
			DcPostConcatenate);

#ifdef debug
    printf("cmp:  wcstofcsmat after map to canonical:\n");
    dor_matrix_print(dre_glbrnd_curcamdat->wcstofcsmat);
#endif

    /* compute camera type, location and normal */

    dor_matrix_load_identity(identmatrix);

    status = dor_matrix_solve(dre_glbrnd_curcamdat->wcstofcsmat,
			      	dre_glbrnd_curcamdat->npctowcsmat,
				identmatrix);

    if (status != 10){
	DDerror (ERR_CANT_INVERT_MATRIX,
		    "drr_glbrnd_compute_wcstofcs", DcNullPtr);
	return;
    }

#ifdef debug
    printf("cmp:  wcstofcsmat after solve:\n");
    dor_matrix_print(dre_glbrnd_curcamdat->wcstofcsmat);
#endif

    dor_math_homo_trns(0.,0.,1.,0.,dre_glbrnd_curcamdat->npctowcsmat,
		       &xtrans,&ytrans,&ztrans,&wtrans);

    /* if wtrans is close to zero, the camera is so far away that
       it doesn't make sense to do a perspective projection, so
       we switch to parallel */
    if (( abs(wtrans) > 1.e-20 ) &&
	(dre_glbrnd_curcamdat->viewtype == 
			dod_glbrnd_viewtype_perspective)){
	dre_glbrnd_curcamdat->camtype = DcPerspective;
	dre_glbrnd_curcamdat->wcscamloc[0] = xtrans/wtrans;
	dre_glbrnd_curcamdat->wcscamloc[1] = ytrans/wtrans;
	dre_glbrnd_curcamdat->wcscamloc[2] = ztrans/wtrans;

#ifdef debug
	printf("positional viewpoint wtrans = %f\n", wtrans);
	printf("eye point = (%f,%f,%f)\n",
	       dre_glbrnd_curcamdat->wcscamloc[0],
	       dre_glbrnd_curcamdat->wcscamloc[1],
	       dre_glbrnd_curcamdat->wcscamloc[2]);
#endif
    } else{
	dre_glbrnd_curcamdat->camtype = DcParallel;

#ifdef debug
	printf("directional viewpoint wtrans = %f\n", wtrans);
	fflush(stdout);
#endif
    }

    dor_math_vectrns(0.,0.,1.,
		     dre_glbrnd_curcamdat->npctowcsmat,
		     &dre_glbrnd_curcamdat->wcscamnorm[0],
		     &dre_glbrnd_curcamdat->wcscamnorm[1],
		     &dre_glbrnd_curcamdat->wcscamnorm[2]);

    dor_math_renormalize_fast(dre_glbrnd_curcamdat->wcscamnorm);

#ifdef debug
    printf("camera normal = (%f,%f,%f)\n",
	   dre_glbrnd_curcamdat->wcscamnorm[0],
	   dre_glbrnd_curcamdat->wcscamnorm[1],
	   dre_glbrnd_curcamdat->wcscamnorm[2]);
#endif
}
/*
 ======================================================================
 */

void drr_glbrnd_get_wcstofcsmat (DtMatrix4x4 wcstofcsmat)
{
    dor_matrix_load(wcstofcsmat,dre_glbrnd_curcamdat->wcstofcsmat);
}
/*
 ======================================================================
 */

void drr_glbrnd_set_camera_matrix (DtMatrix4x4 matrix)
{
       dre_glbrnd_data.viewtype = dod_glbrnd_viewtype_matrix;
	
       dor_matrix_load(dre_glbrnd_data.projmat,matrix);
}
/*
 ======================================================================
 */

void drr_glbrnd_set_parallel_matrix (
    DtReal fov,
    DtReal hither,
    DtReal yon)
{
    DtReal yoverxscalefactor,extentx,extenty;
    DtVolume *volume;

    dre_glbrnd_data.viewtype = dod_glbrnd_viewtype_parallel;

    volume = dor_view_inq_boundary_ptr(dre_glbrnd_data.current_view);

    extentx = volume->fur[0] - volume->bll[0];
    extenty = volume->fur[1] - volume->bll[1];

    yoverxscalefactor = extenty / extentx;

    if ( extentx >= extenty )
	    dor_matrix_load_scale(dre_glbrnd_data.projmat,
				  yoverxscalefactor,1.,1.);
    else
	    dor_matrix_load_scale(dre_glbrnd_data.projmat,1.,
				  1./yoverxscalefactor,1.);

    dor_matrix_parallel(dre_glbrnd_data.projmat,fov,hither,yon,
			DcPostConcatenate);
}
/*
 ======================================================================
 */

void drr_glbrnd_set_perspective_matrix (
    DtReal fov,
    DtReal hither,
    DtReal yon)
{
    DtReal yoverxscalefactor,extentx,extenty;
    DtVolume *volume;

    dre_glbrnd_data.viewtype = dod_glbrnd_viewtype_perspective;

    volume = dor_view_inq_boundary_ptr(dre_glbrnd_data.current_view);

    extentx = volume->fur[0] - volume->bll[0];
    extenty = volume->fur[1] - volume->bll[1];

    yoverxscalefactor = extenty / extentx;

    if (extentx >= extenty){
	dor_matrix_load_scale(dre_glbrnd_data.projmat,
			      yoverxscalefactor,1.,1.);
    } else{
	dor_matrix_load_scale(dre_glbrnd_data.projmat,1.,
			      1./yoverxscalefactor,1.);
    }

    dor_matrix_perspective(dre_glbrnd_data.projmat,fov,hither,yon,
			   DcPostConcatenate);
}
/*
 ======================================================================
 */

void drr_glbrnd_set_projection_matrix (
    DtArea           *window,
    DtProjectionType  ptype,
    DtPoint3          prp,
    DtReal            view_plane,
    DtReal            hither,
    DtReal            yon)
{
    switch (ptype) {
	case DcParallel:
	    dre_glbrnd_data.viewtype = dod_glbrnd_viewtype_parallel;
	    break;

	case DcPerspective:
	    dre_glbrnd_data.viewtype = dod_glbrnd_viewtype_perspective;
	    break;
    }
	
    dor_matrix_load_projection(dre_glbrnd_data.projmat,window,
				ptype,prp,view_plane,hither,yon);
}
