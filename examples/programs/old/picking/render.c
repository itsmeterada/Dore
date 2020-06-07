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
This program demonstrates examples of the following:
	1: Dore picking functionality.
	2: Creating most kinds of Dore primitives.
	3: Dore "compute bounding volume" functionality.
	4: creating single buffered, double buffered and stereo windows.
	5: using overlay graphics.
	   to use overlay graphics define USE_OVERLAY.
	   if the overlay extension is Xov define USE_Xov_EXTENSION
*/

#include <dore/dore.h>
#include <dore/dore_util.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#ifdef titan
#include <X11/XTitan.h>
#endif
#ifdef USE_Xov_EXTENSION
#include <X11/XOverlay.h>
#endif


main(argc,argv)
int argc;
char *argv[];
{
    char argstring[200];
    DtObject device, frame, view;
    DtObject camera_group, object_group;
    DtVolume volume;
    static DtReal background_color[3] = {.2,.3,.5};

    static DtPoint3 pck_pnt = {250.,250.,32768.};
    DtInt hit_count;
    DtInt index[10];
    DtReal z_values[10];
    DtObject views[10];
    DtInt hit_list[1024];
    DtInt e_flag;
    DtInt i, height;
    static DtSize3 aperture = {25., 20., 131072.};

    XVisualInfo *visual_info, vinfo_template, *overlay_visual;
    int nvisuals;
    Colormap cmap;
    Display *displayPtr;
    GC overlay_gc;
    Window  window, overlay_win;
    char *displayName;
    extern char *getenv();
    int screen;
    XSetWindowAttributes attributes;
    XEvent event;
    XGCValues gcvalues;
    int stereo;
    int xap,yap;
#ifdef titan
    int G2_server;
#endif


    /* initialize Dore */
    DsInitialize(0);


    /* create an X11 window for Dore to draw into */

    if((displayName = getenv("DISPLAY")) == NULL){
	printf("main: couldn't get display name\n");
	exit(1);
    }
    if((displayPtr = XOpenDisplay(displayName)) == NULL ){
	printf("couldn't open display\n");
	exit(1);
    }

#ifdef titan
    G2_server = VendorRelease(displayPtr) < 400;
#endif

    prsarg_get_keyword_int(argc, argv, "-stereo", 0, &stereo);

#ifdef titan
    if(stereo && G2_server)
	    screen = 1;
    else
#endif
	    screen = DefaultScreen(displayPtr);

    vinfo_template.screen = screen ;
    vinfo_template.depth = 24;

#ifdef titan
    if (G2_server)
	    vinfo_template.class = DirectColor ;
    else
#endif
	    vinfo_template.class = TrueColor ;
	    
    visual_info = XGetVisualInfo(displayPtr,
				 VisualScreenMask|VisualClassMask|VisualDepthMask,
				 &vinfo_template, &nvisuals) ;

    if (visual_info->visual == DefaultVisual(displayPtr,screen)) {
	printf ("using DefaultColormap of screen %d\n", screen);
	cmap = DefaultColormap(displayPtr,screen);
#ifdef titan
    } else if (G2_server) {
	cmap = XTitanDefaultDirectColormap(displayPtr,screen);
#endif
    } else {
	cmap = XCreateColormap(displayPtr, RootWindow(displayPtr,screen),
			       visual_info->visual,AllocNone);
    }

    attributes.background_pixel = 0x000000;
    attributes.border_pixel = 0xffffff;
    attributes.colormap = cmap ;

#ifdef titan
    if (G2_server) {
	if(inq_expansion_board()){
	    if(stereo) {
		window = XBCreateStereoWindow(displayPtr,
				RootWindow(displayPtr, screen), 5, 5, 500, 250, 5,
				24, InputOutput, visual_info->visual, 2, 1,
				CWBackPixel|CWBorderPixel|CWColormap, &attributes);
	    } else {
		window = XBCreateWindow(displayPtr,
				RootWindow(displayPtr, screen), 5, 5, 500, 500, 5,
				24, InputOutput, visual_info->visual, 2, 1,
				CWBackPixel|CWBorderPixel|CWColormap, &attributes);
	    }
	} else {
	    if(stereo) {
		window = XBCreateWindow(displayPtr,
				RootWindow(displayPtr, screen), 5, 5, 500, 250, 5,
				24, InputOutput, visual_info->visual, 2, 1,
				CWBackPixel|CWBorderPixel|CWColormap, &attributes);
	    } else {
		window = XCreateWindow(displayPtr,
				RootWindow(displayPtr, screen), 5, 5, 500, 500, 5,
				24, InputOutput, visual_info->visual,
				CWBackPixel|CWBorderPixel|CWColormap, &attributes);
	    }
	}
    } else 
#endif
    {
	window = XCreateWindow(displayPtr, 
				RootWindow(displayPtr, screen), 5, 5, 500, 500, 5,
				24, InputOutput, visual_info->visual,
				CWBackPixel|CWBorderPixel|CWColormap, &attributes);
    }


    if(NULL == window) {
	printf("cannot open graphics window.\n");
	exit(1);
    }

    XSelectInput(displayPtr, window, ExposureMask);
    XMapWindow(displayPtr, window);
    while(XNextEvent(displayPtr, &event)){
	if (event.type == Expose)
		break;
    }
    XFlush(displayPtr);

	
#ifdef USE_OVERLAY
    /* create the overlay window for drawing the pick aperture */
    nvisuals = 0;

#ifdef titan
    if (G2_server) {
	vinfo_template.visualid = XTitanOverlayVisualId(displayPtr, screen);

	overlay_visual = XGetVisualInfo( displayPtr, VisualIDMask,
					&vinfo_template, &nvisuals);
    }
#endif
#ifdef USE_Xov_EXTENSION
#ifdef titan
    else
#endif	    
    {
	XovClassMap *overlay_classes;
	int *num_overlay_classes;

	if (!XovInitExtension(displayPtr)) {
	    printf ("no overlay extension\n");
	    exit (1);
	}

	if (!(overlay_classes = XovListClasses(displayPtr, &num_overlay_classes))) {
	    printf ("XovListClasses did not return any overlay visual classes\n");
	    exit (1);
	}

	vinfo_template.class = overlay_classes[0].value;

	overlay_visual = XGetVisualInfo(displayPtr, VisualClassMask,
					&vinfo_template, &nvisuals);

	XFree (num_overlay_classes);
	XFree (overlay_classes);
    }
#endif

    if (nvisuals == 0) {
	printf("no overlay visual\n");
	exit(1);
    }

    attributes.border_pixel = 0;
    attributes.background_pixmap = 0;

#ifdef titan
    if (G2_server) {
	attributes.colormap = XTitanDefaultOverlayColormap(displayPtr,screen);
    } else 
#endif
    {
	attributes.colormap = XCreateColormap(displayPtr, 
					      DefaultRootWindow(displayPtr),
					      overlay_visual->visual,
					      AllocNone);
    }

    overlay_win = XCreateWindow(displayPtr, window, 0, 0, 1280, 1024, 0, 
				overlay_visual->depth, InputOutput, 
				overlay_visual->visual,
				CWBackPixmap|CWBorderPixel|CWColormap, &attributes);
    if(NULL == overlay_win) {
	printf("cannot open overlay window.\n");
	exit(1);
    }

    XSelectInput(displayPtr, overlay_win, ExposureMask);
    XMapWindow(displayPtr, overlay_win);
    while(XNextEvent(displayPtr, &event)){
	if (event.type == Expose)
		break;
    }
    XFlush(displayPtr);

    overlay_gc = XCreateGC(displayPtr, overlay_win, 0, 0);
    gcvalues.function = GXxor;

#ifdef titan
    if (G2_server) {
	gcvalues.foreground = 2;	/* black lines */
    } else
#endif
    {
	XColor def, unused;

	XLookupColor(displayPtr, attributes.colormap, "black", &unused, &def);
	XAllocColor (displayPtr, attributes.colormap, &def);

	gcvalues.foreground = def.pixel;
    }
    XChangeGC(displayPtr, overlay_gc, GCForeground | GCFunction, &gcvalues);
#endif

    /* set up the Dore environment */

    sprintf(argstring, "-window %d -display %d", window, displayPtr);
    device = DoDevice("stdx11", argstring);
    if (device == DcNullObject){
	printf("can't create device\n");
	exit(1);
    }

    DdInqExtent(device,&volume);
    height = volume.fur[1] - volume.bll[1];

    frame = DoFrame();
    DdSetFrame(device, frame);
    DfSetBoundary(frame, &volume);

    view = DoView();
    DvSetBackgroundColor(view, DcRGB, background_color);

    DgAddObjToGroup(DfInqViewGroup(frame), view);

    DvSetBoundary(view, &volume);

    make_objects();

    /* look up the object handles of the groups created by make_objects. */
    camera_group = DsInqObj(DcNameString,"cam",DcTypeGroup);
    object_group = DsInqObj(DcNameString,"obj",DcTypeGroup);

    if((camera_group == DcNullObject) || (object_group == DcNullObject)){
	printf("can't find camera or object group\n");
	exit(1);
    }

    DgAddObjToGroup(DvInqDisplayGroup(view), object_group);

    DgAddObjToGroup(DvInqDefinitionGroup(view), camera_group);

    DvUpdate(view); 
    DvSetUpdateType(view, DcUpdateDisplay);

    prsarg_get_keyword_int(argc,argv,"-xap",25,&xap);
    aperture.width = xap;
    prsarg_get_keyword_int(argc,argv,"-yap",20, &yap);
    aperture.height = yap;
    prsarg_check_bad_arguments(argc,argv);
    DdSetPickAperture(device, &aperture);

    DdSetPickCallback(device, DcPickAll);

    /* call the function that gets the picking point and pass to the
       dore pick function, then print the information returned. */

    while(GetPickPoint(pck_pnt, xap, yap, height, displayPtr, window,
		       overlay_win, overlay_gc)){
	printf("pick point is (%f, %f, %f)\n",
	       pck_pnt[0], pck_pnt[1], pck_pnt[2]);
	printf("\n\n\n******** BEGIN PICK ********\n");
	DdPickObjs(device, pck_pnt, &hit_count, 10, index, 1024, hit_list, 
			z_values, DcNullPtr, DcNullPtr, views, &e_flag);
	printf("******** END PICK ********\n");
	
	printf("\n\n******** PICK REPORT ********\n");
	printf("accepted %d hits, eflag = %0x (hex)\n",
	       hit_count,e_flag);
	clear_colors();
	for(i=0; i<hit_count; i++) {
	    int id;
	    printf("hit %d, z = %f, view = %d, path:\n",
		   i, z_values[i], views[i]);
	    id = PrintPath((index[i+1]-index[i]) /3,
			   hit_list+index[i]);
	    color_it(id);
	}
	DvUpdate(view);
    }

    DsReleaseObj(device);
    XUnmapWindow(displayPtr, window);
    XDestroyWindow(displayPtr, window);
    XCloseDisplay(displayPtr);
    DsTerminate();
}







/* print the contents of a pick path and */
/* returns the pick id for the primitive */

int PrintPath(Size, Path)
DtInt Size, Path[];

{
    DtNameType type;
    DtPtr *name ;
    DtInt i;

    printf("\tgroup     |  pick id  |  ele seq # \n");
    printf("\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    for(i=0; i<Size; i++){
	switch(DsInqObjStatus((DtObject)Path[3*i])){
	case DcObjectValid:
	    DsInqObjName((DtObject)Path[3*i],&type,&name); 
	    switch(type){
	    case DcNameInteger:
		printf("\t%d", *(int *)name);
		break;
	    case DcNameString:
		printf("\t%s", name);
		break;
	    case DcNameNone:
		printf("\t%s", "no name");
		break;
	    default: printf("unknown name type\n");
		break;
	    }
	    break;
	case DcObjectInvalid:
	    printf("deleted!\n");
	    break;
	case DcObjectDeleted:
	    printf("invalid!\n");
	    break;
	default: printf("unknown validity type\n");
	    break;
	}
	printf("\t\t%d\t\t%d\n",Path[3*i+1],Path[3*i+2]);
    }
    putchar('\n');
    return(Path[3*Size - 2]);
}





/* returns into pnt the x and y values of the position of the X pointer */
/* within the Dore window, in Dore device coordinates.                  */
/* the function return value is 1 if the left mouse button was clicked, */
/* 0 otherwise.                                                         */
/* Uses overlay graphics to track the movement of the pointer with a    */
/* box with width and height of xap and yap.                            */

int GetPickPoint(pnt, xap, yap, height, displayPtr, w, ow, ogc)
DtPoint3 pnt;
int xap,yap;
DtInt height;
Display *displayPtr;
Window w, ow;
GC ogc;
{
    XEvent event;
    static int x=0, y=0;
    unsigned int j;

    printf("\nleft mouse button picks (others to quit)\n");
    XSelectInput(displayPtr, w, PointerMotionMask|ButtonPressMask);
#ifdef USE_OVERLAY
    XDrawRectangle(displayPtr, ow, ogc,
		   x - xap/2, y - xap/2, xap, yap); /* draw box */
#endif
    while(1){
	XNextEvent(displayPtr, &event);
	if(event.type == MotionNotify){
#ifdef USE_OVERLAY
	    XDrawRectangle(displayPtr, ow, ogc,
			   x - xap/2, y - xap/2, xap, yap); /* erase box */
#endif
	    x = event.xmotion.x;
	    y = event.xmotion.y;
#ifdef USE_OVERLAY
	    XDrawRectangle(displayPtr, ow, ogc,
			   x - xap/2, y - xap/2, xap, yap); /* redraw it */
#endif
	}
	else {
	    x = event.xbutton.x;
	    y = event.xbutton.y;
	    j = event.xbutton.button;
	    if(1 == j){
		pnt[0] = (double)x;
		pnt[1] = (double)(height - y);
		break;
	    }
	    else
		    return(0);
	}
    }
#ifdef USE_OVERLAY
    XDrawRectangle(displayPtr, ow, ogc,
		   x - xap/2, y - xap/2, xap, yap); /* erase box */
#endif
    return(1);
}
