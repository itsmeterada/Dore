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
   DtMatrix4x4		object_matrix;
   DtMatrix4x4		light_matrix;
   DtMatrix4x4		normal_matrix;
} ddt_x11_XFDI_drom_matrix_element;

typedef struct matrix_stack {
   DtInt		current_element;
   ddt_x11_XFDI_drom_matrix_element *elements;
   DtInt		last_element;
} ddt_x11_XFDI_drom_matrix_stack;

typedef struct clpvol_element {
   struct modclpvol *modclpvol;
} ddt_x11_XFDI_drom_clpvol_element;

typedef struct clpvol_stack {
   DtInt current_element;
   ddt_x11_XFDI_drom_clpvol_element *elements;
   DtInt last_element;
} ddt_x11_XFDI_drom_clpvol_stack;


typedef struct {
   DtObject		current_device;
   DtObject		current_view;

   DtSwitch		ambient_switch;
   DtSwitch		diffuse_switch;
   DtSwitch		specular_switch;

   DtSwitch		hidden_surf_switch;
   DtSwitch		transparent_switch;
   DtSwitch		backface_cullable_switch;
   DtSwitch		backface_cull_switch;
   DtSwitch		modelclip_switch;

   DtReal		ambient_intensity;
   DtColorRGB		diffuse_color;
   DtReal		diffuse_intensity;
   DtColorRGB		specular_color;
   DtReal		specular_intensity;
   DtReal		specular_factor;
   DtColorRGB		transparent_color;
   DtReal		transparent_intensity;

   DtInterpType 	interptype;
   DtRepType		reptype;

   DtInt		shade_index;
   DtObject		surface_shader;
   DtSwitch		constant_shading_switch;

   DtMatrix4x4		projmat;
   DtCameraMatrixType	viewtype;
   DtSwitch		stereo_switch;
   DtReal 		stereo_eyeseparation;
   DtReal		stereo_distance;

   DtColorRGB		light_color;
   DtReal		light_intensity;
   DtReal		light_spread_angle_total;
   DtReal		light_spread_angle_delta;
   DtReal		light_spread_exponent;
   DtReal		light_att_c1;
   DtReal		light_att_c2;
   DtObject		light_type;

   ddt_x11_XFDI_drom_matrix_stack	matrix_stack;

   DtSwitch		transparent_surface_flag;
   DtSwitch		transparent_surface_pass;

   DtSwitch		texture_2D_valid;
   DtSwitch		texture_3D_valid;
   DtSwitch		texture_pass;
   DtSwitch		texture_pass_required;
   DtSwitch		texture_no_norms;
   DtSwitch		map_diffuse_color;
   DtInt		map_diffuse_color_uv_index;
   DtInt		map_diffuse_color_uvw_index;

   DtInt 		max_cvt_colors;
   unsigned long	*converted_colors;

   ddt_x11_XFDI_drom_clpvol_stack	clpvol_stack;

   DtSwitch		spheres_required;
} ddt_x11_XFDI_drom_data;


typedef struct {
   DtSwitch		stereoswitch;
   DtReal		stereo_eyeseparation;
   DtReal		stereo_distance;

   DtCameraMatrixType	viewtype;
   DtMatrix4x4		projmat;
   DtMatrix4x4		camera_to_world_matrix;

   DtMatrix4x4		world_to_camera_matrix;
   DtMatrix4x4		npctowcsmat;
   DtMatrix4x4		world_to_frustum_matrix;
   DtMatrix4x4		tspfcstowcsmat;
   DtInt		camtype;
   DtReal		wcscamloc[3];
   DtReal		wcscamnorm[3];
   DtMatrix4x4		world_to_frustum_matrix_left;
   DtMatrix4x4		world_to_frustum_matrix_right;
   DtMatrix4x4		tspfcstowcsmat_left;
   DtMatrix4x4		tspfcstowcsmat_right;
   DtSwitch		volumeswitch;
   DtReal		camloc[4];
}ddt_x11_XFDI_drom_camera;

typedef struct {
   Display *		display;
   XFDIGC 		gc;
   DtVisualType		visual_type;
   DtShadeMode		shade_mode;
   DtReal		*cwtavgpsweight;
   DtInt		current_shade_range_min;
   DtInt		current_shade_range_max;
   DtInt		shade_range_scale;
   DtReal		ftodscale[3];
   DtReal		annoscale[3];
   DtInt		stereoswitch;
   DtSwitch		stereoscreen;
   DtSwitch		use_spheres;
} ddt_x11_XFDI_drom_device;

typedef struct {
   DtFlag		clear_flag;
   DtColorModel		background_color_model;
   DtColorRGB		background_color;
   ddt_x11_XFDI_drom_camera camera_data;
   DtInt		total_lights;
   DtObject 		light[16];
   DtInt		shade_index;
   DtFlag		texture_pass_needed;
   DtFlag		have_spheres;
} ddt_x11_XFDI_drom_view;

typedef struct {
   DtReal		actviewxmin;
   DtReal		actviewymin;
   DtReal		actviewxmax;
   DtReal		actviewymax;
} ddt_x11_XFDI_drom_window;


typedef struct {
   DtPtr		name;
   DtInt		id;
   DtPFI		crt_rtn;
   DtPFI		prt_rtn;
   DtPFI		del_rtn;
   DtInt		index;
} ddt_x11_XFDI_drom_class_data;

typedef struct {
   DtInt		sequence_number;
   XID			texture_id;
   Pixmap		pixmap;
} ddt_x11_XFDI_drom_rstr;

typedef struct {
   unsigned long	*converted_clrs;
   DtInt		sequence_number;
   DtVisualType		visual_type;
   DtShadeMode		shade_mode;
   DtInt		shade_index;
} ddt_x11_XFDI_drom_cvtclrs_data;



extern ddt_x11_XFDI_drom_data    dde_x11_XFDI_drom_data;
extern ddt_x11_XFDI_drom_device *dde_x11_XFDI_drom_curdevdat;
extern ddt_x11_XFDI_drom_view   *dde_x11_XFDI_drom_curviwdat;
extern ddt_x11_XFDI_drom_window *dde_x11_XFDI_drom_curwindat;
extern ddt_x11_XFDI_drom_camera *dde_x11_XFDI_drom_curcamdat;
extern DtSwitch			 dde_x11_XFDI_drom_pre_init;

extern ddt_x11_XFDI_drom_class_data dde_x11_XFDI_drom_class_data[];

