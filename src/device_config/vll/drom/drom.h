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
#define _VLL_G3
#include <vll.h>
#undef _VLL_G3
 
typedef struct {
    float		object_matrix[4][4];
    float		light_matrix[4][4];
    float		normal_matrix[4][4];
} ddt_vll_drom_matrix_element;

typedef struct matrix_stack {
   DtInt		current_element;
   ddt_vll_drom_matrix_element *elements;
   DtInt		last_element;
} ddt_vll_drom_matrix_stack;

typedef struct clpvol_element {
   struct modclpvol *modclpvol;
} ddt_vll_drom_clpvol_element;

typedef struct clpvol_stack {
   DtInt current_element;
   ddt_vll_drom_clpvol_element *elements;
   DtInt last_element;
} ddt_vll_drom_clpvol_stack;

typedef struct {
   DtObject		current_device;
   DtObject		current_view;

   DtSwitch		ambient_switch;
   DtSwitch		diffuse_switch;
   DtSwitch		specular_switch;

   DtSwitch		hidden_surf_switch;
   DtSwitch		transparent_switch;
   DtSwitch		shadow_switch;
   DtSwitch		backface_cullable_switch;
   DtSwitch		backface_cull_switch;
   DtSwitch		modelclip_switch;

   float		ambient_intensity;
   float		diffuse_color[3];
   float		diffuse_intensity;
   float		specular_color[3];
   float		specular_intensity;
   float		specular_factor;
   float		transparent_color[3];
   float		transparent_intensity;

   DtInterpType 	interptype;
   DtRepType		reptype;

   DtInt		shade_index;
   DtObject		surface_shader;
   DtSwitch		constant_shading_switch;

   DtMatrix4x4		projmat;
   DtCameraMatrixType	viewtype;
   DtReal		hither;
   DtReal		yon;
   DtSwitch		stereo_switch;
   DtReal 		stereo_eyeseparation;
   DtReal		stereo_distance;

   float		light_color[3];
   float		light_intensity;
   float		light_spread_angle_total;
   float		light_spread_angle_delta;
   float		light_spread_exponent;
   float		light_att_c1;
   float		light_att_c2;
   DtObject		light_type;

   ddt_vll_drom_matrix_stack	matrix_stack;

   DtSwitch		texture_2D_valid;
   DtSwitch		texture_3D_valid;
   DtSwitch		texture_env_valid;
   DtSwitch		map_diffuse_color;
   DtSwitch		map_env;
   DtInt		map_diffuse_color_uv_index;
   DtInt		map_diffuse_color_uvw_index;
   DtSwitch		map_extend_black;
   DtTextureOperator	map_operator;

   ddt_vll_drom_clpvol_stack	clpvol_stack;

   DtSwitch		localaa_switch;
   DtLocalAntiAliasStyle localaa_style;

   float		surface_edge_color[3];

   DtInt		traversal_type;

   DtFlag		have_no_norm_textures;
   DtFlag		have_shadows;
   DtFlag		have_spheres;
   DtFlag		have_textures;
   DtFlag		have_transparency;
} ddt_vll_drom_data;


typedef struct {
   DtSwitch		stereoswitch;
   DtReal		stereo_eyeseparation;
   DtReal		stereo_distance;

   DtCameraMatrixType	viewtype;
   DtReal		hither;
   DtReal		yon;
   DtMatrix4x4		projmat;
   DtMatrix4x4		camera_to_world_matrix;

   DtMatrix4x4		world_to_camera_matrix;
   DtMatrix4x4		npctowcsmat;
   DtMatrix4x4		world_to_frustum_matrix;
   DtMatrix4x4		tspfcstowcsmat;
   DtInt		camtype;
   float		wcscamloc[3];
   float		wcscamnorm[3];
   DtMatrix4x4		world_to_frustum_matrix_left;
   DtMatrix4x4		world_to_frustum_matrix_right;
   DtMatrix4x4		tspfcstowcsmat_left;
   DtMatrix4x4		tspfcstowcsmat_right;
   DtSwitch		volumeswitch;
   float		camloc[4];

   DtSwitch		super_sample_switch;
   DtInt		filter_center[2];
   DtReal		filter_step[2];
   DtInt		filter_size[2];
   int			*filter;
}ddt_vll_drom_camera;

typedef struct {
   DtVisualType		visual_type;
   DtShadeMode		shade_mode;
   DtInt		pseudo_range[2];
   float		pseudo_range_weight[3];
   float		pseudo_space_weight[3];
   int			pseudo_space_mask[3];
   DtReal		ftodscale[3];
   DtReal		annoscale[3];
   DtInt		stereoswitch;
   DtSwitch		use_spheres;
   VLLinfo		*features;
} ddt_vll_drom_device;

typedef struct {
   DtFlag		clear_flag;
   DtColorModel		background_color_model;
   DtColorRGB		background_color;
   DtObject		background_raster;
   DtObject		background_callback;
   DtReal		background_just[2];
   ddt_vll_drom_camera camera_data;
   DtInt		total_lights;
   DtInt		max_lights;
   DtObject 		*light;
   DtInt		shade_index;
   DtFlag		have_textures;
   DtFlag		have_spheres;
} ddt_vll_drom_view;

typedef struct {
   DtReal		actviewxmin;
   DtReal		actviewymin;
   DtReal		actviewxmax;
   DtReal		actviewymax;
} ddt_vll_drom_window;


typedef struct {
   DtPtr		name;
   DtInt		id;
   DtPFI		crt_rtn;
   DtPFI		prt_rtn;
   DtPFI		del_rtn;
   DtInt		index;
} ddt_vll_drom_class_data;

typedef struct {
   DtInt		texture_sequence_number;
   VLLtexture		*texture;
   DtPtr		texture_data;

   DtInt		env_texture_sequence_number;
   VLLtexture		*env_texture;
   DtPtr		env_texture_data;

   DtInt		background_sequence_number;
   DtPtr		background_data;
   DtInt		background_mode;
} ddt_vll_drom_rstr;

typedef struct {
   DtInt		*edge_enable;
   DtInt		sequence_number;
} ddt_vll_drom_edgelist_data;

typedef struct {
    DtFlag initialized;
    DtReal bbox[4][2];
} ddt_vll_drom_bboxuv;

typedef struct {
    DtFlag initialized;
    DtReal bbox[8][3];
} ddt_vll_drom_bboxuvw;

/* 
 ======================================================================
 */
 
extern ddt_vll_drom_data dde_vll_drom_data;

extern ddt_vll_drom_device *dde_vll_drom_curdevdat;
extern ddt_vll_drom_view   *dde_vll_drom_curviwdat;
extern ddt_vll_drom_window *dde_vll_drom_curwindat;
extern ddt_vll_drom_camera *dde_vll_drom_curcamdat;

extern DtSwitch dde_vll_drom_pre_init;

extern ddt_vll_drom_class_data dde_vll_drom_class_data[];

/*
 ======================================================================
 */

#define ddd_vll_drom_standard_pass	1
#define ddd_vll_drom_texture_pass	2
#define ddd_vll_drom_transp_pass	3
#define ddd_vll_drom_shadow_pass	4

#define ddd_vll_drom_rstr_user_data	0x1
#define ddd_vll_drom_rstr_with_z	0x2

#define ddd_vll_expand_default		0
#define ddd_vll_expand_replicate	1
#define ddd_vll_expand_bilinear		2

/*
 ======================================================================
 */
