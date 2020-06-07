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

#ifndef GLBRND_H
#define GLBRND_H
 
#include <dore/internal/dogen.h>
#include <dore/internal/list.h>
#include <dore/dore_develop/develop.h>
#include <dore/dore_develop/private/device.h>
#include <dore/dore_develop/private/rayintsph.h>


    /***************************/
    /***  Defined Constants  ***/
    /***************************/

#define dod_glbrnd_fp_max_exp                126
#define dod_glbrnd_fp_min_exp                -126

#define dod_glbrnd_viewtype_perspective      0
#define dod_glbrnd_viewtype_parallel         1
#define dod_glbrnd_viewtype_matrix           2

#define dod_glbrnd_window_completely_obscured		1
#define dod_glbrnd_window_completely_visible		2
#define dod_glbrnd_window_partially_obscured		3


    /*************************/
    /***  Data Structures  ***/
    /*************************/

struct rayint_object_modifier {
	DtPtr shade_data;
	DtMatrix4x4 *lcstowcsmat;
	DtMatrix4x4 *wcstolcsmat;
	};

struct rayint_object {
	struct rayint_object_modifier *modifier;
	dot_object *object;
	};

struct ray {
	DtPtr current_box;
	DtReal wcsorigin[3];
	DtReal wcsdirection[3];
	DtReal wcsslope[3];
	};

struct intersect {
	struct rayint_object *rayint_object;
	DtReal t;
	DtReal lcsloc[3];
	DtFlag lcsnrm_available;
	DtReal lcsnrm[3];
	DtReal wcsloc[3];
	};

struct glbrnd_data {
	dot_object	*current_device;
	dot_object	*current_view;

	DtInt		object_shade_index;

	DtSwitch	pseudocolor_switch;
	DtSwitch 	draw_switch;

	DtSwitch       remote_render_switch;

	DtMatrix4x4 	projmat;
	DtInt 		viewtype;

	DtInt		studio_method;
	DtInt 		display_method;
	DtInt		SpcboxOvr_method;
	DtInt		Rayint_method;
	DtInt		UsrDat_method;
	DtInt		Wcsloc_method;
	DtInt		Wcsnrm_method;
	DtInt		WldBnd_method;

	DtInt		device_index;
	DtInt		view_index;
	};

struct glbrnd_cliprec {
	DtReal xmin;
	DtReal ymin;
	DtReal xmax;
	DtReal ymax;
	};

struct glbrnd_device_data {
	DtVisualType visual_type;
	DtShadeMode shade_mode;
	DtInt expansion_board;
	DtReal current_shade_range_min;
	DtReal current_shade_range_max;
	DtReal ctodscale[3];
	DtReal ctodtranslate_0[3];
	DtReal ctodtranslate_1[3];
	DtInt modification_sequence_number;
	};

struct glbrnd_view_data {
	DtFlag clear_flag;
	DtColorModel background_color_model;
	DtColorRGB background_color;
	struct glbrnd_camera_data *camera_data;
	struct glbrnd_light_data *light_data;
	DtInt window_devicecount;
	dot_object **window_device;
	struct glbrnd_window_data **window_data;
	DtInt modification_sequence_number;
	};

struct glbrnd_camera_data {
	DtMatrix4x4 projmat;
	DtMatrix4x4 ccstowcsmat;
	DtMatrix4x4 wcstoccsmat;
	DtMatrix4x4 npctowcsmat;
	DtMatrix4x4 wcstofcsmat;
	DtInt camtype;
	DtReal wcscamloc[3];
	DtReal wcscamnorm[3];
	DtInt viewtype;
	DtMatrix4x4 lcstofcsmat;
	DtMatrix4x4 lcstofcsmat_temp;
	};

struct glbrnd_light_data {
	DtColorRGB ambient_reflectance;
	DtInt count;
	DtReal cosine[10][3];
	DtReal intens[10];
	DtColorRGB color[10];
	DtReal h[10][3];
	DtReal lcscosine[10][3];
	DtColorRGB scaledcolor[10];
	};

struct glbrnd_window_data {
	DtInt clipping_rectangle_count;
	struct glbrnd_cliprec *clipping_rectangles;
	DtInt visibility_state;
	};


typedef struct {
    char        *name;
    DtMethodPtr  routine;
} drt_glbrnd_methods;


    /*************************************/
    /***  Global Variables/Structures  ***/
    /*************************************/

extern struct glbrnd_data dre_glbrnd_data;

extern struct glbrnd_device_data *dre_glbrnd_curdevdat;
extern struct glbrnd_view_data *dre_glbrnd_curviwdat;
extern struct glbrnd_window_data *dre_glbrnd_curwindat;
extern struct glbrnd_light_data *dre_glbrnd_curlgtdat;
extern struct glbrnd_camera_data *dre_glbrnd_curcamdat;

extern DDt_PROM *dre_glbrnd_PROM_fcns; 

extern DtFlag dre_glbrnd_initialize_studio;

extern drt_glbrnd_methods dre_glbrnd_studio_method[];
extern drt_glbrnd_methods dre_glbrnd_display_method[];
extern drt_glbrnd_methods dre_glbrnd_SpcboxOvr_method[];
extern drt_glbrnd_methods dre_glbrnd_Rayint_method[];
extern drt_glbrnd_methods dre_glbrnd_UsrDat_method[];
extern drt_glbrnd_methods dre_glbrnd_Wcsloc_method[];
extern drt_glbrnd_methods dre_glbrnd_Wcsnrm_method[];
extern drt_glbrnd_methods dre_glbrnd_WldBnd_method[];


    /*****************************/
    /***  Function Prototypes  ***/
    /*****************************/

struct rayintmshtri;
struct bsp_instance;
struct rayinttri;
struct prisrf;
struct rayint_box;
struct rayint_face_node;

void    drr_glbrnd_camera_initialize  (dot_object *);
void    drr_glbrnd_cammat_initialize  (dot_object *);
void    drr_glbrnd_compute_wcstofcs  (void);
struct glbrnd_device_data*
	drr_glbrnd_create_device_data  (void);
struct glbrnd_view_data*
	drr_glbrnd_create_view_data  (void);
void    drr_glbrnd_delete_device_data  (DtObject, struct glbrnd_device_data *);
void    drr_glbrnd_delete_view_data  (DtObject, struct glbrnd_view_data *);
void    drr_glbrnd_get_wcstofcsmat  (DtMatrix4x4);
void    drr_glbrnd_initialize_after_environment  (void);
void    drr_glbrnd_initialize_before_environment  (void);
void    drr_glbrnd_initialize_ccstowcsmat  (void);
void    drr_glbrnd_initialize_environment  (void);
void    drr_glbrnd_initialize_object  (dot_object *);
void    drr_glbrnd_initialize_object_modifier  (DtFlag);
void    drr_glbrnd_light_initialize  (dot_object *);
void    drr_glbrnd_new_class  (char *, DtInt);
void    drr_glbrnd_parallel_initialize  (dot_object *);
void    drr_glbrnd_perspective_initialize  (dot_object *);
void    drr_glbrnd_ply_initialize_objects  (dot_object *);
void    drr_glbrnd_plymsh_initialize_objects  (dot_object *);
void    drr_glbrnd_projection_initialize  (dot_object *);
void    drr_glbrnd_quadlst_initialize_objects  (dot_object *);
void    drr_glbrnd_quadmsh_initialize_objects  (dot_object *);
void    drr_glbrnd_rayintmshtri_bounding_box (
		struct rayintmshtri*, DtReal*, DtReal*, DtReal*, DtReal*,
		DtReal *, DtReal *);
DtFlag  drr_glbrnd_rayintmshtri_intersect_box_triangle (
		DtReal[6], struct rayintmshtri *);
DtFlag  drr_glbrnd_rayintmshtri_intersect_edge_box (
		DtReal,DtReal,DtReal,DtReal,DtReal,DtReal,DtReal, DtReal[6]);
DtFlag  drr_glbrnd_rayintmshtri_intersect_edge_triangle (
		DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal,
		struct rayintmshtri *);
DtFlag  drr_glbrnd_rayintmshtri_intersect_triangle_box (
		struct rayintmshtri *, DtReal[6]);
DtFlag  drr_glbrnd_rayintmshtri_overlap_spacebox (
		dot_object *, DtMatrix4x4 *, DtReal[6], struct bsp_instance *);
DtFlag  drr_glbrnd_rayintmshtri_rayint (
		dot_object *, struct ray *, struct intersect *);
DtFlag  drr_glbrnd_rayintmshtri_rayintersect_containment_test (
		struct rayintmshtri *, DtReal[3]);
DtFlag  drr_glbrnd_rayintmshtri_userdata (
		dot_object *, struct ray *, struct intersect *, DtReal[3]);
DtFlag  drr_glbrnd_rayintmshtri_wcsloc (
		dot_object *, struct ray *, struct intersect *, DtReal[3]);
DtFlag  drr_glbrnd_rayintmshtri_wcsnrm (
		dot_object *, struct ray *, struct intersect *, DtReal[3]);
void    drr_glbrnd_rayintmshtri_world_boundary (
		dot_object*, DtMatrix4x4*, DtReal*, DtReal*, DtReal*, DtReal*,
		DtReal*, DtReal*);
void    drr_glbrnd_rayintsph_bounding_box (
		struct rayintsph*, DtReal*, DtReal*, DtReal*, DtReal*, DtReal*,
		DtReal*);
DtFlag  drr_glbrnd_rayintsph_intersect_box (
		DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal,
		DtReal, DtReal, DtReal, DtReal);
DtFlag  drr_glbrnd_rayintsph_intersect_edge_box (
		DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal,
		DtReal, DtReal, DtReal, DtReal);
DtFlag  drr_glbrnd_rayintsph_overlap_spacebox (
		dot_object*, DtMatrix4x4*, DtReal[6]);
DtFlag  drr_glbrnd_rayintsph_rayint (
		dot_object *, struct ray *, struct intersect *);
DtFlag  drr_glbrnd_rayintsph_userdata (
		dot_object *, struct ray *, struct intersect *, DtReal[3]);
DtFlag  drr_glbrnd_rayintsph_wcsloc (
		dot_object *, struct ray *, struct intersect *, DtReal[3]);
DtFlag  drr_glbrnd_rayintsph_wcsnrm (
		dot_object *, struct ray *, struct intersect *, DtReal[3]);
void    drr_glbrnd_rayintsph_world_boundary (
		dot_object *, DtMatrix4x4, DtReal *, DtReal *, DtReal *,
		DtReal *, DtReal *, DtReal *);
void    drr_glbrnd_rayinttri_bounding_box (
		struct rayinttri *, DtReal *, DtReal *, DtReal *, DtReal *,
		DtReal *, DtReal *);
DtFlag  drr_glbrnd_rayinttri_intersect_edge_box (
		DtReal,DtReal,DtReal,DtReal,DtReal,DtReal,DtReal, DtReal[6]);
DtFlag  drr_glbrnd_rayinttri_intersect_edge_triangle (
		DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal,
		struct rayinttri *);
DtFlag  drr_glbrnd_rayinttri_intersect_spacebox_triangle (
		DtReal[6], struct rayinttri *);
DtFlag  drr_glbrnd_rayinttri_intersect_triangle_spacebox (
		struct rayinttri *, DtReal[6]);
DtFlag  drr_glbrnd_rayinttri_overlap_spacebox (
		dot_object *, DtMatrix4x4 *, DtReal[6]);
DtFlag  drr_glbrnd_rayinttri_rayint (
		dot_object *, struct ray *, struct intersect *);
DtFlag  drr_glbrnd_rayinttri_rayintersect_containment_test (
		struct rayinttri *, DtReal, DtReal, DtReal);
DtFlag  drr_glbrnd_rayinttri_userdata (
		dot_object *, struct ray *, struct intersect *, DtReal[3]);
DtFlag  drr_glbrnd_rayinttri_wcsloc (
		dot_object *, struct ray *, struct intersect *, DtReal[3]);
DtFlag  drr_glbrnd_rayinttri_wcsnrm (
		dot_object *, struct ray *, struct intersect *, DtReal[3]);
void    drr_glbrnd_rayinttri_world_boundary (
		dot_object *, DtMatrix4x4 *, DtReal *, DtReal *, DtReal *,
		DtReal *, DtReal *, DtReal *);
void    drr_glbrnd_render  (dot_object *, dot_object *, DtFlag, DtFlag);
void    drr_glbrnd_set_camera_matrix  (DtMatrix4x4);
void    drr_glbrnd_set_parallel_matrix  (DtReal, DtReal, DtReal);
void    drr_glbrnd_set_perspective_matrix  (DtReal, DtReal, DtReal);
void    drr_glbrnd_set_projection_matrix (
		DtArea *, DtProjectionType, DtPoint3, DtReal, DtReal, DtReal);
void    drr_glbrnd_simply_initialize_objects  (dot_object *);
void    drr_glbrnd_simplymsh_initialize_objects  (dot_object *);
void    drr_glbrnd_sphlst_initialize_objects  (dot_object *);
void    drr_glbrnd_text_initialize_objects  (dot_object *);
void    drr_glbrnd_trilst_initialize_objects  (dot_object *);
void    drr_glbrnd_trimsh_initialize_objects  (dot_object *);
void    drr_glbrnd_tristrip_initialize_objects  (dot_object *);
void    drr_glbrnd_update_device_and_view  (dot_object *, dot_object *);
void    drr_glbrnd_varsimplymsh_initialize_objects  (dot_object *);
void    drr_glbrnd_vartrimsh_initialize_objects  (dot_object *);
void    drr_glbrnd_vartristrip_initialize_objects  (dot_object *);
DtInt   drr_glbshd_cast_shadow  (DtReal[3], DtColorRGB);
struct glbshd_object_data*
	drr_glbshd_create_object_data  (void);
void    drr_glbshd_h_normal  (DtReal[3], DtReal[3], DtReal[3]);
void    drr_glbshd_initialize_after_environment  (void);
void    drr_glbshd_initialize_before_environment  (void);
void    drr_glbshd_initialize_camera  (void);
void    drr_glbshd_initialize_light  (dot_object *);
void    drr_glbshd_initialize_render  (DtColorModel, DtColorRGB);
void    drr_glbshd_initialize_subsystem  (void);
DtFlag  drr_glbrnd_prisrf_rayint_box (
		struct prisrf *, struct ray *, struct intersect *);
void    drr_glbrnd_prisrf_world_boundary_box (
		DtMatrix4x4, DtReal*, DtReal*, DtReal*, DtReal*, DtReal*,
		DtReal *);
DtFlag  drr_glbrnd_prisrf_overlap_spacebox_box (DtMatrix4x4, DtReal [6]);
DtFlag  drr_glbrnd_prisrf_rayint_cone (
		struct prisrf *, struct ray *, struct intersect *);
void     drr_glbrnd_prisrf_world_boundary_cone (
		DtMatrix4x4, DtReal *, DtReal *, DtReal *, DtReal *, DtReal *,
		DtReal *);
DtFlag  drr_glbrnd_prisrf_overlap_spacebox_cone (DtMatrix4x4, DtReal [6]);
DtFlag  drr_glbrnd_prisrf_rayint_cylinder (
		struct prisrf *, struct ray *, struct intersect *);
void    drr_glbrnd_prisrf_world_boundary_cylinder (
		DtMatrix4x4, DtReal *, DtReal *, DtReal *, DtReal *,
		DtReal *, DtReal *);
DtFlag  drr_glbrnd_prisrf_overlap_spacebox_cylinder  (DtMatrix4x4, DtReal [6]);
void    drr_glbrnd_prisrf_initialize_objects  (dot_object *);
DtFlag  drr_glbrnd_prisrf_rayint  (dot_object*, struct ray*, struct intersect*);
void    drr_glbrnd_prisrf_world_boundary (
		dot_object *, DtMatrix4x4, DtReal *, DtReal *, DtReal *,
		DtReal *, DtReal *, DtReal *);
DtFlag  drr_glbrnd_prisrf_overlap_spacebox (
		dot_object *, DtMatrix4x4 *, DtReal [6]);
DtFlag  drr_glbrnd_prisrf_userdata (
		dot_object *, struct ray *, struct intersect *, DtReal [3]);
DtFlag  drr_glbrnd_prisrf_wcsloc (
		dot_object *, struct ray *, struct intersect *, DtReal [3]);
DtFlag  drr_glbrnd_prisrf_wcsnrm (
		dot_object *, struct ray *, struct intersect *, DtReal [3]);
void    drr_glbrnd_prisrf_bound_lcsbox (
		DtMatrix4x4, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal,
		DtReal *, DtReal *, DtReal *, DtReal *, DtReal *, DtReal *);
void    drr_glbrnd_prisrf_calculate_box_limits (
		DtMatrix4x4, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal,
		DtReal *, DtReal *, DtReal *, DtReal *, DtReal *, DtReal *);
DtFlag  drr_glbrnd_prisrf_intersect_edge_box (
		DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal,
		DtReal, DtReal, DtReal, DtReal, DtReal, DtReal);
DtFlag  drr_glbrnd_prisrf_intersect_lcsbox_box (
		DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtMatrix4x4,
		DtReal, DtReal, DtReal, DtReal, DtReal, DtReal);
DtFlag  drr_glbrnd_prisrf_intersect_lcsedge_box (
		DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtMatrix4x4,
		DtReal, DtReal, DtReal, DtReal, DtReal, DtReal);
DtFlag  drr_glbrnd_prisrf_overlap_lcsbox_spacebox (DtReal, DtReal, DtReal,
		DtReal, DtReal, DtReal, DtMatrix4x4, DtReal [6]);
DtFlag  drr_glbrnd_prisrf_rayint_sphere (struct prisrf *, struct ray *,
		struct intersect *);
void    drr_glbrnd_prisrf_world_boundary_sphere (DtMatrix4x4, DtReal *,
		DtReal *, DtReal *, DtReal *, DtReal *, DtReal *);
DtFlag  drr_glbrnd_prisrf_overlap_spacebox_sphere (DtMatrix4x4, DtReal [6]);
void    drr_glbshd_reflection_component  (DtColorRGB);
void    drr_glbshd_shade  (DtColorRGB);
void    drr_glbshd_shade_light  (DtInt, DtColorRGB);
void    drr_glbshd_shade_raytraced (
		struct rayint_object *, struct ray *, struct intersect *,
		DtColorRGB);
DtFlag  drr_glbshd_transmit_shadow (
		struct ray *, struct intersect *, DtReal[3]);
void    drr_glbshd_transmitted_component  (DtColorRGB);
void    drr_rayint_add_statistics_box  (struct rayint_box *);
void    drr_rayint_add_statistics_rayint_object  (void);
void    drr_rayint_box_ray_intersect (
		struct rayint_box *, DtReal[3], DtReal[3], DtReal[3],
		DtInt, DtInt *, DtReal[3]);
void    drr_rayint_calculate_box_boundary  (DtReal[3], DtReal[3]);
DtFlag  drr_rayint_calculate_object_intersect (
		struct rayint_box *, struct list *, struct ray *, DtFlag,
		struct intersect *);
void    drr_rayint_change_adjacent_boxes (
		struct rayint_face_node *, DtInt, DtInt, struct rayint_box *,
		struct rayint_box *);
DtInt   drr_rayint_classify_ray (DtReal[3]);
struct rayint_box*
	drr_rayint_create_box  (DtReal, DtReal, DtReal, DtReal);
struct rayint_bsptree_node*
	drr_rayint_create_bsptree_node  (DtReal[3], DtPtr[8], DtInt);
struct rayint_face_node*
	drr_rayint_create_face_node (
		DtReal, DtReal, struct rayint_box *, struct rayint_box *,
		struct rayint_box *, struct rayint_box *);
void    drr_rayint_delete_face_node  (struct rayint_face_node *);
void    drr_rayint_destroy_box  (struct rayint_box *);
struct rayint_box*
	drr_rayint_get_box  (DtReal[3]);
void    drr_rayint_get_face_node (
		struct rayint_face_node *, struct rayint_box *,
		struct rayint_face_node **, DtInt *);
struct rayint_box*
	drr_rayint_get_first_box  (DtReal[3], DtReal[3], DtReal[3]);
struct rayint_box*
	drr_rayint_get_next_box  (struct rayint_box *, DtInt, DtReal[3]);
void    drr_rayint_initialize_camera  (dot_object *);
void    drr_rayint_initialize_object  (dot_object *);
void    drr_rayint_initialize_object_list  (void);
void    drr_rayint_initialize_object_modifier  (DtFlag);
void    drr_rayint_initialize_render  (void);
void    drr_rayint_initialize_statistics  (void);
void    drr_rayint_initialize_subsystem  (void);
void    drr_rayint_initialize_tree  (void);
DtFlag  drr_rayint_intersect  (struct ray *, DtFlag, struct intersect *);
DtFlag  drr_rayint_intersect_ray_space (
		DtReal[3], DtReal[3], DtReal[3], DtReal[3]);
void    drr_rayint_link_faces (
		struct rayint_box *, struct rayint_box *[8], DtReal[3]);
void    drr_rayint_print_run_statistics  (void);
void    drr_rayint_print_tree_statistics  (void);
void    drr_rayint_set_bit  (DtInt *, DtInt, DtInt);
void    drr_rayint_set_box_face_adjacency (
		struct rayint_box *, struct rayint_box *, struct rayint_box *,
		struct rayint_box *, struct rayint_box *, struct rayint_box *,
		struct rayint_box *);
void    drr_rayint_subdivide_box (
		struct rayint_bsptree_node *, DtInt, struct list *);
void    drr_rayint_subdivide_face (
		struct rayint_box *, struct rayint_box *, struct rayint_box *,
		struct rayint_box *, struct rayint_box *, DtInt, DtReal,DtReal);
void    drr_recrnd_initialize_device_and_view  (void);
void    drr_recrnd_initialize_subsystem  (void);
void    drr_recrnd_render  (void);
DtFlag  drr_recrnd_render_ray  (struct ray *, DtReal *, DtReal *, DtReal *);
void    drr_recrnd_render_rectangle (
		DtInt, DtInt, DtInt, DtInt, DtReal *, DtReal *, DtReal *);
void    drr_recrnd_render_scanline_segment (
		DtInt, DtInt, DtInt, DtReal[], DtReal[], DtReal[]);
void    drr_recrnd_render_screen_point (
		DtReal, DtReal, DtReal *, DtReal *, DtReal *);

#endif
