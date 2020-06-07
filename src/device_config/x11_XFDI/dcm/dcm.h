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
 
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xim.h>
#include <X11/Xcm.h>
#include <X11/Xdb.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <xfdi.h>
#include <fastmath.h>

#include "dore/internal/dogen.h"

typedef struct {
   DtPtr device;
   Display *display;
   Window window;
   GC xgc;
   XFDIGC gc;

   Drawable Vdraw_buffer;
   Drawable draw_buffer;
   Drawable Vdraw_buffer_stereo;
   Drawable draw_buffer_stereo;
   Drawable texture_buffer;
   Drawable filter_buffer;
   Drawable z_buffer;
   Drawable z_buffer_stereo;

   DtFlag multi_pass_transp;
   DtFlag use_spheres;
   Drawable sort_z_buffer;
   Drawable sort_i_buffer;
   Drawable sort_a_buffer;

   int user_display;
   int user_window;

   DtVisualType visualtype;
   int ncolors;
   int double_buffered;
   DtFlag auto_size;
   int stereoswitch;
   DtFlag stereoscreen;
   DtFlag bad_geom;
   XRectangle window_extent;
   XRectangle viewport;
   XWindowAttributes window_attributes;
   XImage *image;

   DtObject current_view;
   DtObject previous_view;

   DtReal background_color[3];

   DtInt shade_index;
   DtReal cbtselpsweight[3];
   DtInt cbtselpsmask[3];
   DtReal cwtavgpsweight[3];
   DtReal cwtavgpslim[3];
   DtShadeMode shade_mode;
   short bg_color_index;

} ddt_x11_XFDI_data;


extern ddt_x11_XFDI_data	*dde_x11_XFDI_current_dcm_data;
extern struct list		*dde_x11_XFDI_dcm_list;
