/* vi:set ts=8 sw=4 ai:
 *[
 *$     method.h
 *
 *      Steve Gray - Wed Jul 19 12:47:51 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	Dore render methods
 *
 *      Usage:
 *		#define	MLRD_MAIN	declared globals
 *		#include "method.h"
 *
 *		#include "method.h"	reference globals
 *
 *      Notes:
 *
 *
 *?
 *!     Changes History
 *
 *]
 */


Int	GEOM_tri_list();
Int	GEOM_tri_mesh();
Int	GEOM_line_list();
Int	GEOM_line_poly();
Int	GEOM_point_list();
Int	GEOM_vartri_mesh();
Int	GEOM_varline_list();
Int	GEOM_varpoint_list();

Int	LIT_make();
Int	CAM_make();
Int	CAM_make_matrix();
Int	CAM_make_perspective();
Int	CAM_make_projection();
Int	CAM_make_parallel();

typedef struct {
	DtPtr	name;
	DtPFI	routine;
} Method;

#ifdef	MLRD_MAIN

Method display_method[] = {
		"DoTriangleList",	GEOM_tri_list,
		"DoTriangleMesh",	GEOM_tri_mesh,
		"DoLineList",		GEOM_line_list,
		"DoPolyline",		GEOM_line_poly,
		"DoPointList",		GEOM_point_list,
		"DoVarTriangleMesh",	GEOM_vartri_mesh,
		"DoVarLineList",	GEOM_varline_list,
		"DoVarPointList",	GEOM_varpoint_list,
		NULL,			NULL
    	    };

Method studio_method[] = {
		"DoLight",		LIT_make,
		"DoCamera",		CAM_make,
		"DoCameraMatrix",	CAM_make_matrix,
		"DoPerspective",	CAM_make_perspective,
		"DoProjection",		CAM_make_projection,
		"DoParallel",		CAM_make_parallel,
		NULL,			NULL
    	    };

#else

extern Method display_method[];
extern Method studio_method[];

#endif

