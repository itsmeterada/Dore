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

#include <dore/dore.h>
#include <dore/dore_develop/error.h>
#include <dore/dore_develop/develop.h>
#include <dore/dore_develop/private/stdtexatt.h>

#ifndef WIN32
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#else
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#endif


    /***************************/
    /***  Macro Definitions  ***/
    /***************************/


/* Identity Matrix Initialization */
#define IDENTITY_MATRIX_4x4  { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 }

/* Copy Dore' Matrix to OpenGL Matrix */
#define CONVERT_DORE_TO_OPENGL(A,B) \
do { \
    (A)[0]  = (B)[0][0]; \
    (A)[1]  = (B)[0][1]; \
    (A)[2]  = (B)[0][2]; \
    (A)[3]  = (B)[0][3]; \
    (A)[4]  = (B)[1][0]; \
    (A)[5]  = (B)[1][1]; \
    (A)[6]  = (B)[1][2]; \
    (A)[7]  = (B)[1][3]; \
    (A)[8]  = (B)[2][0]; \
    (A)[9]  = (B)[2][1]; \
    (A)[10] = (B)[2][2]; \
    (A)[11] = (B)[2][3]; \
    (A)[12] = (B)[3][0]; \
    (A)[13] = (B)[3][1]; \
    (A)[14] = (B)[3][2]; \
    (A)[15] = (B)[3][3]; \
} while (0)

/* Copy Dore' Matrix to OpenGL Matrix */
#define CONVERT_OPENGL_TO_DORE(A,B) \
do { \
    (A)[0][0] = (B)[0]; \
    (A)[0][1] = (B)[1]; \
    (A)[0][2] = (B)[2]; \
    (A)[0][3] = (B)[3]; \
    (A)[1][0] = (B)[4]; \
    (A)[1][1] = (B)[5]; \
    (A)[1][2] = (B)[6]; \
    (A)[1][3] = (B)[7]; \
    (A)[2][0] = (B)[8]; \
    (A)[2][1] = (B)[9]; \
    (A)[2][2] = (B)[10]; \
    (A)[2][3] = (B)[11]; \
    (A)[3][0] = (B)[12]; \
    (A)[3][1] = (B)[13]; \
    (A)[3][2] = (B)[14]; \
    (A)[3][3] = (B)[15]; \
} while (0)

#define LOAD_OPENGL_ID_MATRIX(A) \
do { \
    (A)[0]  = 1.0; \
    (A)[1]  = 0.0; \
    (A)[2]  = 0.0; \
    (A)[3]  = 0.0; \
    (A)[4]  = 0.0; \
    (A)[5]  = 1.0; \
    (A)[6]  = 0.0; \
    (A)[7]  = 0.0; \
    (A)[8]  = 0.0; \
    (A)[9]  = 0.0; \
    (A)[10] = 1.0; \
    (A)[11] = 0.0; \
    (A)[12] = 0.0; \
    (A)[13] = 0.0; \
    (A)[14] = 0.0; \
    (A)[15] = 1.0; \
} while (0)


    /***************************/
    /***   OpenGL Functions  ***/
    /***************************/


# ifdef DORE_REAL_SINGLE
#   define LOAD_GL_MATRIX      glLoadMatrixf
#   define SET_GL_SCALE        glScalef
#   define SET_GL_ROTATE       glRotatef
#   define SET_GL_TRANSLATE    glTranslatef
#   define GET_GL_MATRIX       glGetFloatv
#   define GL_MULT_MATRIX      glMultMatrixf
#     ifdef NO_BUFFERED_EXTENSIONS
#       define GL_VERTEX       glVertex3fv
#       define GL_NORMAL       glNormal3fv
#       define GL_VCOLOR       glColor3fv
#     else
#       define KGL_COLOR_BUF   kglColorBuf3f
#       define KGL_VERTEX_BUF  kglVertexBuf3f
#       define KGL_NORMAL_BUF  kglNormalBuf3f
#     endif
# else
#   define LOAD_GL_MATRIX      glLoadMatrixd
#   define SET_GL_SCALE        glScaled
#   define SET_GL_ROTATE       glRotated
#   define SET_GL_TRANSLATE    glTranslated
#   define GET_GL_MATRIX       glGetDoublev
#   define GL_MULT_MATRIX      glMultMatrixd
#     ifdef NO_BUFFERED_EXTENSIONS
#       define GL_VERTEX       glVertex3dv
#       define GL_NORMAL       glNormal3dv
#       define GL_VCOLOR       glColor3fv
#     else
#       define KGL_COLOR_BUF   kglColorBuf3d
#       define KGL_VERTEX_BUF  kglVertexBuf3d
#       define KGL_NORMAL_BUF  kglNormalBuf3d
#    endif
# endif


#ifdef DORE_REAL_SINGLE
  typedef GLfloat  glReal; 
#else
  typedef GLdouble glReal; 
#endif


typedef struct {
    DtSwitch		status;
    DtInt 		max_count;
    DtInt		count;
    DtObject		*mapping;
    DtObject		*raster;
    dot_stdtexatt	*attributes;
} ddt_kopengl_drom_texmap;



/*
 * Data structure to hold all attributes
 */

typedef struct {

    DtObject	  	   device;
    DtObject		   view;

    /* Camera attributes of next camera */

    DtCameraMatrixType	camera_type;
    DtSwitch		stereo_switch;
    DtReal		stereo_eyeseparation;
    DtReal		stereo_distance;

    /* light attributes for future lights */

    DtInt                  lighting;
    DtObject		   light_type;
    DtColorRGB		   light_color;
    DtReal		   light_intensity;
    DtReal		   light_spread_angle_total;
    DtReal		   light_spread_angle_delta;
    DtReal		   light_spread_exponent;
    DtReal		   light_att_c1;
    DtReal		   light_att_c2;

    /* appearance attributes */

    DtReal		   ambint;
    DtSwitch		   ambswi;
    DtSwitch 		   bacfacculble;
    DtSwitch		   bacfacculswi;
    DtReal		   depcue_zfront;
    DtReal		   depcue_zback;
    DtReal		   depcue_sfront;
    DtReal		   depcue_sback;
    DtColorModel	   depcue_colormodel;
    DtColorRGB		   depcue_color;
    DtSwitch		   depcueswi;
    DtColorRGB		   difclr;
    DtReal		   difint;
    DtSwitch		   difswi;
    DtSwitch		   hidsrfswi;
    DtInterpType	   inttyp;
    DtLineType		   line_type;
    DtReal		   line_width;
    DtLocalAntiAliasStyle  localaasty;
    DtSwitch		   localaaswi;
    DtSwitch		   refswi;
    DtRepType		   reptyp;
    DtSwitch		   shaswi;
    DtInt		   shdidx;
    DtColorRGB		   spcclr;
    DtReal		   spcfct;
    DtReal		   spcint;
    DtSwitch		   spcswi;
    DtColorRGB		   srfedgclr;
    DtObject		   srfshd;
    DtColorRGB		   transpclr;
    DtReal		   transpint;
    DtSwitch		   transpswi;

    /* Texture Mapping Attributes */

    ddt_kopengl_drom_texmap mapbmp;
    ddt_kopengl_drom_texmap mapdifclr;
    ddt_kopengl_drom_texmap mapenv;
    ddt_kopengl_drom_texmap maptrnint;

} ddt_kopengl_drom_data;

/*
 * The local data for the device
 */

typedef struct {
    DtVisualType	visual_type;
    DtShadeMode		shade_mode;
    DtReal 		camera_to_device_scale[3];
    DtReal		camera_to_device_trans[3];
    DtReal		frame_to_device_scale[3];
    DtReal		frame_to_device_trans[3];
    
    DtReal		annoscale[3];
    DtFlag		stereoflag;
} ddt_kopengl_drom_device;

/*
 * The camera data structure
 */

typedef struct {
    DtCameraMatrixType	camera_type;
    DtSwitch		stereoswitch;
    DtReal		stereo_eyeseparation;
    DtReal		stereo_distance;
    glReal              projection_matrix[16];
    glReal              cam_to_world_matrix[16];   /* Camera to world space  */
    glReal              world_to_cam_matrix[16];   /* World to Camera space */
    glReal              cam_to_frustum_matrix[16]; /* Camera to Device space */
} ddt_kopengl_drom_camera;

/*
 * The light data structure
 */

typedef struct {
    DtObject		light;
    DtObject		type;
    glReal              light_to_world_matrix[16];  
    DtReal		position[3];
    DtReal 		direction[3];
    DtColorRGB		color;
    DtReal 		angle_total;
    DtReal		angle_delta;
    DtReal		exponent;
    DtReal		c1;
    DtReal		c2;
    DtSwitch		status;
} ddt_kopengl_drom_light;

/*
 * The local data for the view
 */

typedef struct {
    DtFlag  		     clear_flag;
    DtInt		     shade_index;
    DtColorModel	     background_color_model;
    DtColorRGB		     background_color;
    DtReal		     background_just[2];

 /* Camera */
    ddt_kopengl_drom_camera  camera_data;

 /* Lights */
    DtInt		     total_lights;
    DtInt		     max_lights;
    ddt_kopengl_drom_light   light[GL_MAX_LIGHTS];
} ddt_kopengl_drom_view;
    
/*
 * The local data for the device / view combination
 */

typedef struct {
    DtReal		actviewxmin;
    DtReal		actviewymin;
    DtReal		actviewxmax;
    DtReal		actviewymax;
} ddt_kopengl_drom_window;



    /****************************************/
    /***  External Variable Declarations  ***/
    /****************************************/


extern ddt_kopengl_drom_data    dde_kopengl_drom_data;
extern ddt_kopengl_drom_device *dde_kopengl_drom_curdevdat;
extern ddt_kopengl_drom_view   *dde_kopengl_drom_curviwdat;
extern ddt_kopengl_drom_window *dde_kopengl_drom_curwindat;
extern ddt_kopengl_drom_camera *dde_kopengl_drom_curcamdat;

/*
 * Aliases for data structures
 */
#define DROM     dde_kopengl_drom_data
#define DEVICE   dde_kopengl_drom_curdevdat 
#define VIEW     dde_kopengl_drom_curviwdat 
#define WINDOW   dde_kopengl_drom_curwindat 
#define CAMERA   dde_kopengl_drom_curcamdat 
 

extern DtFlag dde_kopengl_drom_initialized;
extern DtFlag dde_kopengl_drom_pre_init;


    /****************************************/
    /***      Function  Declarations      ***/
    /****************************************/

/*
 * camera.c function prototypes
 */
extern void 
ddr_kopengl_drom_camera 
   (
    void 
   );

extern void 
ddr_kopengl_drom_set_att_stereo 
   (
    DtReal,
    DtReal
   );

extern void 
ddr_kopengl_drom_set_att_stereoswi
   (
    DtSwitch
   );

extern void 
ddr_kopengl_drom_set_camera_matrix 
   (
    DtMatrix4x4 
   );

extern void 
ddr_kopengl_drom_set_parallel_matrix 
   (
    DtReal,
    DtReal,
    DtReal
   );

extern void 
ddr_kopengl_drom_set_perspective_matrix 
   (
    DtReal,
    DtReal,
    DtReal
   );

extern void 
ddr_kopengl_drom_set_projection_matrix 
   (
    DtArea*,
    DtProjectionType,
    DtPoint3,
    DtReal,
    DtReal,
    DtReal      
   );




/*
 * data.c function prototypes
 */

extern DtPtr 
ddr_kopengl_drom_create_local_device_data 
   (
    DtObject
   );

extern DtPtr 
ddr_kopengl_drom_create_local_view_data 
   (
    DtObject
   );

extern DtPtr 
ddr_kopengl_drom_create_local_window_data 
   (
    DtObject,
    DtObject
   );

extern void 
ddr_kopengl_drom_delete_local_device_data 
   (
    ddt_kopengl_drom_device* 
   );

extern void 
ddr_kopengl_drom_delete_local_view_data 
   (
    ddt_kopengl_drom_view* 
   );

extern void 
ddr_kopengl_drom_delete_local_window_data 
   (
    ddt_kopengl_drom_window* 
   );


/* 
 * appatt.c function prototypes 
 */

extern void
ddr_kopengl_drom_set_att_ambswi 
   (
    DtSwitch
   );

extern void
ddr_kopengl_drom_set_att_bacfacculble 
   (
    DtSwitch
   );

extern void
ddr_kopengl_drom_set_att_bacfacculswi
   (
    DtSwitch
   );

extern void
ddr_kopengl_drom_set_att_depcueswi 
   (
    DtSwitch
   );

extern void
ddr_kopengl_drom_set_att_difswi 
   (
    DtSwitch
   );

extern void
ddr_kopengl_drom_set_att_hidsrfswi 
   (
    DtSwitch
   );

extern void
ddr_kopengl_drom_set_att_localaaswi 
   (
    DtSwitch
   );

extern void
ddr_kopengl_drom_set_att_refswi 
   (
    DtSwitch
   );

extern void
ddr_kopengl_drom_set_att_shaswi 
   (
    DtSwitch
   );

extern void
ddr_kopengl_drom_set_att_spcswi 
   (
    DtSwitch
   );

extern void
ddr_kopengl_drom_set_att_transpswi 
   (
    DtSwitch
   );

extern void
ddr_kopengl_drom_set_att_ambint
   (
    DtReal
   );

extern void
ddr_kopengl_drom_set_att_depcue
   (
    DtReal,
    DtReal,
    DtReal,
    DtReal,
    DtColorModel,
    DtReal[]
   );

extern void
ddr_kopengl_drom_set_att_difclr
   (
    DtColorModel,
    DtReal[]
   );

extern void
ddr_kopengl_drom_set_att_difint
   (
    DtReal
   );

extern void
ddr_kopengl_drom_set_att_inttyp
   (
    DtInterpType
   );

extern void
ddr_kopengl_drom_set_att_lintyp
   (
    DtLineType
   );

extern void
ddr_kopengl_drom_set_att_linwid
   (
    DtReal
   );

extern void
ddr_kopengl_drom_set_att_localaasty
   (
    DtLocalAntiAliasStyle
   );

extern void
ddr_kopengl_drom_set_att_reptyp
   (
    DtRepType
   );

extern void
ddr_kopengl_drom_set_att_shdidx
   (
    DtInt
   );

extern void
ddr_kopengl_drom_set_att_spcclr
   (
    DtColorModel,
    DtReal[]
   );

extern void
ddr_kopengl_drom_set_att_spcfct
   (
    DtReal
   );

extern void
ddr_kopengl_drom_set_att_spcint
   (
    DtReal
   );

extern void
ddr_kopengl_drom_set_att_srfedgclr
   (
    DtColorModel,
    DtReal[]
   );

extern void
ddr_kopengl_drom_set_att_srfshd
   (
    DtObject
   );

extern void
ddr_kopengl_drom_set_att_transpclr
   (
    DtColorModel,
    DtReal[]
   );

extern void
ddr_kopengl_drom_set_att_transpint
   (
    DtReal
   );

/* 
 * geoatt.c function prototypes 
 */

extern void 
ddr_kopengl_drom_get_lcstowcsmat 
   (
    DtMatrix4x4
   );

extern void 
ddr_kopengl_drom_update_lcstowcsmat_lokatfrm 
   (
    DtPoint3,
    DtPoint3,
    DtVector3,
    DtCompType
   );

extern void 
ddr_kopengl_drom_update_lcstowcsmat_rotate 
   (
    DtAxis,
    DtReal,
    DtCompType 
   );


extern void 
ddr_kopengl_drom_update_lcstowcsmat_scale 
   (
    DtReal,
    DtReal,
    DtReal,
    DtCompType
   );

extern void 
ddr_kopengl_drom_update_lcstowcsmat_shear 
   (
    DtMajorPlane, 
    DtReal, 
    DtReal, 
    DtCompType
   );

extern void 
ddr_kopengl_drom_update_lcstowcsmat_tfmmat 
   (
    DtMatrix4x4, 
    DtCompType
   );

extern void 
ddr_kopengl_drom_update_lcstowcsmat_transl 
   (
    DtReal,
    DtReal,
    DtReal,
    DtCompType
   );

extern void 
ddr_kopengl_drom_update_lcstowcsmat_push 
   (
    void 
   );

extern void 
ddr_kopengl_drom_update_lcstowcsmat_pop 
   (
    void 
   );

extern void 
ddr_kopengl_drom_get_wcstofcsmat 
   (
    DtMatrix4x4 
   );

extern void 
ddr_kopengl_drom_push_lcstofcsmat 
   (
    DtMatrix4x4 
   );

extern void 
ddr_kopengl_drom_pop_lcstofcsmat 
   (
    void 
   );


static void
ddr_kopengl_drom_transform_by_matrix 
   (
    DtRealTriple, 
    glReal*,
    DtRealTriple
   );


extern DtFlag 
ddr_kopengl_drom_transform_clip_z_point 
   (
    DtRealTriple,
    DtMatrix4x4
   );

/*
 * output.c function prototypes
 */

extern void 
ddr_kopengl_drom_render_point_list
   (
    DtColorModel,
    DtRealTriple[8],
    DtInt,
    DtRealTriple*,
    DtRealTriple*,
    DtRealTriple*,
    DtInt,
    DtRealCouple**,
    DtInt,
    DtRealTriple**
   );

extern void 
ddr_kopengl_drom_render_line_list
   (
    DtColorModel,
    DtRealTriple[8],
    DtInt,
    DtRealTriple*,
    DtRealTriple*,
    DtRealTriple*,
    DtRealTriple*,
    DtInt,
    DtRealCouple**,
    DtInt,
    DtRealTriple**
   );

extern void 
ddr_kopengl_drom_render_connected_line_list
   (
    DtColorModel,
    DtRealTriple[8],
    DtInt,
    DtRealTriple*,
    DtRealTriple*,
    DtRealTriple*,
    DtRealTriple*,
    DtInt,
    DtRealCouple**,
    DtInt,
    DtRealTriple**
   );


extern void 
ddr_kopengl_drom_render_triangle_list
   (
    DtColorModel,
    DtRealTriple[8],
    DtInt,
    DtRealTriple*,
    DtRealTriple*,
    DtRealTriple*,
    DtRealTriple*,
    DtInt,
    DtRealCouple**,
    DtInt,
    DtRealTriple**
   );

extern void 
ddr_kopengl_drom_render_triangle_mesh
   (
    DtColorModel,
    DtRealTriple[8],
    DtInt,
    DtRealTriple*,
    DtInt,
    DtRealTriple*,
    DtRealTriple*,
    DtRealTriple*,
    DtInt[][3],
    DtInt[][3],
    DtInt,
    DtRealCouple**,
    DtInt,
    DtRealTriple**
   );

/*
 * update.c function prototypes
 */


extern void 
ddr_kopengl_drom_pre_initialization
   (
    void
   );

static void
ddr_kopengl_drom_lighting
   (
    void
   );

extern void 
ddr_kopengl_drom_post_initialization
   (
    void
   );

extern void 
ddr_kopengl_drom_update_local_data 
   (
    void 
   );

extern void 
ddr_kopengl_drom_start_update 
   (
    DtObject,
    DtObject,
    DtPtr,
    DtPtr,
    DtPtr 
   );

extern void 
ddr_kopengl_drom_update_studio
   (
    DtPFI 
   );

static void
ddr_gl_drom_update_background   
   (
    void
   );

extern void 
ddr_kopengl_drom_update_display
   (
    DtPFI 
   );


/* 
 * print.c function prototypes 
 */


extern void
ddr_kopengl_drom_print_point_list 
   (
    DtColorModel,
    DtRealTriple[8],
    DtInt,
    DtRealTriple*,
    DtRealTriple*,
    DtRealTriple*,
    DtInt,
    DtRealCouple**,
    DtInt,
    DtRealTriple**
   );

extern void
ddr_kopengl_drom_print_line_list 
   (
    DtColorModel,
    DtRealTriple[8],
    DtInt,
    DtRealTriple*,
    DtRealTriple*,
    DtRealTriple*,
    DtRealTriple*,
    DtInt,
    DtRealCouple**,
    DtInt,
    DtRealTriple**
   );

extern void
ddr_kopengl_drom_print_connected_line_list 
   (
    DtColorModel,
    DtRealTriple[8],
    DtInt,
    DtRealTriple*,
    DtRealTriple*,
    DtRealTriple*,
    DtRealTriple*,
    DtInt,
    DtRealCouple**,
    DtInt,
    DtRealTriple**
   );



extern void
ddr_kopengl_drom_print_triangle_list
   (
    DtColorModel,
    DtRealTriple[8],
    DtInt,
    DtRealTriple*,
    DtRealTriple*,
    DtRealTriple*,
    DtRealTriple*,
    DtInt,
    DtRealCouple**,
    DtInt,
    DtRealTriple**
   );

extern void
ddr_kopengl_drom_print_triangle_mesh
   (
    DtColorModel,
    DtRealTriple[8],
    DtInt,
    DtRealTriple*,
    DtInt,
    DtRealTriple*,
    DtRealTriple*,
    DtRealTriple*,
    DtInt[][3],
    DtInt[][3],
    DtInt,
    DtRealCouple**,
    DtInt,
    DtRealTriple**
   );


extern void
ddr_kopengl_drom_print_matrix 
   ( 
    DtMatrix4x4
   );

extern void
ddr_kopengl_drom_print_studio
   ( 
    void
   );

extern void
ddr_kopengl_drom_print_mapping
   (
    ddt_kopengl_drom_texmap*,
    char*
   );

extern void
ddr_kopengl_drom_print_atts 
   (
    void
   );

extern void
ddr_kopengl_print_dore_matrix
   (
    char*,
    DtMatrix4x4 
   );

extern void
ddr_kopengl_print_opengl_matrix
   (
    char*,
    glReal* 
   );

/* 
 * modclip.c function prototypes 
 */

extern void
ddr_kopengl_drom_set_att_clpswi
   (
    DtSwitch
   );

extern void
ddr_kopengl_drom_apply_att_clpvol
   (
    DtClipOperator,
    DtInt,
    DtHalfSpace*
   );

extern void
ddr_kopengl_drom_push_att_clpvol
   (
    void
   );

extern void
ddr_kopengl_drom_pop_att_clpvol
   (
    void
   );

extern void
ddr_kopengl_drom_set_att_clpvol
   (
    DtClipOperator,
    DtInt,
    DtHalfSpace*
   );



/*
 * light.c function prototypes
 */


extern void
ddr_kopengl_drom_light
   (
    DtObject    
   );

extern void
ddr_kopengl_drom_set_att_lgtatn
   (
    DtReal,
    DtReal
   );

extern void
ddr_kopengl_drom_set_att_lgtclr
   (
    DtColorModel,
    DtReal[]
   );

extern void
ddr_kopengl_drom_set_att_lgtint
   (
    DtReal
   );

extern void
ddr_kopengl_drom_set_att_lgtspdang
   (
    DtReal,
    DtReal
   );

extern void
ddr_kopengl_drom_set_att_lgtspdexp
   (
    DtReal
   );

extern void
ddr_kopengl_drom_set_att_lgtswi
   (
    DtInt,
    DtObject*
   );

extern void
ddr_kopengl_drom_set_att_lgttyp
   (
    DtObject
   );

extern void 
ddr_kopengl_drom_update_display_light
   (
    int
   );

extern void 
ddr_kopengl_drom_update_display_lights
   (
    void
   );


/*
 * map.c function prototypes
 */


extern void
ddr_kopengl_drom_set_att_mapbmp
   (
    DtInt, 
    DtObject[],
    DtObject[],
    dot_stdtexatt[]
   );

extern void
ddr_kopengl_drom_set_att_mapbmpswi
   (
    DtSwitch
   );

extern void
ddr_kopengl_drom_set_att_mapdifclr
   (
    DtInt, 
    DtObject[],
    DtObject[],
    dot_stdtexatt[]
   );

extern void
ddr_kopengl_drom_set_att_mapdifclrswi
   (
    DtSwitch
   );

extern void
ddr_kopengl_drom_set_att_mapenv
   (
    DtInt, 
    DtObject[],
    DtObject[],
    dot_stdtexatt[]
   );

extern void
ddr_kopengl_drom_set_att_mapenvswi
   (
    DtSwitch
   );

extern void
ddr_kopengl_drom_set_att_maptrnint
   (
    DtInt, 
    DtObject[],
    DtObject[],
    dot_stdtexatt[]
   );

extern void
ddr_kopengl_drom_set_att_maptrnintswi
   (
    DtSwitch
   );


/*
 * util.c function prototypes
 */


extern void
ddr_kopengl_restore_specular_color 
   (
    void
   );

extern void
ddr_kopengl_restore_specular_factor 
   (
    void
   );

extern void
ddr_kopengl_restore_diffuse_color 
   (
    void
   );
