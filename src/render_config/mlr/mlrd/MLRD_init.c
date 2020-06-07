/* vi:set ts=8 sw=4 ai:
 *[
 *$     MLRD_init.c
 *
 *      Steve Gray - Sun Jul  9 12:01:26 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	initialize render database
 *
 *      Usage:
 *		MLRD_init()		init database
 *
 *      Notes:
 *
 *
 *?
 *!     Changes History
 *
 *]
 */


/* #define	DEBUG  */


/* system includes */
#include <stdio.h>

/* dore includes */
#include "dore.h"

/* Render includes */
#include "primitives.h"

/* MLRD Dore interface includes */
#include "global.h"


/* first time through flag */
static Int	first_time = TRUE;
static Char	prev_base_name[256];


/*[
 *$     MLRD_init
 *
 *      Steve Gray - Sun Jul  9 12:02:10 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    initialize render globals
 *
 *      Usage:
 *		MLRD_init()
 *
 *      Notes:  
 *		reset limits
 *		open conversion files
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MLRD_init()
{
Char	base_name[256], *tempnam();
Char	sys_cmd[256];


#ifdef	DEBUG
printf( "MLRD_init\n");
#endif


/* reset global ambient level */
    mlr_global_ambient.r = 0.0;
    mlr_global_ambient.g = 0.0;
    mlr_global_ambient.b = 0.0;

/* reset sampling level */
    mlr_sample_level = 1;

/* init conversion database */
    MATD_init();
    LIT_init();
    GEOM_init();

/* reset the limits */
    mlr_limits.cp = 0;

    mlr_limits.tex[0] = 0;
    mlr_limits.tex[1] = 0;

    mlr_limits.tri[0] = 0;
    mlr_limits.tri[1] = 0;

    mlr_limits.ma = 0;

    mlr_limits.li = 0;

    mlr_limits.obj[0] = 0;
    mlr_limits.obj[1] = 0;

    if ( mlr_mode != MLR_MODE_DIRECT )
    {
/* get a the base file name */
        strcpy( base_name, mlr_files.file_prefix);

/* create the render control file */
        strcpy( mlr_files.render_file, base_name);
        strcat( mlr_files.render_file, ".ren");

#ifdef	DEBUG
printf( "render file %s\n", mlr_files.render_file);
#endif

        if ( (mlr_files.render_fp = fopen( mlr_files.render_file, "w")) == NULL)
        {
	    printf( "file name: %s\n", mlr_files.render_file);
	    DDerror( ERR_FATAL_INTERNAL_ERROR, "MLRD_init", 
			"error creating control file");
        }

        fprintf( mlr_files.render_fp, "#\n");
        fprintf( mlr_files.render_fp, "# Media Logic render control file\n");
        fprintf( mlr_files.render_fp, "#\n");

/* create the camera file */
        strcpy( mlr_files.camera_file, base_name);
        strcat( mlr_files.camera_file, ".cam");

#ifdef	DEBUG
printf( "camera file %s\n", mlr_files.camera_file);
#endif

        if ((mlr_files.camera_fp = fopen( mlr_files.camera_file, "w")) == NULL )
        {
	    printf( "file name: %s\n", mlr_files.camera_file);
	    DDerror( ERR_FATAL_INTERNAL_ERROR, "MLRD_init", 
			"error creating control file");
        }

        fprintf( mlr_files.camera_fp, "#\n");
        fprintf( mlr_files.camera_fp, "# Media Logic camera control file\n");
        fprintf( mlr_files.camera_fp, "#\n");

/* create the light file */
        strcpy( mlr_files.light_file, base_name);
        strcat( mlr_files.light_file, ".lit");

#ifdef	DEBUG
printf( "light file %s\n", mlr_files.light_file);
#endif

        if ( (mlr_files.light_fp = fopen( mlr_files.light_file, "w")) == NULL )
        {
	    printf( "file name: %s\n", mlr_files.light_file);
	    DDerror( ERR_FATAL_INTERNAL_ERROR, "MLRD_init", 
			"error creating control file");
        }

        fprintf( mlr_files.light_fp, "#\n");
        fprintf( mlr_files.light_fp, "# Media Logic light control file\n");
        fprintf( mlr_files.light_fp, "#\n");

        fprintf( mlr_files.light_fp, "amod dore\n");


/* create the limits file */
        strcpy( mlr_files.limits_file, base_name);
        strcat( mlr_files.limits_file, ".lim");

#ifdef	DEBUG
printf( "limits file %s\n", mlr_files.limits_file);
#endif

        if ( (mlr_files.limits_fp = fopen( mlr_files.limits_file, "w")) == NULL)
        {
	    printf( "file name: %s\n", mlr_files.limits_file);
	    DDerror( ERR_FATAL_INTERNAL_ERROR, "MLRD_init", 
			"error creating control file");
        }

        fprintf( mlr_files.limits_fp, "#\n");
        fprintf( mlr_files.limits_fp, "# Media Logic limits file\n");
        fprintf( mlr_files.limits_fp, "#\n");

/* create the object file */
        strcpy( mlr_files.object_file, base_name);
        strcat( mlr_files.object_file, ".obj");

#ifdef	DEBUG
printf( "object file %s\n", mlr_files.object_file);
#endif

        if ((mlr_files.object_fp = fopen( mlr_files.object_file, "w")) == NULL )
        {
	    printf( "file name: %s\n", mlr_files.object_file);
	    DDerror( ERR_FATAL_INTERNAL_ERROR, "MLRD_init", 
			"error creating control file");
        }

        fprintf( mlr_files.object_fp, "#\n");
        fprintf( mlr_files.object_fp, "# Media Logic object file\n");
        fprintf( mlr_files.object_fp, "#\n");

        fprintf( mlr_files.object_fp, "reverse on\n");
        fprintf( mlr_files.object_fp, "shade always\n");

/* get the RLE file name */
        strcpy( mlr_files.rle_file, base_name);
        strcat( mlr_files.rle_file, ".rle");

#ifdef	DEBUG
printf( "rle file %s\n", mlr_files.rle_file);
#endif
    }
    else
    {
#ifdef	RENDER_DIRECT
	MLR_amod( ATTENUATION_MODEL_DORE);
	MLR_reverse( TRUE);
	MLR_shade( SHADE_MODE_ALWAYS);
#endif
    }
}




