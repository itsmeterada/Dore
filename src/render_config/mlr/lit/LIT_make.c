/* vi:set ts=8 sw=4 ai:
 *[
 *$     LIT_make.c
 *
 *      Steve Gray - Sun Jul  9 12:20:10 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	functions to make Media Logic lights
 *
 *      Usage:
 *		LIT_make()		light method routine
 *
 *		lit_make_ambient()	make ambient light
 *		lit_make_distant()	make distant light
 *		lit_make_point()	make point light
 *		lit_make_point_decay()	make decaying point light
 *		lit_make_spot()		make spot light
 *		lit_make_spot_decay()	make decaying spot light
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


/* convert radians to degrees */
#define	RAD_TO_DEG(rr)	(((rr) / M_PI) * 180.0)


/*[
 *$     LIT_make
 *
 *      Steve Gray - Sun Jul  9 11:45:07 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    method routine for lights
 *
 *      Usage:
 *		LIT_make( object)
 *		DtObject	object		(in)  light object
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

LIT_make( object)
DtObject	object;
{
DtPFI		routine, DDclass_InqMethod();
DtObject	lighttype;
DtColorRGB	lightcolor;
DtReal		lightintens;
DtMatrix4x4	matrix;
DtReal		w;
DtPoint3	pos;
DtVector3	dir;
DtInt		light;
DtColorModel	colormodel;

#ifdef	DEBUG
printf( "LIT_make\n");
#endif

/* get light type */
    routine = DDclass_InqMethod( DcTypeAttLightType,
				    DcMethodInqGlbAttVal);
    
    (*routine)( &lighttype);

/* get light color */
    routine = DDclass_InqMethod( DcTypeAttLightColor,
				    DcMethodInqGlbAttVal);
    
    (*routine)( &colormodel, lightcolor);

/* get intensity */
    routine = DDclass_InqMethod( DcTypeAttLightIntens,
				    DcMethodInqGlbAttVal);
    
    (*routine)( &lightintens);

/* get light matrix */
    routine = DDclass_InqMethod( DcTypeAttLcstowcsmat,
				    DcMethodInqGlbAttVal);
    
    (*routine)( matrix);

/* set the position and direction */
    dir[0] = 0.0;
    dir[1] = 0.0;
    dir[2] = -1.0;

/* position of the light */
    pos[0] = 0.0;
    pos[1] = 0.0;
    pos[2] = 0.0;

/* print the matrix to the light file */
    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
	if ( mlr_mode == MLR_MODE_FILE )
	{
            fprintf( mlr_files.light_fp, 
		     "mat %f %f %f %f %f %f %f %f %f %f %f %f\n",
	             matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
	             matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
	             matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
	}
	else
	{
#ifdef	RENDER_DIRECT
	    MLR_mat( matrix);
#endif
	}
    }

/* create the light */
    if ( lighttype ==  DcLightAmbient )
    {
	lit_make_ambient( lightcolor, lightintens);
    }
    else if ( lighttype == DcLightInfinite )
    {
	lit_make_distant( lightcolor, lightintens, dir);
    }
    else if ( lighttype == DcLightPoint )
    {
	lit_make_point( lightcolor, lightintens, pos);
    }
    else if ( lighttype == DcLightPointAttn )
    {
	lit_make_point_decay( lightcolor, lightintens, pos);
    }
    else if ( lighttype == DcLightSpot )
    {
	lit_make_spot( lightcolor, lightintens, pos, dir);
    }
    else if ( lighttype == DcLightSpotAttn )
    {
	lit_make_spot_decay( lightcolor, lightintens, pos, dir);
    }
    else
    {
	DDerror( ERR_BAD_OBJECT, "LIT_make", "un-supported light type");
    }

    if ( mlr_mode == MLR_MODE_FILE )
    {
        fprintf( mlr_files.light_fp, "\n");
    }

/* for all light types except ambient, create the light in the light list
 * database
 */
    if ( lighttype != DcLightAmbient )
    {
	LIT_list_add( object);
    }
}


/*[
 *$     lit_make_ambient()
 *
 *      Steve Gray - Sun Jul  9 12:21:00 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    make ambient light
 *
 *      Usage:
 *		lit_make_ambient( color, intensity)
 *		DtColorRGB	color		(in)  color
 *		DtReal		intensity	(in)  intensity
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

lit_make_ambient( color, intensity)
DtColorRGB	color;
DtReal		intensity;
{
Simple_rgb	lit_color;


#ifdef	DEBUG
printf( "lit make ambient %f %f %f (%f)\n", color[0], color[1], color[2],
	intensity);
#endif

/* compute the true light color */
    lit_color.r = color[0] * intensity;
    lit_color.g = color[1] * intensity;
    lit_color.b = color[2] * intensity;

/* add the light into the global ambient level */
    mlr_global_ambient.r += lit_color.r;
    mlr_global_ambient.g += lit_color.g;
    mlr_global_ambient.b += lit_color.b;
}


/*[
 *$     lit_make_distant
 *
 *      Steve Gray - Sun Jul  9 12:52:39 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    make a distant light
 *
 *      Usage:
 *		lit_make_distant( color, intensity, dir)
 *		DtColorRGB	color		(in)  light color
 *		DtFloat		intensity	(in)  light intensity
 *		DtVector3	dir		(in)  direction vector
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

lit_make_distant( color, intensity, dir)
DtColorRGB	color;
DtReal		intensity;
DtVector3	dir;
{
Simple_rgb	lit_color;


#ifdef	DEBUG
printf( "lit make distant %f %f %f (%f) : %f %f %f\n", 
	color[0], color[1], color[2], intensity, dir[0], dir[1], dir[2]);
#endif


/* compute true light color */
    lit_color.r = color[0] * intensity;
    lit_color.g = color[1] * intensity;
    lit_color.b = color[2] * intensity;

/* if we are not counting objects */
    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
	if ( mlr_mode == MLR_MODE_FILE )
	{
/* write the light to the light file */
	    fprintf( mlr_files.light_fp, "lds 0 0 0  %f %f %f  %f %f %f  1.0\n",
		     lit_color.r, lit_color.g, lit_color.b,
		     dir[0], dir[1], dir[2]);

	    fprintf( mlr_files.light_fp, "lp %d   0 0 0  %f %f %f\n",
		     mlr_limits.li, dir[0], dir[1], dir[2]);
	}
	else
	{
#ifdef	RENDER_DIRECT
	    MLR_lds( 0.0, 0.0, 0.0, (Float) lit_color.r, (Float) lit_color.g, 
		     (Float) lit_color.b, (Float) dir[0], (Float) dir[1], 
		     (Float) -dir[2], 1.0);

	    MLR_lp( mlr_limits.li, 0.0, 0.0, 0.0, (Float) dir[0],
		    (Float) dir[1], (Float) -dir[2]);
#endif
	}
    }

/* increment light limits */
    (mlr_limits.li)++;
}


/*[
 *$     lit_make_point
 *
 *      Steve Gray - Sun Jul  9 12:52:39 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    make a point light
 *
 *      Usage:
 *		lit_make_point( color, intensity, pos)
 *		DtColorRGB	color		(in)  light color
 *		DtFloat		intensity	(in)  light intensity
 *		DtVector3	pos		(in)  position
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

lit_make_point( color, intensity, pos)
DtColorRGB	color;
DtReal		intensity;
DtVector3	pos;
{
Simple_rgb	lit_color;


#ifdef	DEBUG
printf( "lit make point %f %f %f (%f) : %f %f %f\n", 
	color[0], color[1], color[2], intensity, pos[0], pos[1], pos[2]);
#endif


/* compute true light color */
    lit_color.r = color[0] * intensity;
    lit_color.g = color[1] * intensity;
    lit_color.b = color[2] * intensity;

/* write the light to the light file */
    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
	if ( mlr_mode == MLR_MODE_FILE )
	{
	    fprintf( mlr_files.light_fp, "lis %f %f %f  %f %f %f  1.0\n",
		     pos[0], pos[1], pos[2],
		     lit_color.r, lit_color.g, lit_color.b);

	    fprintf( mlr_files.light_fp, "lp %d %f %f %f\n",
		     mlr_limits.li, pos[0], pos[1], pos[2]);
	}
	else
	{
#ifdef	RENDER_DIRECT
	    MLR_lis( (Float) pos[0], (Float) pos[1], (Float) pos[2],
		     (Float) lit_color.r, (Float) lit_color.g, 
		     (Float) lit_color.b);

	    MLR_lp( mlr_limits.li, (Float) pos[0], (Float) pos[1], 
		    (Float) pos[2]);
#endif
	}
    }

/* increment light limits */
    (mlr_limits.li)++;
    (mlr_limits.obj[1])++;
}


/*[
 *$     lit_make_point_decay
 *
 *      Steve Gray - Sun Jul  9 12:52:39 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    make a decaying point light
 *
 *      Usage:
 *		lit_make_point_decay( color, intensity, pos)
 *		DtColorRGB	color		(in)  light color
 *		DtFloat		intensity	(in)  light intensity
 *		DtVector3	pos		(in)  position
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

lit_make_point_decay( color, intensity, pos)
DtColorRGB	color;
DtReal		intensity;
DtVector3	pos;
{
Simple_rgb	lit_color;
DtReal		c1, c2;
DtPFI		routine, DDclass_InqMethod();


#ifdef	DEBUG
printf( "lit make point %f %f %f (%f) : %f %f %f\n", 
	color[0], color[1], color[2], intensity, pos[0], pos[1], pos[2]);
#endif


/* compute true light color */
    lit_color.r = color[0] * intensity;
    lit_color.g = color[1] * intensity;
    lit_color.b = color[2] * intensity;

/* get the attenuation factors */
    routine = DDclass_InqMethod( DsInqClassId("AttLightAttenuation"),
				  DcMethodInqGlbAttVal);
    
    (*routine)( &c1, &c2);

/* write the light to the light file */
    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
	if ( mlr_mode == MLR_MODE_FILE )
	{
	    fprintf( mlr_files.light_fp, 
		     "lids %f %f %f  %f %f %f  %f %f  1.0\n",
		     pos[0], pos[1], pos[2],
		     lit_color.r, lit_color.g, lit_color.b, c1, c2);

	    fprintf( mlr_files.light_fp, "lp %d %f %f %f\n",
		     mlr_limits.li, pos[0], pos[1], pos[2]);
	}
	else
	{
#ifdef	RENDER_DIRECT
	    MLR_lids( (Float) pos[0], (Float) pos[1], (Float) pos[2],
		      (Float) lit_color.r, (Float) lit_color.g, 
		      (Float) lit_color.b, (Float) c1, (Float) c2, 1.0);

	    MLR_lp( mlr_limits.li, (Float) pos[0], (Float) pos[1], 
		    (Float) pos[2]);
#endif
	}
    }

/* increment light limits */
    (mlr_limits.li)++;
    (mlr_limits.obj[1])++;
}


/*[
 *$     lit_make_spot
 *
 *      Steve Gray - Sun Jul  9 12:52:39 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    make a spot light
 *
 *      Usage:
 *		lit_make_spot( color, intensity, pos, dir)
 *		DtColorRGB	color		(in)  light color
 *		DtFloat		intensity	(in)  light intensity
 *		DtVector3	pos		(in)  position
 *		DtVector3	dir		(in)  direction
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

lit_make_spot( color, intensity, pos, dir)
DtColorRGB	color;
DtReal		intensity;
DtVector3	pos;
DtVector3	dir;
{
Simple_rgb	lit_color;
DtReal		width, delta, exponent;
DtPFI		routine, DDclass_InqMethod();


#ifdef	DEBUG
printf( "lit make spot %f %f %f (%f) : %f %f %f -> %f %f %f\n", 
	color[0], color[1], color[2], intensity, pos[0], pos[1], pos[2],
	dir[0], dir[1], dir[2]);
#endif


/* get light spread angles */
    routine = DDclass_InqMethod( DsInqClassId("AttLightSpreadAngles"),
				  DcMethodInqGlbAttVal);
    
    (*routine)( &width, &delta);

/* get light spread angles */
    routine = DDclass_InqMethod( DsInqClassId("AttLightSpreadExp"),
				  DcMethodInqGlbAttVal);
    
    (*routine)( &exponent);

/* compute true light color */
    lit_color.r = color[0] * intensity;
    lit_color.g = color[1] * intensity;
    lit_color.b = color[2] * intensity;

/* write the light to the light file */
    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
	if ( mlr_mode == MLR_MODE_FILE )
	{
	    fprintf( mlr_files.light_fp, 
		     "lts %f %f %f  %f %f %f  %f %f %f  %f %f %f  1.0\n",
		     pos[0], pos[1], pos[2],
		     lit_color.r, lit_color.g, lit_color.b,
		     dir[0], dir[1], dir[2],
		     RAD_TO_DEG( width), RAD_TO_DEG( delta), exponent);

	    fprintf( mlr_files.light_fp, "lp %d %f %f %f  %f %f %f\n",
		     mlr_limits.li, pos[0], pos[1], pos[2],
		     dir[0], dir[1], dir[2]);
	}
	else
	{
#ifdef	RENDER_DIRECT
	    MLR_lts( (Float) pos[0], (Float) pos[1], (Float) pos[2],
		     (Float) lit_color.r, (Float) lit_color.g, 
		         (Float) lit_color.b, 
		     (Float) dir[0], (Float) dir[1], (Float) dir[2],
		     (Float) width, (Float) delta, (Float) exponent,
		     (Float) 1.0);

	    MLR_lp( mlr_limits.li, (Float) pos[0], (Float) pos[1], 
		    		       (Float) pos[2],
				   (Float) dir[0], (Float) dir[1],
				       (Float) dir[2]);
#endif
	}
    }

/* increment light limits */
    (mlr_limits.li)++;
    (mlr_limits.obj[1])++;
}

/*[
 *$     lit_make_spot_decay
 *
 *      Steve Gray - Sun Jul  9 12:52:39 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    make a decaying spot light
 *
 *      Usage:
 *		lit_make_spot_decay( color, intensity, pos, dir)
 *		DtColorRGB	color		(in)  light color
 *		DtFloat		intensity	(in)  light intensity
 *		DtVector3	pos		(in)  position
 *		DtVector3	dir		(in)  direction
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

lit_make_spot_decay( color, intensity, pos, dir)
DtColorRGB	color;
DtReal		intensity;
DtVector3	pos;
DtVector3	dir;
{
Simple_rgb	lit_color;
DtReal		width, delta, exponent;
DtReal		c1, c2;
DtPFI		routine, DDclass_InqMethod();


#ifdef	DEBUG
printf( "lit make spot %f %f %f (%f) : %f %f %f -> %f %f %f\n", 
	color[0], color[1], color[2], intensity, pos[0], pos[1], pos[2],
	dir[0], dir[1], dir[2]);
#endif


/* get light spread angles */
    routine = DDclass_InqMethod( DsInqClassId("AttLightSpreadAngles"),
				    DcMethodInqGlbAttVal);
    
    (*routine)( &width, &delta);

/* get light spread angles */
    routine = DDclass_InqMethod( DsInqClassId("AttLightSpreadExp"),
				  DcMethodInqGlbAttVal);
    
    (*routine)( &exponent);

/* get light decay factors */
    routine = DDclass_InqMethod( DsInqClassId("AttLightAttenuation"),
				    DcMethodInqGlbAttVal);
    
    (*routine)( &c1, &c2);

/* compute true light color */
    lit_color.r = color[0] * intensity;
    lit_color.g = color[1] * intensity;
    lit_color.b = color[2] * intensity;

/* write the light to the light file */
    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
	if ( mlr_mode == MLR_MODE_FILE )
	{
	    fprintf( mlr_files.light_fp, 
		     "ltds %f %f %f  %f %f %f  %f %f %f  %f %f %f  %f %f 1.0\n",
		     pos[0], pos[1], pos[2],
		     lit_color.r, lit_color.g, lit_color.b,
		     dir[0], dir[1], dir[2],
		     RAD_TO_DEG( width), RAD_TO_DEG( delta), exponent, c1, c2);

	    fprintf( mlr_files.light_fp, "lp %d %f %f %f  %f %f %f\n",
		     mlr_limits.li, pos[0], pos[1], pos[2],
		     dir[0], dir[1], dir[2]);
	}
	else
	{
#ifdef	RENDER_DIRECT
	    MLR_ltds( (Float) pos[0], (Float) pos[1], (Float) pos[2],
		     (Float) lit_color.r, (Float) lit_color.g, 
		         (Float) lit_color.b, 
		     (Float) dir[0], (Float) dir[1], (Float) dir[2],
		     (Float) width, (Float) delta, (Float) exponent,
		     (Float) c1, (Float) c2, (Float) 1.0);

	    MLR_lp( mlr_limits.li, (Float) pos[0], (Float) pos[1], 
		         	       (Float) pos[2], 
				   (Float) dir[0], (Float) dir[1], 
				       (Float) dir[2]);
#endif
	}
    }

/* increment light limits */
    (mlr_limits.li)++;
    (mlr_limits.obj[1])++;
}


