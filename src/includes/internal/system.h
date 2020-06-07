#ifndef DORE_SYSTEM_H
#define DORE_SYSTEM_H
 
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
 
    /***************************/
    /***  Defined Constants  ***/
    /***************************/

#define dod_system_state_uninitialized	0
#define dod_system_state_initialized	1

#define dod_system_edit_source_valuator	0
#define dod_system_edit_source_others	1

/*
 * even if we could have more than 7 sets of uv/uvw's, we
 * restrict those numbers to 7 because we use them 
 * in the code to allocate static space
 */
#define dod_system_max_vertex_uv_cnt	7
#define dod_system_max_vertex_uvw_cnt	7

    
    /***************************/
    /***  Macro Definitions  ***/
    /***************************/

#define dod_system_inq_vertex_uv_cnt(m)   (((m) >> 16) & 0xF)
#define dod_system_inq_vertex_uvw_cnt(m)  (((m) >> 24) & 0xF)
#define dod_system_inq_vertex_basetype(m) (((m)      ) & 0x7)


    /**************************************/
    /***  Global Variable Declarations  ***/
    /**************************************/

extern DtInt     doe_system_active_traversals;
extern DtFlag    doe_system_alternate_object_execution_flag;
extern DtInt     doe_system_current_method;
extern DtInt     doe_system_main_processors;
extern Dt32Bits  doe_system_state;
extern DtFlag    doe_system_subdivision_modification_flag;
extern DtSwitch  doe_system_valuator_switch;

extern DtInt     doe_system_timing_connected_lines_drawn;
extern DtInt     doe_system_timing_lines_drawn;
extern DtInt     doe_system_timing_mesh_quads_drawn;
extern DtInt     doe_system_timing_mesh_triangles_drawn;
extern DtInt     doe_system_timing_points_drawn;
extern DtInt     doe_system_timing_quads_drawn;
extern DtInt     doe_system_timing_strip_triangles_drawn;
extern DtInt     doe_system_timing_triangles_drawn;
extern DtInt     doe_system_timing_firmware_spheres_drawn;
extern DtInt     doe_system_timing_firmware_cylinders_drawn;
extern DtInt     doe_system_timing_display_traverse_count;


    /*****************************/
    /***  Function Prototypes  ***/
    /*****************************/

void  dor_doresys_initialize  (DtInt);
void  dor_doresys_initialize_drivers  (void);
void  dor_doresys_initialize_renderers  (void);
void  dor_doresys_terminate  (void);

DtVolume*    dor_system_compute_bounding_volume  (DtVolume*, DtObject);
void         dor_system_execute_object  (dot_object*);
DtInt        dor_system_filerstr_read
                     (char*, DtInt*, DtInt*, DtInt*, DtRasterType*, DtPtr*);
void         dor_system_initialize  (DtInt, int);
DtAngleUnits dor_system_inq_angle_units  (void);
DtInt        dor_system_inq_current_method  (void);
DtSwitch     dor_system_inq_execution_depth  (void);
DtInt        dor_system_inq_number_processors  (void);
void         dor_system_inq_version  (DtPtr*);
void         dor_system_radiosity_initialize  (void);
void         dor_system_report_bounding_points  (DtInt, DtReal[]);
DtInt        dor_system_rstr_write  (DtObjectStructure*, char*);
void         dor_system_set_angle_units  (DtAngleUnits);
void         dor_system_set_edit_source  (DtSwitch);
void         dor_system_set_execution_depth  (DtInt);
void         dor_system_set_valuator_switch  (DtSwitch);
void         dor_system_terminate  (void);
DtVertexType dor_system_texture_uv_count  (Dt32Bits);
DtVertexType dor_system_texture_uvw_count  (Dt32Bits);

#endif
