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
	ddr_g4vll_return_DCM_fcns
	ddr_g4vll_dcm_create_local_data
	ddr_g4vll_dcm_initialize_device
	ddr_g4vll_dcm_close_device
	ddr_g4vll_dcm_flush
	ddr_g4vll_dcm_swap_buffers
	ddr_g4vll_dcm_update_geometry
	ddr_g4vll_dcm_become_current_driver
	ddr_g4vll_dcm_set_options
	ddr_g4vll_dcm_setup_shading
	ddr_g4vll_dcm_geom_change_handler
	ddr_g4vll_dcm_convert_y
	create_332_colormap
	create_444_colormap
	create_ramp_colormap
	create_ramp_pseudo_colormap
	ddr_g4vll_dcm_check_for_stereo
	ddr_g4vll_dcm_get_board_num
	static void DorePeerActiveCallback
	static Bool XPeerInitialize
	static Bool XPeerEnableWindow
	static void XPeerDisableWindow 

  ======================================================================
*/
#include <string.h>
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/internal/list.h>
#include <dore/dore_develop/develop.h>
#include "dcm.h"

ddt_g4vll_data *dde_g4vll_current_dcm_data;

struct list *dde_g4vll_dcm_list = NULL;

DtInt dde_g4vll_dcm_open_count = 0;

DDt_DCM *dde_g4vll_DCM_fcns = DcNullPtr;

static void create_332_colormap         (VLLcolor colors[]);
static void create_x_332_colormap       (XColor colors[]);
static void create_444_colormap         (int colors[]);
static void create_ramp_pseudo_colormap (VLLcolor colors[]);
static void create_x_ramp_pseudo_colormap (XColor colors[]);
static void create_ramp_colormap        (VLLcolor colors[]);

/* ====================================================================== */

DtPtr ddr_g4vll_return_DCM_fcns (void)
{
    DDt_DCM *fcns;

    if (dde_g4vll_DCM_fcns == (DDt_DCM *)0) {
	fcns = DDdevice_CreateDCMStruct();

	if (fcns == (DDt_DCM *)0) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_g4vll_return_DCM_fcns",
			DcNullPtr);
	}

	fcns->version_number = 2;

	/* Version 1 Functions */

#       ifdef __STDC__
#           define SetFunction(name) fcns->name = ddr_g4vll_dcm_##name
#       else
#           define SetFunction(name) fcns->name = ddr_g4vll_dcm_/**/name
#       endif

	SetFunction (create_local_data);
	SetFunction (initialize_device);
	SetFunction (become_current_driver);
	SetFunction (close_device);
	SetFunction (flush);
	SetFunction (swap_buffers);
	SetFunction (update_geometry);
	SetFunction (inquire_device_extent);
	SetFunction (inquire_stereo);
	SetFunction (inquire_ncolors);
	SetFunction (inquire_resolution);
	SetFunction (inquire_visual_type);
	SetFunction (inquire_auto_size);
	SetFunction (inquire_color_entries);
	SetFunction (set_color_entries);
	SetFunction (set_foreground_color);
	SetFunction (set_background_color);
	SetFunction (set_shade_index);
	SetFunction (inquire_clip_list);
	SetFunction (clear_rectangle_depth);
	SetFunction (clear_rectangle_depth_and_color);
	SetFunction (set_depth_buffer_enable);
	SetFunction (set_depth_buffer_write);
	SetFunction (write_scanline_byte);
	SetFunction (set_current_view);
	SetFunction (inquire_pixel_data);

	/* Version 2 Functions */

	SetFunction (set_options);

	dde_g4vll_DCM_fcns = fcns;
    }
    return (DtPtr)dde_g4vll_DCM_fcns;
}

/* ====================================================================== */

DtPtr ddr_g4vll_dcm_create_local_data (
    DtObject device)
{
    /*
     * This routine is called when a G4 device is created with DoDevice.
     * It allocates memory required by the G4 driver to maintain its
     * local data.  This data is maintained by the G4 driver and is 
     * accessed only by the kernel.
     */
    ddt_g4vll_data *device_data;

    if ((device_data = 
	 (ddt_g4vll_data *)DDspace_Allocate(sizeof (ddt_g4vll_data))) == NULL) {
	DDerror (ERR_CANT_ALLOC_MEM_SIZE, 
		    "ddr_g4vll_dcm_create_local_data",
		    "sizeof (ddt_g4vll_data)");
	return ((DtPtr) DcNullPtr);
    }

    device_data->device = device;

    if (dde_g4vll_dcm_list == NULL) {
#ifdef DEBUG
	printf ("creating g4vll_dcm_list\n");
#endif
	dde_g4vll_dcm_list = (struct list *)dor_list_create();

	if (dde_g4vll_dcm_list == NULL) {
	    DDerror (ERR_CANT_ALLOC_MEM_SIZE, 
			"ddr_g4vll_dcm_create_local_data",
			"dor_list_create");

	    return ((DtPtr) device_data);
	}
    }

#ifdef DEBUG
    printf ("adding 0x%lx device to list\n", device_data);
#endif
    dor_list_add_data_last(dde_g4vll_dcm_list, device_data);

    return ((DtPtr) device_data);
}

/* ====================================================================== */

DtFlag ddr_g4vll_dcm_initialize_device (
    DtObject device,
    DtPtr devicedata_parm,
    DtPtr argstring,
    DtPtr type)
{
#   define devicedata ((ddt_g4vll_data*)(devicedata_parm))

    VLLinfo 		*features;
    char		*argstringcopy;
    char		*tmp;
    char		*p;
    char		*nextarg;
    static char 	*funcname  = "ddr_g4vll_dcm_initialize_device";
    int			error=0;
    XClassHint		class_hints;
    XVisualInfo		*visual_info, vinfo_template;
    int			nvisuals;
    XSetWindowAttributes window_attributes;
    XWindowAttributes 	user_win_attributes;
    int			attrMask;
    int			screen;
    char		*display_name;
    XEvent		event;
    XSizeHints	 	xsh;
    static VLLcolor	colors[4096];
    static int		transl_table_colors[4096];
    static XColor	xcolors[4096];
    Colormap		cmap;
    VLLcolormap 	*vll_cmap;
    VLLtranslation_table *transl_table;
    VLLBoxRec    	vll_rect;
    int			vll_major, vll_minor;
    DtInt		nplanes;
    DtFlag              offscreen_drawing = DcFalse;
    DtInt               border_width;
    Display		*dpy;
    Window 		root;
    static XWMHints 	xwmh = {
	(InputHint|StateHint),	/* flags */
	True,			/* input */
	NormalState,		/* initial_state */
	0,			/* icon pixmap */
	0,			/* icon window */
	0, 0,			/* icon location */
	0,			/* icon mask */
	0,			/* Window group */
    };

    /* default values for those that might be found in the argstring */

    screen = -1;
    devicedata->device = device;

    devicedata->x = 0;
    devicedata->y = 0;
    devicedata->width = 512;
    devicedata->height = 512;
    devicedata->double_buffered = DcTrue;
    devicedata->use_spheres = DcFalse;
    devicedata->cylinders = ddd_g4vll_cylinders_tesselated;
    devicedata->use_ztc = DcTrue;
    devicedata->use_hw_boundingvol = DcFalse;
    devicedata->transp_mode = ddd_g4vll_transp_two_pass_add;
    devicedata->auto_size = DcTrue;
    devicedata->visualtype = DcTrueColor;
    devicedata->ncolors = 256;
    devicedata->stereoswitch = DcFalse;
    devicedata->stereoscreen = ddd_g4vll_mono_screen;
    devicedata->head = ddd_g4vll_head_zero;
    devicedata->vll_state = NULL;
    devicedata->user_window = DcFalse;
    devicedata->user_display = DcFalse;
    devicedata->nrects = 0;
    devicedata->window_id = 0;
    devicedata->sequence_number = 0;
    devicedata->copy_back_buffer = DcTrue;

    if ((type != (DtPtr)0) &&
	(0 == strcmp(type, "g4vll") ||
	 0 == strcmp(type, "g4"))) {
	devicedata->use_X = DcFalse;
    } else {
	devicedata->use_X = DcTrue;
    }
      
    /*
     * set defaults for Pseudo Color
     */

    devicedata->shade_index = 0;

    devicedata->cbtselpsweight[0] = 255.;
    devicedata->cbtselpsweight[1] =  31.;
    devicedata->cbtselpsweight[2] =   3.;
	
    devicedata->cbtselpsmask[0] = 0xe0;
    devicedata->cbtselpsmask[1] = 0x1c;
    devicedata->cbtselpsmask[2] = 0x03;
	
    devicedata->cwtavgpsweight[0] = .30;
    devicedata->cwtavgpsweight[1] = .59;
    devicedata->cwtavgpsweight[2] = .11;
	
    /*
     * set defaults for other devicedata fields
     */

    devicedata->vll_window       	= NULL;
    devicedata->vll_window_status	= 0;
    devicedata->vll_cmap         	= NULL;
    devicedata->transl_table     	= NULL;

    devicedata->vll_pixmap.pixmap      	= NULL;
    devicedata->vll_pixmap.width   	= 0;
    devicedata->vll_pixmap.height   	= 0;

    devicedata->vll_accumulator.pixmap  = NULL;
    devicedata->vll_accumulator.width   = 0;
    devicedata->vll_accumulator.height  = 0;

    /* Process the argument string. */

    argstringcopy = DDspace_Allocate (strlen(argstring) + 1);
    strcpy (argstringcopy, argstring); /* so as not to alter user's string */
    tmp = argstringcopy;

    /* p points to first sub-string */

    while ((p = DDstrtok(tmp, " \t,"))) {
	tmp = 0;

	if (0 == strcmp(p,"-multipass_transparency")) {
	    devicedata->transp_mode = ddd_g4vll_transp_multi_pass;

	} else if (0 == strcmp(p, "-sort_transparency")) {
	    devicedata->transp_mode = ddd_g4vll_transp_single_pass;

	} else if (0 == strcmp(p, "-two_pass_compos_transparency")) {
	    devicedata->transp_mode = ddd_g4vll_transp_two_pass_compos;

	} else if (0 == strcmp(p, "-nospheres")) {
	    devicedata->use_spheres = DcFalse;

	} else if (0 == strcmp(p, "-nocylinders")) {
	    devicedata->cylinders = ddd_g4vll_cylinders_tesselated;

	} else if (0 == strcmp(p, "-spheres")) {
	    devicedata->use_spheres = DcTrue;

	} else if (0 == strcmp(p, "-cylinders")) {
	    devicedata->cylinders = ddd_g4vll_cylinders_firmware_capped;

	} else if (0 == strcmp(p, "-cylindersnocap")) {
	    devicedata->cylinders = ddd_g4vll_cylinders_firmware_uncapped;

	} else if (0 == strcmp(p, "-ztc")) {
	    devicedata->use_ztc = DcTrue;

	} else if (0 == strcmp(p, "-noztc")) {
	    devicedata->use_ztc = DcFalse;

	} else if (0 == strcmp(p, "-boundingvol")) {
	    devicedata->use_hw_boundingvol = DcTrue;

	} else if (0 == strcmp(p, "-noboundingvol")) {
	    devicedata->use_hw_boundingvol = DcFalse;

	} else if (0 == strcmp(p,"-noautosize")) {
	    devicedata->auto_size = DcFalse;

	} else if (0 == strcmp(p,"-singlebuffered")) {
	    devicedata->double_buffered = DcFalse;

	} else if (0 == strcmp(p,"-stereo")) {
	    devicedata->stereoswitch = DcTrue;

	} else if (0 == strcmp(p, "-windowid")) {
	    if (!(nextarg = DDstrtok(0, " \t,"))){
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "windowid: (no value)");
		error = 1;
	    } else {
		long w;

		sscanf (nextarg, "%ld", &w);
		devicedata->window_id = w;
	    }

	} else if (0 == strcmp(p,"-geometry")){
	    if (!(nextarg = DDstrtok(0, " \t,"))){
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "geometry: (no value)");
		error = 1;
	    } else {
		XParseGeometry(nextarg,
			       &devicedata->x,&devicedata->y,
			       &devicedata->width, &devicedata->height);
	    }

	} else if (0 == strcmp(p,"-ncolors")){
	    if (!(nextarg = DDstrtok(0, " \t,"))){
		DDerror (ERR_BAD_DEVICE_OPT_VAL,funcname,"ncolors: (no value)");
		error = 1;
	    } else {
		auto int w;
		sscanf (nextarg, "%d", &w);
		devicedata->ncolors = w;
	    }
	     
	} else if (0 == strcmp(p, "-visualtype")) {
	    if (!(nextarg = DDstrtok(0, " \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "visualtype: (no value)");

		error = 1;
	    } else	{
		if (0 == strcmp("DcPseudoColor",nextarg)) {
		    devicedata->visualtype = DcPseudoColor;
		} else if ( ! strcmp("DcTrueColor", nextarg)) {
		    devicedata->visualtype = DcTrueColor;
		} else if ( ! strcmp("DcDirectColor", nextarg)) {
		    devicedata->visualtype = DcDirectColor;
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

	} else if (0 == strcmp(p, "-noX")) {
	    devicedata->use_X = DcFalse;

	} else if (0 == strcmp(p, "-X")) {
	    devicedata->use_X = DcTrue;
	 
	} else if (0 == strcmp(p,"-window")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "window: (no value)");
		error = 1;
	    } else	{
		auto long w;
		devicedata->user_window = DcTrue;
		sscanf (nextarg, "%ld", &w);
		devicedata->window = (Window)(w);
	    }
	} else if (0 == strcmp (p, "-display")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL, funcname,"device: (no value)");
		error = 1;
	    }else{
		auto long dptr;
		devicedata->user_display = DcTrue;
		sscanf (nextarg, "%ld", &dptr);
		devicedata->display = (Display *)(dptr);
	    }

	} else if (!strcmp(p,"-screen")) {
            if(!(nextarg = DDstrtok(0," \t,"))) {
                DDerror(ERR_BAD_DEVICE_OPT_VAL,funcname, "screen: (no value)");
                error = 1;
            }
            else {
                sscanf (nextarg, "%ld", &screen);
            }

	} else if (0 == strcmp(p,"-head")) {   /* use this option for no X */
	    if (!(nextarg = DDstrtok(0, " \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "head: (no value)");
		error = 1;
	    } else {
		long h;

		sscanf (nextarg, "%ld", &h);
		if ((h == ddd_g4vll_head_zero) || (h == ddd_g4vll_head_one)) {
		    devicedata->head = h;
		} else {
		    DDerror (ERR_BAD_DEVICE_OPT_VAL, funcname, 
			"head: (bad value); defaulting to single-headed mode");
		}
	    }

	} else if (0 == strcmp (p,"-offscreen")) {
	    offscreen_drawing = DcTrue;
	} else {
	    sprintf(DDerror_GetLine(), "flag '%s'", p);
	    DDerror (ERR_BAD_DEVICE_OPT,
			funcname, DDerror_GetLine());
	}
    }

    DDspace_Deallocate(argstringcopy);

    if ((devicedata->visualtype == DcPseudoColor &&
	 (devicedata->ncolors < 256 || devicedata->ncolors > 4096)) ||
	(devicedata->visualtype != DcPseudoColor && devicedata->ncolors != 256)) {
	sprintf (DDerror_GetLine(), "invalid 'ncolors' %d", devicedata->ncolors);
	DDerror(ERR_BAD_DEVICE_OPT_VAL, funcname,
		   DDerror_GetLine());
	error++;
    }

    if (error) 
	    return(DcFalse);

    if (devicedata->use_X) {
    	/*
     	 * If the display is NOT provided then use the DISPLAY environment 
	 * variable and its default screen.  Then, to establish a valid X 
	 * display/screen/window combination:
     	 *     if the window and screen are user-provided then
     	 *         ensure that the window's screen number matches the 
	 *		provided screen number; match'em up if they don't
     	 *     if the window is user-provided then
     	 *         get the screen number from the window and use it
     	 *     else if both the display and screen are provided then
     	 *         use them (to create the window)
     	 *     else if display only is provided then
     	 *         use it and its default screen (to create the window)
     	 */
	if (!devicedata->user_display) {
	    if ((display_name = DDgetenv("DISPLAY")) == NULL) {
		DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			    funcname, "Display Environment variable not set");
		DDspace_Deallocate(devicedata);
		return DcFalse;
	    }
	
	    if ((devicedata->display = XOpenDisplay(display_name)) == NULL) {
		sprintf(DDerror_GetLine(),
			"Could not open display %s",display_name);
		DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			    funcname, DDerror_GetLine());
		DDspace_Deallocate(devicedata);
		return DcFalse;
	    }
	}
		
	dpy = devicedata->display;

        /* The user didn't supply a window so Dore needs to. */

	if (!devicedata->user_window) {
	    if (screen != -1) {
		vinfo_template.screen = screen;
	    } else {
	    	screen = vinfo_template.screen = XDefaultScreen(dpy);
	    }

	    switch (devicedata->visualtype) {

		case DcPseudoColor:
		    if (devicedata->ncolors > 256) {
			vinfo_template.class = TrueColor;
			vinfo_template.depth = nplanes = 24;
		    } else {
			vinfo_template.class = PseudoColor;
			vinfo_template.depth = nplanes = 8;
		    }
		    break;

		case DcDirectColor:
		    vinfo_template.class = DirectColor;
		    vinfo_template.depth = nplanes = 24;
		    break;

		case DcTrueColor:
		    vinfo_template.depth = nplanes = 24;
		    vinfo_template.class = TrueColor;
		    break;

		case DcStaticColor:
		case DcGreyScale:
		case DcStaticGrey:
		    break;
	    }
	    /*add an XSync for bug in Xlib. See TRs Dore-0657 and 0707 */
	    XSync(dpy, False);
	    visual_info = XGetVisualInfo(dpy,
				VisualScreenMask|VisualClassMask|VisualDepthMask,
				&vinfo_template, &nvisuals);

	    if (nvisuals == 0) {
		DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			    funcname, 
			    "screen doesn't support specified visual type");
		DDspace_Deallocate(devicedata);
		return DcFalse;
	    }

	    attrMask = CWBackPixel|CWBorderPixel;

	    if (offscreen_drawing) {
		/* offscreen drawing not implemented yet -- for now
		  we know that radiosity is opening this window, so
		  prevent the window manager to get a hold of it.
		  Otherwise if the user specified a window width smaller
		  than the smallest size the window manager needs to put
		  all of its widgets, radiosity will be confused */

		attrMask |= CWOverrideRedirect;
		window_attributes.override_redirect = True;
		/* set very small border width */
		border_width = 2;
	    } else {
		/* let window manager set border width */
		border_width = 0;
	    }

	    switch (devicedata->visualtype) {
		case DcPseudoColor:
		    if (devicedata->ncolors == 256) {
			cmap = XCreateColormap
			       (   dpy,
				   RootWindow(dpy, screen),
				   visual_info->visual, AllocAll
			       );
			create_x_332_colormap(xcolors);
			XStoreColors(dpy, cmap, xcolors, 256);
			attrMask |= CWColormap;
			window_attributes.colormap = cmap;
			break;
		    }
		    /* NOTE: if ncolors > 256 then we are doing 12 bit pseudo color
		       but using a TrueColor visual so fall through to the TrueColor 
		       case */
		case DcTrueColor:
		    if (visual_info->visual == 
			    DefaultVisual(dpy, screen)) {
			cmap = DefaultColormap(dpy,screen);
		    } else {
			cmap = XCreateColormap(dpy,
				  	       RootWindow(dpy, screen),
					       visual_info->visual,AllocNone);
		    }
		    attrMask |= CWColormap;
		    window_attributes.colormap = cmap;
		    break;
		case DcDirectColor:
		    if (visual_info->visual == 
			    DefaultVisual(dpy, screen)) {
			cmap = DefaultColormap(dpy,screen);
		    } else {
			cmap = XCreateColormap(dpy,
				   	       RootWindow(dpy, screen),
					       visual_info->visual,AllocAll);
			create_x_ramp_pseudo_colormap(xcolors);
			XStoreColors(dpy, cmap, xcolors, 256);
		    }
		    attrMask |= CWColormap;
		    window_attributes.colormap = cmap;
		    break;

		case DcStaticGrey:
		case DcGreyScale:
		case DcStaticColor:
		    break;
	    }

	    window_attributes.background_pixel = 0x000000;
	    window_attributes.border_pixel = 0xffffff;

	    devicedata->window = XCreateWindow(dpy,
				RootWindow(dpy, screen),
				devicedata->x, devicedata->y,
				devicedata->width, devicedata->height,
				border_width,
				nplanes,InputOutput,visual_info->visual,
				attrMask,&window_attributes);
#ifdef DEBUG_STEREO
                    printf ("Dore created window ...\n");
#endif
	 
	    if (!devicedata->window) {
		DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			    funcname, "Could not create X11 window");

		DDspace_Deallocate(devicedata);

		if (!devicedata->user_display) {
		    XCloseDisplay(dpy);
		}
		return DcFalse;
	    }

	    /*  Set the standard properties for the window managers.  */
      
	    XStoreName(devicedata->display,devicedata->window,"Dore Window");
	    XSetIconName(dpy,devicedata->window,"Dore Window");
	    XSetWMHints(dpy,devicedata->window,&xwmh);

	    xsh.flags = (USPosition|USSize);
	    xsh.width = devicedata->width;
	    xsh.height = devicedata->height;
	    xsh.x = devicedata->x;
	    xsh.y = devicedata->y;
	    XSetSizeHints(dpy,devicedata->window,&xsh,
			  XA_WM_NORMAL_HINTS);
	    class_hints.res_name = "dore";
	    class_hints.res_class = "Dore";
	    XSetClassHint(dpy,devicedata->window,&class_hints);

	    XSelectInput(dpy,devicedata->window,
		ExposureMask | StructureNotifyMask);
	    XMapWindow(dpy,devicedata->window);
	    while (XNextEvent(dpy, &event)) {
		if (event.type == Expose)  break;
	    }
	} else {
	    /*
	     * Grab the screen number for the user-supplied window (if the
	     * screen is also provided then ensure that the screen/window 
	     * combo is valid).
	     */
	    XGetWindowAttributes(dpy, devicedata->window, &user_win_attributes);
	    if (screen != -1) {
	    	root = RootWindow(dpy, screen);
		if (user_win_attributes.root != root) {
		    DDerror (ERR_DEVICE_SYSTEM_ACCESS, funcname, 
			"screens don't match: using screen of provided window");
		    screen = XScreenNumberOfScreen(user_win_attributes.screen);
		}
	    } else {
	    	screen = XScreenNumberOfScreen(user_win_attributes.screen);
	    }
	}

        /*
	 * Determine whether the screen is stereo or mono.  The stereo switch 
	 * will be turned off if stereo is requested but is not available.
         */
	if (!(ddr_g4vll_dcm_check_for_stereo (dpy, screen,
		&(devicedata->stereoscreen)))) {
	    if (devicedata->stereoswitch) {
                DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, "stereo not available on this device");
		devicedata->stereoswitch = DcFalse;
	    }
	}

	devicedata->gc = XCreateGC(dpy, devicedata->window, 0, 0);
	if (!devicedata->gc) {
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, "Could not create graphics context");

	    if (!devicedata->user_window) {
		XUnmapWindow(dpy, devicedata->window);
		XDestroyWindow(dpy, devicedata->window);
	    }
	    if (!devicedata->user_display) {
		XCloseDisplay(dpy);
	    }
	    DDspace_Deallocate(devicedata);
	    return DcFalse;
	}

	ddr_g4vll_dcm_get_board_num(dpy, screen, &(devicedata->head));
	if (!VLLinit (0, (unsigned int)(VLL_X11_PEER | 
		(VLL_DENALI_BOARD(devicedata->head))),
		ddr_g4vll_dcm_geom_change_handler)) {
	    DDerror(ERR_DEVICE_SYSTEM_ACCESS, funcname,
		        "device initialisation failed");
	    return (DcFalse);
	} else {
	    /*
	     * Store this device's (head's) VLL state handle.
	     */
	    devicedata->vll_state = VLLquery_state();
            dde_g4vll_dcm_open_count++;
	}

	if (sizeof(float) != sizeof(DtReal)) {
	        VLLset_data_precision(VLL_DATA_DOUBLE);
	} else {
	        VLLset_data_precision(VLL_DATA_SINGLE);
	}

#	ifdef titan
	    if (DDgetenv("DORE_VLL_NO_FORCE") == NULL) {
		sysmips(SFORCE_NORUN,sysmips(SBOOT_CPU));
	    }
#	endif

	if (!XPeerInitialize(dpy, screen)) {
            DDerror(ERR_DEVICE_SYSTEM_ACCESS, funcname,
			"Peer extension not supported");
	    return DcFalse;
	}
	if (!XPeerEnableWindow(dpy, devicedata->window,
	    (unsigned)((devicedata->double_buffered ? VLL_WINDOW_DBL_BUF : 0) |
	    VLL_WINDOW_ZBUF | (devicedata->use_ztc ? VLL_WINDOW_ZTC_EN  : 0)),
	    &(devicedata->vll_window))) {
		
	    DDerror(ERR_DEVICE_SYSTEM_ACCESS, funcname,
		       "Request for peer window failed");
	    if (!devicedata->user_display) {
		XCloseDisplay(dpy);
	    }
	    DDspace_Deallocate(devicedata);
	    return DcFalse;
	}

    } else /* run without X */ {

	if (!VLLinit (0, (unsigned int)(VLL_DENALI_BOARD(devicedata->head)),
		      ddr_g4vll_dcm_geom_change_handler)) {
	    DDerror(ERR_DEVICE_SYSTEM_ACCESS, funcname,
		       "device initialisation failed");
	    return (DcFalse);
	} else {
	    /*
	     * Store this device's (head's) VLL state handle.
	     */
	    devicedata->vll_state = VLLquery_state();
            dde_g4vll_dcm_open_count++;
	}

	if (sizeof(float) != sizeof(DtReal)) {
	    VLLset_data_precision(VLL_DATA_DOUBLE);
	} else {
	    VLLset_data_precision(VLL_DATA_SINGLE);
	}

#	ifdef titan
	    if (DDgetenv("DORE_VLL_NO_FORCE") == NULL) {
		        sysmips (SFORCE_NORUN,sysmips(SBOOT_CPU));
	    }
#	endif
    }

    dde_g4vll_current_dcm_data = devicedata;

    dde_g4vll_current_dcm_data->bad_geom = DcTrue;

    /*
     * Abort if the link-time and run-time VLL major numbers don't match, and
     * also if the run-time minor is greater than the link-time minor number.
     */
    VLLquery_version(&vll_major, &vll_minor);
    if (vll_major != VLL_API_VERSION_MAJOR) {
	DDerror (ERR_DEVICE_SYSTEM_ACCESS,
		funcname, "VLL major numbers don't match");
	DDspace_Deallocate(devicedata);
	return DcFalse;
    }
    if (vll_minor > VLL_API_VERSION_MINOR) {
	DDerror (ERR_DEVICE_SYSTEM_ACCESS,
		funcname, "Link-time minor number > Run-time minor number");
	DDspace_Deallocate(devicedata);
	return DcFalse;
    }
    
    features = VLLquery_features();

    if (devicedata->stereoscreen != ddd_g4vll_mono_screen) {
	if (features->stereo) {
	    VLLset_screen_type (VLL_STEREO_SCREEN);
	    VLLset_stereo_screen_type(devicedata->stereoscreen);
#ifdef DEBUG_STEREO
            printf ("set VLL screen type to STEREO\n");
#endif
	} else {
	    devicedata->stereoswitch = DcFalse;
#ifdef DEBUG_STEREO
            printf ("No stereo support, so turn stereoswitch off \n");
#endif
	}
    }

    /*
     * VLL computes all lighting and shading in true colour, but the 
     * true colour value can be converted in two different ways to
     * pseudocolour:  via ranges or colour spaces.
     */
    switch(devicedata->visualtype) {
    case DcPseudoColor: 
	{ 
	    int range[2];

	    range[0] = 0;
	    range[1] = (devicedata->ncolors - 1);

	    if (devicedata->ncolors > 256) {
	    	VLLset_pseudo_range(devicedata->cwtavgpsweight, 
				range, VLL_PSEUDO_TYPE_12);
	    	VLLset_pseudo_space(devicedata->cbtselpsweight, 
				devicedata->cbtselpsmask, VLL_PSEUDO_TYPE_12);
	    } else {
	    	VLLset_pseudo_range(devicedata->cwtavgpsweight, 
				range, VLL_PSEUDO_TYPE_RAMDAC);
	    	VLLset_pseudo_space(devicedata->cbtselpsweight, 
				devicedata->cbtselpsmask, VLL_PSEUDO_TYPE_RAMDAC);
	    }
	    VLLset_output_color_model(VLL_PSEUDO_SPACE);

        } break;

    case DcTrueColor:
    case DcDirectColor:
	VLLset_output_color_model(VLL_TRUE_COLOR);
	break;

    case DcStaticGrey:
    case DcGreyScale:
    case DcStaticColor:
	break;
    }

    switch (devicedata->transp_mode) {
    case ddd_g4vll_transp_multi_pass:
	if (features->multi_pass_transp) {
	    VLLset_transparency_mode(VLL_MULTI_PASS);
	    break;
	}
    case ddd_g4vll_transp_single_pass:
	if (features->single_pass_transp) {
	    VLLset_transparency_mode(VLL_SINGLE_PASS);
	    break;
	}
    case ddd_g4vll_transp_two_pass_compos:
	if (features->two_pass_transp) {
	    VLLset_transparency_mode(VLL_TWO_PASS_COMPOS);
	    break;
	}
    default:
	if (features->two_pass_transp) {
	    devicedata->transp_mode = ddd_g4vll_transp_two_pass_add;
	    VLLset_transparency_mode(VLL_TWO_PASS_ADD);
	}
	break;
    }

    if (!devicedata->use_X) {
	if (devicedata->visualtype == DcPseudoColor) {
	    if (devicedata->ncolors > 256) {
		/* 
		 * If > 8-bit pseudocolor, need to set up both the translation 
		 * table (for conversion on write to FB) and the true color map.
		 * The translation table accommodates a 16-to-24 bit conversion,
		 * but a 12-to-24 conversion should suffice for most data ...
		 */
		create_444_colormap(transl_table_colors);
		transl_table = VLLcreate_translation_table(4096);
		VLLstore_translation_table (transl_table, transl_table_colors, 
			0, 4096);
		VLLinstall_translation_table (transl_table, 0);
		devicedata->transl_table = transl_table;
	      
		/*
		 * Create and install a ramp colormap for the translation table
		 * lookup.
		 */
		create_ramp_pseudo_colormap(colors);
	    	vll_cmap = VLLcreate_colormap(VLL_COLORMAP_PSEUDO_8, 256);
	    	VLLset_colormap(vll_cmap, colors, 0, 256);
	    	VLLinstall_colormap(vll_cmap, VLL_LUT_1);
		devicedata->vll_cmap = vll_cmap;
	    } else {
		/*
		 * On Denali, 8-bit pseudocolor values are replicated and stuffed
		 * down all three channels.  The RAMDAC colourmap is indexed.
		 */
		create_ramp_pseudo_colormap(colors);
		vll_cmap = VLLcreate_colormap(VLL_COLORMAP_PSEUDO_8, 256);
		VLLset_colormap(vll_cmap, colors, 0, 256);
	    	VLLinstall_colormap(vll_cmap, VLL_LUT_1);
		devicedata->vll_cmap = vll_cmap;
	    }
	} else {
	    create_ramp_colormap(colors);
	    vll_cmap = VLLcreate_colormap(VLL_COLORMAP_RGB, 256);
	    VLLset_colormap(vll_cmap, colors, 0, 256);
	    VLLinstall_colormap(vll_cmap, VLL_LUT_1);
	    devicedata->vll_cmap = vll_cmap;
	}

	vll_rect.x1 = devicedata->x;
	vll_rect.y1 = devicedata->y;
	vll_rect.x2 = devicedata->x + devicedata->width;
	vll_rect.y2 = devicedata->y + devicedata->height;

	/* Create window with an associated Z buffer; use double buffering
	** and ZTC unless told otherwise by user.  */

	devicedata->vll_window = VLLcreate_window((unsigned)(
		(devicedata->double_buffered ? VLL_WINDOW_DBL_BUF : 0) |
		VLL_WINDOW_ZBUF | 
		(devicedata->use_ztc 	     ? VLL_WINDOW_ZTC_EN  : 0)),
		devicedata->x, devicedata->y, devicedata->width, 
		devicedata->height,  devicedata->window_id,
		VLL_LUT_1, 1, &vll_rect);
    }

    /* 
     * Set the window, etc...
     */
    VLLset_window(devicedata->vll_window);
    ddr_g4vll_dcm_update_geometry();

    return (DcTrue);

#undef devicedata
}

/* ====================================================================== */

static void create_332_colormap (VLLcolor colors[])
{
    DtInt i;

    for (i=0; i<256; i++) {
	colors[i].red = (i>>5) / 7. * 65535.;
	colors[i].green = ((i>>2) & 0x07) / 7. * 65535.;
	colors[i].blue = (i & 0x03) / 3. * 65535.;
	colors[i].flags = DoRed|DoGreen|DoBlue;
	colors[i].pixel = i;
    }
}

/* ====================================================================== */

static void create_x_332_colormap (XColor colors[])
{
    DtInt i;

    for (i=0; i<256; i++) {
	colors[i].red = (i>>5) / 7. * 65535.;
	colors[i].green = ((i>>2) & 0x07) / 7. * 65535.;
	colors[i].blue = (i & 0x03) / 3. * 65535.;
	colors[i].flags = DoRed|DoGreen|DoBlue;
	colors[i].pixel = i;
    }
}

/* ====================================================================== */

static void create_444_colormap (int colors[])
{
    DtInt i;
    int red, green, blue;

    for (i=0; i<4096; i++) {
	red = (i>>8) / 15. * 255.;
	green = ((i>>4) & 0x0f) / 15. * 255.;
	blue = (i & 0x0f) / 15. * 255.;

	colors[i] = (blue << 16) | (green << 8) | red;
    }
}

/* ====================================================================== */

static void create_ramp_pseudo_colormap (VLLcolor colors[])
{
    DtInt i;

    for (i=0; i<256; i++) {
	colors[i].red = colors[i].green = colors[i].blue =  (i / 255.) * 65535.;
	colors[i].flags = DoRed|DoGreen|DoBlue;
	colors[i].pixel = i;
    }
}

/* ====================================================================== */

static void create_x_ramp_pseudo_colormap (XColor colors[])
{
    DtInt i;

    for (i=0; i<256; i++) {
	colors[i].red = colors[i].green = colors[i].blue =  (i / 255.) * 65535.;
	colors[i].flags = DoRed|DoGreen|DoBlue;
	colors[i].pixel = i;
    }
}

/* ====================================================================== */

static void create_ramp_colormap (VLLcolor colors[])
{
    DtInt i;

    for (i=0; i<256; i++) {
	colors[i].red = colors[i].green = colors[i].blue =  (i / 255.) * 65535.;
	colors[i].flags = DoRed|DoGreen|DoBlue;
	colors[i].pixel = i << 16 | i << 8 | i ;
    }
}

/* ====================================================================== */

void ddr_g4vll_dcm_close_device (void)
{
    ddt_g4vll_data *device;
    ddt_g4vll_dcm_offscreen_resource *offscreen_resource;

    if (dde_g4vll_dcm_list != (struct list *)NULL) {
	for (device = dor_list_get_first_data(dde_g4vll_dcm_list);
	     device != (ddt_g4vll_data *)NULL;
	     device = dor_list_get_next_data (dde_g4vll_dcm_list)) {
	    if (device == dde_g4vll_current_dcm_data) {
#ifdef DEBUG
		printf ("deleting current node for data 0x%lx\n", device);
#endif
		dor_list_remove_current_node(dde_g4vll_dcm_list);
		break;
	    }
	}
    }

    if (dde_g4vll_current_dcm_data->use_X) {
	XPeerDisableWindow(dde_g4vll_current_dcm_data->display,
			 dde_g4vll_current_dcm_data->window);
	if (!dde_g4vll_current_dcm_data->user_window) {
	    XUnmapWindow(dde_g4vll_current_dcm_data->display, 
			 dde_g4vll_current_dcm_data->window);
	    XDestroyWindow(dde_g4vll_current_dcm_data->display, 
			   dde_g4vll_current_dcm_data->window);
	}
	if (!dde_g4vll_current_dcm_data->user_display) {
	    XCloseDisplay(dde_g4vll_current_dcm_data->display);
	}
    } else {
    	VLLdestroy_window(dde_g4vll_current_dcm_data->vll_window);

	offscreen_resource = &(dde_g4vll_current_dcm_data->vll_pixmap);
	if (offscreen_resource->pixmap != NULL) {
	    VLLdestroy_pixmap(offscreen_resource->pixmap);
	}
	offscreen_resource = &(dde_g4vll_current_dcm_data->vll_accumulator);
	if (offscreen_resource->pixmap != NULL) {
	    VLLdestroy_pixmap(offscreen_resource->pixmap);
	}
    }

    /*
     * If this device's screen was stereo, reset VLL's screentype to mono before 
     * closing the device.
     */
    if (dde_g4vll_current_dcm_data->stereoscreen != ddd_g4vll_mono_screen) {
	VLLset_screen_type(VLL_MONO_SCREEN);
    }

    /*
     * We won't encounter the MP problem found in the G2 and G3
     * drivers as all the MP is in the Denali's multiple TE's.
     * So, we can exit cleanly here.
     */
    if (!(--dde_g4vll_dcm_open_count)) {
	VLLterminate();
    }

    DDspace_Deallocate(dde_g4vll_current_dcm_data);

    dde_g4vll_current_dcm_data = NULL;
}

/* ====================================================================== */

void ddr_g4vll_dcm_flush (void)
{
}

/* ====================================================================== */

void ddr_g4vll_dcm_swap_buffers (void)
{
    VLLflush ();

    if (dde_g4vll_current_dcm_data->bad_geom)
	return;

    if (dde_g4vll_current_dcm_data->double_buffered) {
    	VLLswap_window ();
	dde_g4vll_current_dcm_data->copy_back_buffer = DcTrue;
    }
}

/* ====================================================================== */

void ddr_g4vll_dcm_update_geometry (void)
{
    ddt_g4vll_data *data;

    data = dde_g4vll_current_dcm_data;

    if (data->use_X) {
	if (data->bad_geom) {
	    VLLset_window(data->vll_window);
	    VLLquery_window_size(&(data->width), &(data->height));
	}

	if (data->double_buffered) {
	    VLLset_output_buffer (VLL_NONDISPLAY_BUFFER);
	} else {
	    VLLset_output_buffer (VLL_DISPLAY_BUFFER);
	}

	data->clr_device = DcTrue;
    } else {
	if (data->bad_geom) {
	    VLLBoxRec rect;

	    rect.x1 = data->x;
	    rect.y1 = data->y;
	    rect.x2 = data->x + data->width;
	    rect.y2 = data->y + data->height;

	    if (data->double_buffered) {
	    	VLLset_output_buffer (VLL_NONDISPLAY_BUFFER);
	    } else {
	    	VLLset_output_buffer (VLL_DISPLAY_BUFFER);
	    }

	    /*
	     * Set up the clip id to be used for the clip rectangle.
	     */
	    VLLset_ztc_enable(VLL_DISABLE);
	    VLLset_control_planes ((int)data->window_id, 
			VLL_CLEAR_ZTC, 1, &rect);
	    if (data->use_ztc) {
	    	VLLset_ztc_enable(VLL_ENABLE);
	    }

	    data->clr_device = DcTrue;
	}
    }
    data->bad_geom = DcFalse;
}

/* ====================================================================== */

void ddr_g4vll_dcm_become_current_driver (DtPtr device_data)
{
    dde_g4vll_current_dcm_data = (ddt_g4vll_data*)(device_data);

    if (dde_g4vll_current_dcm_data->stereoscreen == ddd_g4vll_mono_screen) {
	VLLset_screen_type (VLL_MONO_SCREEN);
    } else {
	VLLset_screen_type (VLL_STEREO_SCREEN);
	VLLset_stereo_screen_type(dde_g4vll_current_dcm_data->stereoscreen);
    }
}

/* ====================================================================== */

void ddr_g4vll_dcm_set_options (
    DtPtr argstring)
{
    VLLinfo 	*features;
    char		*argstringcopy;
    char		*tmp;
    char		*p;
    ddt_g4vll_data *devicedata;
    int		transp_changed = -1;
    int		ztc_changed = -1;


    devicedata = dde_g4vll_current_dcm_data;

    argstringcopy = (char *)DDspace_Allocate(strlen(argstring)+1);
    strcpy(argstringcopy, argstring); /* so as not to alter user's string */
    tmp = argstringcopy;

    /* p points to first sub-string */

    while ((p = DDstrtok(tmp, " \t,"))) {
	tmp = 0;

	if (0 == strcmp(p,"-multipass_transparency")) {
	    transp_changed = ddd_g4vll_transp_multi_pass;

	} else if (0 == strcmp(p, "-sort_transparency")) {
	    transp_changed = ddd_g4vll_transp_single_pass;

	} else if (0 == strcmp(p, "-two_pass_transparency") ||
		   0 == strcmp(p, "-two_pass_add_transparency")) {
	    transp_changed = ddd_g4vll_transp_two_pass_add;

	} else if (0 == strcmp(p, "-two_pass_compos_transparency")) {
	    transp_changed = ddd_g4vll_transp_two_pass_compos;

	} else if (0 == strcmp(p, "-ztc")) {
	    ztc_changed = DcTrue;

	} else if (0 == strcmp(p, "-noztc")) {
	    ztc_changed = DcFalse;

	} else if (0 == strcmp(p, "-nocylinders")) {
	    devicedata->cylinders = ddd_g4vll_cylinders_tesselated;

	} else if (0 == strcmp(p, "-cylinders")) {
	    devicedata->cylinders = ddd_g4vll_cylinders_firmware_capped;

	} else if (0 == strcmp(p, "-cylindersnocap")) {
	    devicedata->cylinders = ddd_g4vll_cylinders_firmware_uncapped;

	} else if (0 == strcmp(p, "-nospheres")) {
	    devicedata->use_spheres = DcFalse;

	} else if (0 == strcmp(p, "-noboundingvol")) {
	    devicedata->use_hw_boundingvol = DcFalse;

	} else if (0 == strcmp(p, "-boundingvol")) {
	    devicedata->use_hw_boundingvol = DcTrue;

	} else if (0 == strcmp(p, "-spheres")) {
	    devicedata->use_spheres = DcTrue;
	}
    }

    DDspace_Deallocate(argstringcopy);
   
    if (transp_changed != -1) {
	features = VLLquery_features();
	switch (transp_changed) {
	    case ddd_g4vll_transp_multi_pass:
		if (features->multi_pass_transp) {
		    devicedata->transp_mode = ddd_g4vll_transp_multi_pass;
		    VLLset_transparency_mode(VLL_MULTI_PASS);
		    break;
		}
	    case ddd_g4vll_transp_single_pass:
		if (features->single_pass_transp) {
		    devicedata->transp_mode = ddd_g4vll_transp_single_pass;
		    VLLset_transparency_mode(VLL_SINGLE_PASS);
		    break;
		}
	    case ddd_g4vll_transp_two_pass_compos:
		if (features->two_pass_transp) {
		    devicedata->transp_mode = ddd_g4vll_transp_two_pass_compos;
		    VLLset_transparency_mode(VLL_TWO_PASS_COMPOS);
		    break;
		}
	    default:
		if (features->two_pass_transp) {
		    devicedata->transp_mode = ddd_g4vll_transp_two_pass_add;
		    VLLset_transparency_mode(VLL_TWO_PASS_ADD);
		}
		break;
	}
    }

    if (ztc_changed != -1) {
	if (devicedata->use_ztc && (ztc_changed == DcFalse)) {
	    devicedata->use_ztc = DcFalse;
	    VLLset_ztc_enable(VLL_DISABLE);
    	} else if (!(devicedata->use_ztc) && (ztc_changed == DcTrue)) {
	    devicedata->use_ztc = DcTrue;
	    VLLset_ztc_enable(VLL_ENABLE);
    	}
    }
}

/* ====================================================================== */

void ddr_g4vll_dcm_setup_shading (void)
{
    DtInt range_min, range_max;

    dde_g4vll_current_dcm_data->shade_mode = DdInqShadeMode 
	    (dde_g4vll_current_dcm_data->device);

    DDdevice_InqShadeRange (dde_g4vll_current_dcm_data->device, 
			 dde_g4vll_current_dcm_data->shade_index,
			 &range_min, &range_max);

    dde_g4vll_current_dcm_data->cwtavgpslim[0] = (DtReal)range_min;
    dde_g4vll_current_dcm_data->cwtavgpslim[1] = (DtReal)range_max;
    dde_g4vll_current_dcm_data->cwtavgpslim[2] = 
	    (dde_g4vll_current_dcm_data->cwtavgpslim[1]-
	     dde_g4vll_current_dcm_data->cwtavgpslim[0]);
}

/* ====================================================================== */

void ddr_g4vll_dcm_geom_change_handler (void)
{
#ifdef DEBUG
    printf ("ddr_g4vll_dcm_geom_change_handler: null function\n");
#endif
}

/* ====================================================================== */

DtReal ddr_g4vll_dcm_convert_y (
    DtReal y)
{
    return dde_g4vll_current_dcm_data->height - y;
}

/* ====================================================================== */

int ddr_g4vll_dcm_check_for_stereo (
    Display  *dpy, 
    int      screen,
    DtFlag   *stereoscreen)
{
    /*
     * Determine whether stereo is supported through the server.
     *
     * If it is supported, the returned property means:
     *	  VLL_STEREO_PLATE	  	nonsquare pixel, shutter plate monitor
     *	  VLL_STEREO_GLASSES	  	nonsquare pixel, glasses/emitter
     *	  VLL_STEREO_PLATE_SQUARE   	square pixel,    shutter plate monitor
     *	  VLL_STEREO_GLASSES_SQUARE 	square pixel,    glasses/emitter
     */
    Atom atomID;
    Window root;
    Bool ret;
    Atom ret_atom;
    int ret_format;
    unsigned long ret_len;
    unsigned long bytes_after;
    int *ret_prop = NULL;
    int success = 1;

    /* Get the atom ID for _XA_STEREO_SCREEN. */
    atomID = XInternAtom(dpy, _XA_STEREO_SCREEN, True);

    if (atomID != None) {
        root = RootWindow(dpy, screen);

        /* Get _XA_STEREO_SCREEN's property data from the root window. */
        ret = XGetWindowProperty(dpy, root, atomID,
            	0L           /* offset from where to retrieve prop data */ ,
            	1000000L     /* length of data to retrieve */ ,
            	FALSE        /* don't delete property after reading */ ,
                XA_INTEGER   /* return property only of type XA_INTEGER */ ,
                &ret_atom    /* actual atom type returned for property */ ,
                &ret_format  /* format of returned property data */ ,
                &ret_len     /* number of items in data */ ,
                &bytes_after /* number of unread bytes in property */ ,
                (unsigned char **)&ret_prop  /* pointer to property data */  );

        if ((ret == Success) && (ret_atom == XA_INTEGER) && 
		(ret_format != 0)) {
	    switch (*ret_prop) {
                case VLL_STEREO_PLATE:
                        *stereoscreen = VLL_STEREO_PLATE;
                        break;
                case VLL_STEREO_GLASSES:
                        *stereoscreen = VLL_STEREO_GLASSES;
                        break;
                case VLL_STEREO_PLATE_SQUARE:
                        *stereoscreen = VLL_STEREO_PLATE_SQUARE;
                        break;
                case VLL_STEREO_GLASSES_SQUARE:
                        *stereoscreen = VLL_STEREO_GLASSES_SQUARE;
                        break;
		default:
			*stereoscreen = ddd_g4vll_mono_screen;
	    }
	}
        XFree(ret_prop);
    }

    if (*stereoscreen == ddd_g4vll_mono_screen) {
	success = 0;
    }

    return(success);
}

/* ====================================================================== */

int ddr_g4vll_dcm_get_board_num (
    Display  *dpy, 
    int      screen,
    int      *boardnum)
{
    /*
     * Determine the Denali board number for the given display.
     */
    Atom atomID;
    Window root;
    Bool ret;
    Atom ret_atom;
    int ret_format;
    unsigned long ret_len;
    unsigned long bytes_after;
    int *ret_prop = NULL;
    int success = 1;

    /* Get the atom ID for _XA_KPC_DENALI_BOARD. */
    atomID = XInternAtom(dpy, _XA_KPC_DENALI_BOARD, True);

    if (atomID != None) {
        root = RootWindow(dpy, screen);

        /* Get _XA_KPC_DENALI_BOARD's property data from the root window. */
        ret = XGetWindowProperty(dpy, root, atomID,
            	0L           /* offset from where to retrieve prop data */ ,
            	1000000L     /* length of data to retrieve */ ,
            	FALSE        /* don't delete property after reading */ ,
                XA_INTEGER   /* return property only of type XA_INTEGER */ ,
                &ret_atom    /* actual atom type returned for property */ ,
                &ret_format  /* format of returned property data */ ,
                &ret_len     /* number of items in data */ ,
                &bytes_after /* number of unread bytes in property */ ,
                (unsigned char **)&ret_prop  /* pointer to property data */  );

        if ((ret == Success) && (ret_atom == XA_INTEGER) && 
		(ret_format != 0)) {
	    *boardnum = *ret_prop;
	} else {
	    *boardnum = ddd_g4vll_head_zero;
	    success = 0;
	}
        XFree(ret_prop);
    }
    return(success);
}

/* ====================================================================== */

/*
 * The XPeer extension changed such that it no longer directly accesses
 * the hardware (i.e. it no longer makes VLL calls).  For continuity, the
 * original XPeer functions called by Dore are here as function wrappers 
 * that call the new XPeer interface.
 */

#define XPEER_SUCCESS True

static void DorePeerActiveCallback (
    Display *display,
    int count)
{
    return;
}

static Bool XPeerInitialize (
    Display *display,
    int screen)
{
    char *peer_device_type;
    XPeerDeviceData peer_device_data;
    int peer_major, peer_minor;

    if (!XPeerQueryExtension(display))
        return False;

    peer_device_type = XPeerGetScreenDeviceType(display, screen);
    if (strcmp(peer_device_type, "TC/Denali") != 0)
        return False;

    peer_device_data = XPeerGetScreenDeviceData(display, screen);
    if (peer_device_data == NULL)
        return False;

    VLLpeer_query_version(&peer_major, &peer_minor);
    if (peer_device_data[0] != peer_major
     || peer_device_data[1] != peer_minor)
        return False;

    XPeerRegisterActive(display, DorePeerActiveCallback);

    return True;
}

static Bool XPeerEnableWindow (
    Display *display,
    Window window,
    unsigned int window_type,
    VLLwindow **vll_win)
{
    XPeerDeviceData	peer_device_data;

    peer_device_data = XPeerCreateWindow(display, window, window_type);
    if (peer_device_data == NULL)
	return False;

    *vll_win = VLLpeer_create_window(peer_device_data[0],
				     &peer_device_data[1]);
    if (*vll_win == NULL)
	return False;

    return True;
}

static void XPeerDisableWindow (
    Display *display,
    Window window)
{
    XPeerDestroyWindow (display, window);
}

/* ====================================================================== */
