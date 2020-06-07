
/* vi:set ts=8 sw=4 ai:
 *[
 *$     MATH_matrix.c
 *
 *      Steve Gray - Fri Jul  7 15:47:42 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	matrix math
 *
 *      Usage:
 *		MATH_matrix_r_to_l()	make right handed xfm at left handed one
 *
 *      Notes:
 *
 *
 *?
 *!     Changes History
 *
 *]
 */



/* #define	DEBUG */

/* system includes */
#include <stdio.h>

/* dore includes */
#include "dore.h"

/* Render includes */
#include "primitives.h"

/* MLRD Dore interface includes */
#include "global.h"



/*[
 *$     MATH_matrix_r_to_l
 *
 *      Steve Gray - Fri Jul  7 15:50:57 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    convert right handled xfm to left handled
 *
 *      Usage:
 *		MATH_matrix_r_to_l( in, out)
 *		DtMatrix4x4	in, out
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MATH_matrix_r_to_l()
{
static conv_mat[4][4] = { { -1.0, 0.0, 0.0, 0.0 },
			  {  0.0, 0.0, 1.0, 0.0 },
			  {  0.0, 1.0, 0.0, 0.0 },
			  {  0.0, 0.0, 0.0, 1.0 } };
}





