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
	void drr_dynrnd_camera (object)
	void drr_dynrnd_stereo ()
	void drr_dynrnd_stereoswi ()
	void drr_dynrnd_camera_matrix (object)
	void drr_dynrnd_parallel_matrix (object)
	void drr_dynrnd_perspective_matrix (object)
	void drr_dynrnd_projection_matrix (object)

  ======================================================================
 */
#include "dynrnd.h"
#include "dore/dore_develop/private/cammat.h"
#include "dore/dore_develop/private/parmat.h"
#include "dore/dore_develop/private/permat.h"
#include "dore/dore_develop/private/prjmat.h"
/*
 ======================================================================
 */
void drr_dynrnd_camera (
    dot_object *object)
{
    DtObject camera;

    camera = DvInqActiveCamera (dre_dynrnd_data.current_view);

    if (camera == DcNullObject || camera == (DtObject) object) {
	(*dre_dynrnd_DROM_fcns->camera) ();
    }
}
/*
 ======================================================================
 */
void drr_dynrnd_stereo (void)
{
    DtReal eyeseparation;
    DtReal distance;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttStereo"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&eyeseparation, &distance);
    (*dre_dynrnd_DROM_fcns->set_att_stereo) (eyeseparation, distance);
}
/*
 ======================================================================
 */
void drr_dynrnd_stereoswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttStereoSwitch"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_stereoswi) (switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_camera_matrix (
    dot_object *object)
{
    struct cammat_data *cammatdat;
    static DtInt class_id = -1;

    if (class_id == -1) {
	class_id = DsInqClassId("DoCameraMatrix");
    }

    if (!DDcondex_QueryExecute(class_id))
	    return;

    cammatdat = (struct cammat_data *)object->data;

    (*dre_dynrnd_DROM_fcns->set_camera_matrix) (cammatdat->matrix);
}
/*
 ======================================================================
 */
void drr_dynrnd_parallel_matrix (
    dot_object *object)
{
    struct parallel_data *paralleldat;
    static DtInt class_id = -1;

    if (class_id == -1) {
	class_id = DsInqClassId("DoParallel");
    }

    if (!DDcondex_QueryExecute(class_id))
	    return;

    paralleldat = (struct parallel_data *)object->data;


    (*dre_dynrnd_DROM_fcns->set_parallel_matrix) (paralleldat->fov_degree,
						  paralleldat->hither,
						  paralleldat->yon);
}
/*
 ======================================================================
 */
void drr_dynrnd_perspective_matrix (
    dot_object *object)
{
    struct perspective_data *perspectivedat;
    static DtInt class_id = -1;

    if (class_id == -1) {
	class_id = DsInqClassId("DoPerspective");
    }

    if (!DDcondex_QueryExecute(class_id))
	    return;

    perspectivedat = (struct perspective_data *)object->data;

    (*dre_dynrnd_DROM_fcns->set_perspective_matrix)(perspectivedat->fov_degree,
						    perspectivedat->hither,
						    perspectivedat->yon);
}
/*
 ======================================================================
 */
void drr_dynrnd_projection_matrix (
    dot_object *object)
{
    struct projection_data *projectiondata;
    static DtInt class_id = -1;

    if (class_id == -1) {
	class_id = DsInqClassId("DoProjection");
    }

    if (!DDcondex_QueryExecute(class_id))
	    return;

    projectiondata = (struct projection_data *)object->data;

    (*dre_dynrnd_DROM_fcns->set_projection_matrix) (
				&projectiondata->window,
				projectiondata->ptype,
				projectiondata->prp,
				projectiondata->view_plane,
				projectiondata->hither,
				projectiondata->yon);
}
/*
 ======================================================================
 */
