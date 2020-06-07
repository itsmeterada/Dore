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
 
/*****************************************************************************
**  This file contains the constants, parameters, macros, data structures and
**  function declarations associated with the PEX dynamic renderer output
**  module.  If you need to hack this code, please refer to the tech notes at
**  the end of this file.
*****************************************************************************/

#define  PROTOS
#include <PEXlib.h>
#include <PEXoc.h>

#include <dore/dore.h>
#include <dore/dore_develop/error.h>
#include <dore/dore_develop/develop.h>
#include <dore/dore_develop/private/stdtexatt.h>
#include <dore/internal/matrix.h>

#include "../dcm/dcm.h"


    /****************************/
    /***  Program Parameters  ***/
    /****************************/

#define MATRIX_STACK_GROW_SIZE	10	/* # Matrices / Matrix Stack Increase */
#define LIGHT_HEAP_GROW_SIZE	10	/* # Lights / Light Heap Increase */

#define DEBUG_CALL 0


    /***************************/
    /***  Macro Definitions  ***/
    /***************************/

#ifndef print
#   define print(s)  fputs(s,stdout)
#endif

	/* Identity Matrix Initialization */

#define IDENTITY_4x4    { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 }

	/* Expands a vector out to a comma-separated list of components. */

#define veclist(v)    (v)[0],(v)[1],(v)[2]
#define vec4list(v)   (v)[0],(v)[1],(v)[2],(v)[3]

	/* Copy Dore' Matrix to Pex Matrix */

#define MATRIX_LOAD(A,B) \
do { \
    (A)[0][0] = (B)[0][0]; \
    (A)[0][1] = (B)[0][1]; \
    (A)[0][2] = (B)[0][2]; \
    (A)[0][3] = (B)[0][3]; \
    (A)[1][0] = (B)[1][0]; \
    (A)[1][1] = (B)[1][1]; \
    (A)[1][2] = (B)[1][2]; \
    (A)[1][3] = (B)[1][3]; \
    (A)[2][0] = (B)[2][0]; \
    (A)[2][1] = (B)[2][1]; \
    (A)[2][2] = (B)[2][2]; \
    (A)[2][3] = (B)[2][3]; \
    (A)[3][0] = (B)[3][0]; \
    (A)[3][1] = (B)[3][1]; \
    (A)[3][2] = (B)[3][2]; \
    (A)[3][3] = (B)[3][3]; \
} while (0)


    /********************************/
    /***  Structure Declarations  ***/
    /********************************/

typedef struct
{   DtMatrix4x4 M_local_world;
    DtMatrix4x4 M_local_frustum;
} ddt_pex_drom_matrix;


	/* The Light Data Structure.  This is allocated as a heap of lights. */

typedef struct
{   DtObject      objid;	/* Dore Light Object Handle */
    DtObject      dore_type;	/* Dore Light Type */
    DtReal        intensity;	/* Dore Light Intensity */
    DtSwitch      status;	/* Light Enabled/Disabled */
    int           pex_index;	/* PEX Light LUT Index */
    pxlLightEntry pex_entry;	/* PEX Lighting Parameters */
} ddt_pex_drom_light;


	/* Model Clipping List */
    
typedef struct _ModClipList
{   struct _ModClipList *next;		/* Next Modeling Clip List */
    pxlHalfSpace        *planes;	/* Array of Clipping Planes */
    int                  num_planes;	/* Number of Clipping Planes */
    int                  array_size;	/* Size of Clipping Plane Array */
} ModClipList;


    /* The following data structure holds all attributes for a given device. */

typedef struct
{
	/* X-Windows Data and Associated Data */

    DtObject             device;		/* Display Device Information */
    DtObject             view;			/* Display View Information */

	/* PEX Data Fields */

    pxlOCBuf            *ocbuffer;		/* PEX Output Command Buffer */
    pxlRenderer          renderer;		/* PEX Renderer */
    pxlPipelineContext   pc;			/* Pipeline Context */
    unsigned long        pc_value_mask[3];	/* Pipeline Context Value Mask*/
    pxlPCAttributes      pc_attrs;		/* PEX Attributes */
    pxlLookupTable       color_approx_LUT;	/* Color Approximation LUT */
    pxlColourApproxEntry color_approx;		/* Color Approximation Values */

	/* Matrices and Stack */

    pexLocalTransform3DData
			 PEX_local_world;	/* PEX Transformation Matrix */
    DtMatrix4x4          M_local_world;		/* Local to World */
    DtMatrix4x4          M_local_frustum;	/* Local to Frustum */
    DtInt                matrix_stack_size;	/* Max Depth of Matrix Stack */
    DtInt                matrix_stack_top;	/* Top of Matrix Stack */
    ddt_pex_drom_matrix *matrix_stack;

	/* Camera Attributes */

    DtCameraMatrixType   camera_type;		/* Type of Camera Projection */
    DtMatrix4x4          M_projection;		/* Camera Projection Matrix */
    pexViewEntry         pex_view;		/* PEX View Parameters */
    pexViewEntry         pex_view_save;		/* Temporary Save Storage */
    pexLookupTable       pex_viewLUT;		/* PEX View Lookup Table ID */
    DtMatrix4x4          M_camera_world;	/* Camera to World Xform */
    DtMatrix4x4          M_world_camera;	/* World to Camera Xform */
    DtMatrix4x4          M_world_frustum;	/* World to Frustum Xform */
    DtMatrix4x4          M_camera_frustum;	/* Camera to Frustum Xform */

	/* Lighting Fields */

    DtInt                light_heap_next;	/* Next Available Light Slot */
    DtInt                light_heap_size;	/* Current Light Heap Size */
    ddt_pex_drom_light  *light_heap;		/* Light Parameters */
    ddt_pex_drom_light   newlight;		/* New Lighting Parameters */
    int                  light_index_pex;	/* Next PEX Light Index */
    pxlLookupTable       light_LUT;		/* Light Lookup Table */

	/* Lighting Equation Values */

    DtSwitch             ambient_switch;	/* Ambient Illumination */
    DtSwitch             specular_switch;	/* Specular Illumination */
    DtSwitch             transp_switch;		/* Transparency */
    DtSwitch             diffuse_switch;	/* Diffuse Illumination */
    DtSwitch             bfcull_switch;		/* Backface Culling */

    DtReal               ambient_intens;	/* Ambient Intensity [0,1] */
    DtReal               diffuse_intens;	/* Diffuse Intensity */
    DtReal               specular_intens;	/* Specular Intensity */
    DtReal               transp_intens;		/* Transparency Intensity */

    pxlLookupTable       depthcue_LUT;		/* Depth-Cue Lookup Table */
    pxlDepthCueEntry     depthcue;		/* Depth-Cueing Entry */
    
    DtSwitch		 modclip_switch;	/* Model Clipping Switch */
    ModClipList         *modclip_list;		/* Modeling Clip List */

	/* Appearance Attributes */

    DtSwitch             bfcullable;		/* Backface Cullable */
    DtRepType            rep_type;		/* Object Representation Type */
    int                  shading_model;		/* Current Shading Model */

} ddt_pex_drom_data;


	/* The local data for the device */

typedef struct
{   DtVisualType visual_type;
    DtShadeMode  shade_mode;
    DtReal       ctodscale[3];	/* Scale for Frustum --> Device Coords */
    DtReal       ctodtrans[3];	/* Translate for Frustum --> Device Coords */
    DtReal       ftodscale[3];	/* Scale for Frame --> Device Coords */
    DtReal       ftodtrans[3];	/* Translate for Frame --> Device Coords */
    DtReal       annoscale[3];
} ddt_pex_drom_device;


	/* The local data for the view */

typedef struct
{   DtColorModel  background_color_model;
    DtColorRGB    background_color;
    DtReal        background_just[2];
} ddt_pex_drom_view;


	/* The local data for the device / view combination */

typedef struct
{   DtReal  actviewxmin;
    DtReal  actviewymin;
    DtReal  actviewxmax;
    DtReal  actviewymax;
} ddt_pex_drom_window;


	/* The DROM Class Data Structure */

typedef struct
{   DtPtr name;			/* Object Class Name */
    DtInt id;			/* Object Class Identifier */
    DtPFI routine_create;	/* Object Class Creation Routine */
    DtPFI routine_print;	/* Object Class Print Routine */
    DtPFI routine_delete;	/* Object Class Delete Routine */
    DtInt index;		/* Object Class Additional Data Index */
} ddt_pex_drom_class_data;


	/* The following structure fields are filled only once after PEX
	// is initialized.  Each field indicates the particular abilities
	// of the current PEX server.  */

typedef struct
{   DtSwitch  light_vector,		/* Vector Lights Supported */
	      light_point,		/* Point Lights Supported */
	      light_spot;		/* Spot Lights Supported */
    DtInt     light_max;		/* Maximum Supported Light Index */
    DtSwitch  line_dashed,		/* Linetype Dashed */
	      line_dotted,		/* Linetype Dotted */
	      line_dashdot;		/* Linetype Dash-Dot */
    DtSwitch  surfinterp_color,		/* Gouraud Surfinterp Supported */
	      surfinterp_dot,		/* Dot-Product Surfinterp Supported */
	      surfinterp_normal;	/* Phong Surfinterp Supported */
    DtSwitch  zbuffer;			/* ZBuffering Supported */
    DtSwitch  ready;			/* True if all values have been set */
} PexStatus;


	/* This structure contains the PEX modes that most closely approximate
	// an unsupported mode.  For example, if spot lights are not supported
	// by the current PEX server, but infinite lights are, then the
	// light_spot field will contain PEXLightWcsVector. */

typedef struct
{   pexEnumTypeIndex light_vector,	/* Supported Infinite Light Method */
		     light_point,	/* Supported Point Light Method */
		     light_spot;	/* Supported Spot Light Method */
    int              light_max;		/* Maximum # Lights Supported */
    int 	     line_dashed,	/* Supported Dashed Line Type */
		     line_dotted,	/* Supported Dotted Line Type */
		     line_dashdot;	/* Supported Dash-Dotted Line Type */
    int              surfinterp_color,	/* Supported Color Surface Interp */
		     surfinterp_dot,	/* Supported Vector-Dot Surface Interp*/
		     surfinterp_normal;	/* Supported Normal Surface Interp */
} PexModes;


    /*******************************/
    /***  Function Declarations  ***/
    /*******************************/

extern void   ddr_pex_drom_apply_att_clpvol ();
extern void   ddr_pex_drom_camera ();
extern DtPtr  ddr_pex_drom_create_local_device_data ();
extern DtPtr  ddr_pex_drom_create_local_view_data ();
extern DtPtr  ddr_pex_drom_create_local_window_data ();
extern void   ddr_pex_drom_delete_local_device_data ();
extern void   ddr_pex_drom_delete_local_view_data ();
extern void   ddr_pex_drom_delete_local_window_data ();
extern void   ddr_pex_drom_get_lcstowcsmat ();
extern void   ddr_pex_drom_get_wcstofcsmat ();
extern void   ddr_pex_drom_light ();
extern void   ddr_pex_drom_pop_att_clpvol ();
extern void   ddr_pex_drom_pop_lcstofcsmat ();
extern void   ddr_pex_drom_post_initialization ();
extern void   ddr_pex_drom_pre_initialization ();
extern void   ddr_pex_drom_push_att_clpvol ();
extern void   ddr_pex_drom_push_lcstofcsmat ();
extern void   ddr_pex_drom_set_att_ambint ();
extern void   ddr_pex_drom_set_att_ambswi ();
extern void   ddr_pex_drom_set_att_bacfacculble ();
extern void   ddr_pex_drom_set_att_bacfacculswi ();
extern void   ddr_pex_drom_set_att_clpswi ();
extern void   ddr_pex_drom_set_att_clpvol ();
extern void   ddr_pex_drom_set_att_depcue ();
extern void   ddr_pex_drom_set_att_depcueswi ();
extern void   ddr_pex_drom_set_att_difclr ();
extern void   ddr_pex_drom_set_att_difint ();
extern void   ddr_pex_drom_set_att_difswi ();
extern void   ddr_pex_drom_set_att_hidsrfswi ();
extern void   ddr_pex_drom_set_att_inttyp ();
extern void   ddr_pex_drom_set_att_lgtatn ();
extern void   ddr_pex_drom_set_att_lgtclr ();
extern void   ddr_pex_drom_set_att_lgtint ();
extern void   ddr_pex_drom_set_att_lgtspdang ();
extern void   ddr_pex_drom_set_att_lgtspdexp ();
extern void   ddr_pex_drom_set_att_lgtswi ();
extern void   ddr_pex_drom_set_att_lgttyp ();
extern void   ddr_pex_drom_set_att_lintyp ();
extern void   ddr_pex_drom_set_att_linwid ();
extern void   ddr_pex_drom_set_att_localaasty ();
extern void   ddr_pex_drom_set_att_localaaswi ();
extern void   ddr_pex_drom_set_att_refswi ();
extern void   ddr_pex_drom_set_att_reptyp ();
extern void   ddr_pex_drom_set_att_shaswi ();
extern void   ddr_pex_drom_set_att_shdidx ();
extern void   ddr_pex_drom_set_att_spcclr ();
extern void   ddr_pex_drom_set_att_spcfct ();
extern void   ddr_pex_drom_set_att_spcint ();
extern void   ddr_pex_drom_set_att_spcswi ();
extern void   ddr_pex_drom_set_att_srfedgclr ();
extern void   ddr_pex_drom_set_att_srfshd ();
extern void   ddr_pex_drom_set_att_transpclr ();
extern void   ddr_pex_drom_set_att_transpint ();
extern void   ddr_pex_drom_set_att_transpswi ();
extern void   ddr_pex_drom_set_camera_matrix ();
extern void   ddr_pex_drom_set_parallel_matrix ();
extern void   ddr_pex_drom_set_perspective_matrix ();
extern void   ddr_pex_drom_set_projection_matrix ();
extern void   ddr_pex_drom_start_update ();
extern DtFlag ddr_pex_drom_transform_clip_z_point ();
extern void   ddr_pex_drom_update_display ();
extern void   ddr_pex_drom_update_lcstowcsmat_lokatfrm ();
extern void   ddr_pex_drom_update_lcstowcsmat_pop ();
extern void   ddr_pex_drom_update_lcstowcsmat_push ();
extern void   ddr_pex_drom_update_lcstowcsmat_rotate ();
extern void   ddr_pex_drom_update_lcstowcsmat_scale ();
extern void   ddr_pex_drom_update_lcstowcsmat_shear ();
extern void   ddr_pex_drom_update_lcstowcsmat_tfmmat ();
extern void   ddr_pex_drom_update_lcstowcsmat_transl ();
extern void   ddr_pex_drom_update_local_data ();
extern void   ddr_pex_drom_update_studio ();

	/* Method Routines */

extern pxlStructure
	      BuildPointList                  ARGS((DtInt, DtRealTriple*));

extern DtPtr ddr_pex_drom_annotext_create     ARGS((DtObjectStructure*));
extern void  ddr_pex_drom_annotext_delete     ARGS((DtObjectStructure*, DtPtr));
extern void  ddr_pex_drom_annotext_print      ARGS((DtObjectStructure*, DtPtr));
extern void  ddr_pex_drom_annotext_render     ARGS((dot_object*));

extern DtPtr ddr_pex_drom_pointlist_create    ARGS((DtObjectStructure*));
extern void  ddr_pex_drom_pointlist_delete    ARGS((DtObjectStructure*, DtPtr));
extern void  ddr_pex_drom_pointlist_print     ARGS((DtObjectStructure*, DtPtr));
extern void  ddr_pex_drom_pointlist_render    ARGS((dot_object*));
extern void  ddr_pex_drom_varpointlist_render ARGS((dot_object*));

extern DtPtr ddr_pex_drom_lines_create        ARGS((DtObjectStructure*));
extern void  ddr_pex_drom_lines_delete        ARGS((DtObjectStructure*, DtPtr));
extern void  ddr_pex_drom_lines_print         ARGS((DtObjectStructure*, DtPtr));
extern void  ddr_pex_drom_linelist_render     ARGS((dot_object*));
extern void  ddr_pex_drom_varlinelist_render  ARGS((dot_object*));
extern void  ddr_pex_drom_polyline_render     ARGS((dot_object*));

extern DtPtr ddr_pex_drom_triangles_create    ARGS((DtObjectStructure*));
extern void  ddr_pex_drom_triangles_delete    ARGS((DtObjectStructure*, DtPtr));
extern void  ddr_pex_drom_triangles_print     ARGS((DtObjectStructure*, DtPtr));
extern void  ddr_pex_drom_trilist_render      ARGS((dot_object*));
extern void  ddr_pex_drom_trimesh_render      ARGS((dot_object*));
extern void  ddr_pex_drom_vartrimesh_render   ARGS((dot_object*));
extern void  ddr_pex_drom_tristrip_render     ARGS((dot_object*));
extern void  ddr_pex_drom_vartristrip_render  ARGS((dot_object*));

	/* Miscellanous Routines */

extern pxlStructure  BuildPointList    ARGS((DtInt, DtRealTriple*));
extern DtInt         GetClassInfo      ARGS((DtInt*, char*, DtInt*));
extern void          PrintAttributes   ARGS((void));
extern void          PrintMatrix       ARGS((DtMatrix4x4));
extern void          PrintStudio       ARGS((void));
extern void          UpdateLightState  ARGS(());


    /****************************************/
    /***  External Variable Declarations  ***/
    /****************************************/

extern DtMatrix4x4             Identity4x4;

extern ddt_pex_drom_data       dde_pex_drom_data;
extern DtFlag                  dde_pex_drom_pre_init;
extern PexStatus               pex_status;
extern PexModes                pex_modes;

extern ddt_pex_drom_device    *dde_pex_drom_device_curr;
extern ddt_pex_drom_view      *dde_pex_drom_view_curr;
extern ddt_pex_drom_window    *dde_pex_drom_window_curr;
extern ddt_pex_drom_class_data dde_pex_drom_class_data[];

	/* DROM Data Structure Aliases */

#define DROM    dde_pex_drom_data
#define DEVICE  dde_pex_drom_device_curr
#define VIEW    dde_pex_drom_view_curr
#define WINDOW  dde_pex_drom_window_curr
