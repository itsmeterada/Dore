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
	void dor_device_set_aborted_draw_flag (device_object, switchvalue)
	void dor_device_set_auto_size (device_object,auto_size)
	void dor_device_set_background_color (device_object, colormodel, color)
	void dor_device_set_current_view (device_object, view_object)
	void dor_device_set_depth_buffer_enable (device_object, flag)
	void dor_device_set_depth_buffer_write (device_object, flag)
	void dor_device_set_foreground_color (device_object, colormodel, color)
	void dor_device_set_shade_index (device_object, index)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/device.h>
/*
 ======================================================================
 */
void dor_device_set_aborted_draw_flag (
    dot_object *device_object,
    DtFlag      switchvalue)
{
    dot_device *device = (dot_device *)(device_object->data);

    device->aborted_draw_flag = switchvalue;
    dor_device_update_notify(device_object);
}
/*
 ======================================================================
 */
void dor_device_set_auto_size (
    dot_object *device_object,
    DtFlag      auto_size)
{
    dot_device *device = (dot_device *)(device_object->data);

    device->auto_size = auto_size;

    dor_device_update_notify(device_object);
}
/*
 ======================================================================
 */
void dor_device_set_background_color (
    dot_object   *device_object,
    DtColorModel  colormodel,
    DtReal        color[])
{
    SET_CURRENT_DEVICE(device_object);

    doe_device_current_device->background_color[0] = color[0] ;
    doe_device_current_device->background_color[1] = color[1] ;
    doe_device_current_device->background_color[2] = color[2] ;

    OUTFUN(set_background_color)(colormodel, color) ;
}
/*
 ======================================================================
 */
void dor_device_set_current_view (
    dot_object *device_object,
    DtObject    view_object)
{
    SET_CURRENT_DEVICE(device_object);
    OUTFUN(set_current_view)(view_object);
}
/*
 ======================================================================
 */
void dor_device_set_depth_buffer_enable (
    dot_object *device_object,
    DtFlag      flag)
{
    SET_CURRENT_DEVICE(device_object);

    doe_device_current_device->depth_buffer_enable = flag ;
    OUTFUN(set_depth_buffer_enable)(flag) ;
}
/*
 ======================================================================
 */
void dor_device_set_depth_buffer_write (
    dot_object *device_object,
    DtFlag      flag)
{
    SET_CURRENT_DEVICE(device_object);

    doe_device_current_device->depth_buffer_write = flag ;
    OUTFUN(set_depth_buffer_write)(flag) ;
}
/*
 ======================================================================
 */
void dor_device_set_foreground_color (
    dot_object   *device_object,
    DtColorModel  colormodel,
    DtReal        color[])
{
    SET_CURRENT_DEVICE(device_object);

    doe_device_current_device->foreground_color[0] = color[0] ;
    doe_device_current_device->foreground_color[1] = color[1] ;
    doe_device_current_device->foreground_color[2] = color[2] ;

    OUTFUN(set_foreground_color)(colormodel, color) ;
}
/*
 ======================================================================
 */
void dor_device_set_shade_index (
    dot_object *device_object,
    DtInt       index)
{
    SET_CURRENT_DEVICE(device_object);

    doe_device_current_device->shade_index = index;
    OUTFUN(set_shade_index)(index);
}
