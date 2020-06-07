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
	dor_device_inq_fonts
	dor_device_inq_frame
	dor_device_inq_num_fonts
	dor_device_inq_pick_aperture
	dor_device_inq_pick_callback
	dor_device_inq_pick_path_order
	dor_device_inq_shade_mode
	dor_device_inq_shade_ranges
	dor_device_inq_visual_type
	dor_device_inq_viewport
	dor_device_inq_color_entries
	dor_device_inq_color_table_size
	dor_device_inq_extent
	dor_device_inq_pixeldata
	dor_device_inq_resolution
	dor_device_inq_num_drivers
	dor_device_inq_driver_info

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/device.h>

/* ====================================================================== */

void dor_device_inq_fonts (
    dot_object     *device_object,
    DtFontPrecision fonts[])
{
    register int ii;

#   ifdef ERRCHK
	if (! dor_object_confirm_type (device_object,doe_device_class_id)){
	    DDerror (ERR_INVALID_DEVICE, "DdInqFonts",DcNullPtr);
	    return;
	}
#   endif

    for (ii=0;  ii < DcAstrology;  ++ii){
	fonts[ii].font = ii + 1;
	fonts[ii].precision = DcStrokePrecision;
    }
}

/* ====================================================================== */

dot_object *dor_device_inq_frame (dot_object *device_object)
{
    register dot_device *device;

#   ifdef ERRCHK
	if (!dor_object_confirm_type(device_object,doe_device_class_id)){
	    DDerror (ERR_INVALID_DEVICE, "DdInqFrame", DcNullPtr);
	    return DcNullPtr;
	}
#   endif

    device = (dot_device *)(device_object->data);

    return device->frame;
}

/* ====================================================================== */

DtInt dor_device_inq_num_fonts (dot_object *device_object)
{
#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object,doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		     "DdInqNumFonts",DcNullPtr);
	return(-1);
    }
#endif

    return(DcAstrology);
}
/*
 ======================================================================
 */
void dor_device_inq_pick_aperture (
    dot_object *device_object,
    DtSize3    *pick_aperture)
{
    dot_device *device;

#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object,doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		     "DdInqPickAperture",DcNullPtr);
	return;
    }
#endif

    device = (dot_device *)(device_object->data);

    *pick_aperture = device->pick_aperture;
}
/*
 ======================================================================
 */
dot_object *dor_device_inq_pick_callback (dot_object *device_object)
{
    dot_device *device;

#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object,doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		     "DdInqPickCallBack",DcNullPtr);
	return(DcNullObject);
    }
#endif

    device = (dot_device *)(device_object->data);

    return(device->pick_callback_object);
}
/*
 ======================================================================
 */
DtPickPathOrder dor_device_inq_pick_path_order(dot_object *device_object)
{
    dot_device *device;

#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object,doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		     "DdInqPickPathOrder",DcNullPtr);
	return -1;		/* returns garbage on error */
    }
#endif

    device = (dot_device *)(device_object->data);

    return(device->pick_path_order);
}
/*
 ======================================================================
 */
DtShadeMode dor_device_inq_shade_mode (dot_object *device_object)
{
    dot_device *device;

#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object,doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		     "DdInqShadeMode",DcNullPtr);
	return -1;	/* note: returns garbage on error */
    }
#endif

    device = (dot_device *)(device_object->data);

    return(device->shade_mode);
}
/*
 ======================================================================
 */
void dor_device_inq_shade_ranges (
    dot_object *device_object,
    DtInt       start,
    DtInt       count,
    DtInt       entries[])
{
    dot_device *device;
    DtInt i;

    device = (dot_device *)(device_object->data);

#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object,doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		     "DdInqShadeRanges",DcNullPtr);
	return;
    }

    if ((start < 0) || (count < 0) || ((start+count) >
				       device->shade_range_table_size)){
	DDerror(ERR_BAD_START_OR_COUNT,
		     "DdInqShadeRanges",DcNullPtr);
	return;
    }
#endif

    for (i=0;i<count;i++){
	entries[2*i] = device->shade_ranges[2*(i+start)];
	entries[2*i+1] = device->shade_ranges[2*(i+start)+1];
    }
}
/*
 ======================================================================
 */
DtVisualType dor_device_inq_visual_type (dot_object *device_object)
{
    dot_device *device;

#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object,doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		     "DdInqVisualType",DcNullPtr);
	return -1;	/* Note: garbage returned on error */
    }
#endif

    device = (dot_device *)(device_object->data);

    return(device->visual_type);
}
/*
 ======================================================================
 */
void dor_device_inq_viewport (
    dot_object *device_object,
    DtVolume   *viewport)
{
    dot_device *device;

#ifdef ERRCHK
    if (!dor_object_confirm_type(device_object,doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		     "DdInqViewport",DcNullPtr);
	return;
    }
#endif

    device = (dot_device *)(device_object->data);

    *viewport = device->viewport;
}
/*
 ======================================================================
 */
void dor_device_inq_color_entries (
    dot_object  *device_object,
    DtColorModel colortype,
    DtInt        start,
    DtInt        count,
    DtReal       entries[])
{
    dot_device *device;
    int i, base, n ;

#ifdef ERRCHK
    if( ! dor_object_confirm_type(device_object, doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		     "DdInqColorEntries", DcNullPtr);
	return;
    }

    if((start < 0) || (count < 0) ||
       (start + count > dor_device_inq_color_table_size(device_object))){
	DDerror(ERR_BAD_START_OR_COUNT,
		     "DdInqColorEntries", DcNullPtr);
	return;
    }

    if (colortype != DcRGB) {
	DDerror(ERR_VALUE_OUT_OF_RANGE, "DdInqColorEntries",
		     "unknown color type");
	return;
    }
#endif


    device = (dot_device *)(device_object->data) ;
    base = start*3 ;	
    n = count*3 ;

    for ( i = 0 ; i < n ; i++ ) {
	entries[i] = device->color_map[base+i] ;
    }
}
/*
 ======================================================================
 */

DtInt dor_device_inq_color_table_size (dot_object *device_object)
{
    dot_device *device;

#ifdef ERRCHK
    if( ! dor_object_confirm_type(device_object, doe_device_class_id)) {
    	DDerror(ERR_INVALID_DEVICE,
    		     "DdInqColorTableSize", DcNullPtr);
    	return(-1);
    }
#endif

    device = (dot_device *)(device_object->data) ;

    return device->ncolors ;
}
/*
 ======================================================================
 */
void dor_device_inq_extent (dot_object *device_object, DtVolume *volume)
{
    dot_device *device;

#ifdef ERRCHK
    if( ! dor_object_confirm_type(device_object, doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		     "DdInqExtent", DcNullPtr);
	return;
    }
#endif

    device = (dot_device *)(device_object->data) ;

    SET_CURRENT_DEVICE (device_object);

    OUTFUN(inquire_device_extent)(&device->extent) ;
    *volume = device->extent ;

    device->xmin = volume->bll[0];
    device->ymin = volume->bll[1];
    device->xmax = volume->fur[0];
    device->ymax = volume->fur[1];
}
/*
 ======================================================================
 */
DtFlag dor_device_inq_pixeldata (
    dot_object   *device_object,
    DtRasterType  requesttype,
    DtInt        *width,
    DtInt        *height,
    DtRasterType *type,
    DtPtr        *data,
    DtFlag       *userdelete)
{
    DtFlag result;
    dot_device *device;

    device = (dot_device *)(device_object->data);

#ifdef ERRCHK
    if (! dor_object_confirm_type(device_object,doe_device_class_id)) {
	DDerror(ERR_INVALID_DEVICE,
		     "DdInqPixelData",DcNullPtr);
	return DcFalse;
    }
#endif

    SET_CURRENT_DEVICE(device_object);
    result = (DtFlag) OUTFUN(inquire_pixel_data)(requesttype, width, 
				height, type, data, userdelete);

    /* 
     * The data is bad if aborted_draw_flag is set (due to events such
     * as unsynchronised locks between a driver and the X server, for example).  
     * Update the geometry and inquire again.
     */

    while (device->aborted_draw_flag) {

	dor_device_update_geometry(device_object);
	device->aborted_draw_flag = DcFalse;

	if (*userdelete) 
            DDspace_Deallocate(*data);

        result = (DtFlag) OUTFUN(inquire_pixel_data)(requesttype, width, 
				height, type, data, userdelete);
    }

    return (result);
}
/*
 ======================================================================
 */
void dor_device_inq_resolution(
    dot_object *device_object,
    DtReal     *x,
    DtReal     *y)
{
    dot_device *device;

#ifdef ERRCHK
    if( ! dor_object_confirm_type(device_object, doe_device_class_id)){
	DDerror(ERR_INVALID_DEVICE,
		     "DdInqResolution", DcNullPtr);
	return;
    }
#endif

    device = (dot_device *)(device_object->data) ;

    *x = device->resolution[0];
    *y = device->resolution[1];
}
/*
 ======================================================================
 */
DtInt dor_device_inq_num_drivers (void)
{
    return doe_device_driver_count;
}
/*
 ======================================================================
 */
void dor_device_inq_driver_info (
    DtInt  count,
    char  *name[],
    char  *description[])
{
    DtInt i;

    for (i=0; i<doe_device_driver_count && i < count; i++) {
	name[i] = doe_device_drivers[i].device_name;
	description[i] = doe_device_drivers[i].description;
    }
}
