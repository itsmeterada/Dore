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
	DtPtr ddr_x11_XFDI_return_DCM_fcns ()
	DtPtr ddr_x11_XFDI_dcm_create_local_data (device)
	DtFlag ddr_x11_XFDI_dcm_initialize_device (device, devicedata, argstring)
	static create_ramp_colormap (ncolors, colors)
	void ddr_x11_XFDI_dcm_close_device ()
	void ddr_x11_XFDI_dcm_flush ()
	void ddr_x11_XFDI_dcm_swap_buffers ()
	void ddr_x11_XFDI_dcm_update_geometry ()
	void ddr_x11_XFDI_dcm_become_current_driver (device_data)
	void ddr_x11_XFDI_dcm_setup_shading ()

  ======================================================================
 */
#include "dcm.h"
#include "dore/dore_develop/develop.h"

ddt_x11_XFDI_data *dde_x11_XFDI_current_dcm_data;

struct list *dde_x11_XFDI_dcm_list = NULL;
/*
 ======================================================================
 */
extern DtPtr ddr_x11_XFDI_dcm_create_local_data();
extern DtFlag ddr_x11_XFDI_dcm_initialize_device();
extern void ddr_x11_XFDI_dcm_become_current_driver();
extern void ddr_x11_XFDI_dcm_close_device();
extern void ddr_x11_XFDI_dcm_flush();
extern void ddr_x11_XFDI_dcm_swap_buffers();
extern void ddr_x11_XFDI_dcm_update_geometry();
extern void ddr_x11_XFDI_dcm_inquire_device_extent();
extern void ddr_x11_XFDI_dcm_inquire_stereo();
extern void ddr_x11_XFDI_dcm_inquire_ncolors();
extern void ddr_x11_XFDI_dcm_inquire_resolution();
extern void ddr_x11_XFDI_dcm_inquire_visual_type();
extern void ddr_x11_XFDI_dcm_inquire_auto_size();
extern void ddr_x11_XFDI_dcm_inquire_color_entries();
extern void ddr_x11_XFDI_dcm_set_color_entries();
extern void ddr_x11_XFDI_dcm_set_foreground_color();
extern void ddr_x11_XFDI_dcm_set_background_color();
extern void ddr_x11_XFDI_dcm_set_shade_index();
extern void ddr_x11_XFDI_dcm_inquire_clip_list();
extern void ddr_x11_XFDI_dcm_clear_rectangle_depth();
extern void ddr_x11_XFDI_dcm_clear_rectangle_depth_and_color();
extern void ddr_x11_XFDI_dcm_set_depth_buffer_enable();
extern void ddr_x11_XFDI_dcm_set_depth_buffer_write();
extern void ddr_x11_XFDI_dcm_write_scanline_byte();
extern void ddr_x11_XFDI_dcm_set_current_view();
extern DtFlag ddr_x11_XFDI_dcm_inquire_pixel_data();

DDt_DCM *dde_x11_XFDI_DCM_fcns = (DDt_DCM *)0;

DtPtr
ddr_x11_XFDI_return_DCM_fcns()
{
    DDt_DCM *fcns;
    DDt_DCM *DDdevice_CreateDCMStruct();

    if (dde_x11_XFDI_DCM_fcns == (DDt_DCM *)0) {
	fcns = DDdevice_CreateDCMStruct();

	if (fcns == (DDt_DCM *)0) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_x11_XFDI_return_DCM_fcns",
			DcNullPtr);
	}

	fcns->version_number 			= 1;
	/* Version 1 Functions */
	fcns->create_local_data 		= (DtPFI) ddr_x11_XFDI_dcm_create_local_data;
	fcns->initialize_device 		= (DtPFI) ddr_x11_XFDI_dcm_initialize_device;
	fcns->become_current_driver 		= (DtPFI) ddr_x11_XFDI_dcm_become_current_driver;
	fcns->close_device			= (DtPFI) ddr_x11_XFDI_dcm_close_device;
	fcns->flush 				= (DtPFI) ddr_x11_XFDI_dcm_flush;
	fcns->swap_buffers 			= (DtPFI) ddr_x11_XFDI_dcm_swap_buffers;
	fcns->update_geometry 			= (DtPFI) ddr_x11_XFDI_dcm_update_geometry;
	fcns->inquire_device_extent 		= (DtPFI) ddr_x11_XFDI_dcm_inquire_device_extent;
	fcns->inquire_stereo 			= (DtPFI) ddr_x11_XFDI_dcm_inquire_stereo;
	fcns->inquire_ncolors 			= (DtPFI) ddr_x11_XFDI_dcm_inquire_ncolors;
	fcns->inquire_resolution 		= (DtPFI) ddr_x11_XFDI_dcm_inquire_resolution;
	fcns->inquire_visual_type 		= (DtPFI) ddr_x11_XFDI_dcm_inquire_visual_type;
	fcns->inquire_auto_size 		= (DtPFI) ddr_x11_XFDI_dcm_inquire_auto_size;
	fcns->inquire_color_entries 		= (DtPFI) ddr_x11_XFDI_dcm_inquire_color_entries;
	fcns->set_color_entries 		= (DtPFI) ddr_x11_XFDI_dcm_set_color_entries;
	fcns->set_foreground_color 		= (DtPFI) ddr_x11_XFDI_dcm_set_foreground_color;
	fcns->set_background_color 		= (DtPFI) ddr_x11_XFDI_dcm_set_background_color;
	fcns->set_shade_index 			= (DtPFI) ddr_x11_XFDI_dcm_set_shade_index;
	fcns->inquire_clip_list 		= (DtPFI) ddr_x11_XFDI_dcm_inquire_clip_list;
	fcns->clear_rectangle_depth 		= (DtPFI) ddr_x11_XFDI_dcm_clear_rectangle_depth;
	fcns->clear_rectangle_depth_and_color 	= (DtPFI) ddr_x11_XFDI_dcm_clear_rectangle_depth_and_color;
	fcns->set_depth_buffer_enable 		= (DtPFI) ddr_x11_XFDI_dcm_set_depth_buffer_enable;
	fcns->set_depth_buffer_write 		= (DtPFI) ddr_x11_XFDI_dcm_set_depth_buffer_write;
	fcns->write_scanline_byte 		= (DtPFI) ddr_x11_XFDI_dcm_write_scanline_byte;
	fcns->set_current_view 			= (DtPFI) ddr_x11_XFDI_dcm_set_current_view;
	fcns->inquire_pixel_data 			= (DtPFI) ddr_x11_XFDI_dcm_inquire_pixel_data;

	dde_x11_XFDI_DCM_fcns 			= fcns;
    }
    return (DtPtr)dde_x11_XFDI_DCM_fcns;
}
/*
 ======================================================================
 */

DtPtr 
ddr_x11_XFDI_dcm_create_local_data (device)
     DtPtr device;
{
    ddt_x11_XFDI_data *device_data;

    if ((device_data = 
	 (ddt_x11_XFDI_data *)DDspace_Allocate(sizeof (ddt_x11_XFDI_data))) == NULL) {
	DDerror (ERR_CANT_ALLOC_MEM_SIZE, 
		    "ddr_x11_XFDI_dcm_create_local_data",
		    "sizeof (ddt_x11_XFDI_data)");
	return ((DtPtr) DcNullPtr);
    }

    device_data->device = device;

    if (dde_x11_XFDI_dcm_list == NULL) {
#ifdef DEBUG
	printf ("creating x11_XFDI_dcm_list\n");
#endif
	dde_x11_XFDI_dcm_list = (struct list *)dor_list_create();

	if (dde_x11_XFDI_dcm_list == NULL) {
	    DDerror (ERR_CANT_ALLOC_MEM_SIZE, 
			"ddr_x11_XFDI_dcm_create_local_data",
			"dor_list_create");

	    return ((DtPtr) device_data);
	}
    }

#ifdef DEBUG
    printf ("adding 0x%x device to list\n", device_data);
#endif
    dor_list_add_data_last(dde_x11_XFDI_dcm_list, device_data);

    return ((DtPtr) device_data);
}
/*
 ======================================================================
 */

DtFlag
ddr_x11_XFDI_dcm_initialize_device (device, devicedata, argstring)
     DtPtr device;
     ddt_x11_XFDI_data *devicedata;
     DtPtr argstring;
{
    char *	getenv();
    char *	display_name;
    XEvent	event;
    XColor	colors[4096];
    char		foo[10];
    unsigned long status;
    char *	argstringcopy, *tmp, *p, *nextarg, *DDstrtok();
    static char *funcname  = "ddr_x11_XFDI_dcm_initialize_device";
    int error=0;
    XFDIGC	gc;
    GC		xgc;
    Window	canvas;
    Drawable	Vdrawable;
    Drawable	main_drawable, z_buffer;
    Drawable	sort_z_buffer, sort_i_buffer, sort_a_buffer;
    Drawable	texture_buffer;
    Drawable	filter_buffer;
    char	*	cmap_name = (char *)0;
    char	*	ProgramName = "dore";
    float	zrange = 0.9;
    XVisualInfo	template;
    XVisualInfo *visinfo;
    XdbBufferAttributes att;
    Visual *	visual;
    int		nitems;
    int       	mask;
    XSizeHints	sizehints;
    Screen *	screen;
    XSetWindowAttributes	attributes;
    int attrMask;
    Colormap cmap;

    /* default values for those that might be found in the argstring */

    sizehints.flags = (USPosition|USSize);
    sizehints.x = 0;
    sizehints.y = 0;
    sizehints.width = 512;
    sizehints.height = 512;

    devicedata->device = device;
    devicedata->window = NULL;
    devicedata->user_window = NULL;
    devicedata->display = NULL;
    devicedata->user_display = NULL;
    devicedata->double_buffered = DcTrue;
    devicedata->Vdraw_buffer = NULL;
    devicedata->draw_buffer = NULL;
    devicedata->Vdraw_buffer_stereo = NULL;
    devicedata->draw_buffer_stereo = NULL;
    devicedata->texture_buffer = NULL;
    devicedata->filter_buffer = NULL;
    devicedata->z_buffer = NULL;
    devicedata->z_buffer_stereo = NULL;

    devicedata->use_spheres = DcFalse;
    devicedata->multi_pass_transp = DcFalse;
    devicedata->sort_z_buffer = NULL;
    devicedata->sort_i_buffer = NULL;
    devicedata->sort_a_buffer = NULL;

    devicedata->auto_size = DcTrue;
    devicedata->visualtype = DcTrueColor;
    devicedata->ncolors = 256;

    devicedata->stereoswitch = DcFalse;
    devicedata->stereoscreen = DcFalse;
    devicedata->current_view = DcNullObject;
    devicedata->previous_view = DcNullObject;

    /*
     * set defaults for Pseudo Color
     */

    devicedata->shade_index = 0;

    devicedata->cbtselpsweight[0] = 4096. / 256.; /* << 4 */
    devicedata->cbtselpsweight[1] = 256. / 256.; /* no shift */
    devicedata->cbtselpsweight[2] = 16. / 256.;	/* >> 4 */
	
    devicedata->cbtselpsmask[0] = 0xf00;
    devicedata->cbtselpsmask[1] = 0x0f0;
    devicedata->cbtselpsmask[2] = 0x00f;
	
    devicedata->cwtavgpsweight[0] = .30;
    devicedata->cwtavgpsweight[1] = .59;
    devicedata->cwtavgpsweight[2] = .11;
	
    /*
     * Process the argstring
     */

    argstringcopy = (char *)DDspace_Allocate(strlen(argstring)+1);
    strcpy(argstringcopy, argstring); /* so as not to alter user's string */
    tmp = argstringcopy;

    /* p points to first sub-string */

    while ((p = DDstrtok(tmp, " \t,"))) {
	tmp = 0;

	if (!strcmp(p,"-multipass_transparency")) {
	    devicedata->multi_pass_transp = DcTrue;

	} else if (!strcmp(p, "-nospheres")) {
	    devicedata->use_spheres = DcFalse;

	} else if (!strcmp(p, "-spheres")) {
	    devicedata->use_spheres = DcTrue;

	} else if (!strcmp(p,"-noautosize")) {
	    devicedata->auto_size = DcFalse;

	} else if (!strcmp(p,"-singlebuffered")) {
	    devicedata->double_buffered = DcFalse;

	} else if (!strcmp(p,"-window")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "window: (no value)");

		error = 1;

	    } else {
		int w;

		devicedata->user_window = 1;
		sscanf(nextarg,"%d",&w);
		devicedata->window = (Window)w;
	    }
	} else if (!strcmp(p,"-display")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "device: (no value)");

		error = 1;
	    } else {
		int dptr;

		devicedata->user_display = 1;
		sscanf(nextarg,"%d",&dptr);
		devicedata->display = (Display *)dptr;
	    }
	}
	else if (!strcmp(p,"-geometry")) {
	    if (!(nextarg = DDstrtok(0, " \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "geometry: (no value)");

		error = 1;
	    } else {
		XParseGeometry(nextarg,&sizehints.x,&sizehints.y,
			       &sizehints.width,&sizehints.height);
#ifdef DEBUG
		printf ("geometry = x %d, y %d, width %d, heigth %d\n",
			sizehints.x, sizehints.y, sizehints.width,
			sizehints.height);
#endif
	    }
	} else if (!strcmp(p, "-visualtype")) {
	    if (!(nextarg = DDstrtok(0, " \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "visualtype: (no value)");

		error = 1;
	    } else	{
		if (!strcmp("DcPseudoColor",nextarg)) {
		    devicedata->visualtype = DcPseudoColor;
		    devicedata->ncolors = 4096;

		} else if ( ! strcmp("DcTrueColor", nextarg)) {
		    devicedata->visualtype = DcTrueColor;
		    devicedata->ncolors = 256;
		} else if ( ! strcmp("DcDirectColor", nextarg)) {
		    devicedata->visualtype = DcDirectColor;
		    devicedata->ncolors = 256;
		}
		/*
		  else if ...
		  (others besides DcDirectColor)
		  */
		else {
		    sprintf (DDerror_GetLine(), "visualtype: '%s'", nextarg);
		    DDerror (ERR_BAD_DEVICE_OPT_VAL,
				funcname, DDerror_GetLine());

		    error = 1;
		}
	    }
	}
	else if (!strcmp(p,"-stereo")) {
	    devicedata->stereoswitch = DcTrue;

	} else {
	    sprintf(DDerror_GetLine(), "flag '%s'", p);
	    DDerror (ERR_BAD_DEVICE_OPT,
			funcname, DDerror_GetLine());
	}
    }

    DDspace_Deallocate(argstringcopy);

    if (error) 
	    return(DcFalse);

    if (!devicedata->display) {
	if ((display_name = getenv("DISPLAY")) == NULL) {
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, "Display Environment variable not set");

	    return (DcFalse);
	}
	
	if ((devicedata->display = XOpenDisplay(display_name)) == NULL) {
	    sprintf(DDerror_GetLine(),
		    "Could not open display %s",display_name);
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, DDerror_GetLine());

	    return (DcFalse);
	}
    }
		
    /* get some info from user's window */

    if (devicedata->window) {
	XWindowAttributes atts;

	XGetWindowAttributes(devicedata->display, devicedata->window,
			     &atts);
	sizehints.x = atts.x;
	sizehints.y = atts.y;
	sizehints.width = atts.width;
	sizehints.height = atts.height;

	switch(atts.visual->class) {
	case PseudoColor:
	    devicedata->visualtype = DcPseudoColor;
	    devicedata->ncolors = atts.visual->map_entries;
	    cmap = atts.colormap;
	    create_ramp_colormap(devicedata->ncolors, colors);
	    XStoreColors(devicedata->display, cmap, colors, devicedata->ncolors);
	    break;

	case DirectColor:
	    devicedata->visualtype = DcDirectColor;
	    devicedata->ncolors = atts.visual->map_entries;
	    break;

	case TrueColor:
	    devicedata->visualtype = DcTrueColor;
	    devicedata->ncolors = atts.visual->map_entries;
	    break;

	case GrayScale:
	case StaticGray:
	case StaticColor:
	default:
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, "Unknown visual type of user window");
	    return (DcFalse);
	}
    } else {
	XSetWindowAttributes window_attributes;

	/* Get the visual info for a visual of the type we want */

	switch (devicedata->visualtype) {
	case DcPseudoColor:
	    template.class = PseudoColor;
	    break;
	case DcDirectColor:
	    template.class = DirectColor;
	    break;
	case DcTrueColor:
	default:
	    template.class = TrueColor;
	    break;
	}

	visinfo = XGetVisualInfo(devicedata->display, VisualClassMask, &template,
				 &nitems);
	if (visinfo == NULL) {
	    DDerror(ERR_DEVICE_SYSTEM_ACCESS,
		       funcname, "Can't get requested visual");
	    return(DcFalse);
	}
	devicedata->ncolors = visinfo->colormap_size;
      
	screen = ScreenOfDisplay(devicedata->display,
				 DefaultScreen(devicedata->display));

	attrMask = CWBackPixel|CWBorderPixel;
	window_attributes.background_pixel = 0x000000;
	window_attributes.border_pixel = 0xffffff;

	switch (devicedata->visualtype) {
	case DcPseudoColor:
	    cmap = XCreateColormap(devicedata->display,
				   XDefaultRootWindow(devicedata->display),
				   visinfo->visual,AllocAll);
	    create_ramp_colormap(devicedata->ncolors, colors);
	    XStoreColors(devicedata->display, cmap, colors, devicedata->ncolors);

	    window_attributes.colormap = cmap;
	    attrMask |= CWColormap;
	    break;
	case DcDirectColor:
	    cmap = DefaultColormap(devicedata->display, 0);

	    window_attributes.colormap = cmap;
	    attrMask |= CWColormap;
	    break;
	}

	/* This is the visual that we will use */

	visual = visinfo->visual;

	devicedata->window = XCreateWindow(devicedata->display,
					   DefaultRootWindow(devicedata->display),
					   sizehints.x,sizehints.y,
					   sizehints.width,sizehints.height,
					   0,
					   visinfo->depth,
					   InputOutput,	
					   visual,
					   attrMask,
					   &window_attributes);
	sizehints.max_width = 1280;
	sizehints.max_height = 1024;
	sizehints.flags = USSize | USPosition | PMaxSize;
	XSetNormalHints(devicedata->display,devicedata->window,&sizehints);
	XSetStandardProperties(devicedata->display,devicedata->window,ProgramName,
			       ProgramName,None,(char **)NULL,0,&sizehints);

	XMapWindow(devicedata->display,devicedata->window);
    }

    xgc = XCreateGC(devicedata->display,devicedata->window,0,0);
    XSetForeground(devicedata->display,xgc,0xff0000);

    /* Make sure we catch our own expose event */

    if (!devicedata->user_window) {
	XSelectInput(devicedata->display,devicedata->window,ExposureMask);
	
	XMapWindow(devicedata->display,devicedata->window);

	while (XNextEvent(devicedata->display,&event)) {
	    if (event.type == Expose) 
		    break;
	}
    }

    XGetWindowAttributes(devicedata->display,devicedata->window,
			 &devicedata->window_attributes);


    att.update_action = XdbRetained;
    att.update_mode = XdbFrequently;
    att.resize_mode = XdbUndefinedBits;
    mask = DBUpdateAction | DBUpdateMode | DBResizeMode;
    main_drawable = XdbCreateBuffer(devicedata->display,devicedata->window,&att,
				    mask,NULL);
    Vdrawable = XVPInstance(devicedata->display, main_drawable, 0, 0,
			    devicedata->window_attributes.width,
			    devicedata->window_attributes.height);
    XFDIMakeRenderable(devicedata->display,Vdrawable);
    z_buffer = XFDICreateZbuffer(devicedata->display,Vdrawable);

    texture_buffer = XdbCreateBuffer(devicedata->display, devicedata->window,
				     &att, mask, NULL);
    XFDIMakeRenderable (devicedata->display, texture_buffer);

    if (devicedata->multi_pass_transp) {
	sort_z_buffer = XdbCreateBuffer(devicedata->display, devicedata->window,
					&att, mask, NULL);
	sort_i_buffer = XdbCreateBuffer(devicedata->display, devicedata->window,
					&att, mask, NULL);
	sort_a_buffer = XdbCreateBuffer(devicedata->display, devicedata->window,
					&att, mask, NULL);

	XFDIMakeRenderable (devicedata->display, sort_z_buffer);
	XFDIMakeRenderable (devicedata->display, sort_i_buffer);
	XFDIMakeRenderable (devicedata->display, sort_a_buffer);
    }

    gc = XFDICreateGc3(devicedata->display,Vdrawable);
    XFDISetRenderable(devicedata->display,gc,Vdrawable);
    XFDISetDrawMode(devicedata->display,gc,XFDIDrawZ);
    XFDISetZRange(devicedata->display,gc,zrange,-zrange);

    XFDISetBuffer(devicedata->display, gc, texture_buffer, XFDIPmapTexUV);

    if (devicedata->multi_pass_transp) {
	XFDISetTransparencyMode(devicedata->display, gc, XFDITransparencyHQ);
	XFDISetBuffer(devicedata->display, gc, sort_z_buffer, XFDIPmapSortZ);
	XFDISetBuffer(devicedata->display, gc, sort_i_buffer, XFDIPmapSortI);
	XFDISetBuffer(devicedata->display, gc, sort_a_buffer, XFDIPmapSortA);
    } else {
	XFDISetTransparencyMode(devicedata->display, gc, XFDITransparencyFast);
    }

    devicedata->xgc = xgc;
    devicedata->gc = gc;
    devicedata->Vdraw_buffer = Vdrawable;
    devicedata->draw_buffer = main_drawable;
    devicedata->z_buffer = z_buffer;
    devicedata->texture_buffer = texture_buffer;
    devicedata->sort_z_buffer = sort_z_buffer;
    devicedata->sort_i_buffer = sort_i_buffer;
    devicedata->sort_a_buffer = sort_a_buffer;

    if (devicedata->stereoswitch) {
	XdbStereoAttributes sattrs;

	main_drawable = XdbCreateBuffer(devicedata->display,devicedata->window,&att,
					mask,NULL);
	Vdrawable = XVPInstance(devicedata->display, main_drawable, 0, 0,
				devicedata->window_attributes.width,
				devicedata->window_attributes.height);
	XFDIMakeRenderable(devicedata->display,Vdrawable);
	if (devicedata->visualtype == DcPseudoColor) {
	    /*
	     * NOTE: with PseudoColor on the gs the z buffer is in the 
	     * top part of the main drawable and associateZBuffer does
	     * not work so allocate a separate z_buffer for stereo with
	     * pseudocolor.
	     */
	    z_buffer = XFDICreateZbuffer(devicedata->display,Vdrawable);
	    devicedata->z_buffer_stereo = z_buffer;
	}
      
	devicedata->Vdraw_buffer_stereo = Vdrawable;
	devicedata->draw_buffer_stereo = main_drawable;

	sattrs.enable = 1;
	XdbChangeStereo(devicedata->display, 
			devicedata->window,
			&sattrs, DBStereoEnable);
    }
    devicedata->window_extent.x = sizehints.x;
    devicedata->window_extent.y = sizehints.y;
    devicedata->window_extent.width = sizehints.width;
    devicedata->window_extent.height = sizehints.height;

    devicedata->image = XCreateImage(devicedata->display,
				     devicedata->window_attributes.visual,
				     devicedata->window_attributes.depth,
				     ZPixmap,0,foo,1,1,32,0);

    XClearWindow(devicedata->display,devicedata->window);

    /*  Setup device specific stuff (if necessary).  */

    /*  Setup error handler and state restorer subroutines in lower level
	device driver (if necessary).  */

    dde_x11_XFDI_current_dcm_data = devicedata;

    dde_x11_XFDI_current_dcm_data->bad_geom = DcTrue;

    ddr_x11_XFDI_dcm_update_geometry();

    return (DcTrue);
}
/*
 ======================================================================
 */
static
create_ramp_colormap(ncolors, colors)
     int ncolors;
     XColor colors[];
{
    DtInt i;
    float base = ncolors - 1.;

    for (i=0; i<ncolors; i++) {
	colors[i].red = i / base * 65535.;
	colors[i].green = i / base * 65535.;
	colors[i].blue = i / base * 65535.;
	colors[i].flags = DoRed|DoGreen|DoBlue;
	colors[i].pixel = i;
    }
}
/*
 ======================================================================
 */

void
ddr_x11_XFDI_dcm_close_device ()
{
    unsigned int color;
    int x, h;
    ddt_x11_XFDI_data *device;
    ddt_x11_XFDI_data *dor_list_get_first_data();
    ddt_x11_XFDI_data *dor_list_get_next_data();

    XFlush(dde_x11_XFDI_current_dcm_data->display);
    XFreeGC(dde_x11_XFDI_current_dcm_data->display,
	    dde_x11_XFDI_current_dcm_data->xgc);
    XFDIDestroyGc3(dde_x11_XFDI_current_dcm_data->display,
		   dde_x11_XFDI_current_dcm_data->gc);

    XdbDestroyBuffer(dde_x11_XFDI_current_dcm_data->display,
		     dde_x11_XFDI_current_dcm_data->draw_buffer);

    XFreePixmap (dde_x11_XFDI_current_dcm_data->display,
		 dde_x11_XFDI_current_dcm_data->Vdraw_buffer);

    XFDIDestroyZbuffer(dde_x11_XFDI_current_dcm_data->display, 
		       dde_x11_XFDI_current_dcm_data->z_buffer);


    XdbDestroyBuffer(dde_x11_XFDI_current_dcm_data->display,
		     dde_x11_XFDI_current_dcm_data->texture_buffer);

    if (dde_x11_XFDI_current_dcm_data->multi_pass_transp) {
	XdbDestroyBuffer(dde_x11_XFDI_current_dcm_data->display,
			 dde_x11_XFDI_current_dcm_data->sort_z_buffer);
	XdbDestroyBuffer(dde_x11_XFDI_current_dcm_data->display,
			 dde_x11_XFDI_current_dcm_data->sort_i_buffer);
	XdbDestroyBuffer(dde_x11_XFDI_current_dcm_data->display,
			 dde_x11_XFDI_current_dcm_data->sort_a_buffer);
    }

    if (dde_x11_XFDI_current_dcm_data->stereoswitch) {
	XdbStereoAttributes sattrs;

	XdbDestroyBuffer(dde_x11_XFDI_current_dcm_data->display,
			 dde_x11_XFDI_current_dcm_data->draw_buffer_stereo);

	XFreePixmap (dde_x11_XFDI_current_dcm_data->display,
		     dde_x11_XFDI_current_dcm_data->Vdraw_buffer_stereo);

	if (dde_x11_XFDI_current_dcm_data->visualtype == DcPseudoColor) {
	    /*
	     * In PseudoColor two z buffers have been allocated because of
	     * a bug that causes AssociateZBuffer to fail for pseudocolor
	     */
	    XFDIDestroyZbuffer(dde_x11_XFDI_current_dcm_data->display, 
			       dde_x11_XFDI_current_dcm_data->z_buffer_stereo);
	}
	sattrs.enable = 0;
	XdbChangeStereo(dde_x11_XFDI_current_dcm_data->display, 
			dde_x11_XFDI_current_dcm_data->window,
			&sattrs, DBStereoEnable);
    }
      

    if (!dde_x11_XFDI_current_dcm_data->user_window) {
	XUnmapWindow(dde_x11_XFDI_current_dcm_data->display,
		     dde_x11_XFDI_current_dcm_data->window);
	XDestroyWindow(dde_x11_XFDI_current_dcm_data->display,
		       dde_x11_XFDI_current_dcm_data->window);
    }

    if (dde_x11_XFDI_dcm_list != (struct list *)NULL) {
	for (device = dor_list_get_first_data(dde_x11_XFDI_dcm_list);
	     device != (ddt_x11_XFDI_data *)NULL;
	     device = dor_list_get_next_data (dde_x11_XFDI_dcm_list)) {
	    if (device == dde_x11_XFDI_current_dcm_data) {
#ifdef DEBUG
		printf ("deleting current node for data 0x%x\n", device);
#endif
		dor_list_remove_current_node(dde_x11_XFDI_dcm_list);
		break;
	    }
	}
    }

    DDspace_Deallocate(dde_x11_XFDI_current_dcm_data);

    dde_x11_XFDI_current_dcm_data = NULL;
}
/*
 ======================================================================
 */

void
ddr_x11_XFDI_dcm_flush ()
{
    XFlush (dde_x11_XFDI_current_dcm_data->display);
}
/*
 ======================================================================
 */

void
ddr_x11_XFDI_dcm_swap_buffers ()
{
    XWindowAttributes atts;
    XdbStereoAttributes sattrs;

    if (dde_x11_XFDI_current_dcm_data->stereoswitch) {
	sattrs.eyestate = 1;	/* left eye */
	XdbChangeStereo(dde_x11_XFDI_current_dcm_data->display, 
			dde_x11_XFDI_current_dcm_data->window,
			&sattrs, DBStereoEyeState);
	XdbUpdateWindows(dde_x11_XFDI_current_dcm_data->display,
			 &dde_x11_XFDI_current_dcm_data->draw_buffer,
			 NULL, 1);
      
	sattrs.eyestate = 2;	/* right eye */
	XdbChangeStereo(dde_x11_XFDI_current_dcm_data->display, 
			dde_x11_XFDI_current_dcm_data->window,
			&sattrs, DBStereoEyeState);
	XdbUpdateWindows(dde_x11_XFDI_current_dcm_data->display,
			 &dde_x11_XFDI_current_dcm_data->draw_buffer_stereo,
			 NULL, 1);
    } else {
	XdbUpdateWindows(dde_x11_XFDI_current_dcm_data->display,
			 &dde_x11_XFDI_current_dcm_data->draw_buffer,
			 NULL, 1);
    }

    XFlush (dde_x11_XFDI_current_dcm_data->display);

    XGetWindowAttributes(dde_x11_XFDI_current_dcm_data->display, 
			 dde_x11_XFDI_current_dcm_data->window,
			 &atts);
    if (dde_x11_XFDI_current_dcm_data->window_extent.x != atts.x ||
	dde_x11_XFDI_current_dcm_data->window_extent.y != atts.y ||
	dde_x11_XFDI_current_dcm_data->window_extent.width != atts.width ||
	dde_x11_XFDI_current_dcm_data->window_extent.height != atts.height) {
#ifdef DEBUG
	printf ("the Geometry of window 0x%x has changed \n",
		dde_x11_XFDI_current_dcm_data->window);
#endif      
	DDdevice_SetAbortedDraw(dde_x11_XFDI_current_dcm_data->device,DcTrue);
	dde_x11_XFDI_current_dcm_data->bad_geom = DcTrue;
    }
}
/*
 ======================================================================
 */

void
ddr_x11_XFDI_dcm_update_geometry ()
{
    Drawable Vdrawable;
    Drawable z_buffer;
    XdbBufferAttributes atts;

    dde_x11_XFDI_current_dcm_data->bad_geom = DcFalse;

    XGetWindowAttributes(dde_x11_XFDI_current_dcm_data->display, 
			 dde_x11_XFDI_current_dcm_data->window,
			 &(dde_x11_XFDI_current_dcm_data->window_attributes));

    if ((dde_x11_XFDI_current_dcm_data->window_attributes.width !=
	 dde_x11_XFDI_current_dcm_data->window_extent.width) ||
	(dde_x11_XFDI_current_dcm_data->window_attributes.height !=
	 dde_x11_XFDI_current_dcm_data->window_extent.height)) {

	atts.resize_mode = XdbUndefinedBits | XdbResizeNow;
	XdbChangeBuffer(dde_x11_XFDI_current_dcm_data->display,
			dde_x11_XFDI_current_dcm_data->draw_buffer,
			&atts, DBResizeMode);
	XdbChangeBuffer(dde_x11_XFDI_current_dcm_data->display,
			dde_x11_XFDI_current_dcm_data->texture_buffer,
			&atts, DBResizeMode);

	if (dde_x11_XFDI_current_dcm_data->multi_pass_transp) {
	    XdbChangeBuffer(dde_x11_XFDI_current_dcm_data->display,
			    dde_x11_XFDI_current_dcm_data->sort_z_buffer,
			    &atts, DBResizeMode);
	    XdbChangeBuffer(dde_x11_XFDI_current_dcm_data->display,
			    dde_x11_XFDI_current_dcm_data->sort_i_buffer,
			    &atts, DBResizeMode);
	    XdbChangeBuffer(dde_x11_XFDI_current_dcm_data->display,
			    dde_x11_XFDI_current_dcm_data->sort_a_buffer,
			    &atts, DBResizeMode);
	}

	XVPChange(dde_x11_XFDI_current_dcm_data->display,
		  dde_x11_XFDI_current_dcm_data->Vdraw_buffer,
		  0, 0, 
		  dde_x11_XFDI_current_dcm_data->window_attributes.width,
		  dde_x11_XFDI_current_dcm_data->window_attributes.height);

	XFDISetRenderable(dde_x11_XFDI_current_dcm_data->display,
			  dde_x11_XFDI_current_dcm_data->gc,
			  dde_x11_XFDI_current_dcm_data->Vdraw_buffer);

	XFDISetBuffer(dde_x11_XFDI_current_dcm_data->display, 
		      dde_x11_XFDI_current_dcm_data->gc, 
		      dde_x11_XFDI_current_dcm_data->texture_buffer, 
		      XFDIPmapTexUV);

	if (dde_x11_XFDI_current_dcm_data->multi_pass_transp) {
	    XFDISetBuffer(dde_x11_XFDI_current_dcm_data->display,
			  dde_x11_XFDI_current_dcm_data->gc, 
			  dde_x11_XFDI_current_dcm_data->sort_z_buffer, 
			  XFDIPmapSortZ);
	    XFDISetBuffer(dde_x11_XFDI_current_dcm_data->display,
			  dde_x11_XFDI_current_dcm_data->gc,
			  dde_x11_XFDI_current_dcm_data->sort_i_buffer, 
			  XFDIPmapSortI);
	    XFDISetBuffer(dde_x11_XFDI_current_dcm_data->display,
			  dde_x11_XFDI_current_dcm_data->gc, 
			  dde_x11_XFDI_current_dcm_data->sort_a_buffer, 
			  XFDIPmapSortA);
	}

	if (dde_x11_XFDI_current_dcm_data->stereoswitch) {
	    XdbChangeBuffer(dde_x11_XFDI_current_dcm_data->display,
			    dde_x11_XFDI_current_dcm_data->draw_buffer_stereo,
			    &atts, DBResizeMode);

	    XVPChange(dde_x11_XFDI_current_dcm_data->display, 
		      dde_x11_XFDI_current_dcm_data->Vdraw_buffer_stereo, 0, 0,
		      dde_x11_XFDI_current_dcm_data->window_attributes.width,
		      dde_x11_XFDI_current_dcm_data->window_attributes.height);
	}
    }

    dde_x11_XFDI_current_dcm_data->window_extent.x =
	    dde_x11_XFDI_current_dcm_data->window_attributes.x;
    dde_x11_XFDI_current_dcm_data->window_extent.y =
	    dde_x11_XFDI_current_dcm_data->window_attributes.y;
    dde_x11_XFDI_current_dcm_data->window_extent.width =
	    dde_x11_XFDI_current_dcm_data->window_attributes.width;
    dde_x11_XFDI_current_dcm_data->window_extent.height =
	    dde_x11_XFDI_current_dcm_data->window_attributes.height;
}
/*
 ======================================================================
 */

void
ddr_x11_XFDI_dcm_become_current_driver (device_data)
     ddt_x11_XFDI_data *device_data;
{
    dde_x11_XFDI_current_dcm_data = device_data;
}
/*
 ======================================================================
 */

void
ddr_x11_XFDI_dcm_setup_shading()
{
    DtInt range_min, range_max;

    dde_x11_XFDI_current_dcm_data->shade_mode = DdInqShadeMode
	    (dde_x11_XFDI_current_dcm_data->device);

    DDdevice_InqShadeRange (dde_x11_XFDI_current_dcm_data->device, 
			 dde_x11_XFDI_current_dcm_data->shade_index,
			 &range_min, &range_max);

    dde_x11_XFDI_current_dcm_data->cwtavgpslim[0] = (DtReal)range_min;
    dde_x11_XFDI_current_dcm_data->cwtavgpslim[1] = (DtReal)range_max;
    dde_x11_XFDI_current_dcm_data->cwtavgpslim[2] = 
	    (dde_x11_XFDI_current_dcm_data->cwtavgpslim[1]-
	     dde_x11_XFDI_current_dcm_data->cwtavgpslim[0]);
}
