/* vi:set ts=8 sw=4 ai:
 *[
 *$     geom_xfm.c
 *
 *      Steve Gray - Mon Jul 17 14:41:55 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	transform geometry
 *
 *      Usage:
 *		geom_xfm_cp()		convert matrix to MLR format
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

/* MLR Dore interface includes */
#include "global.h"



/*[
 *$     geom_xfm_cp
 *
 *      Steve Gray - Thu Oct  5 10:45:44 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    convert matrix from Dore format to Media Logic
 *			format and call MLR_mat to set current CP matrix
 *
 *      Usage:
 *		geom_xfm_cp( mat)
 *		DtMatrix4x4	mat
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

geom_xfm_cp( mat)
DtMatrix4x4	mat;
{
Matrix_4	mlr_mat;
Int		i, j;


    for ( i = 0; i < 4; i++ )
    {
	for ( j = 0; j < 4; j++ )
	{
	    mlr_mat[j][i] = mat[i][j];
	}
    }

#ifdef	RENDER_DIRECT
    MLR_mat( mlr_mat);
#endif
}



