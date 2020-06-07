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
	ddr_gl_return_DCM_fcns
	ddr_gl_dcm_create_local_data
	ddr_gl_dcm_initialize_device
	ddr_gl_dcm_close_device
	ddr_gl_dcm_flush
	ddr_gl_dcm_swap_buffers
	ddr_gl_dcm_update_geometry
	ddr_gl_dcm_become_current_driver

  ======================================================================
 */

#include <string.h>
#include "../gl_driver.h"
#include <dore/dore_develop/error.h>

#ifdef SGI4_0
#   include <X11/Xutil.h>
#endif

#define streq(a,b)   (0 == strcmp((a),(b)))
#define strsize(s)   (1+strlen(s))


ddt_gl_data *dde_gl_current_device_data;

DDt_DCM *dde_gl_DCM_fcns = DcNullPtr;

/* ====================================================================== */

DtPtr ddr_gl_return_DCM_fcns (void)
{
    DDt_DCM *fcns;

    if (!dde_gl_DCM_fcns) {
	fcns = DDdevice_CreateDCMStruct();

	if (!fcns) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_gl_return_DCM_fcns", DcNullPtr);
	}

	fcns->version_number = 1;

#       ifdef __STDC__
#	    define SetFuncPtr(NAME) fcns->NAME = ddr_gl_dcm_##NAME
#       else
#	    define SetFuncPtr(NAME) fcns->NAME = ddr_gl_dcm_/**/NAME
#       endif

	SetFuncPtr (create_local_data);
	SetFuncPtr (initialize_device);
	SetFuncPtr (become_current_driver);
	SetFuncPtr (close_device);
	SetFuncPtr (flush);
	SetFuncPtr (swap_buffers);
	SetFuncPtr (update_geometry);
	SetFuncPtr (inquire_device_extent);
	SetFuncPtr (inquire_stereo);
	SetFuncPtr (inquire_ncolors);
	SetFuncPtr (inquire_resolution);
	SetFuncPtr (inquire_visual_type);
	SetFuncPtr (inquire_auto_size);
	SetFuncPtr (inquire_color_entries);
	SetFuncPtr (set_color_entries);
	SetFuncPtr (set_foreground_color);
	SetFuncPtr (set_background_color);
	SetFuncPtr (set_shade_index);
	SetFuncPtr (inquire_clip_list);
	SetFuncPtr (clear_rectangle_depth);
	SetFuncPtr (clear_rectangle_depth_and_color);
	SetFuncPtr (set_depth_buffer_enable);
	SetFuncPtr (set_depth_buffer_write);
	SetFuncPtr (write_scanline_byte);
	SetFuncPtr (set_current_view);
	SetFuncPtr (inquire_pixel_data);

	/* Version 2 Functions */

	SetFuncPtr (set_options);

	dde_gl_DCM_fcns = fcns;
    }

    return (DtPtr) dde_gl_DCM_fcns;
}



/* ======================================================================== */

DtPtr ddr_gl_dcm_create_local_data (DtObject device)
{
    ddt_gl_data *device_data;

    if (!(device_data = DDspace_Allocate (sizeof (ddt_gl_data)))) {
	DDerror (ERR_CANT_ALLOC_MEM_SIZE, 
		    "ddr_gl_dcm_create_local_data", "sizeof (ddt_gl_data)");
	return DcNullPtr;
    }

    device_data->device = (DtPtr) device;

    return (DtPtr) device_data;
}



/* ======================================================================== */

DtFlag ddr_gl_dcm_initialize_device (
    DtObject device,
    DtPtr    device_data_parm,
    DtPtr    argstring,
    DtPtr    name)	
{
    auto   ddt_gl_data *device_data = device_data_parm;
    auto   char *argstringcopy;
    auto   char *tmp;
    auto   char *p;
    auto   char *nextarg;
    static char *funcname = "ddr_gl_dcm_initialize_device";
    auto   int   error  = 0;
    auto   int   width  = 512;
    auto   int   height = 512;
    auto   int   DoubleBuffer = 1;
    auto   int   user_window = -1;

#   ifdef SGI4_0
	auto   char                *display_name;
	auto   GLXconfig            params[50];
	auto   GLXconfig*           next;
	auto   GLXconfig*           retconfig;
	auto   Colormap             cmap;
	auto   XVisualInfo*         vis;
	auto   XVisualInfo          vtemplate;
	auto   XSetWindowAttributes cwa;
	auto   XWindowAttributes    pwa;
	auto   int                  i, nret;
	auto   Window               win;
	auto   Display             *xdisplay;
	auto   Display             *user_xdisplay=0;
	auto   Window               user_xwindow=0;
	auto   XEvent               event;
#   endif

    /* This function allocates the device driver instance specific
    ** storage and setups up the device for display.  */

    device_data->auto_size   = DcTrue;
    device_data->device      = device;
    device_data->sphere_type = SPHTYPE_DORE;
    device_data->use_X       = DcFalse;
    device_data->visualtype  = DcTrueColor;

    /* Process the argstring */

    argstringcopy = DDspace_Allocate (strsize(argstring));
    strcpy(argstringcopy, argstring); /* so as not to alter user's string */
    tmp = argstringcopy;

    /* p points to first sub-string */

    while ((p = DDstrtok(tmp, " \t,"))) {
	tmp = 0;

	if (streq (p,"-noautosize")) {
	    device_data->auto_size = DcFalse;
	} else if (streq (p, "-visualtype")) {
	    if (!(nextarg = DDstrtok(0, " \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "visualtype: (no value)");
		error = 1;
	    } else {
		if (streq ("DcPseudoColor", nextarg)) {
		    device_data->visualtype = DcPseudoColor;
		} else if (streq ("DcTrueColor", nextarg)) {
		    device_data->visualtype = DcTrueColor;
		} else {
		    sprintf (DDerror_GetLine(), "visualtype: '%s'", nextarg);
		    DDerror (ERR_BAD_DEVICE_OPT_VAL,
				funcname, DDerror_GetLine());
		    error = 1;
		}
	    } 
	} else if (streq (p,"-width")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "width: (no value)");
		error = 1;
	    } else {
		sscanf(nextarg,"%d",&width);
	    }
	} else if (streq (p,"-height")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "height: (no value)");
		error = 1;
	    } else {
		sscanf(nextarg,"%d",&height);
	    }
	} else if (streq (p,"-singlebuffered")) {
	    DoubleBuffer = 0;
	} else if (streq (p, "-spheretype")) {
	    if (!(nextarg = DDstrtok (0," \t"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL, funcname,
			 "unspecified argument for -spheretype");
	    } else {
		if (streq (nextarg, "dore")) {
		    device_data->sphere_type = SPHTYPE_DORE;
		} else if (streq (nextarg, "gl")) {
		    device_data->sphere_type = SPHTYPE_GLFULL;
		} else if (streq (nextarg, "glhemi")) {
		    device_data->sphere_type = SPHTYPE_GLHEMI;
		} else {
		    sprintf (DDerror_GetLine(),
			     "unknown spheretype (%s)", nextarg);
		    DDerror (ERR_BAD_DEVICE_OPT, funcname, DDerror_GetLine());
		}
	    }
	} else if (streq (p,"-window")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "window: (no value)");
		error = 1;
	    } else {
		sscanf(nextarg,"%d",&user_window);
	    }
	}
#ifdef SGI4_0
	else if (streq (p,"-useX")) {
	    device_data->use_X = DcTrue;
	} else if (streq (p,"-xwindow")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "xwindow: (no value)");
		error = 1;
	    } else {
		device_data->use_X = DcTrue;
		sscanf(nextarg,"%d",&user_xwindow);
	    }
	} else if (streq (p,"-xdisplay") || streq(p,"-display")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "xdisplay: (no value)");
		error = 1;
	    } else {
		device_data->use_X = DcTrue;
		sscanf(nextarg,"%d",&user_xdisplay);
	    }
	} else if (streq (p,"-geometry")) {
	    if (!(nextarg = DDstrtok (0, " \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "xdisplay: (no value)");
		error = 1;
	    } else {
		/* Ignore the -geometry option. */
	    }
	}
#endif
	else {
	    sprintf(DDerror_GetLine(), "unknown flag '%s' ignored", p);
	    DDerror (ERR_BAD_DEVICE_OPT,
			funcname, DDerror_GetLine());
	}
    }

    DDspace_Deallocate (argstringcopy);

    if (error) {
	return(DcFalse);
    }

    device_data->width = width;
    device_data->height = height;

    /* Do device specific stuff */

#   ifdef DEBUGP
	printf ("(DEVICE)Making a %dx%d %s window...\n",
	    width, height,
	    (device_data->visualtype == DcPseudoColor)
		? "Pseudo Color" : "True Color");
#   endif

    device_data->backgroundRGB[0] = 0.;
    device_data->backgroundRGB[1] = 0.;
    device_data->backgroundRGB[2] = 0.;
    device_data->backgroundcolor  = 0;

    if (device_data->use_X) {
#ifdef SGI4_0
	if (user_xwindow >(Window)0 && (!user_xdisplay)) {
	    DDerror (ERR_BAD_DEVICE_OPT,
		     funcname, "must set -xdisplay with -xwindow\n");
	    return(DcFalse);
	}

	if (!user_xdisplay) {
	    if((display_name = DDgetenv("DISPLAY")) == NULL) {
		DDerror(ERR_DEVICE_SYSTEM_ACCESS,funcname,
			"Display Environment variable not set");
		return DcFalse;
	    }
	    if((device_data->xdisplay = XOpenDisplay(display_name)) == NULL) {
		sprintf(DDerror_GetLine(),
			"Could not open display %s", display_name);
		DDerror(ERR_DEVICE_SYSTEM_ACCESS, funcname, DDerror_GetLine());
		return DcFalse;
	    }
	    device_data->user_xdisplay = DcFalse;
	} else {
	    device_data->user_xdisplay = DcTrue;
	    device_data->xdisplay = user_xdisplay;
	}

	xdisplay = device_data->xdisplay;

	/* get gl info */

	next = params;

	next->buffer = GLX_NORMAL;
	next->mode = GLX_RGB;
	if (device_data->visualtype != DcPseudoColor) {
	    next->arg = TRUE;
	} else {
	    next->arg = FALSE;
	}
	next++;
	
	next->buffer = GLX_NORMAL;
	next->mode = GLX_DOUBLE;
	if (DoubleBuffer) {
	    next->arg = TRUE;
	} else {
	    next->arg = FALSE;
	}
	next++;

	/* null terminate the list */
	next->buffer = 0;
	next->mode = 0;
	next->arg = 0;

	/*
	 * Get configuration data for a window based on above parameters
	 * First we have to find out which screen the parent window is on,
	 * then we can call GXLgetconfig()
	 */
    
	retconfig = GLXgetconfig(xdisplay, XDefaultScreen(xdisplay), params);
	if (!retconfig) {
	    DDerror(ERR_DEVICE_SYSTEM_ACCESS,
		    funcname, "Requested type of window unavailable");

	    if (user_xdisplay > (Display *)0) {
		XCloseDisplay(xdisplay);
	    }
	    return DcFalse;
	}
	/*
	 * The GL sets its own X error handlers, which aren't as informative
	 * when errors happen.  Calling XSetErrorHandler(0) here will
	 * reset back to the default Xlib version.
	 */
	XSetErrorHandler(0);

	if (user_xwindow == 0) {
	    /*
	     * Scan through config info, pulling info needed to create a window
	     * that supports the rendering mode.
	     */

	    cmap= DefaultColormap(xdisplay, DefaultScreen(xdisplay));

	    for (next = retconfig; next->buffer; next++) {
		unsigned long buffer = next->buffer;
		unsigned long mode = next->mode;
		unsigned long value = next->arg;
		switch (mode) {
		case GLX_COLORMAP:
		    if (buffer == GLX_NORMAL) {
			cmap = value;
		    }
		    break;
		case GLX_VISUAL:
		    if (buffer == GLX_NORMAL) {
			vtemplate.visualid = value;
			vtemplate.screen = DefaultScreen(xdisplay);
			vis = XGetVisualInfo(xdisplay, 
					     VisualScreenMask|VisualIDMask,
					     &vtemplate, &nret);
		    }
		    break;
		}
	    }

	    /* Create the window */

	    cwa.colormap = cmap;
	    /* Even if we don't use it, it must be something */
	    cwa.border_pixel = 0;

	    win = XCreateWindow(xdisplay, DefaultRootWindow(xdisplay), 
				0, 0, width, height,
				0, vis->depth, InputOutput, vis->visual,
				CWColormap|CWBorderPixel, &cwa);

	    device_data->xwindow = win;
	    device_data->user_xwindow = DcFalse;

	    XSelectInput(xdisplay, device_data->xwindow, ExposureMask);
	    XMapWindow(xdisplay, device_data->xwindow);
	    while(XNextEvent(xdisplay,&event)) {
                 if(event.type == Expose)
                       break;
	    }
	} else {
	    device_data->xwindow = user_xwindow;
	    device_data->user_xwindow = DcTrue;
	}

	/* Rescan configuration info and find window slot that getconfig
	** provided.  Fill it in with the window we just created.  */

	for (next = retconfig; next->buffer; next++) {
	    if ((next->buffer == GLX_NORMAL) && (next->mode == GLX_WINDOW)) {
		next->arg = device_data->xwindow;
		break;
	    }
	}

	/*
	 * Now "retconfig" contains all the information the GL needs to
	 * configure the window and its own internal state.
	 */

	i = GLXlink(xdisplay, retconfig);
	if (i < 0) {
	    DDerror(ERR_DEVICE_SYSTEM_ACCESS,
		    funcname, "GLXlink failed");

	    if (user_xdisplay != 0) {
		XCloseDisplay(xdisplay);
	    }
	    return DcFalse;
	}
#endif 
    } else {

	if (user_window < 0) {
	    foreground();
	    prefsize(width,height);
	    device_data->winid = winopen("Dore");
	    winconstraints();
	    device_data->user_window = 0;
	} else {
	    winset(user_window);
	    device_data->winid = user_window;
	    device_data->user_window = 1;
	}

	if (device_data->visualtype != DcPseudoColor)
		RGBmode();
	else {
	    cmode();
	    if (getgdesc(GD_NMMAPS) > 1) {
		multimap();
	    }
	}

	if (DoubleBuffer)
	    doublebuffer ();
	else
	    singlebuffer ();

	lsetdepth (getgdesc(GD_ZMIN), getgdesc(GD_ZMAX));
	gconfig ();

	if (device_data->visualtype == DcPseudoColor)
	    if (getgdesc(GD_NMMAPS) > 1)
		setmap (getgdesc(GD_NMMAPS) - 1);
    }

    CURDEV = device_data;

#if 0
    /* Where should this really go? */
     
     { ddr_gl_dcm_dith_color_table *ct;
     int i;
     
     ct = ddr_gl_dcm_dith_make_color_table();
     
     for (i=0; i<Dith_NumVout; i++)
     mapcolor(i,ct->red[i],ct->green[i],ct->blue[i]);
     
     free(ct);
     }
#endif

    return DcTrue;
}



/*==========================================================================
==  This procedure resets some of the command-line options while the device
==  is currently open.
==========================================================================*/

void ddr_gl_dcm_set_options (DtPtr argstring)
{
    auto     char *argstring_copy;	/* Copy of Argument String */
    static   char *funcname = "ddr_gl_dcm_set_options";
    register char *nextarg;		/* Next Option Argument Pointer */
    register char *ptr;			/* Argument String Pointer */
    auto     char *tmp;			/* Temporary Token Pointer */


    /* We need to make a copy of the option argument string so we don't
    ** destroy the string as we tokenize it. */

    argstring_copy = DDspace_Allocate (strsize(argstring));
    strcpy (argstring_copy, argstring);
    tmp = argstring_copy;

    /* ptr points to first sub-string */

    while ((ptr = DDstrtok (tmp, " \t,"))) {
	tmp = 0;

	if (streq (ptr, "-spheretype")) {
	    if (!(nextarg = DDstrtok (0," \t"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL, funcname,
			 "unspecified argument for -spheretype");
	    } else {
		if (streq (nextarg, "dore")) {
		    CURDEV->sphere_type = SPHTYPE_DORE;
		} else if (streq (nextarg, "gl")) {
		    CURDEV->sphere_type = SPHTYPE_GLFULL;
		} else if (streq (nextarg, "glhemi")) {
		    CURDEV->sphere_type = SPHTYPE_GLHEMI;
		} else {
		    sprintf (DDerror_GetLine(),
			     "unknown argument ignored (%s)", nextarg);
		    DDerror (ERR_BAD_DEVICE_OPT, funcname, DDerror_GetLine());
		}
	    }
	}
	else {
	    sprintf (DDerror_GetLine(), "unknown flag '%s' ignored", ptr);
	    DDerror (ERR_BAD_DEVICE_OPT, funcname, DDerror_GetLine());
	}
    }

    DDspace_Deallocate (argstring_copy);
}



/*============================================================================
**  This routine closes the display device window.  There are four possible
**  types of windows to consider:  user-supplied X window, user-supplied GL
**  window, default X window, and default GL window.  If the user supplied the
**  window, then we let the user close it.  If we opened a default window,
**  then we need to close it here and free up any resources.
**==========================================================================*/

void ddr_gl_dcm_close_device (void)
{
    /* If the device's use_X flag is set, then we're using an X window that is
    ** either user-supplied or our own default X window. */

    if (CURDEV->use_X) {
#   ifdef SGI4_0

	/* If user_xwindow is zero, then we supplied a default X window,
	** so we need to close it here. */

	if (!CURDEV->user_xwindow) {
	    XUnmapWindow(CURDEV->xdisplay, CURDEV->xwindow);
	    XDestroyWindow(CURDEV->xdisplay, CURDEV->xwindow);
	}

	/* If user_xdisplay is zero, then we supplied a default X display,
	** so we need to close it. */

	if (!CURDEV->user_xdisplay) {
	    XCloseDisplay(CURDEV->xdisplay);
	}
#   endif
    } else {
	/* If user_window is zero, then we supplied the GL window, so we
	** need to close it. */

	if (!CURDEV->user_window)
	    winclose(CURDEV->winid);
    }

    DDspace_Deallocate (CURDEV);
    CURDEV = NULL;		
}



/* ====================================================================== */

void ddr_gl_dcm_flush (void)
{   return;
}

/* ====================================================================== */

void ddr_gl_dcm_swap_buffers (void)
{
#   ifdef DEBUGP
	printf("(DEVICE)Swapbuffers called\n");
#   endif
    swapbuffers();
}

/* ====================================================================== */

void ddr_gl_dcm_update_geometry (void)
{   return;
}

/* ====================================================================== */

void ddr_gl_dcm_become_current_driver (DtPtr device_data)
{
#   ifdef DEBUGP
	printf("(DEVICE)Become_current_driver() called\n");
#   endif

    CURDEV = (ddt_gl_data*) device_data;

    if (CURDEV->use_X) {
#       ifdef SGI4_0
	    if (GLXwinset(CURDEV->xdisplay, CURDEV->xwindow) < 0) {
		DDerror(ERR_DEVICE_SYSTEM_ACCESS,
			"ddr_gl_dcm_become_current_driver", "GLXwinset failed");
	}
#     endif
    }
    else
    {   winset(CURDEV->winid);
    }
}
