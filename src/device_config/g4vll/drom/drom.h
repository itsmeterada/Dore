#ifndef DORE_G4VLL_DROM_H
#define DORE_G4VLL_DROM_H

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
#include <dore/internal/modclpvol.h>
#include <dore/dore_develop/develop.h>
#include <dore/dore_develop/private/stdtexatt.h>
#include <dore/dore_develop/private/mapdifclr.h>
#include <dore/dore_develop/private/sphlst.h>
#include <dore/dore_develop/private/prisrf.h>
#include <vll.h>
 
typedef struct {
    float  object_matrix[4][4];
    float  light_matrix[4][4];
    float  normal_matrix[4][4];
} ddt_g4vll_drom_matrix_element;

typedef struct matrix_stack {
   DtInt		current_element;
   ddt_g4vll_drom_matrix_element *elements;
   DtInt		last_element;
} ddt_g4vll_drom_matrix_stack;

typedef struct clpvol_element {
   dot_modclpvol *modclpvol;
} ddt_g4vll_drom_clpvol_element;

typedef struct clpvol_stack {
   DtInt current_element;
   ddt_g4vll_drom_clpvol_element *elements;
   DtInt last_element;
} ddt_g4vll_drom_clpvol_stack;

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
   DtSwitch		super_sample_switch;
   DtInt		samples[2];
   DtObject		filter;
   DtReal		filter_width[2];

   float		light_color[3];
   float		light_intensity;
   float		light_spread_angle_total;
   float		light_spread_angle_delta;
   float		light_spread_exponent;
   float		light_att_c1;
   float		light_att_c2;
   DtObject		light_type;

   ddt_g4vll_drom_matrix_stack	matrix_stack;

   DtSwitch		texture_2D_valid;
   DtSwitch		texture_3D_valid;
   DtSwitch             tex2D_finished;		/*   Current Texmap has no */
   DtSwitch             tex3D_finished;         /* more tiles left to render. */
   DtSwitch		texture_env_valid;
   DtSwitch		map_diffuse_color_switch;
   DtSwitch		map_env_switch;
   DtInt		map_diffuse_color_uv_index;
   DtInt		map_diffuse_color_uvw_index;
   DtSwitch		map_extend_black;
   int			vll_extend_mode[3]; /*  0=u, 1=v, 2=w  */
   DtTextureOperator	map_operator;
   DtTextureAntiAliasMode map_tile_aamode;  /* Currently None or Bilinear only*/

   ddt_g4vll_drom_clpvol_stack	clpvol_stack;

   DtSwitch		localaa_switch;
   DtLocalAntiAliasStyle localaa_style;

   float		surface_edge_color[3];

   DtInt		traversal_type;

   DtFlag		have_no_norm_textures;
   DtFlag		have_shadows;
   DtFlag		have_textures;
   DtFlag		have_transparency;
   DtFlag		have_transp_textured_objects;
   DtFlag		have_opaque_objects;
   DtFlag		have_env_mapping;
   DtFlag		have_mip_mapping;

   DtInt		num_texture_passes; /* >1 if tiling is necessary */
   DtInt		cur_texture_pass;

   unsigned		texpass_modes;      /* Tex draw/lite/vcol/spec pass */
   unsigned		current_texpass_mode;		

   VLLclip_status	*clip_status;

   float		transporient_color[3];
   float		transporient_intensity;
   float		transporient_exp;

   DtSwitch 		pre_init;

} ddt_g4vll_drom_data;


typedef struct {
   DtSwitch		stereoswitch;
   DtReal		stereo_eyeseparation;
   DtReal		stereo_distance;
   DtReal		stereo_view_scale;

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
} ddt_g4vll_drom_camera;

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
   int			cylinders;
   DtSwitch		use_hw_boundingvol;
   VLLinfo		*features;
   DtFlag		stereoscreen;
   int			head;
   VLLstate		*vll_state;
} ddt_g4vll_drom_device;

typedef struct {
   DtFlag		clear_flag;
   DtColorModel		background_color_model;
   DtColorRGB		background_color;
   DtObject		background_raster;
   DtObject		background_callback;
   DtReal		background_just[2];
   ddt_g4vll_drom_camera camera_data;
   DtInt		total_lights;
   DtInt		max_lights;
   DtObject 		*light;
   DtInt		shade_index;
   DtFlag		have_textures;
} ddt_g4vll_drom_view;

typedef struct {
   DtReal		actviewxmin;
   DtReal		actviewymin;
   DtReal		actviewxmax;
   DtReal		actviewymax;
} ddt_g4vll_drom_window;


typedef struct {
   DtPtr	name;
   DtInt	id;
   DtMethodPtr	crt_rtn;
   DtMethodPtr	prt_rtn;
   DtMethodPtr	del_rtn;
   DtInt	index;
} ddt_g4vll_drom_class_data;

typedef struct {
    DtInt               texture_sequence_number;
    DtPtr               texture_data;

    DtInt               env_texture_sequence_number;
    VLLtexture         *env_texture;
    DtPtr               env_texture_data;

    DtInt		background_sequence_number;
    DtPtr		background_data;
    DtInt		background_mode;

    /* Raster Tiling Data */

    DtFlag              tiling_initialized;  /* True if raster is ready. */
    DtInt               num_tiles;           /* Number of Raster Tiles */
    DtInt               num_tiles_row;       /* # Tiles Per Row */
    DtInt               num_tiles_col;       /* # Tiles Per Column */
    DtInt               num_tiles_depth;     /* # Tiles Deep */
    DtInt               last_tile_width;     /* Width of Last Tile */
    DtInt               last_tile_height;    /* Height of Last Tile */
    DtInt               last_tile_depth;     /* Depth of Last Tile */
    DtReal              width_scaler;        /* Width Scale & Xlate for Tile */
    DtReal              height_scaler;       /* Height & Xlate for Tile */
    DtReal              depth_scaler;        /* Depth & Xlate for Tile */
    DtReal              last_width_scaler;   /* Scale & Xlate for Last Tile */
    DtReal              last_height_scaler;  /* Scale & Xlate for Last Tile */
    DtReal              last_depth_scaler;   /* Scale & Xlate for Last Tile */
    DtTextureAntiAliasMode aamode;           /* Raster Anti-Alias Mode Used */
    VLLtexture        **texture;             /* List of VLL Texture Pointers */
} ddt_g4vll_drom_rstr;

typedef struct {
   DtInt		*edge_enable;
   DtInt		sequence_number;
} ddt_g4vll_drom_edgelist_data;

typedef struct {
    DtFlag initialized;
    DtReal bbox[4][2];
} ddt_g4vll_drom_bboxuv;

typedef struct {
    DtFlag initialized;
    DtReal bbox[8][3];
} ddt_g4vll_drom_bboxuvw;

/* The following structures will hold additional data required by G4 VLL for 
** the various primitives.  Common fields are:
**      bboxuv and bboxuvw: the convex hulls enclosing all texture indices of
**                          the current texture, specified in uv and uvw space
**                          respectively.
**/

typedef struct {
    ddt_g4vll_drom_bboxuv  *bboxuv;
    ddt_g4vll_drom_bboxuvw *bboxuvw;
} ddt_g4vll_drom_trilst;

typedef struct {
    DtInt                       uvcount;
    DtInt                       uvwcount;
    ddt_g4vll_drom_bboxuv *     bboxuv;
    ddt_g4vll_drom_bboxuvw *    bboxuvw;
    DtInt                       sequence_number;
} ddt_g4vll_drom_vartrilist;

typedef struct {
    DtInt                  *uvvertexlist;	  /* uv conn. list for mesh */
    DtFlag  	            grid_check;		  /* msh-to-strip check done? */
    DtInt  		    tristrip_vertexcount; /* num vertices in strip */
    DtInt                  *tristrip_indices;     /* Indices describe strip */
    DtObjectStructure      *altobj_tristrip;      /* Alt object if converted */
    ddt_g4vll_drom_bboxuv  *bboxuv;
    ddt_g4vll_drom_bboxuvw *bboxuvw;
} ddt_g4vll_drom_trimsh;

typedef struct {
    DtInt *                     uvvertexlist;
    DtFlag                      grid_check;
    DtInt                       tristrip_vertexcount;
    DtInt *                     tristrip_indices;
    DtObjectStructure *         altobj_tristrip;
    DtInt                       uvcount;
    DtInt                       uvwcount;
    ddt_g4vll_drom_bboxuv *     bboxuv;
    ddt_g4vll_drom_bboxuvw *    bboxuvw;
    DtInt                       sequence_number;
} ddt_g4vll_drom_vartrimsh;

typedef struct {
    DtReal *radius;
} ddt_g4vll_drom_sphlst;

typedef struct {
    ddt_g4vll_drom_bboxuv  *bboxuv;
    ddt_g4vll_drom_bboxuvw *bboxuvw;
} ddt_g4vll_drom_tristrip;

typedef struct {
    DtInt                       uvcount;
    DtInt                       uvwcount;
    ddt_g4vll_drom_bboxuv *     bboxuv;
    ddt_g4vll_drom_bboxuvw *    bboxuvw;
    DtInt                       sequence_number;
} ddt_g4vll_drom_vartristrip;

typedef struct {
    ddt_g4vll_drom_bboxuv  *bboxuv;
    ddt_g4vll_drom_bboxuvw *bboxuvw;
} ddt_g4vll_drom_quadlst;

typedef struct {
    ddt_g4vll_drom_bboxuv  *bboxuv;
    ddt_g4vll_drom_bboxuvw *bboxuvw;
} ddt_g4vll_drom_quadgrid;

typedef struct {
    ddt_g4vll_drom_bboxuv  *bboxuv;
    ddt_g4vll_drom_bboxuvw *bboxuvw;
    DtObjectStructure 	   *altobj_quadlst;
} ddt_g4vll_drom_quadmsh;

typedef struct {
    DtObjectStructure *tristrip_altobj;
} ddt_g4vll_drom_prisrf;

/* Global object handles or additional data ptrs to get at additional data.  */

extern ddt_g4vll_drom_quadlst     *dde_g4vll_drom_quadlst;
extern ddt_g4vll_drom_quadgrid    *dde_g4vll_drom_quadgrid;
extern ddt_g4vll_drom_trilst      *dde_g4vll_drom_trilst;
extern ddt_g4vll_drom_trimsh      *dde_g4vll_drom_trimsh;
extern ddt_g4vll_drom_tristrip    *dde_g4vll_drom_tristrip;
extern ddt_g4vll_drom_vartrilist   *dde_g4vll_drom_vartrilist;
extern ddt_g4vll_drom_vartrimsh   *dde_g4vll_drom_vartrimsh;
extern ddt_g4vll_drom_vartristrip *dde_g4vll_drom_vartristrip;


/* ====================================================================== */
 
extern ddt_g4vll_drom_data dde_g4vll_drom_data;

extern ddt_g4vll_drom_device *dde_g4vll_drom_curdevdat;
extern ddt_g4vll_drom_view   *dde_g4vll_drom_curviwdat;
extern ddt_g4vll_drom_window *dde_g4vll_drom_curwindat;
extern ddt_g4vll_drom_camera *dde_g4vll_drom_curcamdat;

extern ddt_g4vll_drom_class_data dde_g4vll_drom_class_data[];

/* ====================================================================== */

#define ddd_g4vll_drom_standard_pass        1
#define ddd_g4vll_drom_texture_pass         2
#define ddd_g4vll_drom_transp_pass          3
#define ddd_g4vll_drom_transp_texture_pass  4
#define ddd_g4vll_drom_shadow_pass          5

#define ddd_g4vll_drom_rstr_user_data     0x1
#define ddd_g4vll_drom_rstr_with_z        0x2

#define ddd_g4vll_drom_texmap_none          0
#define ddd_g4vll_drom_texmap_2d            1
#define ddd_g4vll_drom_texmap_3d            2
#define ddd_g4vll_drom_texmap_env           3
#define ddd_g4vll_drom_texmap_finished      4

#define ddd_g4vll_drom_opaque_obj           0
#define ddd_g4vll_drom_mapdif_obj           1
#define ddd_g4vll_drom_mapenv_obj           2
#define ddd_g4vll_drom_transp_obj           3
#define ddd_g4vll_drom_transp_mapdif_obj    4

#define ddd_g4vll_drom_texpass_draw       0x1
#define ddd_g4vll_drom_texpass_light      0x2
#define ddd_g4vll_drom_texpass_vcol       0x4
#define ddd_g4vll_drom_texpass_spec       0x8

#define ddd_g4vll_mono_screen             -1

#define ddd_g4vll_transp_two_pass_add     0
#define ddd_g4vll_transp_single_pass      1
#define ddd_g4vll_transp_multi_pass       2
#define ddd_g4vll_transp_two_pass_compos  3

#define ddd_g4vll_pixmap         0
#define ddd_g4vll_pixmap_z       1
#define ddd_g4vll_accumulator    2
#define ddd_g4vll_accumulator_z  3

#define ddd_g4vll_cylinders_tesselated		0
#define ddd_g4vll_cylinders_firmware_capped	1
#define ddd_g4vll_cylinders_firmware_uncapped	2

/*
 * Head constants for running multi-headed Denali systems.
 */
#define ddd_g4vll_head_zero			0
#define ddd_g4vll_head_one			1


    /*****************************/
    /***  Function Prototypes  ***/
    /*****************************/

void	ddr_g4vll_drom_apply_att_clpvol  (DtClipOperator, DtInt, DtHalfSpace *);
void	ddr_g4vll_drom_bndvol_execute  (dot_object *);
DtFlag	ddr_g4vll_drom_do_vertex_color_texture_pass(DtRealTriple *, int);
void	ddr_g4vll_drom_camera  (void);
void	ddr_g4vll_drom_compute_view_parameters  (void);
DtPtr	ddr_g4vll_drom_create_local_device_data  (DtObject);
DtPtr	ddr_g4vll_drom_create_local_view_data  (DtObject);
DtPtr	ddr_g4vll_drom_create_local_window_data  (DtObject, DtObject);
void	ddr_g4vll_drom_delete_local_device_data  (DtPtr);
void	ddr_g4vll_drom_delete_local_view_data  (DtPtr);
void	ddr_g4vll_drom_delete_local_window_data  (DtPtr);
void	ddr_g4vll_drom_do_background  (void);
DtPtr	ddr_g4vll_drom_filerstr_create  (DtObject);
void	ddr_g4vll_drom_filerstr_delete  (DtObject, DtObjectStructure *);
void	ddr_g4vll_drom_filerstr_print  (DtObject, DtObjectStructure *);
void	ddr_g4vll_drom_filerstr_update  (DtObjectStructure *);
void	ddr_g4vll_drom_get_lcstowcsmat  (DtMatrix4x4);
void	ddr_g4vll_drom_get_wcstofcsmat  (DtMatrix4x4);
DtFlag	ddr_g4vll_drom_gridmesh_to_strip (
		DtInt, DtInt, DtInt[][3], DtInt*, DtInt**, DtInt*);
void	ddr_g4vll_drom_initialize_after_environment  (void);
void	ddr_g4vll_drom_initialize_before_environment  (void);
void	ddr_g4vll_drom_light  (DtObject);
void	ddr_g4vll_drom_new_class  (DtPtr, DtInt);
void	ddr_g4vll_drom_pop_att_clpvol  (void);
void	ddr_g4vll_drom_pop_lcstofcsmat  (void);
void	ddr_g4vll_drom_post_initialization  (void);
void	ddr_g4vll_drom_pre_initialization  (void);
ddt_g4vll_drom_prisrf*
	ddr_g4vll_drom_prisrf_create  (DtObjectStructure *);
dot_object*
	ddr_g4vll_drom_prisrf_create_alternate_geometry  (struct prisrf *);
dot_object*
	ddr_g4vll_drom_prisrf_create_alternate_geometry_cone (
		DtInt, DtReal, DtSwitch, DtSwitch);
dot_object*
	ddr_g4vll_drom_prisrf_create_alternate_geometry_cylinder (
		DtInt, DtReal, DtSwitch, DtSwitch);
void	ddr_g4vll_drom_prisrf_cylinder_render(dot_object *);
void	ddr_g4vll_drom_prisrf_cylinder_render_firmware();
void	ddr_g4vll_drom_prisrf_cylinder_render_software(dot_object *);
void	ddr_g4vll_drom_prisrf_delete(DtObjectStructure*,ddt_g4vll_drom_prisrf*);
void	ddr_g4vll_drom_prisrf_print(DtObjectStructure*, ddt_g4vll_drom_prisrf*);
void	ddr_g4vll_drom_prisrf_render  (dot_object *);
void	ddr_g4vll_drom_prisrf_sphere_render(dot_object *);
void	ddr_g4vll_drom_prisrf_sphere_render_firmware();
void	ddr_g4vll_drom_prisrf_sphere_render_software(dot_object *);
dot_object*
	ddr_g4vll_drom_prisrf_update_tristrip_alternate  (dot_object *);
void	ddr_g4vll_drom_push_att_clpvol  (void);
void	ddr_g4vll_drom_push_lcstofcsmat  (DtMatrix4x4);
ddt_g4vll_drom_quadgrid*
	ddr_g4vll_drom_quadgrid_create  (DtObjectStructure *);
void	ddr_g4vll_drom_quadgrid_delete  (DtObject, ddt_g4vll_drom_quadgrid *);
void	ddr_g4vll_drom_quadgrid_print  (DtObject, DtInt *);
void	ddr_g4vll_drom_quadgrid_render  (dot_object *);
ddt_g4vll_drom_quadlst*
	ddr_g4vll_drom_quadlst_create  (DtObjectStructure *);
void	ddr_g4vll_drom_quadlst_delete  (DtObject, ddt_g4vll_drom_quadlst *);
void	ddr_g4vll_drom_quadlst_print  (DtObject, DtInt *);
void	ddr_g4vll_drom_quadlst_render  (dot_object *);
ddt_g4vll_drom_quadmsh*
	ddr_g4vll_drom_quadmsh_create (DtObjectStructure *object);
dot_object*
	ddr_g4vll_drom_quadmsh_create_alternate_quadlst  (dot_object *);
void    ddr_g4vll_drom_quadmsh_delete  (DtObject, ddt_g4vll_drom_quadmsh *);
void    ddr_g4vll_drom_quadmsh_print  (DtObject, DtInt *);
void    ddr_g4vll_drom_quadmsh_render  (dot_object *);
void    ddr_g4vll_drom_query_vll_matrix (int select, DtMatrix4x4 matrix);
void	ddr_g4vll_drom_render_connected_line_list (
		DtColorModel, DtRealTriple [8], DtInt, DtRealTriple*,
		DtRealTriple*, DtRealTriple*, DtRealTriple*, DtInt,
		DtRealCouple**, DtInt, DtRealTriple**, DtReal*);
void	ddr_g4vll_drom_render_line_list (
		DtColorModel, DtRealTriple [8], DtInt, DtRealTriple *,
		DtRealTriple *, DtRealTriple *, DtRealTriple *, DtInt,
		DtRealCouple**, DtInt, DtRealTriple**, DtReal*);
int	ddr_g4vll_drom_render_pass  (DtPFI);
void	ddr_g4vll_drom_render_point_list (
		DtColorModel, DtRealTriple [8], DtInt, DtRealTriple *,
		DtRealTriple *, DtRealTriple *, DtInt, DtRealCouple**,
		DtInt, DtRealTriple**, DtReal*);
DtInt	ddr_g4vll_drom_render_prisrf_sphere  (struct prisrf *);
void	ddr_g4vll_drom_render_quad_grid  (
		DtColorModel, DtRealTriple [8], DtInt, DtInt, DtRealTriple *,
		DtRealTriple *, DtRealTriple *, DtRealTriple *, DtInt,
		DtRealCouple *[], DtInt, DtRealTriple *[], DtReal*);
void	ddr_g4vll_drom_render_quad_list (
		DtColorModel, DtRealTriple [8], DtInt, DtRealTriple *,
		DtRealTriple *, DtRealTriple *, DtRealTriple *, DtInt,
		DtRealCouple *[], DtInt, DtRealTriple *[], DtReal*);
void	ddr_g4vll_drom_render_triangle_list (
		DtColorModel, DtRealTriple [8], DtInt, DtRealTriple *,
		DtRealTriple *, DtRealTriple *, DtRealTriple *, DtInt,
		DtRealCouple *[], DtInt, DtRealTriple *[], DtReal*);
void	ddr_g4vll_drom_render_triangle_mesh (
		DtColorModel, DtRealTriple [8], DtInt, DtRealTriple *, DtInt,
		DtRealTriple *, DtRealTriple *, DtRealTriple *, DtInt [][3],
		DtInt [][3], DtInt, DtRealCouple *[], DtInt, DtRealTriple *[],
		DtReal*);
void	ddr_g4vll_drom_render_triangle_strip (
		DtColorModel, DtRealTriple [8], DtInt, DtRealTriple *,
		DtRealTriple *, DtRealTriple *, DtRealTriple *, DtInt,
		DtRealCouple *[], DtInt, DtRealTriple *[], DtReal*);
ddt_g4vll_drom_rstr*
	ddr_g4vll_drom_rstr_create  (DtObject);
void	ddr_g4vll_drom_rstr_delete  (DtObject, ddt_g4vll_drom_rstr *);
void	ddr_g4vll_drom_rstr_print  (DtObject, ddt_g4vll_drom_rstr *);
void	ddr_g4vll_drom_rstr_update  (DtObjectStructure *);
void	ddr_g4vll_drom_rstr_update_background  (DtObjectStructure *);
void	ddr_g4vll_drom_rstr_update_env  (DtObjectStructure *);
void	ddr_g4vll_drom_set_att_ambint  (DtReal);
void	ddr_g4vll_drom_set_att_ambswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_bacfacculble  (DtSwitch);
void	ddr_g4vll_drom_set_att_bacfacculswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_clpswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_clpvol  (DtClipOperator, DtInt, DtHalfSpace *);
void	ddr_g4vll_drom_set_att_depcue (
		DtReal, DtReal, DtReal, DtReal, DtColorModel, DtReal *);
void	ddr_g4vll_drom_set_att_depcueswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_difclr  (DtColorModel, DtReal []);
void	ddr_g4vll_drom_set_att_difint  (DtReal);
void	ddr_g4vll_drom_set_att_difswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_hidsrfswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_inttyp  (DtInterpType);
void	ddr_g4vll_drom_set_att_lgtatn  (DtReal, DtReal);
void	ddr_g4vll_drom_set_att_lgtclr  (DtColorModel, DtReal []);
void	ddr_g4vll_drom_set_att_lgtint  (DtReal);
void	ddr_g4vll_drom_set_att_lgtspdang  (DtReal, DtReal);
void	ddr_g4vll_drom_set_att_lgtspdexp  (DtReal);
void	ddr_g4vll_drom_set_att_lgtswi  (DtInt, DtObject *);
void	ddr_g4vll_drom_set_att_lgttyp  (DtObject);
void	ddr_g4vll_drom_set_att_lintyp  (DtLineType);
void	ddr_g4vll_drom_set_att_linwid  (DtReal);
void	ddr_g4vll_drom_set_att_localaasty  (DtLocalAntiAliasStyle);
void	ddr_g4vll_drom_set_att_localaaswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_mapbmp (
		DtInt, DtObject [], DtObject [], dot_stdtexatt []);
void	ddr_g4vll_drom_set_att_mapbmpswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_mapdifclr (
		DtInt, DtObject [], DtObjectStructure *[], dot_stdtexatt []);
void	ddr_g4vll_drom_set_att_mapdifclrswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_mapenv  (
		DtInt, DtObject [], DtObjectStructure *[], dot_stdtexatt []);
void	ddr_g4vll_drom_set_att_mapenvswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_maptrnint  (
		DtInt, DtObject [], DtObject [], dot_stdtexatt []);
void	ddr_g4vll_drom_set_att_maptrnintswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_refswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_reptyp  (DtRepType);
void	ddr_g4vll_drom_set_att_shaswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_shdidx  (DtInt);
void	ddr_g4vll_drom_set_att_smpadapt  (DtReal);
void	ddr_g4vll_drom_set_att_smpadaptswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_smpfilter  (DtObject, DtReal, DtReal);
void	ddr_g4vll_drom_set_att_smpjitter  (DtReal);
void	ddr_g4vll_drom_set_att_smpjitterswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_smpsuper  (DtInt, DtInt);
void	ddr_g4vll_drom_set_att_smpsuperswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_spcclr  (DtColorModel, DtReal []);
void	ddr_g4vll_drom_set_att_spcfct  (DtReal);
void	ddr_g4vll_drom_set_att_spcint  (DtReal);
void	ddr_g4vll_drom_set_att_spcswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_srfedgclr  (DtColorModel, DtReal []);
void	ddr_g4vll_drom_set_att_srfshd  (DtObject);
void	ddr_g4vll_drom_set_att_stereo  (DtReal, DtReal);
void	ddr_g4vll_drom_set_att_stereoswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_transpclr  (DtColorModel, DtReal []);
void	ddr_g4vll_drom_set_att_transpint  (DtReal);
void	ddr_g4vll_drom_set_att_transporientclr  (DtColorModel, DtReal []);
void	ddr_g4vll_drom_set_att_transporientexp  (DtReal);
void	ddr_g4vll_drom_set_att_transporientint  (DtReal);
void	ddr_g4vll_drom_set_att_transporientswi  (DtSwitch);
void	ddr_g4vll_drom_set_att_transpswi  (DtSwitch);
void	ddr_g4vll_drom_set_camera_matrix  (DtMatrix4x4);
void	ddr_g4vll_drom_set_parallel_matrix  (DtReal, DtReal, DtReal);
void	ddr_g4vll_drom_set_perspective_matrix  (DtReal, DtReal, DtReal);
void	ddr_g4vll_drom_set_prim_execute  (void);
void	ddr_g4vll_drom_set_projection_matrix (
		DtArea *, DtProjectionType, DtPoint3, DtReal, DtReal, DtReal);
void    ddr_g4vll_drom_set_vll_matrix (int select, DtMatrix4x4 matrix);
ddt_g4vll_drom_sphlst*
	ddr_g4vll_drom_sphlst_create (DtObjectStructure*);
void	ddr_g4vll_drom_sphlst_delete(DtObjectStructure*,ddt_g4vll_drom_sphlst*);
void	ddr_g4vll_drom_sphlst_print (DtObjectStructure*,ddt_g4vll_drom_sphlst*);
void	ddr_g4vll_drom_sphlst_render  (DtObjectStructure *);
void	ddr_g4vll_drom_sphlst_render_firmware  (struct sphlst *, ddt_g4vll_drom_sphlst *);
void  	ddr_g4vll_drom_sphlst_render_software  (DtObjectStructure *);

void	ddr_g4vll_drom_start_update (DtObject, DtObject, DtPtr, DtPtr, DtPtr);
DtFlag	ddr_g4vll_drom_transform_clip_z_point  (DtRealTriple, DtMatrix4x4);
ddt_g4vll_drom_trilst*
	ddr_g4vll_drom_trilst_create  (DtObjectStructure *);
void	ddr_g4vll_drom_trilst_delete  (DtObject, ddt_g4vll_drom_trilst *);
void	ddr_g4vll_drom_trilst_print  (DtObject, DtInt *);
void	ddr_g4vll_drom_trilst_render  (dot_object *);
ddt_g4vll_drom_trimsh*
	ddr_g4vll_drom_trimsh_create  (DtObjectStructure *);
dot_object*
	ddr_g4vll_drom_trimsh_create_alternate_tristrip  (dot_object *);
void	ddr_g4vll_drom_trimsh_delete  (DtObject, ddt_g4vll_drom_trimsh *);
void	ddr_g4vll_drom_trimsh_print  (DtObject, DtInt *);
void	ddr_g4vll_drom_trimsh_render  (dot_object *);
ddt_g4vll_drom_tristrip*
	ddr_g4vll_drom_tristrip_create  (DtObjectStructure *);
void	ddr_g4vll_drom_tristrip_delete  (DtObject, ddt_g4vll_drom_tristrip *);
void	ddr_g4vll_drom_tristrip_print  (DtObject, DtInt *);
void	ddr_g4vll_drom_tristrip_render  (dot_object *);
void	ddr_g4vll_drom_update_clpvol  (dot_modclpvol *);
void	ddr_g4vll_drom_update_display  (DtPFI);
void	ddr_g4vll_drom_update_lcstowcsmat_lokatfrm (
		DtPoint3, DtPoint3, DtVector3, DtCompType);
void	ddr_g4vll_drom_update_lcstowcsmat_pop  (void);
void	ddr_g4vll_drom_update_lcstowcsmat_push  (void);
void	ddr_g4vll_drom_update_lcstowcsmat_rotate  (DtAxis, DtReal, DtCompType);
void	ddr_g4vll_drom_update_lcstowcsmat_scale (
		DtReal, DtReal, DtReal, DtCompType);
void	ddr_g4vll_drom_update_lcstowcsmat_shear (
		DtMajorPlane, DtReal, DtReal, DtCompType);
void	ddr_g4vll_drom_update_lcstowcsmat_tfmmat (DtMatrix4x4, DtCompType);
void	ddr_g4vll_drom_update_lcstowcsmat_transl (
		DtReal, DtReal, DtReal, DtCompType);
void	ddr_g4vll_drom_update_local_data  (void);
void	ddr_g4vll_drom_update_matrices  (float[4][4], float[4][4], DtCompType);
void	ddr_g4vll_drom_update_studio  (DtPFI);
void	ddr_g4vll_drom_update_surface_properties  (void);
ddt_g4vll_drom_vartrimsh*
	ddr_g4vll_drom_vartrimsh_create  (DtObjectStructure *);
dot_object*
	ddr_g4vll_drom_vartrimsh_create_alternate_tristrip  (dot_object *);
void	ddr_g4vll_drom_vartrimsh_delete  (DtObject, ddt_g4vll_drom_vartrimsh *);
void	ddr_g4vll_drom_vartrimsh_print  (DtObject, DtInt *);
void	ddr_g4vll_drom_vartrimsh_render  (dot_object *);
ddt_g4vll_drom_vartrilist*
	ddr_g4vll_drom_vartrilist_create  (DtObjectStructure *);
dot_object*
	ddr_g4vll_drom_vartrilist_create_alternate_tristrip  (dot_object *);
void	ddr_g4vll_drom_vartrilist_delete  (DtObject, ddt_g4vll_drom_vartrilist *);
void	ddr_g4vll_drom_vartrilist_print  (DtObject, DtInt *);
void	ddr_g4vll_drom_vartrilist_render  (dot_object *);
ddt_g4vll_drom_vartristrip*
	ddr_g4vll_drom_vartristrip_create  (DtObjectStructure *);
void	ddr_g4vll_drom_vartristrip_delete(DtObject,ddt_g4vll_drom_vartristrip*);
void	ddr_g4vll_drom_vartristrip_print  (DtObject, DtInt *);
void	ddr_g4vll_drom_vartristrip_render  (dot_object *);
DtPtr	ddr_g4vll_return_DROM_fcns  (void);
DtPtr	ddr_g4vll_return_DROM_methods  (void);

#endif
