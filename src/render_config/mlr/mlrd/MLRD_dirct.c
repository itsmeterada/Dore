/* vi:set ts=8 sw=4 ai:
 *[
 *$     MLRD_direct.c
 *
 *      Steve Gray - Mon Aug  7 14:59:18 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	handle direct mode rendering
 *
 *      Usage:
 *		MLRD_direct_limits()	set limits
 *		MLRD_direct_exec()	execute the render
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
#include "method.h"



/*[
 *$     MLRD_direct_limits
 *
 *      Steve Gray - Mon Aug  7 15:00:14 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    set limits
 *
 *      Usage:
 *		MLRD_direct_limits()
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MLRD_direct_limits()
{
#ifdef	DEBUG
printf( "MLRD_direct_limits\n");
printf( "cp %d\n", mlr_limits.cp);
printf( "tex %d %d\n", mlr_limits.tex[0], mlr_limits.tex[1]);
printf( "tri %d %d\n", mlr_limits.tri[0], mlr_limits.tri[1]);
printf( "line %d %d\n", mlr_limits.line[0], mlr_limits.line[1]);
printf( "ma %d\n", mlr_limits.ma);
printf( "li %d\n", mlr_limits.li);
printf( "obj %d %d\n", mlr_limits.obj[0], mlr_limits.obj[1]);
#endif

#ifdef	RENDER_DIRECT
    MLR_init( "Dore");

/* kludge object limits */
    mlr_limits.obj[1] = mlr_limits.obj[0] * mlr_limits.li * 2;
    MLR_ctl_init( FALSE, TRUE, FALSE, FALSE, TRUE, FALSE);

/* write limits to limits file */
    MLR_limit( LIMIT_CP, mlr_limits.cp);
    MLR_limit( LIMIT_TEX, mlr_limits.tex[0], mlr_limits.tex[1]);
    MLR_limit( LIMIT_TRI, mlr_limits.tri[0], mlr_limits.tri[1]);
    MLR_limit( LIMIT_LINE, mlr_limits.line[0], mlr_limits.line[1]);
    MLR_limit( LIMIT_MA, mlr_limits.ma);
    MLR_limit( LIMIT_LI, mlr_limits.li);
    MLR_limit( LIMIT_OBJ, mlr_limits.obj[0], mlr_limits.obj[1]);
    
    if ( mlr_limits.vox != 0 )
    {
        MLR_limit( LIMIT_VOX, mlr_limits.vox, mlr_limits.vox);
    }
#endif
}


/*[
 *$     MLRD_direct_exec
 *
 *      Steve Gray - Mon Aug  7 15:02:54 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    execute direct mode render
 *
 *      Usage:
 *		MLRD_direct_exec()
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MLRD_direct_exec()
{
Short	lit_list[1000];
Short	obj_id;
Short	lit_count;
Short	lit_id;


#ifdef	DEBUG
printf( "MLRD_direct_exec\n");
#endif

/* set global ambient level */
#ifdef	RENDER_DIRECT
    MLR_glamb(mlr_global_ambient.r, mlr_global_ambient.g, mlr_global_ambient.b);
#endif

/* set global light model info */
    MATD_print_gllmod();

/* assign lights to objects, since every object gets all the lights,
 * build a light list with all the lights, and then assign it to each
 * object
 */
    lit_count = mlr_limits.li;
    for ( lit_id = 0; lit_id < mlr_limits.li; lit_id++ )
    {
	lit_list[lit_id] = lit_id;
    }

    for ( obj_id = 0; obj_id < mlr_limits.obj[0]; obj_id++ )
    {
#ifdef	RENDER_DIRECT
	MLR_lto( obj_id, lit_count, lit_list);
#endif
    }

/* do it */
#ifdef	RENDER_DIRECT
    MLR_render();
#endif
}

