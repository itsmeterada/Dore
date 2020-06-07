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
 
/**************************************************************************
  Functions:
    ddr_kopengl_return_DCM_fcns 
    ddr_kopengl_dcm_create_local_data 
    ddr_kopengl_dcm_initialize_device
    ddr_kopengl_dcm_close_device 
    ddr_kopengl_dcm_flush 
    ddr_kopengl_dcm_swap_buffers 
    ddr_kopengl_dcm_update_geometry 
    ddr_kopengl_dcm_become_current_driver 
    ddr_kopengl_dcm_set_options 
    parse_command_args 
    get_display_connection 
    get_visual 
    create_window 
 **************************************************************************/



/* Include files */
#include <stdio.h>
#include <string.h>
#include "dcm.h"
#include <dore/dore_develop/error.h>

/* Macros */
#define streq(a,b)   (0 == strcmp((a),(b)))
#define strsize(s)   (1+strlen(s))

/* Global Pointers */
ddt_kopengl_data  *dde_kopengl_current_device_data;
DDt_DCM           *dde_kopengl_DCM_fcns = (DDt_DCM *)0;


/* Start of functions */

DtPtr ddr_kopengl_return_DCM_fcns (void)
 {
  DDt_DCM *fcns;

  if (dde_kopengl_DCM_fcns == (DDt_DCM *)0) 
    {
     fcns = DDdevice_CreateDCMStruct();

     if (fcns == (DDt_DCM *)0) 
       {
	DDerror (ERR_CANT_ALLOC_MEM, "dde_kopengl_return_DCM_fcns",
		 DcNullPtr);
       }

# ifndef WIN32

#ifdef __STDC__
# define SetFuncPtr(NAME) fcns->NAME = ddr_kopengl_dcm_##NAME
#else
# define SetFuncPtr(NAME) fcns->NAME = ddr_kopengl_dcm_/**/NAME
#endif

     fcns->version_number = 2;

     /* Version 1 Functions */
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

# else 

	 fcns->create_local_data = ddr_kopengl_dcm_create_local_data;
     fcns->initialize_device = ddr_kopengl_dcm_initialize_device;
     fcns->become_current_driver = ddr_kopengl_dcm_become_current_driver;
     fcns->close_device = ddr_kopengl_dcm_close_device;
     fcns->flush = ddr_kopengl_dcm_flush;
     fcns->swap_buffers = ddr_kopengl_dcm_swap_buffers;
     fcns->update_geometry = ddr_kopengl_dcm_update_geometry;
     fcns->inquire_device_extent = ddr_kopengl_dcm_inquire_device_extent;
     fcns->inquire_stereo = ddr_kopengl_dcm_inquire_stereo;
     fcns->inquire_ncolors = ddr_kopengl_dcm_inquire_ncolors;
     fcns->inquire_resolution = ddr_kopengl_dcm_inquire_resolution;
     fcns->inquire_visual_type = ddr_kopengl_dcm_inquire_visual_type;
     fcns->inquire_auto_size = ddr_kopengl_dcm_inquire_auto_size;
     fcns->inquire_color_entries = ddr_kopengl_dcm_inquire_color_entries;
     fcns->set_color_entries = ddr_kopengl_dcm_set_color_entries;
     fcns->set_foreground_color = ddr_kopengl_dcm_set_foreground_color;
     fcns->set_background_color = ddr_kopengl_dcm_set_background_color;
     fcns->set_shade_index = ddr_kopengl_dcm_set_shade_index;
     fcns->inquire_clip_list = ddr_kopengl_dcm_inquire_clip_list;
     fcns->clear_rectangle_depth = ddr_kopengl_dcm_clear_rectangle_depth;
     fcns->clear_rectangle_depth_and_color = ddr_kopengl_dcm_clear_rectangle_depth_and_color;
     fcns->set_depth_buffer_enable = ddr_kopengl_dcm_set_depth_buffer_enable;
     fcns->set_depth_buffer_write = ddr_kopengl_dcm_set_depth_buffer_write;
     fcns->write_scanline_byte = ddr_kopengl_dcm_write_scanline_byte;
     fcns->set_current_view = ddr_kopengl_dcm_set_current_view;
     fcns->inquire_pixel_data = ddr_kopengl_dcm_inquire_pixel_data;

     /* Version 2 Functions */
     fcns->set_options = ddr_kopengl_dcm_set_options;

# endif

     dde_kopengl_DCM_fcns = fcns;
    }

  return (DtPtr) dde_kopengl_DCM_fcns;
 }



/****************************************************************************
 *                               
 ****************************************************************************/

DtPtr ddr_kopengl_dcm_create_local_data (DtObject device)
 {
  ddt_kopengl_data *device_data;

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ddr_kopengl_dcm_create_local_data\n");
#endif 

  if (!(device_data = (ddt_kopengl_data *)
        DDspace_Allocate(sizeof (ddt_kopengl_data)))) 
    {
     DDerror (ERR_CANT_ALLOC_MEM_SIZE, 
		 "ddr_kopengl_dcm_create_local_data",
		 "sizeof (ddt_kopengl_data)");
     return (DcNullPtr);
    }

  device_data->device = device;

  return ((DtPtr) device_data);
 }




/****************************************************************************
 * This function parses the command line options and sets the fields in the
 * device data struct accordingly. Command line options that are not valid
 * are reported and the processing of commands continues until no more 
 * arguments are left. The functions returns DcFalse if a command line option
 * has an error; else returns DcTrue.  
 ****************************************************************************/

#ifndef WIN32

DtFlag parse_command_args (char *argstring)
 {
  static char       funcname[] = "parse_command_args";

  ddt_kopengl_data *device_data;
  DtFlag            retval;          /* return value */
  char             *argstringcopy;   /* Copy of Argument String */
  char             *tmp;             /* Tokenizing Function Parameter */
  char             *p;               /* String Pointer */
  char             *nextarg;         /* Next Command Argument */

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ..parsing driver options\n");
#endif 

  /* Make a copy of the argument string for parsing */
  argstringcopy = (char *)DDspace_Allocate(strlen(argstring)+1);
  strcpy(argstringcopy, argstring); /* so as not to alter user's string */
  tmp = argstringcopy;
  retval = DcTrue;

  /* Get a pointer to the current device data */
  device_data = DCM;

  /* p points to first sub-string */

  while ((p = DDstrtok(tmp, " \t,"))) 
    {
     tmp = 0;

     if (streq(p,"-noautosize")) 
       device_data->auto_size = DcFalse;
     else 
       {
        if (streq(p, "-visualtype")) 
          {
	   if (!(nextarg = DDstrtok(0, " \t,"))) 
             {
	      DDerror (ERR_BAD_DEVICE_OPT_VAL,
	         funcname, "visualtype: (no value)");
              retval = DcFalse;
	     } 
           else 
             {
	      if ( streq ("DcPseudoColor", nextarg)) 
	 	device_data->visualtype = DcPseudoColor;
              else 
                {
                 if ( streq ("DcTrueColor", nextarg)) 
		   device_data->visualtype = DcTrueColor;
                 else 
                   {
		    sprintf (DDerror_GetLine(), "visualtype: '%s'", nextarg);
		       DDerror (ERR_BAD_DEVICE_OPT_VAL,
		       funcname, DDerror_GetLine());
		    retval = DcFalse;
		   }
                }
	     } 
          }
        else 
          {
           if (streq(p,"-window")) 
             { 
              if (!(nextarg = DDstrtok(0," \t,"))) 
                {
                 DDerror (ERR_BAD_DEVICE_OPT_VAL,
                              funcname, "window: (no value)");
                 retval = DcFalse;
                } 
              else 
                {
                 /* 
                  * If -window is used to supply a window ID
                  * set a flag (device_data->user_xwin_flag) to
                  * remind us that this is user supplied
                  */
                 sscanf(nextarg,"%d",&(device_data->xwindow));
                 device_data->user_xwin_flag = DcTrue;
                }
             } 
           else 
             {
              if (streq(p,"-display")) 
                {
                 if (!(nextarg = DDstrtok(0," \t,"))) 
                   {
                    DDerror (ERR_BAD_DEVICE_OPT_VAL,
                               funcname, "display: (no value)");
                    retval = DcFalse;
                   } 
                 else 
                   {
                    long dptr;

                    sscanf (nextarg,"%ld",&dptr);
                    device_data->xdisplay = (Display *) dptr;
                    device_data->user_xdsp_flag = DcTrue;
                   }
                }
              else 
                {
                 if (streq(p,"-geometry")) 
                   {
	            if (!(nextarg = DDstrtok(0," \t,"))) 
                      {
	   	       DDerror (ERR_BAD_DEVICE_OPT_VAL,
		         	 funcname, "geometry: (no value)");
		       retval = DcFalse;
	              } 
                    else 
                      {
                       int perr;

                       /*
                        * parse the geometry and set into the 
                        * device_data struct 
                        */
	 	       perr = XParseGeometry (nextarg, &device_data->x, 
                                                       &device_data->y, 
                                                       &device_data->width, 
                                                       &device_data->height);
                       /* Check for Negative width or height */
                       if ( (perr == XNegative) ||
                            (perr == YNegative) )
                         {
                          DDerror (ERR_BAD_DEVICE_OPT_VAL,
                              funcname, "geometry: (negative width or height)");
                          retval = DcFalse;
                         }
                      }
                   }
                 else 
                   {
                    if (streq(p,"-singlebuffered")) 
	                 device_data->double_buffered = DcFalse;
                    else 
                      {
	               sprintf(DDerror_GetLine(), "unknown flag '%s'", p);
	               DDerror (ERR_BAD_DEVICE_OPT, 
                                      funcname, DDerror_GetLine());
	              }
                   }
                }
             }
          }
       }
    } /* end of while */

  /* De-Allocate memory for args and check for errors */
  DDspace_Deallocate(argstringcopy);
 
  return retval;  
 }

#else /* if WIN32 */

#endif


/****************************************************************************
 * This is a support function for ddr_kopengl_dcm_initialize_device (). It
 * gets a display connection if one was not provided by the user 
 ****************************************************************************/

#ifndef WIN32

DtFlag get_display_connection (void)
  {
   static char              funcname[] = "get_display_connection";
   auto   ddt_kopengl_data *device_data;

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ..establishing a display connection\n");
#endif

    /* Get a pointer to the current device data */
    device_data = DCM;

    /*
     * If the user specifies the -window option we take it that he/she
     * would like to draw to a window opened by another application.
     * In this case we should ensure that he/she also provides us with
     * a valid display using the -display option. 
     */

    if (device_data->user_xwin_flag && !(device_data->user_xdsp_flag))
      {
       DDerror (ERR_BAD_DEVICE_OPT,
                     funcname, "must set -xdisplay with -xwindow\n");
       return DcFalse;
      }

    /*
     * Establish a display connection if not supplied by the user
     */

    /* Check if user supplied a display connection ID */
    if (!device_data->user_xdsp_flag)
      {
       char    *display_name;

       /* Ensure that the DISPLAY environment was set */
       if ((display_name = DDgetenv("DISPLAY")) == NULL)
         {
          DDerror(ERR_DEVICE_SYSTEM_ACCESS,funcname,
                        "Display Environment variable not set");
          return DcFalse;
         }

       /* Establish a display connection */
       if ((device_data->xdisplay = XOpenDisplay(display_name)) == NULL)
         {
          sprintf (DDerror_GetLine(),
                        "Could not open display %s", display_name);
          DDerror(ERR_DEVICE_SYSTEM_ACCESS, funcname, DDerror_GetLine());
          return DcFalse;
         }

      }

   return DcTrue;
 }

#endif

/****************************************************************************
 * This is a support function for ddr_kopengl_dcm_initialize_device (). It
 * gets a visual 
 ****************************************************************************/

#ifndef WIN32

XVisualInfo *get_visual (void)
 {
  auto   int               l;
  auto   int               num_visuals;
  static char              funcname[] = "get_visual";
  auto   ddt_kopengl_data *device_data;
  auto   XVisualInfo       vinfo_template;
  auto   Visual           *vis;
  auto   XVisualInfo      *visual_info;
  auto   GLenum            list[20];

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ..selecting an appropriate visual\n");
#endif
 
  num_visuals = -1;

  /* Get a pointer to the current device data */
  device_data = DCM;


  /*
   * Selecting a visual: If we are give a window by the user then
   * this implies a particular visual and colormap type to use. If
   * we are asked to create the window and if -visualtype is explicitly
   * set then we try to provide the requested visual. If we are asked
   * to create a window without -visualtype being set then we grab the
   * default visual.
   */

  /* Handle a user supplied window. Ignore -visualtype option */
  if (device_data->user_xwin_flag)
    {
     XWindowAttributes  window_attributes;

     /*
      * Get the attributes of the window supplied by the user to find
      * out what kind of visual is specified in his/her window. This
      * will allow us to set up an appropriate color map.
      */
     XSync (device_data->xdisplay, False);
     XGetWindowAttributes (device_data->xdisplay, device_data->xwindow,
                                  &window_attributes);

     /* 
      * Set the dimensions of the user specified
      * window in the device data structure 
      */   
     device_data->x = window_attributes.x;
     device_data->y = window_attributes.y;
     device_data->width = window_attributes.width;
     device_data->height = window_attributes.height;

     vinfo_template.visualid = (window_attributes.visual)->visualid;
     vinfo_template.class = (window_attributes.visual)->class;

     visual_info = XGetVisualInfo (device_data->xdisplay,
                           VisualIDMask|VisualClassMask, &vinfo_template,
                           &num_visuals);

     /* Just in case */
     if (num_visuals != 1)
       return ((XVisualInfo *)NULL);

    }
  else
    {
     /*
      * If a True color visual is explicitly requested then grab the
      * default visual. Check to see if the default visual is indeed
      * a True Color visual. If it is then get the visual info struct.
      */

     if (device_data->visualtype == DcTrueColor)
       {
        l = 0;

        list[l++] = GLX_LEVEL;
        list[l++] = 0;

     /* Note on DENALI: The OpenGL implementation on the Denali
      * required a particular version of the X Server to get 
      * double buffered visuals to work. Therefore.... 
      */

#ifndef DENALI
        if (device_data->double_buffered)
          list[l++] = GLX_DOUBLEBUFFER;
#endif
        list[l++] = GLX_RGBA;
        list[l++] = GLX_RED_SIZE;
        list[l++] = 1;
        list[l++] = GLX_GREEN_SIZE;
        list[l++] = 1;
        list[l++] = GLX_BLUE_SIZE;
        list[l++] = 1;
#ifndef DENALI
        list[l++] = GLX_ALPHA_SIZE;
        list[l++] = 1;
        list[l++] = GLX_DEPTH_SIZE;
        list[l++] = 1;
#endif
        list[l] = (int)None;

        visual_info = glXChooseVisual (device_data->xdisplay,
                           DefaultScreen(device_data->xdisplay), (int *)list);
       }
     else
       {
        /*
         * If a PseudoColor visual is requested explicitly then let
         * GLX chose a visual with the information you provide.
         */
        if (device_data->visualtype == DcPseudoColor)
          {
           l = 0;
           list[l++] = GLX_LEVEL;
           list[l++] = 0;
           if (device_data->double_buffered)
             list[l++] = GLX_DOUBLEBUFFER;
           list[l++] = GLX_BUFFER_SIZE;
           list[l++] = 1;
           list[l++] = GLX_ALPHA_SIZE;
           list[l++] = 1;
           list[l++] = GLX_DEPTH_SIZE;
           list[l++] = 1;
           list[l] = (int)None;

           visual_info = glXChooseVisual (device_data->xdisplay,
                           DefaultScreen(device_data->xdisplay), (int *)list);

          }
        /*
         * This means that -visualtype was not set. We just grab the
         * default visual.
         */
        else
          {
           vis = DefaultVisual (device_data->xdisplay,
                               DefaultScreen(device_data->xdisplay));

           vinfo_template.visualid = vis->visualid;
           vinfo_template.class = vis->class;

           visual_info = XGetVisualInfo (device_data->xdisplay,
                             VisualIDMask|VisualClassMask, &vinfo_template,
                                &num_visuals);

           /* Just in case */
           if (num_visuals != 1)
             return ((XVisualInfo *)NULL);

           /* 
            * If this is a pseudo color or true color visual set the
            * visualtype field else return NULL visual info 
            */
           switch (visual_info->class)
             {
              case PseudoColor:
                device_data->visualtype = DcPseudoColor;
                break;
              case TrueColor:
                device_data->visualtype = DcTrueColor;
                break;
              default:
                return ((XVisualInfo *)NULL);
             }

          } /* else -visualtype not set */
       } /* else not True Color visual */
    } /* else not user supplied window */


  return visual_info;
 }

#else

#endif

/****************************************************************************
 * This is a support function for ddr_kopengl_dcm_initialize_device (). It
 * is called from that routine to create/map a new display window if the 
 * user did not provide one. 
 ****************************************************************************/

#ifndef WIN32

DtFlag create_window (XVisualInfo *visual_info)
  {
   static char                  funcname[] = "create_window";
   auto   ddt_kopengl_data     *device_data;
   auto   XSetWindowAttributes  win_attr;
   auto   unsigned long         attr_mask;
   auto   XEvent                event;
   auto   XSizeHints            xsh;
   auto   XClassHint            class_hints;
   static XWMHints              xwmh =
      {InputHint | StateHint,   /* Flags */
       True,                    /* Input */
       NormalState,             /* Initial State */
       0, 0, 0, 0, 0,           /* Icon Pixmap,Window,Location,Mask */
       0,                       /* Window Group */
      };


#ifdef _DEBUG_KOPENGL_
   fprintf (stdout, "[KOpenGL Driver] ..creating a window\n");
#endif

   /* Get a pointer to the current device data */
   device_data = DCM;

   /* Set the border and background pixels */
   win_attr.background_pixel = BlackPixel(device_data->xdisplay,
                                     DefaultScreen(device_data->xdisplay));
   win_attr.border_pixel = WhitePixel(device_data->xdisplay,
                                     DefaultScreen(device_data->xdisplay));
   attr_mask = CWBackPixel | CWBorderPixel;

   /*
    * If we aren't using the default visual type, we'll
    * need to create a colormap because we can't inherit
    * it from parent window.
    */
   if (visual_info->visual != DefaultVisual(device_data->xdisplay,
                                     DefaultScreen(device_data->xdisplay)))
     {
      device_data->cmap = XCreateColormap (device_data->xdisplay,
                                  XDefaultRootWindow(device_data->xdisplay),
                                  visual_info->visual,AllocNone);
     }
   else
     {
      device_data->cmap = DefaultColormap (device_data->xdisplay,
                                     DefaultScreen(device_data->xdisplay));
     }
   win_attr.colormap = device_data->cmap;
   attr_mask |= CWColormap;

   device_data->xwindow = XCreateWindow (device_data->xdisplay,
                     DefaultRootWindow(device_data->xdisplay),
                     device_data->x, device_data->y, device_data->width,
                     device_data->height, 0, visual_info->depth,
                     InputOutput, visual_info->visual, attr_mask, &win_attr);

   if (!device_data->xwindow)
     {
      DDerror(ERR_DEVICE_SYSTEM_ACCESS, funcname,
                            "Could not create a X11 window");
      return DcFalse;
     }

   /* Set the name and Icon properties */
   XStoreName(device_data->xdisplay,device_data->xwindow,
                  "Dore");
   XSetIconName(device_data->xdisplay,device_data->xwindow,
                  "Dore");
   XSetWMHints  (device_data->xdisplay, device_data->xwindow, &xwmh);

   /* Set the size hints to indicate the position and size */
   xsh.flags  = USPosition | USSize;
   xsh.x      = device_data->x;
   xsh.y      = device_data->y;
   xsh.width  = device_data->width;
   xsh.height = device_data->height;
  
   XSetSizeHints (device_data->xdisplay, device_data->xwindow, 
                                       &xsh, XA_WM_NORMAL_HINTS);

   /* Hints to indicate the type of resource we are running */
   class_hints.res_name = "dore";
   class_hints.res_class = "Dore";

   XSetClassHint (device_data->xdisplay, device_data->xwindow, &class_hints);


#ifdef _DEBUG_KOPENGL_
   fprintf (stdout, "[KOpenGL Driver] ..mapping the window\n");
#endif

   /* Wait for the window we created to get mapped */
   XSelectInput (device_data->xdisplay, device_data->xwindow,
                                                       ExposureMask);
   XMapWindow (device_data->xdisplay, device_data->xwindow);
   while (XNextEvent (device_data->xdisplay, &event))
     {
      if (event.type == Expose)
        break;
     }

   return DcTrue;
  }

#else /* if WIN32 */

#endif


/****************************************************************************
 * This function provides OpenGL with a window. There are two types to
 * be considered:
 *    1. window supplied by the user
 *    2. window created by us 
 ****************************************************************************/

#ifndef WIN32

DtFlag ddr_kopengl_dcm_initialize_device (DtObject  device, 
                                          DtPtr     device_data_param, 
                                          DtPtr     argstring,
                                          DtPtr     name)
  {
    static char           *funcname  = "ddr_kopengl_dcm_initialize_device";
    char                  *argstringcopy, *tmp, *p, *nextarg;
    XVisualInfo           *visual_info;
    Visual                *vis;
    ddt_kopengl_data      *device_data = device_data_param;

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_initialize_device\n");
#endif 

    /* Initialize device data struct */
    DCM = device_data;

    device_data->device          = device;
    device_data->xwindow         = (Window) 0;
    device_data->visualtype      = -1;
    device_data->x               = 0;
    device_data->y               = 0;
    device_data->width           = 512;
    device_data->height          = 512;
    device_data->bg_color[0]     = 0.0;
    device_data->bg_color[1]     = 0.0;
    device_data->bg_color[2]     = 0.0;

    device_data->auto_size       = DcTrue;
    device_data->double_buffered = DcTrue;
    device_data->user_xwin_flag  = DcFalse; 
    device_data->user_xdsp_flag  = DcFalse;

    /*
     * Process the argstring
     */
    if (!parse_command_args (argstring))
      return DcFalse;    

    /*
     * Get a display connection 
     */
    if (!get_display_connection ())
      return DcFalse;


     /* Check for GLX extension support */
     {
      int error_base;
      int event_base;

      if (!glXQueryExtension(device_data->xdisplay, 
                           &error_base, &event_base))
        {
         DDerror(ERR_DEVICE_SYSTEM_ACCESS, funcname, 
                     "GLX extension not supported by X server"); 
         return DcFalse;
        }
     }


    /* Do the visual selection */
    visual_info = get_visual ();
    if (!visual_info)
      {
       DDerror (ERR_DEVICE_SYSTEM_ACCESS, funcname, 
                     "requested visual type is not supported"); 
       return DcFalse;
      }

    /* 
     * Lets open up a window if the user asked us to 
     */
    if (!device_data->user_xwin_flag)
      {
       if (!create_window (visual_info))
         return DcFalse;
      }

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ..installing color map\n");
#endif 
    /*    
     * If this is a pseudo color visual and the colormap is not the
     * default colormap then we need to install one here.
     */
    if ( (visual_info->class == PseudoColor) && 
         (DefaultColormap (device_data->xdisplay, 
             DefaultScreen(device_data->xdisplay)) != device_data->cmap) )
      {
       device_data->cmap = XCreateColormap (device_data->xdisplay,
                                  XDefaultRootWindow(device_data->xdisplay),
                                  visual_info->visual, AllocNone);
      }


    /* Create a GLX context */
    device_data->context = glXCreateContext (device_data->xdisplay, 
                                 visual_info, None, True);
                                
    if (!device_data->context)
      {
       DDerror(ERR_DEVICE_SYSTEM_ACCESS, funcname,
                               "unable to create GLX context");
       return DcFalse;
      }


    /* Attach the GLX context to the window */
    if (!glXMakeCurrent (device_data->xdisplay, device_data->xwindow,  
                                                  device_data->context))
      {
       /* If not a user supplied window close the display*/
       if (!device_data->user_xdsp_flag)
         {
          XCloseDisplay(device_data->xdisplay);
          DDspace_Deallocate (device_data);
         }
       DDerror(ERR_DEVICE_SYSTEM_ACCESS, funcname,
                          "unable to attach GLX context to window");
       return DcFalse;
      }

    if (device_data->double_buffered)
      glDrawBuffer (GL_BACK);

    /* Initial setup of view matrix required to make the PROM renderer work */
    glOrtho (DCM->x, DCM->x + DCM->width, DCM->y, DCM->y + DCM->height, 0.0, 1.0);

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ..done initializing the device\n");
#endif 

    /* clean up */ 
    XFree (visual_info);

    return DcTrue;
  }

#else

DtFlag ddr_kopengl_dcm_initialize_device (DtObject  device, 
                                          DtPtr     device_data_param, 
                                          DtPtr     argstring,
                                          DtPtr     name)
  {
    static char           *funcname  = "ddr_kopengl_dcm_initialize_device";
    char                  *argstringcopy, *tmp, *p, *nextarg;
    ddt_kopengl_data      *device_data = device_data_param;
    GLenum                 type;

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_initialize_device\n");
#endif 

    /* Initialize device data struct */
    DCM = device_data;

    device_data->device          = device;
    device_data->x               = 0;
    device_data->y               = 0;
    device_data->width           = 300;
    device_data->height          = 300;
    device_data->bg_color[0]     = 0.0;
    device_data->bg_color[1]     = 0.0;
    device_data->bg_color[2]     = 0.0;

    device_data->auto_size       = DcTrue;
    device_data->double_buffered = DcTrue;


    auxInitPosition (device_data->x, device_data->y, device_data->width, 
                     device_data->height);

    type |= AUX_RGB;
    type |= device_data->double_buffered ? AUX_DOUBLE : AUX_SINGLE ;
   
    auxInitDisplayMode (type);

    if (auxInitWindow ("Dore Window") == GL_FALSE)
      {
       auxQuit();
      }
    
    if (device_data->double_buffered)
      glDrawBuffer (GL_BACK);

    /* Initial setup of view matrix required to make the PROM renderer work */
    glOrtho (DCM->x, DCM->x + DCM->width, DCM->y, DCM->y + DCM->height, 0.0, 1.0);

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ..done initializing the device\n");
#endif 

    return DcTrue;
  }
#endif

/****************************************************************************
 * Close the windows we created and clean up 
 ****************************************************************************/

#ifndef WIN32

void ddr_kopengl_dcm_close_device (void)
  {
   ddt_kopengl_data *device_data;

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_close_device\n");
#endif 

   /* Get a pointer to the current device data */
   device_data = DCM;

   XFlush (device_data->xdisplay);

   /* Close the window if we created it */
   if (!device_data->user_xwin_flag)
     {
      XUnmapWindow (device_data->xdisplay, device_data->xwindow);
      XDestroyWindow (device_data->xdisplay, device_data->xwindow);
     }

   /* Destroy the GLX context */
   glXDestroyContext (device_data->xdisplay, device_data->context);

   /* Close the display connection if we opened it */
   if (!device_data->user_xdsp_flag)
     {
      XCloseDisplay (device_data->xdisplay);
     }

   /* Free up the device data structure */
   DDspace_Deallocate (device_data);
   return;
  }

#else

void ddr_kopengl_dcm_close_device (void)
  {
   ddt_kopengl_data *device_data;

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_close_device\n");
#endif

   /* Get a pointer to the current device data */
   device_data = DCM;

   /* Finish rendering and close the window */
   glFinish ();
   auxCloseWindow ();

   /* Free up the device data structure */
   DDspace_Deallocate (device_data);
   return;
  }
 
#endif


/****************************************************************************
 * Flush the display                              
 ****************************************************************************/

void ddr_kopengl_dcm_flush (void)
  {
   ddt_kopengl_data *device_data;

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_flush\n");
#endif 

  }


/****************************************************************************
 * Swap the buffers                              
 ****************************************************************************/

void ddr_kopengl_dcm_swap_buffers (void)
  {
   ddt_kopengl_data *device_data;

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_swap_buffers\n");
#endif 

   /* Get a pointer to the current device data */
   device_data = DCM;

#ifdef WIN32
   auxSwapBuffers ();
#else
   glXSwapBuffers (device_data->xdisplay, device_data->xwindow);
#endif
  }


/****************************************************************************
 *                               
 ****************************************************************************/

void ddr_kopengl_dcm_update_geometry (void)
  {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_update_geometry\n");
#endif 

  }



/****************************************************************************
 *                               
 ****************************************************************************/


void ddr_kopengl_dcm_become_current_driver (DtPtr device_data)
  {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_become_current driver\n");
#endif 
    DCM = (ddt_kopengl_data *)device_data;
  }



/****************************************************************************
 *                               
 ****************************************************************************/

void ddr_kopengl_dcm_set_options (DtPtr argstring)
  {
    char		*argstringcopy;
    char		*tmp;
    char		*p;
    ddt_kopengl_data 	*device_data;

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] Setting options\n");
#endif 

    device_data = DCM;

    argstringcopy = (char *)DDspace_Allocate(strlen(argstring)+1);
    strcpy(argstringcopy, argstring); /* so as not to alter user's string */
    tmp = argstringcopy;

    /* p points to first sub-string */

    while ((p = DDstrtok(tmp, " \t,"))) {
	tmp = 0;

	if (streq(p, "-doublebuffered")) {
	    
	    device_data->double_buffered = DcTrue;

	} else if (streq(p, "-singlebuffered")) {
	    device_data->double_buffered = DcFalse;
	}
    }

    DDspace_Deallocate(argstringcopy);
  }


/****************************************************************************
 *                               
 ****************************************************************************/

#define isnum(arg) ((arg) >= '0' && (arg) <= '9')
#define GetInt(arg,val) for ((val)=0; isnum(*(arg)); (arg)++) \
	    (val) = ((val) * 10) + (*(arg) - '0')
