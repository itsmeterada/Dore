/* vi:set ts=8 sw=4 ai:
 *[
 *$     cam.h
 *
 *      Steve Gray - Mon Jul 17 17:12:00 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	camera module globals, types, and constants
 *
 *      Usage:
 *		#define	CAM_MAIN	declare globals
 *		#include "cam.h"
 *
 *		#include "cam.h"	reference globals
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

#ifdef	CAM_MAIN
#define	GLOBAL
#else
#define	GLOBAL	extern
#endif


/* camera types */
#define	DORE_CAMERA_PERSPECTIVE		0
#define	DORE_CAMERA_PROJECTION		1
#define	DORE_CAMERA_PARALLEL		2

typedef struct {
	Int	type;

	Float	fov;		/* for perspective only */
	Float	focal_lth;	/* for projection only */
	Float	size;		/* for parallel only */

	Float	left, right, bottom, top;	/* for projection only */

	Float	hither;
	Float	yon;
} Camera;


GLOBAL Camera	camera;





