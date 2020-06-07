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
 

    /**********************/
    /***  Header Files  ***/
    /**********************/
 
#include <math.h>
#include "dore/internal/dogen.h"
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <PEXlib.h>


    /***************************/
    /***  Macro Definitions  ***/
    /***************************/

#define print(s)    fputs(s,stdout)

#define CLAMP(t,lo,hi)	(((t) < (lo)) ? (lo) : (((t) > (hi)) ? (hi) : (t)))

#define LUMINANCE(red,grn,blu) \
    ((0.299 * (red)) + (0.587 * (grn)) + (0.114 * (blu)))


    /*******************************/
    /***  Structure Definitions  ***/
    /*******************************/

typedef struct
{   DtObject          device;		/* Dore' Device Handle */
    Window            window;		/* X Window */
    int               screen;		/* User Screen */
    Display          *display;		/* X Display */
    GC                gc;		/* X Graphics Context */
    XImage           *image;		/* X Scanline Buffer */
    Pixmap            pixmap;		/* X Pixmap for Double Buffering */
    XWindowAttributes window_attrs;	/* X Window Information */

    DtVisualType      visualtype;	/* Visual Color Type */
    int               ncolors;		/* Number of Color Entries */
    Colormap          colormap;		/* Color Map for Pseudo Color */
    DtInt             shadeindex;	/* Shade-Range Index */

    DtFlag            user_display;	/* Existing Display to Use*/
    DtFlag            user_window;	/* Existing Window to Use*/
    DtFlag            auto_size;	/* true= Window Preserves Perspective */
    int               winX, winY;	/* Window Location */
    unsigned int      width, height;	/* Size of the Display Window */
} ddt_pex_data;


    /*******************************/
    /***  Function Declarations  ***/
    /*******************************/

void   ddr_pex_dcm_become_current_driver  ARGS((ddt_pex_data*));
void   ddr_pex_dcm_clear_rectangle_depth  ARGS((DtShort, DtShort, DtShort,
					        DtShort));
void   ddr_pex_dcm_clear_rectangle_depth_and_color ARGS((DtShort, DtShort,
					                 DtShort, DtShort));
void   ddr_pex_dcm_close_device           ARGS((void));
DtPtr  ddr_pex_dcm_create_local_data      ARGS((DtPtr));
void   ddr_pex_dcm_flush                  ARGS((void));
DtFlag ddr_pex_dcm_initialize_device      ARGS((DtPtr, ddt_pex_data*, DtPtr));
void   ddr_pex_dcm_inquire_auto_size	  ARGS((DtFlag*));
void   ddr_pex_dcm_inquire_clip_list	  ARGS((DtUInt*, DtReal**, DtReal[4],
					        DtFlag*));
void   ddr_pex_dcm_inquire_color_entries  ARGS((DtColorModel, DtInt, DtInt,
					        DtReal[]));
void   ddr_pex_dcm_inquire_device_extent  ARGS((DtVolume*));
void   ddr_pex_dcm_inquire_ncolors	  ARGS((DtInt*));
DtFlag ddr_pex_dcm_inquire_pixel_data	  ARGS((DtRasterType, DtInt*, DtInt*,
					        DtRasterType*,DtPtr*,DtFlag*));
void   ddr_pex_dcm_inquire_resolution	  ARGS((DtReal*, DtReal*));
DtFlag ddr_pex_dcm_inquire_spheres	  ARGS((void));
void   ddr_pex_dcm_inquire_stereo	  ARGS((DtFlag*));
void   ddr_pex_dcm_inquire_visual_type	  ARGS((DtVisualType*));
void   ddr_pex_dcm_inq_pseudo_range	  ARGS((float[3]));
void   ddr_pex_dcm_inq_pseudo_space	  ARGS((float[3], int[3]));
DtInt  ddr_pex_dcm_inq_texture_expand	  ARGS((void));
void   ddr_pex_dcm_set_background_color	  ARGS((DtColorModel, DtReal*));
void   ddr_pex_dcm_set_viewport           ARGS((DtReal,DtReal,DtReal,DtReal));
void   ddr_pex_dcm_set_color_entries	  ARGS((DtColorModel, DtInt, DtInt,
					        DtReal[]));
void   ddr_pex_dcm_set_current_view	  ARGS((DtObject));
void   ddr_pex_dcm_set_depth_buffer_enable ARGS((DtFlag));
void   ddr_pex_dcm_set_depth_buffer_write ARGS((DtFlag));
void   ddr_pex_dcm_set_foreground_color	  ARGS((DtColorModel, DtReal*));
void   ddr_pex_dcm_set_options		  ARGS((DtPtr));
void   ddr_pex_dcm_set_shade_index	  ARGS((DtInt));
void   ddr_pex_dcm_set_stereo_eye	  ARGS((int));
void   ddr_pex_dcm_swap_buffers		  ARGS((void));
void   ddr_pex_dcm_update_geometry	  ARGS((void));
void   ddr_pex_dcm_write_scanline_byte	  ARGS((DtInt, DtInt, DtInt, DtUChar*));
void   ddr_pex_Dither			  ARGS((DtInt,DtInt,DtReal*));
void   ddr_pex_DitherColor		  ARGS((DtInt,DtInt,DtReal*));
void   ddr_pex_DrawLine			  ARGS((DtInt*, DtInt*, DtColorRGB,
					        DtColorRGB));
void   ddr_pex_DrawPoint		  ARGS((DtInt*, DtColorRGB));
void   ddr_pex_DrawTriangle		  ARGS((DtInt*, DtInt*, DtInt*,
					        DtColorRGB, DtColorRGB,
						DtColorRGB));
void   ddr_pex_PutPixel			  ARGS((DtInt,DtInt,DtColorRGB));
DtPtr  ddr_pex_return_DCM_fcns		  ARGS((void));

void   ddr_pex_drom_update_shade_index    ARGS((void));


    /**************************/
    /***  Global Variables  ***/
    /**************************/

#ifdef DEFINE_GLOBALS
#   define GLOBAL /* Empty */
#else
#   define GLOBAL extern
#endif

GLOBAL ddt_pex_data *dde_pex_current_dcm_data;	/* DCM Structure */
#define DCM          dde_pex_current_dcm_data	/* Short-Name Alias */

GLOBAL DtInt         dde_pex_dcm_open_count;	/* Number of Open DCM's */
