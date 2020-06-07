
/* vi:set ts=8 sw=4 ai:
 *[
 *$     GEOM_init.c
 *
 *      Steve Gray - Wed Jul 19 10:43:52 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	initialize GEOM module
 *
 *      Usage:
 *
 *      Notes:
 *
 *
 *?
 *!     Changes History
 *
 *]
 */





/* system includes */
#include <stdio.h>

/* dore includes */
#include "dore.h"

/* Render includes */
#include "primitives.h"

/* MLR Dore interface includes */
#include "global.h"
#include "geom.h"



/*[
 *$     GEOM_init
 *
 *      Steve Gray - Mon Nov 13 16:24:35 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    initialize GEOM module
 *
 *      Usage:
 *		GEOM_init()
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

GEOM_init()
{
    geom_index = 0;
}



