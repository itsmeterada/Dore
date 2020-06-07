
/* vi:set ts=8 sw=4 ai:
 *[
 *$     mat.h
 *
 *      Steve Gray - Mon Jul 17 13:43:45 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	types, constants, and globals for MAT module
 *
 *      Usage:
 *		#define	MATD_MAIN	declare globals
 *		#include "matd.h"
 *
 *		#include "matd.h"	reference globals
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

#ifdef	MATD_MAIN
#define	GLOBAL
#else
#define	GLOBAL	extern
#endif


/* material definition */
typedef struct {
	Int		lmod;

	Simple_rgb	amb;
	Simple_rgb	diff;
	Simple_rgb	spec;
	Float		exponent;

	Simple_rgb	rflclr;
	Int		rflmap_present;

	Simple_rgb	rfrclr;
	Int		rfrmap_present;

	Simple_rgb	refl;
	Float		rfridx;

	Float		center_transp_intens;
	Simple_rgb	center_transp;
	Float		edge_transp_intens;
	Simple_rgb	edge_transp;
	Float		transp_exp;

	Simple_rgb	shad;
} Material;


/* Dore material attributes */
typedef struct {
	Int		amb_switch;
	Float		amb_intensity;

	Int		diff_switch;
	Float		diff_intensity;
	Simple_rgb	diff_color;

	Int		spec_switch;
	Float		spec_intensity;
	Simple_rgb	spec_color;
	Float		spec_exponent;

	Int		transp_switch;
	Simple_rgb	transp_color;
	Float		transp_intensity;

	Int		transp_orient_switch;
	Simple_rgb	transp_orient_color;
	Float		transp_orient_intensity;
	Float		transp_exp;

	Int		refl_switch;
	Int		shad_switch;

	Int		refr_switch;
	Float		refr_index;
} Dore_material;


typedef struct {
	Char		map_name[256];
	DtObject	dore_object;
	Int		type;
} Mat_mapfile;


/* file types */
#define	MAT_MAP_TYPE_DORE	0		/* a DORE raster file */
#define	MAT_MAP_TYPE_ART	1		/* an ART file */
#define	MAT_MAP_TYPE_RLE	2		/* an ART RLE file */
#define	MAT_MAP_TYPE_TMAP	3		/* MLR TMAP file */
#define	MAT_MAP_TYPE_RMAP	4		/* MLR RMAP file */


/* current material parameters */
GLOBAL Material		matd_curr;
GLOBAL Dore_material	matd_dore;

/* global light model so far */
GLOBAL Int	matd_lmod;


/* global map file count */
GLOBAL Int	matd_map_count;

#define	MAT_MAX_MAPFILES	50
GLOBAL Mat_mapfile	matd_map_file[MAT_MAX_MAPFILES];

