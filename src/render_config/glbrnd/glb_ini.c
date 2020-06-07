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
	int drr_glbrnd_initialize_before_environment ()
	int drr_glbrnd_initialize_environment ()
	int drr_glbrnd_initialize_after_environment ()
	int drr_glbrnd_initialize_ccstowcsmat ()
	int drr_glbrnd_initialize_object (object)
	int drr_glbrnd_initialize_object_modifier (lcstowcsflag)

  ======================================================================
 */
#include "glbrnd.h"
#include <dore/internal/glbatt.h>
#include <dore/internal/matrix.h>

/*
 ======================================================================
 */

void drr_glbrnd_initialize_before_environment (void)
{

    /*  The lcstowcsmat attribute is always initialized to the
	identity matrix.  When the initialize camera method is 
	being executed on the definition object, the lcstowcsmat 
	attribute represents the current local to world 
	transformation for the camera.  When the camera is executed, 
	the current value of this transformation represents the 
	final local to world transformation for the camera, and 
	will be loaded into the render_data data structure.  */

    dre_glbrnd_data.viewtype = dod_glbrnd_viewtype_perspective;

    dor_matrix_load_perspective(dre_glbrnd_data.projmat,
				90.,-0.01,-1.);

    dor_matrix_load(dre_glbrnd_curcamdat->projmat, 
		    dre_glbrnd_data.projmat);
    dre_glbrnd_curcamdat->viewtype = dre_glbrnd_data.viewtype;

    drr_glbrnd_initialize_ccstowcsmat();

    drr_rayint_initialize_camera(DcNullObject);

    drr_glbshd_initialize_before_environment();

    drr_glbshd_initialize_camera();
}
/*
 ======================================================================
 */

    /*  The purpose of environmental initialization is to collect
	data related to the cameras and lights.  The
	camera data always includes the camera's local to world
	transform (which is inverted to obtain the world to camera
	transform).  */

void drr_glbrnd_initialize_environment (void)
{
    drr_glbrnd_initialize_before_environment();

    DDview_TraverseStudio (dre_glbrnd_data.current_view,
			     dre_glbrnd_data.studio_method);

    drr_glbrnd_initialize_after_environment();
}
/*
 ======================================================================
 */

void drr_glbrnd_initialize_after_environment (void)
{
    dor_matrix_load(dre_glbrnd_curcamdat->wcstoccsmat,
		    dre_glbrnd_curcamdat->ccstowcsmat);
    dor_matrix_invert(dre_glbrnd_curcamdat->wcstoccsmat);

    drr_glbrnd_compute_wcstofcs();

    dor_math_pnttrns(0.,0.,0.,
		     dre_glbrnd_curcamdat->ccstowcsmat,
		     &dre_glbrnd_curcamdat->wcscamloc[0],
		     &dre_glbrnd_curcamdat->wcscamloc[1],
		     &dre_glbrnd_curcamdat->wcscamloc[2]);

    dor_math_vectrns(0.,0.,-1.,
		     dre_glbrnd_curcamdat->ccstowcsmat,
		     &dre_glbrnd_curcamdat->wcscamnorm[0],
		     &dre_glbrnd_curcamdat->wcscamnorm[1],
		     &dre_glbrnd_curcamdat->wcscamnorm[2]);

    dor_math_renormalize_fast(dre_glbrnd_curcamdat->wcscamnorm);

    drr_glbshd_initialize_after_environment();
}
/*
 ======================================================================
 */

void drr_glbrnd_initialize_ccstowcsmat (void)
{
    DtMatrix4x4 lcstowcsmat;

    dor_global_lcstowcsmat_get_value (lcstowcsmat);
    dor_matrix_load (dre_glbrnd_curcamdat->ccstowcsmat, lcstowcsmat);
}
/*
 ======================================================================
 */

	/*  Initialize an individual object.  */

void drr_glbrnd_initialize_object (dot_object *object)
{
    drr_rayint_initialize_object(object);
}
/*
 ======================================================================
 */

	/*  Initialize an object modifier.  */

void drr_glbrnd_initialize_object_modifier (DtFlag lcstowcsflag)
{
    drr_rayint_initialize_object_modifier(lcstowcsflag);
}
/*
 ======================================================================
 */
