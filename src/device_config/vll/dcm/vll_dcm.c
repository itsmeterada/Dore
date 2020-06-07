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
	DtPtr ddr_vll_return_DCM_fcns ()
	DtPtr ddr_vll_dcm_create_local_data (device)
	DtFlag ddr_vll_dcm_initialize_device (device, devicedata, argstring, type)
	static create_332_colormap (colors)
	static create_444_colormap (colors)
	static create_ramp_colormap (colors)
	void ddr_vll_dcm_close_device ()
	void ddr_vll_dcm_flush ()
	void ddr_vll_dcm_swap_buffers ()
	void ddr_vll_dcm_update_geometry ()
	void ddr_vll_dcm_become_current_driver (device_data)
	void ddr_vll_dcm_set_options (argstring)
	void ddr_vll_dcm_setup_shading ()
	void ddr_vll_dcm_geom_change_handler ()
	DtReal ddr_vll_dcm_convert_y (y)

  ======================================================================
 */
#include "dcm.h"
#include "dore/dore_develop/develop.h"
#include <sys/sysmips.h>

ddt_vll_data *dde_vll_current_dcm_data;

struct list *dde_vll_dcm_list = NULL;

DtInt dde_vll_dcm_open_count = 0;

extern DtPtr ddr_vll_dcm_create_local_data();
extern DtFlag ddr_vll_dcm_initialize_device();
extern void ddr_vll_dcm_become_current_driver();
extern void ddr_vll_dcm_close_device();
extern void ddr_vll_dcm_flush();
extern void ddr_vll_dcm_swap_buffers();
extern void ddr_vll_dcm_update_geometry();
extern void ddr_vll_dcm_inquire_device_extent();
extern void ddr_vll_dcm_inquire_stereo();
extern void ddr_vll_dcm_inquire_ncolors();
extern void ddr_vll_dcm_inquire_resolution();
extern void ddr_vll_dcm_inquire_visual_type();
extern void ddr_vll_dcm_inquire_auto_size();
extern void ddr_vll_dcm_inquire_color_entries();
extern void ddr_vll_dcm_set_color_entries();
extern void ddr_vll_dcm_set_foreground_color();
extern void ddr_vll_dcm_set_background_color();
extern void ddr_vll_dcm_set_shade_index();
extern void ddr_vll_dcm_inquire_clip_list();
extern void ddr_vll_dcm_clear_rectangle_depth();
extern void ddr_vll_dcm_clear_rectangle_depth_and_color();
extern void ddr_vll_dcm_set_depth_buffer_enable();
extern void ddr_vll_dcm_set_depth_buffer_write();
extern void ddr_vll_dcm_write_scanline_byte();
extern void ddr_vll_dcm_set_current_view();
extern DtFlag ddr_vll_dcm_inquire_pixel_data();
extern void ddr_vll_dcm_set_options();

DDt_DCM *dde_vll_DCM_fcns = (DDt_DCM *)0;

/*
 ======================================================================
 */
DtPtr
ddr_vll_return_DCM_fcns()
{
    DDt_DCM *fcns;

    if (dde_vll_DCM_fcns == (DDt_DCM *)0) {
	fcns = DDdevice_CreateDCMStruct();

	if (fcns == (DDt_DCM *)0) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_vll_return_DCM_fcns",
			DcNullPtr);
	}

	fcns->version_number 		= 2;
	/* Version 1 Functions */
	fcns->create_local_data 	= (DtPFI) ddr_vll_dcm_create_local_data;
	fcns->initialize_device 	= (DtPFI) ddr_vll_dcm_initialize_device;
	fcns->become_current_driver 	= (DtPFI) ddr_vll_dcm_become_current_driver;
	fcns->close_device		= (DtPFI) ddr_vll_dcm_close_device;
	fcns->flush 			= (DtPFI) ddr_vll_dcm_flush;
	fcns->swap_buffers 		= (DtPFI) ddr_vll_dcm_swap_buffers;
	fcns->update_geometry 		= (DtPFI) ddr_vll_dcm_update_geometry;
	fcns->inquire_device_extent 	= (DtPFI) ddr_vll_dcm_inquire_device_extent;
	fcns->inquire_stereo 		= (DtPFI) ddr_vll_dcm_inquire_stereo;
	fcns->inquire_ncolors 		= (DtPFI) ddr_vll_dcm_inquire_ncolors;
	fcns->inquire_resolution 	= (DtPFI) ddr_vll_dcm_inquire_resolution;
	fcns->inquire_visual_type 	= (DtPFI) ddr_vll_dcm_inquire_visual_type;
	fcns->inquire_auto_size 	= (DtPFI) ddr_vll_dcm_inquire_auto_size;
	fcns->inquire_color_entries 	= (DtPFI) ddr_vll_dcm_inquire_color_entries;
	fcns->set_color_entries 	= (DtPFI) ddr_vll_dcm_set_color_entries;
	fcns->set_foreground_color 	= (DtPFI) ddr_vll_dcm_set_foreground_color;
	fcns->set_background_color 	= (DtPFI) ddr_vll_dcm_set_background_color;
	fcns->set_shade_index 		= (DtPFI) ddr_vll_dcm_set_shade_index;
	fcns->inquire_clip_list 	= (DtPFI) ddr_vll_dcm_inquire_clip_list;
	fcns->clear_rectangle_depth 	= (DtPFI) ddr_vll_dcm_clear_rectangle_depth;
	fcns->clear_rectangle_depth_and_color 	= (DtPFI) ddr_vll_dcm_clear_rectangle_depth_and_color;
	fcns->set_depth_buffer_enable 	= (DtPFI) ddr_vll_dcm_set_depth_buffer_enable;
	fcns->set_depth_buffer_write 	= (DtPFI) ddr_vll_dcm_set_depth_buffer_write;
	fcns->write_scanline_byte 	= (DtPFI) ddr_vll_dcm_write_scanline_byte;
	fcns->set_current_view 		= (DtPFI) ddr_vll_dcm_set_current_view;
	fcns->inquire_pixel_data 		= (DtPFI) ddr_vll_dcm_inquire_pixel_data;
	/* Version 2 Functions */
	fcns->set_options		= (DtPFI) ddr_vll_dcm_set_options;

	dde_vll_DCM_fcns 		= fcns;
    }
    return (DtPtr)dde_vll_DCM_fcns;
}
/*
 ======================================================================
 */
DtPtr 
ddr_vll_dcm_create_local_data (device)
	DtPtr device;
{
    ddt_vll_data *device_data;

    if ((device_data = 
	 (ddt_vll_data *)DDspace_Allocate(sizeof (ddt_vll_data))) == NULL) {
	DDerror (ERR_CANT_ALLOC_MEM_SIZE, 
		    "ddr_vll_dcm_create_local_data",
		    "sizeof (ddt_vll_data)");
	return ((DtPtr) DcNullPtr);
    }

    device_data->device = device;

    if (dde_vll_dcm_list == NULL) {
#ifdef DEBUG
	printf ("creating vll_dcm_list\n");
#endif
	dde_vll_dcm_list = (struct list *)dor_list_create();

	if (dde_vll_dcm_list == NULL) {
	    DDerror (ERR_CANT_ALLOC_MEM_SIZE, 
			"ddr_vll_dcm_create_local_data",
			"dor_list_create");

	    return ((DtPtr) device_data);
	}
    }

#ifdef DEBUG
    printf ("adding 0x%x device to list\n", device_data);
#endif
    dor_list_add_data_last(dde_vll_dcm_list, device_data);

    return ((DtPtr) device_data);
}
/*
 ======================================================================
 */
DtFlag
ddr_vll_dcm_initialize_device (device, devicedata, argstring, type)
	DtPtr device;
	ddt_vll_data *devicedata;
	DtPtr argstring;
	DtPtr type;
{
    VLLinfo 	*features;
    extern void	ddr_vll_dcm_geom_change_handler();
    char		*argstringcopy;
    char		*tmp;
    char		*p;
    char		*nextarg;
    static char 	*funcname  = "ddr_vll_dcm_initialize_device";
    int		error=0;
    char 	*getenv();
    XClassHint	class_hints;
    XVisualInfo	*visual_info, vinfo_template;
    int		nvisuals;
    XSetWindowAttributes window_attributes;
    int		attrMask;
    int		screen;
    char		*display_name;
    XSizeHints	 xsh;
    XEvent	event;
    static XColor	colors[4096];
    Colormap	cmap;
    DtInt	nplanes;
    unsigned int	client_ref, window_ref, mapped_ref;
    static XWMHints xwmh = {
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

    devicedata->device = device;

    devicedata->x = 0;
    devicedata->y = 0;
    devicedata->width = 512;
    devicedata->height = 512;
    devicedata->double_buffered = DcTrue;
    devicedata->use_spheres = DcFalse;
    devicedata->transp_type = 0;
    devicedata->auto_size = DcTrue;
    devicedata->visualtype = DcTrueColor;
    devicedata->ncolors = 256;
    devicedata->stereoswitch = DcFalse;
    devicedata->user_window = DcFalse;
    devicedata->user_display = DcFalse;
    devicedata->nrects = 0;
    devicedata->clip_rect = 0;
    devicedata->sequence_number = 0;
    devicedata->clipid = 0;
    devicedata->texture_expand = ddd_vll_expand_default;
    devicedata-> offscreen = DcFalse;
    devicedata->output_buffer = VLL_NONDISPLAY_BUFFER;

    devicedata->use_X = DcTrue;
    if ((type != (DtPtr)0) &&
	(!strcmp(type, "vll") ||
	 !strcmp(type, "g3") ||
	 !strcmp(type, "st"))) {
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
     * Process the argstring
     */

    argstringcopy = (char *)DDspace_Allocate(strlen(argstring)+1);
    strcpy(argstringcopy, argstring); /* so as not to alter user's string */
    tmp = argstringcopy;

    /* p points to first sub-string */

    while ((p = DDstrtok(tmp, " \t,"))) {
	tmp = 0;

	if (!strcmp(p,"-multipass_transparency")) {
	    devicedata->transp_type = 2;

	} else if (!strcmp(p, "-sort_transparency")) {
	    devicedata->transp_type = 1;

	} else if (!strcmp(p, "-two_pass_compos_transparency")) {
	    devicedata->transp_type = 3;

	} else if (!strcmp(p, "-nospheres")) {
	    devicedata->use_spheres = DcFalse;

	} else if (!strcmp(p, "-spheres")) {
	    devicedata->use_spheres = DcTrue;

	} else if (!strcmp(p,"-noautosize")) {
	    devicedata->auto_size = DcFalse;

	} else if (!strcmp(p,"-singlebuffered")) {
	    devicedata->double_buffered = DcFalse;

        } else if (!strcmp(p,"-offscreen")) {
            /* if offscreen drawing is used, no colormap setting
               nor control_planes setting should occur */
            devicedata->offscreen = DcFalse;
#ifdef DEBUG_STATE
            devicedata->offscreen = DcTrue;
            devicedata->output_buffer = VLL_PIXMAP_BUFFER;
            devicedata->use_X = DcFalse;
#endif

	}	else if (!strcmp(p,"-geometry")){
	    if (!(nextarg = DDstrtok(0, " \t,"))){
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "geometry: (no value)");
		error = 1;
	    } else {
		XParseGeometry(nextarg,
			       &devicedata->x,&devicedata->y,
			       &devicedata->width, &devicedata->height);
	    }

	}	else if (!strcmp(p,"-ncolors")){
	    if (!(nextarg = DDstrtok(0, " \t,"))){
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "ncolors: (no value)");
		error = 1;
	    } else {
		int w;

		sscanf(nextarg,"%d",&w);
		devicedata->ncolors = w;
	    }
	     
	} else if (!strcmp(p, "-visualtype")) {
	    if (!(nextarg = DDstrtok(0, " \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "visualtype: (no value)");

		error = 1;
	    } else	{
		if (!strcmp("DcPseudoColor",nextarg)) {
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
	} else if (!strcmp(p,"-stereo")) {
	    devicedata->stereoswitch = DcTrue;

	} else if (!strcmp(p, "-noX")) {
	    devicedata->use_X = DcFalse;

	} else if (!strcmp(p, "-X")) {
	    devicedata->use_X = DcTrue;
	 
	}	else if (!strcmp(p,"-window")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "window: (no value)");
		error = 1;
	    } else	{
		int w;

		devicedata->user_window = DcTrue;
		sscanf(nextarg,"%d",&w);
		devicedata->window = (Window)w;
	    }
	}	else if (!strcmp(p,"-display"))	{
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "device: (no value)");
		error = 1;
	    }else{
		int dptr;
		devicedata->user_display = DcTrue;
		sscanf(nextarg,"%d",&dptr);
		devicedata->display = (Display *)dptr;
	    }

	} else if (!strcmp(p, "-texture_expand")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "texture_expand: (no value)");
		error = 1;
	    }else{
		if (!strcmp(nextarg, "bilinear")) {
		    devicedata->texture_expand = ddd_vll_expand_bilinear;
		} else if (!strcmp(nextarg, "replicate")) {
		    devicedata->texture_expand = ddd_vll_expand_replicate;
		} else {
		    devicedata->texture_expand = ddd_vll_expand_default;
		}
	    }
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
	if (!devicedata->user_display) {
	    if ((display_name = getenv("DISPLAY")) == NULL) {
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
		
	/* get some info from user's window */

	if (!devicedata->user_window) {
	    vinfo_template.screen = XDefaultScreen(devicedata->display);

	    screen = 0;
      
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
	    }

	    visual_info = XGetVisualInfo(devicedata->display,
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

	    switch (devicedata->visualtype) {
	    case DcPseudoColor:
		if (devicedata->ncolors == 256) {
		    cmap = XCreateColormap(devicedata->display,
					   XDefaultRootWindow(devicedata->display),
					   visual_info->visual,AllocAll);
		    create_332_colormap(colors);
		    XStoreColors(devicedata->display, cmap, colors, 256);
		    attrMask |= CWColormap;
		    window_attributes.colormap = cmap;
		    break;
		}
		/* NOTE: if ncolors > 256 then we are doing 12 bit pseudo color
		   but using a TrueColor visual so fall through to the TrueColor 
		   case */
	    case DcTrueColor:
		if (visual_info->visual == DefaultVisual(devicedata->display, screen)) {
		    cmap = DefaultColormap(devicedata->display,screen);
		} else {
		    cmap = XCreateColormap(devicedata->display,
					   XDefaultRootWindow(devicedata->display),
					   visual_info->visual,AllocNone);
		}
		attrMask |= CWColormap;
		window_attributes.colormap = cmap;
		break;
	    case DcDirectColor:
		if (visual_info->visual == DefaultVisual(devicedata->display, screen)) {
		    cmap = DefaultColormap(devicedata->display,screen);
		} else {
		    cmap = XCreateColormap(devicedata->display,
					   XDefaultRootWindow(devicedata->display),
					   visual_info->visual,AllocAll);
		    create_ramp_colormap(colors);
		    XStoreColors(devicedata->display, cmap, colors, 256);
		}
		attrMask |= CWColormap;
		window_attributes.colormap = cmap;
		break;

	    }

	    window_attributes.background_pixel = 0x000000;
	    window_attributes.border_pixel = 0xffffff;

	    devicedata->window = XCreateWindow(devicedata->display,
					       DefaultRootWindow(devicedata->display),
					       devicedata->x, devicedata->y,
					       devicedata->width, devicedata->height,0,
					       nplanes,InputOutput,visual_info->visual,
					       attrMask,&window_attributes);
	 
	    if (!devicedata->window) {
		DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			    funcname, "Could not create X11 window");

		DDspace_Deallocate(devicedata);

		if (!devicedata->user_display) {
		    XCloseDisplay(devicedata->display);
		}
		return DcFalse;
	    }

	    /*  Set the standard properties for the window managers.  */
      
	    XStoreName(devicedata->display,devicedata->window,"Dore Window");
	    XSetIconName(devicedata->display,devicedata->window,"Dore Window");
	    XSetWMHints(devicedata->display,devicedata->window,&xwmh);

	    xsh.flags = (USPosition|USSize);
	    xsh.width = devicedata->width;
	    xsh.height = devicedata->height;
	    xsh.x = devicedata->x;
	    xsh.y = devicedata->y;
	    XSetSizeHints(devicedata->display,devicedata->window,&xsh,
			  XA_WM_NORMAL_HINTS);
	    class_hints.res_name = "dore";
	    class_hints.res_class = "Dore";
	    XSetClassHint(devicedata->display,devicedata->window,&class_hints);

	    XSelectInput(devicedata->display,devicedata->window,ExposureMask);
      
	    XMapWindow(devicedata->display,devicedata->window);

	    while (XNextEvent(devicedata->display,&event)) {
		if (event.type == Expose) 
			break;
	    }
	}
	devicedata->gc = XCreateGC(devicedata->display, devicedata->window, 0, 0);
	if (!devicedata->gc) {
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, "Could not create graphics context");

	    if (!devicedata->user_window) {
		XUnmapWindow(devicedata->display, devicedata->window);
		XDestroyWindow(devicedata->display, devicedata->window);
	    }
	    if (!devicedata->user_display) {
		XCloseDisplay(devicedata->display);
	    }
	    DDspace_Deallocate(devicedata);
	    return DcFalse;
	}

	if (XTitanAllocClipID(devicedata->display, devicedata->window, 
			      &(devicedata->clipid), &client_ref, &window_ref,
			      &mapped_ref) == 0) {
	    DDerror(ERR_DEVICE_SYSTEM_ACCESS, funcname,
		       "Request for clip id failed");
	    if (!devicedata->user_window) {
		XUnmapWindow(devicedata->display, devicedata->window);
		XDestroyWindow(devicedata->display, devicedata->window);
	    }
	    if (!devicedata->user_display) {
		XCloseDisplay(devicedata->display);
	    }
	    DDspace_Deallocate(devicedata);
	    return DcFalse;
	}
	 
	if (client_ref > 1) {
	    DDerror(ERR_DEVICE_SYSTEM_ACCESS, funcname,
		       "Could not get a free clip id");
	    if (!devicedata->user_window) {
		XUnmapWindow(devicedata->display, devicedata->window);
		XDestroyWindow(devicedata->display, devicedata->window);
	    }
	    if (!devicedata->user_display) {
		XCloseDisplay(devicedata->display);
	    }
	    DDspace_Deallocate(devicedata);
	    return DcFalse;
	}

	XTitanAssociateClipID(devicedata->display, devicedata->window, 
			      devicedata->clipid, 0);
    } else {
	devicedata->clipid = 0;
        devicedata->nrects = 1;
        devicedata->clip_rect = (XBoxRec *)DDspace_Allocate(sizeof(XBoxRec));
        devicedata->clip_rect->x1 =  devicedata->x;
        devicedata->clip_rect->y1 =  devicedata->y;
        devicedata->clip_rect->x2 =  devicedata->x + devicedata->width - 1; 
        devicedata->clip_rect->y2 =  devicedata->y + devicedata->height - 1;
    }

    if (dde_vll_dcm_open_count == 0) {
	if (!VLLinit (doe_system_main_processors, 
		      (unsigned int)(devicedata->use_X ? VLL_X11_PEER : 0x0),
		      ddr_vll_dcm_geom_change_handler))
		return (DcFalse);

	if (sizeof(float) != sizeof(DtReal)) {
	    VLLset_data_precision(VLL_DATA_DOUBLE);
	} else {
	    VLLset_data_precision(VLL_DATA_SINGLE);
	}

	if (DDgetenv("DORE_VLL_NO_FORCE") == NULL) {
	    if (doe_system_main_processors > 1) {
		dor_doresys_titan_setup_threads(doe_system_main_processors);
	    } else {
		sysmips(SFORCE_NORUN,sysmips(SBOOT_CPU));
	    }
	}
    }
    dde_vll_dcm_open_count++;

    dde_vll_current_dcm_data = devicedata;

    dde_vll_current_dcm_data->bad_geom = DcTrue;

    features = VLLquery_features();

    if (devicedata->stereoswitch) {
	if (features->stereo && !devicedata->use_X &&
	    devicedata->stereoswitch) {
	    VLLset_screen_type (VLL_STEREO_SCREEN);
	} else {
	    devicedata->stereoswitch = DcFalse;
	}
    }

    switch(devicedata->visualtype) {
    case DcPseudoColor:
	{ int range[2];

	  range[0] = 0;
	  range[1] = (devicedata->ncolors - 1);

	  VLLset_pseudo_range(devicedata->cwtavgpsweight, range, 
			      (unsigned)devicedata->ncolors);
	  VLLset_pseudo_space(devicedata->cbtselpsweight, devicedata->cbtselpsmask,
			      (unsigned)devicedata->ncolors);
	  VLLset_output_color_model(VLL_PSEUDO_SPACE);
      }
	break;

    case DcTrueColor:
    case DcDirectColor:
	VLLset_output_color_model(VLL_TRUE_COLOR);
	break;
    }

    switch (devicedata->transp_type) {
    case 2:
	if (features->multi_pass_transp) {
	    devicedata->transp_type = 2;
	    VLLset_transparency_mode(VLL_MULTI_PASS);
	    break;
	}
    case 1:
	if (features->single_pass_transp) {
	    devicedata->transp_type = 1;
	    VLLset_transparency_mode(VLL_SINGLE_PASS);
	    break;
	}
    case 3:
	if (features->two_pass_transp) {
	    devicedata->transp_type = 3;
	    VLLset_transparency_mode(VLL_TWO_PASS_COMPOS);
	    break;
	}
    default:
	if (features->two_pass_transp) {
	    devicedata->transp_type = 0;
	    VLLset_transparency_mode(VLL_TWO_PASS_ADD);
	}
	break;
    }

    if (!devicedata->use_X && !devicedata->offscreen) {
	create_ramp_colormap(colors);
	if (devicedata->visualtype == DcPseudoColor) {
	    if (devicedata->ncolors > 256) {
		VLLset_output_color_model(VLL_TRUE_COLOR);
		VLLload_colormap(VLLquery_colormap_id(VLL_TRUE_COLOR),
				 colors, 0, 256);
	      
		VLLset_output_color_model(VLL_PSEUDO_SPACE);
		create_444_colormap(colors);
		VLLload_colormap(VLL_LUT_PSEUDO_16, colors, 0, 4096);
	    } else {
		VLLload_colormap(VLLquery_colormap_id(VLL_PSEUDO_COLOR),
				 colors, 0, 256);
	    }
	} else {
	    VLLload_colormap(VLLquery_colormap_id(VLL_TRUE_COLOR),
			     colors, 0,256);
	}
    }

    VLLset_dst_window(devicedata->x, devicedata->y, 
		      devicedata->width, devicedata->height, 
		      (int)devicedata->clipid);
    ddr_vll_dcm_update_geometry();

    /* tell VLL to draw quads using triangles */

    VLLset_quad_draw_mode(VLL_QUAD_DRAW_TRIS);

    return (DcTrue);
}
/*
  ======================================================================
 */
static
create_332_colormap(colors)
	XColor colors[];
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
/*
  ======================================================================
 */
static
create_444_colormap(colors)
	XColor colors[];
{
    DtInt i;

    for (i=0; i<4096; i++) {
	colors[i].red = (i>>8) / 15. * 65535.;
	colors[i].green = ((i>>4) & 0x0f) / 15. * 65535.;
	colors[i].blue = (i & 0x0f) / 15. * 65535.;
	colors[i].flags = DoRed|DoGreen|DoBlue;
	colors[i].pixel = i;
    }
}
/*
  ======================================================================
 */
static
create_ramp_colormap(colors)
	XColor colors[];
{
    DtInt i;

    for (i=0; i<256; i++) {
	colors[i].red = colors[i].green = colors[i].blue =  (i / 255.) * 65535.;
	colors[i].flags = DoRed|DoGreen|DoBlue;
	colors[i].pixel = i;
    }
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_close_device ()
{
    ddt_vll_data *device;
    ddt_vll_data *dor_list_get_first_data();
    ddt_vll_data *dor_list_get_next_data();

    if (dde_vll_dcm_list != (struct list *)NULL) {
	for (device = dor_list_get_first_data(dde_vll_dcm_list);
	     device != (ddt_vll_data *)NULL;
	     device = dor_list_get_next_data (dde_vll_dcm_list)) {
	    if (device == dde_vll_current_dcm_data) {
#ifdef DEBUG
		printf ("deleting current node for data 0x%x\n", device);
#endif
		dor_list_remove_current_node(dde_vll_dcm_list);
		break;
	    }
	}
    }

    if (dde_vll_current_dcm_data->use_X) {
	XTitanFreeClipID(dde_vll_current_dcm_data->display,
			 dde_vll_current_dcm_data->window,
			 dde_vll_current_dcm_data->clipid);

	if (!dde_vll_current_dcm_data->user_window) {
	    XUnmapWindow(dde_vll_current_dcm_data->display, 
			 dde_vll_current_dcm_data->window);
	    XDestroyWindow(dde_vll_current_dcm_data->display, 
			   dde_vll_current_dcm_data->window);
	}
	if (!dde_vll_current_dcm_data->user_display) {
	    XCloseDisplay(dde_vll_current_dcm_data->display);
	}
    } else {
	if (dde_vll_current_dcm_data->stereoswitch) {
	    VLLset_screen_type(VLL_MONO_SCREEN);
	}
    }

    /*
     *  Termination and reinitialisation in MP cannot be done:  only
     *  the distinguished thread will be aware of file descriptors
     *  that were closed and reopened, and the other thread(s) will
     *  cause errors.  Solution is not to terminate until end of
     *  program execution.

    if (!(--dde_vll_dcm_open_count)) 
	VLLterminate();
    */

    DDspace_Deallocate(dde_vll_current_dcm_data);

    dde_vll_current_dcm_data = NULL;
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_flush ()
{
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_swap_buffers ()
{
    VLLflush();

    if (dde_vll_current_dcm_data->bad_geom)
	    return;

    VLLswap_window();
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_update_geometry ()
{
    ddt_vll_data *data;
    XBoxPtr pBoxBase;
    XBoxRec extent;
    int dbmode;

    data = dde_vll_current_dcm_data;

    if (data->use_X) {
	if (data->bad_geom || data->nrects == 0) {
	    pBoxBase = XTitanGetClipList(data->display, data->window, data->gc,
					 &(data->nrects), &(data->sequence_number),
					 &extent, &dbmode);

	    if (data->clip_rect) {
		XFree(data->clip_rect);
	    }
	    data->clip_rect = pBoxBase;

	    data->x = extent.x1;
	    data->y = extent.y1;
	    data->width = extent.x2 - extent.x1;
	    data->height = extent.y2 - extent.y1;

	    VLLset_draw_key((unsigned)data->sequence_number);
	    VLLset_dst_clip_list(data->nrects, (VLLrect *)data->clip_rect);
	    VLLset_dst_window(data->x, data->y, data->width, data->height,
			      (int)data->clipid);
	    VLLset_output_buffer(data->output_buffer);

	    data->clr_device = DcTrue;
	}
    } else {
	if (data->bad_geom) {
	    VLLrect rect;
	    rect.xl = data->x;
	    rect.yt = data->y;
	    rect.xr = data->x + data->width - 1;
	    rect.yb = data->y + data->height - 1;
	    VLLset_dst_clip_list(data->nrects, (VLLrect *)data->clip_rect);
	    VLLset_dst_window(data->x, data->y, data->width, data->height, 
			      (int)data->clipid);
	    VLLset_output_buffer(data->output_buffer);
            if (!data->offscreen) {
	        if (data->visualtype == DcPseudoColor) {
	      	    if (data->ncolors > 256) {
		        VLLset_control_planes ((int)data->clipid, 
				VLLquery_colormap_id(VLL_TRUE_COLOR), 1, 
					   &rect);
	   	    } else {
		        VLLset_control_planes ((int)data->clipid, 
				VLLquery_colormap_id(VLL_PSEUDO_COLOR), 1, 
					   &rect);
	 	    }
	        } else {
		     VLLset_control_planes ((int)data->clipid, 
				VLLquery_colormap_id(VLL_TRUE_COLOR), 1, &rect);
	        }
	        data->clr_device = DcTrue;
	    }
        }
    }
    data->bad_geom = DcFalse;
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_become_current_driver (device_data)
	ddt_vll_data *device_data;
{
    dde_vll_current_dcm_data = device_data;
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_set_options (argstring)
	DtPtr argstring;
{
    VLLinfo 	*features;
    char		*argstringcopy;
    char		*tmp;
    char		*p;
    char		*nextarg;
    ddt_vll_data *devicedata;
    int		transp_changed = -1;


    devicedata = dde_vll_current_dcm_data;

    argstringcopy = (char *)DDspace_Allocate(strlen(argstring)+1);
    strcpy(argstringcopy, argstring); /* so as not to alter user's string */
    tmp = argstringcopy;

    /* p points to first sub-string */

    while ((p = DDstrtok(tmp, " \t,"))) {
	tmp = 0;

	if (!strcmp(p,"-multipass_transparency")) {
	    transp_changed = 2;

	} else if (!strcmp(p, "-sort_transparency")) {
	    transp_changed = 1;

	} else if (!strcmp(p, "-two_pass_transparency") ||
		   !strcmp(p, "-two_pass_add_transparency")) {
	    transp_changed = 0;

	} else if (!strcmp(p, "-two_pass_compos_transparency")) {
	    transp_changed = 3;

	} else if (!strcmp(p, "-nospheres")) {
	    devicedata->use_spheres = DcFalse;

	} else if (!strcmp(p, "-spheres")) {
	    devicedata->use_spheres = DcTrue;
	} else if (!strcmp(p, "-texture_expand")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    "ddr_vll_dcm_set_options",
			    "texture_expand: (no value)");
	    }else{
		if (!strcmp(nextarg, "bilinear")) {
		    devicedata->texture_expand = ddd_vll_expand_bilinear;
		} else if (!strcmp(nextarg, "replicate")) {
		    devicedata->texture_expand = ddd_vll_expand_replicate;
		} else {
		    devicedata->texture_expand = ddd_vll_expand_default;
		}
	    }
	}
    }

    DDspace_Deallocate(argstringcopy);

   
    if (transp_changed != -1) {
	features = VLLquery_features();
	switch (transp_changed) {
	case 2:
	    if (features->multi_pass_transp) {
		devicedata->transp_type = 2;
		VLLset_transparency_mode(VLL_MULTI_PASS);
		break;
	    }
	case 1:
	    if (features->single_pass_transp) {
		devicedata->transp_type = 1;
		VLLset_transparency_mode(VLL_SINGLE_PASS);
		break;
	    }
	case 3:
	    if (features->two_pass_transp) {
		devicedata->transp_type = 3;
		VLLset_transparency_mode(VLL_TWO_PASS_COMPOS);
		break;
	    }
	default:
	    if (features->two_pass_transp) {
		devicedata->transp_type = 0;
		VLLset_transparency_mode(VLL_TWO_PASS_ADD);
	    }
	    break;
	}
    }
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_setup_shading()
{
    DtInt range_min, range_max;

    dde_vll_current_dcm_data->shade_mode = DdInqShadeMode 
	    (dde_vll_current_dcm_data->device);

    DDdevice_InqShadeRange (dde_vll_current_dcm_data->device, 
			 dde_vll_current_dcm_data->shade_index,
			 &range_min, &range_max);

    dde_vll_current_dcm_data->cwtavgpslim[0] = (DtReal)range_min;
    dde_vll_current_dcm_data->cwtavgpslim[1] = (DtReal)range_max;
    dde_vll_current_dcm_data->cwtavgpslim[2] = 
	    (dde_vll_current_dcm_data->cwtavgpslim[1]-
	     dde_vll_current_dcm_data->cwtavgpslim[0]);
}
/*
 ======================================================================
 */
void
ddr_vll_dcm_geom_change_handler()
{
    ddt_vll_data *device;
    ddt_vll_data *dor_list_get_first_data();
    ddt_vll_data *dor_list_get_next_data();

    if (dde_vll_dcm_list != (struct list *)0) {
	for (device = dor_list_get_first_data(dde_vll_dcm_list);
	     device != (ddt_vll_data *)0;
	     device = dor_list_get_next_data (dde_vll_dcm_list)) {
#ifdef DEBUG
	    printf ("setting aborted draw flag / bad geom for 0x%x\n", device);
#endif
	    DDdevice_SetAbortedDraw(device->device,DcTrue);
	    device->bad_geom = DcTrue;
	    device->current_view = DcNullObject;
	}
    } else {
#ifdef DEBUG
	printf ("setting aborted draw flag / bad geom for current\n");
#endif
	DDdevice_SetAbortedDraw(dde_vll_current_dcm_data->device,DcTrue);
	dde_vll_current_dcm_data->bad_geom = DcTrue;
	dde_vll_current_dcm_data->current_view = DcNullObject;
    }
}
/*
 ======================================================================
 */
DtReal
ddr_vll_dcm_convert_y(y)
     DtReal y;
{
    return dde_vll_current_dcm_data->height - y;
}
/*
 ======================================================================
 */
