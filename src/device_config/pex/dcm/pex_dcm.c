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
 
/*****************************************************************************
**  This is the device-control module for the PEX device driver.  It contains
**  the core functions used by the dynamic renderer and the production
**  renderer.
*****************************************************************************/

#include <string.h>
#include "../pex_driver.h"

#define DEBUG 0		/* Debug Level. */

    /*  Macro Definitions  */

#define streq(a,b)	(strcmp(a,b) == 0)

    /* Function Prototypes */

static DtFlag  ParseCommandArgs (char*);

    /*  Global Variables  */

DDt_DCM *dde_pex_DCM_fcns = 0;



/*****************************************************************************
**  This routine opens a new display window.  It is called on device
**  initialization if the user didn't supply a display.  This function returns
**  DcTrue if the open was successful, otherwise it returns DcFalse.
*****************************************************************************/

static DtFlag  OpenNewWindow  (void)
{
    static char funcname[] = "OpenNewWindow";

    auto   int                   attrMask;	  /* Window Attribute Flags */
    auto   int                   class;
    auto   XClassHint            class_hints;
    auto   Colormap              colormap;
    auto   unsigned int          depth;
    auto   DtInt                 nplanes;	  /* # Bitplanes in Display */
    auto   int                   nvisuals;
    static XVisualInfo           vinfo_template;
    auto   XVisualInfo          *visual_info;
    auto   XSetWindowAttributes  window_attr_set; /* Window Attributes to Set */
    auto   XSizeHints            xsh;
    static XWMHints              xwmh =
    {   InputHint | StateHint,		/* Flags */
	True,				/* Input */
	NormalState,			/* Initial State */
	0, 0, 0,0, 0,			/* Icon Pixmap,Window,Location,Mask */
	0,				/* Window Group */
    };

#   if DEBUG_CALL
	print ("OpenNewWindow()\n");
	fflush (stdout);
#   endif

    /* Find a visual that matches the screen, class and depth that we're
    ** looking for.  To to this, load up the XSetWindowAttributes variable
    ** to reflect the fields we want and call XGetVisualInfo.  */

    vinfo_template.screen = XDefaultScreen (DCM->display);

    if (DCM->visualtype == DcPseudoColor)
    {   vinfo_template.class = PseudoColor;
	vinfo_template.depth = nplanes = 8;
    }
    else
    {   vinfo_template.class  = TrueColor;
	vinfo_template.depth  = nplanes = 24;
    }

    /*add an XSync for bug in Xlib. See TRs Dore-0657 and 0707 */
    XSync(DCM->display, False);
    visual_info = XGetVisualInfo
		  (   DCM->display,
		      VisualScreenMask | VisualClassMask | VisualDepthMask,
		      &vinfo_template, &nvisuals
		  );

    /* If no visuals matched the one we're looking for, then the screen
    ** doesn't support the type we want, so flag an error and return. */

    if (nvisuals == 0)
    {   DDerror (ERR_DEVICE_SYSTEM_ACCESS, funcname,
		    "screen doesn't support specified visual type");
	DDspace_Deallocate (DCM);
	return DcFalse;
    }

    DCM->screen = visual_info->screen;
    class  = visual_info->class;
    depth  = visual_info->depth;

    /* Prior to creating the window, we need to set up the colormap that
    ** corresponds to the class we want.  In addition, set the window
    ** attributes fields that we want for the new window.  */

    if (visual_info->visual == DefaultVisual (DCM->display, DCM->screen))
	colormap = DefaultColormap (DCM->display, DCM->screen);
    else
    {   colormap = XCreateColormap (   DCM->display,
				       XDefaultRootWindow (DCM->display),
				       visual_info->visual, AllocNone
				   );
    }

    attrMask = CWColormap;
    window_attr_set.colormap = colormap;
    window_attr_set.background_pixel = BlackPixel (DCM->display,DCM->screen);
    window_attr_set.border_pixel     = WhitePixel (DCM->display,DCM->screen);
    attrMask |= CWBackPixel | CWBorderPixel;

    /* Create the X window.  If the window could not be created, then flag
    ** and error and return.  */

    DCM->window = XCreateWindow
		  (   DCM->display, DefaultRootWindow (DCM->display),
		      DCM->winX, DCM->winY, DCM->width, DCM->height,
		      0, nplanes, InputOutput, visual_info->visual,
		      attrMask, &window_attr_set
		  );

    if (!DCM->window)
    {
	DDerror (ERR_DEVICE_SYSTEM_ACCESS, funcname,
		"Couldn't create X11 window");

	DDspace_Deallocate (DCM);

	if (!DCM->user_display)
	    XCloseDisplay (DCM->display);

	return DcFalse;
    }

    /*  Set the standard properties for the window managers.  */

    XStoreName   (DCM->display, DCM->window, "Dore' Window");
    XSetIconName (DCM->display, DCM->window, "Dore' Window");
    XSetWMHints  (DCM->display, DCM->window, &xwmh);

    /* Set the size hints to indicate the position and size of the window. */

    xsh.flags  = USPosition | USSize;
    xsh.x      = DCM->winX;
    xsh.y      = DCM->winY;
    xsh.width  = DCM->width;
    xsh.height = DCM->height;

    XSetSizeHints(DCM->display, DCM->window, &xsh, XA_WM_NORMAL_HINTS);

    /* Set the class hints to indicate the type of resource we're running. */

    class_hints.res_name = "dore";
    class_hints.res_class = "Dore";

    XSetClassHint (DCM->display, DCM->window, &class_hints);

    /* Do double or single buffering as necessary.  If we're supposed to do
    ** double buffering, but don't have the multi-buffer extension to X or
    ** if the multi-buffer create fails, then fall back to single buffering
    ** and just using the X window for the updates.  */

    if (DCM->nbuffers == 2)
    {   auto int mbx_event;
	auto int mbx_error;
        auto int nbuffs_allocated;

	/* See if the multi-buffering X extension is supported. */

	if (!XmbufQueryExtension (DCM->display, &mbx_event, &mbx_error))
	{   sprintf
	    (   DDerror_GetLine(),
		"Multi-buffer extension isn't supported on %s.",
		XDisplayString (DCM->display)
	    );
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS, funcname, DDerror_GetLine());

	    /* Fall back to single-buffered display. */
	    DCM->nbuffers = 1;
	}
	else
        {   /* The extension is supported, can we allocate the buffers? */

	    nbuffs_allocated =
		XmbufCreateBuffers
		(   DCM->display, DCM->window, 2,
		    MultibufferUpdateActionUndefined,
		    MultibufferUpdateHintFrequent, DCM->buffers
		);

	    if (nbuffs_allocated == 2)
		DCM->dispbuff = DCM->buffers [1];
	    else
	    {   DCM->nbuffers = 1;
		XmbufDestroyBuffers (DCM->display, DCM->window);
		DDerror
		(   ERR_DEVICE_SYSTEM_ACCESS, funcname,
		    "Unable to allocate two buffers; using single buffer mode."
		);
	    }
	}
    }

    if (DCM->nbuffers != 2)
        DCM->dispbuff = DCM->window;

    return DcTrue;
}



/*****************************************************************************
**  This function parses the command-line options and sets the corresponding
**  device fields to reflect the settings.  Command-line options which are
**  not understood are flagged with an error, after which processing continues
**  until there are no more arguments left.  If a command-line value is not
**  appropriate or is an error, this function will return DcFalse, otherwise
**  it returns DcTrue.
*****************************************************************************/

static DtFlag  ParseCommandArgs  (char *argstring)
{
    static char funcname[] = "ParseCommandArgs";

    auto   char   *argstringcopy;	/* Copy of Argument String */
    auto   DtFlag  retcode;		/* If true, then successful */
    auto   char   *tmp;			/* Tokenizing Function Parameter */
    auto   char   *ptr;			/* String Pointer */
    auto   char   *nextarg;		/* Next Command Argument */

    /* Make a copy of the argument string for parsing. */

    argstringcopy = (char*) DDspace_Allocate (strlen(argstring) + 1);
    strcpy (argstringcopy, argstring);
    tmp = argstringcopy;

    /* ptr points to first sub-string */

    retcode = DcTrue;

    while ((ptr = DDstrtok (tmp, " \t,")))
    {   tmp = 0;

	if (streq (ptr, "-display"))
	{   if (!(nextarg = DDstrtok (0," \t,")))
	    {   DDerror (ERR_BAD_DEVICE_OPT_VAL,funcname,"device: (no value)");
		retcode = DcFalse;
	    }
	    else
	    {   auto long dptr;
		DCM->user_display = DcTrue;
		sscanf (nextarg, "%ld", &dptr);
		DCM->display = (Display *) dptr;
	    }
	}

	else if (streq (ptr, "-geometry"))
	{   if (!(nextarg = DDstrtok (0, " \t,"))){
		DDerror(ERR_BAD_DEVICE_OPT_VAL,funcname,"geometry: (no value)");
		retcode = DcFalse;
	    } else {
		XParseGeometry(nextarg, &DCM->winX, &DCM->winY,
			       &DCM->width, &DCM->height);
	    }
	}

	else if (streq (ptr, "-ncolors"))
	{   if (!(nextarg = DDstrtok (0," \t,")))
	    {   DDerror (ERR_BAD_DEVICE_OPT_VAL,funcname,
			"ncolors:  (no value)");
		retcode = DcFalse;
	    }
	    else
		sscanf (nextarg, "%d", &(DCM->ncolors));
	}

	else if (streq (ptr, "-noautosize"))
	{   DCM->auto_size = DcFalse;
	}

	else if (streq (ptr, "-noztc"))
	{   /* Ignore */
	}

	else if (streq (ptr, "-singlebuffered"))
	{   DCM->nbuffers = 1;
	}

	else if (streq (ptr, "-visualtype"))
	{   if (!(nextarg = DDstrtok (0," \t,")))
	    {   DDerror (ERR_BAD_DEVICE_OPT_VAL,funcname,
			"visualtype:  (no value)");
		retcode = DcFalse;
	    }
	    else
	    {   if (streq ("DcPseudoColor", nextarg))
		    DCM->visualtype = DcPseudoColor;
		else if (streq ("DcTrueColor", nextarg))
		    DCM->visualtype = DcTrueColor;
		else
		{   sprintf (DDerror_GetLine(), "Bad visual type (%s)",nextarg);
		    DDerror (ERR_BAD_DEVICE_OPT_VAL,
			     funcname, DDerror_GetLine());
		}
	    }
	}

	else if (streq (ptr, "-window"))
	{   if (!(nextarg = DDstrtok (0," \t,")))
	    {   DDerror (ERR_BAD_DEVICE_OPT_VAL,funcname,"window: (no value)");
		retcode = DcFalse;
	    }
	    else
	    {   auto long val;
		DCM->user_window = DcTrue;
		sscanf (nextarg,"%ld",&val);
		DCM->window = (Window) val;
	    }
	}

	else
	{   sprintf (DDerror_GetLine(), "flag '%s'", ptr);
	    DDerror (ERR_BAD_DEVICE_OPT, funcname, DDerror_GetLine());
	}
    }

    if ((DCM->visualtype == DcPseudoColor) && (DCM->ncolors != 256))
    {   sprintf (DDerror_GetLine(),
	    "Expected 'ncolors' value of 256 for pseudo-color, got %d.",
	    DCM->ncolors);
	DDerror (ERR_BAD_DEVICE_OPT_VAL, funcname, DDerror_GetLine());
	retcode = DcFalse;
    }

    DDspace_Deallocate (argstringcopy);

    return retcode;
}



/*****************************************************************************
**  become_current_driver is called each time this specific device becomes the
**  current device (as a result of an update being performed on the device).
*****************************************************************************/

void ddr_pex_dcm_become_current_driver (DtPtr device_data)
{
#   if DEBUG_CALL
	printf ("ddr_pex_dcm_become_current_driver (%lx)\n", device_data);
	fflush (stdout);
#   endif

    DCM = (ddt_pex_dcm_data*) device_data;
}



/*****************************************************************************
**  This function closes the current PEX device, frees any space that the
**  driver has allocated with ddr_pex_dcm_create_local_data, and releases any
**  other resources associated with the current PEX driver.
*****************************************************************************/

void ddr_pex_dcm_close_device (void)
{
#   if DEBUG_CALL
	printf ("ddr_pex_dcm_close_device ()\n");
	fflush (stdout);
#   endif

    /* Finish up all X stuff. */

    XFlush (DCM->display);

    /* If we're using the multi-buffer extension to do double buffering, then
    ** destroy the multi-buffers. */

    if (DCM->nbuffers == 2)
	XmbufDestroyBuffers (DCM->display, DCM->window);

    /* If the window was created by the user, then let the user clean it up.
    ** If we created the window, then we clean it up. */

    if (!DCM->user_window)
    {   XUnmapWindow (DCM->display, DCM->window);
	XDestroyWindow (DCM->display, DCM->window);
    }

    /* Free up the graphics context. */

    XFreeGC (DCM->display, DCM->gc);

    if (!DCM->user_display)
	XCloseDisplay (DCM->display);
    else
	XFlush (DCM->display);

    /* Don't do a destroy image because there is no data here. */

    XFree ((char*) DCM->image);

    /* Finally, free up the device data structure associated with the current
    ** device. */

    DDspace_Deallocate (DCM);
    DCM = NULL;
}



/*****************************************************************************
**  This routine is called by DoDevice to allocate any memory required by the
**  device driver to maintain local data, and returns a pointer to this
**  allocated memory.
*****************************************************************************/

DtPtr ddr_pex_dcm_create_local_data (DtObject device)
{
    auto ddt_pex_dcm_data *device_data;	/* Device Data Memory Pointer */

#   if DEBUG_CALL
	printf ("ddr_pex_dcm_create_local_data (%lx)\n", device);
	fflush (stdout);
#   endif

    device_data = DDspace_Allocate (sizeof(ddt_pex_dcm_data));

    if (!device_data)
    {   DDerror (ERR_CANT_ALLOC_MEM_SIZE, "ddr_pex_dcm_create_local_data",
		"sizeof (ddt_pex_dcm_data)");
	return DcNullPtr;
    }

    device_data->device = device;

    return (DtPtr) device_data;
}



/*****************************************************************************
**  This routine flushes the device driver.  All commands currently queued for
**  execution will be sent to the display device.
*****************************************************************************/

void ddr_pex_dcm_flush (void)
{
#   if DEBUG_CALL
	printf ("ddr_pex_dcm_flush ()\n");
	fflush (stdout);
#   endif

    XFlush (DCM->display);
}



/*****************************************************************************
**  This function is called by the Dore' kernel to create a new instance of
**  the Pex driver when the application calls DoDevice.  This function returns
**  DcTrue if the initialization succeeded.
*****************************************************************************/

DtFlag ddr_pex_dcm_initialize_device (
    DtObject device,
    DtPtr    device_data,	/* Allocated by create_local_data */
    DtPtr    argstring,
    DtPtr    name)
{
    static char funcname[] = "ddr_pex_dcm_initialize_device";

    auto   XEvent   event;		/* X Update Event */
    auto   char    *display_name;	/* Name of DISPLAY env. var. */

#   if DEBUG_CALL
	printf ("ddr_pex_dcm_initialize_device (%lx, %lx, \"%s\", \"%s\")\n",
	    device, device_data, argstring, name);
	fflush (stdout);
#   endif

    /* Set the default values for the Pex device driver. */

    DCM = (ddt_pex_dcm_data*) device_data;
    DCM->device = device;

    DCM->visualtype   = DcTrueColor;
    DCM->ncolors      = 256;
    DCM->shadeindex   = 0;
    DCM->auto_size    = DcFalse;
    DCM->user_window  = DcFalse;
    DCM->user_display = DcFalse;

    DCM->winX = 0;
    DCM->winY = 0;
    DCM->width  = 512;
    DCM->height = 512;

    DCM->nbuffers = 2;

    /* Parse the command-line arguments. */

    if (!ParseCommandArgs (argstring))
	return DcFalse;

    /* If the user has already opened a display, then we'll use that one.  If
    ** the user has _not_ opened a display, then do it here. */

    if (!DCM->user_display)
    {
#       if DEBUG
	    printf ("%s:  Opening our own display.\n", funcname);
#       endif

	/* We need to get the DISPLAY environment variable to know the
	** display name.  */

	if (NULL == (display_name = DDgetenv ("DISPLAY")))
	{   DDerror (ERR_DEVICE_SYSTEM_ACCESS, funcname,
		"Environment variable 'DISPLAY' not set");
	    DDspace_Deallocate (DCM);
	    return DcFalse;
	}

	/* Open the X display. */

	if (NULL == (DCM->display = XOpenDisplay (display_name)))
	{   sprintf(DDerror_GetLine(),"Could not open display %s",display_name);
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS, funcname, DDerror_GetLine());
	    DDspace_Deallocate (DCM);
	    return DcFalse;
	}
    }

    /* If the user has already supplied a window, then use that one for the
    ** display.  If we aren't given a user window, then try to open one up for
    ** the display.  If the attempt to open a window fails, then give up. */

    if (!DCM->user_window && !OpenNewWindow ())
	return DcFalse;

    /* Set up the graphics context.  If this fails, flag an error and return. */

#   if DEBUG
	printf ("%s:  Creating graphics context.\n", funcname);
#   endif

    DCM->gc = XCreateGC (DCM->display, DCM->window, 0,0);

    if (!DCM->gc)
    {   DDerror (ERR_DEVICE_SYSTEM_ACCESS, funcname,
		"Couldn't create graphics context");

	if (DCM->nbuffers == 2)
	    XmbufDestroyBuffers (DCM->display, DCM->window);

	if (!DCM->user_window)
	{   XUnmapWindow (DCM->display, DCM->window);
	    XDestroyWindow (DCM->display, DCM->window);
	}

	if (!DCM->user_display)
	    XCloseDisplay (DCM->display);

	DDspace_Deallocate (DCM);
	return DcFalse;
    }

    /* If we're starting up and new window (i.e. not using a user-supplied
    ** window), then enable exposure-event notification, map the window
    ** to the display (enable the window), and wait on the initial exposure
    ** event.  */

    if (!DCM->user_window)
    {
        XSelectInput (DCM->display, DCM->window, ExposureMask);
        XMapWindow (DCM->display, DCM->window);

	while (XNextEvent (DCM->display,&event) && (event.type != Expose))
	    ;
    }

    /*add an XSync for bug in Xlib. See TRs Dore-0657 and 0707 */
    XSync(DCM->display, False);
    XGetWindowAttributes (DCM->display, DCM->window, &DCM->window_attrs);

    /* If the visual type is pseudo-color, then allocate and install the
    ** colormap. */

    if (DCM->visualtype == DcPseudoColor)
    {
	DCM->colormap = XCreateColormap (DCM->display, DCM->window,
					 DCM->window_attrs.visual, AllocAll);
	XSetWindowColormap (DCM->display, DCM->window, DCM->colormap);
    }

    DCM->width  = DCM->window_attrs.width;
    DCM->height = DCM->window_attrs.height;

    /* Create the display image. */

    DCM->image = XCreateImage
		 (   DCM->display, DCM->window_attrs.visual,
		     DCM->window_attrs.depth, ZPixmap, 0, NULL,
		     DCM->width, DCM->height, 32, 0
		 );

#   ifdef PIXMAP
	/* Create the pixmap used for double-buffering. */

	DCM->pixmap = XCreatePixmap (DCM->display, DCM->window,
	    DCM->width, DCM->height, DCM->window_attrs.depth);
#   endif

    /* Initialize PEX. */

#   if DEBUG_SYNC
	XSynchronize (DCM->display, True);
#   endif

    PEX.error_string = DDspace_Allocate (PEXErrorStringLength);

    if (0 != PEXInitialize (DCM->display, &PEX.ext_info, PEXErrorStringLength,
			    PEX.error_string))
    {   DDerror (ERR_NO_OUTPUT_MODULE, funcname,
		"Attempt to open PEX output module failed");

	if (DCM->nbuffers == 2)
	    XmbufDestroyBuffers (DCM->display, DCM->window);

	if (!DCM->user_window)
	{   XUnmapWindow
		(((ddt_pex_dcm_data*)(device_data))->display, DCM->window);
	    XDestroyWindow (DCM->display, DCM->window);
	}

	if (!DCM->user_display)
	    XCloseDisplay (DCM->display);

	DDspace_Deallocate (DCM);
	return DcFalse;
    }

    return DcTrue;
}



/*****************************************************************************
**  This routine changes the device-specific options that are initially set
**  with the argstring argument to the initialize_device routine.
*****************************************************************************/

void ddr_pex_dcm_set_options (DtPtr argstring)
{
#   if DEBUG_CALL
	printf ("ddr_pex_dcm_set_options (\"%s\")\n", argstring);
	fflush (stdout);
#   endif
}



/*****************************************************************************
**  This procedure swaps display buffers if the device supports double
**  buffering.  At this time double-buffering is sort of faked by rendering
**  to a pixmap on the side, and then using XCopyArea() to copy the off-screen
**  pixmap to the window pixmap.
*****************************************************************************/

void ddr_pex_dcm_swap_buffers (void)
{
#   if DEBUG_CALL
	print  ("ddr_pex_dcm_swap_buffers ()\n");
	fflush (stdout);
#   endif

#   ifdef PIXMAP
	XCopyArea (DCM->display, DCM->pixmap, DCM->window, DCM->gc,
	    0, 0, DCM->width, DCM->height, 0, 0);
	XSync (DCM->display, False);
#   endif

    /* If double-buffering is in effect, then display the finished image and
    ** set up the next buffer as the destination for future drawing. */

    if (DCM->nbuffers == 2)
    {   XmbufDisplayBuffers (DCM->display, 1, &(DCM->dispbuff), 0,0);
	DCM->dispbuff = (DCM->dispbuff == DCM->buffers[0])
		      ? DCM->buffers[1] : DCM->buffers[0];
    }
}



/*****************************************************************************
**  ddr_pex_dcm_update_geometry causes the device driver to take appropriate
**  action to update the clipping list maintained by the device driver.
*****************************************************************************/

void ddr_pex_dcm_update_geometry (void)
{
#   if DEBUG_CALL
	printf ("ddr_pex_dcm_update_geometry ()\n");
	fflush (stdout);
#   endif
}



/*****************************************************************************
**  This routine returns a pointer to the function pointer structure for the
**  device control module.  For each driver function, the associated structure
**  field is set to the function address.
*****************************************************************************/

DtPtr ddr_pex_return_DCM_fcns (void)
{
    auto DDt_DCM *fcns;

#   if DEBUG_CALL
	printf ("ddr_pex_return_DCM_fcns ()\n");
	fflush (stdout);
#   endif

    if (!dde_pex_DCM_fcns)
    {
	if (!(fcns = DDdevice_CreateDCMStruct ()))
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_pex_return_DCM_fcns", DcNullPtr);

	fcns->version_number = 2;

#       ifdef __STDC__
#           define Add_Function(NAME) fcns->NAME = ddr_pex_dcm_##NAME;
#       else
#           define Add_Function(NAME) fcns->NAME = ddr_pex_dcm_/**/NAME;
#       endif

	/* Version 1 Functions */

	Add_Function (become_current_driver);
	Add_Function (clear_rectangle_depth);
	Add_Function (clear_rectangle_depth_and_color);
	Add_Function (close_device);
	Add_Function (create_local_data);
	Add_Function (flush);
	Add_Function (initialize_device);
	Add_Function (inquire_auto_size);
	Add_Function (inquire_clip_list);
	Add_Function (inquire_color_entries);
	Add_Function (inquire_device_extent);
	Add_Function (inquire_ncolors);
	Add_Function (inquire_pixel_data);
	Add_Function (inquire_resolution);
	Add_Function (inquire_stereo);
	Add_Function (inquire_visual_type);
	Add_Function (set_background_color);
	Add_Function (set_color_entries);
	Add_Function (set_current_view);
	Add_Function (set_depth_buffer_enable);
	Add_Function (set_depth_buffer_write);
	Add_Function (set_foreground_color);
	Add_Function (set_shade_index);
	Add_Function (swap_buffers);
	Add_Function (update_geometry);
	Add_Function (write_scanline_byte);

	/* Version 2 Functions */

	Add_Function (set_options);
	dde_pex_DCM_fcns  = fcns;
    }

    return (DtPtr) dde_pex_DCM_fcns;
}
