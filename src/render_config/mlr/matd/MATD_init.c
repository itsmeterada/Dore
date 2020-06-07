/* vi:set ts=8 sw=4 ai:
 *[
 *$     MATD_init.c
 *
 *      Steve Gray - Mon Jul 17 13:48:26 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	initialize mat module
 *
 *      Usage:
 *		MATD_init()
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
#include "matd.h"


/*[
 *$     MATD_init
 *
 *      Steve Gray - Mon Jul 17 13:49:11 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    initialize mat module
 *
 *      Usage:
 *		MATD_init()
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MATD_init()
{
/* initialize global light model */
    matd_lmod = 0;

/* initialize map list */
    MATD_map_init();
}



