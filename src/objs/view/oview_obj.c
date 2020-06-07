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
	int dor_view_initialize ()
	dot_object * dor_view_create ()
	int dor_view_delete (view_object)
	int dor_view_add_reference (view_object,reference_object)
	int dor_view_remove_reference (view_object,reference_object)
	int dor_view_print (object)

  ======================================================================
 */

#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/view.h>

dot_object *doe_view_list;
DtInt doe_view_class_id;

/*
 ======================================================================
 */

static DtMethodEntry view_methods[] = {
    { DcMethodAddReference,    (DtMethodPtr) dor_view_add_reference },
    { DcMethodDestroy,         (DtMethodPtr) dor_view_delete },
    { DcMethodPrint,           (DtMethodPtr) dor_view_print },
    { DcMethodRemoveReference, (DtMethodPtr) dor_view_remove_reference },
};

void dor_view_initialize (void)
{
    doe_view_list = DcNullPtr;

    doe_view_class_id = dor_class_install (DcTypeView, "DoView", 
				4,view_methods, DcNullPtr);

}
/*
 ======================================================================
 */

dot_object *dor_view_create (void)
{
    dot_view *view;
    dot_object *view_object;

    view = ( dot_view *)dor_space_allocate(sizeof(dot_view));

    view->active_camera = DcNullPtr;

    view->color_type = DcRGB;
    view->background_color[0] = 0.;
    view->background_color[1] = 0.;
    view->background_color[2] = 0.;

    view->shade_index = 0;

    view->clear_flag = DcTrue;

    view->display_group = dor_group_create(DcFalse,DcFalse);
    view->definition_group = dor_group_create(DcFalse,DcFalse);

    view->render_style = DcRealTime;

    view->update_type = DcUpdateAll;

    view->boundary.fur[0] = 1.0;
    view->boundary.fur[1] = 1.0;
    view->boundary.fur[2] = 1.0;
    view->boundary.bll[0] = 0.0;
    view->boundary.bll[1] = 0.0;
    view->boundary.bll[2] = 0.0;

    view->references = (dot_object_reference *)0;

    view->modification_sequence_number = 0;

    view->background_left_just = 0.5;
    view->background_bottom_just = 0.5;
    view->background_raster = (dot_object *)0;
    view->background_callback = (dot_object *)0;
    view->background_raster_count = 0;
    view->background_resized_raster = (dot_resized_raster *)0;

    view->rad_convergence_type = DcConvGlobalRel;
    view->rad_convergence_parms = (DtReal *)dor_space_allocate(sizeof (DtReal));
    view->rad_convergence_parms[0] = 0.1;


    /* default for radiosity ffactor comp. is a 128x128 hemicube */
    view->formfactor_comptype_parms =
           (DtReal *)dor_space_allocate(sizeof (DtReal));
    view->formfactor_comptype_parms[0] = 128;

    view->formfactor_comptype = DcFormFactorHemicube;
   /* turn radiosity off by default */
    view->rad_update_type = DcRadiosityNone;

    view_object = dor_object_create(doe_view_class_id,view);

    if ( view_object ) {
	view->next = doe_view_list;
	doe_view_list = view_object;
    }

    /* This view's display and definition groups are referenced by it */

    dor_object_add_reference(view->definition_group,view_object);
    dor_object_add_reference(view->display_group,view_object);

    return view_object;
}

/*
 ======================================================================
 */

void dor_view_delete (dot_object *view_object)
{
    dot_view *view;
    dot_object *previous_view,*current_view;
    DtInt i;

    view = (dot_view *)view_object->data;

    /* free display and deinition groups */

    dor_group_empty(view->definition_group);
    dor_group_empty(view->display_group);

    /* detach from list of defined views */

    previous_view = DcNullPtr;
    current_view = doe_view_list;

    while (current_view) {
	if (current_view == view_object) {
	    if ( previous_view ) {
		((dot_view *)(previous_view->data))->next =
			((dot_view *)(current_view->data))->next;
	    } else {
		doe_view_list = ((dot_view *)(current_view->data))->next;
	    }
			
	    break;
	}

	previous_view = current_view;
	current_view = ((dot_view *)(current_view->data))->next;
    }

    if (view->active_camera) {
	dor_object_delete_reference(view->active_camera,view_object);
    }
	

    if (view->background_raster != (dot_object *)0)
	    dor_object_delete_reference(view->background_raster, view_object);
    if (view->background_callback != (dot_object *)0)
	    dor_object_delete_reference(view->background_callback, view_object);

    if (view->background_raster_count > 0) {
	for (i=0 ;i<view->background_raster_count; i++) {
	    dor_object_delete_reference(view->background_resized_raster[i].raster,
					view_object);
	}
	dor_space_deallocate (view->background_resized_raster);
	view->background_resized_raster = (dot_resized_raster *)0;
    }
    view->background_raster_count = 0;

    dor_space_deallocate(view);
}
/*
 ======================================================================
 */

void dor_view_add_reference (
    dot_object *view_object,
    dot_object *reference_object)
{
    dot_view *view = (dot_view *)view_object->data;

    dor_object_reference_insert(&(view->references),reference_object);
}
/*
 ======================================================================
 */

void dor_view_remove_reference (
    dot_object *view_object,
    dot_object *reference_object)
{
    dot_view *view = (dot_view *)view_object->data;

    dor_object_reference_remove(&(view->references),reference_object);
}
/*
 ======================================================================
 */

void dor_view_print (dot_object *object)
{
    dot_view *view = (dot_view *)object->data;

    dor_print_output("background color model = DcRGB");

    sprintf(dor_print_get_line(),"background color = (%f,%f,%f)", 
	    view->background_color[0],
	    view->background_color[1],
	    view->background_color[2]);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"shade index = %d",view->shade_index);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"clear flag = %d",view->clear_flag);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"render style = %d",view->render_style);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"update type = %d",view->update_type);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"boundary (bll,fur) = ((%f,%f,%f),(%f,%f,%f))",
	    view->boundary.bll[0],view->boundary.bll[1],view->boundary.bll[2],
	    view->boundary.fur[0],view->boundary.fur[1],view->boundary.fur[2]);
    dor_print_output(dor_print_get_line());

    dor_print_output("view definition group:");
    dor_print_indent_up();

    dor_object_print(view->definition_group);

    dor_print_indent_down();

    dor_print_output("view display group:");
    dor_print_indent_up();

    dor_object_print(view->display_group);

    dor_print_indent_down();
}
