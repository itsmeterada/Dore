#ifndef DORE_G4VLL_DCM_H
#define DORE_G4VLL_DCM_H

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
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/XPeer.h>
#include <vll.h>

#include <dore/internal/dogen.h>

#ifdef titan
#   include <sys/sysmips.h>
#endif
 
typedef struct g4vll_offscreen_resource {
    DtInt		width;
    DtInt		height;
    VLLpixmap		*pixmap;
} ddt_g4vll_dcm_offscreen_resource;
   
typedef struct {
    DtObject			device;

    int				x;
    int				y;
    int				width;
    int				height;

    Window			window;
    Display *			display;
    GC				gc;
    DtFlag			user_window;
    DtFlag			user_display;
    DtFlag			use_X;
    int				nrects;
    unsigned int		sequence_number;
    int				window_id;

    int				double_buffered;
    DtFlag			use_spheres;
    int				cylinders;
    DtFlag			use_ztc;
    DtFlag			use_hw_boundingvol;
    int				transp_mode;
    DtFlag			auto_size;
    DtVisualType		visualtype;
    int				ncolors;
    DtSwitch			stereoswitch;
    DtFlag			bad_geom;

    DtObject			current_view;
    DtObject			previous_view;

    float			background_color[3];
    DtInt			shade_index;
    float			cbtselpsweight[3];
    DtInt			cbtselpsmask[3];
    float			cwtavgpsweight[3];
    float			cwtavgpslim[3];
    DtShadeMode			shade_mode;
    unsigned long		bg_color_index;
    DtFlag			clr_device;
    DtFlag              	stereoscreen;
    int                 	screenwidth;
    int                 	screenheight;

    VLLwindow			*vll_window;
    unsigned int 		vll_window_status;
    VLLcolormap			*vll_cmap;
    VLLtranslation_table 	*transl_table;

    ddt_g4vll_dcm_offscreen_resource vll_pixmap;
    ddt_g4vll_dcm_offscreen_resource vll_accumulator;

    int				head;
    VLLstate			*vll_state;

    DtFlag			copy_back_buffer;
} ddt_g4vll_data;

/*
 ======================================================================
 */

#define ddd_g4vll_expand_default		0
#define ddd_g4vll_expand_replicate		1
#define ddd_g4vll_expand_bilinear		2

#define ddd_g4vll_mono_screen			-1

#define ddd_g4vll_transp_two_pass_add		0
#define ddd_g4vll_transp_single_pass		1
#define ddd_g4vll_transp_multi_pass		2
#define ddd_g4vll_transp_two_pass_compos	3

#define ddd_g4vll_cylinders_tesselated		0
#define ddd_g4vll_cylinders_firmware_capped	1
#define ddd_g4vll_cylinders_firmware_uncapped	2

/*
 * Head constants for running multi-headed Denali systems.
 */
#define ddd_g4vll_head_zero			0
#define ddd_g4vll_head_one			1


/* The following features require VLL offscreen resources:
 * 
 *	FEATURE				RESOURCES
 *	-------				---------
 * 	multi-pass transparency 	single precision pixmap with Z and
 *	  				   double precision accumulator with Z
 * 	two-pass (add and composite) 	single precision pixmap with Z 
 *	   transparency
 * 	single-pass transparency	single precision pixmap with Z
 * 	global antialiasing		double precision accumulator only
 *	MIP mapping			double precision accumulator only
 *	environment mapping		double precision accumulator only
 *
 */

#define ddd_g4vll_pixmap			0
#define ddd_g4vll_pixmap_z			1
#define ddd_g4vll_accumulator			2
#define ddd_g4vll_accumulator_z			3


/*
 * Peer rewrite left these atoms out.  They're included here as a short-term
 * solution ...
 */
#define _XA_KPC_DENALI_BOARD "_KPC_DENALI_BOARD"
#define _XA_STEREO_SCREEN "_STEREO_SCREEN"

/*
 ======================================================================
 */ 

extern ddt_g4vll_data *dde_g4vll_current_dcm_data;
extern struct list *dde_g4vll_dcm_list;
extern DtInt dde_g4vll_dcm_open_count;

    
    /*****************************/
    /***  Function Prototypes  ***/
    /*****************************/

void   ddr_g4vll_dcm_allocate_offscreen_resource  (int);
void   ddr_g4vll_dcm_become_current_driver  (DtPtr);
void   ddr_g4vll_dcm_blend_filter  (int);
void   ddr_g4vll_dcm_blend_texture (DtShort, DtShort, DtShort, DtShort, DtFlag);
int    ddr_g4vll_dcm_blend_transp  (DtShort, DtShort, DtShort, DtShort);
int    ddr_g4vll_dcm_check_device_extent  (void);
int    ddr_g4vll_dcm_get_board_number (Display *, int, int *); 
int    ddr_g4vll_dcm_check_for_stereo (Display *, int, DtFlag *);
void   ddr_g4vll_dcm_clear_filter_buffer  (void);
void   ddr_g4vll_dcm_clear_rectangle_depth (DtShort, DtShort, DtShort, DtShort);
void   ddr_g4vll_dcm_clear_rectangle_depth_and_color (
		DtShort, DtShort, DtShort, DtShort);
void   ddr_g4vll_dcm_clear_rectangle_transp_buffers (
		DtShort, DtShort, DtShort, DtShort);
void   ddr_g4vll_dcm_close_device  (void);
DtReal ddr_g4vll_dcm_convert_y  (DtReal);
DtPtr  ddr_g4vll_dcm_create_local_data  (DtObject);
void   ddr_g4vll_dcm_finish_transp  (void);
void   ddr_g4vll_dcm_flush  (void);
void   ddr_g4vll_dcm_geom_change_handler  (void);
DtFlag ddr_g4vll_dcm_initialize_device  (DtObject, DtPtr, DtPtr, DtPtr);
void   ddr_g4vll_dcm_inq_pseudo_range  (float [3]);
void   ddr_g4vll_dcm_inq_pseudo_space  (float [3], int [3]);
DtInt  ddr_g4vll_dcm_inq_transp_mode  (void);
void   ddr_g4vll_dcm_inquire_auto_size  (DtFlag*);
void   ddr_g4vll_dcm_inquire_clip_list (DtUInt*, DtReal**, DtReal[4], DtFlag*);
void   ddr_g4vll_dcm_inquire_color_entries  (DtColorModel, int, int, DtReal*);
void   ddr_g4vll_dcm_inquire_device_extent  (DtVolume *);
int    ddr_g4vll_dcm_inquire_head_number (void);
DtFlag ddr_g4vll_dcm_inquire_hw_boundingvol  (void);
void   ddr_g4vll_dcm_inquire_ncolors  (DtInt *);
DtFlag ddr_g4vll_dcm_inquire_pixel_data (
		DtRasterType, DtInt*, DtInt*, DtRasterType*, DtPtr*, DtFlag*);
void   ddr_g4vll_dcm_inquire_resolution  (DtReal *, DtReal *);
DtFlag ddr_g4vll_dcm_inquire_spheres  (void);
int    ddr_g4vll_dcm_inquire_cylinders  (void);
void   ddr_g4vll_dcm_inquire_state (VLLstate **);
DtFlag ddr_g4vll_dcm_inquire_if_states_match (VLLstate *);
void   ddr_g4vll_dcm_inquire_stereo  (DtFlag *);
DtFlag ddr_g4vll_dcm_inquire_stereo_screen  (void);
void   ddr_g4vll_dcm_inquire_visual_type  (DtVisualType *);
void   ddr_g4vll_dcm_set_background_color  (DtColorModel, DtReal *);
void   ddr_g4vll_dcm_set_color_entries  (DtColorModel, int, int, DtReal *);
void   ddr_g4vll_dcm_set_current_view  (DtObject);
void   ddr_g4vll_dcm_set_depth_buffer_enable  (DtFlag);
void   ddr_g4vll_dcm_set_depth_buffer_write  (DtFlag);
void   ddr_g4vll_dcm_set_foreground_color  (DtColorModel, DtReal *);
void   ddr_g4vll_dcm_set_offscreen_resource  (int);
void   ddr_g4vll_dcm_set_options  (DtPtr);
void   ddr_g4vll_dcm_set_shade_index  (DtInt);
void   ddr_g4vll_dcm_set_state  (VLLstate *);
void   ddr_g4vll_dcm_set_stereo_eye  (int);
void   ddr_g4vll_dcm_set_viewport  (DtReal, DtReal, DtReal, DtReal);
void   ddr_g4vll_dcm_setup_shading  (void);
void   ddr_g4vll_dcm_setup_transp  (DtFlag, DtFlag);
void   ddr_g4vll_dcm_swap_buffers  (void);
void   ddr_g4vll_dcm_update_geometry  (void);
void   ddr_g4vll_dcm_write_scanline_byte  (DtInt, DtInt, DtInt, DtUChar *);
DtPtr  ddr_g4vll_return_DCM_fcns  (void);

static void DorePeerActiveCallback (Display *, int);
static Bool XPeerEnableWindow (Display *, Window, unsigned int, VLLwindow **);
static void XPeerDisableWindow (Display *, Window);
static Bool XPeerInitialize (Display *, int);
static Bool XPeerUpdateState (Window);

#endif
