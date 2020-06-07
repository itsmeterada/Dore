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
 
#include <stdio.h>
#include <dore/dore.h>
#include "trnsfrmr.h"
#include "render.h"

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#ifdef titan
#include <X11/XTitan.h>
#include <sys/sysmips.h>
#endif

#define GRAPHIC_NONE    0x0
#define GRAPHIC_G2      0x1
#define GRAPHIC_G3      0x2
#define GRAPHIC_G4      0x4

Display		*displayPtr;
Window		Xwindow;
extern int	gr_type;

/* ====================================================================== */

int setup_x11_window (
    DtInt topx,
    DtInt topy,
    DtInt width,
    DtInt height,
    char *argstring)
{
   XVisualInfo *visual_info,vinfo_template;
   DtVisualType new_visualtype;
   int nvisuals;
   Colormap cmap;
   char *displayName;
   extern char *getenv();
   int screen;
   XSetWindowAttributes attributes;
   XEvent event;
   int buffercount;
   int attrMask;
   static XWMHints xwmh = {
      (InputHint|StateHint),/* flags */
      True,		/* input */
      NormalState,	/* initial_state */
      0,		/* icon pixmap */
      0,		/* icon window */
      0, 0,		/* icon location */
      0,		/* icon mask */
      0,		/* Window group */
   };
   XSizeHints  xsh;
   XClassHint class_hints;
   DtInt nplanes;

#if defined(use_DGL) && defined(XQUERYBUFFERS)
   unsigned short n_buffers_return;
   unsigned short display_buffer_return;
   unsigned short draw_buffer_return;
   unsigned char stereo_return;
   unsigned char left_return;
#endif

   xsh.flags = (USPosition|USSize);
   xsh.width = width;
   xsh.height = height;
   xsh.x = topx;
   xsh.y = topy;

   /* create an X11 window,and Dore device (window) */

   if ((displayName = getenv("DISPLAY")) == NULL) {
      printf ("main:  Couldn't get display name.\n");
      exit(1);
   }

   if ((displayPtr = XOpenDisplay(displayName)) == NULL ) {
      printf ("Couldn't open display.\n");
      exit(1);
   }

   screen = XDefaultScreen(displayPtr);

#if defined(use_DGL) || defined(use_VLL)
   if (stereoflag) {
      screen = 1;
   }
#endif

   vinfo_template.screen = screen;

   attributes.background_pixel = 0x000000;
   attributes.border_pixel = 0xffffff;
   attrMask = CWBackPixel|CWBorderPixel;

   switch (visualtype) {
      case DcTrueColor:
	 new_visualtype = DcTrueColor;
	 vinfo_template.class = TrueColor;
	 vinfo_template.depth = 24;
	 nplanes = 24;
	 visual_info = XGetVisualInfo(displayPtr,
			    VisualScreenMask|VisualClassMask|VisualDepthMask,
			    &vinfo_template,&nvisuals);
	 if (nvisuals > 0) {
	    if (visual_info->visual == DefaultVisual(displayPtr,screen)) {
		printf ("Using DefaultColormap (TrueColor) of screen %d.\n",
			screen);
		cmap = DefaultColormap(displayPtr,screen);
	    } else {
		cmap = XCreateColormap
		       (  displayPtr,
			  RootWindow (displayPtr, screen),
			  visual_info->visual,
			  AllocNone
		       );
	    }
	    if (!cmap) {
		printf ("Could not allocate color map.\n");
		exit(1);
	    }
	    attributes.colormap = cmap;
	    attrMask |= CWColormap;
	    
	    break;
	 }

      case DcDirectColor:
	 new_visualtype = DcDirectColor;
	 vinfo_template.class = DirectColor;
	 vinfo_template.depth = 24;
	 nplanes = 24;
	 visual_info = XGetVisualInfo(displayPtr,
			    VisualScreenMask|VisualClassMask|VisualDepthMask,
			    &vinfo_template,&nvisuals);
	 if (nvisuals > 0) {
#           if defined(titan)
	       if (use_Xtitan) 
		  cmap = XTitanDefaultDirectColormap(displayPtr,screen);
	       else
#           endif
	    cmap = XCreateColormap (displayPtr, RootWindow(displayPtr,screen),
				    visual_info->visual, AllocAll);
	    if (!cmap) {
	       printf ("Could not allocate color map.\n");
	       exit (1);
	    }
	    attributes.colormap = cmap;
	    attrMask |= CWColormap;
	    break;
	 }

      case DcPseudoColor:
	 new_visualtype = DcPseudoColor;
	 vinfo_template.class = PseudoColor;
	 vinfo_template.depth = 12;
	 nplanes = 12;
	 visual_info = XGetVisualInfo(displayPtr,
			    VisualScreenMask|VisualClassMask|VisualDepthMask,
			    &vinfo_template,&nvisuals);
	 if (nvisuals == 0) {
	    vinfo_template.class = PseudoColor;
	    vinfo_template.depth = 8;
	    nplanes = 8;
	    visual_info = XGetVisualInfo(displayPtr,
			    VisualScreenMask|VisualClassMask|VisualDepthMask,
			    &vinfo_template,&nvisuals);
	 }
	 if (nvisuals > 0) {
	    if (visual_info->visual == DefaultVisual(displayPtr,screen)) {
		printf ("Using DefaultColormap (PseudoColor) of screen %d.\n",
		    screen);
		cmap = DefaultColormap(displayPtr,screen);
	    } else {
		 cmap = XCreateColormap
			(  displayPtr,
			   RootWindow (displayPtr, screen),
			   visual_info->visual,
			   AllocAll
			);
		 if (!cmap) {
		     printf ("Could not allocate color map.\n");
		     exit(1);
		 }
		 setup_colormaps(visual_info->visual->map_entries);
	    }
	    setup_colormaps(visual_info->visual->map_entries);
	    attributes.colormap = cmap;
	    attrMask |= CWColormap;
	    break;
	 }

      case DcStaticGrey:
	 new_visualtype = DcStaticGrey;
	 vinfo_template.class = StaticGray;
	 vinfo_template.depth = 1;
	 nplanes = 1;
	 visual_info = XGetVisualInfo(displayPtr,
			    VisualScreenMask|VisualClassMask|VisualDepthMask,
			    &vinfo_template,&nvisuals);
	 if (nvisuals > 0) break;
      default:
	 nvisuals = 0;
  }

   if (nvisuals == 0) {
      printf ("Screen doesn't support the specified visual type.\n");
      exit (1);
   }

   if (visualtype != new_visualtype) {
      printf ("Using %s visual type, not %s visualtype.\n",
	      print_visual(new_visualtype), print_visual(visualtype));
      visualtype = new_visualtype;
   }
	      
#ifdef titan
   if (use_Xtitan) {
      if(VendorRelease(displayPtr) >= 211) {
	 if ((visualtype != DcPseudoColor) &&
	     !XTitanGetExpansionStatus(displayPtr, screen)) {
	    printf ("No expansion board so using single buffered mode.\n");
	    singlebuffered = DcTrue;
	 }
      }
   }

   if (use_XB) {
      if (singlebuffered) {
	 buffercount = 1;
      } else {
	 buffercount = 2;
      }

      if (!stereoflag) {
	 Xwindow = XBCreateWindow(displayPtr,
				  RootWindow(displayPtr,screen),
				  xsh.x, xsh.y, xsh.width, xsh.height, 0,
				  nplanes,InputOutput,visual_info->visual,
				  buffercount,1,
				  attrMask,&attributes);
      } else {
	 Xwindow = XBCreateStereoWindow(displayPtr,
					RootWindow(displayPtr,screen),
					xsh.x,xsh.y,xsh.width,xsh.height,0,
					nplanes,InputOutput,visual_info->visual,
					buffercount,1,
					attrMask,&attributes);
      }
   } else 
#endif
   {
      Xwindow = XCreateWindow(displayPtr,
			      RootWindow(displayPtr,screen),
			      xsh.x, xsh.y, xsh.width, xsh.height, 0,
			      nplanes,InputOutput,visual_info->visual,
			      attrMask,&attributes);
   }

   if (!Xwindow) {
      printf ("main:  Couldn't open window.\n");
      exit(1);
   }

   XStoreName(displayPtr,Xwindow,"Dore Window");
   XSetIconName(displayPtr,Xwindow,"Dore Window");
   XSetWMHints(displayPtr,Xwindow,&xwmh);
   XSetSizeHints(displayPtr,Xwindow,&xsh,XA_WM_NORMAL_HINTS);
   class_hints.res_name = "dore";
   class_hints.res_class = "Dore";
   XSetClassHint(displayPtr,Xwindow,&class_hints);

   XSelectInput(displayPtr,Xwindow,ExposureMask);
   XMapWindow(displayPtr,Xwindow);
   while(XNextEvent(displayPtr,&event)) {
      if (event.type == Expose)
	    break;
   }

   XFlush (displayPtr);

   sprintf
   (  argstring + strlen(argstring), 
#     ifdef sgi
	 " -geometry %dx%d+%d+%d -xdisplay %ld -xwindow %ld",
#     else
	 " -geometry %dx%d+%d+%d -display %ld -window %ld",
#     endif
      width, height,
      topx, topy,
      displayPtr, Xwindow
   );

   sprintf (argstring + strlen(argstring), " -visualtype %s",
      print_visual (visualtype));

      sprintf (argstring + strlen(argstring), "%s%s%s%s",
         singlebuffered		? " -singlebuffered" 		: "",
         stereoflag	 	? " -stereo" 			: "",
         multi_pass_transp==1	? " -multipass_transparency" 	: 
         multi_pass_transp==2	? " -sort_transparency"		: "",
         cylinders==1           ? " -cylinders "                :
         cylinders==2           ? " -cylindersnocap "           : "",
         use_spheres		? " -spheres" 			: "");

#if defined(use_DGL) && defined(XQUERYBUFFERS)
   XBQueryBuffers(displayPtr,Xwindow, &n_buffers_return, &stereo_return,
		  &left_return, &display_buffer_return, &draw_buffer_return);

   printf ("buffers = %d\n",n_buffers_return);
   printf ("stereo = %d\n",stereo_return);
   printf ("left = %d\n",left_return);
   printf ("display_buffer = %d\n",display_buffer_return);
   printf ("draw_buffer = %d\n",draw_buffer_return);
#endif
}
/*
  ======================================================================
 */
Bool check_reconfigure (display, event, args)
     Display *display;
     XEvent *event;
     char *args;
{
   printf ("In check reconfigure (display = 0x%x).\n", display);
   if (event->type == ConfigureNotify) {
      printf ("Found reconfigure.\n");
      *args = 1;
   }
   return False;
}
/*
  ======================================================================
 */
int x11_process_input (void)
{
   XEvent event, newevent;
   static DtPoint3 pck_pnt = {250., 250., 4000.};
   DtVolume volume, t_volume;
   static char buffer[10];
   static int bufsize = 10;
   int count;
   int i,j;
   DtReal range;
   KeySym keysym;
   XComposeStatus compose;
   char found_reconfigure;
   char reconfigure = 0;

   XSelectInput(displayPtr,Xwindow,
		KeyPressMask|ButtonPressMask|ExposureMask|StructureNotifyMask);

   while(1) {
      XNextEvent(displayPtr,&event);

      switch (event.type) {
	  case KeyPress:
	     count = XLookupString (&event, buffer, bufsize, &keysym, &compose);
	     for (i=0; i<count; i++) {
		if (process_character(buffer[i])) {
		   return;
		}
	     }
	     break;
	  case ButtonPress:
	     if (event.xbutton.button == Button1) {
		DdInqExtent(device, &volume);
		pck_pnt[0] = (DtReal)(event.xbutton.x);
		pck_pnt[1] = (volume.fur[1] - volume.bll[1]) - 
		      (DtReal)event.xbutton.y;
		pck_pnt[2] = (volume.fur[2] - volume.bll[2])/2.0;
		process_pick (pck_pnt);
	     }
	     break;
	  case Expose:
	     if (event.xexpose.count == 0) {
		if (reconfigure) {
		   printf ("Expose after reconfigure.\n");
		   reconfigure = 0;
		   for (i=0; i<4; i++) {
		      DvSetUpdateType(m_view[i], DcUpdateAll);
		   }
		   DvSetUpdateType(view, DcUpdateAll);
		   DdUpdate(device);
		   for (i=0; i<4; i++) {
		      DvSetUpdateType(m_view[i], DcUpdateDisplay);
		   }
		   DvSetUpdateType(view, DcUpdateDisplay);
		} else {
		   printf ("Pending events %d.\n", XPending (displayPtr));
		   found_reconfigure = 0;
		   XCheckIfEvent (displayPtr, &newevent, check_reconfigure, &found_reconfigure);
		   if (!found_reconfigure) {
		      printf ("Expose event.\n");
		      DdUpdate(device);
		   } else {
		      printf ("Reconfigure in queue.\n");
		   }
		}
	     }
	     break;
	  case ConfigureNotify:
	     printf ("Updating extents.\n");
	     DdInqExtent(device,&volume);
	     volume.bll[0] = 0.;
	     volume.bll[1] = 0.;
	     volume.fur[0] = event.xconfigure.width;
	     volume.fur[1] = event.xconfigure.height;
	     DfSetBoundary(frame,&volume);
	     DvSetBoundary(view,&volume);
	     for (i=0; i<4; i++) {
		for (j=0; j<3; j++) {
		   range = volume.fur[j] - volume.bll[j];
		   t_volume.bll[j] = volume.bll[j] + range*m_volume[i].bll[j];
		   t_volume.fur[j] = volume.bll[j] + range*m_volume[i].fur[j];
		}
		DvSetBoundary(m_view[i],&t_volume);
	     }
	     reconfigure = 1;
	     break;
      }
   }
}
/*
  ======================================================================
 */
