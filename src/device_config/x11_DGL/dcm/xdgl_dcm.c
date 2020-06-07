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
	DtPtr ddr_x11_DGL_return_DCM_fcns ()
	DtPtr ddr_x11_DGL_dcm_create_local_data (device)
	DtFlag ddr_x11_DGL_dcm_initialize_device (device, device_data, argstring)
	static create_332_colormap (colors)
	void ddr_x11_DGL_dcm_close_device ()
	void ddr_x11_DGL_dcm_flush ()
	void ddr_x11_DGL_dcm_swap_buffers ()
	void ddr_x11_DGL_dcm_update_geometry ()
	void ddr_x11_DGL_dcm_become_current_driver (device_data)
	void ddr_x11_DGL_dcm_setup_shading ()
	int ddr_x11_DGL_dcm_cswap_error_handler (error_code, pDraw)

  ======================================================================
 */
#include "dcm.h"
#include <sys/sysmips.h>

ddt_x11_DGL_data *dde_x11_DGL_current_dcm_data;

int dde_x11_DGL_user_initialized = 0;
int dde_x11_DGL_open_count = 0;

struct list *dde_x11_DGL_dcm_list = NULL;
/*
 ======================================================================
 */
extern DtPtr ddr_x11_DGL_dcm_create_local_data();
extern DtFlag ddr_x11_DGL_dcm_initialize_device();
extern void ddr_x11_DGL_dcm_become_current_driver();
extern void ddr_x11_DGL_dcm_close_device();
extern void ddr_x11_DGL_dcm_flush();
extern void ddr_x11_DGL_dcm_swap_buffers();
extern void ddr_x11_DGL_dcm_update_geometry();
extern void ddr_x11_DGL_dcm_inquire_device_extent();
extern void ddr_x11_DGL_dcm_inquire_stereo();
extern void ddr_x11_DGL_dcm_inquire_ncolors();
extern void ddr_x11_DGL_dcm_inquire_resolution();
extern void ddr_x11_DGL_dcm_inquire_visual_type();
extern void ddr_x11_DGL_dcm_inquire_auto_size();
extern void ddr_x11_DGL_dcm_inquire_color_entries();
extern void ddr_x11_DGL_dcm_set_color_entries();
extern void ddr_x11_DGL_dcm_set_foreground_color();
extern void ddr_x11_DGL_dcm_set_background_color();
extern void ddr_x11_DGL_dcm_set_shade_index();
extern void ddr_x11_DGL_dcm_inquire_clip_list();
extern void ddr_x11_DGL_dcm_clear_rectangle_depth();
extern void ddr_x11_DGL_dcm_clear_rectangle_depth_and_color();
extern void ddr_x11_DGL_dcm_set_depth_buffer_enable();
extern void ddr_x11_DGL_dcm_set_depth_buffer_write();
extern void ddr_x11_DGL_dcm_write_scanline_byte();
extern void ddr_x11_DGL_dcm_set_current_view();
extern DtFlag ddr_x11_DGL_dcm_inquire_pixel_data();

DDt_DCM *dde_x11_DGL_DCM_fcns = (DDt_DCM *)0;

/*
 ======================================================================
 */
DtPtr
ddr_x11_DGL_return_DCM_fcns()
{
    DDt_DCM *fcns;

    if (dde_x11_DGL_DCM_fcns == (DDt_DCM *)0) {
	fcns = DDdevice_CreateDCMStruct();

	if (fcns == (DDt_DCM *)0) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_x11_DGL_return_DCM_fcns",
			DcNullPtr);
	}

	fcns->version_number 			= 1;
	/* Version 1 Functions */
	fcns->create_local_data 		= (DtPFI) ddr_x11_DGL_dcm_create_local_data;
	fcns->initialize_device 		= (DtPFI) ddr_x11_DGL_dcm_initialize_device;
	fcns->become_current_driver 		= (DtPFI) ddr_x11_DGL_dcm_become_current_driver;
	fcns->close_device			= (DtPFI) ddr_x11_DGL_dcm_close_device;
	fcns->flush 				= (DtPFI) ddr_x11_DGL_dcm_flush;
	fcns->swap_buffers 			= (DtPFI) ddr_x11_DGL_dcm_swap_buffers;
	fcns->update_geometry 			= (DtPFI) ddr_x11_DGL_dcm_update_geometry;
	fcns->inquire_device_extent 		= (DtPFI) ddr_x11_DGL_dcm_inquire_device_extent;
	fcns->inquire_stereo 			= (DtPFI) ddr_x11_DGL_dcm_inquire_stereo;
	fcns->inquire_ncolors 			= (DtPFI) ddr_x11_DGL_dcm_inquire_ncolors;
	fcns->inquire_resolution 		= (DtPFI) ddr_x11_DGL_dcm_inquire_resolution;
	fcns->inquire_visual_type 		= (DtPFI) ddr_x11_DGL_dcm_inquire_visual_type;
	fcns->inquire_auto_size 		= (DtPFI) ddr_x11_DGL_dcm_inquire_auto_size;
	fcns->inquire_color_entries 		= (DtPFI) ddr_x11_DGL_dcm_inquire_color_entries;
	fcns->set_color_entries 		= (DtPFI) ddr_x11_DGL_dcm_set_color_entries;
	fcns->set_foreground_color 		= (DtPFI) ddr_x11_DGL_dcm_set_foreground_color;
	fcns->set_background_color 		= (DtPFI) ddr_x11_DGL_dcm_set_background_color;
	fcns->set_shade_index 			= (DtPFI) ddr_x11_DGL_dcm_set_shade_index;
	fcns->inquire_clip_list 		= (DtPFI) ddr_x11_DGL_dcm_inquire_clip_list;
	fcns->clear_rectangle_depth 		= (DtPFI) ddr_x11_DGL_dcm_clear_rectangle_depth;
	fcns->clear_rectangle_depth_and_color 	= (DtPFI) ddr_x11_DGL_dcm_clear_rectangle_depth_and_color;
	fcns->set_depth_buffer_enable 		= (DtPFI) ddr_x11_DGL_dcm_set_depth_buffer_enable;
	fcns->set_depth_buffer_write 		= (DtPFI) ddr_x11_DGL_dcm_set_depth_buffer_write;
	fcns->write_scanline_byte 		= (DtPFI) ddr_x11_DGL_dcm_write_scanline_byte;
	fcns->set_current_view 			= (DtPFI) ddr_x11_DGL_dcm_set_current_view;
	fcns->inquire_pixel_data 			= (DtPFI) ddr_x11_DGL_dcm_inquire_pixel_data;

	dde_x11_DGL_DCM_fcns 			= fcns;
    }
    return (DtPtr)dde_x11_DGL_DCM_fcns;
}
/*
 ======================================================================
 */
DtPtr 
ddr_x11_DGL_dcm_create_local_data (device)
	DtPtr device;
{
    ddt_x11_DGL_data *device_data;

    if ((device_data = (DtPtr)DDspace_Allocate(sizeof (ddt_x11_DGL_data))) 
	== NULL) {
	DDerror (ERR_CANT_ALLOC_MEM_SIZE, 
		    "ddr_x11_DGL_dcm_create_local_data",
		    "sizeof (ddt_x11_DGL_data)");
	return ((DtPtr) DcNullPtr);
    }

    device_data->device = device;

    if (dde_x11_DGL_dcm_list == NULL) {
#ifdef DEBUG
	printf ("creating DGL_dcm_list\n");
#endif
	dde_x11_DGL_dcm_list = (struct list *)dor_list_create();
	if (dde_x11_DGL_dcm_list == NULL) {
	    DDerror (ERR_CANT_ALLOC_MEM_SIZE, 
			"ddr_x11_DGL_dcm_create_local_data",
			"dor_list_create");
	    return ((DtPtr) device_data);
	}
    }
#ifdef DEBUG
    printf ("adding 0x%x device to list\n", device_data);
#endif
    dor_list_add_data_last(dde_x11_DGL_dcm_list, device_data);

    return ((DtPtr) device_data);
}
/*
 ======================================================================
 */
DtFlag
ddr_x11_DGL_dcm_initialize_device (device, device_data, argstring)
	DtPtr device;
	ddt_x11_DGL_data *device_data;
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
    Colormap cmap;
    char *argstringcopy, *tmp, *p, *nextarg, *DDstrtok();
    static char *funcname  = "ddr_x11_DGL_dcm_initialize_device";
    int error=0;
    extern ddr_x11_DGL_dcm_cswap_error_handler();
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
    XpDRAW(device_data) = 0;
    XUserDrawable(device_data) = 0;
    XpGC(device_data) = 0;

    device_data->auto_size = DcTrue;
    device_data->visualtype = DcDirectColor;
    nplanes = 24;

    device_data->stereoswitch = DcFalse;
    device_data->stereoscreen = DcFalse;
    device_data->current_view = DcNullObject;
    device_data->previous_view = DcNullObject;
    device_data->eye = XBLeftEye;

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
	} else if (!strcmp(p,"-windowhandle")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "windowhandle: (no value)");
		error = 1;
	    } else	{
		int w;

		XUserDrawable(device_data) = 1;
		sscanf(nextarg, "%d", &w);
		XpDRAW(device_data) = (XdDrawablePtr) w;

	    }
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
	DDspace_Deallocate(device_data);
	return(DcFalse);
    }

    if (!XDISPLAY(device_data)) {
	if ((display_name = DDgetenv("DISPLAY")) == NULL) {
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, "Display Environment variable not set");
	    DDspace_Deallocate(device_data);
	    return DcFalse;
	}
	
	if ((XDISPLAY(device_data) = XOpenDisplay(display_name)) == NULL) {
	    sprintf(DDerror_GetLine(),
		    "Could not open display %s",display_name);
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, DDerror_GetLine());
	    DDspace_Deallocate(device_data);
	    return DcFalse;
	}
    }
		
    /* get some info from user's window */

    if (XWINDOW(device_data)) {
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

	    DDspace_Deallocate(device_data);

	    if (!XUserDisplay(device_data)) {
		XCloseDisplay(XDISPLAY(device_data));
	    }
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

    if (!dde_x11_DGL_open_count) {
	if (XdQueryInitialized()) {
	    dde_x11_DGL_user_initialized = 1;
	}	else {
	    XdInitializeSystem(doe_system_main_processors);
	}

	if (doe_system_main_processors > 1) {
	    dor_doresys_titan_setup_threads(doe_system_main_processors);
	} else {
	    sysmips(SFORCE_NORUN,sysmips(SBOOT_CPU));
	}
    }
    dde_x11_DGL_open_count++;

    XGC(device_data) = XCreateGC(XDISPLAY(device_data),
				 XWINDOW(device_data),0,0);

    if (!XGC(device_data)) {
	DDerror (ERR_DEVICE_SYSTEM_ACCESS,
		    funcname, "Could not create graphics context");
	goto error_exit;
    }

    if (XpDRAW(device_data)) {
	unsigned short nBuffers;
	unsigned char stereo;
	unsigned char eye;
	unsigned short displayBuffer;
	unsigned short drawBuffer;

	XdQueryBuffers(XpDRAW(device_data), &nBuffers, &stereo, &eye,
		       &displayBuffer, &drawBuffer);

	if (stereo) {
	    device_data->stereoswitch = DcTrue;
	    device_data->stereoscreen = DcTrue;
	}

	XDoubleBuffered(device_data) = (nBuffers > 1);
	if (XDoubleBuffered(device_data)) {
	    XDrawBuffer(device_data) = drawBuffer; 
	    device_data->eye = eye;
	} else {
	    XDrawBuffer(device_data) = 0; 
	}
    } else {
	XpDRAW(device_data) = 
		(XdDrawablePtr) XdCreateWindowHandle(XDISPLAY(device_data),
						     XWINDOW(device_data));
	if (!XpDRAW(device_data)) {
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, "Could not create DGL Drawable");

	    goto error_exit;
	}

	if (XDoubleBuffered(device_data)) {
	    XDrawBuffer(device_data) = 1; 
	    XdSetDrawBuffer(XpDRAW(device_data), XDrawBuffer(device_data),
			    XBLeftEye);
	    XdSetDisplayBuffer(XpDRAW(device_data), 
			       XDisplayBuffer(device_data));
	} else {
	    XDrawBuffer(device_data) = 0; 
	}
    }

    XpGC(device_data) = 
	    (XdGCPtr) XdCreateGC(XpDRAW(device_data));
    if (!XpGC(device_data)) {
	DDerror (ERR_DEVICE_SYSTEM_ACCESS,
		    funcname, "Could not create DGL GC");

	goto error_exit;
    }

    XClipRects(device_data) = NULL;

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

    XClearWindow(XDISPLAY(device_data),XWINDOW(device_data));

    /*  Check if expansion board is available.  */

    if (VendorRelease(XDISPLAY(device_data)) >= 211) {
	ExpansionBoard(device_data) =
		XTitanGetExpansionStatus(XDISPLAY(device_data), 
					 device_data->stereoscreen ? 1 : 0);
    } else {
	DDerror (ERR_DEVICE_SYSTEM_ACCESS,
		    funcname, "can't determine if expansion board");
	goto error_exit;
    }

    XdSetErrorHandler(ddr_x11_DGL_dcm_cswap_error_handler);

    if (!ExpansionBoard(device_data)) {
	if (device_data->stereoswitch) {
	    DDerror (ERR_BAD_DEVICE_OPT,
			funcname,
			"Cannot do Stereo on Base Board");
	    goto error_exit;
	}
	if ((device_data->visualtype == DcDirectColor) &&
	    XDoubleBuffered(device_data)&&
	    !device_data->stereoscreen) {
	    DDerror (ERR_BAD_DEVICE_OPT,
			funcname, 
			"Cannot do Double Buffered True Color on Base Board");
	    goto error_exit;
	}
    }

    dde_x11_DGL_current_dcm_data = device_data;

    dde_x11_DGL_current_dcm_data->bad_geom = DcTrue;

    ddr_x11_DGL_dcm_update_geometry();

    device_data->background_color[0] = 0.;
    device_data->background_color[1] = 0.;
    device_data->background_color[2] = 0.;
    device_data->bg_color_index = 0;

    if (XWINDOWATTRIBUTES(device_data).map_state != IsUnmapped) {
	ddr_x11_DGL_dcm_clear_rectangle_depth_and_color(0, 0,
							   XExtent(device_data).x2, 
							   XExtent(device_data).y2);
	XdFlush(XpDRAW(device_data));
    }

#ifdef DEBUG
    printf ("stereoscreen = %s\nstereodevice = %s\n",
	    device_data->stereoscreen ? "DcTrue" : "DcFalse",
	    device_data->stereoswitch ? "DcTrue" : "DcFalse");
    printf ("expansion Board = %d\n", ExpansionBoard(device_data));
#endif

    return DcTrue;

 error_exit:
    if (!XUserDrawable(device_data)) {
	if (XpDRAW(device_data) != 0) {
	    XdDestroyDrawable(XpDRAW(device_data));
	}
    }

    if (XpGC(device_data) != 0) {
	XdFreeGC(XpGC(device_data));
    }

    if (!XUserWindow(device_data)) {
	if ((XWINDOW(device_data) != 0) &&
	    (XDISPLAY(device_data) != 0)) {
	    XUnmapWindow(XDISPLAY(device_data), XWINDOW(device_data));
	    XDestroyWindow(XDISPLAY(device_data), XWINDOW(device_data));
	}
    }
    if (!XUserDisplay(device_data) && XDISPLAY(device_data) != 0) {
	XCloseDisplay(XDISPLAY(device_data));
    }

    dde_x11_DGL_open_count--;
    if (!dde_x11_DGL_open_count && !dde_x11_DGL_user_initialized) {
	XdTerminateSystem();
    }

    return DcFalse;
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
ddr_x11_DGL_dcm_close_device ()
{
    ddt_x11_DGL_data *device;
    ddt_x11_DGL_data *dor_list_get_first_data();
    ddt_x11_DGL_data *dor_list_get_next_data();

    /* restore graphics state */

/*
 *  Termination and reinitialisation in MP cannot be done:  only
 *  the distinguished thread will be aware of file descriptors
 *  that were closed and reopened, and the other thread(s) will
 *  cause errors.  Solution is not to terminate until end of
 *  program execution.
    dde_x11_DGL_open_count--;
 */

    XdFlush(XpDRAW(dde_x11_DGL_current_dcm_data));

    if (!XUserWindow(dde_x11_DGL_current_dcm_data)) {
	XUnmapWindow(XDISPLAY(dde_x11_DGL_current_dcm_data),
		     XWINDOW(dde_x11_DGL_current_dcm_data));
	XDestroyWindow(XDISPLAY(dde_x11_DGL_current_dcm_data),
		       XWINDOW(dde_x11_DGL_current_dcm_data));
    }

    if (XGC(dde_x11_DGL_current_dcm_data)) {
	XFreeGC (XDISPLAY(dde_x11_DGL_current_dcm_data),
		 XGC(dde_x11_DGL_current_dcm_data));
    }

    if (!XUserDisplay(dde_x11_DGL_current_dcm_data)) {
	XCloseDisplay(XDISPLAY(dde_x11_DGL_current_dcm_data));
    }
    if (!XUserDrawable(dde_x11_DGL_current_dcm_data)) {
	XdDestroyDrawable(dde_x11_DGL_current_dcm_data->pDraw);
    }

    if (XpGC(dde_x11_DGL_current_dcm_data)) {
	XdFreeGC(XpGC(dde_x11_DGL_current_dcm_data));
    }

    if (!dde_x11_DGL_open_count && !dde_x11_DGL_user_initialized) {
#ifdef DEBUG
	printf ("closing DGL\n");
#endif
	XdTerminateSystem();
    }

#ifdef DEBUG
    printf ("cls: local data 0x%x\n", dde_x11_DGL_current_dcm_data);
#endif
    if (dde_x11_DGL_dcm_list != (struct list *)0) {
	for (device = dor_list_get_first_data(dde_x11_DGL_dcm_list);
	     device != (ddt_x11_DGL_data *)0;
	     device = dor_list_get_next_data (dde_x11_DGL_dcm_list)) {
	    if (device == dde_x11_DGL_current_dcm_data) {
#ifdef DEBUG
		printf ("deleting current node for data 0x%x\n", device);
#endif
		dor_list_remove_current_node(dde_x11_DGL_dcm_list);
		break;
	    }
	}
    }
    DDspace_Deallocate(dde_x11_DGL_current_dcm_data);
    dde_x11_DGL_current_dcm_data = NULL;
}
/*
 ======================================================================
 */

void
ddr_x11_DGL_dcm_flush ()
{
    XdFlush(XpDRAW(dde_x11_DGL_current_dcm_data));
}
/*
 ======================================================================
 */

void
ddr_x11_DGL_dcm_swap_buffers ()
{

#ifdef DEBUG
    printf ("Swapping buffers %s\n",
	    XDoubleBuffered(dde_x11_DGL_current_dcm_data) ? "Double Buffered" : "");
#endif

    XdFlush(XpDRAW(dde_x11_DGL_current_dcm_data));
    XdSynchronize();

    if (XDoubleBuffered(dde_x11_DGL_current_dcm_data)) {
	/* double buffer case */

	if (dde_x11_DGL_current_dcm_data->bad_geom) {
	    return;
	}

	XDrawBuffer(dde_x11_DGL_current_dcm_data) =
		XDisplayBuffer(dde_x11_DGL_current_dcm_data);
	 
	XdSetDisplayBuffer(XpDRAW(dde_x11_DGL_current_dcm_data),
			   XDisplayBuffer(dde_x11_DGL_current_dcm_data));
	XdSetDrawBuffer(XpDRAW(dde_x11_DGL_current_dcm_data),
			XDrawBuffer(dde_x11_DGL_current_dcm_data),
			dde_x11_DGL_current_dcm_data->eye);
    }
}
/*
 ======================================================================
 */

void
ddr_x11_DGL_dcm_update_geometry ()
{
    DtReal *p;
    XBoxPtr pBox, pBoxBase;
    int nbox;
    int height;
    int width;

    XdReset();

    dde_x11_DGL_current_dcm_data->bad_geom = DcFalse;

    pBoxBase = XTitanGetClipList(XDISPLAY(dde_x11_DGL_current_dcm_data),
				 XWINDOW(dde_x11_DGL_current_dcm_data),
				 XGC(dde_x11_DGL_current_dcm_data),
				 &XNRECTS(dde_x11_DGL_current_dcm_data),
				 &XSEQUENCENO(dde_x11_DGL_current_dcm_data),
				 &XExtent(dde_x11_DGL_current_dcm_data),
				 &DBMODE(dde_x11_DGL_current_dcm_data));

    DBMODE(dde_x11_DGL_current_dcm_data) = XDBServerSwap;

    nbox = XNRECTS(dde_x11_DGL_current_dcm_data);
    pBox = pBoxBase;

    if (nbox == 0) {
	return;
    }

    p = XClipRects(dde_x11_DGL_current_dcm_data);

    if (p) {
	p = (DtReal *)DDspace_Reallocate(p, 4*nbox*sizeof(DtReal));
    } else {
	p = (DtReal *)DDspace_Allocate(4*nbox*sizeof(DtReal));
    }

    XClipRects(dde_x11_DGL_current_dcm_data) = p;

    width = WidthOfScreen(XWINDOWATTRIBUTES(dde_x11_DGL_current_dcm_data).screen);
    height = HeightOfScreen(XWINDOWATTRIBUTES(dde_x11_DGL_current_dcm_data).screen);

    while (nbox--) {
	/*
	 * Make visible rectangles relative to window origin
	 */

	pBox->x1 -= XExtent(dde_x11_DGL_current_dcm_data).x1;
	pBox->x2 -= XExtent(dde_x11_DGL_current_dcm_data).x1;
	pBox->y1 -= XExtent(dde_x11_DGL_current_dcm_data).y1;
	pBox->y2 -= XExtent(dde_x11_DGL_current_dcm_data).y1;

	p[0] = max(pBox->x1,0);
	p[1] = max(pBox->y1,0);
	p[2] = min(pBox->x2,width-1);
	p[3] = min(pBox->y2,height-1);

	p += 4;
	pBox++;
    }

    if (pBoxBase) {
	XFree(pBoxBase);
    }

    /*
     * Make extent relative to origin of the window
     */
    XExtent(dde_x11_DGL_current_dcm_data).x2 -= 
	    XExtent(dde_x11_DGL_current_dcm_data).x1;
    XExtent(dde_x11_DGL_current_dcm_data).y2 -= 
	    XExtent(dde_x11_DGL_current_dcm_data).y1;
    XExtent(dde_x11_DGL_current_dcm_data).x1 = 0;
    XExtent(dde_x11_DGL_current_dcm_data).y1 = 0;


    if (XNRECTS(dde_x11_DGL_current_dcm_data) == 1) {
	p = XClipRects(dde_x11_DGL_current_dcm_data);
	dde_x11_DGL_current_dcm_data->partially_obscure = 
		!(((p)[0] == 
		   (DtReal)XExtent(dde_x11_DGL_current_dcm_data).x1) &&
		  ((p)[1] == 
		   (DtReal)XExtent(dde_x11_DGL_current_dcm_data).y1) &&
		  ((p)[2] == 
		   (DtReal)XExtent(dde_x11_DGL_current_dcm_data).x2) &&
		  ((p)[3] == 
		   (DtReal)XExtent(dde_x11_DGL_current_dcm_data).y2));
#ifdef DEBUG
	printf ("Clip Rect 1: (%g %g) (%g %g)\n",
		p[0], p[1], p[2], p[3]);
#endif
    } else {
	dde_x11_DGL_current_dcm_data->partially_obscure = DcTrue;
    }

#ifdef DEBUG
    printf ("partially obscure = %d\n", 
	    dde_x11_DGL_current_dcm_data->partially_obscure);
    printf ("XExtent: (%d %d) (%d %d)\n",
	    XExtent(dde_x11_DGL_current_dcm_data).x1,
	    XExtent(dde_x11_DGL_current_dcm_data).y1,
	    XExtent(dde_x11_DGL_current_dcm_data).x2,
	    XExtent(dde_x11_DGL_current_dcm_data).y2);
#endif

    XdSetViewport(XpGC(dde_x11_DGL_current_dcm_data),
		  XExtent(dde_x11_DGL_current_dcm_data).x1,
		  XExtent(dde_x11_DGL_current_dcm_data).y1,
		  XExtent(dde_x11_DGL_current_dcm_data).x2,
		  XExtent(dde_x11_DGL_current_dcm_data).y2);

}
/*
 ======================================================================
 */

void
ddr_x11_DGL_dcm_become_current_driver (device_data)
     ddt_x11_DGL_data *device_data;
{
    dde_x11_DGL_current_dcm_data = device_data;
}
/*
 ======================================================================
 */

void
ddr_x11_DGL_dcm_setup_shading()
{
    DtInt range_min, range_max;

    dde_x11_DGL_current_dcm_data->shade_mode = 
	    DdInqShadeMode (dde_x11_DGL_current_dcm_data->device);
    if (dde_x11_DGL_current_dcm_data->shade_mode == DcRange) {
	DDdevice_InqShadeRange (dde_x11_DGL_current_dcm_data->device, 
			     dde_x11_DGL_current_dcm_data->shade_index,
			     &range_min, &range_max);
	dde_x11_DGL_current_dcm_data->cwtavgpslim[0] = (DtReal)range_min;
	dde_x11_DGL_current_dcm_data->cwtavgpslim[1] = (DtReal)range_max;
	dde_x11_DGL_current_dcm_data->cwtavgpslim[2] = 
		(dde_x11_DGL_current_dcm_data->cwtavgpslim[1]-dde_x11_DGL_current_dcm_data->cwtavgpslim[0])/255.;
    }
}
/*
 ======================================================================
 */

ddr_x11_DGL_dcm_cswap_error_handler(error_code, pDraw)
     int error_code;
     XdDrawablePtr pDraw;
{
    ddt_x11_DGL_data *device;
    ddt_x11_DGL_data *dor_list_get_first_data();
    ddt_x11_DGL_data *dor_list_get_next_data();

    if (dde_x11_DGL_dcm_list != (struct list *)0) {
	for (device = dor_list_get_first_data(dde_x11_DGL_dcm_list);
	     device != (ddt_x11_DGL_data *)0;
	     device = dor_list_get_next_data (dde_x11_DGL_dcm_list)) {
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
	DDdevice_SetAbortedDraw(dde_x11_DGL_current_dcm_data->device,DcTrue);
	dde_x11_DGL_current_dcm_data->bad_geom = DcTrue;
	dde_x11_DGL_current_dcm_data->current_view = DcNullObject;
    }
}
/*
 ======================================================================
 */
