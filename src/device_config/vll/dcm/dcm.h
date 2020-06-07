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
 
#include "dore/internal/dogen.h"
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/XTitan.h>
#define _VLL_G3
#include <vll.h>
#undef _VLL_G3
 
typedef struct {
    DtObject		device;

    int			x;
    int			y;
    int			width;
    int			height;

    Window		window;
    Display *		display;
    GC			gc;
    DtFlag		user_window;
    DtFlag		user_display;
    DtFlag		use_X;
    XBoxPtr		clip_rect;
    int			nrects;
    unsigned int	sequence_number;
    unsigned int	clipid;

    int			double_buffered;
    DtFlag		use_spheres;
    int			transp_type;
    DtFlag		auto_size;
    DtVisualType	visualtype;
    int			ncolors;
    int			stereoswitch;
    DtFlag		bad_geom;

    DtObject		current_view;
    DtObject		previous_view;

    float		background_color[3];
    DtInt		shade_index;
    float		cbtselpsweight[3];
    DtInt		cbtselpsmask[3];
    float		cwtavgpsweight[3];
    float		cwtavgpslim[3];
    DtShadeMode		shade_mode;
    unsigned long	bg_color_index;
    int			texture_expand;
    DtFlag		clr_device;
    DtFlag		offscreen;
    unsigned int	output_buffer;
} ddt_vll_data;

/*
 ======================================================================
 */

#define ddd_vll_expand_default		0
#define ddd_vll_expand_replicate	1
#define ddd_vll_expand_bilinear		2

/*
 ======================================================================
 */ 

extern ddt_vll_data *dde_vll_current_dcm_data;
extern struct list *dde_vll_dcm_list;
extern DtInt dde_vll_dcm_open_count;

/*
 ======================================================================
 */ 
