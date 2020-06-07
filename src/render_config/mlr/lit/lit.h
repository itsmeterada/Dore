/* vi:set ts=8 sw=4 ai:
 *[
 *$     lit.h
 *
 *      Steve Gray - 
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	light module constants, types, and globals
 *
 *      Usage:
 *		#define	LIT_MAIN	declare globals
 *		#include "lit.h"
 *
 *		#include "lit.h"	reference globals
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

#ifdef	LIT_MAIN
#define	GLOBAL
#else
#define	GLOBAL	extern
#endif


/* light list */
typedef struct {
	DtObject	dore_object;	/* Dore light ptr */
	Int		on;		/* if light is on/off */
} Light_map;



GLOBAL Light_map	lit_info[LIT_MAX];
GLOBAL Int		lit_count;

