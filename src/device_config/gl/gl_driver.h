/***************************************************************************
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
***************************************************************************/

/*****************************************************************************
**  This header file contains the include directives, definitions and
**  declarations for the GL device driver for Dore'.
*****************************************************************************/

#ifndef GL_DRIVER_H
#define GL_DRIVER_H

#include <dore/internal/dogen.h>
#include <dore/dore_develop/develop.h>

#ifdef SGI4_0
#   include <X11/Xlib.h>
#   include <gl/glws.h>
#endif

#include <gl.h>


    /***************************/
    /***  Defined Constants  ***/
    /***************************/

	/* If any of these #defines are changed, dith.c must (of course) be
	** recompiled.
	**
        **     Typical use:
        **
        **     load_color_table (ct=ddr_gl_dcm_dith_make_color_table());
        **     free(ct);
        **     ...
        **     putpixel(x,y,ddr_gl_dcm_dith_convert_3_to_1(x,y,r,g,b));
        */

	/* Range of the primaries to be fed to ddr_gl_dcm_dith_convert_3_to_1
        ** (must be power of two).  Log2 of that:  */

#define Dith_NumVin 256
#define Dith_NumVinB 8

	/* Range of the primaries in the color table: */

#define Dith_NumVout 256

	/* Number of shades of each primary.  The product of all these things
	** determines the range of the output from
	** ddr_gl_dcm_dith_convert_3_to_1().  (e.g. NumRed * NumGreen * NumBlue
	** should be 256 or less for an 8 bit colormap index).  */

#define Dith_NumRed   6
#define Dith_NumGreen 7
#define Dith_NumBlue  6

#define Dith_NumColors (Dith_NumRed*Dith_NumGreen*Dith_NumBlue)


    /***************************/
    /***  Macro Definitions  ***/
    /***************************/

#define RGBshade(r,g,b)  (((r) * 0.30) + ((g) * 0.59) + ((b) * 0.11))

#define RGBpack(r,g,b) \
(     (unsigned long)(0xFF000000) \
    | (((unsigned long) ((b) * 255.0)) << (unsigned long)(16)) \
    | (((unsigned long) ((g) * 255.0)) << (unsigned long)(8))  \
    | (((unsigned long) ((r) * 255.0)) << (unsigned long)(0))  \
)

#define IrisLightNo(num)	(LIGHT0+(num))
#define IrisLightDefnNo(num)	(num+1)

#define CLAMP(x,lo,hi)   (((x) < (lo)) ? (lo) : ((x) > (hi)) ? (hi) : (x))

    /* The following set of macros provide for texture, color, normal and
    ** position information for a given vertex.  The routines are defined
    ** here as macros to provide for a consistent Dore'-GL interface,
    ** regardless of the Dore' real-value type (float or double).  Note that
    ** the do { ... } while(0) form is used here so that the macro calls can
    ** be terminated with a semi-colon, even in if-then-else statements.  */

#ifdef DORE_REAL_SINGLE
#   define SET_TEXTURE		t2f
#   define SET_COLOR		c3f
#   define SET_NORMAL		n3f
#   define SET_LOCATION		v3f
#else
#   define SET_TEXTURE		t2d
#   define SET_COLOR(c)		\
    do { auto float temp[3];	\
	temp[0] = (c)[0];	\
	temp[1] = (c)[1];	\
	temp[2] = (c)[2];	\
	c3f(temp);		\
    } while (0)
#   define SET_NORMAL(n)	\
    do { auto float temp[3];	\
	temp[0] = (n)[0];	\
	temp[1] = (n)[1];	\
	temp[2] = (n)[2];	\
	n3f(temp);		\
    } while (0)
#   define SET_LOCATION		v3d
#endif


    /**************************/
    /***  Type Definitions  ***/
    /**************************/

typedef enum { SPHTYPE_DORE, SPHTYPE_GLFULL, SPHTYPE_GLHEMI } SphereType;


    /*************************/
    /***  Data Structures  ***/
    /*************************/

typedef struct {
    DtPtr	 device;		/* Dore' Device */
    int		 width;			/* Display Width */
    int		 height;		/* Display Height */
    DtVisualType visualtype;
    DtFlag	 auto_size;		/* If True, Autosize Display Window */

    long 	 winid;			/* GL Window ID */
    int  	 user_window;

    SphereType   sphere_type;		/* Type of Spheres to Use */

    DtInt	 shadeindex;
    DtInt	 backgroundcolor;	/* Valid only in PseudoColor mode. */
    DtReal	 backgroundRGB[3];	/* Background Color */
    DtFlag	 use_X;
#ifdef SGI4_0
    Display 	*xdisplay;
    Window 	 xwindow;
    DtFlag	 user_xdisplay;
    DtFlag	 user_xwindow;
#endif
} ddt_gl_data;

typedef struct {
    int red  [Dith_NumColors];	/* In the range [0,Dith_NumVout-1] */
    int green[Dith_NumColors];	/* In the range [0,Dith_NumVout-1] */
    int blue [Dith_NumColors];	/* In the range [0,Dith_NumVout-1] */
} ddr_gl_dcm_dith_color_table;

typedef struct {
    DtObject	light;
    DtFlag	enabled;
    Matrix	light_to_world_matrix;	/* Matrix to position light in world */
    DtReal	position[3];
    DtReal	direction[3];
    DtReal	color[3];		/* RGB color of light */
    DtObject	type;			/* Ambient, Infinite, Local, etc.. */
    DtReal	spread_angle_total;
    DtReal	spread_angle_delta;
    DtReal	spread_exponent;
    DtReal	att_c1;
    DtReal	att_c2;
} light_data;

struct view_data {
    DtFlag clear_flag;
    DtReal backgroundRGB[3];
    DtInt  backgroundindex;
    DtReal background_just[2];

    Matrix cam_to_world_matrix;	/* Camera space to world space  */
    Matrix world_to_cam_matrix;	/* World space to camera space  */
    Matrix projection_matrix;	/* Camera space to device space */
    Matrix cam_to_frustum_matrix;

    int	numlights;
    light_data	light[MAXLIGHTS];
};

struct window_data {
    Screencoord	minx,maxx,miny,maxy;	/* Iris Viewport */
    DtReal frame_to_view_scale[3];	/* 1./framesize */
};

struct device_status {
    DtObject dore_device;
    DtObject dore_view;

    struct view_data   *view_data;
    struct window_data *window_data;

    DtInt	traversal_number;	/* 2nd pass for frame coord prims */
    DtSwitch	frame_coord_prim;	/* indicates if frame coord prim */
    DtSwitch	have_frame_coord_prim;

    DtInt	visualtype;

    DtSwitch	cull;
    DtSwitch	cullable;

    DtRepType	reptype;		/* Object Representation Type */

    DtInt	shadeindex;
    DtInt	rampmin,rampmax;	/* Ramp range for shadeindex */

    DtObject	lighttype;
    DtReal	lightcolor[3];		/* Color of light */
    DtReal	lightintens;		/* Intensity of light */
    DtReal	light_spread_angle_total;
    DtReal	light_spread_angle_delta;
    DtReal	light_spread_exponent;
    DtReal	light_att_c1;
    DtReal	light_att_c2;

    DtInt	lighting;		/* Yes/No light source shading */
    DtReal	diffusecolor[3];	/* Not multiplied by diffuseintens */
    DtReal	diffuseintens;
    DtInt	diffuseswitch;
    DtInt	ambientswitch;
    DtReal	ambientintens;

    DtInt	specularswitch;
    DtReal	specularfactor;
    DtReal	specularcolor[3];
    DtReal	specularintens;

    DtInt	transpswitch;
    DtReal	transpintens;

    DtInt	smoothswitch;		/* on=gouraud, off=flat */

    DtReal	depcuecolor[3];
    DtReal	depcuezfront;
    DtReal	depcuezback;
    DtReal	depcuesfront;
    DtReal	depcuesback;
    DtInt	depcueswitch;

    DtSwitch	map_diffuse_color_switch;

    int		texture_2D;
    DtInt	texture_uv_index;
    DtTextureOperator texture_op;
    DtInt	next_texid;
};

typedef struct {
    DtPtr       name;		/* Object Class Name */
    DtInt       id;		/* Object Class Identifier */
    DtMethodPtr crt_rtn;	/* Object Class Creation Routine */
    DtMethodPtr prt_rtn;	/* Object Class Print Routine */
    DtMethodPtr del_rtn;	/* Object Class Delete Routine */
    DtInt       index;		/* Object Class Additional Data Index */
} ddt_gl_drom_class_data;

typedef struct {
    struct {
	DtInt		sequence_number;
	DtPtr		data;
	DtPtr		z;
    } bgd;
    struct {
	DtInt		sequence_number;
	DtPtr		data;
	DtTextureAntiAliasMode aa_mode;
	DtExtendMode	extenduv[2];
	int		id;
    } tex;
} ddt_gl_drom_rstr;


    /**************************/
    /***  Global Variables  ***/
    /**************************/

#define CURDEV  			dde_gl_current_device_data
#define DevStatus 			dde_gl_drom_device_status
#define ddd_gl_drom_rstr_user_data	0x1

extern ddt_gl_data             *CURDEV;
extern struct device_status	DevStatus;
extern ddt_gl_drom_class_data	dde_gl_drom_class_data[];


    /*****************************/
    /***  Function Prototypes  ***/
    /*****************************/

Object MakeGLCylinder  (Object, DtInt*, DtInt*);
Object MakeGLSphereList (Object, DtInt*,DtInt*, DtInt, DtReal*,DtReal*,DtReal*);
void   ddr_dore_to_iris_matrix  (DtMatrix4x4, Matrix);
void   ddr_gl_dcm_become_current_driver  (DtPtr);
int    ddr_gl_dcm_check_device_extent  (void);
void   ddr_gl_dcm_clear_both_buffers  (void);
void   ddr_gl_dcm_clear_rectangle_depth  (DtShort, DtShort, DtShort, DtShort);
void   ddr_gl_dcm_clear_rectangle_depth_and_color (
	    DtShort, DtShort, DtShort, DtShort);
void   ddr_gl_dcm_close_device  (void);
DtPtr  ddr_gl_dcm_create_local_data  (DtObject);
int    ddr_gl_dcm_dith_compress  (int, int, int);
int    ddr_gl_dcm_dith_convert_3_to_1  (int, int, int, int, int);
int    ddr_gl_dcm_dith_expand  (int, int);
ddr_gl_dcm_dith_color_table*
       ddr_gl_dcm_dith_make_color_table  (void);
int    ddr_gl_dcm_dith_merge_rgb  (int, int, int);
void   ddr_gl_dcm_flush  (void);
DtFlag ddr_gl_dcm_initialize_device  (DtObject, DtPtr, DtPtr, DtPtr);
void   ddr_gl_dcm_inquire_auto_size  (DtFlag *);
void   ddr_gl_dcm_inquire_clip_list  (DtUInt*, DtReal**, DtReal[4], DtFlag*);
void   ddr_gl_dcm_inquire_color_entries  (DtColorModel, int, int, DtReal *);
void   ddr_gl_dcm_inquire_device_extent  (DtVolume *);
void   ddr_gl_dcm_inquire_ncolors  (DtInt *);
DtFlag ddr_gl_dcm_inquire_pixel_data (
	    DtRasterType, DtInt*, DtInt*, DtRasterType*, DtPtr*, DtFlag*);
void   ddr_gl_dcm_inquire_resolution  (DtReal *, DtReal *);
void   ddr_gl_dcm_inquire_stereo  (DtFlag *);
void   ddr_gl_dcm_inquire_visual_type  (DtVisualType *);
void   ddr_gl_dcm_set_background_color  (DtColorModel, DtReal *);
void   ddr_gl_dcm_set_color_entries  (DtColorModel, int, int, DtReal *);
void   ddr_gl_dcm_set_current_view  (DtObject);
void   ddr_gl_dcm_set_depth_buffer_enable  (DtFlag);
void   ddr_gl_dcm_set_depth_buffer_write  (DtFlag);
void   ddr_gl_dcm_set_foreground_color  (DtColorModel, DtReal *);
void   ddr_gl_dcm_set_options  (DtPtr);
void   ddr_gl_dcm_set_shade_index  (DtInt);
void   ddr_gl_dcm_swap_buffers  (void);
void   ddr_gl_dcm_update_geometry  (void);
void   ddr_gl_dcm_write_scanline_byte  (DtInt, DtInt, DtInt, DtUChar *);
void   ddr_gl_drom_apply_att_clpvol  (DtClipOperator, DtInt, DtHalfSpace *);
void   ddr_gl_drom_camera  (void);
DtPtr  ddr_gl_drom_create_local_device_data  (DtObject);
DtPtr  ddr_gl_drom_create_local_view_data  (DtObject);
DtPtr  ddr_gl_drom_create_local_window_data  (DtObject, DtObject);
void   ddr_gl_drom_delete_local_device_data  (DtPtr);
void   ddr_gl_drom_delete_local_view_data  (DtPtr);
void   ddr_gl_drom_delete_local_window_data  (DtPtr);
DtPtr  ddr_gl_drom_filerstr_create  (DtObject);
void   ddr_gl_drom_filerstr_delete  (DtObject, DtObjectStructure *);
void   ddr_gl_drom_filerstr_print  (DtObject, DtObjectStructure *);
void   ddr_gl_drom_filerstr_update  (DtObjectStructure *);
void   ddr_gl_drom_get_frame_to_view_matrix  (void);
void   ddr_gl_drom_get_lcstowcsmat  (DtMatrix4x4);
void   ddr_gl_drom_get_wcstofcsmat  (DtMatrix4x4);
void   ddr_gl_drom_light  (DtObject);
void   ddr_gl_drom_lighting  (void);
void   ddr_gl_drom_new_class  (DtPtr, DtInt);
void   ddr_gl_drom_pop_att_clpvol  (void);
void   ddr_gl_drom_pop_lcstofcsmat  (void);
void   ddr_gl_drom_post_initialization  (void);
void   ddr_gl_drom_pre_initialization  (void);
DtPtr  ddr_gl_drom_primsurf_create  (DtObjectStructure *);
void   ddr_gl_drom_primsurf_delete  (DtObjectStructure *, DtPtr);
void   ddr_gl_drom_primsurf_print  (DtObjectStructure *, DtPtr);
void   ddr_gl_drom_primsurf_render  (dot_object *);
void   ddr_gl_drom_push_att_clpvol  (void);
void   ddr_gl_drom_push_lcstofcsmat  (DtMatrix4x4);
void   ddr_gl_drom_render_connected_line_list (
	    DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtRealTriple*,
	    DtRealTriple*, DtRealTriple*, DtInt, DtRealCouple**, DtInt,
	    DtRealTriple**, DtReal*);
void   ddr_gl_drom_render_line_list (
	    DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtRealTriple*,
	    DtRealTriple*, DtRealTriple*, DtInt, DtRealCouple**, DtInt,
	    DtRealTriple**, DtReal*);
void   ddr_gl_drom_render_point_list (
	    DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtRealTriple*,
	    DtRealTriple*, DtInt, DtRealCouple**, DtInt, DtRealTriple**,
	    DtReal*);
void   ddr_gl_drom_render_triangle_list (
	    DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtRealTriple*,
	    DtRealTriple*, DtRealTriple*, DtInt, DtRealCouple*[], DtInt,
	    DtRealTriple**, DtReal*);
void   ddr_gl_drom_render_triangle_mesh (
	    DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtInt,
	    DtRealTriple*, DtRealTriple*, DtRealTriple*, DtInt[][3], DtInt[][3],
	    DtInt, DtRealCouple**, DtInt, DtRealTriple**, DtReal*);
void   ddr_gl_drom_reset_lights  (void);
ddt_gl_drom_rstr*
       ddr_gl_drom_rstr_create  (DtObject);
void   ddr_gl_drom_rstr_delete  (DtObject, ddt_gl_drom_rstr *);
void   ddr_gl_drom_rstr_print  (DtObject, ddt_gl_drom_rstr *);
void   ddr_gl_drom_rstr_tex_update  (DtObjectStructure *);
void   ddr_gl_drom_rstr_update  (DtObjectStructure *);
void   ddr_gl_drom_set_att_ambint  (DtReal);
void   ddr_gl_drom_set_att_ambswi  (DtSwitch);
void   ddr_gl_drom_set_att_bacfacculble  (DtSwitch);
void   ddr_gl_drom_set_att_bacfacculswi  (DtSwitch);
void   ddr_gl_drom_set_att_clpswi  (DtSwitch);
void   ddr_gl_drom_set_att_clpvol  (DtClipOperator, DtInt, DtHalfSpace *);
void   ddr_gl_drom_set_att_depcue (
	    DtReal, DtReal, DtReal, DtReal, DtColorModel, DtReal*);
void   ddr_gl_drom_set_att_depcueswi  (DtSwitch);
void   ddr_gl_drom_set_att_difclr  (DtColorModel, DtReal []);
void   ddr_gl_drom_set_att_difint  (DtReal);
void   ddr_gl_drom_set_att_difswi  (DtSwitch);
void   ddr_gl_drom_set_att_hidsrfswi  (DtSwitch);
void   ddr_gl_drom_set_att_inttyp  (DtInterpType);
void   ddr_gl_drom_set_att_lgtatn  (DtReal, DtReal);
void   ddr_gl_drom_set_att_lgtclr  (DtColorModel, DtReal []);
void   ddr_gl_drom_set_att_lgtint  (DtReal);
void   ddr_gl_drom_set_att_lgtspdang  (DtReal, DtReal);
void   ddr_gl_drom_set_att_lgtspdexp  (DtReal);
void   ddr_gl_drom_set_att_lgtswi  (DtInt, DtObject *);
void   ddr_gl_drom_set_att_lgttyp  (DtObject);
void   ddr_gl_drom_set_att_lintyp  (DtLineType);
void   ddr_gl_drom_set_att_linwid  (DtReal);
void   ddr_gl_drom_set_att_localaasty  (DtLocalAntiAliasStyle);
void   ddr_gl_drom_set_att_localaaswi  (DtSwitch);
void   ddr_gl_drom_set_att_mapbmp (
	    DtInt, DtObject[], DtObject[], dot_stdtexatt[]);
void   ddr_gl_drom_set_att_mapbmpswi  (DtSwitch);
void   ddr_gl_drom_set_att_mapdifclr (
	    DtInt, DtObject[], DtObjectStructure*[], dot_stdtexatt[]);
void   ddr_gl_drom_set_att_mapdifclrswi  (DtSwitch);
void   ddr_gl_drom_set_att_mapenv (
	    DtInt, DtObject[], DtObject[], dot_stdtexatt[]);
void   ddr_gl_drom_set_att_mapenvswi  (DtSwitch);
void   ddr_gl_drom_set_att_maptrnint (
	    DtInt, DtObject[], DtObject[], dot_stdtexatt[]);
void   ddr_gl_drom_set_att_maptrnintswi  (DtSwitch);
void   ddr_gl_drom_set_att_refswi  (DtSwitch);
void   ddr_gl_drom_set_att_reptyp  (DtRepType);
void   ddr_gl_drom_set_att_shaswi  (DtSwitch);
void   ddr_gl_drom_set_att_shdidx  (DtInt);
void   ddr_gl_drom_set_att_spcclr  (DtColorModel, DtReal []);
void   ddr_gl_drom_set_att_spcfct  (DtReal);
void   ddr_gl_drom_set_att_spcint  (DtReal);
void   ddr_gl_drom_set_att_spcswi  (DtSwitch);
void   ddr_gl_drom_set_att_srfedgclr  (DtColorModel, DtReal []);
void   ddr_gl_drom_set_att_srfshd  (DtObject);
void   ddr_gl_drom_set_att_stereo  (DtReal, DtReal);
void   ddr_gl_drom_set_att_stereoswi  (DtSwitch);
void   ddr_gl_drom_set_att_transpclr  (DtColorModel, DtReal []);
void   ddr_gl_drom_set_att_transpint  (DtReal);
void   ddr_gl_drom_set_att_transpswi  (DtSwitch);
void   ddr_gl_drom_set_camera_matrix  (DtMatrix4x4);
void   ddr_gl_drom_set_parallel_matrix  (DtReal, DtReal, DtReal);
void   ddr_gl_drom_set_perspective_matrix  (DtReal, DtReal, DtReal);
void   ddr_gl_drom_set_projection_matrix (
	    DtArea*, DtProjectionType, DtPoint3, DtReal, DtReal, DtReal);
DtPtr  ddr_gl_drom_spherelist_create  (DtObjectStructure *);
void   ddr_gl_drom_spherelist_delete  (DtObjectStructure *, DtPtr);
void   ddr_gl_drom_spherelist_print  (DtObjectStructure *, DtPtr);
void   ddr_gl_drom_spherelist_render  (dot_object *);
void   ddr_gl_drom_start_update  (DtObject, DtObject, DtPtr, DtPtr, DtPtr);
void   ddr_gl_drom_transform_by_matrix  (DtRealTriple, Matrix, DtRealTriple);
DtFlag ddr_gl_drom_transform_clip_z_point (DtRealTriple, DtMatrix4x4);
void   ddr_gl_drom_update_background  (void);
void   ddr_gl_drom_update_display  (DtPFI);
void   ddr_gl_drom_update_display_light  (light_data *, int);
void   ddr_gl_drom_update_display_lights  (void);
void   ddr_gl_drom_update_lcstowcsmat_lokatfrm (
	    DtPoint3, DtPoint3, DtVector3, DtCompType);
void   ddr_gl_drom_update_lcstowcsmat_pop  (void);
void   ddr_gl_drom_update_lcstowcsmat_push  (void);
void   ddr_gl_drom_update_lcstowcsmat_rotate  (DtAxis, DtReal, DtCompType);
void   ddr_gl_drom_update_lcstowcsmat_scale (DtReal,DtReal,DtReal, DtCompType);
void   ddr_gl_drom_update_lcstowcsmat_shear (
	    DtMajorPlane, DtReal, DtReal, DtCompType);
void   ddr_gl_drom_update_lcstowcsmat_tfmmat  (DtMatrix4x4, DtCompType);
void   ddr_gl_drom_update_lcstowcsmat_transl (DtReal,DtReal,DtReal, DtCompType);
void   ddr_gl_drom_update_local_data  (void);
void   ddr_gl_drom_update_studio  (DtPFI);
void   ddr_gl_prom_write_rectangle_byte_rgb (DtInt,DtInt,DtInt,DtInt, DtUChar*);
DtPtr  ddr_gl_return_DCM_fcns  (void);
DtPtr  ddr_gl_return_DROM_fcns  (void);
DtPtr  ddr_gl_return_DROM_methods  (void);
DtPtr  ddr_gl_return_PROM_fcns  (void);
void   ddr_gl_return_functions  (DtInt, DtPtr *);
void   ddr_iris_id_matrix  (Matrix);
void   ddr_iris_init_patterns  (void);
void   ddr_iris_loadmatrix_id  (void);
void   ddr_iris_restore_depcue  (void);
void   ddr_iris_restore_diffuse_color  (void);
void   ddr_iris_restore_specular_color  (void);
void   ddr_iris_restore_specular_factor  (void);
void   ddr_iris_restore_transparency  (void);
void   ddr_iris_set_depcue_color  (DtReal[3]);
void   ddr_iris_set_diffuse_color  (DtReal[3]);
void   ddr_iris_to_dore_matrix  (Matrix, DtMatrix4x4);
void   ddr_print_current_matrix  (void);
void   ddr_set_iris_material_color_range  (int, int);

#endif
