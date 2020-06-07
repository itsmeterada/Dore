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
	DtPtr ddr_x11_titang2_return_DCM_fcns ()
	DtPtr ddr_x11_titang2_dcm_create_local_data (device)
	DtFlag ddr_x11_titang2_dcm_initialize_device (device, device_data, argstring)
	static create_332_colormap (colors)
	void ddr_x11_titang2_dcm_close_device ()
	void ddr_x11_titang2_dcm_flush ()
	int ddr_x11_titang2_par_tg_flush ()
	void ddr_x11_titang2_dcm_swap_buffers ()
	int ddr_x11_titang2_par_swap_buffers_flush_lock_and_drain ()
	void ddr_x11_titang2_dcm_update_geometry ()
	void ddr_x11_titang2_dcm_become_current_driver (device_data)
	void ddr_x11_titang2_dcm_setup_shading ()
	int ddr_x11_titang2_dcm_cswap_error_handler (tg_cbm,tg_cbs,class,str,x)
	int ddr_x11_titang2_dcm_restore_state ()

  ======================================================================
 */
#include "dcm.h"
#include <sys/sysmips.h>


ddt_x11_titang2_data *dde_x11_titang2_current_dcm_data;
int dde_x11_titang2_thread_lock = 1;
int dde_x11_titang2_tg_fd;

int dde_x11_titang2_tg_open_count = 0;

struct list *dde_x11_titang2_dcm_list = NULL;

ddt_x11_titang2_dcm_tigr_state dde_x11_titang2_tigr_state;
/*
 ======================================================================
 */
extern DtPtr ddr_x11_titang2_dcm_create_local_data();
extern DtFlag ddr_x11_titang2_dcm_initialize_device();
extern void ddr_x11_titang2_dcm_become_current_driver();
extern void ddr_x11_titang2_dcm_close_device();
extern void ddr_x11_titang2_dcm_flush();
extern void ddr_x11_titang2_dcm_swap_buffers();
extern void ddr_x11_titang2_dcm_update_geometry();
extern void ddr_x11_titang2_dcm_inquire_device_extent();
extern void ddr_x11_titang2_dcm_inquire_stereo();
extern void ddr_x11_titang2_dcm_inquire_ncolors();
extern void ddr_x11_titang2_dcm_inquire_resolution();
extern void ddr_x11_titang2_dcm_inquire_visual_type();
extern void ddr_x11_titang2_dcm_inquire_auto_size();
extern void ddr_x11_titang2_dcm_inquire_color_entries();
extern void ddr_x11_titang2_dcm_set_color_entries();
extern void ddr_x11_titang2_dcm_set_foreground_color();
extern void ddr_x11_titang2_dcm_set_background_color();
extern void ddr_x11_titang2_dcm_set_shade_index();
extern void ddr_x11_titang2_dcm_inquire_clip_list();
extern void ddr_x11_titang2_dcm_clear_rectangle_depth();
extern void ddr_x11_titang2_dcm_clear_rectangle_depth_and_color();
extern void ddr_x11_titang2_dcm_set_depth_buffer_enable();
extern void ddr_x11_titang2_dcm_set_depth_buffer_write();
extern void ddr_x11_titang2_dcm_write_scanline_byte();
extern void ddr_x11_titang2_dcm_set_current_view();
extern DtFlag ddr_x11_titang2_dcm_inquire_pixel_data();
extern void ddr_x11_titang2_dcm_set_options();

DDt_DCM *dde_x11_titang2_DCM_fcns = (DDt_DCM *)0;

/*
 ======================================================================
 */
DtPtr
ddr_x11_titang2_return_DCM_fcns()
{
    DDt_DCM *fcns;

    if (dde_x11_titang2_DCM_fcns == (DDt_DCM *)0) {
	fcns = DDdevice_CreateDCMStruct();

	if (fcns == (DDt_DCM *)0) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_x11_titang2_return_DCM_fcns",
			DcNullPtr);
	}

	fcns->version_number 			= 1;
	/* Version 1 Functions */
	fcns->create_local_data 		= (DtPFI) ddr_x11_titang2_dcm_create_local_data;
	fcns->initialize_device 		= (DtPFI) ddr_x11_titang2_dcm_initialize_device;
	fcns->become_current_driver 		= (DtPFI) ddr_x11_titang2_dcm_become_current_driver;
	fcns->close_device			= (DtPFI) ddr_x11_titang2_dcm_close_device;
	fcns->flush 				= (DtPFI) ddr_x11_titang2_dcm_flush;
	fcns->swap_buffers 			= (DtPFI) ddr_x11_titang2_dcm_swap_buffers;
	fcns->update_geometry 			= (DtPFI) ddr_x11_titang2_dcm_update_geometry;
	fcns->inquire_device_extent 		= (DtPFI) ddr_x11_titang2_dcm_inquire_device_extent;
	fcns->inquire_stereo 			= (DtPFI) ddr_x11_titang2_dcm_inquire_stereo;
	fcns->inquire_ncolors 			= (DtPFI) ddr_x11_titang2_dcm_inquire_ncolors;
	fcns->inquire_resolution 		= (DtPFI) ddr_x11_titang2_dcm_inquire_resolution;
	fcns->inquire_visual_type 		= (DtPFI) ddr_x11_titang2_dcm_inquire_visual_type;
	fcns->inquire_auto_size 		= (DtPFI) ddr_x11_titang2_dcm_inquire_auto_size;
	fcns->inquire_color_entries 		= (DtPFI) ddr_x11_titang2_dcm_inquire_color_entries;
	fcns->set_color_entries 		= (DtPFI) ddr_x11_titang2_dcm_set_color_entries;
	fcns->set_foreground_color 		= (DtPFI) ddr_x11_titang2_dcm_set_foreground_color;
	fcns->set_background_color 		= (DtPFI) ddr_x11_titang2_dcm_set_background_color;
	fcns->set_shade_index 			= (DtPFI) ddr_x11_titang2_dcm_set_shade_index;
	fcns->inquire_clip_list 		= (DtPFI) ddr_x11_titang2_dcm_inquire_clip_list;
	fcns->clear_rectangle_depth 		= (DtPFI) ddr_x11_titang2_dcm_clear_rectangle_depth;
	fcns->clear_rectangle_depth_and_color 	= (DtPFI) ddr_x11_titang2_dcm_clear_rectangle_depth_and_color;
	fcns->set_depth_buffer_enable 		= (DtPFI) ddr_x11_titang2_dcm_set_depth_buffer_enable;
	fcns->set_depth_buffer_write 		= (DtPFI) ddr_x11_titang2_dcm_set_depth_buffer_write;
	fcns->write_scanline_byte 		= (DtPFI) ddr_x11_titang2_dcm_write_scanline_byte;
	fcns->set_current_view 			= (DtPFI) ddr_x11_titang2_dcm_set_current_view;
	fcns->inquire_pixel_data 			= (DtPFI) ddr_x11_titang2_dcm_inquire_pixel_data;

	dde_x11_titang2_DCM_fcns 			= fcns;
    }
    return (DtPtr)dde_x11_titang2_DCM_fcns;
}
/*
 ======================================================================
 */

DtPtr 
ddr_x11_titang2_dcm_create_local_data (device)
     DtPtr device;
{
    ddt_x11_titang2_data *device_data;

    if ((device_data = (DtPtr)DDspace_Allocate(sizeof (ddt_x11_titang2_data))) 
	== NULL) {
	DDerror (ERR_CANT_ALLOC_MEM_SIZE, 
		    "ddr_x11_titang2_dcm_create_local_data",
		    "sizeof (ddt_x11_titang2_data)");
	return ((DtPtr) DcNullPtr);
    }

    device_data->device = device;

    if (dde_x11_titang2_dcm_list == NULL) {
#ifdef DEBUG
	printf ("creating titang2_dcm_list\n");
#endif
	dde_x11_titang2_dcm_list = (struct list *)dor_list_create();
	if (dde_x11_titang2_dcm_list == NULL) {
	    DDerror (ERR_CANT_ALLOC_MEM_SIZE, 
			"ddr_x11_titang2_dcm_create_local_data",
			"dor_list_create");
	    return ((DtPtr) device_data);
	}
    }
#ifdef DEBUG
    printf ("adding 0x%x device to list\n", device_data);
#endif
    dor_list_add_data_last(dde_x11_titang2_dcm_list, device_data);

    return ((DtPtr) device_data);
}
/*
 ======================================================================
 */
DtFlag
ddr_x11_titang2_dcm_initialize_device (device, device_data, argstring)
     DtPtr device;
     ddt_x11_titang2_data *device_data;
     DtPtr argstring;
{
    extern int _taskno;
    XClassHint class_hints;
    XVisualInfo *visual_info, vinfo_template;
    int nvisuals;
    XSetWindowAttributes window_attributes;
    int attrMask;
    int screen;
    char *display_name;
    char *screenname;
    XSizeHints  xsh;
    XEvent event;
    XColor colors[256];
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
    char foo[10];
    unsigned long status;
    Colormap cmap;
    char *argstringcopy, *tmp, *p, *nextarg, *DDstrtok();
    static char *funcname  = "ddr_x11_titang2_dcm_initialize_device";
    int error=0;
    extern ddr_x11_titang2_dcm_cswap_error_handler();
    extern ddr_x11_titang2_dcm_restore_state();
    DtInt buffercount;
    DtInt nplanes;

    /* default values for those that might be found in the argstring */

    xsh.flags = (USPosition|USSize);
    xsh.width = xsh.height = 512;
    xsh.x = xsh.y = 0;

    device_data->device = device;
    XWINDOW(device_data) = 0;
    XUserWindow(device_data) = 0;
    XDISPLAY(device_data) = 0;
    XUserDisplay(device_data) = 0;
    XDoubleBuffered(device_data) = DcTrue;

    device_data->auto_size = DcTrue;
    device_data->visualtype = DcDirectColor;
    nplanes = 24;

    device_data->stereoswitch = DcFalse;
    device_data->stereoscreen = DcFalse;
    device_data->current_view = DcNullObject;
    device_data->previous_view = DcNullObject;

    /*
     * set defaults for Pseudo Color
     */

    device_data->shade_index = 0;

    device_data->cbtselpsweight[0] = 256. / 256.;
    device_data->cbtselpsweight[1] = 32.  / 256.;
    device_data->cbtselpsweight[2] = 4.   / 256.;
   
    device_data->cbtselpsmask[0] = 0xe0;
    device_data->cbtselpsmask[1] = 0x1c;
    device_data->cbtselpsmask[2] = 0x03;
   
    device_data->cwtavgpsweight[0] = .30;
    device_data->cwtavgpsweight[1] = .59;
    device_data->cwtavgpsweight[2] = .11;
   
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
	}	else if (!strcmp(p,"-singlebuffered")) {
	    XDoubleBuffered(device_data) = DcFalse;
	}	else if (!strcmp(p,"-window")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "window: (no value)");
		error = 1;
	    } else	{
		int w;

		XUserWindow(device_data) = 1;
		sscanf(nextarg,"%d",&w);
		XWINDOW(device_data) = (Window)w;
	    }
	}	else if (!strcmp(p,"-display"))	{
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "device: (no value)");
		error = 1;
	    }else{
		int dptr;
		XUserDisplay(device_data) = 1;
		sscanf(nextarg,"%d",&dptr);
		XDISPLAY(device_data) = (Display *)dptr;
	    }
	}	else if (!strcmp(p,"-geometry")){
	    if (!(nextarg = DDstrtok(0, " \t,"))){
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "geometry: (no value)");
		error = 1;
	    } else {
		XParseGeometry(nextarg,
			       &xsh.x,&xsh.y,&xsh.width,&xsh.height);
	    }
	}	else if (!strcmp(p, "-visualtype")) {
	    if (!(nextarg = DDstrtok(0, " \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "visualtype: (no value)");
		error = 1;
	    } else	{
		if (! strcmp("DcPseudoColor", nextarg)) {
		    device_data->visualtype = DcPseudoColor;
		    nplanes = 8;
		} else if ( ! strcmp("DcDirectColor", nextarg)) {
		    device_data->visualtype = DcDirectColor;
		    nplanes = 24;
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
	}	else if (!strcmp(p,"-stereo")) {
	    device_data->stereoswitch = DcTrue;
	}	else {
	    sprintf(DDerror_GetLine(), "flag '%s'", p);
	    DDerror (ERR_BAD_DEVICE_OPT,
			funcname, DDerror_GetLine());
	    error = 1;
	}
    }

    DDspace_Deallocate(argstringcopy);

    if (error) {
	return(DcFalse);
    }

    if (!XDISPLAY(device_data)) {
	if ((display_name = DDgetenv("DISPLAY")) == NULL) {
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, "Display Environment variable not set");
	    return DcFalse;
	}
	
	if ((XDISPLAY(device_data) = XOpenDisplay(display_name)) == NULL) {
	    sprintf(DDerror_GetLine(),
		    "Could not open display %s",display_name);
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, DDerror_GetLine());
	    return DcFalse;
	}
    }
		
    /* get some info from user's window */

    if (XWINDOW(device_data))	{
	unsigned short nBuffers, garbage;
	unsigned char stereo_window;
	XWindowAttributes attributes;

	XBQueryBuffers(XDISPLAY(device_data),XWINDOW(device_data),&nBuffers,
		       &stereo_window,&garbage,&garbage,&garbage);
	XDoubleBuffered(device_data) = (nBuffers > 1);
	if (stereo_window) {
	    device_data->stereoswitch = DcTrue;
	    device_data->stereoscreen = DcTrue;
	}
	XGetWindowAttributes(XDISPLAY(device_data),XWINDOW(device_data),
			     &attributes);
	if (ScreenOfDisplay(XDISPLAY(device_data), 1)  == attributes.screen) {
	    /*
	     * the window is on the stereo screen
	     */
	    device_data->stereoscreen = DcTrue;
	}
    }  else			/* open xwindow for the user */ {
	if (!device_data->stereoswitch) {
	    screen = XDefaultScreen(XDISPLAY(device_data));
	} else {
	    screen = 1;
	}

	if (screen == 1) {
	    device_data->stereoscreen = DcTrue;
	}

	vinfo_template.screen = screen;
	vinfo_template.depth = nplanes;
      
	if (device_data->visualtype == DcPseudoColor) {
	    vinfo_template.class = PseudoColor;
	}	else {
	    vinfo_template.class = DirectColor;
	}

	visual_info = XGetVisualInfo(XDISPLAY(device_data),
				     VisualScreenMask|VisualClassMask|VisualDepthMask,
				     &vinfo_template, &nvisuals);

	if (nvisuals == 0) {
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, 
			"screen doesn't support specified visual type");
	}

	attrMask = CWBackPixel|CWBorderPixel|CWColormap;

	if (device_data->visualtype == DcPseudoColor)	{
	    cmap = XCreateColormap(XDISPLAY(device_data),
				   XDefaultRootWindow(XDISPLAY(device_data)),visual_info->visual,AllocAll);
	    create_332_colormap(colors);
	    XStoreColors(XDISPLAY(device_data), cmap, colors, 256);
	}	else {
	    cmap = XTitanDefaultDirectColormap(XDISPLAY(device_data),screen);
	}

	window_attributes.background_pixel = 0x000000;
	window_attributes.border_pixel = 0xffffff;
	window_attributes.colormap = cmap;	

	if (XDoubleBuffered(device_data)){
	    buffercount = 2;
	} else {
	    buffercount = 1;
	}

	if (!device_data->stereoswitch) {
	    XWINDOW(device_data) = XBCreateWindow(XDISPLAY(device_data),
						  DefaultRootWindow(XDISPLAY(device_data)),
						  xsh.x,xsh.y,xsh.width,xsh.height,0,
						  nplanes,InputOutput,visual_info->visual,
						  buffercount,1,attrMask,&window_attributes);
	} else {
	    XWINDOW(device_data) = XBCreateStereoWindow(XDISPLAY(device_data),
							RootWindow(XDISPLAY(device_data),1),
							xsh.x,xsh.y,xsh.width,xsh.height,0,
							nplanes,InputOutput,visual_info->visual,
							buffercount,1,attrMask,&window_attributes);
	}

	if (!XWINDOW(device_data)) {
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, "Could not create X11 window");
	    return DcFalse;
	}

	/*  Set the standard properties for the window managers.  */
      
	XStoreName(XDISPLAY(device_data),XWINDOW(device_data),"Dore Window");
	XSetIconName(XDISPLAY(device_data),XWINDOW(device_data),"Dore Window");
	XSetWMHints(XDISPLAY(device_data),XWINDOW(device_data),&xwmh);
	XSetSizeHints(XDISPLAY(device_data),XWINDOW(device_data),&xsh,XA_WM_NORMAL_HINTS);
	class_hints.res_name = "dore";
	class_hints.res_class = "Dore";
	XSetClassHint(XDISPLAY(device_data),XWINDOW(device_data),&class_hints);
    }

    XGC(device_data) = XCreateGC(XDISPLAY(device_data),XWINDOW(device_data),0,0);

    if (!XGC(device_data)) {
	DDerror (ERR_DEVICE_SYSTEM_ACCESS,
		    funcname, "Could not create graphics context");
	return DcFalse;
    }

    XClipRects(device_data) = NULL;
    device_data->clearClipRects = NULL;

    if (XDoubleBuffered(device_data)) {
	XDrawBuffer(device_data) = 1; 
	XBSetDrawBuffer(XDISPLAY(device_data),XWINDOW(device_data),
			XDrawBuffer(device_data),XBLeftEye);
	XBSetDisplayBuffer(XDISPLAY(device_data),XWINDOW(device_data),
			   XDisplayBuffer(device_data));
    } else {
	XDrawBuffer(device_data) = 0; 
    }

    /* Make sure we catch our own expose event */

    if (!XUserWindow(device_data)) {
	XSelectInput(XDISPLAY(device_data),XWINDOW(device_data),ExposureMask);
      
	XMapWindow(XDISPLAY(device_data),XWINDOW(device_data));

	while (XNextEvent(XDISPLAY(device_data),&event)) {
	    if (event.type == Expose) 
		    break;
	}
    }

    XGetWindowAttributes(XDISPLAY(device_data),XWINDOW(device_data),
			 &XWINDOWATTRIBUTES(device_data));

    XIMAGE(device_data) = XCreateImage(XDISPLAY(device_data),
				       XWINDOWATTRIBUTES(device_data).visual,
				       XWINDOWATTRIBUTES(device_data).depth,
				       ZPixmap,0,foo,1,1,32,0);

    XClearWindow(XDISPLAY(device_data),XWINDOW(device_data));

    if (!dde_x11_titang2_tg_open_count) {
	dde_x11_titang2_tigr_state.color_index = 0;
	dde_x11_titang2_tigr_state.z_function = 1;
	dde_x11_titang2_tigr_state.control = 0;
	dde_x11_titang2_tigr_state.pixel_function = 0;
	dde_x11_titang2_tigr_state.pixel_clip = 1;
	dde_x11_titang2_tigr_state.vector_pattern = 0;
	dde_x11_titang2_tigr_state.fg_r = 255;
	dde_x11_titang2_tigr_state.fg_g = 255;
	dde_x11_titang2_tigr_state.fg_b = 255;
	dde_x11_titang2_tigr_state.bg_r = 0;
	dde_x11_titang2_tigr_state.bg_g = 0;
	dde_x11_titang2_tigr_state.bg_b = 0;
	dde_x11_titang2_tigr_state.mask_r = 0xffff;
	dde_x11_titang2_tigr_state.mask_g = 0xffff;
	dde_x11_titang2_tigr_state.mask_b = 0xffff;

	screenname = (char *)strchr (DisplayString(XDISPLAY(device_data)), ':');
	if (screenname == NULL) 
		screenname = "0";
	else 
		screenname++;

	if (doe_system_main_processors > 1) {
	    for (_taskno=doe_system_main_processors;--_taskno>=0;) {
		dde_x11_titang2_tg_fd=tg_open(0,screenname, 0);
	    }
	    _taskno=0;

	    dor_doresys_titan_setup_threads(doe_system_main_processors);
	} else {
	    sysmips(SFORCE_NORUN,sysmips(SBOOT_CPU));
	    dde_x11_titang2_tg_fd = tg_open(0,screenname, 0);
	}
    }
    dde_x11_titang2_tg_open_count++;

    /*  Check if expansion board is available.  */

    if (VendorRelease(XDISPLAY(device_data)) >= 211) {
	ExpansionBoard(device_data) =
		XTitanGetExpansionStatus(XDISPLAY(device_data), 
					 device_data->stereoscreen ? 1 : 0);
    } else {
	ExpansionBoard(device_data) = -1;
    }
    if (ExpansionBoard(device_data) == -1) {
	if (ioctl(dde_x11_titang2_tg_fd,TGIOSBUS,&status) < 0) {
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, "can't get status bit");
	    return DcFalse;
	}
	ExpansionBoard(device_data) = ((status & 0x1000) != 0);
    }

    if (ExpansionBoard(device_data)) {
	dde_x11_titang2_tigr_state.select = RC_SELECT_ALL;
    } else {
	dde_x11_titang2_tigr_state.select = RC_SELECT_R|RC_SELECT_Z;
    }

    tg_handler(0,0,ddr_x11_titang2_dcm_cswap_error_handler);

    tg_restore_state(0, 0, ddr_x11_titang2_dcm_restore_state);

    dde_x11_titang2_current_dcm_data = device_data;

    ddr_x11_titang2_dcm_restore_state();

    if (ExpansionBoard(device_data)) {
	if (doe_system_main_processors > 1) {
	    for (_taskno=doe_system_main_processors;--_taskno>=0;) {
		tg_ras_sel(RC_SELECT_ALL);
	    }
	    _taskno=0;
	} else {
	    tg_ras_sel(RC_SELECT_ALL);
	}
    } else {
	if (doe_system_main_processors > 1) {
	    for (_taskno=doe_system_main_processors;--_taskno>=0;) {
		tg_ras_sel(RC_SELECT_R|RC_SELECT_Z);
	    }
	    _taskno=0;
	} else {
	    tg_ras_sel(RC_SELECT_R|RC_SELECT_Z);
	}
    }

    if (device_data->visualtype == DcDirectColor) {
	tg_set_pseudo_flag(0);
    } else if (device_data->visualtype == DcPseudoColor) {
	tg_set_pseudo_flag(1);
    }

    dde_x11_titang2_current_dcm_data->bad_geom = DcTrue;

    ddr_x11_titang2_dcm_update_geometry();

#ifdef DEBUG
    printf ("stereoscreen = %s\nstereodevice = %s\n",
	    device_data->stereoscreen ? "DcTrue" : "DcFalse",
	    device_data->stereoswitch ? "DcTrue" : "DcFalse");
    printf ("expansion Board = %d\n", ExpansionBoard(device_data));
#endif
    return DcTrue;
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
void
ddr_x11_titang2_dcm_close_device ()
{
    extern ddr_x11_titang2_par_tg_flush();
    unsigned int color;
    int x, h;
    ddt_x11_titang2_data *device;
    ddt_x11_titang2_data *dor_list_get_first_data();
    ddt_x11_titang2_data *dor_list_get_next_data();

    /* restore graphics state */

    dde_x11_titang2_tg_open_count--;

    if (!dde_x11_titang2_tg_open_count) {
#ifdef DEBUG
	printf ("closing tigr\n");
#endif

#ifdef notdef
	/* NOTE: this code is to clear overlay planes? if 
	   taking advantage of only double buffered window on 
	   screen */

	h = XExtent(dde_x11_titang2_current_dcm_data).y2 -
		XExtent(dde_x11_titang2_current_dcm_data).y1;

	tg_ras_sel(RC_SELECT_R);
	tg_set_mod(0);
	tg_reg_write(RC_REG_PX_FUNC,0);
	tg_reg_write(RC_REG_F0,0x0);
	color = 0x30;
	tg_reg_write(RC_REG_IMG_MSK,color<<8);

	x = (XExtent(dde_x11_titang2_current_dcm_data).x1/5) * 5;

	/* round up to next superpixel */
   
	if (XExtent(dde_x11_titang2_current_dcm_data).x1 % 5) {
	    x += 5;
	}

	tg_rect(0x800|x,0x800|XExtent(dde_x11_titang2_current_dcm_data).y1,0x1,h);

	x = (XExtent(dde_x11_titang2_current_dcm_data).x2/5) * 5;

	if (XExtent(dde_x11_titang2_current_dcm_data).x2) {
	    x -= 5;
	}

	if (x < 1280) {
	    tg_rect(0x800|x,0x800|XExtent(dde_x11_titang2_current_dcm_data).y1,0x1,h);
	}

	if (ExpansionBoard(dde_x11_titang2_current_dcm_data)) {
	    tg_ras_sel(RC_SELECT_ALL);
	} else {
	    tg_ras_sel(RC_SELECT_R|RC_SELECT_Z);
	}

	tg_reg_write(RC_REG_IMG_MSK,0xffff);

#endif
    }

    if (doe_system_main_processors > 0) {
	ddr_x11_titang2_par_tg_flush();
    } else {
	tgi_cswap();		/*tg_flush();*/
    }

    XFlush(XDISPLAY(dde_x11_titang2_current_dcm_data));

    if (!XUserWindow(dde_x11_titang2_current_dcm_data)) {
	XUnmapWindow(XDISPLAY(dde_x11_titang2_current_dcm_data),
		     XWINDOW(dde_x11_titang2_current_dcm_data));
	XDestroyWindow(XDISPLAY(dde_x11_titang2_current_dcm_data),
		       XWINDOW(dde_x11_titang2_current_dcm_data));
    }

    XFreeGC(XDISPLAY(dde_x11_titang2_current_dcm_data),
	    XGC(dde_x11_titang2_current_dcm_data));

    if (!XUserDisplay(dde_x11_titang2_current_dcm_data)) {
	XCloseDisplay(XDISPLAY(dde_x11_titang2_current_dcm_data));
    }
    if (!dde_x11_titang2_tg_open_count) {
	tg_close();
    }

#ifdef DEBUG
    printf ("cls: local data 0x%x\n", dde_x11_titang2_current_dcm_data);
#endif
    if (dde_x11_titang2_dcm_list != (struct list *)0) {
	for (device = dor_list_get_first_data(dde_x11_titang2_dcm_list);
	     device != (ddt_x11_titang2_data *)0;
	     device = dor_list_get_next_data (dde_x11_titang2_dcm_list)) {
	    if (device == dde_x11_titang2_current_dcm_data) {
#ifdef DEBUG
		printf ("deleting current node for data 0x%x\n", device);
#endif
		dor_list_remove_current_node(dde_x11_titang2_dcm_list);
		break;
	    }
	}
    }
    DDspace_Deallocate(dde_x11_titang2_current_dcm_data);
    dde_x11_titang2_current_dcm_data = NULL;
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_flush ()
{
    extern ddr_x11_titang2_par_tg_flush();
    int processorno;
    extern int tg_fdh[4];

    XFlush(XDISPLAY(dde_x11_titang2_current_dcm_data));

    if (doe_system_main_processors > 0) {
	ddr_x11_titang2_par_tg_flush();

	for (processorno=0;
	     processorno<doe_system_main_processors;
	     processorno++) {
	    ioctl(tg_fdh[processorno],TGIOLOOK,0);
	}
    } else {
	tgi_cswap();		/* tg_flush();*/
      
	ioctl(dde_x11_titang2_tg_fd,TGIOLOOK,0);
    }
}
/*
  ======================================================================
 */
ddr_x11_titang2_par_tg_flush()
{
    extern int _taskno;

    for (_taskno=0;_taskno<doe_system_main_processors;_taskno++)	{
	tgi_cswap();		/*tg_flush();*/
    }

    _taskno = 0;
}
/*
 ======================================================================
 */
/* SPECIAL: tgwait problem fixed */



void
ddr_x11_titang2_dcm_swap_buffers ()
{
    extern ddr_x11_titang2_par_swap_buffers_flush_lock_and_drain();
    extern int _taskno;

    if (doe_system_main_processors > 0) {
	if (XDoubleBuffered(dde_x11_titang2_current_dcm_data)) {
	    int nminor, minors[5];

	    if (DBMODE(dde_x11_titang2_current_dcm_data) != XDBBitSwap) {
		dde_x11_titang2_thread_lock++;
	    }

	    ddr_x11_titang2_par_swap_buffers_flush_lock_and_drain();

	    if (dde_x11_titang2_current_dcm_data->bad_geom) {
		return;
	    }

	    /*  All threads have now completed drawing, and have been cocked
		(waiting for the next vertical retrace and therefore
		double buffer swap to happen),  */

	    XDrawBuffer(dde_x11_titang2_current_dcm_data) =
		    XDisplayBuffer(dde_x11_titang2_current_dcm_data);
	
	    if (DBMODE(dde_x11_titang2_current_dcm_data) == XDBBitSwap) {
		ioctl(dde_x11_titang2_tg_fd,TGIOXCOND,0);
	    } else {
		/*  Request server to match locks on all threads
		    (minor devices).  */
	
		nminor = tg_get_minor_list(minors);
	    
		XTitanSetDisplayBufferList(XDISPLAY(dde_x11_titang2_current_dcm_data),
					   XWINDOW(dde_x11_titang2_current_dcm_data),
					   XDisplayBuffer(dde_x11_titang2_current_dcm_data),
					   nminor,minors,dde_x11_titang2_thread_lock);
		XBSetDrawBuffer(XDISPLAY(dde_x11_titang2_current_dcm_data),
				XWINDOW(dde_x11_titang2_current_dcm_data),
				XDrawBuffer(dde_x11_titang2_current_dcm_data),
				XBLeftEye);
				       
	    }
	} else {
	    ddr_x11_titang2_par_swap_buffers_flush_lock_and_drain();

	    /* all threads have now completed drawing */
	}
    } else {
	if (XDoubleBuffered(dde_x11_titang2_current_dcm_data)) {
	    /* double buffer case */

	    tgi_cswap();

	    if (dde_x11_titang2_current_dcm_data->bad_geom) {
		return;
	    }

	    XDrawBuffer(dde_x11_titang2_current_dcm_data) =
		    XDisplayBuffer(dde_x11_titang2_current_dcm_data);
	 
	    if (DBMODE(dde_x11_titang2_current_dcm_data) == XDBBitSwap) {
		ioctl(dde_x11_titang2_tg_fd,TGIOLOOK,0);

		ioctl(dde_x11_titang2_tg_fd,TGIOXCOND,0);
	    } else {
		/*  Request server to match lock.  */

		dde_x11_titang2_thread_lock++;

		tg_drain();
		tg_dbwait(dde_x11_titang2_thread_lock);

		XTitanSetDisplayBuffer(XDISPLAY(dde_x11_titang2_current_dcm_data),
				       XWINDOW(dde_x11_titang2_current_dcm_data),
				       XDisplayBuffer(dde_x11_titang2_current_dcm_data),
				       tg_get_minor(),dde_x11_titang2_thread_lock);
		XBSetDrawBuffer(XDISPLAY(dde_x11_titang2_current_dcm_data),
				XWINDOW(dde_x11_titang2_current_dcm_data),
				XDrawBuffer(dde_x11_titang2_current_dcm_data),
				XBLeftEye);
	    }
	} else {
	    tgi_cswap();	/*tg_flush();*/

	    tg_drain();
	}
    }
}
/*
  ======================================================================
 */
ddr_x11_titang2_par_swap_buffers_flush_lock_and_drain()
{
    extern int _taskno;

    for (_taskno=0;_taskno<doe_system_main_processors;_taskno++)	{
	/*  Make sure all drawing commands are sent to the command list.  */

	tgi_cswap();		/*tg_flush();*/

	/*  Wait for the kernel to process all of the cmds for this thread.  */

	tg_drain();

	/* add a request to rendezvous */
	/* double-buffered case */

	if (dde_x11_titang2_thread_lock>1 &&
	    !dde_x11_titang2_current_dcm_data->bad_geom) {
	    tg_dbwait(dde_x11_titang2_thread_lock);
	}

    }

    _taskno = 0;
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_update_geometry ()
{
    DtReal *p;
    DtReal *clear_p;
    XBoxPtr pBox, pBoxBase;
    DtInt processorno;
    extern int tg_fdh[4];
    int nbox;
    int height;
    int width;

    dde_x11_titang2_current_dcm_data->bad_geom = DcFalse;

    pBoxBase = XTitanGetClipList(XDISPLAY(dde_x11_titang2_current_dcm_data),
				 XWINDOW(dde_x11_titang2_current_dcm_data),
				 XGC(dde_x11_titang2_current_dcm_data),
				 &XNRECTS(dde_x11_titang2_current_dcm_data),
				 &XSEQUENCENO(dde_x11_titang2_current_dcm_data),
				 &XExtent(dde_x11_titang2_current_dcm_data),
				 &DBMODE(dde_x11_titang2_current_dcm_data));

    DBMODE(dde_x11_titang2_current_dcm_data) = XDBServerSwap;

    if (doe_system_main_processors > 0) {
	for (processorno=0;
	     processorno<doe_system_main_processors;
	     processorno++) {
	    ioctl(tg_fdh[processorno],TGIOGEOMUSE,
		  XSEQUENCENO(dde_x11_titang2_current_dcm_data));
	    ioctl(tg_fdh[processorno],TGIOCLRERR);
	}
    } else {
	ioctl(dde_x11_titang2_tg_fd,TGIOGEOMUSE,
	      XSEQUENCENO(dde_x11_titang2_current_dcm_data));
	ioctl(dde_x11_titang2_tg_fd,TGIOCLRERR);
    }

    nbox = XNRECTS(dde_x11_titang2_current_dcm_data);
    pBox = pBoxBase;

    if (nbox == 0) {
	return;
    }

    p = XClipRects(dde_x11_titang2_current_dcm_data);
    clear_p = dde_x11_titang2_current_dcm_data->clearClipRects;

    if (p) {
	p = (DtReal *)DDspace_Reallocate(p, 4*nbox*sizeof(DtReal));
    } else {
	p = (DtReal *)DDspace_Allocate(4*nbox*sizeof(DtReal));
    }

    if (clear_p) {
	clear_p = (DtReal *)DDspace_Reallocate(clear_p, 4*nbox*sizeof(DtReal));
    } else {
	clear_p = (DtReal *)DDspace_Allocate(4*nbox*sizeof(DtReal));
    }


    XClipRects(dde_x11_titang2_current_dcm_data) = p;
    dde_x11_titang2_current_dcm_data->clearClipRects = clear_p;

    width = WidthOfScreen(XWINDOWATTRIBUTES(dde_x11_titang2_current_dcm_data).screen);
    height = HeightOfScreen(XWINDOWATTRIBUTES(dde_x11_titang2_current_dcm_data).screen);

    while (nbox--) {
	p[0] = ((max(pBox->x1,1)+4)/5)*5;
	clear_p[0] = max(pBox->x1,1);

	p[1] = max(pBox->y1,1);
	clear_p[1] = p[1];

	p[2] = (min(pBox->x2,width-2)/5)*5;
	clear_p[2] = min(pBox->x2,width-2);
      
	p[3] = min(pBox->y2,height-2);
	clear_p[3] = min(pBox->y2,height-2);

	p += 4;
	clear_p += 4;
	pBox++;
    }

    if (pBoxBase) {
	XFree(pBoxBase);
    }

    if (XNRECTS(dde_x11_titang2_current_dcm_data) == 1) {
	clear_p = dde_x11_titang2_current_dcm_data->clearClipRects;
	dde_x11_titang2_current_dcm_data->partially_obscure = 
		!(((clear_p)[0] == 
		   (DtReal)XExtent(dde_x11_titang2_current_dcm_data).x1) &&
		  ((clear_p)[1] == 
		   (DtReal)XExtent(dde_x11_titang2_current_dcm_data).y1) &&
		  ((clear_p)[2] == 
		   (DtReal)XExtent(dde_x11_titang2_current_dcm_data).x2) &&
		  ((clear_p)[3] == 
		   (DtReal)XExtent(dde_x11_titang2_current_dcm_data).y2));
    } else {
	dde_x11_titang2_current_dcm_data->partially_obscure = DcTrue;
    }

#ifdef DEBUG
    printf ("partially obscure = %d\n", 
	    dde_x11_titang2_current_dcm_data->partially_obscure);
    printf ("XExtent: (%d %d) (%d %d)\n",
	    XExtent(dde_x11_titang2_current_dcm_data).x1,
	    XExtent(dde_x11_titang2_current_dcm_data).y1,
	    XExtent(dde_x11_titang2_current_dcm_data).x2,
	    XExtent(dde_x11_titang2_current_dcm_data).y2);
#endif
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_become_current_driver (device_data)
     ddt_x11_titang2_data *device_data;
{
    dde_x11_titang2_current_dcm_data = device_data;
}
/*
 ======================================================================
 */

void
ddr_x11_titang2_dcm_setup_shading()
{
    DtInt range_min, range_max;

    dde_x11_titang2_current_dcm_data->shade_mode = 
	    DdInqShadeMode (dde_x11_titang2_current_dcm_data->device);
    if (dde_x11_titang2_current_dcm_data->shade_mode == DcRange) {
	DDdevice_InqShadeRange (dde_x11_titang2_current_dcm_data->device, 
			     dde_x11_titang2_current_dcm_data->shade_index,
			     &range_min, &range_max);
	dde_x11_titang2_current_dcm_data->cwtavgpslim[0] = (DtReal)range_min;
	dde_x11_titang2_current_dcm_data->cwtavgpslim[1] = (DtReal)range_max;
	dde_x11_titang2_current_dcm_data->cwtavgpslim[2] = 
		(dde_x11_titang2_current_dcm_data->cwtavgpslim[1]-dde_x11_titang2_current_dcm_data->cwtavgpslim[0])/255.;
    }
}
/*
 ======================================================================
 */

ddr_x11_titang2_dcm_cswap_error_handler(tg_cbm,tg_cbs,class,str,x)
     struct cbmem *tg_cbm;
     struct cbsub *tg_cbs;
     int class;
     char *str;
     va_list x;
{
    ddt_x11_titang2_data *device;
    ddt_x11_titang2_data *dor_list_get_first_data();
    ddt_x11_titang2_data *dor_list_get_next_data();

    tg_discard_cmdq();

    if (dde_x11_titang2_dcm_list != (struct list *)0) {
	for (device = dor_list_get_first_data(dde_x11_titang2_dcm_list);
	     device != (ddt_x11_titang2_data *)0;
	     device = dor_list_get_next_data (dde_x11_titang2_dcm_list)) {
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
	DDdevice_SetAbortedDraw(dde_x11_titang2_current_dcm_data->device,DcTrue);
	dde_x11_titang2_current_dcm_data->bad_geom = DcTrue;
	dde_x11_titang2_current_dcm_data->current_view = DcNullObject;
    }
}
/*
 ======================================================================
 */
ddr_x11_titang2_dcm_restore_state()
{
    /* set current z function */

#ifdef DEBUG
    printf ("restoring tigr state\n");
    printf ("   RC_REG_Z_FUNC:  %d\n", dde_x11_titang2_tigr_state.z_function);
    printf ("   mod:            %d\n", dde_x11_titang2_tigr_state.control);
    printf ("   RC_REG_PX_FUNC: %d\n", dde_x11_titang2_tigr_state.pixel_function);
    if (ExpansionBoard(dde_x11_titang2_current_device_data)) {
	printf ("   --- Expansion Board---\n");
	printf ("   foreground      (%d %d %d)\n",
		dde_x11_titang2_tigr_state.fg_r,
		dde_x11_titang2_tigr_state.fg_g,
		dde_x11_titang2_tigr_state.fg_b);
	printf ("   background      (%d %d %d)\n",
		dde_x11_titang2_tigr_state.bg_r,
		dde_x11_titang2_tigr_state.bg_g,
		dde_x11_titang2_tigr_state.bg_b);
	printf ("   color index     %d\n",
		dde_x11_titang2_tigr_state.color_index);
	printf ("   red mask:       0x%x\n",
		dde_x11_titang2_tigr_state.mask_r);
	printf ("   green mask:     0x%x\n",
		dde_x11_titang2_tigr_state.mask_g);
	printf ("   blue mask:      0x%x\n",
		dde_x11_titang2_tigr_state.mask_b);
	printf ("   RC_REG_VEC0     %d\n",
		dde_x11_titang2_tigr_state.vector_pattern);
    } else {
	printf ("   --- No Expansion Board---\n");
	printf ("   color index     %d\n",
		dde_x11_titang2_tigr_state.color_index);
	printf ("   red mask:       0x%x\n",
		dde_x11_titang2_tigr_state.mask_r);
	printf ("   RC_REG_VEC0     %d\n",
		dde_x11_titang2_tigr_state.vector_pattern);
    }
    printf ("  select           0x%x\n", 
	    dde_x11_titang2_tigr_state.select);
#endif

    tg_ras_sel(RC_SELECT_Z);

    tg_reg_write(RC_REG_Z_FUNC, dde_x11_titang2_tigr_state.z_function);

    /* select rasterizer's */

    if (ExpansionBoard(dde_x11_titang2_current_dcm_data)) {
	tg_ras_sel(RC_SELECT_FULL);

	/* set control word */

	tg_set_mod(dde_x11_titang2_tigr_state.control);

	/* set pixel function */

	tg_reg_write(RC_REG_PX_FUNC, dde_x11_titang2_tigr_state.pixel_function);

	tg_foreground(dde_x11_titang2_tigr_state.fg_r,
		      dde_x11_titang2_tigr_state.fg_g,
		      dde_x11_titang2_tigr_state.fg_b);
	tg_background(dde_x11_titang2_tigr_state.bg_r,
		      dde_x11_titang2_tigr_state.bg_g,
		      dde_x11_titang2_tigr_state.bg_b);
	tg_ras_sel(RC_SELECT_R);
	tg_reg_write(RC_REG_IMG_MSK,dde_x11_titang2_tigr_state.mask_r);
	tg_reg_write(RC_REG_F0, dde_x11_titang2_tigr_state.color_index);
	tg_ras_sel(RC_SELECT_G);
	tg_reg_write(RC_REG_IMG_MSK,dde_x11_titang2_tigr_state.mask_g);
	tg_ras_sel(RC_SELECT_B);
	tg_reg_write(RC_REG_IMG_MSK,dde_x11_titang2_tigr_state.mask_b);
	tg_ras_sel(RC_SELECT_FULL);

	tg_reg_write(RC_REG_VEC0, dde_x11_titang2_tigr_state.vector_pattern);

    } else {
	tg_ras_sel(RC_SELECT_R);

	/* set control word */

	tg_set_mod(dde_x11_titang2_tigr_state.control);

	/* set pixel function */

	tg_reg_write(RC_REG_PX_FUNC, dde_x11_titang2_tigr_state.pixel_function);

	tg_reg_write(RC_REG_F0, dde_x11_titang2_tigr_state.color_index);

	tg_reg_write(RC_REG_IMG_MSK,dde_x11_titang2_tigr_state.mask_r);

	tg_reg_write(RC_REG_VEC0, dde_x11_titang2_tigr_state.vector_pattern);
    }

    tg_ras_sel(dde_x11_titang2_tigr_state.select);
}
/*
 ======================================================================
 */
