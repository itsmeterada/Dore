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
 
/******************************************************************************
**  This header file contains declarations and definitions related to the PEX
**  device driver.
******************************************************************************/

    /**********************/
    /***  Header Files  ***/
    /**********************/
 
#include <stdio.h>
#include <math.h>

#define  FUNCPROTO 1
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/multibuf.h>

#include <X11/PEX5/PEXlib.h>
#include <X11/PEX5/PEXproto.h>

#include <dore/internal/dogen.h>
#include <dore/internal/driver.h>
#include <dore/internal/matrix.h>
#include <dore/dore_develop/error.h>
#include <dore/dore_develop/develop.h>
#include <dore/dore_develop/private/stdtexatt.h>


    /****************************/
    /***  Program Parameters  ***/
    /****************************/

#define MATRIX_STACK_GROW_SIZE	10	/* # Matrices / Matrix Stack Increase */
#define LIGHT_HEAP_GROW_SIZE	10	/* # Lights / Light Heap Increase */


    /***************************/
    /***  Macro Definitions  ***/
    /***************************/

	/* The following macros are defined in dore/include/internal/dogen.h,
	** but the compiler on the Alpha (at this time) incorrectly handles
	** expressions like foo.max.x - it expands "max." to the max macro with
	** null parameters.  As a workaround, I undefine the min, max and abs
	** macros here.  */

#undef min
#undef max
#undef abs

#define print(s)    fputs(s,stdout)

#define CLAMP(t,lo,hi)	(((t) < (lo)) ? (lo) : (((t) > (hi)) ? (hi) : (t)))

#define LUMINANCE(red,grn,blu) \
    ((0.299 * (red)) + (0.587 * (grn)) + (0.114 * (blu)))

	/* Identity Matrix Initialization */

#define IDENTITY_4x4    { {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} }

	/* Expands a vector out to a comma-separated list of components. */

#define veclist(v)    (v)[0],(v)[1],(v)[2]
#define vec4list(v)   (v)[0],(v)[1],(v)[2],(v)[3]

	/* Set PEX Color Entry */

#define SETCOLOR(entry,Cred,Cgreen,Cblue) \
(   entry.type            = PEXColorTypeRGB, \
    entry.value.rgb.red   = Cred,   \
    entry.value.rgb.green = Cgreen, \
    entry.value.rgb.blue  = Cblue   \
)

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


    /*******************************/
    /***  Structure Definitions  ***/
    /*******************************/

typedef struct
{
    PEXRenderer          renderer;		/* PEX Renderer */
    PEXPipelineContext   pc;			/* Pipeline Context */
    unsigned long        pc_value_mask[3];	/* Pipeline Context Value Mask*/
    PEXPCAttributes      pc_attrs;		/* PEX Attributes */
    PEXExtensionInfo    *ext_info;		/* PEX Extension Information */
    char                *error_string;		/* PEX Error Message String */
} ddt_pex_data;

typedef struct
{   DtObject          device;		/* Dore' Device Handle */
    Window            window;		/* X Window */
    int               screen;		/* User Screen */
    Display          *display;		/* X Display */
    GC                gc;		/* X Graphics Context */
    XImage           *image;		/* X Scanline Buffer */
#ifdef PIXMAP
    Pixmap            pixmap;		/* X Pixmap for Double Buffering */
#endif
    XWindowAttributes window_attrs;	/* X Window Information */
    DtInt             nbuffers;		/* Number of Display Buffers (1 / 2) */
    Multibuffer       buffers[2];	/* Mult. Image Buffs - Dbl Buffering */
    Multibuffer       dispbuff;		/* Current Display Buffer */

    DtVisualType      visualtype;	/* Visual Color Type */
    int               ncolors;		/* Number of Color Entries */
    Colormap          colormap;		/* Color Map for Pseudo Color */
    DtInt             shadeindex;	/* Shade-Range Index */

    DtFlag            user_display;	/* Existing Display to Use*/
    DtFlag            user_window;	/* Existing Window to Use*/
    DtFlag            auto_size;	/* true= Window Preserves Perspective */
    DtFlag            singlebuffered;	/* true= Single Buffered Display */
    int               winX, winY;	/* Window Location */
    unsigned int      width, height;	/* Size of the Display Window */

    DtColorRGB        bg_color;		/* Background Color */
} ddt_pex_dcm_data;


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
    PEXLightEntry pex_entry;	/* PEX Lighting Parameters */
} ddt_pex_drom_light;


	/* Model Clipping List */
    
typedef struct _ModClipList
{   struct _ModClipList *next;		/* Next Modeling Clip List */
    PEXHalfSpace        *planes;	/* Array of Clipping Planes */
    int                  num_planes;	/* Number of Clipping Planes */
    int                  array_size;	/* Size of Clipping Plane Array */
} ModClipList;


    /* The following data structure holds all attributes for a given device. */

typedef struct
{
	/* X-Windows Data and Associated Data */

    DtObject             device;		/* Display Device Information */
    DtObject             view;			/* Display View Information */

	/* Matrices and Stack */

    DtMatrix4x4          M_local_world;		/* Local to World */
    DtMatrix4x4          M_local_frustum;	/* Local to Frustum */
    DtInt                matrix_stack_size;	/* Max Depth of Matrix Stack */
    DtInt                matrix_stack_top;	/* Top of Matrix Stack */
    ddt_pex_drom_matrix *matrix_stack;

	/* Camera Attributes */

    DtCameraMatrixType   camera_type;		/* Type of Camera Projection */
    DtMatrix4x4          M_projection;		/* Camera Projection Matrix */
    PEXViewEntry         pexview;		/* PEX View Parameters */
    PEXViewEntry         pexview_save;		/* Temporary Save Storage */
    PEXLookupTable       pexview_LUT;		/* PEX View Lookup Table ID */
    DtMatrix4x4          M_camera_world;	/* Camera to World Xform */
    DtMatrix4x4          M_world_camera;	/* World to Camera Xform */
    DtMatrix4x4          M_world_frustum;	/* World to Frustum Xform */
    DtMatrix4x4          M_camera_frustum;	/* Camera to Frustum Xform */

	/* Color Approximation */

    PEXLookupTable       color_approx_LUT;	/* Color Approximation LUT */
    PEXColorApproxEntry  color_approx;		/* Color Approximation Values */

	/* Lighting Fields */

    DtInt                light_heap_next;	/* Next Available Light Slot */
    DtInt                light_heap_size;	/* Current Light Heap Size */
    ddt_pex_drom_light  *light_heap;		/* Light Parameters */
    ddt_pex_drom_light   newlight;		/* New Lighting Parameters */
    int                  light_index_pex;	/* Next PEX Light Index */
    PEXLookupTable       light_LUT;		/* Light Lookup Table */

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

    PEXLookupTable       depthcue_LUT;		/* Depth-Cue Lookup Table */
    PEXDepthCueEntry     depthcue;		/* Depth-Cueing Entry */
    
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
{   DtColorModel  background_color_model;   /* Background Color/Image Data */
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
    DtMethodPtr routine_create;	/* Object Class Creation Routine */
    DtMethodPtr routine_print;	/* Object Class Print Routine */
    DtMethodPtr routine_delete;	/* Object Class Delete Routine */
    DtInt index;		/* Object Class Additional Data Index */
} ddt_pex_drom_class_data;


	/* The following structure fields are filled only once after PEX
	** is initialized.  Each field indicates the particular abilities
	** of the current PEX server.  */

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
	** an unsupported mode.  For example, if spot lights are not supported
	** by the current PEX server, but infinite lights are, then the
	** light_spot field will contain PEXLightWcsVector. */

typedef struct
{   PEXEnumTypeIndex
	light_vector,		/* Supported Infinite Light Method */
	light_point,		/* Supported Point Light Method */
	light_spot;		/* Supported Spot Light Method */
    int light_max;		/* Maximum # Lights Supported */
    int line_dashed,		/* Supported Dashed Line Type */
	line_dotted,		/* Supported Dotted Line Type */
	line_dashdot;		/* Supported Dash-Dotted Line Type */
    int surfinterp_color,	/* Supported Color Surface Interp */
	surfinterp_dot,		/* Supported Vector-Dot Surface Interp*/
	surfinterp_normal;	/* Supported Normal Surface Interp */
} PexModes;


    /*******************************/
    /***  Function Declarations  ***/
    /*******************************/

void   ddr_pex_dcm_become_current_driver  (DtPtr);
void   ddr_pex_dcm_clear_rectangle_depth  (DtShort, DtShort, DtShort, DtShort);
void   ddr_pex_dcm_clear_rectangle_depth_and_color
		(DtShort, DtShort, DtShort, DtShort);
void   ddr_pex_dcm_close_device  (void);
DtPtr  ddr_pex_dcm_create_local_data  (DtObject);
void   ddr_pex_dcm_flush  (void);
DtFlag ddr_pex_dcm_initialize_device  (DtObject, DtPtr, DtPtr, DtPtr);
void   ddr_pex_dcm_inquire_auto_size  (DtFlag*);
void   ddr_pex_dcm_inquire_clip_list  (DtUInt*, DtReal**, DtReal[4], DtFlag*);
void   ddr_pex_dcm_inquire_color_entries  (DtColorModel,DtInt,DtInt,DtReal[]);
void   ddr_pex_dcm_inquire_device_extent  (DtVolume*);
void   ddr_pex_dcm_inquire_ncolors  (DtInt*);
DtFlag ddr_pex_dcm_inquire_pixel_data
		(DtRasterType, DtInt*, DtInt*, DtRasterType*, DtPtr*, DtFlag*);
void   ddr_pex_dcm_inquire_resolution  (DtReal*, DtReal*);
void   ddr_pex_dcm_inquire_stereo  (DtFlag*);
void   ddr_pex_dcm_inquire_visual_type  (DtVisualType*);
void   ddr_pex_dcm_set_background_color  (DtColorModel, DtReal[]);
void   ddr_pex_dcm_set_color_entries  (DtColorModel, DtInt, DtInt, DtReal[]);
void   ddr_pex_dcm_set_current_view  (DtObject);
void   ddr_pex_dcm_set_depth_buffer_enable  (DtFlag);
void   ddr_pex_dcm_set_depth_buffer_write  (DtFlag);
void   ddr_pex_dcm_set_foreground_color  (DtColorModel, DtReal[]);
void   ddr_pex_dcm_set_options  (DtPtr);
void   ddr_pex_dcm_set_shade_index  (DtInt);
void   ddr_pex_dcm_set_viewport  (DtReal, DtReal, DtReal, DtReal);
void   ddr_pex_dcm_swap_buffers  (void);
void   ddr_pex_dcm_update_geometry  (void);
void   ddr_pex_dcm_write_scanline_byte  (DtInt, DtInt, DtInt, DtUChar*);
DtPtr  ddr_pex_return_DCM_fcns  (void);

PEXStructure BuildPointList    (DtInt, DtRealTriple*);
DtInt        GetClassInfo      (DtInt*, char*, DtInt*);
void         PrintAttributes   (void);
void         PrintMatrix       (DtMatrix4x4);
void         PrintStudio       (void);
void         UpdateLightState  (void);

DtPtr  ddr_pex_drom_annotext_create  (DtObjectStructure*);
void   ddr_pex_drom_annotext_delete  (DtObjectStructure*, DtPtr);
void   ddr_pex_drom_annotext_print  (DtObjectStructure*, DtPtr);
void   ddr_pex_drom_annotext_render  (dot_object*);
void   ddr_pex_drom_apply_att_clpvol  (DtClipOperator, DtInt, DtHalfSpace*);
void   ddr_pex_drom_camera  (void);
DtPtr  ddr_pex_drom_create_local_device_data  (DtObject);
DtPtr  ddr_pex_drom_create_local_view_data  (DtObject);
DtPtr  ddr_pex_drom_create_local_window_data  (DtObject, DtObject);
void   ddr_pex_drom_delete_local_device_data  (DtPtr);
void   ddr_pex_drom_delete_local_view_data  (DtPtr);
void   ddr_pex_drom_delete_local_window_data  (DtPtr);
void   ddr_pex_drom_get_lcstowcsmat  (DtMatrix4x4);
void   ddr_pex_drom_get_wcstofcsmat  (DtMatrix4x4);
void   ddr_pex_drom_light  (DtObject);
void   ddr_pex_drom_linelist_render  (dot_object*);
DtPtr  ddr_pex_drom_lines_create  (DtObjectStructure*);
void   ddr_pex_drom_lines_delete  (DtObjectStructure*, DtPtr);
void   ddr_pex_drom_lines_print  (DtObjectStructure*, DtPtr);
void   ddr_pex_drom_new_class  (char*, DtInt);
DtPtr  ddr_pex_drom_pointlist_create  (DtObjectStructure*);
void   ddr_pex_drom_pointlist_delete  (DtObjectStructure*, DtPtr);
void   ddr_pex_drom_pointlist_print  (DtObjectStructure*, DtPtr);
void   ddr_pex_drom_pointlist_render  (dot_object*);
void   ddr_pex_drom_polyline_render  (dot_object*);
void   ddr_pex_drom_pop_att_clpvol  (void);
void   ddr_pex_drom_pop_lcstofcsmat  (void);
void   ddr_pex_drom_post_initialization  (void);
void   ddr_pex_drom_pre_initialization  (void);
void   ddr_pex_drom_push_att_clpvol  (void);
void   ddr_pex_drom_push_lcstofcsmat  (DtMatrix4x4);
void   ddr_pex_drom_set_att_ambint  (DtReal);
void   ddr_pex_drom_set_att_ambswi  (DtSwitch);
void   ddr_pex_drom_set_att_bacfacculble  (DtSwitch);
void   ddr_pex_drom_set_att_bacfacculswi  (DtSwitch);
void   ddr_pex_drom_set_att_clpswi  (DtSwitch);
void   ddr_pex_drom_set_att_clpvol  (DtClipOperator, DtInt, DtHalfSpace*);
void   ddr_pex_drom_set_att_depcue
		(DtReal, DtReal, DtReal, DtReal, DtColorModel, DtReal[]);
void   ddr_pex_drom_set_att_depcueswi  (DtSwitch);
void   ddr_pex_drom_set_att_difclr  (DtColorModel, DtReal[]);
void   ddr_pex_drom_set_att_difint  (DtReal);
void   ddr_pex_drom_set_att_difswi  (DtSwitch);
void   ddr_pex_drom_set_att_hidsrfswi  (DtSwitch);
void   ddr_pex_drom_set_att_inttyp  (DtInterpType);
void   ddr_pex_drom_set_att_lgtatn  (DtReal, DtReal);
void   ddr_pex_drom_set_att_lgtclr  (DtColorModel, DtReal[]);
void   ddr_pex_drom_set_att_lgtint  (DtReal);
void   ddr_pex_drom_set_att_lgtspdang  (DtReal, DtReal);
void   ddr_pex_drom_set_att_lgtspdexp  (DtReal);
void   ddr_pex_drom_set_att_lgtswi  (DtInt, DtObject*);
void   ddr_pex_drom_set_att_lgttyp  (DtObject);
void   ddr_pex_drom_set_att_lintyp  (DtLineType);
void   ddr_pex_drom_set_att_linwid  (DtReal);
void   ddr_pex_drom_set_att_reptyp  (DtRepType);
void   ddr_pex_drom_set_att_shdidx  (DtInt);
void   ddr_pex_drom_set_att_spcclr  (DtColorModel, DtReal[]);
void   ddr_pex_drom_set_att_spcfct  (DtReal);
void   ddr_pex_drom_set_att_spcint  (DtReal);
void   ddr_pex_drom_set_att_spcswi  (DtSwitch);
void   ddr_pex_drom_set_att_srfedgclr  (DtColorModel, DtReal[]);
void   ddr_pex_drom_set_att_srfshd  (DtObject);
void   ddr_pex_drom_set_att_transpclr  (DtColorModel, DtReal[]);
void   ddr_pex_drom_set_att_transpint  (DtReal);
void   ddr_pex_drom_set_att_transpswi  (DtSwitch);
void   ddr_pex_drom_set_camera_matrix  (DtMatrix4x4);
void   ddr_pex_drom_set_parallel_matrix  (DtReal, DtReal, DtReal);
void   ddr_pex_drom_set_perspective_matrix  (DtReal, DtReal, DtReal);
void   ddr_pex_drom_set_projection_matrix
		(DtArea*, DtProjectionType, DtPoint3, DtReal, DtReal, DtReal);
void   ddr_pex_drom_start_update  (DtObject, DtObject, DtPtr, DtPtr, DtPtr);
DtFlag ddr_pex_drom_transform_clip_z_point  (DtRealTriple, DtMatrix4x4);
DtPtr  ddr_pex_drom_triangles_create  (DtObjectStructure*);
void   ddr_pex_drom_triangles_delete  (DtObjectStructure*, DtPtr);
void   ddr_pex_drom_triangles_print  (DtObjectStructure*, DtPtr);
void   ddr_pex_drom_trilist_render  (dot_object*);
void   ddr_pex_drom_trimesh_render  (dot_object*);
void   ddr_pex_drom_tristrip_render  (dot_object*);
void   ddr_pex_drom_update_display  (DtPFI);
void   ddr_pex_drom_update_lcstowcsmat_lokatfrm
		(DtPoint3, DtPoint3, DtVector3, DtCompType);
void   ddr_pex_drom_update_lcstowcsmat_pop  (void);
void   ddr_pex_drom_update_lcstowcsmat_push  (void);
void   ddr_pex_drom_update_lcstowcsmat_rotate  (DtAxis, DtReal, DtCompType);
void   ddr_pex_drom_update_lcstowcsmat_scale  (DtReal,DtReal,DtReal,DtCompType);
void   ddr_pex_drom_update_lcstowcsmat_shear
		(DtMajorPlane, DtReal, DtReal, DtCompType);
void   ddr_pex_drom_update_lcstowcsmat_tfmmat  (DtMatrix4x4, DtCompType);
void   ddr_pex_drom_update_lcstowcsmat_transl (DtReal,DtReal,DtReal,DtCompType);
void   ddr_pex_drom_update_local_data  (void);
void   ddr_pex_drom_update_shade_index  (void);
void   ddr_pex_drom_update_studio  (DtPFI);
void   ddr_pex_drom_varlinelist_render  (dot_object*);
void   ddr_pex_drom_varpointlist_render  (dot_object*);
void   ddr_pex_drom_vartrimesh_render  (dot_object*);
void   ddr_pex_drom_vartristrip_render  (dot_object*);
DtPtr  ddr_pex_return_DROM_fcns  (void);
DtPtr  ddr_pex_return_DROM_methods  (void);

void   ddr_pex_prom_write_rectangle_byte_rgb (DtInt,DtInt,DtInt,DtInt,DtUChar*);
DtPtr  ddr_pex_return_PROM_fcns (void);


    /**************************/
    /***  Global Variables  ***/
    /**************************/

extern DtMatrix4x4 Identity4x4;

extern DtFlag      dde_pex_drom_pre_init;
extern PexStatus   pex_status;
extern PexModes    pex_modes;

extern ddt_pex_data             dde_pex_data;
extern ddt_pex_dcm_data        *dde_pex_current_dcm_data;
extern ddt_pex_drom_data        dde_pex_drom_data;
extern ddt_pex_drom_device     *dde_pex_drom_device_curr;
extern ddt_pex_drom_view       *dde_pex_drom_view_curr;
extern ddt_pex_drom_window     *dde_pex_drom_window_curr;
extern ddt_pex_drom_class_data  dde_pex_drom_class_data[];

#ifdef DEFINE_PEX_GLOBALS
    DtMatrix4x4 Identity4x4;

    DtFlag dde_pex_drom_pre_init;

    ddt_pex_data         dde_pex_data;
    ddt_pex_dcm_data    *dde_pex_current_dcm_data;
    ddt_pex_drom_data    dde_pex_drom_data;
    ddt_pex_drom_device *dde_pex_drom_device_curr;
    ddt_pex_drom_view   *dde_pex_drom_view_curr;
    ddt_pex_drom_window *dde_pex_drom_window_curr;
#endif

	/* Data Structure Aliases */

#define PEX     dde_pex_data
#define DCM     dde_pex_current_dcm_data	/* Short-Name Alias */
#define DROM    dde_pex_drom_data
#define DEVICE  dde_pex_drom_device_curr
#define VIEW    dde_pex_drom_view_curr
#define WINDOW  dde_pex_drom_window_curr
