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
	dor_pick_set_camera_matrix
	dor_pick_set_parallel_matrix
	dor_pick_set_perspective_matrix
	dor_pick_set_projection_matrix

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/matrix.h>
#include "pick.h"

/* ====================================================================== */

void dor_pick_set_camera_matrix (DtMatrix4x4 matrix)
{
    doe_pick_data.object_viewtype = dod_pick_viewtype_matrix;
	
    dor_matrix_load(doe_pick_data.object_projmat,matrix);
}

/* ====================================================================== */

void dor_pick_set_parallel_matrix (
    DtReal fov,
    DtReal hither,
    DtReal yon)
{
    DtReal yoverxscalefactor,extentx,extenty;
    DtVolume *volume;

    doe_pick_data.object_viewtype = dod_pick_viewtype_parallel;

    volume = dor_view_inq_boundary_ptr(doe_pick_data.view);

    extentx = volume->fur[0] - volume->bll[0];
    extenty = volume->fur[1] - volume->bll[1];

    yoverxscalefactor = extenty / extentx;

    if ( extentx >= extenty )
	dor_matrix_load_scale(doe_pick_data.object_projmat,
					yoverxscalefactor,1.,1.);
    else
	dor_matrix_load_scale(doe_pick_data.object_projmat,
					1.,1./yoverxscalefactor,1.);

    dor_matrix_parallel(doe_pick_data.object_projmat,
			fov,hither,yon,DcPostConcatenate);
}

/* ====================================================================== */

void dor_pick_set_perspective_matrix (
    DtReal fov,
    DtReal hither,
    DtReal yon)
{
    DtReal yoverxscalefactor,extentx,extenty;
    DtVolume *volume;

    doe_pick_data.object_viewtype = dod_pick_viewtype_perspective;

    volume = dor_view_inq_boundary_ptr(doe_pick_data.view);

    extentx = volume->fur[0] - volume->bll[0];
    extenty = volume->fur[1] - volume->bll[1];

    yoverxscalefactor = extenty / extentx;

    if (extentx >= extenty){
	dor_matrix_load_scale(doe_pick_data.object_projmat,
				yoverxscalefactor,1.,1.);
    } else{
	dor_matrix_load_scale(doe_pick_data.object_projmat,
				1.,1./yoverxscalefactor,1.);
    }

    dor_matrix_perspective(doe_pick_data.object_projmat,
			   fov,hither,yon,DcPostConcatenate);
}

/* ====================================================================== */

void dor_pick_set_projection_matrix (
    DtArea           *window,
    DtProjectionType  ptype,
    DtPoint3          prp,
    DtReal            view_plane,
    DtReal            hither,
    DtReal            yon)
{
    switch (ptype) {
	case DcParallel:
	    doe_pick_data.object_viewtype = dod_pick_viewtype_parallel;
	    break;

	case DcPerspective:
	    doe_pick_data.object_viewtype = dod_pick_viewtype_perspective;
	    break;
    }

    dor_matrix_load_projection(doe_pick_data.object_projmat,window,
			       ptype,prp,view_plane,hither,yon);
}
