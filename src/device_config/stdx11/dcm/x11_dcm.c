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
	ddr_stdx11_return_DCM_fcns
	ddr_stdx11_dcm_create_local_data
	ddr_stdx11_dcm_initialize_device
	ddr_stdx11_dcm_close_device
	ddr_stdx11_dcm_flush
	ddr_stdx11_dcm_swap_buffers
	ddr_stdx11_dcm_update_geometry
	ddr_stdx11_dcm_become_current_driver
	ddr_stdx11_dcm_set_options
	ddr_stdx11_dcm_setup_shading
	ddr_stdx11_InitSimulator
	ddr_stdx11_FreeBuffer
	ddr_stdx11_UpdateSimulator
	ddr_stdx11_dcm_color_to_pixel

  ======================================================================
 */
#include <string.h>
#include "dcm.h"
#include "../../soft_tools/softpipe/drom.h"

ddt_stdx11_data *dde_stdx11_current_device_data;
static DDt_DCM  *dde_stdx11_DCM_fcns = 0;

static int  available_entries (Display*, Visual*, Colormap);
static int  set_pseudo_ramp   (Colormap, int, int, int, ddt_stdx11_data*);
static void set_true_ramp     (Visual*, int, int, int, ddt_stdx11_data*);
static int  count_zeros       (unsigned int val);
static int  count_bits        (unsigned int val);

/* ====================================================================== */

DtPtr ddr_stdx11_return_DCM_fcns (void)
{
    DDt_DCM *fcns;

    if (dde_stdx11_DCM_fcns == (DDt_DCM *)0) {
	fcns = DDdevice_CreateDCMStruct();

	if (fcns == (DDt_DCM *)0) {
	    DDerror (ERR_CANT_ALLOC_MEM,"dde_stdx11_return_DCM_fcns",DcNullPtr);
	}

	fcns->version_number = 2;

#       ifdef __STDC__
#	    define AddFunction(name) \
		fcns->name = ddr_stdx11_dcm_##name
#       else
#	    define AddFunction(name) \
		fcns->name = ddr_stdx11_dcm_/**/name
#       endif

	AddFunction (create_local_data);
	AddFunction (initialize_device);
	AddFunction (become_current_driver);
	AddFunction (close_device);
	AddFunction (flush);
	AddFunction (swap_buffers);
	AddFunction (update_geometry);
	AddFunction (inquire_device_extent);
	AddFunction (inquire_stereo);
	AddFunction (inquire_ncolors);
	AddFunction (inquire_resolution);
	AddFunction (inquire_visual_type);
	AddFunction (inquire_auto_size);
	AddFunction (inquire_color_entries);
	AddFunction (set_color_entries);
	AddFunction (set_foreground_color);
	AddFunction (set_background_color);
	AddFunction (set_shade_index);
	AddFunction (inquire_clip_list);
	AddFunction (clear_rectangle_depth);
	AddFunction (clear_rectangle_depth_and_color);
	AddFunction (set_depth_buffer_enable);
	AddFunction (set_depth_buffer_write);
	AddFunction (write_scanline_byte);
	AddFunction (set_current_view);
	AddFunction (inquire_pixel_data);
	AddFunction (set_options);

	dde_stdx11_DCM_fcns = fcns;
    }
    return (DtPtr)dde_stdx11_DCM_fcns;
}

/* ====================================================================== */

DtPtr ddr_stdx11_dcm_create_local_data (DtObject device)
{
    ddt_stdx11_data *device_data;

#ifdef DEBUG
    printf("... processing device create local data ...\n");
    fflush(stdout);
#endif

    if((device_data = DDspace_Allocate (sizeof(ddt_stdx11_data))) 
       == NULL) {
	DDerror(ERR_CANT_ALLOC_MEM_SIZE, "ddr_stdx11_dcm_create_local_data",
		   "sizeof(ddt_stdx11_data)");
	return((DtPtr)DcNullPtr);
    }

    device_data->device = device;

    return device_data;
}

/* ====================================================================== */

DtFlag ddr_stdx11_dcm_initialize_device (
    DtObject device,
    DtPtr    device_data_parm,
    DtPtr    argstring,
    DtPtr    name)
{
#   define device_data ((ddt_stdx11_data*)(device_data_parm))

    char *argstringcopy, *tmp, *p, *nextarg;
    static char *funcname  = "ddr_stdx11_dcm_initialize_device";
    int error=0;

    char *display_name;
    int screen;
    XVisualInfo *visual_info, vinfo_template;
    unsigned int depth;
    int class;
    int nvisuals;
    XSetWindowAttributes window_attributes;
    unsigned long attr_mask;
    XSizeHints xsh;
    XClassHint class_hints;
    XEvent event;
    Display *display;
    int nred, ngreen, nblue, choice;
    Visual *visual;
    static XWMHints xwmh = {
	(InputHint|StateHint),
	True,
	NormalState,
	0,
	0,
	0, 0,
	0,
	0,
    };
    Colormap cmap;
    char foo[10];
    int i;
    int default_map;
#if defined(X11_PRE_R4) || defined(XLIB_ILLEGAL_ACCESS)
    ScreenFormat *fmt;
#else
    int npixmap_formats;
    XPixmapFormatValues *pixmap_formats, *fmt;
#endif

#ifdef DEBUG
    printf("... processing device initialize device ...\n");
    fflush(stdout);
#endif

    dde_stdx11_current_device_data = device_data;

    /* This function allocates the device driver instance specific
    ** storage and setups up the device for display.  */

    device_data->device = device;
    device_data->auto_size = DcTrue;

    depth = -1;
    class = -1;
    screen = -1;
    cmap = -1;
    nred = ngreen = nblue = -1;
    default_map = 0;

    DdSetShadeMode(device,DcComponent);

    xsh.flags = (USPosition|USSize);
    xsh.width = xsh.height = 512;
    xsh.x = xsh.y = 0;

    device_data->user_window = 0;
    device_data->user_display = 0;
    device_data->shade_index = 0;
    device_data->simulator = 0;
    device_data->dither = 0;
    device_data->dbf_enable = 0;
    device_data->doubleBuffered = 1;
    device_data->update_needed = DcTrue;

    device_data->current_view = DcNullObject;
    device_data->previous_view = DcNullObject;

    device_data->cwtavgpsweight[0] = 0.33;
    device_data->cwtavgpsweight[1] = 0.33;
    device_data->cwtavgpsweight[2] = 0.33;

    /*
     * Process the argstring
     */

    argstringcopy = (char *)DDspace_Allocate(strlen(argstring)+1);
    strcpy(argstringcopy, argstring); /* so as not to alter user's string */
    tmp = argstringcopy;

    /* p points to first sub-string */

    while((p = DDstrtok(tmp," \t,"))) {
	tmp = 0;

	if(!strcmp(p,"-noautosize")) {
	    device_data->auto_size = DcFalse;
	}
	else if (!strcmp(p,"-screen")) {
	    if(!(nextarg = DDstrtok(0," \t,"))) {
		DDerror(ERR_BAD_DEVICE_OPT_VAL,funcname,
			   "screen: (no value)");
		error = 1;
	    }
	    else {
		sscanf (nextarg, "%d", &screen);
	    }
	}
	else if (!strcmp(p,"-depth")) {
	    if(!(nextarg = DDstrtok(0," \t,"))) {
		DDerror(ERR_BAD_DEVICE_OPT_VAL,funcname,
			   "depth: (no value)");
		error = 1;
	    }
	    else {
		sscanf(nextarg,"%d",&depth);
	    }
	}
	else if (!strcmp(p,"-visualtype")) {
	    if(!(nextarg = DDstrtok(0," \t,"))) {
		DDerror(ERR_BAD_DEVICE_OPT_VAL,funcname,
			   "visualtype: (no value)");
		error = 1;
	    }
	    else {
		if(!strcmp("DcPseudoColor",nextarg)) {
		    class = PseudoColor;
		    device_data->visualtype = DcPseudoColor;
		}
		else if(!strcmp("DcStaticGrey",nextarg)) {
		    class = StaticGray;
		    device_data->visualtype = DcStaticGrey;
		} 
		else if(!strcmp("DcTrueColor",nextarg)) {
		    class = TrueColor;
		    device_data->visualtype = DcTrueColor;
		}
		else  {
		    sprintf(DDerror_GetLine(),
			    "visualtype: '%s'",nextarg);
		    DDerror(ERR_BAD_DEVICE_OPT_VAL,
			       funcname,DDerror_GetLine());
		    error = 1;
		}
	    } 
	}
	else if(!strcmp(p,"-display")) {
	    if(!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,funcname,"display: (no value)");
		error = 1;
	    }
	    else {
		long dptr;

		device_data->user_display = 1;
		sscanf (nextarg, "%ld", &dptr);
		device_data->display = (Display *) dptr;
	    }
	}
	else if(!strcmp(p,"-window")) {
	    if(!(nextarg = DDstrtok(0," \t,"))) {
		DDerror(ERR_BAD_DEVICE_OPT_VAL,
			   funcname, "window: (no value)");
		error = 1;
	    }
	    else {
		long w;

		device_data->user_window = 1;
		sscanf (nextarg, "%ld", &w);
		device_data->window = (Window) w;
	    }
	}
	else if(!strcmp(p,"-geometry")) {
	    if(!(nextarg = DDstrtok(0," \t,"))) {
		DDerror(ERR_BAD_DEVICE_OPT_VAL,
			   funcname, "geometry: (no value)");
		error = 1;
	    }
	    else {
		XParseGeometry(nextarg, &xsh.x,&xsh.y,&xsh.width,&xsh.height);
	    }
	}
	else if(!strcmp(p,"-singlebuffered")) {
	    device_data->doubleBuffered = 0;
	}
	/* 
	 * If the user doesn't specify a window id, he/she can
	 * use the -defaultmap to say: get the colors from the
	 * default colormap.  Otherwise, we create a new one
	 * so that we're compatible with the way this used to
	 * work.
	 */
	else if(!strcmp(p,"-defaultmap")) {
	    default_map = 1;
	}
	else if(!strcmp(p,"-simulator") ||
		!strcmp(p,"-zbuffer")) {
	    device_data->simulator = 1;
	}
	else if(!strcmp(p,"-dither")) {
	    device_data->dither = 1;
	}
	else if(!strcmp(p,"-trueramp"))  {
	    char *red, *green, *blue;

	    if(!(red = DDstrtok(0," \t,")) ||
	       !(green = DDstrtok(0, " \t,")) ||
	       !(blue = DDstrtok(0, " \t,"))) {
		DDerror(ERR_BAD_DEVICE_OPT_VAL,
			   funcname, "trueramp: (red green blue)");
		error = 1;
	    }
	    else {
		sscanf(red,"%d",&nred);
		sscanf(green,"%d",&ngreen);
		sscanf(blue,"%d",&nblue);
	    }
	}
	else {
	    sprintf(DDerror_GetLine(),"flag '%s'",p);
	    DDerror(ERR_BAD_DEVICE_OPT,
		       funcname, DDerror_GetLine());
	    error = 1;
	}
    }

    DDspace_Deallocate(argstringcopy);

    if(error) {
	return(DcFalse);
    }

    device_data->width = xsh.width;
    device_data->height = xsh.height;

    /*
     * do device specific stuff
     */

    if(!(device_data->user_display)) {
	if((display_name = DDgetenv("DISPLAY")) == NULL) {
	    DDerror(ERR_DEVICE_SYSTEM_ACCESS,funcname,
		       "Display Environment variable not set");
	    return DcFalse;
	}
	if((device_data->display = XOpenDisplay(display_name)) == NULL) {
	    sprintf(DDerror_GetLine(),
		    "Could not open display %s",display_name);
	    DDerror(ERR_DEVICE_SYSTEM_ACCESS,
		       funcname, DDerror_GetLine());
	    return DcFalse;
	}
    }
    display = device_data->display;

    /* 
     * If we are given a window, it will imply a particular visual 
     * and colormap to use.  If not, we have to figure out how to 
     * choose one.  The user can specify a preference of screen,
     * depth or class.  
     *
     * If there is no preference we look through the visuals in
     * the list to see if there is an acceptable one.  We prefer
     * to take the default visual over other acceptable choices
     * because the default visual plays better with other windows
     * on the screen better.
     */
    if(!(device_data->user_window)) {
	int mask;
	mask = 0;
	if (screen != -1) {
	    vinfo_template.screen = screen;
	    mask |= VisualScreenMask;
	}
	if (depth != -1) {
	    vinfo_template.depth = depth;
	    mask |= VisualDepthMask;
	}
	if (class != -1) {
	    vinfo_template.class = class;
	    mask |= VisualClassMask;
	}

	/*add an XSync for bug in Xlib. See TRs Dore-0657 and 0707 */
   	XSync(device_data->display, False);
	visual_info = XGetVisualInfo(device_data->display,
				     mask,&vinfo_template,&nvisuals);

	choice = -1;
	/*
	 * Look for any visual of the classes we like but choose
	 * the default over other possibles...  If a screen is
	 * specified, make sure to check the screen's visual --
	 * otherwise its the default screens.
	 */
	for (i = 0; i < nvisuals; i++) {
	    if ((visual_info[i].class == TrueColor ||
		 visual_info[i].class == PseudoColor ||
		 visual_info[i].class == StaticGray) &&
		(choice == -1 || 
		 visual_info[i].visual == 
		 DefaultVisual(display,(screen == -1 ? 
					DefaultScreen(display) :
					screen))))
		    choice = i;
	}

	if(choice == -1) {
	    if (class == -1 && depth == -1 && screen == -1)
		    DDerror(ERR_DEVICE_SYSTEM_ACCESS,funcname,
			       "screen doesn't support required visual type");
	    else
		    DDerror(ERR_DEVICE_SYSTEM_ACCESS,funcname,
			       "screen doesn't support specified visual type");
	    return DcFalse;
	}

	screen = visual_info[choice].screen;
	depth = visual_info[choice].depth;
	class = visual_info[choice].class;

	window_attributes.background_pixel = 
		BlackPixel(device_data->display,screen);
	window_attributes.border_pixel = 
		WhitePixel(device_data->display,screen);
	attr_mask = CWBackPixel|CWBorderPixel;

	/* 
	 * If we aren't creating the default visual type, we'll
	 * need to create a colormap because we can't inherit 
	 * it from parent window.  Otherwise, the user must
	 * specify the -defaultmap option in order to use the 
	 * default colormap.
	 */
	if (visual_info[choice].visual != DefaultVisual(display,screen)
	    || !default_map) {
	    cmap = XCreateColormap(device_data->display,
				   XDefaultRootWindow(device_data->display),
				   visual_info[choice].visual,AllocNone);

	    window_attributes.colormap = cmap;
	    attr_mask |= CWColormap;
	}
	else cmap = DefaultColormap(display,screen);

	visual = visual_info[choice].visual;

	device_data->window = XCreateWindow(device_data->display,
					    DefaultRootWindow(device_data->display),
					    xsh.x, xsh.y, xsh.width, xsh.height, 0,
					    depth,InputOutput,visual,
					    attr_mask,&window_attributes);

	XFree(visual_info);

	if(!(device_data->window)) {
	    DDerror(ERR_DEVICE_SYSTEM_ACCESS,funcname,
		       "Could not create X11 window");
	    return DcFalse;
	}

	XStoreName(device_data->display,device_data->window,
		   "Dore Window");
	XSetIconName(device_data->display,device_data->window,
		     "Dore Window");
	XSetWMHints(device_data->display,device_data->window,&xwmh);
	XSetSizeHints(device_data->display,device_data->window,
		      &xsh,XA_WM_NORMAL_HINTS);
	class_hints.res_name = "dore";
	class_hints.res_class = "Dore";
	XSetClassHint(device_data->display,device_data->window,
		      &class_hints);
    }

    device_data->gc = XCreateGC(device_data->display,
				device_data->window,0,0);
    if(!(device_data->gc)) {
	DDerror(ERR_DEVICE_SYSTEM_ACCESS,funcname,
		   "Could not create graphics context");
	return DcFalse;
    }

    if(!(device_data->user_window)) {
	XSelectInput(device_data->display,device_data->window,
		     ExposureMask);
	XMapWindow(device_data->display,device_data->window);

	while(XNextEvent(device_data->display,&event)) {
	    if(event.type == Expose)
		    break;
	}
    }

    /*add an XSync for bug in Xlib. See TRs Dore-0657 and 0707 */
    XSync(device_data->display, False);
    XGetWindowAttributes(device_data->display,device_data->window,
			 &(device_data->window_attributes));

    device_data->width = device_data->window_attributes.width;
    device_data->height = device_data->window_attributes.height;
    visual = device_data->window_attributes.visual;
    cmap = device_data->window_attributes.colormap;

    if(visual->class == PseudoColor) {
	XColor tmpcolor;

	device_data->visualtype = DcPseudoColor;

	tmpcolor.red = tmpcolor.green = tmpcolor.blue = 0;

	if ((DefaultColormapOfScreen(device_data->window_attributes.screen) != 
	     cmap)) {
             if (!XAllocColor(display, cmap, &tmpcolor)) {
	        /*
	         * The colormap is not the default colormap and it was
	         * not possible to allocate a read-only color in this colormap
	         * therefore this colormap all read/write.
	         *
	         * So Create a new colormap AllocNone
	         */
	        cmap = XCreateColormap(device_data->display,
				   XDefaultRootWindow(device_data->display),
				   visual,AllocNone);

	        DDerror (ERR_DEVICE_SYSTEM_ACCESS, funcname,
			"can't get read-only colorcell: creating new colormap");

	        XSetWindowColormap(display,device_data->window, cmap);
    		/*add an XSync for bug in Xlib. See TRs Dore-0657 and 0707 */
    		XSync(device_data->display, False);
	        XGetWindowAttributes(device_data->display,device_data->window,
				 &(device_data->window_attributes));
	        if (cmap != device_data->window_attributes.colormap) {
		    DDerror(ERR_DEVICE_SYSTEM_ACCESS,funcname,
			   "window attributes.colormap doesn't match cmap");
	        }
	    }
            else
            { 
               /* free the "dummy" allocated cell */
               XFreeColors(display, cmap, &tmpcolor.pixel, 1, 0);
            }
        }

	/* 
	 * If we haven't been given any guidlines, choose the best
	 * number of colors that we can by looking at how many
	 * colortable entries we can allocate in this colormap.
	 */
	if (nred == -1 || ngreen == -1 || nblue == -1) {
	    int entries;

	    entries = available_entries(display,visual,cmap);
	    if (entries == 0) {
		DDerror(ERR_DEVICE_SYSTEM_ACCESS,funcname,
			   "could not get color map with available entries");
		return DcFalse;
	    }

	    if (entries >= MAX_PSEUDO) {
		nred = MAX_RED;
		ngreen = MAX_GREEN;
		nblue = MAX_BLUE;
	    }
	    else if (entries >= 256) {
		nred = 8;
		ngreen = 8;
		nblue = 4;
	    }
	    else {		/* This is the min that we will try and alloc */
		nred = 4;
		ngreen = 4;
		nblue = 4;
	    }
	}

	if (!set_pseudo_ramp(cmap,nred,ngreen,nblue,device_data)) {
	    cmap = XCreateColormap(device_data->display,
				   XDefaultRootWindow(device_data->display),
				   visual,AllocNone);

	    DDerror (ERR_DEVICE_SYSTEM_ACCESS, funcname,
			"can't allocate colors from colormap: creating new colormap");

	    XSetWindowColormap(display,device_data->window, cmap);
	    /*add an XSync for bug in Xlib. See TRs Dore-0657 and 0707 */
   	    XSync(device_data->display, False);
	    XGetWindowAttributes(device_data->display,device_data->window,
				 &(device_data->window_attributes));
	    if (cmap != device_data->window_attributes.colormap) {
		DDerror(ERR_DEVICE_SYSTEM_ACCESS,funcname,
			   "window attributes.colormap doesn't match cmap");
	    }
	    if (!set_pseudo_ramp(cmap,nred,ngreen,nblue,device_data)) {
	        DDerror(ERR_DEVICE_SYSTEM_ACCESS,funcname,
		      "could not allocate colors from colormap");
		return DcFalse;
	    }
	}
	device_data->fg_color_index = 0;
    }
    else if(visual->class == StaticGray) {
	device_data->visualtype = DcStaticGrey;

	device_data->bg_color_index = 
		BlackPixelOfScreen(device_data->window_attributes.screen);
	device_data->fg_color_index = 
		WhitePixelOfScreen(device_data->window_attributes.screen);
    }
    else if(visual->class == TrueColor) {
	device_data->visualtype = DcTrueColor;

	/* Now set up the color table properly */
	nred = count_bits(visual->red_mask);
	ngreen = count_bits(visual->green_mask);
	nblue = count_bits(visual->blue_mask);
	set_true_ramp(visual, 1 << nred,1 << ngreen,1 << nblue,
		      device_data);
	device_data->fg_color_index = 0;
    }
    else
	    {
		DDerror(ERR_DEVICE_SYSTEM_ACCESS,funcname,
			   "not support specified visual type.");
	    }

    XSetForeground(device_data->display,device_data->gc,
		   device_data->fg_color_index);

    device_data->image = XCreateImage(device_data->display,
				      device_data->window_attributes.visual,
				      device_data->window_attributes.depth,
				      ZPixmap,0,foo,1,1,32,0);

    /* Look for the first pixmap format that has this depth -- use it's
       depth as the bits per pixel.  Pre-R4 servers allowed us to look
       directly at the pixmap_format field; >=R5 servers likely do not. */

#if defined(X11_PRE_R4) || defined(XLIB_ILLEGAL_ACCESS)
    for (fmt = device_data->display->pixmap_format; 
	 fmt->depth != device_data->window_attributes.depth; fmt++)
#else
    pixmap_formats = XListPixmapFormats(device_data->display, &npixmap_formats);
    for (fmt = pixmap_formats; 
	 fmt->depth != device_data->window_attributes.depth; fmt++)
#endif
	    ;
    device_data->bytes_per_pixel = fmt->bits_per_pixel/8;

    if(device_data->simulator)
	    ddr_stdx11_InitSimulator();

    XClearWindow(device_data->display,device_data->window);

    if(device_data->doubleBuffered) {
	device_data->draw_buf =
		device_data->pixmap =
			XCreatePixmap(device_data->display,device_data->window,
				      device_data->width,device_data->height,
				      device_data->window_attributes.depth);
	if(!(device_data->draw_buf)) {
	    DDerror(ERR_DEVICE_SYSTEM_ACCESS,funcname,
		       "could not create Pixmap");
	}
    }
    else {
	device_data->draw_buf = device_data->window;
	device_data->pixmap = 0;
    }

#ifdef DEBUG
    printf("end of initial driver\n");
    fflush(stdout);
#endif

    return DcTrue;

#   undef device_data
}

/* ====================================================================== */

void ddr_stdx11_dcm_close_device (void)
{
   ddt_stdx11_data *device_data;

#ifdef DEBUG
   printf("... processing device close device ...\n");
   fflush(stdout);
#endif

   device_data = dde_stdx11_current_device_data;

   XFlush(device_data->display);


   if (!device_data->user_window) {
      XUnmapWindow(device_data->display,device_data->window);
      XDestroyWindow(device_data->display,device_data->window);
   }

   XFreeGC(device_data->display,device_data->gc);

   if (!device_data->user_display) {
      XCloseDisplay(device_data->display);
   }
   else XFlush(device_data->display);

   if (device_data->visualtype == DcPseudoColor && 
       device_data->pseudomap != NULL)
	 DDspace_Deallocate(device_data->pseudomap);

   /* Don't do a destroy image because there is no data here */
   XFree(device_data->image);

   if(device_data->simulator)
	 ddr_stdx11_FreeBuffer();
}

/* ====================================================================== */

void ddr_stdx11_dcm_flush (void)
{
#ifdef DEBUG
   printf("... processing device flush ...\n");
   fflush(stdout);
#endif

   XFlush(dde_stdx11_current_device_data->display);
}

/* ====================================================================== */

void ddr_stdx11_dcm_swap_buffers (void)
{
#ifdef DEBUG
   printf("...processing device swap buffers \n");
   fflush(stdout);
#endif

   if(dde_stdx11_current_device_data->doubleBuffered) {
      XCopyArea(dde_stdx11_current_device_data->display,
		dde_stdx11_current_device_data->draw_buf,
		dde_stdx11_current_device_data->window,
		dde_stdx11_current_device_data->gc,0,0,
		dde_stdx11_current_device_data->width,
		dde_stdx11_current_device_data->height,0,0);
      XFlush(dde_stdx11_current_device_data->display);
   }
}

/* ====================================================================== */

void ddr_stdx11_dcm_update_geometry (void)
{
#ifdef DEBUG
   printf("...processing device update geometry\n");
   fflush(stdout);
#endif

   if(dde_stdx11_current_device_data->update_needed == DcFalse) {
      return;
   }

   /*add an XSync for bug in Xlib. See TRs Dore-0657 and 0707 */
   XSync(dde_stdx11_current_device_data->display, False);
   XGetWindowAttributes(dde_stdx11_current_device_data->display,
			dde_stdx11_current_device_data->window,
			&(dde_stdx11_current_device_data->window_attributes));

   dde_stdx11_current_device_data->width = 
	 dde_stdx11_current_device_data->window_attributes.width;
   dde_stdx11_current_device_data->height = 
	 dde_stdx11_current_device_data->window_attributes.height;

   if(dde_stdx11_current_device_data->simulator)
	 ddr_stdx11_UpdateSimulator();

   /* 
    * Need to create a new pixmap even if double buffering is currently
    * off because someone may turn it back on again.  
    */
   if(dde_stdx11_current_device_data->pixmap != 0) {
      XFreePixmap(dde_stdx11_current_device_data->display,
		  dde_stdx11_current_device_data->pixmap);

      dde_stdx11_current_device_data->pixmap = 
	    XCreatePixmap(dde_stdx11_current_device_data->display,
			  dde_stdx11_current_device_data->window,
			  dde_stdx11_current_device_data->width,
			  dde_stdx11_current_device_data->height,
			  dde_stdx11_current_device_data
			  ->window_attributes.depth);
      if(!(dde_stdx11_current_device_data->pixmap)) {
	 DDerror(ERR_DEVICE_SYSTEM_ACCESS,"update geometry",
		    "could not create pixmap");
      }
      if (dde_stdx11_current_device_data->draw_buf !=
	  dde_stdx11_current_device_data->window) {
	 dde_stdx11_current_device_data->draw_buf = 
	       dde_stdx11_current_device_data->pixmap;
      }
   }

   dde_stdx11_current_device_data->update_needed = DcFalse;
}

/* ====================================================================== */

void ddr_stdx11_dcm_become_current_driver (DtPtr device_data)
{
#ifdef DEBUG
    printf("... processing device become current driver ...\n");
    fflush(stdout);
#endif

    dde_stdx11_current_device_data = device_data;

    ddr_softpipe_drom_set_output_functions
    (   (DtPFV) ddr_stdx11_DrawPoint,
	(DtPFV) ddr_stdx11_DrawLine,
	(DtPFV) ddr_stdx11_DrawTriangle
    );

    if (((ddt_stdx11_data*)(device_data)) -> simulator) {
	ddr_zbuffer_SetBuffer
	(   ((ddt_stdx11_data*)(device_data)) -> width,
	    ((ddt_stdx11_data*)(device_data)) -> height,
	    ((ddt_stdx11_data*)(device_data)) -> depth,
	    ddr_stdx11_PutPixel
	);
    }
}

/* ====================================================================== */

void ddr_stdx11_dcm_set_options (DtPtr argstring)
{
    char	    *argstringcopy;
    char	    *tmp;
    char	    *p;
    ddt_stdx11_data *devicedata;


    devicedata = dde_stdx11_current_device_data;

    argstringcopy = (char *)DDspace_Allocate(strlen(argstring)+1);
    strcpy(argstringcopy, argstring); /* so as not to alter user's string */
    tmp = argstringcopy;

    /* p points to first sub-string */

    while ((p = DDstrtok(tmp, " \t,"))) {
	tmp = 0;

	if (!strcmp(p,"-dither")) {
	    devicedata->dither = 1;

	} else if (!strcmp(p,"-nodither")) {
	    devicedata->dither = 0;

	} else if (!strcmp(p, "-doublebuffered")) {
	    devicedata->doubleBuffered = 1;

	    if (devicedata->pixmap != 0) {
		devicedata->draw_buf = devicedata->pixmap;
	    } else {
		devicedata->draw_buf = devicedata->pixmap =
			XCreatePixmap(devicedata->display,devicedata->window,
				      devicedata->width,devicedata->height,
				      devicedata->window_attributes.depth);
	    }

	} else if (!strcmp(p, "-singlebuffered")) {
	    devicedata->draw_buf = devicedata->window;
	    devicedata->doubleBuffered = 0;

	} else if (!strcmp(p, "-zbuffer")) {
	    if (!(devicedata->simulator)) {
		devicedata->simulator = 1;
		ddr_stdx11_InitSimulator();
	    }

	} else if (!strcmp(p, "-nozbuffer")) {
	    if (devicedata->simulator) {
		devicedata->simulator = 0;
		ddr_stdx11_FreeBuffer();
	    }
	}
    }

    DDspace_Deallocate(argstringcopy);
}

/* ====================================================================== */

void ddr_stdx11_dcm_setup_shading (void)
{
   DtInt range_min, range_max;
   ddt_stdx11_data *dev;

#ifdef DEBUG
   printf("...processing device setup shading \n");
   fflush(stdout);
#endif

   dev = dde_stdx11_current_device_data;
   dev->shade_mode = DdInqShadeMode(dev->device);
   if(dev->shade_mode == DcRange) {
      DDdevice_InqShadeRange(dev->device,
			  dev->shade_index, &range_min, &range_max);
#ifdef DEBUG
      printf("index: %d, range: %d - %d\n",
	     dev->shade_index,
	     range_min,range_max);
      fflush(stdout);
#endif
      dev->cwtavgpslim[0] = (DtReal)range_min;
      dev->cwtavgpslim[1] = (DtReal)range_max;
      dev->cwtavgpslim[2] = (dev->cwtavgpslim[1] - 
			     dev->cwtavgpslim[0]);
   }
}

/* ====================================================================== */

void ddr_stdx11_InitSimulator (void)
{
    ddt_stdx11_data *device_data;
    DtInt size;

#ifdef DEBUG
    printf("...processing ddr_stdx11_InitSimulator\n");
    fflush(stdout);
#endif

    device_data = dde_stdx11_current_device_data;

    size = device_data->width * device_data->height * sizeof(DtUShort);

#ifdef DEBUG
    printf("size = %d\n",size);
#endif

    device_data->depth = (DtUShort *)DDspace_Allocate(size);
    if(!(device_data->depth))
	    DDerror(ERR_DEVICE_SYSTEM_ACCESS,"ddr_stdx11_InitSimulator",
		       "Not enough memory");

    ddr_zbuffer_SetBuffer(device_data->width, device_data->height,
			  device_data->depth,
			  ddr_stdx11_PutPixel);

    ddr_zbuffer_Clear();
}

/* ====================================================================== */

void ddr_stdx11_FreeBuffer (void)
{
#ifdef DEBUG
    printf("...processing ddr_stdx11_FreeBuffer\n");
    fflush(stdout);
#endif

    DDspace_Deallocate(dde_stdx11_current_device_data->depth);

    dde_stdx11_current_device_data->depth = 0;

}

/* ====================================================================== */

void ddr_stdx11_UpdateSimulator (void)
{
    ddt_stdx11_data *device_data;
    DtInt size;

#ifdef DEBUG
    printf("...processing ddr_stdx11_UpdateSimulator\n");
    fflush(stdout);
#endif

    device_data = dde_stdx11_current_device_data;

    size = device_data->width * device_data->height * sizeof(DtUShort);

#ifdef DEBUG
    printf("size = %d\n",size);
#endif

    device_data->depth = (DtUShort *)DDspace_Reallocate(device_data->depth,size);
    if(!(device_data->depth))
	    DDerror(ERR_DEVICE_SYSTEM_ACCESS,"ddr_stdx11_UpdateSimulator",
		       "Not enough memory");

    ddr_zbuffer_SetBuffer(device_data->width, device_data->height,
			  device_data->depth,
			  ddr_stdx11_PutPixel);

    ddr_zbuffer_Clear();
}

/*
 * available_entries
 *
 *   This routine tries to home in on the number of available colormap
 * entries in the given pseudo color colormap.  It will only work 
 * for pseudo color maps and is slightly pessimistic because it looks for 
 * read/write entries, not read-only ones (which we will eventually 
 * allocate).
 */

static int available_entries (
    Display *dpy,
    Visual *visual,
    Colormap cmap)
{
    int pixels[MAX_PSEUDO];
    int masks;
    int npixels;

    npixels = (visual->map_entries > MAX_PSEUDO ? MAX_PSEUDO : 
	       visual->map_entries);
   
    while (npixels > 20) {
	if (XAllocColorCells(dpy,cmap,0,&masks,0,pixels,npixels)) {
	    XFreeColors(dpy,cmap,pixels,npixels,0);
	    return(npixels);
	}
	npixels -= npixels / 10;
    }
    return(0);
}

/* ====================================================================== */

static int set_pseudo_ramp (
    Colormap cmap,
    int nred,
    int ngreen,
    int nblue,
    ddt_stdx11_data *device_data)
{
    float rscale, gscale, bscale;
    int r, g, b, i;
    XColor color;

    device_data->nred = nred;
    device_data->ngreen = ngreen;
    device_data->nblue = nblue;

    device_data->pseudomap = (unsigned long *)DDspace_Allocate(nred * ngreen * 
						     nblue * sizeof(unsigned long));
    rscale = 255.0 / ((float) nred-1.0);
    gscale = 255.0 / ((float) ngreen-1.0);
    bscale = 255.0 / ((float) nblue-1.0);
    for (i = r = 0; r < nred; r++) {
	for (g = 0; g < ngreen; g++) {
	    for (b = 0; b < nblue; b++, i++) {
		color.red = ((int) (r * rscale)) << 8;
		color.green = ((int) (g * gscale)) << 8; 
		color.blue = ((int) (b * bscale)) << 8; 
		if (!XAllocColor(device_data->display,cmap,&color)) return(0);
		device_data->pseudomap[i] = color.pixel;
	    }
	}
    }
    /* Accelerators to help quickly convert R,G,B's to indices */
    device_data->rscale = (float) ngreen * nblue;
    device_data->gscale = (float) nblue;
    device_data->bscale = (float) 1;

    return(1);
}

/* ====================================================================== */

static void set_true_ramp (
    Visual *visual,
    int nred,
    int ngreen,
    int nblue,
    ddt_stdx11_data *device_data)
{
    int r, g, b;
    int redshift, greenshift, blueshift;
    float rscale, gscale, bscale;

    redshift = count_zeros(visual->red_mask);
    greenshift = count_zeros(visual->green_mask);
    blueshift = count_zeros(visual->blue_mask);

    rscale = nred / 256.0;
    gscale = ngreen / 256.0;
    bscale = nblue / 256.0;

    for (r = 0; r < 256; r++)
	    device_data->redmap[r] = ((int)(r * rscale)) << redshift;
    for (g = 0; g < 256; g++)
	    device_data->greenmap[g] =((int)(g * gscale)) << greenshift; 
    for (b = 0; b < 256; b++)
	    device_data->bluemap[b] = ((int)(b * bscale)) << blueshift; 

    device_data->nred = nred;
    device_data->ngreen = ngreen;
    device_data->nblue = nblue;
}

/* ====================================================================== */

static int count_zeros (unsigned int val)
{
    int ct;
    unsigned int p;
    for (ct = 0, p = 1; !(p & val) && p; p <<= 1, ct++)
	    ;
    return(ct);
}

/* Return the number of set bits in a function */
static int count_bits (unsigned int val)
{
    int ct;
    unsigned int p;
    for (ct = 0, p = 1; p; p <<= 1) if (val & p) ct++;
    return(ct);
}

/* ====================================================================== */

int ddr_stdx11_dcm_color_to_pixel (
    DtReal c[3],
    ddt_stdx11_data *dev)
{
    int ind = 0;

    switch (dev->visualtype) {
	case DcPseudoColor:
	    if (dev->shade_mode == DcComponent) {
		ind = ((int)(c[0] * (dev->nred-1) + 0.5)) * dev->rscale + 
			((int)(c[1] * (dev->ngreen-1) + 0.5)) * dev->gscale + 
				(int)(c[2] * (dev->nblue-1) + 0.5);
		ind = dev->pseudomap[ind]; 
	    } else {
		ind = dev->cwtavgpslim[0]
			+ (dev->cwtavgpsweight[0]*c[0]
			   + dev->cwtavgpsweight[1]*c[1]
			   + dev->cwtavgpsweight[2]*c[2])
				* dev->cwtavgpslim[2];
		if(ind > dev->cwtavgpslim[1]) ind = dev->cwtavgpslim[1];
		ind = dev->pseudomap[ind];
	    }
	    break;

	case DcTrueColor:
	    ind = dev->redmap[(int)(c[0] * 255.0)] +
		    dev->greenmap[(int)(c[1] * 255.0)] +
			    dev->bluemap[(int)(c[2] * 255.0)];
	    break;

	case DcStaticGrey:
	case DcGreyScale:
	case DcStaticColor:
	case DcDirectColor:
	default:
	    break;
    }

    return (ind);
}
