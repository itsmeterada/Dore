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
	int dor_view_update (view_object)
	int dor_view_update_notify (view_object)
	DtObject dor_view_resize_raster (view_object, width, height)
	void dor_view_traverse_studio (view_object, method_id)
	void dor_view_traverse_display (view_object, method_id)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/view.h>

/*
 ======================================================================
 */

/* Update the view in all of the frames to which it is attached. */

void dor_view_update (dot_object *view_object)
{
    dot_view *view;
    dot_object_reference *viewrefs;
    dot_object_reference *grprefs;
    static DtInt group_class_id = -1;
    static DtInt frame_class_id = -1;

    if (group_class_id == -1) {
	group_class_id = dor_class_inq_id ("DoGroup");
    }

    if (frame_class_id == -1) {
	frame_class_id = dor_class_inq_id ("DoFrame");
    }

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id)){
	DDerror(ERR_INVALID_VIEW,
		   "DvUpdate",DcNullPtr);
	return;
    }
#endif

    /*
     * The variable doe_system_active_traversals is used to indicate
     * that a function is active that requires traversals of the
     * data base.  These functions include:
     *   DdUpdate	
     *   DdPick	
     *   DfUpdate		
     *   DgCheck		
     *   DsCompBoundingVol
     *   DvUpdate		
     * Multiple active traversals are not possible because
     * the state of a traversal is maintained in the global 
     * attributes.  This state would need to be saved and 
     * restored if traversals were allowed to be re-entrant.
     * Further each of the above sub-systems also maintains state
     * during traversal and that state would likewise need to be 
     * saved and restored.
     */

    if (doe_system_active_traversals) {
	DDerror (ERR_MULTIPLE_TRAVERSAL,
		    "DvUpdate", DcNullPtr);
	return;
    }

    doe_system_active_traversals++;

    view = (dot_view *)view_object->data;

    for (viewrefs=view->references;viewrefs!=NULL;viewrefs=viewrefs->next){
	if (dor_object_inq_type(viewrefs->object) == group_class_id){
	    grprefs = dor_group_inq_references(viewrefs->object);

	    /*  Get the last reference (should be first reference, 
		which means changing the object manager).  */

	    if (grprefs == NULL){
		continue;
	    }

	    while((grprefs->next) != NULL){
		grprefs = grprefs->next;
	    }

	    if (dor_object_inq_type(grprefs->object) == frame_class_id){
		dor_frame_update_view(grprefs->object,view_object);
	    }
	}
    }
    doe_system_active_traversals--;
}
/*
 ======================================================================
 */

void dor_view_update_notify (dot_object *view_object)
{
    dot_view *view;
    dot_object_reference *viewrefs;
    dot_object_reference *grprefs;
    DtInt group_class_id = dor_class_inq_id ("DoGroup");
    DtInt frame_class_id = dor_class_inq_id ("DoFrame");

    view = (dot_view *)view_object->data;

    /*  Tell the frames to which this view is attached that their
	environment has been modified.  */

    for (viewrefs=view->references;viewrefs;viewrefs=viewrefs->next) {
	if (dor_object_inq_type(viewrefs->object) == group_class_id) {
	    grprefs = dor_group_inq_references(viewrefs->object);

	    /* Get the last reference (should be first ref which
	       means changing the object manager) */

	    if (!grprefs) {
		continue;
	    }

	    while (grprefs->next) {
		grprefs = grprefs->next;
	    }

	    if (dor_object_inq_type(grprefs->object) == frame_class_id) {
		dor_frame_update_notify(grprefs->object);
	    }
	}
    }

    view->modification_sequence_number++;
}
/*
 ======================================================================
 */
DtObject dor_view_resize_raster (
    dot_object *view_object,
    DtInt       width,
    DtInt       height)
{
    dot_view *view;
    DtObject new_raster_object;
    DtInt dataptr_class_id = dor_class_inq_id ("DoDataPtr");
    DtInt dataval_class_id = dor_class_inq_id ("DoDataVal");
    DtPtr dataptr;
    DtInt type;
    DtPFI function;
    DtFlag pass_by_ref;
    DtObject data_object;
    DtInt i;
    DtInt index;

#ifdef ERRCHK
    if (!dor_object_confirm_type(view_object,doe_view_class_id))	{
	DDerror(ERR_INVALID_VIEW, 
			"DDview_ResizeRaster",DcNullPtr);
	return DcNullObject;
    }
#endif

    view = (dot_view *)view_object->data;

    if (view->background_raster == (dot_object *)0) {
	return DcNullObject;
    }
    if (view->background_callback == (dot_object *)0) {
	return view->background_raster;
    }

    for (i=0; i < view->background_raster_count; i++) {
	if (view->background_resized_raster[i].width == width &&
	    view->background_resized_raster[i].height == height) 
		return (view->background_resized_raster[i].raster);
    }

    /*
     * don't already have a raster of requested size so create one
     */
    function = (DtPFI)dor_calbak_get_function(view->background_callback);
    data_object = (DtObject)dor_calbak_get_dataobject(view->background_callback);
    pass_by_ref = dor_calbak_get_pass_by_ref(view->background_callback);

    dataptr = (DtPtr)0;
    if ((data_object != DcNullObject) &&
	(DcObjectValid == dor_object_validate(data_object))) {
	if ((type = dor_object_inq_type(data_object)) == dataptr_class_id) {
	    dataptr = dor_datptr_get_value(data_object);
	} else if (type == dataval_class_id) {
	    dataptr = (DtPtr) dor_datval_get_value(data_object);
	}
    }

    if (pass_by_ref) {
	new_raster_object = (DtObject)(*function)(&dataptr, 
					 &(view->background_raster), 
					 &width, &height);
    } else {
	new_raster_object = (DtObject)(*function)(dataptr, 
					 view->background_raster, 
					 width, height);
    }

    if (new_raster_object) {
	if (view->background_raster_count == 0) {
	    view->background_resized_raster = (dot_resized_raster *)
		    dor_space_allocate(sizeof(dot_resized_raster));
	} else {
	    view->background_resized_raster = (dot_resized_raster *)
		    dor_space_reallocate(view->background_resized_raster, 
					 (view->background_raster_count + 1)*
					 sizeof(dot_resized_raster));
	}
	index = view->background_raster_count++;
	view->background_resized_raster[index].raster = new_raster_object;
	view->background_resized_raster[index].width = width;
	view->background_resized_raster[index].height = height;
	dor_object_add_reference (new_raster_object, view_object);
	return new_raster_object;

    } else {
	return view->background_raster;
    }
}
/*
 ======================================================================
 */
DtFlag dor_view_traverse_studio (dot_object *view_object, DtInt method_id)
{
    dot_object  *definitiongroup;
    DtMethodPtr  executemethod;

    /*  Ensure that the method_id exists.  */

    if (!dor_object_method_id_valid (method_id))
	return DcFalse;

    /*  Indicate to all of the attribute objects what the current method
    **  being executed is.  */

    doe_system_current_method = method_id;

    definitiongroup = dor_view_inq_definition_group(view_object);

    executemethod = dor_object_get_current_method(definitiongroup);

    (*executemethod)(definitiongroup);

    doe_system_current_method = DcMethodNull;

    return DcTrue;
}
/*
 ======================================================================
 */
DtFlag dor_view_traverse_display (dot_object *view_object, DtInt method_id)
{
    dot_object  *displaygroup;
    DtMethodPtr  executemethod;

    /*  Ensure that the method_id exists.  */

    if (!dor_object_method_id_valid (method_id))
	return DcFalse;

    /*  Indicate to all of the attribute objects what the current method
    **  being executed is.  */

    doe_system_current_method = method_id;

    displaygroup = dor_view_inq_display_group(view_object);

    executemethod = dor_object_get_current_method(displaygroup);

    (*executemethod)(displaygroup);

    doe_system_current_method = DcMethodNull;

    return DcTrue;
}
