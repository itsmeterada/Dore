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
#include "cg.con.h"
#include "cg.str.h"

struct mat_element {
   DtMatrix4x4 mat;
};

struct matrix_stack {
   DtInt current_element;
   struct mat_element *elements;
   DtInt last_element;
};

struct clpvol_element {
   struct modclpvol *modclpvol;
};

struct clpvol_stack {
   DtInt current_element;
   struct clpvol_element *elements;
   DtInt last_element;
};

struct drom_data {
   DtObject		current_device;
   DtObject		current_view;

   DtReal 		object_ambient_intensity;
   DtSwitch 		object_ambient_switch;
   DtSwitch 		object_cull_switch;
   DtSwitch 		object_cullable_switch;
   DtColorRGB		object_depthcue_color;
   DtReal		object_depthcue_sfront;
   DtReal		object_depthcue_sback;
   DtSwitch		object_depthcue_switch;
   DtReal		object_depthcue_zfront;
   DtReal		object_depthcue_zback;
   DtColorRGB 		object_diffuse_color;
   DtReal 		object_diffuse_intensity;
   DtSwitch 		object_diffuse_switch;
   DtInt		object_hidsurf_switch;
   DtInterpType 	object_interpolation_type;
   DtMatrix4x4 		object_lcstowcsmat;
   DtSwitch		object_modelclip_switch;
   struct modclpvol	*object_modelclip_volume;
   DtRepType 		object_representation_type;
   DtInt		object_shade_index;
   DtColorRGB 		object_specular_color;
   DtReal 		object_specular_intensity;
   DtReal 		object_specular_factor;
   DtSwitch 		object_specular_switch;
   dot_object		*object_surface_shader;
   DtColorRGB 		object_transparent_color;
   DtReal 		object_transparent_intensity;
   DtSwitch 		object_transparent_switch;

   DtColorRGB		diffuse_reflectance;
   DtColorRGB		specular_reflectance;
   DtReal		specular_factor_table[256];
   DtColorRGB		transparent_reflectance;

   DtMatrix4x4		projmat;
   DtInt		viewtype;

   struct matrix_stack	mat_stack;
   struct clpvol_stack clpvol_stack;

   DtMatrix4x4		nrmwcstolcsmat;

   DtSwitch		pseudocolor_switch;
   DtSwitch 		draw_switch;

   DtFlag		cgparms_cullfl;

   DtFlag		transparent_surface_pass;
   DtFlag		transparent_surface_flag;

   DtInt		stereo_leye_flag;

   DtInt		modelclip_halfspacecount;
   DtPoint4		modelclip_plneqns[20];

   DtInt		mesh_vertex_count;
   double		*mesh_vertex_space;
   double		*mesh_perspective_space;

   DtInt		mesh_sync;

   DtColorRGB		ambient_reflectance;

   DtColorRGB		light_color;
   DtObject		light_type;
   DtReal		light_intensity;
   DtSwitch		stereo_switch;
   DtReal 		stereo_eyeseparation;
   DtReal		stereo_distance;
};

struct drom_cliprec {
   DtReal xmin;
   DtReal ymin;
   DtReal xmax;
   DtReal ymax;
};

struct drom_device_data {
   DtVisualType visual_type;
   DtShadeMode shade_mode;
   DtReal current_shade_range_min;
   DtReal current_shade_range_max;
   DtReal extent_leye_fbuf[4];
   DtReal extent_leye_bbuf[4];
   DtReal extent_reye_fbuf[4];
   DtReal extent_reye_bbuf[4];
   DtReal ctodscale_leye_fbuf[3];
   DtReal ctodscale_leye_bbuf[3];
   DtReal ctodscale_reye_fbuf[3];
   DtReal ctodscale_reye_bbuf[3];
   DtReal ctodtrans_leye_fbuf[3];
   DtReal ctodtrans_leye_bbuf[3];
   DtReal ctodtrans_reye_fbuf[3];
   DtReal ctodtrans_reye_bbuf[3];
   DtReal ftodscale[3];
   DtInt expansion_board;
   DtReal annoscale[3];
   DtInt stereoswitch;
   DtSwitch stereoscreen;
};

struct drom_view_data {
   DtFlag clear_flag;
   DtColorModel background_color_model;
   DtColorRGB background_color;
   struct drom_camera_data *camera_data;
   struct drom_light_data *light_data;
   DtInt shade_index;
};

struct drom_camera_data {
   DtMatrix4x4 projmat;
   DtMatrix4x4 ccstowcsmat;
   DtMatrix4x4 wcstoccsmat;
   DtMatrix4x4 npctowcsmat;
   DtMatrix4x4 wcstofcsmat;
   DtMatrix4x4 tspfcstowcsmat;
   DtInt camtype;
   DtReal wcscamloc[3];
   DtReal wcscamnorm[3];
   DtInt viewtype;
   DtSwitch stereoswitch;
   DtReal stereo_eyeseparation;
   DtReal stereo_distance;
   DtMatrix4x4 wcstofcsmat_left;
   DtMatrix4x4 wcstofcsmat_right;
   DtMatrix4x4 tspfcstowcsmat_left;
   DtMatrix4x4 tspfcstowcsmat_right;
   DtMatrix4x4 lcstofcsmat;
   DtMatrix4x4 lcstofcsmat_temp;
};

struct drom_light_data {
   DtColorRGB ambient_reflectance;
   DtInt count;
   DtInt max_count;
   DtRealTriple *cosine;
   DtReal *intens;
   DtColorRGB *color;
   DtRealTriple *h;
   DtRealTriple *lcscosine;
   DtColorRGB *scaledcolor;
};

struct drom_window_data {
   DtInt clipping_rectangle_count;
   DtReal obs_leye_fbuf[50][4];
   DtReal obs_leye_bbuf[50][4];
   DtReal obs_reye_fbuf[50][4];
   DtReal obs_reye_bbuf[50][4];

   DtReal clr_leye_fbuf[50][4];
   DtReal clr_leye_bbuf[50][4];
   DtReal clr_reye_fbuf[50][4];
   DtReal clr_reye_bbuf[50][4];
   DtInt visibility_state;
};


extern struct drom_data		 dde_x11_titang2_drom_data;
extern struct cg_tlparms	 dde_x11_titang2_drom_cg_tlparms[4];
extern struct cg_sparms		 dde_x11_titang2_drom_cg_sparms;
extern struct drom_device_data	*dde_x11_titang2_drom_curdevdat;
extern struct drom_view_data	*dde_x11_titang2_drom_curviwdat;
extern struct drom_window_data	*dde_x11_titang2_drom_curwindat;
extern struct drom_light_data	*dde_x11_titang2_drom_curlgtdat;
extern struct drom_camera_data	*dde_x11_titang2_drom_curcamdat;
extern DtSwitch			 dde_x11_titang2_drom_pre_init;
extern DtInt			 dde_x11_titang2_drom_draw_buffer;


#define ddd_x11_titang2_drom_viewtype_perspective      0
#define ddd_x11_titang2_drom_viewtype_parallel         1
#define ddd_x11_titang2_drom_viewtype_matrix           2

#define ddd_x11_titang2_drom_window_completely_obscured		1
#define ddd_x11_titang2_drom_window_completely_visible		2
#define ddd_x11_titang2_drom_window_partially_obscured		3

#define ddd_x11_titang2_drom_prigeotyp_trimsh			1
#define ddd_x11_titang2_drom_prigeotyp_trilst			2
#define ddd_x11_titang2_drom_prigeotyp_linlst			3
#define ddd_x11_titang2_drom_prigeotyp_pntlst			4
#define ddd_x11_titang2_drom_prigeotyp_conlinlst		5
