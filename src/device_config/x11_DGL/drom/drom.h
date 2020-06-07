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
#include "dore/dore_develop/develop.h"

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/XTitan.h>
#include <X11/XB.h>
#include <X11/Xd.h>

struct mat_element {
   XdMatrix4x4		mat;
};

struct matrix_stack {
   DtInt		current_element;
   struct mat_element	*elements;
   DtInt		last_element;
};

struct clpvol_element {
   struct modclpvol	*modclpvol;
};

struct clpvol_stack {
   DtInt		current_element;
   struct clpvol_element *elements;
   DtInt		last_element;
};

struct drom_data {
   DtObject		current_device;
   DtObject		current_view;

   DtSwitch 		object_cull_switch;
   DtSwitch 		object_cullable_switch;

   XdReal 		object_ambient_intensity;
   XdReal 		object_diffuse_intensity;
   XdColor 		object_diffuse_color;
   XdColor 		object_transparent_color;
   XdReal 		object_transparent_intensity;
   DtSwitch 		object_transparent_switch;
   XdReal		object_transparent_diffuse_intensity;
   XdReal		object_transparent_ambient_intensity;

   DtInt		object_hidsurf_switch;
   DtInt		object_shade_index;

   XdReal 		object_specular_factor;

   dot_object		*object_surface_shader;
   DtReal		specular_factor_table[256];

   XdMatrix4x4		projmat;
   DtInt		viewtype;

   XdMatrix4x4 		object_lcstowcsmat;
   struct matrix_stack	mat_stack;

   DtSwitch		object_modelclip_switch;
   struct modclpvol	*object_modelclip_volume;
   struct clpvol_stack	clpvol_stack;

   XdMatrix4x4		nrmwcstolcsmat;

   DtSwitch		pseudocolor_switch;
   DtSwitch 		draw_switch;

   DtFlag		transparent_surface_pass;
   DtFlag		transparent_surface_flag;

   DtInt		stereo_leye_flag;

   DtInt		modelclip_halfspacecount;
   XdReal		modelclip_plneqns[50][4];

   DtColorRGB		ambient_reflectance;

   XdColor		light_color;
   XdReal		light_intensity;
   XdLightType		light_type;
   unsigned int		light_mask;
   DtReal		light_spread_exponent;
   DtReal		light_total_angle;
   DtReal		light_delta_angle;
   DtReal		light_atten_c1;
   DtReal		light_atten_c2;

   DtSwitch		stereo_switch;
   DtReal 		stereo_eyeseparation;
   DtReal		stereo_distance;
};

struct drom_device_data {
   XdDrawablePtr	pDraw;
   XdGCPtr		pGC;
   DtVisualType		visual_type;
   DtShadeMode		shade_mode;
   DtInt		current_shade_range_min;
   DtInt		current_shade_range_max;
   XdReal		ctodscale[3];
   XdReal		ctodtrans[3];
   DtReal		ftodscale[3];
   XdReal		annoscale[3];
   DtInt		stereoswitch;
   DtSwitch		stereoscreen;
};

struct drom_view_data {
   DtFlag		clear_flag;
   DtColorModel		background_color_model;
   DtColorRGB		background_color;
   DtInt		shade_index;
   struct drom_camera_data *camera_data;
   struct drom_light_data *light_data;
};

struct drom_camera_data {
   XdMatrix4x4		projmat;
   XdMatrix4x4		ccstowcsmat;
   XdMatrix4x4		wcstoccsmat;
   XdMatrix4x4		npctowcsmat;
   XdMatrix4x4		ccstofcsmat;
   XdMatrix4x4		wcstofcsmat;
   XdMatrix4x4		tspfcstowcsmat;
   DtInt		camtype;
   DtReal		wcscamloc[3];
   DtReal		wcscamnorm[3];
   DtInt		viewtype;
   DtSwitch		stereoswitch;
   DtReal		stereo_eyeseparation;
   DtReal		stereo_distance;
   XdMatrix4x4		wcstofcsmat_left;
   XdMatrix4x4		wcstofcsmat_right;
   XdMatrix4x4		tspfcstowcsmat_left;
   XdMatrix4x4		tspfcstowcsmat_right;
   XdMatrix4x4		lcstofcsmat;
   XdMatrix4x4		lcstofcsmat_temp;
};

struct drom_light_data {
   DtInt		count;
   DtInt		max_count;
   DtObject		*light_objs;
   XdLightPtr		lights;
};

struct drom_window_data {
   DtReal		extent[4];
   DtInt		clipping_rectangle_count;
   DtReal		vis_rect[100][4];
   DtInt		visibility_state;
};

extern struct drom_data		 dde_x11_DGL_drom_data;
extern struct drom_device_data	*dde_x11_DGL_drom_curdevdat;
extern struct drom_view_data	*dde_x11_DGL_drom_curviwdat;
extern struct drom_window_data	*dde_x11_DGL_drom_curwindat;
extern struct drom_light_data	*dde_x11_DGL_drom_curlgtdat;
extern struct drom_camera_data	*dde_x11_DGL_drom_curcamdat;
extern DtSwitch			 dde_x11_DGL_drom_pre_init;

#define ddd_x11_DGL_drom_viewtype_perspective      0
#define ddd_x11_DGL_drom_viewtype_parallel         1
#define ddd_x11_DGL_drom_viewtype_matrix           2

#define ddd_x11_DGL_drom_window_completely_obscured	1
#define ddd_x11_DGL_drom_window_completely_visible	2
#define ddd_x11_DGL_drom_window_partially_obscured	3
