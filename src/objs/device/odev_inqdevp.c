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
	dor_device_get_aborted_draw_flag
	dor_device_inq_auto_size
	dor_device_inq_actual_viewport_ptr
	dor_device_inq_clipping_volume_ptr
	dor_device_inq_sequence_number
	dor_device_inq_view_extent
	dor_device_get_device_pick_volume
	dor_device_get_ftod_parms
	dor_device_get_scltrn_parms
	dor_device_inquire_clip_list
	dor_device_inq_device_extent
	dor_device_inquire_interface_functions
	dor_device_inquire_stereo
	dor_device_inq_shade_range

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/device.h>
/*
 ======================================================================
 */
DtFlag dor_device_get_aborted_draw_flag (dot_object *device_object)
{
    dot_device *device = (dot_device *)(device_object->data);

    return (device->aborted_draw_flag);
}
/*
 ======================================================================
 */
DtFlag dor_device_inq_auto_size (dot_object *device_object)
{
    dot_device *device = (dot_device *)(device_object->data);

    return(device->auto_size);
}
/*
 ======================================================================
 */
DtVolume *dor_device_inq_actual_viewport_ptr (dot_object *device_object)
{
    dot_device *device = (dot_device *)(device_object->data);

    return (&(device->actual_viewport));
}
/*
 ======================================================================
 */
DtVolume *dor_device_inq_clipping_volume_ptr (dot_object *device_object)
{
    dot_device *device = (dot_device *)(device_object->data);

    return (&(device->clipping_volume));
}
/*
 ======================================================================
 */
void dor_device_inq_sequence_number (
    dot_object *device_object,
    DtInt      *sequencenumber)
{
    dot_device *device = (dot_device *)(device_object->data);

    *sequencenumber = device->modification_sequence_number;
}
/*
 ======================================================================
 */
void dor_device_inq_view_extent(
    dot_object *device,
    dot_object *view,
    DtVolume   *actual_extent,
    DtVolume   *display_extent)
{
    dot_device *devicedata = (dot_device *)(device->data);
    DtVolume *boundary;
    DtReal *scale,*translate;

    boundary = dor_view_inq_boundary_ptr(view);

    display_extent->bll[0] = devicedata->actual_viewport.bll[0];
    display_extent->bll[1] = devicedata->actual_viewport.bll[1];
    display_extent->bll[2] = devicedata->actual_viewport.bll[2];

    display_extent->fur[0] = devicedata->actual_viewport.fur[0];
    display_extent->fur[1] = devicedata->actual_viewport.fur[1];
    display_extent->fur[2] = devicedata->actual_viewport.fur[2];

    /* compute image of view boundary on device's view surface */

    scale = devicedata->ftodscale;
    translate = devicedata->ftodtranslate;

    actual_extent->bll[0] = boundary->bll[0]*scale[0]+translate[0];
    actual_extent->bll[1] = boundary->bll[1]*scale[1]+translate[1];
    actual_extent->bll[2] = boundary->bll[2]*scale[2]+translate[2];

    actual_extent->fur[0] = boundary->fur[0]*scale[0]+translate[0];
    actual_extent->fur[1] = boundary->fur[1]*scale[1]+translate[1];
    actual_extent->fur[2] = boundary->fur[2]*scale[2]+translate[2];
}
/*
 ======================================================================
 */
DtVolume *dor_device_get_device_pick_volume (dot_object *device_object)
{
    dot_device *device = (dot_device *)(device_object->data);

    return(&(device->device_pick_volume));
}
/*
 ======================================================================
 */
void dor_device_get_ftod_parms (
    dot_object *device_object,
    DtReal      ftodscale[3],
    DtReal      ftodtrans[3])
{
    dot_device *device = (dot_device *)(device_object->data);
    DtInt coordno;

    for (coordno=0;coordno<3;coordno++){
	ftodscale[coordno] = device->ftodscale[coordno];
	ftodtrans[coordno] = device->ftodtranslate[coordno];
    }
}
/*
 ======================================================================
 */
void dor_device_get_scltrn_parms (
    dot_object *device_object,
    DtReal      ctodscale[3],
    DtReal      ctodtranslate[3])
{
    dot_device *device = (dot_device *)(device_object->data);
    DtInt coordno;

    for (coordno=0;coordno<3;coordno++){
	ctodscale[coordno] = device->ctodscale[coordno];
	ctodtranslate[coordno] = device->ctodtranslate[coordno];
    }
}
/*
 ======================================================================
 */
void dor_device_inquire_clip_list (
    dot_object *device_object,
    DtUInt     *nrects,
    DtReal    **clip_list,
    DtReal      extent[4],
    DtFlag     *partiallyobscuredflag)
{
    SET_CURRENT_DEVICE(device_object);
    OUTFUN(inquire_clip_list)(nrects,clip_list,extent,partiallyobscuredflag);
}
/*
 ======================================================================
 */
void dor_device_inq_device_extent (
    dot_object *device_object,
    DtReal     *xmin,
    DtReal     *ymin,
    DtReal     *zmin,
    DtReal     *xmax,
    DtReal     *ymax,
    DtReal     *zmax)
{
    DtVolume volume;

    dor_device_inq_extent(device_object, &volume);

    *xmin = volume.bll[0] ;
    *ymin = volume.bll[1] ;
    *zmin = volume.bll[2] ;

    *xmax = volume.fur[0];
    *ymax = volume.fur[0];
    *zmax = volume.fur[2];
}
/*
 ======================================================================
 */
void dor_device_inquire_interface_functions (
    dot_object *device_object,
    DtInt       type,
    DtPtr      *fcns)
{
    SET_CURRENT_DEVICE(device_object);
    (*doe_device_current_driver->get_interface_functions)(type, fcns);
}
/*
 ======================================================================
 */
void dor_device_inquire_stereo (
    dot_object *device_object,
    DtFlag     *stereo)
{
    SET_CURRENT_DEVICE(device_object);
    OUTFUN(inquire_stereo)(stereo);
}
/*
 ======================================================================
 */
void dor_device_inq_shade_range (
    dot_object *device_object,
    DtInt       index,
    DtInt      *range_min,
    DtInt      *range_max)
{
    dot_device *device = (dot_device *)(device_object->data);

    if (index < 0 || index > device->shade_range_table_size) {
	*range_min = device->shade_ranges[0];
	*range_max = device->shade_ranges[1];
    } else {
	*range_min = device->shade_ranges[2*index];
	*range_max = device->shade_ranges[2*index+1];
    }
}
