#ifndef DORE_RAD_H
#define DORE_RAD_H
 
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
 
#include <math.h>
#include <dore/dore.h>
#include <dore/internal/dogen.h> 
#include <dore/internal/system.h> 
#include <dore/internal/gpolyd.h> 

#include <dore/dore_develop/private/simplymsh.h>
#include <dore/dore_develop/private/varsplymsh.h>

#define NOT_INSTALLED		-1
#define MAXIMUM_COLOR		16777214  /* this is 2^24 - 2 */
#define MAX_PARAM_SIZE		128
#define HITHER_OVER_YON         0.00005
#define MAX_LOOP                15
#define ENERGY_EPSILON		1e-10

/* ====================================================================== */

#define DOT_PRODUCT(a,b) ((a)[0]*(b)[0] + (a)[1]*(b)[1] + (a)[2]*(b)[2])

#define CROSS_VEC(a,b,c) ((c)[0] = (a)[1]*(b)[2] - (a)[2]*(b)[1], \
                          (c)[1] = (a)[2]*(b)[0] - (a)[0]*(b)[2], \
                          (c)[2] = (a)[0]*(b)[1] - (a)[1]*(b)[0])

#define LENGTH_VEC(a,l)  ((l)=sqrt(DOT_PRODUCT(a,a)))

#define NORMALIZE_VEC(a,n)   ((n)=sqrt(DOT_PRODUCT(a,a)), \
                             (n)?((a)[0]/=n, (a)[1]/=n, (a)[2]/=n):0)

#define ADD_VEC(a,b,c)   ((c)[0] = (a)[0]+(b)[0], (c)[1] = (a)[1]+(b)[1], \
                          (c)[2] = (a)[2]+(b)[2])

#define SUB_VEC(a,b,c)   ((c)[0] = (a)[0]-(b)[0], (c)[1] = (a)[1]-(b)[1], \
                          (c)[2] = (a)[2]-(b)[2])

/* ====================================================================== */

typedef struct {
    DtInt       renderer_method_id;
    DtPFI       renderer_reset_method_id_fcn;

    DtInt       del_datalsts_method_id;

    DtInt       hcube_setup_method_id;
    DtInt       hcube_render_method_id; 
    DtInt       resolution;
    DtObject    device;
    DtObject    view;
    DtObject    frame;
    DtObject    display_group;
    DtObject    studio_group;
    DtObject    cameras[5];
    DtInt       next_integer_color;

    DtInt       hcube_distrib_method_id;
    DtRadiosityConvType   conv_type; 
    DtReal      conv_parms[MAX_PARAM_SIZE];

    DtInt       hcube_select_method_id;
} dpt_rad_hcube_data;


typedef struct {
    DtInt       renderer_method_id;
    DtPFI       renderer_reset_method_id_fcn;
    DtObject    device;
    DtObject    view;
    DtObject    active_camera;

    DtInt       find_dsptyp_method_id; 
    DtIntermediateDispType   interm_dsptyp; 
    DtInt       dsptyp_parms[MAX_PARAM_SIZE];

    DtInt       show_render_method_id; 
} dpt_rad_show_data;

typedef struct {
    DtPtr       name;
    DtInt       id;
    DtMethodPtr crt_rtn;
    DtMethodPtr prt_rtn;
    DtMethodPtr del_rtn;
    DtInt       index;
} dpt_rad_additional_data;

typedef struct {
    char        *name;
    DtMethodPtr  routine;
} dpt_rad_method_rtn_list;

typedef struct primitive_data {
    DtInt		   usageCount;
    DtInt		   usageCur;
    DtRealTriple	   objectReflectance; /* initialized to diffclr */
    DtRealTriple          *patchRad;
    DtRealTriple          *patchUnshotRad;
    DtInt	          *patchIntElemColor; 
    DtRealTriple          *patchElemColor; 
    DtReal	          *patchArea;
    DtRealTriple          *vertexRad;
    struct primitive_data *next;
} dpt_rad_primitive_data;

typedef struct {
    DtInt	  intElemColor;
    DtReal	  energy;
    DtInt	  nVert;
    DtRealTriple *vertices;
    DtMatrix4x4	  lcswcs;	
    DtReal	  area; 
    DtReal	 *unshotRadPtr; 
} dpt_rad_brightest_patch;

typedef struct {
    DtReal	  total;
    DtReal	  total_unshot;
    DtRealTriple  total_unshot_per_channel;
} dpt_rad_energy;

typedef struct {
    DtRealTriple  component;		
    DtReal	  sum_area;		
    DtRealTriple  sum_reflecXarea;
} dpt_rad_ambient;

extern dpt_rad_hcube_data         dpe_rad_hcube_data;
extern dpt_rad_show_data          dpe_rad_show_data;
extern dpt_rad_brightest_patch    dpe_rad_brightest_patch;

extern DtFlag  dpe_rad_radiosity_installed;
extern DtInt   dpe_rad_base_method_id;
extern DtInt   dpe_rad_iteration_number;
extern DtRadiosityUpdateType  dpe_rnd_updtype;

extern dpt_rad_ambient    dpe_rad_ambient;
extern dpt_rad_energy     dpe_rad_energy;
extern DtInt              dpe_rad_int_backg_col;

extern DtFlag dpe_rad_converged;
extern DtInt  dpe_rad_pixel_offset;


    /*****************************/
    /***  Function Prototypes  ***/
    /*****************************/

void   dor_radiosity_count_complex_simplymsh_triangle  (void);
void   dor_radiosity_count_complex_varsimplymsh_triangle  (void);
void   dor_radiosity_count_simple_simplymsh_triangle  (void);
void   dor_radiosity_count_simple_varsimplymsh_triangle  (void);
void   dor_radiosity_simplymsh_get_tris_per_polygon (struct simplymsh*, DtInt*);
void   dpr_radiosity_swap_check  (void);
void   dor_radiosity_varsimplymsh_get_tris_per_polygon  (
	    struct varsimplymsh *, DtInt *);
void   dpr_radiosity_callback_show_render  (dot_object *);
void   dpr_radiosity_camera_find_dsptyp  (dot_object *);
void   dpr_radiosity_change_seed (unsigned);
void   dpr_radiosity_clean_up (void);
void   dpr_radiosity_color_int_to_normvec (DtInt, DtReal[3]);
void   dpr_radiosity_compute_bounding_volume (DtVolume*, DtObject);
void   dpr_radiosity_compute_center_and_normal (
	    DtRealTriple[], DtInt, DtRealTriple, DtRealTriple);
void   dpr_radiosity_compute_iteration (void);
void   dpr_radiosity_compute_next_color (DtInt*, DtReal[3]);
DtReal dpr_radiosity_compute_polygon_area (DtRealTriple[], DtInt);
void   dpr_radiosity_compute_side_delta_form_factors  (DtInt);
void   dpr_radiosity_compute_top_delta_form_factors  (DtInt);
DtReal dpr_radiosity_compute_triangle_area (
	    DtRealTriple, DtRealTriple, DtRealTriple);
void   dpr_radiosity_copy_matrix  (DtMatrix4x4, DtMatrix4x4);
void   dpr_radiosity_create_hcube_display_grp (void);
void   dpr_radiosity_create_hcube_studio_grp (void);
void   dpr_radiosity_del_datalsts  (dot_object *);
void   dpr_radiosity_del_datalsts_execute  (void);
DtInt  dpr_radiosity_del_datalsts_method_initialize  (void);
void   dpr_radiosity_del_datalsts_new_class  (DtPtr, DtInt);
void   dpr_radiosity_del_simply_datalsts  (dot_object *);
void   dpr_radiosity_del_simplymsh_datalsts  (dot_object *);
void   dpr_radiosity_del_varsimplymsh_datalsts  (dot_object *);
void   dpr_radiosity_device_update_view  (dot_object *, dot_object *);
void   dpr_radiosity_difclr_hcube_setup  (void);
void   dpr_radiosity_display (void);
void   dpr_radiosity_distribute_complex_simplymsh_triangle  (
		struct simplymsh *, dot_gpolyd_interpolated_vertex *,
		dot_gpolyd_interpolated_vertex *,
		dot_gpolyd_interpolated_vertex *);
void   dpr_radiosity_distribute_complex_varsimplymsh_triangle  (
		struct varsimplymsh *, dot_gpolyd_interpolated_vertex *,
		dot_gpolyd_interpolated_vertex *,
		dot_gpolyd_interpolated_vertex *);
void   dpr_radiosity_distribute_simple_simplymsh_triangle  (
		struct simplymsh *, DtInt, DtInt, DtInt);
void   dpr_radiosity_distribute_simple_varsimplymsh_triangle  (
		struct varsimplymsh *, DtInt, DtInt, DtInt);
void   dpr_radiosity_emiclr_hcube_setup  (void);
void   dpr_radiosity_emifct_hcube_setup  (void);
void   dpr_radiosity_emiswi_hcube_setup  (void);
DtInt  dpr_radiosity_find_dsptyp_method_initialize  (void);
void   dpr_radiosity_find_dsptyp_new_class  (DtPtr, DtInt);
void   dpr_radiosity_find_intermediate_disp_type  (void);
DtFlag dpr_radiosity_gather_scene_energy_info (void);
void   dpr_radiosity_generic_hcube_render_rtn  (dot_object *);
void   dpr_radiosity_generic_show_render_rtn  (dot_object *);
void   dpr_radiosity_geoatt_hcube_setup  (dot_object *);
DtFlag dpr_radiosity_geom_check_already_installed (void);
DtInt  dpr_radiosity_get_additional_data_index (DtPtr);
dpt_rad_primitive_data*
       dpr_radiosity_get_current_set_additional_data (dpt_rad_primitive_data*);
DtInt  dpr_radiosity_get_id_additional_data_index (DtInt);
dpt_rad_primitive_data*
       dpr_radiosity_get_next_set_additional_data (dpt_rad_primitive_data*);
void   dpr_radiosity_group_execute_current_hcube_render  (dot_object *);
void   dpr_radiosity_group_execute_current_show_render  (dot_object *);
void   dpr_radiosity_hcube_distrib_execute  (void);
void   dpr_radiosity_hcube_distrib_init_execution  (void);
DtInt  dpr_radiosity_hcube_distrib_method_initialize  (void);
void   dpr_radiosity_hcube_distrib_new_class  (DtPtr, DtInt);
void   dpr_radiosity_hcube_distrib_update_form_factors (DtInt, unsigned char *);
DtInt  dpr_radiosity_hcube_render_method_initialize  (void);
void   dpr_radiosity_hcube_render_new_class  (char*, DtInt);
void   dpr_radiosity_hcube_select_att_initialize  (void);
void   dpr_radiosity_hcube_select_compute_ambient  (void);
void   dpr_radiosity_hcube_select_execute  (void);
DtInt  dpr_radiosity_hcube_select_method_initialize  (void);
void   dpr_radiosity_hcube_select_new_class  (DtPtr, DtInt);
DtFlag dpr_radiosity_hcube_select_test_convergence  (void);
void   dpr_radiosity_hcube_setup_att_initialize  (void);
void   dpr_radiosity_hcube_setup_execute  (void);
DtInt  dpr_radiosity_hcube_setup_method_initialize  (void);
void   dpr_radiosity_hcube_setup_new_class  (DtPtr, DtInt);
void   dpr_radiosity_initialize (void);
void   dpr_radiosity_lcswcs_hcube_select  (dot_object *);
void   dpr_radiosity_lcswcs_hcube_setup  (void);
DtFlag dpr_radiosity_need_to_display (void);
void   dpr_radiosity_new_class (DtPtr, DtInt);
DtFlag dpr_radiosity_open_hcube_device (void);
void   dpr_radiosity_ply_hcube_setup  (dot_object *);
void   dpr_radiosity_ply_show_render  (dot_object *);
void   dpr_radiosity_plymsh_hcube_setup  (dot_object *);
void   dpr_radiosity_plymsh_show_render  (dot_object *);
DtPtr  dpr_radiosity_prim_create_data (dot_object*);
void   dpr_radiosity_prim_delete_data (dot_object*, dpt_rad_primitive_data*);
void   dpr_radiosity_print_null_rtn (dot_object*, dpt_rad_primitive_data*);
void   dpr_radiosity_prisrf_hcube_setup  (dot_object *);
void   dpr_radiosity_prisrf_show_render  (dot_object *);
void   dpr_radiosity_quadgrid_hcube_setup  (dot_object *);
void   dpr_radiosity_quadgrid_show_render  (dot_object *);
void   dpr_radiosity_quadlst_hcube_setup  (dot_object *);
void   dpr_radiosity_quadlst_show_render  (dot_object *);
void   dpr_radiosity_quadmsh_hcube_setup  (dot_object *);
void   dpr_radiosity_quadmsh_show_render  (dot_object *);
void   dpr_radiosity_radswi_hcube_setup  (void);
DtReal dpr_radiosity_random (void);
void   dpr_radiosity_reptyp_hcube_setup  (void);
void   dpr_radiosity_request_additional_data (DtPtr, DtInt);
void   dpr_radiosity_set_current_method_base  (void);
void   dpr_radiosity_set_current_method_show_render  (void);
DtInt  dpr_radiosity_show_render_method_initialize  (void);
void   dpr_radiosity_show_render_new_class  (DtPtr, DtInt);
void   dpr_radiosity_simply_hcube_distrib  (dot_object *);
void   dpr_radiosity_simply_hcube_render  (dot_object *);
void   dpr_radiosity_simply_hcube_select  (dot_object *);
void   dpr_radiosity_simply_hcube_setup  (dot_object *);
void   dpr_radiosity_simply_show_render  (dot_object *);
void   dpr_radiosity_simplymsh_hcube_distrib  (dot_object *);
void   dpr_radiosity_simplymsh_hcube_render  (dot_object *);
void   dpr_radiosity_simplymsh_hcube_select  (dot_object *);
void   dpr_radiosity_simplymsh_hcube_setup  (dot_object *);
void   dpr_radiosity_simplymsh_show_render  (dot_object *);
void   dpr_radiosity_sphlst_hcube_setup  (dot_object *);
void   dpr_radiosity_sphlst_show_render  (dot_object *);
DtFlag dpr_rad_support (dot_object *);
void   dpr_radiosity_torus_hcube_setup  (dot_object *);
void   dpr_radiosity_torus_show_render  (dot_object *);
void   dpr_radiosity_transform_vertices (
		DtRealTriple*, DtInt, DtMatrix4x4, DtRealTriple*);
void   dpr_radiosity_trilst_hcube_distrib  (dot_object *);
void   dpr_radiosity_trilst_hcube_render  (dot_object *);
void   dpr_radiosity_trilst_hcube_select  (dot_object *);
void   dpr_radiosity_trilst_hcube_setup  (dot_object *);
void   dpr_radiosity_trilst_show_render  (dot_object *);
void   dpr_radiosity_trimsh_hcube_distrib  (dot_object *);
void   dpr_radiosity_trimsh_hcube_render  (dot_object *);
void   dpr_radiosity_trimsh_hcube_select  (dot_object *);
void   dpr_radiosity_trimsh_hcube_setup  (dot_object *);
void   dpr_radiosity_trimsh_show_render  (dot_object *);
void   dpr_radiosity_tristrip_hcube_distrib  (dot_object *);
void   dpr_radiosity_tristrip_hcube_render  (dot_object *);
void   dpr_radiosity_tristrip_hcube_select  (dot_object *);
void   dpr_radiosity_tristrip_hcube_setup  (dot_object *);
void   dpr_radiosity_tristrip_show_render  (dot_object *);
void   dpr_radiosity_varquadgrid_hcube_setup  (dot_object *);
void   dpr_radiosity_varquadgrid_show_render  (dot_object *);
void   dpr_radiosity_varquadmsh_hcube_setup  (dot_object *);
void   dpr_radiosity_varquadmsh_show_render  (dot_object *);
void   dpr_radiosity_varsimplymsh_hcube_distrib  (dot_object *);
void   dpr_radiosity_varsimplymsh_hcube_render  (dot_object *);
void   dpr_radiosity_varsimplymsh_hcube_select  (dot_object *);
void   dpr_radiosity_varsimplymsh_hcube_setup  (dot_object *);
void   dpr_radiosity_varsimplymsh_show_render  (dot_object *);
void   dpr_radiosity_vartrimsh_hcube_distrib  (dot_object *);
void   dpr_radiosity_vartrimsh_hcube_render  (dot_object *);
void   dpr_radiosity_vartrimsh_hcube_select  (dot_object *);
void   dpr_radiosity_vartrimsh_hcube_setup  (dot_object *);
void   dpr_radiosity_vartrimsh_show_render  (dot_object *);
void   dpr_radiosity_vartristrip_hcube_distrib  (dot_object *);
void   dpr_radiosity_vartristrip_hcube_render  (dot_object *);
void   dpr_radiosity_vartristrip_hcube_select  (dot_object *);
void   dpr_radiosity_vartristrip_hcube_setup  (dot_object *);
void   dpr_radiosity_vartristrip_show_render  (dot_object *);
void   dpr_radiosty_update_form_factors  (unsigned char*, DtInt,DtInt, DtReal*);

#endif
