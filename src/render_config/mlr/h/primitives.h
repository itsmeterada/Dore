/* vi:set ts=8 sw=4 ai:
 *[
 *$     primitives.h
 *
 *      Steve Gray - Mon Mar 19 13:31:06 PST 1990
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	primitive type definitions 
 *
 *      Usage:
 *		#include "primitives.h"
 *
 *      Notes:
 *		this file contains types and defines extracted from the
 *		Renderer "primitives.h" and "processor.h"
 *
 *		if the direct interface is being used (i.e. the Render
 *		library is linked into Dore) YOU MUST HAVE THE SAME
 *		DEFINITIONS FOR THESE TYPES.  Of special interest should
 *		be the type "Float" which can either be "float" or "double".
 *		If it is wrong, the system will obviously not work
 *
 *?
 *!     Changes History
 *
 *]
 */


/* ------------------------ from h/processor.h ------------------------------ */

double	sqrt();
#define	SQRT(xx)	sqrt( (double) (xx))


#define	Char		char
#define	Short		short
#define	Int		int
#define	Float		float		/* MAKE SURE THIS MATCHES RENDERER */
#define	Double		double


/* ----------------------- from main/h/command.h ---------------------------- */

/* limit types for limit command */
#define	LIMIT_CP	1		/* set cp limits */
#define	LIMIT_TEX	2		/* set texture cp limits */
#define LIMIT_TRI	3		/* set triangle limit */
#define LIMIT_SPH	4		/* set triangle limit */
#define	LIMIT_LI	5		/* set light limit */
#define	LIMIT_MA	6		/* set material limit */
#define	LIMIT_OBJ	7		/* object and light assignment limits */
#define	LIMIT_VOX	8		/* voxel subdivision limits */
#define	LIMIT_VDA	9		/* VDA surface limits */
#define	LIMIT_LINE	10		/* line limits */


/* ma (material) command argument */
#define	MA_BEGIN	1		/* begin material */
#define	MA_END		2		/* end material */


/* --------------------------- h/primitives.h ------------------------------- */

/* magic numbers for the different file types */
#define	ART_MAGIC_NUMBER		(0xdeb88bed)
#define	TMAP_MAGIC_NUMBER		(0x45533554)
#define	RMAP_MAGIC_NUMBER		(0x53454553)
#define	BINARY_FILE_MAGIC_NUMBER	(0xcdbddbdc)

#define ABEL_MAGIC_NUMBER      		(0164200)


/* define MAXFLOAT is its not defined */
#ifndef	MAXFLOAT
#define	MAXFLOAT	HUGE
#endif

/* something is wrong with "HUGE" on the HP */
#ifdef	HP300
#undef	HUGE
#define	HUGE	100000000000.0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


/* define some values for PI */
#ifndef	M_PI
#define	M_PI		3.141592
#define	M_PI_2		1.570796
#endif

#define	M_2X_PI_INV	.1591549

#define	M_2X_PI		(2.0 * M_PI)


/* output file iamge types */
#define	FORMAT_SPECIAL		0	/* user specified format */

#define	FORMAT_35mm		1	/* 35mm film format */
#define	FORMAT_1K		2	/* 35mm film format */
#define	FORMAT_PANAVISION	3	/* Panavision film format */
#define	FORMAT_NTSC		4	/* NTSC (US TV) format */
#define	FORMAT_PAL		5	/* PAL (European format) */

/* shading modes, DO NOT RE-ORDER THESE CONSTANTS!!!! */
#define	SHADE_MODE_FRONT	0	/* shade only front faces */

#define	SHADE_MODE_BOTH		1	/* shade both sides, but don't flip
					   interpolated normals unless the
					   triangle itself needs flipping */

#define	SHADE_MODE_ALWAYS	2	/* always flip normals */


/* light model active flags */
#define LMODFLAG_AMB      0x0001        /* use ambient color */
#define LMODFLAG_SHAD     0x0006        /* use light colors */
#define LMODFLAG_LIGHT    0x0008        /* use light colors */
#define LMODFLAG_REFL     0x0030        /* use reflected ray color */
#define LMODFLAG_DISS     0x00c0        /* use dissolve factor */
#define LMODFLAG_REFR     0x0700        /* use refracted ray color */
#define LMODFLAG_TRANSP   0x1800        /* use refracted ray color */
#define LMODFLAG_USER1    0x2000        /* user define model 1 */
#define LMODFLAG_USER2    0x4000        /* user define model 2 */
#define LMODFLAG_DECAY    0x8000        /* use decay of ray color */

/* texture map parameter replacement mask values */
#define TEXFLAG_AMB     	0x00000001   /* map ambient color */
#define TEXFLAG_DIFF    	0x00000002   /* map diffuse color */
#define TEXFLAG_SPEC    	0x00000004   /* map specular color */
#define TEXFLAG_EXP     	0x00000008   /* map specular exponent */
#define TEXFLAG_REFL    	0x00000010   /* map reflective color */
#define TEXFLAG_REFR    	0x00000020   /* map refractive color */
#define TEXFLAG_SHAD    	0x00000040   /* map shadow color */
#define TEXFLAG_DISS  		0x00000080   /* map dissolve color */
#define TEXFLAG_DISS_EXP	0x00000100   /* map dissolve exponent */
#define TEXFLAG_RFLCLR  	0x00000200   /* map reflection map color */
#define TEXFLAG_RFRCLR  	0x00000400   /* map refraction map color */
#define TEXFLAG_RFRIDX  	0x00000800   /* map index of refraction */
#define TEXFLAG_DECAY   	0x00001000   /* map decay factor */
#define TEXFLAG_FADE    	0x00002000   /* map fade color */
#define TEXFLAG_OPACITY    	0x00004000   /* map opacity color */
#define TEXFLAG_OPACITY_EXP    	0x00008000   /* map opacity exponent */
#define TEXFLAG_TRANSP    	0x00010000   /* map opacity color */
#define TEXFLAG_TRANSP_INTENS  	0x00020000   /* map opacity color */
#define TEXFLAG_TRANSP_EXP    	0x00040000   /* map opacity exponent */

#define	TEXFLAG_RFLMAP		0x00080000   /* reflection map */
#define TEXFLAG_RFRMAP		0x00100000   /* refraction map */

#define TEXFLAG_BUMP    	0x00200000   /* perturb normal (bump mapping) */


/* file  types */
#define	FILE_TYPE_NULL		0
#define	FILE_TYPE_ART		1
#define	FILE_TYPE_ART_RLE	2
#define	FILE_TYPE_TMAP		3
#define	FILE_TYPE_RMAP		4
#define	FILE_TYPE_ABEL		5
#define	FILE_TYPE_ABEL_CMAP	6
#define	FILE_TYPE_ABEL_RMAP	7
#define	FILE_TYPE_DORE		8

/* map types */
#define	TMAP_TYPE_REPLACE	1	/* replace material color */
#define	TMAP_TYPE_MULTIPLY	2	/* multiply by material color */
#define	TMAP_TYPE_ADD		3	/* add to material color */
#define	TMAP_TYPE_COMPOSITE	4	/* composite with material color */


/* map wrapping modes */
#define	TMAP_WRAP_NONE		0x00	/* do not wrap in either U or V */
#define	TMAP_WRAP_U		0x01	/* wrap only in U */
#define	TMAP_WRAP_V		0x02	/* wrap only in V */
#define	TMAP_WRAP_UV		0x03	/* wrap both U & V */


/* anti-aliasing types */
#define	TMAP_AA_NONE		1	/* no anti-aliasing */
#define	TMAP_AA_MIP		2	/* mip-mapping */
#define	TMAP_AA_ADAPTIVE	3	/* adaptive anti-aliasing */


/* uv coord clipping styles */
#define	TMAP_EXTEND_REPEAT	0x00	/* UV coords wrap */
#define	TMAP_EXTEND_NONE	0x11	/* no map outside of 0..1 */
#define	TMAP_EXTEND_BLACK	0x22	/* map is black outside of 0..1 */
#define	TMAP_EXTEND_CLAMP	0x33	/* clamp values to 0 or 1 */

#define	TMAP_EXTEND_REPEAT_U	0x00	/* U coord wraps */
#define	TMAP_EXTEND_NONE_U	0x01	/* no map outside of U = 0..1 */
#define	TMAP_EXTEND_BLACK_U	0x02	/* map is black outside of U = 0..1 */
#define	TMAP_EXTEND_CLAMP_U	0x03	/* clamp U value to 0 or 1 */

#define	TMAP_EXTEND_REPEAT_V	0x00	/* V coord wraps */
#define	TMAP_EXTEND_NONE_V	0x10	/* no map outside of V = 0..1 */
#define	TMAP_EXTEND_BLACK_V	0x20	/* map is black outside of V = 0..1 */
#define	TMAP_EXTEND_CLAMP_V	0x30	/* clamp V value to 0 or 1 */

#define	TMAP_EXTEND_MASK_U	0x0f	/* get U extend mode */
#define	TMAP_EXTEND_MASK_V	0xf0	/* get V extend mode */


/* max levels of texture maps on a given property */
#define	TMAP_MAX_LEVELS		5


/* object usage mask value */
#define	USE_IN_PRIMARY		0x01	/* use object only for primary rays */
#define USE_IN_SECONDARY	0x02	/* use object only for secondary rays */
#define	USE_IN_BOTH		0x03	/* use object for both types of rays */
 

/* Interpolation mask value */
#define	INTERP_NONE		0x00	/* Interpolate no parameters */
#define INTERP_MATERIAL		0x01	/* Interpolation material params */
#define INTERP_SHADING		0x02	/* Interpolation shading params */
#define INTERP_BOTH		0x03	/* Interpolation both params */
 

/* camera projection types */
#define PROJ_ORTHO      1               /* orthographic */
#define PROJ_PERSP      2               /* perspective */


/* matte types */
#define	MATTE_TYPE_ABEL		1	/* abel style mattes */
#define	MATTE_TYPE_LUCAS	2	/* lucas film style mattes */
#define	MATTE_TYPE_ABEL_TRACE	3	/* abel matte for traced pixels */


/* simple (single) rgb color */
typedef struct {
	Float	r;
	Float	g;
	Float	b;
} Simple_rgb;

/* simple (single) rgb + alpha color */
typedef struct {
	Float	r;
	Float	g;
	Float	b;
	Float	a;
} Simple_rgba;

/* simple (single) 4d coord */
typedef struct {
	Float	x;
	Float	y;
	Float	z;
	Float	w;
} Simple_coord_4d;

/* simple (single) 3d coord */
typedef struct {
	Float	x;
	Float	y;
	Float	z;
} Simple_coord_3d;

/* simple (single) 2d coord */
typedef struct {
	Float	u;
	Float	v;
} Simple_coord_2d;

/* simple (single) double precision 3d coord */
typedef struct {
	Double	x;
	Double	y;
	Double	z;
} Simple_double_coord_3d;

/* simple (single) double precision 2d coord */
typedef struct {
	Double	u;
	Double	v;
} Simple_double_coord_2d;

/* simple (single) Integer 3d coordinate */
typedef struct {
	Int	x;
	Int	y;
	Int	z;
} Simple_Int_coord_3d;

/* simple (single) Short 3d coordinate */
typedef struct {
	Short	x;
	Short	y;
	Short	z;
} Simple_Short_coord_3d;

/* time Interval */
typedef struct {
	Float	t_min;
	Float	t_max;
} Time_Interval;

/* 4 by 4 matrix */
typedef Float	Matrix_4[4][4];

/* 3 by 3 matrix */
typedef Float	Matrix_3[3][3];

/* 2 by 2 matrix */
typedef Float	Matrix_2[2][2];

