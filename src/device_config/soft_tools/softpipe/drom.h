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

#ifndef SOFTPIPE_DROM_H
#define SOFTPIPE_DROM_H
 
#include <dore/internal/dogen.h>
#include <dore/internal/driver.h>
#include <dore/dore_develop/develop.h>
#include <dore/dore_develop/private/stdtexatt.h>

struct mat_element {
    DtMatrix4x4 mat;
};

struct matrix_stack {
    DtInt current_element;
    struct mat_element *elements;
    DtInt last_element;
};

struct dynout_data {
    DtObject		current_device;
    DtObject		current_view;

    DtReal		object_ambient_intensity;
    DtSwitch		object_ambient_switch;
    DtSwitch		object_cull_switch;
    DtSwitch		object_cullable_switch;
    DtColorRGB		object_depthcue_color;
    DtReal		object_depthcue_sfront;
    DtReal		object_depthcue_sback;
    DtReal		object_depthcue_switch;
    DtReal		object_depthcue_zfront;
    DtReal		object_depthcue_zback;
    DtColorRGB		object_diffuse_color;
    DtReal		object_diffuse_intensity;
    DtSwitch		object_diffuse_switch;
    DtInt		object_hidsurf_switch;
    DtInterpType	object_interpolation_type;
    DtMatrix4x4		object_lcstowcsmat;
    DtSwitch		object_modelclip_switch;
    DtInt		object_modelclip_halfspacecount;
    DtPoint3		*object_modelclip_point;
    DtVector3		*object_modelclip_vector;
    DtPoint4		*object_modelclip_plneqn;
    DtRepType		object_representation_type;
    DtInt		object_shade_index;
    DtColorRGB		object_specular_color;
    DtReal		object_specular_intensity;
    DtReal		object_specular_factor;
    DtSwitch		object_specular_switch;
    DtObject		object_surface_shader;
    DtColorRGB		object_transparent_color;
    DtReal		object_transparent_intensity;
    DtSwitch		object_transparent_switch;

    DtColorRGB		light_color;
    DtObject		light_type;
    DtReal		light_intensity;

    DtSwitch		stereo_switch;
    DtReal		stereo_eyeseparation;
    DtReal		stereo_distance;

    DtColorRGB		ambient_reflectance;
    DtColorRGB		diffuse_reflectance;
    DtColorRGB		specular_reflectance;
    DtReal		specular_factor_table[256];
    DtColorRGB		transparent_reflectance;

    DtMatrix4x4		projmat;
    DtInt		viewtype;

    struct matrix_stack	mat_stack;

    DtMatrix4x4		nrmwcstolcsmat;

    DtSwitch		pseudocolor_switch;
    DtSwitch		draw_switch;

    DtFlag		xyclipped_flag;

    DtRealTriple		*colored_elements;
    DtInt		colored_element_count;

    DtInt		*color_indices;
    DtInt		color_indices_count;
    DtInt		*backfacing_flags;
    DtInt		backfacing_flags_count;

    DtRealTriple	*xformed_vtxlocs;
    DtInt		xformed_vtxlocs_count;

    DtRealTriple	*trimsh_unfold_vtx;
    DtInt		trimsh_unfold_vtx_cnt;

    DtRealTriple	*trimsh_unfold_clr;
    DtInt		trimsh_unfold_clr_cnt;

    DtRealTriple	*trimsh_unfold_nrm;
    DtInt		trimsh_unfold_nrm_cnt;

    void (*draw_point_dc)    (/* DtInt *point, DtColorRGB color */);
    void (*draw_line_dc)     (/* DtInt *point1, DtInt *point2,
				 DtColorRGB color1, DtColorRGB color2 */);
    void (*draw_triangle_dc) (/* DtInt *point1, DtInt *point2,
				 DtInt *point3, DtColorRGB color1,
				 DtColorRGB color2,  DtColorRGB color3 */);
};

struct dynout_device_data {
   DtSwitch		stereoswitch;
   DtVisualType		visualtype;
   DtShadeMode		shade_mode;
   DtReal		current_shade_range_min;
   DtReal		current_shade_range_max;
   DtReal		ctodscale[3];
   DtReal		ctodtranslate[3];
   DtReal		ftodscale[3];
   DtReal		annoscale[3];
};

struct dynout_view_data {
   DtFlag		clear_flag;
   DtColorModel		background_color_model;
   DtColorRGB		background_color;
   DtInt		shade_index;
   struct dynout_camera_data *camera_data;
   struct dynout_light_data *light_data;
};

struct dynout_camera_data {
   DtMatrix4x4		projmat;
   DtMatrix4x4		ccstowcsmat;
   DtMatrix4x4		wcstoccsmat;
   DtMatrix4x4		npctowcsmat;
   DtMatrix4x4		wcstofcsmat;
   DtInt		camtype;
   DtReal		wcscamloc[3];
   DtReal		wcscamnorm[3];
   DtInt		viewtype;
   DtMatrix4x4		lcstofcsmat;
   DtMatrix4x4		lcstofcsmat_temp;
};

struct dynout_light_data {
   DtColorRGB		ambient_reflectance;
   DtInt		count;
   DtInt		max_count;
   DtRealTriple		*cosine;
   DtReal		*intens;
   DtColorRGB		*color;
   DtRealTriple		*h;
   DtRealTriple		*lcscosine;
   DtColorRGB		*scaledcolor;
};

struct dynout_cliprec {
   DtReal		xmin;
   DtReal		ymin;
   DtReal		xmax;
   DtReal		ymax;
};

struct dynout_window_data {
   DtInt		clipping_rectangle_count;
   struct dynout_cliprec clipping_rectangles[100];
   DtInt		visibility_state;
};

#define ddd_softpipe_drom_viewtype_perspective		0
#define ddd_softpipe_drom_viewtype_parallel		1
#define ddd_softpipe_drom_viewtype_matrix		2

#define ddd_softpipe_drom_window_completely_obscured	1
#define ddd_softpipe_drom_window_completely_visible	2
#define ddd_softpipe_drom_window_partially_obscured	3

#define ddd_softpipe_drom_no_normals			0
#define ddd_softpipe_drom_element_normals		1
#define ddd_softpipe_drom_vertex_normals		2

#define ddd_softpipe_drom_element_shading		1
#define ddd_softpipe_drom_vertex_shading		2

extern struct dynout_data		dde_softpipe_drom_data;
extern struct dynout_device_data	*dde_softpipe_drom_curdevdat;
extern struct dynout_view_data		*dde_softpipe_drom_curviwdat;
extern struct dynout_window_data	*dde_softpipe_drom_curwindat;
extern struct dynout_light_data		*dde_softpipe_drom_curlgtdat;
extern struct dynout_camera_data	*dde_softpipe_drom_curcamdat;

extern int dde_softpipe_drom_pre_init;

#endif
