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
	void dor_device_set_frame (device_object,frame_object)
	void dor_device_set_pick_aperture (device_object,pick_aperture)
	void dor_device_set_pick_callback (device_object,pick_callback)
	void dor_device_set_pick_path_order (device_object,pick_path_order)
	void dor_device_set_shade_mode (device_object,mode)
	void dor_device_set_shade_ranges (device_object,start,count,entries)
	void dor_device_set_viewport (device_object,viewport)
	void dor_device_set_color_entries (device_object, colortype, start, count, entries)
	void dor_device_set_options (device_object, argstring)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/device.h>
/*
 ======================================================================
 */
void dor_device_set_frame (
    dot_object *device_object,
    dot_object *frame_object)
{
    dot_device *device;
    static DtInt frame_class_id = -1;

    if (frame_class_id == -1) {
	frame_class_id = dor_class_inq_id("DoFrame");
    }
	
#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object,doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		     "DdSetFrame",DcNullPtr);
	return;
    }

    if (!dor_object_confirm_type(frame_object,frame_class_id)){
	DDerror(ERR_INVALID_FRAME,
		     "DdSetFrame",DcNullPtr);
	return;
    }
#endif

    device = (dot_device *)(device_object->data);

    if (device->frame == frame_object)
	    return;

    if (device->frame != DcNullPtr){
	dor_device_update_notify(device_object);
	dor_object_delete_reference(device->frame,device_object);
    }

    device->frame = frame_object;

    if (frame_object != DcNullPtr){
	dor_device_update_notify(device_object);
	dor_object_add_reference(frame_object,device_object);
    }
}
/*
 ======================================================================
 */
void dor_device_set_pick_aperture (
    dot_object *device_object,
    DtSize3    *pick_aperture)
{
    dot_device *device;

#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object,doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		     "DdSetPickAperture",DcNullPtr);
	return;
    }
#endif

    device = (dot_device *)(device_object->data);

    device->pick_aperture = *pick_aperture;
}
/*
 ======================================================================
 */
void dor_device_set_pick_callback (
    dot_object *device_object,
    dot_object *pick_callback)
{
    dot_device *device;
    static DtInt calbak_class_id = -1;

    if (calbak_class_id == -1) {
	calbak_class_id = dor_class_inq_id ("DoCallback");
    }

#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object,doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		     "DdSetPickCallBack",DcNullPtr);
	return;
    }

    if (!dor_object_confirm_type(pick_callback,calbak_class_id)){
	DDerror(ERR_INVALID_CALLBACK,
		     "DdSetPickCallBack",DcNullPtr);
	return;
    }
#endif

    device = (dot_device *)(device_object->data);

    if (device->pick_callback_object == pick_callback)
	    return;

    if (device->pick_callback_object != DcNullPtr){
	dor_object_delete_reference(device->pick_callback_object,device_object);
    }

    if (pick_callback != DcNullPtr){
	dor_object_add_reference(pick_callback,device_object);
    }

    device->pick_callback_object = pick_callback;
}
/*
 ======================================================================
 */
void dor_device_set_pick_path_order (
    dot_object      *device_object,
    DtPickPathOrder  pick_path_order)
{
    dot_device *device;

#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object,doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		     "DdSetPickPathOrder",DcNullPtr);
	return;
    }
#endif

    device = (dot_device *)(device_object->data);

    device->pick_path_order = pick_path_order;
}
/*
 ======================================================================
 */
void dor_device_set_shade_mode (
    dot_object *device_object,
    DtShadeMode mode)
{
    dot_device *device;

#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object,doe_device_class_id))
	    DDerror(ERR_INVALID_DEVICE,
			 "DdSetShadeMode",DcNullPtr);
#endif
    device = (dot_device *)(device_object->data);

    device->shade_mode = mode;
}
/*
 ======================================================================
 */
void dor_device_set_shade_ranges (
    dot_object *device_object,
    DtInt       start,
    DtInt       count,
    DtInt       entries[][2])
{
    DtInt rangemax;
    DtInt rangeindex;
    dot_device *device;

#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object,doe_device_class_id))
	    DDerror(ERR_INVALID_DEVICE,
			 "DdSetShadeMode",DcNullPtr);
#endif
    device = (dot_device *)(device_object->data);

    rangemax = start + count - 1;

    if ((rangemax+1) > device->shade_range_table_size){
	device->shade_ranges = (DtInt *)
		dor_space_reallocate(device->shade_ranges,
				     (rangemax+1)*2*(sizeof (DtInt)));

	device->shade_range_table_size = rangemax + 1;
    }

    for (rangeindex=start;rangeindex<=rangemax;rangeindex++){
	device->shade_ranges[2*rangeindex] = entries[rangeindex-start][0];
	device->shade_ranges[2*rangeindex+1] = entries[rangeindex-start][1];
    }
}
/*
 ======================================================================
 */
void dor_device_set_viewport (
    dot_object *device_object,
    DtVolume   *viewport)
{
    dot_device *device;

#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object,doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		     "DdSetViewport",DcNullPtr);
	return;
    }
#endif

    device = (dot_device *)(device_object->data);

    device->viewport = *viewport;

    dor_device_update_notify(device_object);
}
/*
 ======================================================================
 */
void dor_device_set_color_entries (
    dot_object   *device_object,
    DtColorModel  colortype,
    DtInt         start,
    DtInt         count,
    DtReal        entries[])
{
    int i, base, n ;
    dot_device *device;

#ifdef ERRCHK
    if( ! dor_object_confirm_type(device_object, doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		     "DdSetColorEntries", DcNullPtr);
	return;
    }

    if((start < 0) || (count < 0) ||
       (start + count > dor_device_inq_color_table_size(device_object))){
	DDerror(ERR_BAD_START_OR_COUNT,
		     "DdSetColorEntries", DcNullPtr);
	return;
    }

    if (colortype != DcRGB) {
	DDerror(ERR_VALUE_OUT_OF_RANGE, "DdSetColorEntries",
		     "unknown color type");
	return;
    }
#endif
  
    device = (dot_device *)(device_object->data) ;
  
    SET_CURRENT_DEVICE (device_object);

    base = 3*start ;
    n = 3*count ;

    for ( i = 0 ; i < n ; i++ )
	    device->color_map[i+base] = entries[i] ;

    OUTFUN(set_color_entries)(colortype, start, count, entries) ;
}
/*
 ======================================================================
 */
void dor_device_set_options (
    dot_object *device_object,
    DtPtr       argstring)
{
#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object, doe_device_class_id)) {
	DDerror(ERR_INVALID_DEVICE, "DdSetOptions", DcNullPtr);
	return;
    }
#endif

    SET_CURRENT_DEVICE(device_object);

    if (doe_device_current_functions->version_number >= 2) {
	OUTFUN(set_options)(argstring);
    }
}
