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

/************************************************************************* 
  This header file contains the include directives, definitions and
  declarations for the Kubota OpenGL device driver for Dore'.
**************************************************************************/

#ifndef _KOPENGL_DCM_H_
#define _KOPENGL_DCM_H_


/* Include files */
#include <dore/dore.h>
#include <dore/dore_develop/error.h>
#include <dore/dore_develop/develop.h>

#ifdef WIN32
# include <stdio.h>
# include <stdlib.h>
# include <windows.h>
# include <string.h>
# include <gl/gl.h>
# include <gl/glu.h>
# include <gl/glaux.h>
#else
# include <X11/Xlib.h>
# include <X11/Xresource.h>
# include <X11/Xatom.h>
# include <X11/Xutil.h>
# include <GL/gl.h>
# include <GL/glx.h>
#endif


    /****************************************/
    /***      Structure Definition        ***/
    /****************************************/

typedef struct {
    DtObject	  device;
#ifndef WIN32
    Window        xwindow;
    Display      *xdisplay;
    DtVisualType  visualtype;
    Colormap      cmap;
    GLXContext    context;
#else  /* if WIN32 */
    DtVisualType  visualtype;
#endif
    DtInt	  x;
    DtInt	  y;
    DtInt	  width;
    DtInt	  height;
    DtColorRGB    bg_color; 
 /* Flags */
#ifndef WIN32
    DtFlag        user_xdsp_flag;
    DtFlag        user_xwin_flag;
#endif
    DtFlag	  auto_size;
    DtFlag	  double_buffered;

} ddt_kopengl_data;


    /****************************************/
    /***        Global Variables          ***/
    /****************************************/

extern   ddt_kopengl_data *dde_kopengl_current_device_data;
#define  DCM               dde_kopengl_current_device_data


    /****************************************/
    /***      Function  Declarations      ***/
    /****************************************/

/*
 * declarations for dcm.c 
 */

extern DtPtr ddr_kopengl_return_DCM_fcns 
 (
  void
 );

extern DtPtr ddr_kopengl_dcm_create_local_data 
 (
  DtObject
 );

#ifndef WIN32
static DtFlag parse_command_args 
 (
  char*
 );

static DtFlag get_display_connection
 (
  void
 );

static XVisualInfo *get_visual 
 (
  void
 );

static DtFlag create_window 
 (
  XVisualInfo*
 );
#endif

extern DtFlag ddr_kopengl_dcm_initialize_device 
 (
  DtObject,
  DtPtr,
  DtPtr,
  DtPtr
 );

extern void ddr_kopengl_dcm_close_device
 (
  void
 );

extern void ddr_kopengl_dcm_flush
 (
  void
 );

extern void ddr_kopengl_dcm_swap_buffers
 (
  void
 );

extern void ddr_kopengl_dcm_update_geometry
 (
  void
 );

extern void ddr_kopengl_dcm_become_current_driver 
 (
  DtPtr 
 );

extern void ddr_kopengl_dcm_set_options
 (
  DtPtr
 );



/*
 * declarations for output.c 
 */



extern void ddr_kopengl_dcm_clear_rectangle_depth 
 ( 
  DtShort,    
  DtShort, 
  DtShort, 
  DtShort
 );

extern void ddr_kopengl_dcm_clear_rectangle_depth_and_color 
 (
  DtShort,    
  DtShort, 
  DtShort, 
  DtShort
 );

extern void ddr_kopengl_dcm_write_scanline_byte 
 (
  DtInt,
  DtInt,
  DtInt,
  DtUChar*
 );


/*
 * declarations for inq.c 
 */

extern void ddr_kopengl_dcm_inquire_color_entries 
 (
  DtColorModel,
  int,
  int,
  DtReal*
 );

extern void ddr_kopengl_dcm_inquire_auto_size 
 (
  DtFlag*
 );

extern void ddr_kopengl_dcm_inquire_clip_list
 (
  DtUInt*,
  DtReal**,
  DtReal[4],
  DtFlag*
 );

extern void ddr_kopengl_dcm_inquire_device_extent
 (
  DtVolume*
 );

extern void ddr_kopengl_dcm_inquire_ncolors
 (
  DtInt*
 );

extern DtFlag ddr_kopengl_dcm_inquire_pixel_data
 (
  DtRasterType,
  DtInt*,
  DtInt*,
  DtRasterType*,
  DtPtr*,
  DtFlag* 
 );

extern void ddr_kopengl_dcm_inquire_stereo
 (
  DtFlag*
 );

extern void ddr_kopengl_dcm_inquire_ncolors
 (
  DtInt*
 );

extern void ddr_kopengl_dcm_inquire_resolution
 (
  DtReal*,
  DtReal* 
 );

extern void ddr_kopengl_dcm_inquire_visual_type
 (
  DtVisualType*
 );

/*
 * declarations for set.c 
 */

extern void ddr_kopengl_dcm_set_color_entries
 (
  DtColorModel,
  int,
  int,
  DtReal* 
 );

extern void ddr_kopengl_dcm_set_foreground_color
 (
  DtColorModel,
  DtReal*
 );

extern void ddr_kopengl_dcm_set_background_color
 (
  DtColorModel,
  DtReal*
 );

extern void ddr_kopengl_dcm_set_current_view
 (
  DtObject
 );

extern void ddr_kopengl_dcm_set_depth_buffer_enable
 (
  DtFlag
 );

extern void ddr_kopengl_dcm_set_depth_buffer_write
 (
  DtFlag
 );

extern void ddr_kopengl_dcm_set_shade_index
 (
  DtInt
 );

#endif /* _KOPENGL_DCM_H_ */
