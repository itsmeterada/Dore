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
 
#include <dore/dore.h>
#include <dore/dore_develop/error.h>
#include <dore/dore_develop/develop.h>
#include <dore/dore_develop/private/stdtexatt.h>

typedef struct {
    DtMatrix4x4		lcstowcsmat;
    DtMatrix4x4		lcstofcsmat;
    DtMatrix4x4		nrmlcstowcsmat;
} ddt_template_drom_matrix;

typedef struct {
    DtSwitch		status;
    DtInt 		max_count;
    DtInt		count;
    DtObject		*mapping;
    DtObject		*raster;
    dot_stdtexatt	*attributes;
} ddt_template_drom_texmap;

/*
 * Data structure to hold all attributes
 */

typedef struct {
    DtObject		device;
    DtObject		view;

    /* matrices and stack */

    DtMatrix4x4		lcstowcsmat;	/* local to world */
    DtMatrix4x4		lcstofcsmat;	/* local to frustum */
    DtMatrix4x4		nrmlcstowcsmat;	/* local to world for normals */

    DtInt		max_matrices;	/* max depth of matrix stack */
    DtInt		num_matrices;	/* depth of matrix stack */
    ddt_template_drom_matrix *matrix_stack;

    /* camera attributes */

    DtReal		stereo_eyeseparation;
    DtReal		stereo_distance;
    DtSwitch		stereo_switch;
    DtCameraMatrixType	camera_type;
    DtMatrix4x4		projmat;

    /* light attributes */

    DtObject		light_type;
    DtColorRGB		light_color;
    DtReal		light_intensity;
    DtReal		light_spread_angle_total;
    DtReal		light_spread_angle_delta;
    DtReal		light_spread_exponent;
    DtReal		light_att_c1;
    DtReal		light_att_c2;

    /* appearance attributes */

    DtReal		ambint;
    DtSwitch		ambswi;
    DtSwitch 		bacfacculble;
    DtSwitch		bacfacculswi;
    DtReal		depcue_zfront;
    DtReal		depcue_zback;
    DtReal		depcue_sfront;
    DtReal		depcue_sback;
    DtColorModel	depcue_colormodel;
    DtColorRGB		depcue_color;
    DtSwitch		depcueswi;
    DtColorRGB		difclr;
    DtReal		difint;
    DtSwitch		difswi;
    DtSwitch		hidsrfswi;
    DtInterpType	inttyp;
    DtLineType		lintyp;
    DtReal		linwid;
    DtLocalAntiAliasStyle localaasty;
    DtSwitch		localaaswi;
    DtSwitch		refswi;
    DtRepType		reptyp;
    DtSwitch		shaswi;
    DtInt		shdidx;
    DtColorRGB		spcclr;
    DtReal		spcfct;
    DtReal		spcint;
    DtSwitch		spcswi;
    DtColorRGB		srfedgclr;
    DtObject		srfshd;
    DtColorRGB		transpclr;
    DtReal		transpint;
    DtSwitch		transpswi;

    /* Texture Mapping Attributes */

    ddt_template_drom_texmap mapbmp;
    ddt_template_drom_texmap mapdifclr;
    ddt_template_drom_texmap mapenv;
    ddt_template_drom_texmap maptrnint;

} ddt_template_drom_data;

/*
 * The local data for the device
 */

typedef struct {
    DtVisualType	visual_type;
    DtShadeMode		shade_mode;
    DtReal 		ctodscale[3];
    DtReal		ctodtrans[3];
    DtReal		ftodscale[3];
    DtReal		annoscale[3];
    DtInt		stereoswitch;
} ddt_template_drom_device;

/*
 * The camera data structure
 */

typedef struct {
    DtCameraMatrixType	camera_type;
    DtSwitch		stereoswitch;
    DtReal		stereo_eyeseparation;
    DtReal		stereo_distance;
    DtMatrix4x4		projmat;
    DtMatrix4x4		camtowcsmat;	/* camera to world matrix */
    DtMatrix4x4		wcstocammat;	/* world to camera matrix */
    DtMatrix4x4		wcstofcsmat;	/* world to frustum matrix */
    DtReal		wcscamloc[3];	/* camera location in world coordinates */
    DtReal		wcscamnorm[3];	/* camera direction in world coordinates */
} ddt_template_drom_camera;

/*
 * The light data structure
 */

typedef struct {
    DtObject		light;
    DtObject		type;
    DtReal		position[3];
    DtReal 		direction[3];
    DtColorRGB		color;
    DtReal 		angle_total;
    DtReal		angle_delta;
    DtReal		exponent;
    DtReal		c1;
    DtReal		c2;
    DtSwitch		status;
} ddt_template_drom_light;

/*
 * The local data for the view
 */

typedef struct {
    DtFlag		clear_flag;
    DtInt		shade_index;
    DtColorModel	background_color_model;
    DtColorRGB		background_color;
    DtReal		background_just[2];
    ddt_template_drom_camera camera_data;
    DtInt		total_lights;
    DtInt		max_lights;
    ddt_template_drom_light *light;
} ddt_template_drom_view;
    
/*
 * The local data for the device / view combination
 */

typedef struct {
    DtReal		actviewxmin;
    DtReal		actviewymin;
    DtReal		actviewxmax;
    DtReal		actviewymax;
} ddt_template_drom_window;

/*
 ======================================================================
 */
#define abs(a)		(((a) >= 0.) ? (a) : -(a))
/*
 ======================================================================
 */

extern ddt_template_drom_data dde_template_drom_data;

extern ddt_template_drom_device *dde_template_drom_curdevdat;
extern ddt_template_drom_view   *dde_template_drom_curviwdat;
extern ddt_template_drom_window *dde_template_drom_curwindat;
extern ddt_template_drom_camera *dde_template_drom_curcamdat;

extern DtFlag dde_template_drom_initialized;
extern DtFlag dde_template_drom_pre_init;

/*
 ======================================================================
 */
