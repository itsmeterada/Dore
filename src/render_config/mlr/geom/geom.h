/* vi:set ts=8 sw=4 ai:
 *[
 *$     geom.h
 *
 *      Steve Gray - Mon Nov 13 16:22:08 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	GEOM module constants, types, and globals
 *
 *      Usage:
 *		#define	GEOM_MAIN		declare globals
 *		#include "geom.h"
 *
 *		#include "geom.h"		reference globals
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

#ifdef	GEOM_MAIN
#define	GLOBAL
#else
#define	GLOBAL	extern
#endif



/* current geometry index */
GLOBAL Int	geom_index;



