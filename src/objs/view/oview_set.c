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
	int dor_view_set_active_camera (view_object,camera_object)
	int dor_view_set_background_color (view_object,color_type,color)
	int dor_view_set_boundary (view_object,boundary)
	void dor_view_set_background_just (view_object, left, bottom)
	void dor_view_set_background_raster (view_object, raster, callback)
	int dor_view_set_clear_flag (view_object,clear_flag)
	int dor_view_set_render_style (view_object,render_style)
	int dor_view_set_render_data (view_object,renderstyle,dataptr)
	int dor_view_set_shade_index (view_object, shade_index)
	int dor_view_set_update_type (view_object,update_type)
	int dor_view_set_rad_convergence(view_object, conv_type, parms)
	int dor_view_set_rad_updatetype(view_object, updatetype)
	int dor_view_set_formfac_comptype(view_object,formfactor_comptype,parms)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/view.h>

/*
 ======================================================================
 */

void dor_view_set_active_camera (
    dot_object *view_object,
    dot_object *camera_object)
{
    dot_view *view;
    static DtInt camera_class_id = -1;

    if (camera_class_id == -1) {
	camera_class_id = dor_class_inq_id ("DoCamera");
    }

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW,
		     "DvSetActiveCamera",DcNullPtr);
	return;
    }

    if (!dor_object_confirm_type(camera_object,camera_class_id)){
	DDerror(ERR_INVALID_CAMERA,
		     "DvSetActiveCamera",DcNullPtr);
	return;
    }
#endif

    view = (dot_view *)view_object->data;

    if (view->active_camera){
	dor_object_delete_reference(view->active_camera,view_object);
    }

    view->active_camera = camera_object;

    if (camera_object){
	dor_object_add_reference(camera_object,view_object);
    }

    dor_view_update_notify(view_object);
}
/*
 ======================================================================
 */

void dor_view_set_background_color (
    dot_object   *view_object,
    DtColorModel  color_type,
    DtReal        color[])
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW,
		     "DvSetBackgroundColor",DcNullPtr);
	return;
    }
#endif

    view = (dot_view *)view_object->data;

    view->color_type = color_type;

    view->background_color[0] = color[0];
    view->background_color[1] = color[1];
    view->background_color[2] = color[2];

    dor_view_update_notify(view_object);
}
/*
 ======================================================================
 */

void dor_view_set_boundary (
    dot_object *view_object,
    DtVolume   *boundary)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW,
		     "DvSetBoundary",DcNullPtr);
	return;
    }
#endif

    view = (dot_view *)view_object->data;

    view->boundary = *boundary;

    dor_view_update_notify(view_object);
}
/*
 ======================================================================
 */
 
void dor_view_set_background_just (
    dot_object *view_object,
    DtReal      left,
    DtReal      bottom)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id))	{
	DDerror(ERR_INVALID_VIEW, "DvSetBackgroundJust",DcNullPtr);
	return;
    }
#endif

    view = (dot_view *)view_object->data;

    view->background_left_just = left;;
    view->background_bottom_just = bottom;

    dor_view_update_notify(view_object);
}
/*
 ======================================================================
 */
 
void dor_view_set_background_raster (
    dot_object *view_object,
    dot_object *raster,
    dot_object *callback)
{
    dot_view *view;
    DtInt i;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id))	{
	DDerror(ERR_INVALID_VIEW, 
			"DvSetBackgroundRaster",DcNullPtr);
	return;
    }

    if (raster != (dot_object *)0 &&
	!dor_object_confirm_type(raster, dor_class_inq_id("DoRaster")) &&
	!dor_object_confirm_type(raster, dor_class_inq_id("DoFileRaster"))) {
	DDerror(ERR_INVALID_RASTER, 
			"DvSetBackgroundRaster",DcNullPtr);
	return;
    }
      
    if (callback != (dot_object *)0 &&
	!dor_object_confirm_type(callback, dor_class_inq_id("DoCallback"))) {
	DDerror(ERR_INVALID_CALLBACK, 
			"DvSetBackgroundCallback",DcNullPtr);
	return;
    }
#endif

    view = (dot_view *)view_object->data;

    if (raster != (dot_object *)0)
	    dor_object_add_reference(raster, view_object);
    if (callback != (dot_object *)0)
	    dor_object_add_reference(callback, view_object);

    if (view->background_raster != (dot_object *)0)
	    dor_object_delete_reference(view->background_raster, view_object);
    if (view->background_callback != (dot_object *)0)
	    dor_object_delete_reference(view->background_callback, view_object);

    view->background_raster = raster;
    view->background_callback = callback;

    if (view->background_raster_count > 0) {
	for (i=0 ;i<view->background_raster_count; i++) {
	    dor_object_delete_reference(view->background_resized_raster[i].raster,
					view_object);
	}
	dor_space_deallocate (view->background_resized_raster);
	view->background_resized_raster = (dot_resized_raster *)0;
    }
    view->background_raster_count = 0;

    dor_view_update_notify(view_object);
}
/*
 ======================================================================
 */

void dor_view_set_clear_flag (
    dot_object *view_object,
    DtFlag      clear_flag)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW,
		     "DvSetClearFlag",DcNullPtr);
	return;
    }
#endif

    view = (dot_view *)view_object->data;

    view->clear_flag = clear_flag;

    dor_view_update_notify(view_object);
}
/*
 ======================================================================
 */

void dor_view_set_render_style (
    dot_object    *view_object,
    DtRenderStyle  render_style)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW,
		     "DvSetRendStyle",DcNullPtr);
	return;
    }
#endif

    view = (dot_view *)view_object->data;

    view->render_style = render_style;

    dor_view_update_notify(view_object);
}
/*
 ======================================================================
 */

void dor_view_set_render_data (
    dot_object *view_object,
    DtInt       renderstyle,
    DtPtr       dataptr)
{
    dot_view *view;

    view = (dot_view *)(view_object->data);

    view->render_data[renderstyle] = dataptr;
}
/*
 ======================================================================
 */

void dor_view_set_shade_index (
    dot_object *view_object,
    DtInt       shade_index)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)) {
	DDerror(ERR_INVALID_VIEW, "DvSetShadeIndex",DcNullPtr);
	return;
    }
#endif

    view = (dot_view *)view_object->data;

    view->shade_index = shade_index;

    dor_view_update_notify(view_object);
}
/*
 ======================================================================
 */

void dor_view_set_update_type (
    dot_object  *view_object,
    DtUpdateType update_type)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW,
		     "DvSetUpdateType",DcNullPtr);
	return;
    }
#endif

    view = (dot_view *)view_object->data;

    view->update_type = update_type;

    /*
      dor_view_update_notify(view_object);
      */
}

/*
 ======================================================================
 */

void dor_view_set_rad_convergence (
    dot_object          *view_object,
    DtRadiosityConvType  conv_type,
    DtReal              *parms)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
        DDerror(ERR_INVALID_VIEW,
                     "DvSetRadiosityConvergence",DcNullPtr);
        return;
    }
#endif

    view = (dot_view *)view_object->data;

    view->rad_convergence_type = conv_type;

    /* currently all types require only one parameter */
    view->rad_convergence_parms[0] = parms[0];

    dor_view_update_notify(view_object);
}

/*
 ======================================================================
 */

void dor_view_set_rad_updatetype (
    dot_object           *view_object,
    DtRadiosityUpdateType updatetype)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
        DDerror(ERR_INVALID_VIEW,
                     "DvSetRadiosityUpdateType",DcNullPtr);
        return;
    }
#endif

    view = (dot_view *)view_object->data;

    view->rad_update_type = updatetype;

    dor_view_update_notify(view_object);
}

/*
 ======================================================================
 */

void dor_view_set_formfac_comptype (
    dot_object          *view_object,
    DtFormFactorCompType formfactor_comptype,
    DtReal              *parms)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
        DDerror(ERR_INVALID_VIEW,
                     "DvSetFormFactorCompType",DcNullPtr);
        return;
    }
#endif

    view = (dot_view *)view_object->data;

    view->formfactor_comptype = formfactor_comptype;

    view->formfactor_comptype_parms[0] = parms[0];

    dor_view_update_notify(view_object);
}
