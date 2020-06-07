#ifndef DORE_DOGEN_H
#define DORE_DOGEN_H
 
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

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <io.h>
#endif
#include <dore/dore.h>
#include <dore/dore_develop/error.h>
#include <dore/dore_develop/develop.h>
#include <dore/internal/error.h>
#include <dore/internal/util.h>
#include <dore/internal/font.h>


    /***************************/
    /***  Macro Definitions  ***/
    /***************************/

#define dod_no_enabled_edges	0
#define dod_edge1_enabled	1
#define dod_edge2_enabled	2
#define dod_edge3_enabled	4

#define	min(a,b)   (((a) < (b)) ? (a) : (b))
#define max(a,b)   (((a) > (b)) ? (a) : (b))
#define abs(a)	   (((a) >= 0 ) ? (a) : -(a))

#ifdef titan
#   define PRAGMA(STUFF) #pragma STUFF
#else
#   define PRAGMA(STUFF)
#endif


    /**************************/
    /***  Type Definitions  ***/
    /**************************/

typedef struct object   dot_object;   /* struct object defined in dore.h */


    /***********************************/
    /***  Data Structure Definitions ***/
    /***********************************/
 
typedef struct txtupvec {
    DtReal xup;
    DtReal yup;
} dot_txtupvec;
 
typedef struct txtaln {
    DtTextAlignHorizontal halign;
    DtTextAlignVertical valign;
} dot_txtaln;

typedef struct object_reference {
    struct object_reference *next;
    dot_object              *object;
} dot_object_reference;

typedef struct {
   DtReal width;
   DtReal height;
   DtReal depth;
} dot_size ;


    /*****************************/
    /***  Function Prototypes  ***/
    /*****************************/

void        dor_alternate_object_execute  (dot_object *);

dot_object* dor_calbak_create             (DtPFI, DtObject, DtFlag);
void        dor_calbak_execute            (dot_object *);
dot_object* dor_calbak_get_dataobject     (dot_object *);
DtPFI       dor_calbak_get_function       (dot_object *);
DtFlag      dor_calbak_get_pass_by_ref    (dot_object *);
void        dor_calbak_initialize         (void);
void        dor_calbak_print              (dot_object *);
void	    dor_calbak_destroy		  (dot_object *);

DtFlag      dor_check_degen_triangle_vtx_values (DtReal[3],DtReal[3],DtReal[3]);
DtFlag      dor_check_degen_quad_vtx_values 
		    (DtReal[3],DtReal[3],DtReal[3],DtReal[3]);

DtInt       dor_class_add  (char*, DtInt, DtMethodEntry[], DtMethodPtr);
void        dor_class_add_notify  (DtPFV);
DtInt       dor_class_add_object_data
		    (DtInt, DtMethodPtr, DtMethodPtr, DtMethodPtr);
DtInt       dor_class_all_add_initialized_method
		    (char*, DtInt, DtMethodEntry[], DtMethodPtr);
DtInt       dor_class_all_add_method   (char*, DtInt, DtMethodEntry[]);
DtInt       dor_class_all_copy_method
		    (DtInt, char*, DtInt, DtMethodEntry[]);
DtFlag      dor_class_all_install_method
		    (DtInt, DtInt, DtMethodEntry[], DtInt, DtMethodPtr);
DtInt       dor_class_all_replace_method
		    (char*, DtInt, DtInt, DtMethodEntry[], DtInt);
dot_object *dor_class_create  (DtInt, DtInt, DtMethodEntry[], DtMethodPtr);
void        dor_class_delete  (dot_object *);
DtMethodPtr dor_class_inq_default_method  (DtInt);
DtInt       dor_class_inq_id  (char*);
DtMethodPtr dor_class_inq_method  (DtInt, DtInt);
DtInt       dor_class_install  (DtInt,char*,DtInt,DtMethodEntry[],DtMethodPtr);
void        dor_class_print  (dot_object *);
void        dor_class_set_method  (DtInt, DtInt, DtMethodPtr);

void        dor_condex_initialize                   (void);
DtFlag      dor_condex_query_execute                (DtInt);
DtFlag      dor_condex_query_render                 (DtInt);
DtFlag      dor_condex_query_pick                   (DtInt);
void        dor_condex_set_disable_prim_exec_flag   (DtFlag);
DtFlag      dor_condex_query_disable_prim_exec_flag (void);

dot_object* dor_datptr_create             (DtPtr);
DtPtr       dor_datptr_get_value          (dot_object *);
void        dor_datptr_initialize         (void);
void        dor_datptr_print              (dot_object *);
void	    dor_datptr_destroy		  (dot_object *);
dot_object* dor_datval_create             (Dt32Bits);
Dt32Bits    dor_datval_get_value          (dot_object *);
void        dor_datval_initialize         (void);
void        dor_datval_print              (dot_object *);
void	    dor_datval_destroy		  (dot_object *);

struct dictionary
	   *dor_dictionary_create           (void);
void        dor_dictionary_set_name_integer (struct dictionary *, dot_object*,
					     DtInt, DtFlag);
void        dor_dictionary_set_name_string  (struct dictionary *, dot_object*,
					     char*, DtFlag);
DtNameType  dor_dictionary_inq_name_type    (struct dictionary *, dot_object*);
DtInt       dor_dictionary_inq_name_integer (struct dictionary *, dot_object*);
char*       dor_dictionary_inq_name_string  (struct dictionary *, dot_object*);
dot_object *dor_dictionary_inq_obj_integer  (struct dictionary *, DtInt, DtInt);
dot_object *dor_dictionary_inq_obj_string   (struct dictionary *, char*, DtInt);

dot_object* dor_fil_create          (DtFilter, DtInt, DtInt[], DtSetOperation);
void        dor_fil_execute         (dot_object *);
void        dor_fil_initialize      (void);
void        dor_fil_print           (dot_object *);
dot_object* dor_filerstr_create     (char*, char*);
void        dor_filerstr_destroy    (dot_object *);
void        dor_filerstr_initialize (void);
void        dor_filerstr_print      (dot_object *);

void        dor_font_draw_text      (dot_font_state *);
dot_font*   dor_font_get_font       (DtInt);
int         dor_font_get_type       (dot_font_state *);
void        dor_font_initialize     (void);
void        dor_font_inquire_bounds (DtInt, DtInt *, DtInt *);
dot_font*   dor_font_load           (DtInt);
char*       dor_get_font_name       (DtInt);

void        dor_frame_add_reference     (dot_object *, dot_object *);
dot_object* dor_frame_create            (void);
void        dor_frame_delete            (dot_object *);
void        dor_frame_initialize        (void);
void        dor_frame_inq_boundary      (dot_object *, DtVolume *);
DtVolume*   dor_frame_inq_boundary_ptr  (dot_object *);
void        dor_frame_inq_justification (dot_object *, DtReal *, DtReal *);
dot_object* dor_frame_inq_view_group    (dot_object *);
void        dor_frame_pick              (dot_object *, dot_object *);
void        dor_frame_print             (dot_object *);
void        dor_frame_remove_reference  (dot_object *, dot_object *);
void        dor_frame_set_boundary      (dot_object *, DtVolume *);
void        dor_frame_set_justification (dot_object *, DtReal, DtReal);
void        dor_frame_update            (dot_object *);
void        dor_frame_update_device     (dot_object *, dot_object *);
void        dor_frame_update_notify     (dot_object *);
void        dor_frame_update_view       (dot_object *, dot_object *);

void        dor_gas_initialize          (void);

struct group;	/* This structure is used in some dor_group functions. */

void        dor_group_add_object            (dot_object *);
void        dor_group_add_object_into       (dot_object *, dot_object *);
void        dor_group_add_reference         (dot_object *, dot_object *);
DtGroupNetworkStatus
	    dor_group_check                 (dot_object *);
void        dor_group_check_network         (dot_object *);
dot_object* dor_group_close                 (void);
dot_object* dor_group_create                (DtFlag, DtFlag);
DtFlag      dor_group_delete_between_labels (DtInt, DtInt);
void        dor_group_delete_element_range  (DtInt, DtInt);
void        dor_group_delete_elements       (DtInt);
void        dor_group_destroy               (dot_object *);
void        dor_group_empty                 (dot_object *);
void        dor_group_execute_current       (dot_object *);
DtInt       dor_group_find_label            (DtInt);
dot_object* dor_group_get_current_group     (void);
void        dor_group_initialize            (void);
DtInt       dor_group_inq_element_pointer   (void);
dot_object* dor_group_inq_open              (void);
dot_object_reference* dor_group_inq_references (dot_object *);
DtInt       dor_group_inq_size              (dot_object *);
dot_object* dor_group_inquire_object_at_position
					    (dot_object*, DtInt, DtInt);
void        dor_group_insert_element        (dot_object *, dot_object *);
void        dor_group_open                  (dot_object *, DtFlag);
void        dor_group_pick                  (dot_object *);
void        dor_group_pop                   (DtFlag);
void        dor_group_print                 (dot_object *);
void        dor_group_push                  (dot_object *, DtFlag);
void        dor_group_record_attribute      (dot_object *, DtPFV);
void        dor_group_remove_reference      (dot_object *, dot_object *);
void        dor_group_replace               (dot_object *);
void        dor_group_replace_element       (dot_object *, dot_object *);
void        dor_group_replace_in            (dot_object *, dot_object *);
DtFlag      dor_group_set_ele_ptr_rel_label (DtInt, DtInt);
void        dor_group_set_element_pointer   (DtInt, DtInt);
void        dor_group_table_add_block       (void);
void        dor_group_table_delete_group    (struct group *);
void        dor_group_table_initialize      (void);
void        dor_group_table_print           (void);
void        dor_group_traversal_abort       (void);
void        dor_group_traversal_return      (void);

DtInt       drr_glbrnd_install_renderer  (DtInt);

FILE*       dor_io_file_open_read  (char *);
FILE*       dor_io_file_open_write  (char *);
int         dor_io_open_read  (char *);
int         dor_io_open_write  (char *);
int         dor_io_read  (int, char*, unsigned int);
void        dor_io_write  (int, char*, unsigned int);
int         dor_io_file_close  (FILE *);
int         dor_io_close  (int);

dot_object* dor_label_create            (DtInt);
void        dor_label_initialize        (void);
void        dor_label_print             (dot_object *);

dot_object* dor_namset_create           (DtInt, DtInt [], DtSetOperation);
void        dor_namset_execute          (dot_object *);
void        dor_namset_initialize       (void);
void        dor_namset_print            (dot_object *);

DtObject    dor_null_create             (DtPtr);
void       *dor_null_default_method     (dot_object *);
void        dor_null_initialize         (void);

dot_object* dor_mapbmpswi_create        (DtSwitch);
void        dor_mapbmpswi_execute       (dot_object *);
void        dor_mapbmpswi_initialize    (void);
void        dor_mapbmpswi_print         (dot_object *);
dot_object* dor_mapdifclrswi_create     (DtSwitch);
void        dor_mapdifclrswi_execute    (dot_object *);
void        dor_mapdifclrswi_initialize (void);
void        dor_mapdifclrswi_print      (dot_object *);
dot_object* dor_mapenvswi_create        (DtSwitch);
void        dor_mapenvswi_execute       (dot_object *);
void        dor_mapenvswi_initialize    (void);
void        dor_mapenvswi_print         (dot_object *);
dot_object* dor_maptrnintswi_create     (DtSwitch);
void        dor_maptrnintswi_execute    (dot_object *);
void        dor_maptrnintswi_initialize (void);
void        dor_maptrnintswi_print      (dot_object *);

void        dor_object_add_reference      (dot_object *, dot_object *);
void        dor_object_allocate           (void);
void        dor_object_check_deletion     (dot_object *);
DtFlag      dor_object_confirm_type       (dot_object *, DtInt);
dot_object* dor_object_create             (DtInt, DtPtr);
void        dor_object_delete_reference   (dot_object *, dot_object *);
DtMethodPtr dor_object_get_current_method (dot_object *);
DtMethodPtr dor_object_get_method         (dot_object *, DtInt);
dot_object* dor_object_hold               (dot_object *);
void        dor_object_initialize         (void);
DtInt       dor_object_inq_class_type     (dot_object *);
DtMethodPtr dor_object_inq_default_method (dot_object *);
dot_object* dor_object_inq_via_integer    (DtInt, DtInt);
dot_object* dor_object_inq_via_string     (char*, DtInt);
DtFlag      dor_object_inq_hold           (dot_object *);
DtInt       dor_object_inq_method_id      (char*);
DtNameType  dor_object_inq_name_type      (dot_object*);
DtInt       dor_object_inq_name_integer   (dot_object*);
char*       dor_object_inq_name_string    (dot_object*);
int         dor_object_inq_ntypes         (void);
DtInt       dor_object_inq_type           (dot_object *);
DtFlag      dor_object_method_id_valid    (DtInt);
void       *dor_object_method_null        (dot_object *);
void        dor_object_print              (dot_object *);
void        dor_object_print_method_table (void);
void        dor_object_print_type         (dot_object *);
void        dor_object_reference_insert   (dot_object_reference**, dot_object*);
void        dor_object_reference_remove   (dot_object_reference**, dot_object*);
void        dor_object_release            (dot_object *);
void        dor_object_set_name_integer   (dot_object*, DtInt, DtFlag);
void        dor_object_set_name_string    (dot_object*, char*, DtFlag);
void        dor_object_terminate          (void);
DtInt       dor_object_validate           (dot_object *);

dot_object* dor_pckid_create              (DtInt);
void        dor_pckid_execute             (dot_object *);
void        dor_pckid_initialize          (void);
void        dor_pckid_print               (dot_object *);

void        dor_pick_connected_line_list  (DtRealTriple[8],DtInt,DtRealTriple*);
void        dor_pick_initialize  (void);
void        dor_pick_initialize_camera  (dot_object *);
void        dor_pick_initialize_element  (DtFlag);
void        dor_pick_initialize_object  (dot_object *);
void        dor_pick_line_list  (DtRealTriple [8], DtInt, DtReal [][3]);
void        dor_pick_point_list  (DtRealTriple [8], DtInt, DtRealTriple *);
void        dor_pick_pop_group  (void);
void        dor_pick_pop_lcstofcsmat  (void);
void        dor_pick_push_group  (dot_object*);
void        dor_pick_push_lcstofcsmat  (DtMatrix4x4);
void        dor_pick_set_camera_matrix  (DtMatrix4x4);
void        dor_pick_set_parallel_matrix  (DtReal, DtReal, DtReal);
void        dor_pick_set_perspective_matrix  (DtReal, DtReal, DtReal);
void        dor_pick_set_projection_matrix  (DtArea*, DtProjectionType,
					     DtPoint3, DtReal, DtReal, DtReal);
void        dor_pick_setup  (DtObject, DtInt*, DtInt, DtInt[], DtInt, long[],
			     DtReal[],DtReal[],DtReal[],dot_object*[],DtInt*);
void        dor_pick_terminate_element  (void);
DtFlag      dor_pick_transform_clip_z_point  (DtRealTriple, DtMatrix4x4);
void        dor_pick_triangle_list  (DtRealTriple[8], DtInt, DtRealTriple*);
void        dor_pick_triangle_mesh  (DtRealTriple[8], DtInt, DtInt,
				     DtRealTriple*, DtInt[][3]);
void        dor_pick_triangle_strip  (DtRealTriple[8], DtInt, DtRealTriple*);
void        dor_pick_view  (dot_object*, dot_object*);

dot_object* dor_popatt_create             (void);
void        dor_popatt_execute            (dot_object *);
void        dor_popatt_initialize         (void);
void        dor_popatt_print              (dot_object *);

dot_object* dor_pshatt_create             (void);
void        dor_pshatt_destroy            (dot_object *);
void        dor_pshatt_execute            (dot_object *);
void        dor_pshatt_initialize         (void);
void        dor_pshatt_print              (dot_object *);

DtInt       dor_render_add_renderer (
                  DtInt, char*, DtMethodPtr, DtMethodPtr, DtPFV, DtMethodPtr);
void        dor_render_get_wcstofcsmat    (DtMatrix4x4);
void        dor_render_initialize         (void);
DtInt       dor_render_inq_num_renderers  (void);
DtInt       dor_render_inq_renderer_id    (char*);
void        dor_render_inq_renderer_names (char*[]);
void        dor_render_render             (dot_object *, dot_object *);

dot_object* dor_rstr_create               (DtInt, DtInt, DtInt, DtRasterType,
					   DtPtr, DtPtr, DtObject);
void        dor_rstr_destroy              (dot_object *);
void        dor_rstr_init_delete_callback (void);
void        dor_rstr_initialize           (void);
void        dor_rstr_inq                  (dot_object*, DtInt*, DtInt*, DtInt*,
					   DtRasterType*, char**, DtPtr*);
void        dor_rstr_print                (dot_object *);
void        dor_rstr_update               (dot_object *);

void        dor_slot_activate             (dot_object *, DtReal);
dot_object* dor_slot_create               (void);
void        dor_slot_destroy              (dot_object *);
void        dor_slot_enqueue_input        (dot_object *, DtReal);
void        dor_slot_flush_input          (void);
void        dor_slot_initialize           (void);
void        dor_slot_input_value          (dot_object *, DtReal);
dot_object* dor_slot_inq_valuator_group   (dot_object *);
void        dor_slot_print                (dot_object *);

dot_object* dor_texaa_create            (DtTextureAntiAliasMode);
void        dor_texaa_destroy           (dot_object *);
void        dor_texaa_execute           (dot_object *);
void        dor_texaa_initialize        (void);
void        dor_texaa_print             (dot_object *);
dot_object* dor_texextuv_create         (DtExtendMode, DtExtendMode);
void        dor_texextuv_destroy        (dot_object *);
void        dor_texextuv_execute        (dot_object *);
void        dor_texextuv_initialize     (void);
void        dor_texextuv_print          (dot_object *);
dot_object* dor_texextuvw_create       (DtExtendMode,DtExtendMode,DtExtendMode);
void        dor_texextuvw_destroy       (dot_object *);
void        dor_texextuvw_execute       (dot_object *);
void        dor_texextuvw_initialize    (void);
void        dor_texextuvw_print         (dot_object *);
dot_object* dor_texintsrf_create        (DtObject, Dt2PartProjType);
void        dor_texintsrf_destroy       (dot_object *);
void        dor_texintsrf_execute       (dot_object *);
void        dor_texintsrf_initialize    (void);
void        dor_texintsrf_print         (dot_object *);
dot_object* dor_texmatuv_create         (DtMatrix3x3);
void        dor_texmatuv_destroy        (dot_object *);
void        dor_texmatuv_execute        (struct object *);
void        dor_texmatuv_initialize     (void);
void        dor_texmatuv_print          (dot_object *);
dot_object* dor_texmatuvw_create        (DtMatrix4x4);
void        dor_texmatuvw_destroy       (dot_object *);
void        dor_texmatuvw_execute       (struct object *);
void        dor_texmatuvw_initialize    (void);
void        dor_texmatuvw_print         (dot_object *);
dot_object* dor_texop_create            (DtTextureOperator);
void        dor_texop_destroy           (dot_object *);
void        dor_texop_execute           (dot_object *);
void        dor_texop_initialize        (void);
void        dor_texop_print             (dot_object *);
dot_object* dor_texscluv_create         (DtReal, DtReal);
void        dor_texscluv_destroy        (dot_object *);
void        dor_texscluv_execute        (dot_object *);
void        dor_texscluv_initialize     (void);
void        dor_texscluv_print          (dot_object *);
dot_object* dor_texscluvw_create        (DtReal, DtReal, DtReal);
void        dor_texscluvw_destroy       (dot_object *);
void        dor_texscluvw_execute       (dot_object *);
void        dor_texscluvw_initialize    (void);
void        dor_texscluvw_print         (dot_object *);
dot_object* dor_textrnuv_create         (DtReal, DtReal);
void        dor_textrnuv_destroy        (dot_object *);
void        dor_textrnuv_execute        (dot_object *);
void        dor_textrnuv_initialize     (void);
void        dor_textrnuv_print          (dot_object *);
dot_object* dor_textrnuvw_create        (DtReal, DtReal, DtReal);
void        dor_textrnuvw_destroy       (dot_object *);
void        dor_textrnuvw_execute       (dot_object *);
void        dor_textrnuvw_initialize    (void);
void        dor_textrnuvw_print         (dot_object *);
dot_object* dor_texuvidx_create         (DtInt);
void        dor_texuvidx_destroy        (dot_object *);
void        dor_texuvidx_execute        (dot_object *);
void        dor_texuvidx_initialize     (void);
void        dor_texuvidx_print          (dot_object *);
dot_object* dor_texuvwidx_create        (DtInt);
void        dor_texuvwidx_destroy       (dot_object *);
void        dor_texuvwidx_execute       (dot_object *);
void        dor_texuvwidx_initialize    (void);
void        dor_texuvwidx_print         (dot_object *);

void        dor_utilpri_print_vertextype  (DtVertexType);

void        dor_view_add_reference        (dot_object *, dot_object *);
dot_object *dor_view_create               (void);
void        dor_view_delete               (dot_object *);
void        dor_view_initialize           (void);
dot_object *dor_view_inq_active_camera    (dot_object*);
void        dor_view_inq_background_color (dot_object*, DtColorModel*,DtReal[]);
void        dor_view_inq_background_just  (dot_object*, DtReal*, DtReal*);
void        dor_view_inq_background_raster(dot_object*, dot_object**,
					   dot_object**);
void        dor_view_inq_boundary         (dot_object*, DtVolume*);
DtVolume   *dor_view_inq_boundary_ptr     (dot_object*);
DtFlag      dor_view_inq_clear_flag       (dot_object*);
dot_object *dor_view_inq_definition_group (dot_object*);
dot_object *dor_view_inq_display_group    (dot_object*);
void        dor_view_inq_formfac_comptype (dot_object*, DtFormFactorCompType*,
					   DtReal*);
void        dor_view_inq_rad_convergence  (dot_object*, DtRadiosityConvType*,
					   DtReal*);
DtRadiosityUpdateType
	    dor_view_inq_rad_updatetype   (dot_object*);
void        dor_view_inq_render_data      (dot_object*, DtInt, DtPtr*);
DtRenderStyle
	    dor_view_inq_render_style     (dot_object*);
void        dor_view_inq_sequence_number  (dot_object*, DtInt*);
DtInt       dor_view_inq_shade_index      (dot_object*);
DtUpdateType
	    dor_view_inq_update_type     (dot_object*);
void        dor_view_print                (dot_object*);
void        dor_view_remove_reference     (dot_object*, dot_object *);
DtObject    dor_view_resize_raster        (dot_object*, DtInt, DtInt);
void        dor_view_set_active_camera    (dot_object*, dot_object*);
void        dor_view_set_background_color (dot_object*, DtColorModel, DtReal[]);
void        dor_view_set_background_just  (dot_object*, DtReal, DtReal);
void        dor_view_set_background_raster(dot_object*,dot_object*,dot_object*);
void        dor_view_set_boundary         (dot_object*, DtVolume*);
void        dor_view_set_clear_flag       (dot_object*, DtFlag);
void        dor_view_set_formfac_comptype (dot_object*, DtFormFactorCompType,
					   DtReal*);
void        dor_view_set_rad_convergence  (dot_object*, DtRadiosityConvType,
					   DtReal*);
void        dor_view_set_rad_updatetype   (dot_object*, DtRadiosityUpdateType);
void        dor_view_set_render_data      (dot_object*, DtInt, DtPtr);
void        dor_view_set_render_style     (dot_object*, DtRenderStyle);
void        dor_view_set_shade_index      (dot_object*, DtInt);
void        dor_view_set_update_type      (dot_object*, DtUpdateType);
DtFlag      dor_view_traverse_display     (dot_object*, DtInt);
DtFlag      dor_view_traverse_studio      (dot_object*, DtInt);
void        dor_view_update               (dot_object*);
void        dor_view_update_notify        (dot_object*);

void        dpr_radiosity_initialize  (void);
DtFlag      dpr_radiosity_render      (dot_object*, dot_object*, DtInt, DtPFI);

void        dre_dynrnd_reset_current_display_method  (DtInt);

DtInt       drr_dynrnd_install_renderer  (DtInt);

void        fortran_call_error_handler  (DtPFV, DtInt, DtInt, char*, char*);

#endif
