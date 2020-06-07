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
	DtPtr ddr_template_return_DCM_fcns ()
	DtPtr ddr_template_dcm_create_local_data (device)
	DtFlag ddr_template_dcm_initialize_device (device, device_data, argstring)
	void ddr_template_dcm_close_device ()
	void ddr_template_dcm_flush ()
	void ddr_template_dcm_swap_buffers ()
	void ddr_template_dcm_update_geometry ()
	void ddr_template_dcm_become_current_driver (device_data)
	void ddr_template_dcm_set_options (argstring)
	static int parse_geometry(string, xoffset, yoffset, width, height)
  ======================================================================
 */
#include "dcm.h"
/*
 ======================================================================
 */

ddt_template_data *dde_template_current_device_data;

/*
 ======================================================================
 */

extern DtPtr ddr_template_dcm_create_local_data();
extern DtFlag ddr_template_dcm_initialize_device();
extern void ddr_template_dcm_become_current_driver();
extern void ddr_template_dcm_close_device();
extern void ddr_template_dcm_flush();
extern void ddr_template_dcm_swap_buffers();
extern void ddr_template_dcm_update_geometry();
extern void ddr_template_dcm_inquire_device_extent();
extern void ddr_template_dcm_inquire_stereo();
extern void ddr_template_dcm_inquire_ncolors();
extern void ddr_template_dcm_inquire_resolution();
extern void ddr_template_dcm_inquire_visual_type();
extern void ddr_template_dcm_inquire_auto_size();
extern void ddr_template_dcm_inquire_color_entries();
extern void ddr_template_dcm_set_color_entries();
extern void ddr_template_dcm_set_foreground_color();
extern void ddr_template_dcm_set_background_color();
extern void ddr_template_dcm_set_shade_index();
extern void ddr_template_dcm_inquire_clip_list();
extern void ddr_template_dcm_clear_rectangle_depth();
extern void ddr_template_dcm_clear_rectangle_depth_and_color();
extern void ddr_template_dcm_set_depth_buffer_enable();
extern void ddr_template_dcm_set_depth_buffer_write();
extern void ddr_template_dcm_write_scanline_byte();
extern void ddr_template_dcm_set_current_view();
extern DtFlag ddr_template_dcm_inquire_pixel_data();
extern void ddr_template_dcm_set_options();

DDt_DCM *dde_template_DCM_fcns = (DDt_DCM *)0;
/*
 ======================================================================
 */
DtPtr ddr_template_return_DCM_fcns (void)
{
    DDt_DCM *fcns;

    if (dde_template_DCM_fcns == (DDt_DCM *)0) {
	fcns = DDdevice_CreateDCMStruct();

	if (fcns == (DDt_DCM *)0) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_template_return_DCM_fcns",
			DcNullPtr);
	}

	fcns->version_number 		= 2;
	/* Version 1 Functions */
	fcns->create_local_data 	= (DtPFI) ddr_template_dcm_create_local_data;
	fcns->initialize_device 	= (DtPFI) ddr_template_dcm_initialize_device;
	fcns->become_current_driver 	= (DtPFI) ddr_template_dcm_become_current_driver;
	fcns->close_device		= (DtPFI) ddr_template_dcm_close_device;
	fcns->flush 			= (DtPFI) ddr_template_dcm_flush;
	fcns->swap_buffers 		= (DtPFI) ddr_template_dcm_swap_buffers;
	fcns->update_geometry 		= (DtPFI) ddr_template_dcm_update_geometry;
	fcns->inquire_device_extent 	= (DtPFI) ddr_template_dcm_inquire_device_extent;
	fcns->inquire_stereo 		= (DtPFI) ddr_template_dcm_inquire_stereo;
	fcns->inquire_ncolors 		= (DtPFI) ddr_template_dcm_inquire_ncolors;
	fcns->inquire_resolution 	= (DtPFI) ddr_template_dcm_inquire_resolution;
	fcns->inquire_visual_type 	= (DtPFI) ddr_template_dcm_inquire_visual_type;
	fcns->inquire_auto_size 	= (DtPFI) ddr_template_dcm_inquire_auto_size;
	fcns->inquire_color_entries 	= (DtPFI) ddr_template_dcm_inquire_color_entries;
	fcns->set_color_entries 	= (DtPFI) ddr_template_dcm_set_color_entries;
	fcns->set_foreground_color 	= (DtPFI) ddr_template_dcm_set_foreground_color;
	fcns->set_background_color 	= (DtPFI) ddr_template_dcm_set_background_color;
	fcns->set_shade_index 		= (DtPFI) ddr_template_dcm_set_shade_index;
	fcns->inquire_clip_list 	= (DtPFI) ddr_template_dcm_inquire_clip_list;
	fcns->clear_rectangle_depth 	= (DtPFI) ddr_template_dcm_clear_rectangle_depth;
	fcns->clear_rectangle_depth_and_color 	= (DtPFI) ddr_template_dcm_clear_rectangle_depth_and_color;
	fcns->set_depth_buffer_enable 	= (DtPFI) ddr_template_dcm_set_depth_buffer_enable;
	fcns->set_depth_buffer_write 	= (DtPFI) ddr_template_dcm_set_depth_buffer_write;
	fcns->write_scanline_byte 	= (DtPFI) ddr_template_dcm_write_scanline_byte;
	fcns->set_current_view 		= (DtPFI) ddr_template_dcm_set_current_view;
	fcns->inquire_pixel_data 		= (DtPFI) ddr_template_dcm_inquire_pixel_data;
	/* Version 2 Functions */
	fcns->set_options		= (DtPFI) ddr_template_dcm_set_options;

	dde_template_DCM_fcns 		= fcns;
    }
    return (DtPtr)dde_template_DCM_fcns;
}
/*
 ======================================================================
 */
DtPtr ddr_template_dcm_create_local_data (
    DtPtr device)
{
    ddt_template_data *device_data;

    if ((device_data = (ddt_template_data *)
	 DDspace_Allocate(sizeof (ddt_template_data))) == (ddt_template_data *)0) {
	DDerror (ERR_CANT_ALLOC_MEM_SIZE, 
		 "ddr_template_dcm_create_local_data",
		 "sizeof (ddt_template_data)");
	return (DcNullPtr);
    }

    device_data->device = device;

    return ((DtPtr) device_data);
}
/*
 ======================================================================
 */
DtFlag ddr_template_dcm_initialize_device (
    DtPtr device,
    ddt_template_data *device_data,
    DtPtr argstring)
{
    char *argstringcopy, *tmp, *p, *nextarg;
    static char *funcname  = "ddr_template_dcm_initialize_device";
    int error=0;
    int x = 0;
    int y = 0;
    int width = 512;
    int height = 512;

    /*
     * This function allocates the device driver instance specific
     * storage and setups up the device for display.
     */

    device_data->device = device;
    device_data->auto_size = DcTrue;
    device_data->visualtype = DcTrueColor;
    device_data->double_buffered = DcTrue;

    /*
     * Process the argstring
     */

    argstringcopy = (char *)DDspace_Allocate(strlen(argstring)+1);
    strcpy(argstringcopy, argstring); /* so as not to alter user's string */
    tmp = argstringcopy;

    /* p points to first sub-string */

    while ((p = DDstrtok(tmp, " \t,"))) {
	tmp = 0;

	if (!strcmp(p,"-noautosize")) {
	    device_data->auto_size = DcFalse;
	} else if (!strcmp(p, "-visualtype")) {
	    if (!(nextarg = DDstrtok(0, " \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			 funcname, "visualtype: (no value)");
		error = 1;
	    } else {
		if ( ! strcmp("DcStaticGrey", nextarg)) {
		    device_data->visualtype = DcStaticGrey;
		} else if ( ! strcmp("DcGreyScale", nextarg)) {
		    device_data->visualtype = DcGreyScale;
		} else if ( ! strcmp ("DcStaticColor", nextarg)) {
		    device_data->visualtype = DcStaticColor;
		} else if ( ! strcmp ("DcPseudoColor", nextarg)) {
		    device_data->visualtype = DcPseudoColor;
		} else if ( ! strcmp ("DcTrueColor", nextarg)) {
		    device_data->visualtype = DcTrueColor;
		} else if ( ! strcmp ("DcDirectColor", nextarg)) {
		    device_data->visualtype = DcDirectColor;
		} else {
		    sprintf (DDerror_GetLine(), "visualtype: '%s'", nextarg);
		    DDerror (ERR_BAD_DEVICE_OPT_VAL,
			     funcname, DDerror_GetLine());
		    error = 1;
		}
	    } 
	} else if (!strcmp(p,"-geometry")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			 funcname, "width: (no value)");
		error = 1;
	    } else {
		parse_geometry (nextarg, &x, &y, &width, &height);
	    }
	} else if (!strcmp(p,"-singlebuffered")) {
	    device_data->double_buffered = DcFalse;
	} else {
	    sprintf(DDerror_GetLine(), "ignored flag '%s'", p);
	    DDerror (ERR_BAD_DEVICE_OPT,
		     funcname, DDerror_GetLine());
	}
    }

    DDspace_Deallocate(argstringcopy);

    if (error) {
	return(DcFalse);
    }

    device_data->x = x;
    device_data->y = y;
    device_data->width = width;
    device_data->height = height;

    /*
     * do device specific stuff
     */



    return DcTrue;
}
/*
 ======================================================================
 */
void ddr_template_dcm_close_device (void)
{
}
/*
 ======================================================================
 */
void ddr_template_dcm_flush (void)
{
}
/*
 ======================================================================
 */
void ddr_template_dcm_swap_buffers (void)
{
}
/*
 ======================================================================
 */
void ddr_template_dcm_update_geometry (void)
{
}
/*
 ======================================================================
 */
void ddr_template_dcm_become_current_driver (
    ddt_template_data *device_data)
{
    dde_template_current_device_data = device_data;
}
/*
 ======================================================================
 */
void ddr_template_dcm_set_options (
    DtPtr argstring)
{
    char		*argstringcopy;
    char		*tmp;
    char		*p;
    ddt_template_data 	*device_data;

    device_data = dde_template_current_device_data;

    argstringcopy = (char *)DDspace_Allocate(strlen(argstring)+1);
    strcpy(argstringcopy, argstring); /* so as not to alter user's string */
    tmp = argstringcopy;

    /* p points to first sub-string */

    while ((p = DDstrtok(tmp, " \t,"))) {
	tmp = 0;

	if (!strcmp(p, "-doublebuffered")) {
	    
	    device_data->double_buffered = DcTrue;

	} else if (!strcmp(p, "-singlebuffered")) {
	    device_data->double_buffered = DcFalse;
	}
    }

    DDspace_Deallocate(argstringcopy);
}
/*
 ======================================================================
 */
#define isnum(arg) ((arg) >= '0' && (arg) <= '9')
#define GetInt(arg,val) for ((val)=0; isnum(*(arg)); (arg)++) \
	    (val) = ((val) * 10) + (*(arg) - '0')

/*
 ======================================================================
 Returns a bitmask indicating if either the xoffset or yoffset are 
 negative.  If they are then the offset is relative to the other side of
 the device.  ie -xoffset is from the right edge of the device rather
 than the left edge.
 The returned values will always be positive.

 0x1 indicates xoffset is negative
 0x2 indicates yoffset is negative
 ======================================================================
 */
static int parse_geometry (
    char *arg,
    int *xoffset,
    int *yoffset,
    int *width,
    int *height)
{
    char *string;
    int rtn = 0;

    string = arg;

    if (*string == '=') string++;

    if (isnum(*string)){
	GetInt (string, *width);
	if (*string++ == 'x' && isnum(*string)) {
	    GetInt (string, *height);
	} else {
	    return rtn;
	}
    }
    if ((*string == '+' || *string == '-') && isnum(*(string+1))){
	if (*string++ == '-') rtn |= 1;
	GetInt (string, *xoffset);
	if ((*string == '+' || *string == '-') && isnum(*(string+1))){
	    if (*string++ == '-') rtn |= 2;
	    GetInt (string, *yoffset);
	}
    }
    return rtn;
}
