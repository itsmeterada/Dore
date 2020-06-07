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
 
#include <dore/internal/dogen.h>
#include <dore/dore_develop/develop.h>
#include <dore/dore_develop/private/zbuffer.h>

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

/* ====================================================================== */

/* The maximum number of pseudo color entries that we will choose */

#define MAX_PSEUDO 1024

/* The maximum number of red/green/blue values for the max pseudo ramp */

#define MAX_RED    10
#define MAX_GREEN  10
#define MAX_BLUE   10

#define DITHER_SIZE 4

/* ====================================================================== */

typedef struct {
    DtObject		device;
    Display *		display;
    Window		window;
    GC			gc;
    XWindowAttributes	window_attributes;
    DtFlag		update_needed;
    int			doubleBuffered;
    Drawable		draw_buf;
    DtInt		shade_index;
    DtReal		cwtavgpsweight[3];
    DtReal		cwtavgpslim[3];
    DtShadeMode		shade_mode;
    XImage *		image;
    int			width;
    int			height;
    unsigned long	bg_color_index;
    unsigned long	fg_color_index;
    int			user_window;
    int			user_display;
    DtVisualType	visualtype;
    DtFlag		auto_size;
    int			simulator;
    int			dbf_enable;
    DtUShort *		depth;
    DtObject		current_view;
    DtObject		previous_view;
    unsigned long	redmap[256], greenmap[256], bluemap[256];
    unsigned long *	pseudomap;
    int			nred, ngreen, nblue;
    float		rscale, gscale, bscale;
    int			bytes_per_pixel;
    int			dither;
    Drawable		pixmap;
} ddt_stdx11_data;

extern ddt_stdx11_data *dde_stdx11_current_device_data;


    /*****************************/
    /***  Function Prototypes  ***/
    /*****************************/

void   ddr_stdx11_dcm_become_current_driver  (DtPtr);
void   ddr_stdx11_dcm_clear_rectangle_depth  (DtShort,DtShort,DtShort,DtShort);
void   ddr_stdx11_dcm_clear_rectangle_depth_and_color (
		DtShort, DtShort, DtShort, DtShort);
void   ddr_stdx11_dcm_close_device  (void);
int    ddr_stdx11_dcm_color_to_pixel  (DtReal [3], ddt_stdx11_data *);
DtPtr  ddr_stdx11_dcm_create_local_data  (DtObject);
void   ddr_stdx11_dcm_flush  (void);
DtFlag ddr_stdx11_dcm_initialize_device  (DtObject, DtPtr, DtPtr, DtPtr);
void   ddr_stdx11_dcm_inquire_auto_size  (DtFlag *);
void   ddr_stdx11_dcm_inquire_clip_list (DtUInt*, DtReal**, DtReal[4], DtFlag*);
void   ddr_stdx11_dcm_inquire_color_entries  (DtColorModel, int, int, DtReal*);
void   ddr_stdx11_dcm_inquire_device_extent  (DtVolume *);
void   ddr_stdx11_dcm_inquire_ncolors  (DtInt *);
DtFlag ddr_stdx11_dcm_inquire_pixel_data (
		DtRasterType, DtInt*, DtInt*, DtRasterType*, DtPtr*, DtFlag*);
void   ddr_stdx11_dcm_inquire_resolution  (DtReal *, DtReal *);
void   ddr_stdx11_dcm_inquire_stereo  (DtFlag *);
void   ddr_stdx11_dcm_inquire_visual_type  (DtVisualType *);
void   ddr_stdx11_dcm_setup_shading  (void);
void   ddr_stdx11_dcm_set_background_color  (DtColorModel, DtReal *);
void   ddr_stdx11_dcm_set_color_entries  (DtColorModel, int, int, DtReal *);
void   ddr_stdx11_dcm_set_current_view  (DtObject);
void   ddr_stdx11_dcm_set_depth_buffer_enable  (DtFlag);
void   ddr_stdx11_dcm_set_depth_buffer_write  (DtFlag);
void   ddr_stdx11_dcm_set_foreground_color  (DtColorModel, DtReal *);
void   ddr_stdx11_dcm_set_options  (DtPtr);
void   ddr_stdx11_dcm_set_shade_index  (DtInt);
void   ddr_stdx11_dcm_swap_buffers  (void);
void   ddr_stdx11_dcm_update_geometry  (void);
void   ddr_stdx11_dcm_write_scanline_byte  (DtInt, DtInt, DtInt, DtUChar *);
void   ddr_stdx11_Dither  (DtInt, DtInt, DtReal *);
void   ddr_stdx11_DitherColor  (DtInt, DtInt, DtReal *);
void   ddr_stdx11_DrawLine  (DtInt *, DtInt *, DtColorRGB, DtColorRGB);
void   ddr_stdx11_DrawPoint  (DtInt *, DtColorRGB);
void   ddr_stdx11_DrawTriangle (
		DtInt*, DtInt*, DtInt*, DtColorRGB, DtColorRGB, DtColorRGB);
void   ddr_stdx11_FreeBuffer  (void);
void   ddr_stdx11_InitSimulator  (void);
void   ddr_stdx11_PutPixel  (DtInt, DtInt, DtColorRGB);
DtPtr  ddr_stdx11_return_DCM_fcns  (void);
void   ddr_stdx11_UpdateSimulator  (void);
