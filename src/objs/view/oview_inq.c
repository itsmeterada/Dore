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
	dot_object * dor_view_inq_active_camera (view_object)
	int dor_view_inq_background_color (view_object,color_type,color)
	int dor_view_inq_boundary (view_object,boundary)
	DtVolume * dor_view_inq_boundary_ptr (view_object)
	void dor_view_inq_background_just (view_object, left, bottom)
	void dor_view_inq_background_raster (view_object, raster, callback)
	DtFlag dor_view_inq_clear_flag (view_object)
	dot_object * dor_view_inq_definition_group (view_object)
	dot_object * dor_view_inq_display_group (view_object)
	DtRenderStyle dor_view_inq_render_style (view_object)
	int dor_view_inq_render_data (view_object,renderstyle,renderdata)
	int dor_view_inq_sequence_number (view_object,sequencenumber)
	DtInt dor_view_inq_shade_index (view_object)
	DtUpdateType dor_view_inq_update_type (view_object)
	void dor_view_inq_rad_convergence(view_object, type, parms)
	DtRadiosityUpdateType dor_view_inq_rad_updatetype(view_object)
	void dor_view_inq_formfac_comptype(view_object, type, parms)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/view.h>

/*
 ======================================================================
 */

dot_object *dor_view_inq_active_camera (dot_object *view_object)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW,
		     "DvInqActiveCamera",DcNullPtr);
	return(DcNullObject);
    }
#endif

    view = (dot_view *)view_object->data;

    return(view->active_camera);
}
/*
 ======================================================================
 */

void dor_view_inq_background_color (
    dot_object   *view_object,
    DtColorModel *color_type,
    DtReal        color[])
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW, "DvInqBackgroundColor",DcNullPtr);
	return;
    }
#endif

    view = (dot_view *)view_object->data;

    color[0] = view->background_color[0];
    color[1] = view->background_color[1];
    color[2] = view->background_color[2];
}
/*
 ======================================================================
 */

void dor_view_inq_boundary (dot_object *view_object, DtVolume *boundary)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW,
		     "DvInqBoundary",DcNullPtr);
	return;
    }
#endif

    view = (dot_view *)view_object->data;

    *boundary = view->boundary;
}
/*
 ======================================================================
 */

DtVolume *dor_view_inq_boundary_ptr (dot_object *view_object)
{
    dot_view *view = (dot_view *)view_object->data;

    return(&(view->boundary));
}
/*
 ======================================================================
 */
 
void dor_view_inq_background_just (
    dot_object *view_object,
    DtReal     *left,
    DtReal     *bottom)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id))	{
	DDerror(ERR_INVALID_VIEW, 
			"DvInqBackgroundJust",DcNullPtr);
	return;
    }
#endif

    view = (dot_view *)view_object->data;

    *left = view->background_left_just;
    *bottom = view->background_bottom_just;
}
/*
 ======================================================================
 */

void dor_view_inq_background_raster (
    dot_object  *view_object,
    dot_object **raster,
    dot_object **callback)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id))	{
	DDerror(ERR_INVALID_VIEW, 
		"DvInqBackgroundRaster",DcNullPtr);
	return;
    }
#endif

    view = (dot_view *)view_object->data;

    *raster = view->background_raster;
    *callback = view->background_callback;
}
/*
 ======================================================================
 */

DtFlag dor_view_inq_clear_flag (dot_object *view_object)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW,
		     "DvInqClearFlag",DcNullPtr);
	return(-1);
    }
#endif

    view = (dot_view *)view_object->data;

    return(view->clear_flag);
}
/*
 ======================================================================
 */

dot_object *dor_view_inq_definition_group (dot_object *view_object)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW,
		     "DvInqDefinitionGroup",DcNullPtr);
	return(DcNullObject);
    }
#endif

    view = (dot_view *)view_object->data;

    return(view->definition_group);
}
/*
 ======================================================================
 */

dot_object *dor_view_inq_display_group (dot_object *view_object)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW,
		     "DvInqDisplayGroup",DcNullPtr);
	return(DcNullObject);
    }
#endif

    view = (dot_view *)view_object->data;

    return(view->display_group);
}
/*
 ======================================================================
 */

DtRenderStyle dor_view_inq_render_style (dot_object *view_object)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW,
		     "DvInqRendStyle",DcNullPtr);
	return(-1);
    }
#endif

    view = (dot_view *)view_object->data;

    return(view->render_style);
}
/*
 ======================================================================
 */

void dor_view_inq_render_data (
    dot_object *view_object,
    DtInt       renderstyle,
    DtPtr      *renderdata)
{
    dot_view *view;

    view = (dot_view *)(view_object->data);

    *renderdata = view->render_data[renderstyle];
}
/*
 ======================================================================
 */

void dor_view_inq_sequence_number (
    dot_object *view_object,
    DtInt      *sequencenumber)
{
    dot_view *view;

    view = (dot_view *)(view_object->data);

    *sequencenumber = view->modification_sequence_number;
}
/*
 ======================================================================
 */

DtInt dor_view_inq_shade_index (dot_object *view_object)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)) { 
	DDerror(ERR_INVALID_VIEW, 
		"DvInqShadeIndex",DcNullPtr);
	return(0);
    }
#endif

    view = (dot_view *)view_object->data;

    return(view->shade_index);
}
/*
 ======================================================================
 */

DtUpdateType dor_view_inq_update_type (dot_object *view_object)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW,
		     "DvInqUpdateType",DcNullPtr);
	return(-1);
    }
#endif

    view = (dot_view *)view_object->data;

    return(view->update_type);
}

/*
 ======================================================================
 */

void dor_view_inq_rad_convergence (
    dot_object          *view_object,
    DtRadiosityConvType *type,
    DtReal              *parms)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW,
		     "DvInqRadiosityConvergence",DcNullPtr);
	return;
    }
#endif

    view = (dot_view *)view_object->data;

    *type = view->rad_convergence_type; 
    parms[0] = view->rad_convergence_parms[0];
}

/*
 ======================================================================
 */

DtRadiosityUpdateType dor_view_inq_rad_updatetype (dot_object *view_object)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW,
		     "DvInqRadiosityUpdateType",DcNullPtr);
	return(-1);
    }
#endif

    view = (dot_view *)view_object->data;

    return(view->rad_update_type);
}

/*
 ======================================================================
 */


void dor_view_inq_formfac_comptype (
    dot_object           *view_object,
    DtFormFactorCompType *type,
    DtReal               *parms)
{
    dot_view *view;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW,
		     "DvInqFormFactorCompType",DcNullPtr);
	return;
    }
#endif

    view = (dot_view *)view_object->data;

    *type = view->formfactor_comptype;
    parms[0] = view->formfactor_comptype_parms[0];
}
