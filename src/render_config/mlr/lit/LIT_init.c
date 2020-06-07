/* vi:set ts=8 sw=4 ai:
 *[
 *$     LIT_init.c
 *
 *      Steve Gray - Sun Jul  9 11:44:23 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	light initializations
 *
 *      Usage:
 *		LIT_init()	init light conversion
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



/*[
 *$     LIT_init
 *
 *      Steve Gray - Mon Nov 13 15:58:13 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    initialize light database
 *
 *      Usage:
 *		LIT_init()
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

LIT_init()
{
    LIT_list_init();
}





