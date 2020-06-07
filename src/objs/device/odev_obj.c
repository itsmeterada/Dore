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
	dor_device_initialize
	dor_device_create
	dor_device_delete
	dor_device_print
	dor_device_pick
	dor_device_pick_view
	dor_device_add_reference
	dor_device_remove_reference
	dor_device_terminate
	dor_device_return_current_device_object

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/device.h>

/* ====================================================================== */

static DtMethodEntry device_methods[] = {
    { DcMethodAddReference,	(DtMethodPtr) dor_device_add_reference },
    { DcMethodDestroy,		(DtMethodPtr) dor_device_delete },
    { DcMethodPrint,		(DtMethodPtr) dor_device_print },
    { DcMethodRemoveReference,	(DtMethodPtr) dor_device_remove_reference }
};

DtPoint3		doe_device_pick_point;
dot_object *		doe_device_list;
dot_object *		doe_device_current_device_object;
dot_device *		doe_device_current_device;
dot_device_driver *	doe_device_current_driver;
DDt_DCM *		doe_device_current_functions;
dot_device_driver *	doe_device_drivers;
DtInt			doe_device_max_drivers;
DtInt			doe_device_driver_count;
DtInt			doe_device_class_id;

/* ====================================================================== */

void dor_device_initialize (void)
{
    doe_device_list = DcNullPtr;
    doe_device_max_drivers = 5;
    doe_device_driver_count = 0;

    doe_device_drivers = (dot_device_driver *)dor_space_allocate
	    (5*sizeof(dot_device_driver));
    if (doe_device_drivers == (dot_device_driver *)0) {
	DDerror(ERR_CANT_ALLOC_MEM, "dor_device_initialize", DcNullPtr);
	doe_device_max_drivers = 0;
    }

    doe_device_class_id = dor_class_install (DcTypeDevice, "DoDevice", 4,
					     device_methods, DcNullPtr);

    doe_device_current_device = (dot_device *)0;
    doe_device_current_driver = (dot_device_driver *)NULL;

    dor_doresys_initialize_drivers();
}

/* ====================================================================== */

dot_object *dor_device_create (
    char *type,
    char *argstring)
{
    static char funcname[] = "DoDevice";
    dot_device        *device;
    dot_device_driver *driver;
    dot_object        *device_object;
    DDt_DCM           *functions;
    int                i;

    device = (dot_device *) dor_space_allocate (sizeof(dot_device));

    if (device == dod_device_null){
	DDerror(ERR_CANT_ALLOC_DEVICE, funcname,DcNullPtr);
	return(DcNullObject);
    }

    /* find the appropriate device driver */

    for (i = 0,driver = NULL; i < doe_device_driver_count; i++) {
	if (!strcmp(type,doe_device_drivers[i].device_name)) {
	    driver = &(doe_device_drivers[i]);
	    break;
	}
    }

    if (driver == NULL) {
	sprintf(DDerror_GetLine(), 
		"unknown device type %s",type);
	DDerror (ERR_VALUE_OUT_OF_RANGE, 
		    funcname, DDerror_GetLine());
	return (DcNullObject);
    }

    device->driver = driver;
    device->modification_sequence_number = -1;
    device->frame = NULL;
    device->references = NULL;

    /* create device object before initializing device driver */

    device_object = dor_object_create(doe_device_class_id,device);
    if (device_object != DcNullPtr){
	device->next = doe_device_list;
	doe_device_list = device_object;
    }

    doe_device_current_driver = driver;
    doe_device_current_device = device;
    functions = driver->functions;

    device->device_data = (DtPtr) (*functions->create_local_data)(device_object);

    if (!(*functions->initialize_device)(device_object, 
					 device->device_data, 
					 argstring, type)) {
	sprintf (DDerror_GetLine(), 
		 "'%s' driver initialization failed", type);
	DDerror(ERR_CANT_ALLOC_DEVICE, funcname,
		     DDerror_GetLine());
	dor_object_check_deletion(device_object);
	return(DcNullObject);
    }

    (*functions->become_current_driver)(device->device_data);

    /* allocate software color map(s) */

    (*functions->inquire_ncolors)(&(device->ncolors));

    if ((device->color_map = (DtReal *)
	 dor_space_allocate(3*device->ncolors*sizeof(DtReal))) == NULL) {
	DDerror (ERR_CANT_ALLOC_MEM,
		    funcname, "colormap");
	dor_object_check_deletion(device_object);
	return(DcNullObject);
    }

    (*functions->inquire_color_entries)(DcRGB, 0,device->ncolors,
					device->color_map);
    /* initialize default shade range to be the entire color map */

    device->shade_ranges = (DtInt *)dor_space_allocate(2*(sizeof (DtInt)));
    device->shade_ranges[0] = 0;
    device->shade_ranges[1] = device->ncolors - 1;
    device->shade_range_table_size = 1;

    (*functions->inquire_device_extent)(&device->extent);
    (*functions->inquire_device_extent)(&device->viewport);

    device->xmin = device->viewport.bll[0];
    device->ymin = device->viewport.bll[1];
    device->xmax = device->viewport.fur[0];
    device->ymax = device->viewport.fur[1];


    (*functions->inquire_resolution)(&(device->resolution[0]),
				     &(device->resolution[1]));
    (*functions->inquire_visual_type)(&(device->visual_type));
    (*functions->inquire_auto_size)(&(device->auto_size));

    device->shade_index = 0;

    device->foreground_color[0] = 1.0;
    device->foreground_color[1] = 1.0;
    device->foreground_color[2] = 1.0;

    device->background_color[0] = 0.0;
    device->background_color[1] = 0.0;
    device->background_color[2] = 0.0;

    device->depth_buffer_enable = DcFalse ;
    device->depth_buffer_write = DcFalse ;

    (*functions->set_shade_index)(device->shade_index);
    (*functions->set_foreground_color)(DcRGB, device->foreground_color);
    (*functions->set_background_color)(DcRGB, device->background_color);
    (*functions->set_depth_buffer_enable)(device->depth_buffer_enable);
    (*functions->set_depth_buffer_write)(device->depth_buffer_write);

    /*  Initialize pick stuff.  */

    device->pick_aperture.width = 10.;
    device->pick_aperture.height = 8.;
    device->pick_aperture.depth = 
	    (device->extent.fur[2] - device->extent.bll[2])*4.;
    device->pick_callback_object = (dot_object *)DcPickFirst;
    device->pick_path_order = DcTopFirst;

    /* initialize default shade mode */

    device->shade_mode = DcComponent;
    device->modification_sequence_number = 0;
    device->aborted_draw_flag = DcFalse;

    return(device_object);
}

/* ====================================================================== */

void dor_device_delete (dot_object *device_object)
{
    dot_device *device = (dot_device *)device_object->data;
    dot_object *previous_device;
    dot_object *current_device;

    if (device->modification_sequence_number != -1) {
	/* device was sucessfully opened so close the 
	   attached physical device */

	SET_CURRENT_DEVICE(device_object);
    }

    if (DcNullObject != device->frame){
	dor_object_delete_reference(device->frame,device_object);
    }

    if (device->modification_sequence_number != -1) {
	/* call the device driver to close the driver */

	OUTFUN(close_device)() ;
    }

    /* detach from list of defined devices */

    previous_device = DcNullPtr;
    current_device = doe_device_list;

    while (current_device != DcNullPtr){
	if (current_device == device_object){
	    if (previous_device != DcNullPtr){
		((dot_device *)(previous_device->data))->next =
			((dot_device *)(current_device->data))->next;
	    } else {
		doe_device_list = ((dot_device *)(current_device->data))->next;
	    }
	    break;
	}
	previous_device = current_device;
	current_device = ((dot_device *)(current_device->data))->next;
    }

    dor_space_deallocate(device);
}

/* ====================================================================== */

void dor_device_print (dot_object *device_object)
{
    dot_device *device;

    device = (dot_device *)device_object->data;

    if (device == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),
	    "extent (bll,fur) = ((%f,%f,%f),(%f,%f,%f))",
	    device->extent.bll[0],device->extent.bll[1],device->extent.bll[2],
	    device->extent.fur[0],device->extent.fur[1],device->extent.fur[2]);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "viewport (bll,fur) = ((%f,%f,%f),(%f,%f,%f))",
	    device->viewport.bll[0],device->viewport.bll[1],device->viewport.bll[2],
	    device->viewport.fur[0],device->viewport.fur[1],device->viewport.fur[2]);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "pick aperture = (%f,%f,%f)",
	    device->pick_aperture.width,
	    device->pick_aperture.height,
	    device->pick_aperture.depth);
    dor_print_output(dor_print_get_line());

    if (device->pick_path_order == DcTopFirst) {
	dor_print_output("pick path order = DcTopFirst");
    } else {
	dor_print_output("pick path order = DcBottomFirst");
    }

    switch (device->visual_type) {
    case DcStaticGrey:
	dor_print_output("visual type = DcStaticGrey");
	break;
    case DcGreyScale:
	dor_print_output("visual type = DcGreyScale");
	break;
    case DcStaticColor:
	dor_print_output("visual type = DcStaticColor");
	break;
    case DcPseudoColor:
	dor_print_output("visual type = DcPseudoColor");
	break;
    case DcTrueColor:
	dor_print_output("visual type = DcTrueColor");
	break;
    case DcDirectColor:
	dor_print_output("visual type = DcDirectColor");
	break;
    }
}

/* ====================================================================== */

void dor_device_pick (
    dot_object *device_object,
    DtPoint3    pick_point,
    DtInt      *hit_count,	/* RETURN */
    DtInt       index_size,
    DtInt       index[],	/* RETURN */
    DtInt       list_size,
    long        hit_list[],     /* RETURN */
    DtReal      z_values[],     /* RETURN */
    DtReal      wcs_points[],	/* RETURN */
    DtReal      lcs_points[],	/* RETURN */
    dot_object *views[], 	/* RETURN */
    DtInt      *error_word)     /* RETURN */
{
    dot_device *device;
    dot_object *frame_object;

#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object,doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		   "DdPick",DcNullPtr);
	return;
    }

    if ((index_size<=0) || (list_size<=0)){
	DDerror(ERR_BAD_INDEX_OR_LIST,
		   "DdPick",DcNullPtr);
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
	DDerror (ERR_MULTIPLE_TRAVERSAL, "DdPick", DcNullPtr);
	return;
    }

    device = (dot_device *)(device_object->data);
    frame_object = device->frame;

    if (frame_object == DcNullPtr){
	*hit_count = 0;
	return;
    }

    doe_system_active_traversals++;

    doe_device_pick_point[0] = pick_point[0];
    doe_device_pick_point[1] = pick_point[1];
    doe_device_pick_point[2] = pick_point[2];

    dor_pick_setup(device_object,hit_count,index_size,index,list_size,
		   hit_list,z_values,wcs_points,lcs_points,views,error_word);

    dor_frame_pick(frame_object,device_object);

    doe_system_active_traversals--;
}

/* ====================================================================== */

void dor_device_pick_view (
    dot_object *device_object,
    dot_object *frame_object,
    dot_object *view_object)
{
    dot_device *device = (dot_device *)(device_object->data);

    SET_CURRENT_DEVICE (device_object);

    if (device->auto_size){
	dor_device_viewport_size(device_object);
    }

    dor_device_compute_frame_trans(device);

    dor_device_compute_pick_volume(device,view_object);

    if (device->all_clipped){
	return;
    }

    dor_device_compute_actual_viewport (device);

    dor_pick_view(device_object,view_object);
}

/* ====================================================================== */

void dor_device_add_reference (
    dot_object *device_object,
    dot_object *reference_object)
{
    dot_device *device = (dot_device *)device_object->data;

    dor_object_reference_insert(&(device->references), reference_object);
}

/* ====================================================================== */

void dor_device_remove_reference (
    dot_object *device_object,
    dot_object *reference_object)
{
    dot_device *device = (dot_device *)device_object->data;

    dor_object_reference_remove(&(device->references),reference_object);
}

/* ====================================================================== */

void dor_device_terminate (void)
{
}

/* ====================================================================== */

dot_object *dor_device_return_current_device_object (void)
{
    return doe_device_current_device_object;
}
