/* vi:set ts=8 sw=4 ai:
 *[
 *$     global.h
 *
 *      Steve Gray - Fri Jul  7 15:53:14 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	MLR Dore conversion globals
 *
 *      Usage:
 *		#define	MLR_MAIN	declare globals
 *		#include "global.h"
 *
 *		#include "global.h"	reference globals
 *
 *      Notes:
 *
 *
 *?
 *!     Changes History
 *
 *]
 */


#undef	GLOBAL

#ifdef	MLRD_MAIN
#define	GLOBAL
#else
#define	GLOBAL	extern
#endif


/* this is needed for the Dore_texture_map type */
#include "dore/dore_develop/private/stdtexatt.h"
#include "dore/dore_develop/error.h"
#include "dore/dore_develop/develop.h"

/* max number of lights */
#define	LIT_MAX	500



/* material coefficient IDs */
#define	MAT_COEFF_AMB		0
#define	MAT_COEFF_DIFF		1
#define	MAT_COEFF_SPEC		2


/* Dore related control info */
typedef struct {
	DtObjectStructure *device;
	DtObjectStructure *view;
	DDt_PROM	*PROM_fcns;
	DtInt		studio_traversal;
	DtInt		display_traversal;
	DtInt		view_index;
	DtInt		device_index;
	DtInt		render_id;
} Dore_controls;


/* Media Logic conversion files */
typedef struct {
	Char		file_prefix[256];

	Char		render_file[256];
	FILE		*render_fp;

	Char		camera_file[256];
	FILE		*camera_fp;

	Char		light_file[256];
	FILE		*light_fp;

	Char		limits_file[256];
	FILE		*limits_fp;

	Char		object_file[256];
	FILE		*object_fp;

	Char		rle_file[256];
} Render_files;


/* limit counters */
typedef struct {
	Int		cp;
	Int		tex[2];
	Int		tri[2];
	Int		line[2];
	Int		ma;
	Int		li;
	Int		obj[2];
	Int		vox;
} Render_limits;


/* type defining a Dore map set */
typedef struct {
	Int		num_textures;
	DtObject	mapping[TMAP_MAX_LEVELS];
	DtObject	raster[TMAP_MAX_LEVELS];
	dot_stdtexatt	attributes[TMAP_MAX_LEVELS];
	DtInt		attribute_sizes[TMAP_MAX_LEVELS];
} Dore_texture_map;


GLOBAL Dore_controls	mlr_control;
GLOBAL Render_files	mlr_files;
GLOBAL Render_limits	mlr_limits;

GLOBAL Simple_rgb	mlr_global_ambient;

GLOBAL Int		mlr_sample_level;


/* execution modes */
#define	MLR_MODE_FILE		1	/* produce data files */
#define	MLR_MODE_DIRECT		2	/* direct access to renderer */
#define	MLR_MODE_DIRECT_COUNT	3	/* count phase of direct mode */

GLOBAL Int		mlr_mode;


/* turn if file mode render should automatically execute */
GLOBAL Int		mlr_auto_render;



#ifdef	MLRD_MAIN

DtMatrix4x4	right_to_left_mat = { {  1.0,  0.0,  0.0,  0.0 },
			  	      {  0.0,  1.0,  0.0,  0.0 },
			  	      {  0.0,  0.0, -1.0,  0.0 },
			  	      {  0.0,  0.0,  0.0,  1.0 } };

#else

extern DtMatrix4x4	right_to_left_mat;

#endif



