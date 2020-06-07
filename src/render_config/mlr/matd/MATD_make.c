/* vi:set ts=8 sw=4 ai:
 *[
 *$     MATD_make.c
 *
 *      Steve Gray - Mon Jul 17 13:40:21 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	make materials
 *
 *      Usage:
 *		MATD_make_local()	start local material group
 *		MATD_make_init()	initialize a new material
 *		MATD_make_from_dore()	get material props from Dore stack
 *		MATD_make_coeff()	set material coefficient using Dore info
 *		MATD_texmap_color()	texture map the color 
 *		MATD_texmap_transp()	texture map the transparency 
 *		MATD_texmap_bump()	texture map the normals 
 *		MATD_make_complete()	complete material definition
 *
 *      Notes:
 *
 *	01/17/90	add uv transformation matrix support		gray
 *	01/18/90	fix extend mode to work seperatly in U and V	gray
 *	01/18/90	add uv coordinate matrix			gray
 *	01/23/90	fix shadow color				gray
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

/* MLR Dore interface includes */
#include "global.h"
#include "matd.h"


static float	id_mat[3][3] = 
			{
			 { 1.0, 0.0, 0.0},
			 { 0.0, 1.0, 0.0},
			 { 0.0, 0.0, 1.0}
			};

/*[
 *$     MATD_make_local
 *
 *      Steve Gray - Mon Jul 17 13:41:50 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    start local material definition
 *
 *      Usage:
 *		MATD_make_local()
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MATD_make_local()
{
    if ( mlr_mode == MLR_MODE_FILE )
    {
        fprintf( mlr_files.object_fp, "localma\n");
    }
    else
    {
#ifdef	RENDER_DIRECT
	MLR_localma();
#endif
    }
}


/*[
 *$     MATD_make_init()
 *
 *      Steve Gray - Mon Jul 17 13:43:04 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    initialize material definition
 *
 *      Usage:
 *		MATD_make_init()
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MATD_make_init()
{
    matd_curr.lmod = 0;

    matd_curr.amb.r = 0.0;
    matd_curr.amb.g = 0.0;
    matd_curr.amb.b = 0.0;

    matd_curr.diff.r = 0.0;
    matd_curr.diff.g = 0.0;
    matd_curr.diff.b = 0.0;

    matd_curr.spec.r = 0.0;
    matd_curr.spec.g = 0.0;
    matd_curr.spec.b = 0.0;

    matd_curr.exponent = 1.0;

    matd_curr.rflclr.r = 1.0;
    matd_curr.rflclr.g = 1.0;
    matd_curr.rflclr.b = 1.0;

    matd_curr.rflmap_present = FALSE;

    matd_curr.rfrclr.r = 1.0;
    matd_curr.rfrclr.g = 1.0;
    matd_curr.rfrclr.b = 1.0;

    matd_curr.rfrmap_present = FALSE;

    matd_curr.refl.r = 0.0;
    matd_curr.refl.g = 0.0;
    matd_curr.refl.b = 0.0;

    matd_curr.rfridx = 1.0;

    matd_curr.center_transp.r = 0.0;
    matd_curr.center_transp.g = 0.0;
    matd_curr.center_transp.b = 0.0;

    matd_curr.edge_transp.r = 0.0;
    matd_curr.edge_transp.g = 0.0;
    matd_curr.edge_transp.b = 0.0;

    matd_curr.transp_exp = 0.0;

    (mlr_limits.ma)++;

/* open the material definition */
    if ( mlr_mode == MLR_MODE_FILE )
    {
        fprintf( mlr_files.object_fp, "ma\n");
    }
    else
    {
#ifdef	RENDER_DIRECT
	MLR_ma( MA_BEGIN, NULL);
#endif
    }
}


/*[
 *$     MATD_make_from_dore
 *
 *      Steve Gray - Mon Jul 17 13:51:30 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    get material info from Dore attribute info
 *
 *      Usage:
 *		MATD_make_from_dore()
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MATD_make_from_dore()
{
DtPFI		routine, DDclass_InqMethod();
DtSwitch	sw;
DtReal		intensity, factor;
DtNColor3	color;
DtColorModel	colormodel;

#ifdef	DEBUG
printf( "MATD_make_from_dore\n");
#endif


/* get ambient info */
    routine = DDclass_InqMethod( DcTypeAttAmbientSwitch,
				    DcMethodInqGlbAttVal);
    
    (*routine)( &sw);

#ifdef	DEBUG
printf( "ambient switch %d\n", sw);
#endif

    matd_dore.amb_switch = sw;

    if ( sw )
    {
        routine = DDclass_InqMethod( DcTypeAttAmbientIntens,
				        DcMethodInqGlbAttVal);
        
        (*routine)( &intensity);

#ifdef	DEBUG
printf( "ambient intensity %f\n", intensity);
#endif

	matd_dore.amb_intensity = intensity;
    }

/* get diffuse info */
    routine = DDclass_InqMethod( DcTypeAttDiffuseSwitch,
				    DcMethodInqGlbAttVal);
    
    (*routine)( &sw);

#ifdef	DEBUG
printf( "diffuse switch %d\n", sw);
#endif

    matd_dore.diff_switch = sw;

    if ( sw )
    {
        routine = DDclass_InqMethod( DcTypeAttDiffuseIntens,
				        DcMethodInqGlbAttVal);
        
        (*routine)( &intensity);

#ifdef	DEBUG
printf( "diffuse intensity %f\n", intensity);
#endif

	matd_dore.diff_intensity = intensity;

        routine = DDclass_InqMethod( DcTypeAttDiffuseColor,
				        DcMethodInqGlbAttVal);
        
        (*routine)( &colormodel, &color);

#ifdef	DEBUG
printf( "diffuse color %f %f %f\n", color.r, color.g, color.b);
#endif

	matd_dore.diff_color.r = color.r;
	matd_dore.diff_color.g = color.g;
	matd_dore.diff_color.b = color.b;
    }

/* get specular info */
    routine = DDclass_InqMethod( DcTypeAttSpecularSwitch,
				    DcMethodInqGlbAttVal);
    
    (*routine)( &sw);

#ifdef	DEBUG
printf( "specular switch %d\n", sw);
#endif

    matd_dore.spec_switch = sw;

    if ( sw )
    {
        routine = DDclass_InqMethod( DcTypeAttSpecularIntens,
				        DcMethodInqGlbAttVal);
        
        (*routine)( &intensity);

#ifdef	DEBUG
printf( "specular intensity %f\n", intensity);
#endif

	matd_dore.spec_intensity = intensity;

        routine = DDclass_InqMethod( DcTypeAttSpecularColor,
				        DcMethodInqGlbAttVal);
        
        (*routine)( &colormodel, &color);

#ifdef	DEBUG
printf( "specular color %f %f %f\n", color.r, color.g, color.b);
#endif

	matd_dore.spec_color.r = color.r;
	matd_dore.spec_color.g = color.g;
	matd_dore.spec_color.b = color.b;

        routine = DDclass_InqMethod( DcTypeAttSpecularFactor,
				        DcMethodInqGlbAttVal);
        
        (*routine)( &factor);

#ifdef	DEBUG
printf( "specular factor %f\n", factor);
#endif

	matd_dore.spec_exponent = factor;
    }

/* get transparency info */
    routine = DDclass_InqMethod( DcTypeAttTranspSwitch,
				    DcMethodInqGlbAttVal);
    
    (*routine)( &sw);

#ifdef	DEBUG
printf( "transparency switch %d\n", sw);
#endif

    matd_dore.transp_switch = sw;

    if ( sw )
    {
        routine = DDclass_InqMethod( DcTypeAttTranspIntens,
				        DcMethodInqGlbAttVal);
        
        (*routine)( &intensity);

#ifdef	DEBUG
printf( "transparency intensity %f\n", intensity);
#endif

	matd_dore.transp_intensity = intensity;

        routine = DDclass_InqMethod( DcTypeAttTranspColor,
				        DcMethodInqGlbAttVal);
        
        (*routine)( &colormodel, &color);

#ifdef	DEBUG
printf( "transparency color %f %f %f\n", color.r, color.g, color.b);
#endif

	matd_dore.transp_color.r = color.r;
	matd_dore.transp_color.g = color.g;
	matd_dore.transp_color.b = color.b;
    }

/* get oriented transparency info */
    routine = DDclass_InqMethod( DsInqClassId( "AttTranspOrientSwitch"),
				  DcMethodInqGlbAttVal);
    
    (*routine)( &sw);

#ifdef	DEBUG
printf( "transparency orientation switch %d\n", sw);
#endif

    matd_dore.transp_orient_switch = sw;

    if ( sw )
    {
        routine = DDclass_InqMethod( DsInqClassId( "AttTranspOrientIntens"),
				      DcMethodInqGlbAttVal);
        
        (*routine)( &intensity);

#ifdef	DEBUG
printf( "transparency orient intensity %f\n", intensity);
#endif

	matd_dore.transp_orient_intensity = intensity;

        routine = DDclass_InqMethod( DsInqClassId( "AttTranspOrientColor"),
				      DcMethodInqGlbAttVal);
        
        (*routine)( &colormodel, &color);

#ifdef	DEBUG
printf( "transparency orient color %f %f %f\n", color.r, color.g, color.b);
#endif

        routine = DDclass_InqMethod( DsInqClassId( "AttTranspOrientExp"),
				      DcMethodInqGlbAttVal);
        
        (*routine)( &factor);

	matd_dore.transp_orient_color.r = color.r;
	matd_dore.transp_orient_color.g = color.g;
	matd_dore.transp_orient_color.b = color.b;

	matd_dore.transp_exp = factor;
    }

/* get reflection info */
    routine = DDclass_InqMethod( DcTypeAttReflectionSwitch,
				    DcMethodInqGlbAttVal);
    
    (*routine)( &sw);

#ifdef	DEBUG
printf( "reflection switch %d\n", sw);
#endif

    matd_dore.refl_switch = sw;

/* get shadow info */
    routine = DDclass_InqMethod( DcTypeAttShadowSwitch,
				    DcMethodInqGlbAttVal);
    
    (*routine)( &sw);

#ifdef	DEBUG
printf( "shadow switch %d\n", sw);
#endif

    matd_dore.shad_switch = sw;

/* get refraction info */
    routine = DDclass_InqMethod( DsInqClassId( "AttRefractionSwitch"),
				  DcMethodInqGlbAttVal);
    
    (*routine)( &sw);

#ifdef	DEBUG
printf( "refraction switch %d\n", sw);
#endif

    matd_dore.refr_switch = sw;

    if ( sw )
    {
        routine = DDclass_InqMethod( DsInqClassId( "AttRefractionIndex"),
				      DcMethodInqGlbAttVal);
        
        (*routine)( &factor);

#ifdef	DEBUG
printf( "refraction index %f\n", factor);
#endif

	matd_dore.refr_index = factor;

	if ( !matd_dore.transp_switch )
	{
            routine = DDclass_InqMethod( DcTypeAttTranspIntens,
				        DcMethodInqGlbAttVal);
        
            (*routine)( &intensity);

#ifdef	DEBUG
printf( "transparency intensity %f\n", intensity);
#endif

	    matd_dore.transp_intensity = intensity;

            routine = DDclass_InqMethod( DcTypeAttTranspColor,
				            DcMethodInqGlbAttVal);
        
            (*routine)( &colormodel, &color);

#ifdef	DEBUG
printf( "transparency color %f %f %f\n", color.r, color.g, color.b);
#endif

	    matd_dore.transp_color.r = color.r;
	    matd_dore.transp_color.g = color.g;
	    matd_dore.transp_color.b = color.b;
	}
    }

/* set default colors from Dore */
    if ( matd_dore.amb_switch )
    {
        matd_curr.amb.r = matd_dore.diff_color.r * matd_dore.amb_intensity;
        matd_curr.amb.g = matd_dore.diff_color.g * matd_dore.amb_intensity;
        matd_curr.amb.b = matd_dore.diff_color.b * matd_dore.amb_intensity;

	matd_curr.lmod |= LMODFLAG_AMB;
    }

    if ( matd_dore.diff_switch )
    {
        matd_curr.diff.r = matd_dore.diff_color.r * matd_dore.diff_intensity;
        matd_curr.diff.g = matd_dore.diff_color.g * matd_dore.diff_intensity;
        matd_curr.diff.b = matd_dore.diff_color.b * matd_dore.diff_intensity;

	matd_curr.lmod |= LMODFLAG_LIGHT;
    }

    if ( matd_dore.spec_switch )
    {
        matd_curr.spec.r = matd_dore.spec_color.r * matd_dore.spec_intensity;
        matd_curr.spec.g = matd_dore.spec_color.g * matd_dore.spec_intensity;
        matd_curr.spec.b = matd_dore.spec_color.b * matd_dore.spec_intensity;

	matd_curr.exponent = matd_dore.spec_exponent;

	matd_curr.lmod |= LMODFLAG_LIGHT;
    }

    if ( matd_dore.refl_switch )
    {
	if ( matd_dore.spec_switch )
	{
	    matd_curr.refl.r = matd_dore.spec_color.r;
	    matd_curr.refl.g = matd_dore.spec_color.g;
	    matd_curr.refl.b = matd_dore.spec_color.b;
	}
	else
	{
	    matd_curr.refl.r = 1.0;
	    matd_curr.refl.g = 1.0;
	    matd_curr.refl.b = 1.0;
	}

	matd_curr.lmod |= LMODFLAG_REFL;
    }

    if ( matd_dore.shad_switch )
    {
	if ( matd_dore.transp_switch )
	{
	    matd_curr.shad.r = matd_dore.transp_intensity *
			       matd_dore.transp_color.r;

	    matd_curr.shad.g = matd_dore.transp_intensity *
			       matd_dore.transp_color.g;

	    matd_curr.shad.b = matd_dore.transp_intensity *
			       matd_dore.transp_color.b;
	}
	else
	{
	    matd_curr.shad.r = 0.0;
	    matd_curr.shad.g = 0.0;
	    matd_curr.shad.b = 0.0;
	}

	matd_curr.lmod |= LMODFLAG_SHAD;
    }

    if ( matd_dore.transp_switch )
    {
	matd_curr.center_transp_intens = matd_dore.transp_intensity;
	matd_curr.center_transp.r = matd_dore.transp_color.r;
	matd_curr.center_transp.g = matd_dore.transp_color.g;
	matd_curr.center_transp.b = matd_dore.transp_color.b;

	if ( matd_dore.transp_orient_switch )
	{
	    matd_curr.edge_transp_intens = matd_dore.transp_orient_intensity;
	    matd_curr.edge_transp.r = matd_dore.transp_orient_color.r;
	    matd_curr.edge_transp.g = matd_dore.transp_orient_color.g;
	    matd_curr.edge_transp.b = matd_dore.transp_orient_color.b;

	    matd_curr.transp_exp = matd_dore.transp_exp;
	}
	else
	{
	    matd_curr.transp_exp = 0.0;
	}

	if ( !matd_dore.refr_switch )
	{
	    matd_curr.lmod |= LMODFLAG_TRANSP;
	}
	else
	{
	    matd_curr.lmod |= LMODFLAG_REFR;
	    matd_curr.rfridx = matd_dore.refr_index;
	}
    }
}


/*[
 *$     MATD_make_coeff
 *
 *      Steve Gray - Mon Jul 17 14:06:22 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    set material coefficient using Dore info
 *
 *      Usage:
 *		MATD_make_coeff( c_id, v0, v1, v2, v3)
 *		Int	c_id		(in)  material coefficient ID
 *		Float	*value		(in)  coefficient value(s)
 *
 *      Notes:  
 *		call MATD_make_dore() before calling this
 *
 *		coefficient IDs are in global.h
 *
 *		value list formats for the different coefficients
 *
 *		ID			v0	v1	v2	v3
 *
 *		MAT_COEFF_AMB		red	green	blue	-- unused --
 *		MAT_COEFF_DIFF		red	green	blue	-- unused --
 *		MAT_COEFF_SPEC		------------ unused ------------
 *		
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MATD_make_coeff( c_id, v0, v1, v2, v3)
Int	c_id;
Float	v0, v1, v2, v3;
{
#ifdef	DEBUG
printf( "MATD_make_coeff: %d\n", c_id);
#endif

    switch ( c_id ) {
    case MAT_COEFF_AMB:
	if ( matd_dore.amb_switch )
	{
	    matd_curr.amb.r = v0 * matd_dore.amb_intensity;
	    matd_curr.amb.g = v1 * matd_dore.amb_intensity;
	    matd_curr.amb.b = v2 * matd_dore.amb_intensity;

#ifdef	DEBUG
printf( "v %f %f %f: %f\n", v0, v1, v2, matd_dore.amb_intensity);
#endif
	}

	matd_curr.lmod |= LMODFLAG_AMB;
	break;
    
    case MAT_COEFF_DIFF:
	if ( matd_dore.diff_switch )
	{
	    matd_curr.diff.r = v0 * matd_dore.diff_intensity;
	    matd_curr.diff.g = v1 * matd_dore.diff_intensity;
	    matd_curr.diff.b = v2 * matd_dore.diff_intensity;

#ifdef	DEBUG
printf( "v %f %f %f: %f\n", v0, v1, v2, matd_dore.diff_intensity);
#endif

	}
	matd_curr.lmod |= LMODFLAG_LIGHT;
	break;
    
    case MAT_COEFF_SPEC:
	matd_curr.spec.r = matd_dore.spec_intensity * matd_dore.spec_color.r;
	matd_curr.spec.g = matd_dore.spec_intensity * matd_dore.spec_color.g;
	matd_curr.spec.b = matd_dore.spec_intensity * matd_dore.spec_color.b;
	matd_curr.exponent = matd_dore.spec_exponent;

#ifdef	DEBUG
printf( "clr %f %f %f: %f\n", matd_dore.spec_color.r, matd_dore.spec_color.g, 
	matd_dore.spec_color.b, matd_dore.amb_intensity);
#endif

	matd_curr.lmod |= LMODFLAG_LIGHT;
	break;
    
    default:
	DDerror( ERR_SEVERE_INTERNAL_ERROR, "MATD_make_coeff", 
		    "unknown material coefficient");
	break;
    }
}


/*[
 *$     MATD_texmap_color
 *
 *      Steve Gray - Mon Jul 31 15:15:08 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    set to texture map color (diffuse and ambient)
 *
 *      Usage:
 *		MATD_texmap_color( dore_map)
 *		Dore_texture_map	*dore_map	(in)  Dore texture map
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *	01/17/90	add uv transformation matrix			gray
 *
 *]
 */


MATD_texmap_color( dore_map)
Dore_texture_map	*dore_map;
{
Int		t, map_index;
Int		uv_index, extend, wrap, aa_type, type;
Matrix_3	matrix;
DtInt		raster_type;
Char		amb_string[64];


    for ( t = 0; t < dore_map->num_textures; t++ )
    {
	if ( dore_map->mapping[t] != DcStdTableLookup )
	{
	    DDerror( ERR_VALUE_OUT_OF_RANGE, "MATD_texmap_color", 
			"unknown mapping type");
	}
	else
	{
	    matd_get_map_params( &(dore_map->attributes[t]), &uv_index, &extend, 
				 &wrap, &aa_type, &type, matrix);

	    raster_type = DsInqObjClassId( dore_map->raster[t]);

	    if ( raster_type == DsInqClassId( "DoRaster") )
	    {
		map_index = MATD_map_add_raster( dore_map->raster[t]);
	    }
	    else if ( raster_type == DsInqClassId( "DoFileRaster") )
	    {
		map_index = MATD_map_add_file( dore_map->raster[t]);
	    }

	    if ( mlr_mode == MLR_MODE_FILE )
	    {
		matd_print_texmap_info( "diff", map_index, uv_index, extend, 
					wrap, aa_type, type, matrix);
		
		sprintf( amb_string, "amb range 0.0 %f", 
			 matd_dore.amb_intensity);

		matd_print_texmap_info( amb_string, map_index, uv_index, extend,
					wrap, aa_type, type, matrix);
	    }
	    else
	    {
#ifdef	RENDER_DIRECT
	        MLR_texmap( TEXFLAG_DIFF, map_index, uv_index, matrix, extend, 
			    wrap, aa_type, type, 0.0, 1.0);
	    
	        MLR_texmap( TEXFLAG_AMB, map_index, uv_index, matrix, extend, 
			    wrap, aa_type, type, 0.0, matd_dore.amb_intensity);
#endif
            }
	}
    }
}



/*[
 *$     MATD_texmap_bump
 *
 *      Steve Gray - Mon Jul 31 15:15:08 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    set to texture map normals
 *
 *      Usage:
 *		MATD_texmap_bump( dore_map)
 *		Dore_texture_map	*dore_map	(in)  Dore texture map
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *	01/17/90	add uv transformation matrix			gray
 *
 *]
 */


MATD_texmap_bump( dore_map)
Dore_texture_map	*dore_map;
{
Int		t, map_index;
Int		uv_index, extend, wrap, aa_type, type;
Matrix_3	matrix;
DtInt		raster_type;


    for ( t = 0; t < dore_map->num_textures; t++ )
    {
	if ( dore_map->mapping[t] != DcStdBumpMap )
	{
	    DDerror( ERR_VALUE_OUT_OF_RANGE, "MATD_texmap_bump", 
			"unknown mapping type");
	}
	else
	{
	    matd_get_map_params( &(dore_map->attributes[t]), &uv_index, &extend,
				 &wrap, &aa_type, &type, matrix);

	    raster_type = DsInqObjClassId( dore_map->raster[t]);

	    if ( raster_type == DsInqClassId( "DoRaster") )
	    {
		map_index = MATD_map_add_raster( dore_map->raster[t]);
	    }
	    else if ( raster_type == DsInqClassId( "DoFileRaster") )
	    {
		map_index = MATD_map_add_file( dore_map->raster[t]);
	    }

            /* force type to be replace, regardless of the DoTextureOp
               attribute value. Other modes (multiply, blend, add) don't
               work with bump mapping in Media Logic renderer */
	    type = TMAP_TYPE_REPLACE;
	    if ( mlr_mode == MLR_MODE_FILE )
	    {
		matd_print_texmap_info( "bump", map_index, uv_index, extend, 
					wrap, aa_type, type, matrix);
		fprintf( mlr_files.object_fp, "nf 4.0\n");
	    }
	    else
	    {
#ifdef	RENDER_DIRECT
	        MLR_texmap( TEXFLAG_BUMP, map_index, uv_index,
		            matrix, extend, wrap, aa_type, type,
		            0.0, 1.0);
	        MLR_nf( 1.0);
#endif
	    }
        }
    }
}

/*[
 *$     MATD_texmap_transp_intens
 *
 *      Steve Gray - Mon Jul 31 15:15:08 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    set to texture map transparency intensity
 *
 *      Usage:
 *		MATD_texmap_transp_intens( dore_map)
 *		Dore_texture_map	*dore_map	(in)  Dore texture map
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *	01/17/90	add uv transformation matrix			gray
 *
 *]
 */


MATD_texmap_transp_intens( dore_map)
Dore_texture_map	*dore_map;
{
Int		t, map_index;
Int		uv_index, extend, wrap, aa_type, type;
Matrix_3	matrix;
DtInt		raster_type;


    for ( t = 0; t < dore_map->num_textures; t++ )
    {
	if ( dore_map->mapping[t] != DcStdTableLookup )
	{
	    DDerror( ERR_VALUE_OUT_OF_RANGE, "MATD_texmap_transp_intens", 
			"unknown mapping type");
	}
	else
	{
            matd_get_map_params( &(dore_map->attributes[t]), &uv_index, &extend,
			         &wrap, &aa_type, &type, matrix);

	    raster_type = DsInqObjClassId( dore_map->raster[t]);

	    if ( raster_type == DsInqClassId( "DoRaster") )
	    {
	        map_index = MATD_map_add_raster( dore_map->raster[t]);
	    }
	    else if ( raster_type == DsInqClassId( "DoFileRaster") )
	    {
	        map_index = MATD_map_add_file( dore_map->raster[t]);
	    }

            if ( mlr_mode == MLR_MODE_FILE )
            {
	        matd_print_texmap_info( "transpintens", map_index, 
				        uv_index, extend, wrap, aa_type, type,
				        matrix);
            }
            else
            {
#ifdef	RENDER_DIRECT
	        MLR_texmap( TEXFLAG_TRANSP_INTENS, 
			    map_index, uv_index, matrix, extend, wrap, aa_type, 
			    type, 0.0, 1.0);
#endif
            }
	}
    }
}


/*[
 *$     MATD_rflmap
 *
 *      Steve Gray - Mon Jul 31 15:15:08 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    set to map environment
 *
 *      Usage:
 *		MATD_rflmap( dore_map)
 *		Dore_texture_map	*dore_map	(in)  Dore texture map
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *	01/17/90	add uv transformation matrix			gray
 *	01/17/90	get full Dore parameter set			gray
 *
 *]
 */


MATD_rflmap( dore_map)
Dore_texture_map	*dore_map;
{
Int		t, map_index;
Int		uv_index, extend, wrap, aa_type, type;
Matrix_3	matrix;
DtInt		raster_type;


    for ( t = 0; t < dore_map->num_textures; t++ )
    {
	if ( dore_map->mapping[t] != DcStdSphereEnvironMap )
	{
	    DDerror( ERR_VALUE_OUT_OF_RANGE, "MATD_rflmap", 
			"unknown mapping type");
	}
	else
	{
            matd_get_map_params( &(dore_map->attributes[t]), &uv_index, &extend,
			         &wrap, &aa_type, &type, matrix);

	    raster_type = DsInqObjClassId( dore_map->raster[t]);

	    if ( raster_type == DsInqClassId( "DoRaster") )
	    {
		map_index = MATD_map_add_raster( dore_map->raster[t]);
	    }
	    else if ( raster_type == DsInqClassId( "DoFileRaster") )
	    {
		map_index = MATD_map_add_file( dore_map->raster[t]);
	    }

	    if ( mlr_mode == MLR_MODE_FILE )
	    {
		matd_print_texmap_info( "rflenv", map_index, uv_index, extend, 
					wrap, aa_type, type, matrix);
	    }
	    else
	    {
#ifdef	RENDER_DIRECT
	        MLR_texmap( TEXFLAG_RFLMAP, map_index, uv_index, matrix, extend,
			    wrap, aa_type, type, 0.0, 1.0);
#endif
	    }

	    matd_curr.rflmap_present = TRUE;
        }
    }
}




/*[
 *$     MATD_make_complete
 *
 *      Steve Gray - Mon Jul 17 14:05:09 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    complete material definition
 *
 *      Usage:
 *		MATD_make_complete()
 *
 *      Notes:  
 *		prints current material info to object file
 *
 *?
 *!     Changes History
 *
 *	01/23/90	fix shadow color				gray
 *
 *]
 */

MATD_make_complete()
{
    if ( (matd_curr.lmod & LMODFLAG_AMB) != 0 )
    {
        if ( mlr_mode == MLR_MODE_FILE )
        {
	    fprintf( mlr_files.object_fp, "amb %f %f %f\n",
		     matd_curr.amb.r, matd_curr.amb.g, matd_curr.amb.b);
        }
        else
        {
#ifdef	RENDER_DIRECT
	    MLR_amb( matd_curr.amb.r, matd_curr.amb.g, matd_curr.amb.b);
#endif
        }
    }

    if ( (matd_curr.lmod & LMODFLAG_LIGHT) != 0 )
    {
        if ( mlr_mode == MLR_MODE_FILE )
        {
	    fprintf( mlr_files.object_fp, "diff %f %f %f\n",
		     matd_curr.diff.r, matd_curr.diff.g, matd_curr.diff.b);

	    fprintf( mlr_files.object_fp, "spec %f %f %f  %f\n",
		     matd_curr.spec.r, matd_curr.spec.g, matd_curr.spec.b,
		     matd_curr.exponent);
        }
        else
        {
#ifdef	RENDER_DIRECT
	    MLR_diff( matd_curr.diff.r, matd_curr.diff.g, matd_curr.diff.b);
	    MLR_spec( matd_curr.spec.r, matd_curr.spec.g, matd_curr.spec.b,
		      matd_curr.exponent);
#endif
	}
    }

    if ( (matd_curr.lmod & LMODFLAG_REFL) != 0 )
    {
        if ( mlr_mode == MLR_MODE_FILE )
        {
	    fprintf( mlr_files.object_fp, "refl %f %f %f\n",
		     matd_curr.refl.r, matd_curr.refl.g, matd_curr.refl.b);
	    
	    if ( matd_curr.rflmap_present )
	    {
	        fprintf( mlr_files.object_fp, "rflclr %f %f %f\n",
		         matd_curr.rflclr.r, matd_curr.rflclr.g, 
			 matd_curr.rflclr.b);
	    
	    }
        }
        else
        {
#ifdef	RENDER_DIRECT
	    MLR_refl( matd_curr.refl.r, matd_curr.refl.g, matd_curr.refl.b);

	    if ( matd_curr.rflmap_present )
	    {
	        MLR_rflclr( matd_curr.rflclr.r, matd_curr.rflclr.g, 
			    matd_curr.rflclr.b);
	    }
#endif
	}
    }
    else if ( matd_dore.refl_switch && matd_curr.rflmap_present )
    {
	matd_curr.lmod |= LMODFLAG_REFL;

	if ( mlr_mode == MLR_MODE_FILE )
	{
	    fprintf( mlr_files.object_fp, "refl 0.0 0.0 0.0\n");
	    
	    fprintf( mlr_files.object_fp, "rflclr %f %f %f\n",
		     matd_curr.spec.r, matd_curr.spec.g, 
		     matd_curr.spec.b);
	}
	else
	{
#ifdef	RENDER_DIRECT
	    MLR_refl( 0.0, 0.0, 0.0);
	    MLR_rflclr( matd_curr.spec.r, matd_curr.spec.g, 
			matd_curr.spec.b);
#endif
	}
    }

    if ( (matd_curr.lmod & (LMODFLAG_TRANSP | LMODFLAG_REFR)) != 0 )
    {
        if ( mlr_mode == MLR_MODE_FILE )
        {
	    fprintf(mlr_files.object_fp,"transp %f %f %f %f  %f %f %f %f  %f\n",
		    matd_curr.center_transp_intens, matd_curr.center_transp.r,
			matd_curr.center_transp.g, matd_curr.center_transp.b,
		    matd_curr.edge_transp_intens, matd_curr.edge_transp.r,
			matd_curr.edge_transp.g, matd_curr.edge_transp.b,
		    matd_curr.transp_exp);
        }
        else
        {
#ifdef	RENDER_DIRECT
	    MLR_transp( matd_curr.center_transp_intens, 
			matd_curr.center_transp.r, 
			matd_curr.center_transp.g, 
			matd_curr.center_transp.b,
	                matd_curr.edge_transp_intens, 
			matd_curr.edge_transp.r, 
			matd_curr.edge_transp.g, 
			matd_curr.edge_transp.b,
			matd_curr.transp_exp);
#endif
	}
    }

    if ( (matd_curr.lmod & LMODFLAG_REFR) != 0 )
    {
        if ( mlr_mode == MLR_MODE_FILE )
        {
	    fprintf( mlr_files.object_fp, "rfridx %f\n", matd_curr.rfridx);
        }
        else
        {
#ifdef	RENDER_DIRECT
	    MLR_rfridx( matd_curr.rfridx);, 
#endif
	}
    }

/* print light model info */
    if ( mlr_mode == MLR_MODE_FILE )
    {
	fprintf( mlr_files.object_fp, "lmod");
	if ( (matd_curr.lmod & LMODFLAG_AMB) != 0 )
	{
	    fprintf( mlr_files.object_fp, " amb");
	}
	if ( (matd_curr.lmod & LMODFLAG_LIGHT) != 0 )
	{
	    fprintf( mlr_files.object_fp, " light");
	}
	if ( (matd_curr.lmod & LMODFLAG_REFL) != 0 )
	{
	    fprintf( mlr_files.object_fp, " refl");
	}
	if ( (matd_curr.lmod & LMODFLAG_SHAD) != 0 )
	{
	    fprintf( mlr_files.object_fp, " shad");
	}
	if ( (matd_curr.lmod & LMODFLAG_TRANSP) != 0 )
	{
	    fprintf( mlr_files.object_fp, " transp");
	}
	if ( (matd_curr.lmod & LMODFLAG_REFR) != 0 )
	{
	    fprintf( mlr_files.object_fp, " refr");
	}
	fprintf( mlr_files.object_fp, "\n");

        fprintf( mlr_files.object_fp, "end\n");
    }
    else
    {
#ifdef	RENDER_DIRECT
	MLR_lmod( (Short) matd_curr.lmod);
	MLR_ma( MA_END);
#endif
    }

/* update the global light model */
    matd_lmod |= matd_curr.lmod;
}


/*[
 *$     MATD_print_gllmod
 *
 *      Steve Gray - Mon Jul 17 14:05:09 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    print global light model info
 *
 *      Usage:
 *		MATD_print_gllmod()
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MATD_print_gllmod()
{
#ifdef	DEBUG
printf( "MATD_print_gllmod, gllmod = %x\n", matd_lmod);
#endif

    if ( mlr_mode == MLR_MODE_FILE )
    {
	fprintf( mlr_files.camera_fp, "gllmod");
	if ( (matd_lmod & LMODFLAG_AMB) != 0 )
	{
	    fprintf( mlr_files.camera_fp, " amb");
	}
	if ( (matd_lmod & LMODFLAG_LIGHT) != 0 )
	{
	    fprintf( mlr_files.camera_fp, " light");
	}
	if ( (matd_lmod & LMODFLAG_REFL) != 0 )
	{
	    fprintf( mlr_files.camera_fp, " refl");
	}
	if ( (matd_lmod & LMODFLAG_REFR) != 0 )
	{
	    fprintf( mlr_files.camera_fp, " refr");
	}
	if ( (matd_lmod & LMODFLAG_TRANSP) != 0 )
	{
	    fprintf( mlr_files.camera_fp, " transp");
	}
	fprintf( mlr_files.camera_fp, "\n");
    }
    else
    {
#ifdef	RENDER_DIRECT
	MLR_gllmod( (Short) matd_lmod);
#endif
    }

    if ( (matd_lmod & LMODFLAG_SHAD) != 0 )
    {
	if ( mlr_mode == MLR_MODE_FILE )
	{
	    fprintf( mlr_files.camera_fp, "shadlim 1\n");
	}
	else
	{
#ifdef	RENDER_DIRECT
	    MLR_shadlim( FALSE, 1, 1);
#endif
	}

	mlr_limits.vox = 8;
    }
}

/*[
 *$     matd_get_map_params
 *
 *      Steve Gray - Tue Nov 14 13:54:27 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    get mapping parameters from Dore environment
 *
 *      Usage:
 *		matd_get_map_params( dore_att, uv_index, extend, wrap, aa_type,
 *				     type, matrix)
 *
 *		dot_stdtexatt	*dore_att	(in)  dore attributes
 *		Int		*uv_index	(out) uv coord index
 *		Int		*extend		(out) coordinate extend mode
 *		Int		*wrap		(out) anti-aliasing wrap mode
 *		Int		*aa_type	(out) anti-aliasing type
 *		Int		*type		(out) map type
 *		Matrix_3	matrix		(out) UV transformation matrix
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *	01/17/90	add uv transformation matrix			gray
 *	01/18/90	fix extend mode to work seperatly in U and V	gray
 *
 *]
 */

matd_get_map_params( dore_att, uv_index, extend, wrap, aa_type, type, matrix)
dot_stdtexatt	*dore_att;
Int		*uv_index;
Int		*extend;
Int		*wrap;
Int		*aa_type;
Int		*type;
Matrix_3	matrix;
{
    DtShort row, column;

/* convert to Media Logic info */
    *uv_index = dore_att->uv.idx;

    switch ( dore_att->uv.extend[0] ) {
    case DcExtendNone:
	*extend = TMAP_EXTEND_NONE_U;
	*wrap = TMAP_WRAP_NONE;
	break;
    
    case DcExtendBlack:
	*extend = TMAP_EXTEND_BLACK_U;
	*wrap = TMAP_WRAP_NONE;
	break;
    
    case DcExtendClamp:
	*extend = TMAP_EXTEND_CLAMP_U;
	*wrap = TMAP_WRAP_NONE;
	break;
    
    case DcExtendRepeat:
	*extend = TMAP_EXTEND_REPEAT_U;
	*wrap = TMAP_WRAP_U;
	break;
    
    default:
	DDerror( ERR_VALUE_OUT_OF_RANGE, "matd_get_map_params", 
		    "unknown extend mode");

	*extend = TMAP_EXTEND_NONE_U;		/* set default */
	*wrap = TMAP_WRAP_NONE;

	break;
    }

    switch ( dore_att->uv.extend[1] ) {
    case DcExtendNone:
	*extend |= TMAP_EXTEND_NONE_V;
	break;
    
    case DcExtendBlack:
	*extend |= TMAP_EXTEND_BLACK_V;
	break;
    
    case DcExtendClamp:
	*extend |= TMAP_EXTEND_CLAMP_V;
	break;
    
    case DcExtendRepeat:
	*extend |= TMAP_EXTEND_REPEAT_V;
	*wrap |= TMAP_WRAP_V;
	break;
    
    default:
	DDerror( ERR_VALUE_OUT_OF_RANGE, "matd_get_map_params", 
		    "unknown extend mode");
	break;
    }

    switch ( dore_att->operator ) {
    case DcTextureReplace:
	*type = TMAP_TYPE_REPLACE;
	break;
    
    case DcTextureMultiply:
	*type = TMAP_TYPE_MULTIPLY;
	break;
    
    case DcTextureBlend:
	*type = TMAP_TYPE_COMPOSITE;
	break;
    
    case DcTextureAdd:
	*type = TMAP_TYPE_ADD;
	break;
    
    default:
	DDerror( ERR_VALUE_OUT_OF_RANGE, "matd_get_map_params", 
		    "unknown map operator");

	*type = TMAP_TYPE_REPLACE;		/* set default */
	break;
    }

    switch ( dore_att->aamode ) {
    case DcTextureAANone:
    case DcTextureAABilinear:
    case DcTextureAASumArea:
	*aa_type = TMAP_AA_NONE;
	break;
    
    case DcTextureAAMIPPoint:
	*aa_type = TMAP_AA_MIP;
	break;
    
    case DcTextureAAAdaptive:
	*aa_type = TMAP_AA_ADAPTIVE;
	break;
    
    default:
	ERROR( ERR_VALUE_OUT_OF_RANGE, "matd_get_map_params", 
	       "unknown anti-alias mode");
    }

/* compute transformation matrix (transpose the Dore uv matrix) */
    for (row=0; row<3; row++)
	for (column=0; column<3; column++)
	    matrix[row][column] = dore_att->uv.matrix[column][row];
}


/*[
 *$     matd_print_texmap_info
 *
 *      Steve Gray - Tue Nov 14 14:08:21 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    print texture map info
 *
 *      Usage:
 *		matd_print_texmap_info( cname, map_index, uv_index, extend,
 *					wrap, aa_type, type)
 *
 *		Char		*cname		(in)  coefficient name(s)
 *		Int		map_index	(in)  map index
 *		Int		uv_index	(in)  uv index
 *		Int		extend		(in)  extend mode
 *		Int		wrap		(in)  wrap mode
 *		Int		aa_type		(in)  antialias type
 *		Int		type		(in)  map type
 *		Matrix_3	matrix		(in)  matrix
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *	01/18/90	fix extend mode to work seperatly in U and V	gray
 *	01/18/90	add uv coordinate matrix			gray
 *
 *]
 */

matd_print_texmap_info( cname, map_index, uv_index, extend, wrap, aa_type, type,
			matrix)
Char		*cname;
Int		map_index;
Int		uv_index;
Int		extend;
Int		wrap;
Int		aa_type;
Int		type;
Matrix_3	matrix;
{
    fprintf( mlr_files.object_fp, "texmap %s", cname);

    switch ( extend & TMAP_EXTEND_MASK_U ) {
    case TMAP_EXTEND_NONE_U:
	fprintf( mlr_files.object_fp, " limitu");
	break;

    case TMAP_EXTEND_BLACK_U:
	fprintf( mlr_files.object_fp, " blacku");
	break;

    case TMAP_EXTEND_CLAMP_U:
	fprintf( mlr_files.object_fp, " clampu");
	break;
    }

    switch ( extend & TMAP_EXTEND_MASK_V ) {
    case TMAP_EXTEND_NONE_V:
	fprintf( mlr_files.object_fp, " limitv");
	break;

    case TMAP_EXTEND_BLACK_V:
	fprintf( mlr_files.object_fp, " blackv");
	break;

    case TMAP_EXTEND_CLAMP_V:
	fprintf( mlr_files.object_fp, " clampv");
	break;
    }

    switch ( aa_type ) {
    case TMAP_AA_NONE:
	fprintf( mlr_files.object_fp, " noaa");
	break;

    case TMAP_AA_ADAPTIVE:
	fprintf( mlr_files.object_fp, " adapt");
	break;
    }

    switch ( type ) {
    case TMAP_TYPE_MULTIPLY:
	fprintf( mlr_files.object_fp, " mult");
	break;

    case TMAP_TYPE_ADD:
	fprintf( mlr_files.object_fp, " add");
	break;

    case TMAP_TYPE_COMPOSITE:
	fprintf( mlr_files.object_fp, " comp");
	break;
    }

    fprintf( mlr_files.object_fp, " %d %d", map_index, uv_index);

    fprintf( mlr_files.object_fp, " %f %f %f %f %f %f %f %f %f\n",
	     matrix[0][0], matrix[1][0], matrix[2][0],
	     matrix[0][1], matrix[1][1], matrix[2][1],
	     matrix[0][2], matrix[1][2], matrix[2][2]);
}



