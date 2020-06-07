/* vi:set ts=8 sw=4 ai:
 *[
 *$     GEOM_line.c
 *
 *      Steve Gray - Sat Jul  8 16:44:53 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	geometry conversion: line list, poly line and varline list
 *
 *      Usage:
 *		GEOM_line_list()		convert list
 *		GEOM_line_poly()		convert mesh
 *		GEOM_varline_list()		convert varlist
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
/*#include "dogen.h"*/
#include "dore.h"
#include "dore/dore_develop/private/linlst.h"
#include "dore/dore_develop/private/plylin.h"
#include "dore/dore_develop/private/varlinlst.h"
#include "dore/dore_develop/private/datptr.h"

/* Render includes */
#include "primitives.h"

/* MLR Dore interface includes */
#include "global.h"
#include "geom.h"


/*[
 *$     GEOM_line_list
 *
 *      Steve Gray - Wed Dec 13 15:48:00 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    render a line list
 *
 *      Usage:
 *		GEOM_line_list( object)
 *		DtObjectStructure	*object		(in)  object
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

GEOM_line_list( object)
DtObjectStructure	*object;
{
struct linlst	*line_list;
DtNPoint3	*vtx_loc;
DtNPoint3	*vtx_norm;
DtNColor3	*vtx_color;
DtMatrix4x4	vtx_mat;
DtPFI		routine, DDclass_InqMethod();

Simple_rgb	next_color;
Simple_rgb	prev_color;

Int		v, t, vr;
Int		base_index;
Int		uv_base_index;
Int		local_material_index;
Int		vertex_colors_present;
Int		vertex_normals_present;

typedef DtObjectStructure *dot_object;

DtInt			uv_count, uv_set, uv;
DtReal			width;
Float			line_width;

Char		line_output_line[256];
Char		line_output_next[256];

Float		u_list[50];
Float		v_list[50];

Int		vr_list[2];
Int		tr_list[2];
Int		mr_list[2];

DtSwitch	diffuse_map_on;
DtSwitch	bump_map_on;
DtSwitch	transp_map_on;
DtSwitch	env_map_on;

Dore_texture_map	diffuse_map;
Dore_texture_map	bump_map;
Dore_texture_map	transp_map;
Dore_texture_map	env_map;

DtInt		off_count;
DtObject	off_list[LIT_MAX];


#ifdef	DEBUG
printf( "GEOM_line_list\n");
#endif



    line_list = (struct linlst *) (object->data);

/* get the line width */
    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
        routine = DDclass_InqMethod( DcTypeAttLineWidth,
				        DcMethodInqGlbAttVal);

        (*routine)( &width);
	line_width = width;
    }

/* check the texture map switch */
    if ( line_list->uv_count != 0 )
    {
	routine = DDclass_InqMethod( DsInqClassId("AttTextureMapDiffuseColorSwitch"),
				        DcMethodInqGlbAttVal);
    
	(*routine)( &diffuse_map_on);

	routine = DDclass_InqMethod( DsInqClassId("AttTextureMapBumpSwitch"),
			    		DcMethodInqGlbAttVal);
    
	(*routine)( &bump_map_on);

	routine = DDclass_InqMethod( DsInqClassId("AttTextureMapTranspIntensSwitch"),
			    		DcMethodInqGlbAttVal);
    
	(*routine)( &transp_map_on);
    }
    else
    {
	diffuse_map_on = FALSE;
	bump_map_on = FALSE;
	transp_map_on = FALSE;
    }

/* if we are not in the count phase of a direct render */
    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
/* get the transformation matrix for this set of control points */
        routine = DDclass_InqMethod( DcTypeAttLcstowcsmat,
				        DcMethodInqGlbAttVal);
    
        (*routine)( vtx_mat);

/* check for environment (reflection) map */
        routine = DDclass_InqMethod( DsInqClassId("AttTextureMapEnvironSwitch"),
			                DcMethodInqGlbAttVal);
    
        (*routine)( &env_map_on);

/* if we are environment mapping */
        if ( env_map_on )
        {
#ifdef	DEBUG
printf( "env map on\n");
#endif

	    routine = DDclass_InqMethod( DsInqClassId("AttTextureMapEnviron"), 
					    DcMethodInqGlbAttVal);
	
	    (*routine)( TMAP_MAX_LEVELS, 
			&env_map.num_textures,
			env_map.mapping,
			env_map.raster,
			sizeof(dot_stdtexatt),
			env_map.attributes,
			env_map.attribute_sizes);

	    if (env_map.raster[0] == DcNullObject)
                /* no env map specified */
                env_map_on = FALSE;
        }

/* if we are texture mapping */
        if ( diffuse_map_on )
        {
#ifdef	DEBUG
printf( "diffuse map on\n");
#endif

	    routine = DDclass_InqMethod( DsInqClassId("AttTextureMapDiffuseColor"), 
					    DcMethodInqGlbAttVal);
	
	    (*routine)( TMAP_MAX_LEVELS, 
			&diffuse_map.num_textures,
			diffuse_map.mapping,
			diffuse_map.raster,
			sizeof(dot_stdtexatt),
			diffuse_map.attributes,
			diffuse_map.attribute_sizes);

	    if (diffuse_map.raster[0] == DcNullObject)
                /* no diffuse map specified */
                diffuse_map_on = FALSE;
        }

        if ( transp_map_on )
        {
#ifdef	DEBUG
printf( "dissolve map on\n");
#endif

	    routine = DDclass_InqMethod( DsInqClassId("AttTextureMapTranspIntens"), 
					    DcMethodInqGlbAttVal);
	
	    (*routine)( TMAP_MAX_LEVELS, 
			&transp_map.num_textures,
			transp_map.mapping,
			transp_map.raster,
			sizeof(dot_stdtexatt),
			transp_map.attributes,
			transp_map.attribute_sizes);

	    if (transp_map.raster[0] == DcNullObject)
                /* no transp map specified */
                transp_map_on = FALSE;
        }

/* if we are bump mapping */
        if ( bump_map_on )
        {
#ifdef	DEBUG
printf( "bump map on\n");
#endif


	    routine = DDclass_InqMethod( DsInqClassId("AttTextureMapBump"), 
					    DcMethodInqGlbAttVal);
	
	    (*routine)( TMAP_MAX_LEVELS, 
			&bump_map.num_textures,
			bump_map.mapping,
			bump_map.raster,
			sizeof(dot_stdtexatt),
			bump_map.attributes,
			bump_map.attribute_sizes);

	    if (bump_map.raster[0] == DcNullObject)
                /* no bump map specified */
                bump_map_on = FALSE;
        }

/* save base control point index */
        base_index = mlr_limits.cp;
        uv_base_index = mlr_limits.tex[0];

/* write out all the vertecies */
        vtx_loc = (DtNPoint3 *) (line_list->vertex_location);

        if ( (vtx_norm = (DtNPoint3 *) (line_list->vertex_normal)) != NULL )
        {
	    vertex_normals_present = TRUE;
        }
        else
        {
	    vertex_normals_present = FALSE;
        }

/* print the matrix */
	if ( mlr_mode == MLR_MODE_FILE )
	{
            fprintf( mlr_files.object_fp, 
		    "mat %f %f %f %f %f %f %f %f %f %f %f %f\n",
	            vtx_mat[0][0], vtx_mat[0][1], vtx_mat[0][2], vtx_mat[0][3],
	            vtx_mat[1][0], vtx_mat[1][1], vtx_mat[1][2], vtx_mat[1][3],
	            vtx_mat[2][0], vtx_mat[2][1], vtx_mat[2][2], vtx_mat[2][3]);
	}
	else if ( mlr_mode == MLR_MODE_DIRECT )
	{
	    geom_xfm_cp( vtx_mat);
	}

#ifdef	DEBUG
{ DtReal *v;
  DtInt	  i;
printf( "vertex list\n");
v = (DtReal *) line_list->vertex_location;
for ( i = 0; i < line_list->line_count; i++, v += 6 )
printf( " %lf %lf %lf %lf %lf %lf\n", v[0], v[1], v[2], v[3], v[4], v[5]);
}
#endif


	if ( mlr_mode == MLR_MODE_FILE )
	{
            fprintf( mlr_files.object_fp, "convex on\n");
	}
	else
	{
#ifdef	RENDER_DIRECT
	    MLR_convex( TRUE);
#endif
	}
    }

    for ( v = line_list->line_count * 2; v-- > 0; vtx_loc++, vtx_norm++ )
    {
	if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
	{
	    if ( vertex_normals_present )
	    {
	        if ( mlr_mode == MLR_MODE_FILE )
	        {
		    fprintf( mlr_files.object_fp, "cn %f %f %f  %f %f %f\n",
			     vtx_loc->x, vtx_loc->y, vtx_loc->z, 
			     -vtx_norm->x, -vtx_norm->y, -vtx_norm->z);
		}
		else
		{
#ifdef	RENDER_DIRECT
		    MLR_cn( (Float) vtx_loc->x, (Float) vtx_loc->y, 
				(Float) vtx_loc->z, 
			    (Float) -vtx_norm->x, (Float) -vtx_norm->y, 
				(Float) -vtx_norm->z);
#endif
		}
	    }
	    else
	    {
	        if ( mlr_mode == MLR_MODE_FILE )
	        {
		    fprintf( mlr_files.object_fp, "cp %f %f %f\n",
			     vtx_loc->x, vtx_loc->y, vtx_loc->z);
		}
		else
		{
#ifdef	RENDER_DIRECT
		    MLR_cp( (Float) vtx_loc->x, (Float) vtx_loc->y, 
			    (Float) vtx_loc->z);
#endif
		}
	    }
	}
	
	(mlr_limits.cp)++;
    }

/* output texture map coords (if any) */
    if ( diffuse_map_on || bump_map_on || transp_map_on )
    {
	uv_count = line_list->uv_count;

	for ( uv = 0; uv < line_list->line_count * 2; uv++ )
	{
	    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
	    {
		if ( mlr_mode == MLR_MODE_FILE )
		{
		    fprintf( mlr_files.object_fp, "tex");

		    for ( uv_set = 0; uv_set < uv_count; uv_set++ )
		    {
			fprintf( mlr_files.object_fp, " %f %f",
				 (line_list->uv_list)[uv_set][uv][0],
				 (line_list->uv_list)[uv_set][uv][1]);
		    }

		    fprintf( mlr_files.object_fp, "\n");
		}
		else
		{
		    for ( uv_set = 0; uv_set < uv_count; uv_set++ )
		    {
			u_list[uv_set] = (line_list->uv_list)[uv_set][uv][0];
			v_list[uv_set] = (line_list->uv_list)[uv_set][uv][1];
		    }

#ifdef	RENDER_DIRECT
		    MLR_tex( uv_count, u_list, v_list);
#endif
		}
	    }

	    (mlr_limits.tex[0])++;
	    (mlr_limits.tex[1]) += uv_count;
	}
    }

/* get the first color */
    if ( (vtx_color = (DtNColor3 *) line_list->vertex_color) != NULL )
    {
	vertex_colors_present = TRUE;

	prev_color.r = vtx_color->r;
	prev_color.g = vtx_color->g;
	prev_color.b = vtx_color->b;
    }
    else
    {
	vertex_colors_present = FALSE;

	prev_color.r = 1.0;
	prev_color.g = 1.0;
	prev_color.b = 1.0;

	next_color.r = 1.0;
	next_color.g = 1.0;
	next_color.b = 1.0;
    }

    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
/* start a local material group */
        MATD_make_local();
    }

/* start a new material */
    MATD_make_init();

    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
/* get the material properties from the global attribute stack */
        MATD_make_from_dore();

/* set the ambient, diffuse and specular info */
        if ( vertex_colors_present )
        {
	    MATD_make_coeff( MAT_COEFF_AMB, 
			    prev_color.r, prev_color.g, prev_color.b);
	    MATD_make_coeff( MAT_COEFF_DIFF,
			    prev_color.r, prev_color.g, prev_color.b);
	    MATD_make_coeff( MAT_COEFF_SPEC);
        }
    
/* if diffuse mapping is present */
        if ( diffuse_map_on )
        {
	    MATD_texmap_color( &diffuse_map);
        }

/* if dissolve mapping is present */
        if ( transp_map_on )
        {
	    MATD_texmap_transp_intens( &transp_map);
        }

/* if bump mapping is present */
        if ( bump_map_on )
        {
	    MATD_texmap_bump( &bump_map);
        }

/* if environment mapping is present */
        if ( env_map_on )
        {
	    MATD_rflmap( &env_map);
        }

/* close the material definition */
        MATD_make_complete();
    }

/* we are on the first local material */
    local_material_index = 0;

/* write out all the lines */
#ifdef	DEBUG
printf( "line count %d\n", line_list->line_count);
#endif

    v = 0;
    for ( t = line_list->line_count; t-- > 0; v += 2 )
    {
	if ( mlr_mode == MLR_MODE_FILE )
	{
/* start the line command */
	    if ( diffuse_map_on || bump_map_on || transp_map_on )
	    {
		sprintf( line_output_line, "linetex b b %d n %f", 
			 mlr_limits.obj[0], line_width);
	    }
	    else
	    {
		sprintf( line_output_line, "line b b %d n %f", 
			 mlr_limits.obj[0], line_width);
	    }
	}

/* for each vertex reference */
	for ( vr = 0; vr < 2; vr++ )
	{
/* get the next vertex color */
	    if ( vertex_colors_present )
	    {
		next_color.r = vtx_color[v + vr].r;
		next_color.g = vtx_color[v + vr].g;
		next_color.b = vtx_color[v + vr].b;
	    }

/* if it is not the same as the previous color */
	    if ( (next_color.r != prev_color.r) || 
		 (next_color.g != prev_color.g) ||
		 (next_color.b != prev_color.b) )
	    {
/* make a new material (note that the dore attributes have not changed) */
		MATD_make_init();

		if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
		{
		    MATD_make_coeff( MAT_COEFF_AMB, next_color.r, next_color.g, 
				    next_color.b);
		    MATD_make_coeff( MAT_COEFF_DIFF, next_color.r, next_color.g,
				    next_color.b);
		    MATD_make_coeff( MAT_COEFF_SPEC);

/* if diffuse mapping is present */
		    if ( diffuse_map_on )
		    {
		        MATD_texmap_color( &diffuse_map);
		    }

/* if dissolve mapping is present */
		    if ( transp_map_on )
		    {
		        MATD_texmap_transp_intens( &transp_map);
		    }

/* if bump mapping is present */
		    if ( bump_map_on )
		    {
		        MATD_texmap_bump( bump_map);
		    }

/* if environment mapping is present */
		    if ( env_map_on )
		    {
		        MATD_rflmap( env_map);
		    }

		    MATD_make_complete();
		}

/* set the prev to be the next */
		prev_color.r = next_color.r;
		prev_color.g = next_color.g;
		prev_color.b = next_color.b;

/* increment the local material index */
		local_material_index++;
	    }

	    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
	    {
		if ( mlr_mode == MLR_MODE_FILE )
		{
		    if ( diffuse_map_on || bump_map_on || transp_map_on )
		    {
			sprintf( line_output_next, "  %d %d %d",
				 base_index + v + vr, local_material_index,
				 uv_base_index + v + vr);
		    }
		    else
		    {
			sprintf( line_output_next, "  %d %d",
				 base_index + v + vr, local_material_index);
		    }
	
		    strcat( line_output_line, line_output_next);
		}
		else
		{
		    vr_list[vr] = base_index + v + vr;
		    tr_list[vr] = uv_base_index + v + vr;
		    mr_list[vr] = local_material_index;
		}
	    }
	}

/* complete the line command */
	if ( mlr_mode == MLR_MODE_FILE )
	{
	    fprintf( mlr_files.object_fp, "%s\n", line_output_line);
	}
	else if ( mlr_mode == MLR_MODE_DIRECT )
	{
	    if ( diffuse_map_on || bump_map_on || transp_map_on )
	    {
#ifdef	RENDER_DIRECT
		MLR_linetex( mlr_limits.obj[0], USE_IN_BOTH, INTERP_BOTH, FALSE,
			     line_width, vr_list[0], mr_list[0], tr_list[0],
			     vr_list[1], mr_list[1], tr_list[1]);
#endif
	    }
	    else
	    {
#ifdef	RENDER_DIRECT
		MLR_line( mlr_limits.obj[0], USE_IN_BOTH, INTERP_BOTH, FALSE,
			  line_width, vr_list[0], mr_list[0], 
			  vr_list[1], mr_list[1]);
#endif
	    }
	}

/* increment line limit */
	(mlr_limits.line[0])++;

	if ( diffuse_map_on || bump_map_on || transp_map_on )
	{
	    (mlr_limits.line[1])++;
	}
    }

/* get the current list of lights which are off */
    routine = DDclass_InqMethod( DsInqClassId( "AttLightSwitch"), 
				    DcMethodInqGlbAttVal);

    (*routine)( &off_count, off_list, LIT_MAX);

/* reset all light to on, process the off list, and make the lto command */
    LIT_list_reset();
    LIT_list_process( off_count, off_list);
    LIT_list_lto( mlr_limits.obj[0]);

/* increment object count */
    (mlr_limits.obj[0])++;
}

/*[
 *$     GEOM_line_poly
 *
 *      Steve Gray - Wed Dec 13 15:48:00 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    render a line poly
 *
 *      Usage:
 *		GEOM_line_poly( object)
 *		DtObjectStructure	*object		(in)  object
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

GEOM_line_poly( object)
DtObjectStructure	*object;
{
struct plylin	*line_list;
DtNPoint3	*vtx_loc;
DtNPoint3	*vtx_norm;
DtNColor3	*vtx_color;
DtMatrix4x4	vtx_mat;
DtPFI		routine, DDclass_InqMethod();

Simple_rgb	next_color;
Simple_rgb	prev_color;

Int		v, t, vr;
Int		base_index;
Int		uv_base_index;
Int		local_material_index;
Int		vertex_colors_present;
Int		vertex_normals_present;

typedef DtObjectStructure *dot_object;

DtInt			uv_count, uv_set, uv;
DtReal			width;
Float			line_width;

Char		line_output_line[256];
Char		line_output_next[256];

Float		u_list[50];
Float		v_list[50];

Int		vr_list[2];
Int		tr_list[2];
Int		mr_list[2];

DtSwitch	diffuse_map_on;
DtSwitch	bump_map_on;
DtSwitch	transp_map_on;
DtSwitch	env_map_on;

Dore_texture_map	diffuse_map;
Dore_texture_map	bump_map;
Dore_texture_map	transp_map;
Dore_texture_map	env_map;

DtInt		off_count;
DtObject	off_list[LIT_MAX];


#ifdef	DEBUG
printf( "GEOM_line_poly\n");
#endif



    line_list = (struct plylin *) (object->data);

/* get the line width */
    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
        routine = DDclass_InqMethod( DcTypeAttLineWidth,
				        DcMethodInqGlbAttVal);

        (*routine)( &width);
	line_width = width;
    }

/* check the texture map switch */
    if ( line_list->uv_count != 0 )
    {
	routine = DDclass_InqMethod( DsInqClassId("AttTextureMapDiffuseColorSwitch"),
				        DcMethodInqGlbAttVal);
    
	(*routine)( &diffuse_map_on);

	routine = DDclass_InqMethod( DsInqClassId("AttTextureMapBumpSwitch"),
			    		DcMethodInqGlbAttVal);
    
	(*routine)( &bump_map_on);

	routine = DDclass_InqMethod( DsInqClassId("AttTextureMapTranspIntensSwitch"),
			    		DcMethodInqGlbAttVal);
    
	(*routine)( &transp_map_on);
    }
    else
    {
	diffuse_map_on = FALSE;
	bump_map_on = FALSE;
	transp_map_on = FALSE;
    }

/* if we are not in the count phase of a direct render */
    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
/* get the transformation matrix for this set of control points */
        routine = DDclass_InqMethod( DcTypeAttLcstowcsmat,
				        DcMethodInqGlbAttVal);
    
        (*routine)( vtx_mat);

/* check for environment (reflection) map */
        routine = DDclass_InqMethod( DsInqClassId("AttTextureMapEnvironSwitch"),
			                DcMethodInqGlbAttVal);
    
        (*routine)( &env_map_on);

/* if we are environment mapping */
        if ( env_map_on )
        {
#ifdef	DEBUG
printf( "env map on\n");
#endif

	    routine = DDclass_InqMethod( DsInqClassId("AttTextureMapEnviron"), 
					    DcMethodInqGlbAttVal);
	
	    (*routine)( TMAP_MAX_LEVELS, 
			&env_map.num_textures,
			env_map.mapping,
			env_map.raster,
			sizeof(dot_stdtexatt),
			env_map.attributes,
			env_map.attribute_sizes);

	    if (env_map.raster[0] == DcNullObject)
                /* no env map specified */
                env_map_on = FALSE;
        }

/* if we are texture mapping */
        if ( diffuse_map_on )
        {
#ifdef	DEBUG
printf( "diffuse map on\n");
#endif

	    routine = DDclass_InqMethod( DsInqClassId("AttTextureMapDiffuseColor"), 
					    DcMethodInqGlbAttVal);
	
	    (*routine)( TMAP_MAX_LEVELS, 
			&diffuse_map.num_textures,
			diffuse_map.mapping,
			diffuse_map.raster,
			sizeof(dot_stdtexatt),
			diffuse_map.attributes,
			diffuse_map.attribute_sizes);

	    if (diffuse_map.raster[0] == DcNullObject)
                /* no diffuse map specified */
                diffuse_map_on = FALSE;
        }

        if ( transp_map_on )
        {
#ifdef	DEBUG
printf( "dissolve map on\n");
#endif

	    routine = DDclass_InqMethod( DsInqClassId("AttTextureMapTranspIntens"), 
					    DcMethodInqGlbAttVal);
	
	    (*routine)( TMAP_MAX_LEVELS, 
			&transp_map.num_textures,
			transp_map.mapping,
			transp_map.raster,
			sizeof(dot_stdtexatt),
			transp_map.attributes,
			transp_map.attribute_sizes);

	    if (transp_map.raster[0] == DcNullObject)
                /* no transp map specified */
                transp_map_on = FALSE;
        }

/* if we are bump mapping */
        if ( bump_map_on )
        {
#ifdef	DEBUG
printf( "bump map on\n");
#endif


	    routine = DDclass_InqMethod( DsInqClassId("AttTextureMapBump"), 
					    DcMethodInqGlbAttVal);
	
	    (*routine)( TMAP_MAX_LEVELS, 
			&bump_map.num_textures,
			bump_map.mapping,
			bump_map.raster,
			sizeof(dot_stdtexatt),
			bump_map.attributes,
			bump_map.attribute_sizes);

	    if (bump_map.raster[0] == DcNullObject)
                /* no bump map specified */
                bump_map_on = FALSE;
        }

/* save base control point index */
        base_index = mlr_limits.cp;
        uv_base_index = mlr_limits.tex[0];

/* write out all the vertecies */
        vtx_loc = (DtNPoint3 *) (line_list->vertex_location);

        if ( (vtx_norm = (DtNPoint3 *) (line_list->vertex_normal)) != NULL )
        {
	    vertex_normals_present = TRUE;
        }
        else
        {
	    vertex_normals_present = FALSE;
        }

/* print the matrix */
	if ( mlr_mode == MLR_MODE_FILE )
	{
            fprintf( mlr_files.object_fp, 
		    "mat %f %f %f %f %f %f %f %f %f %f %f %f\n",
	            vtx_mat[0][0], vtx_mat[0][1], vtx_mat[0][2], vtx_mat[0][3],
	            vtx_mat[1][0], vtx_mat[1][1], vtx_mat[1][2], vtx_mat[1][3],
	            vtx_mat[2][0], vtx_mat[2][1], vtx_mat[2][2], vtx_mat[2][3]);
	}
	else if ( mlr_mode == MLR_MODE_DIRECT )
	{
	    geom_xfm_cp( vtx_mat);
	}

#ifdef	DEBUG
{ DtReal *v;
  DtInt	  i;
printf( "vertex list\n");
v = (DtReal *) line_list->vertex_location;
for ( i = 0; i < line_list->vertex_count; i++, v += 3 )
printf( " %lf %lf %lf\n", v[0], v[1], v[2]);
}
#endif


	if ( mlr_mode == MLR_MODE_FILE )
	{
            fprintf( mlr_files.object_fp, "convex on\n");
	}
	else
	{
#ifdef	RENDER_DIRECT
	    MLR_convex( TRUE);
#endif
	}
    }

    for ( v = line_list->vertex_count; v-- > 0; vtx_loc++, vtx_norm++ )
    {
	if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
	{
	    if ( vertex_normals_present )
	    {
	        if ( mlr_mode == MLR_MODE_FILE )
	        {
		    fprintf( mlr_files.object_fp, "cn %f %f %f  %f %f %f\n",
			     vtx_loc->x, vtx_loc->y, vtx_loc->z, 
			     -vtx_norm->x, -vtx_norm->y, -vtx_norm->z);
		}
		else
		{
#ifdef	RENDER_DIRECT
		    MLR_cn( (Float) vtx_loc->x, (Float) vtx_loc->y, 
				(Float) vtx_loc->z, 
			    (Float) -vtx_norm->x, (Float) -vtx_norm->y, 
				(Float) -vtx_norm->z);
#endif
		}
	    }
	    else
	    {
	        if ( mlr_mode == MLR_MODE_FILE )
	        {
		    fprintf( mlr_files.object_fp, "cp %f %f %f\n",
			     vtx_loc->x, vtx_loc->y, vtx_loc->z);
		}
		else
		{
#ifdef	RENDER_DIRECT
		    MLR_cp( (Float) vtx_loc->x, (Float) vtx_loc->y, 
			    (Float) vtx_loc->z);
#endif
		}
	    }
	}
	
	(mlr_limits.cp)++;
    }

/* output texture map coords (if any) */
    if ( diffuse_map_on || bump_map_on || transp_map_on )
    {
	uv_count = line_list->uv_count;

	for ( uv = 0; uv < line_list->vertex_count; uv++ )
	{
	    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
	    {
		if ( mlr_mode == MLR_MODE_FILE )
		{
		    fprintf( mlr_files.object_fp, "tex");

		    for ( uv_set = 0; uv_set < uv_count; uv_set++ )
		    {
			fprintf( mlr_files.object_fp, " %f %f",
				 (line_list->uv_list)[uv_set][uv][0],
				 (line_list->uv_list)[uv_set][uv][1]);
		    }

		    fprintf( mlr_files.object_fp, "\n");
		}
		else
		{
		    for ( uv_set = 0; uv_set < uv_count; uv_set++ )
		    {
			u_list[uv_set] = (line_list->uv_list)[uv_set][uv][0];
			v_list[uv_set] = (line_list->uv_list)[uv_set][uv][1];
		    }

#ifdef	RENDER_DIRECT
		    MLR_tex( uv_count, u_list, v_list);
#endif
		}
	    }

	    (mlr_limits.tex[0])++;
	    (mlr_limits.tex[1]) += uv_count;
	}
    }

/* get the first color */
    if ( (vtx_color = (DtNColor3 *) line_list->vertex_color) != NULL )
    {
	vertex_colors_present = TRUE;

	prev_color.r = vtx_color->r;
	prev_color.g = vtx_color->g;
	prev_color.b = vtx_color->b;
    }
    else
    {
	vertex_colors_present = FALSE;

	prev_color.r = 1.0;
	prev_color.g = 1.0;
	prev_color.b = 1.0;

	next_color.r = 1.0;
	next_color.g = 1.0;
	next_color.b = 1.0;
    }

    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
/* start a local material group */
        MATD_make_local();
    }

/* start a new material */
    MATD_make_init();

    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
/* get the material properties from the global attribute stack */
        MATD_make_from_dore();

/* set the ambient, diffuse and specular info */
        if ( vertex_colors_present )
        {
	    MATD_make_coeff( MAT_COEFF_AMB, 
			    prev_color.r, prev_color.g, prev_color.b);
	    MATD_make_coeff( MAT_COEFF_DIFF,
			    prev_color.r, prev_color.g, prev_color.b);
	    MATD_make_coeff( MAT_COEFF_SPEC);
        }
    
/* if diffuse mapping is present */
        if ( diffuse_map_on )
        {
	    MATD_texmap_color( &diffuse_map);
        }

/* if dissolve mapping is present */
        if ( transp_map_on )
        {
	    MATD_texmap_transp_intens( &transp_map);
        }

/* if bump mapping is present */
        if ( bump_map_on )
        {
	    MATD_texmap_bump( &bump_map);
        }

/* if environment mapping is present */
        if ( env_map_on )
        {
	    MATD_rflmap( &env_map);
        }

/* close the material definition */
        MATD_make_complete();
    }

/* we are on the first local material */
    local_material_index = 0;

/* write out all the lines */
    v = 0;
    for ( t = line_list->vertex_count - 1; t-- > 0; v += 2 )
    {
	if ( mlr_mode == MLR_MODE_FILE )
	{
/* start the line command */
	    if ( diffuse_map_on || bump_map_on || transp_map_on )
	    {
		sprintf( line_output_line, "linetex b b %d p", 
			 mlr_limits.obj[0]);
	    }
	    else
	    {
		sprintf( line_output_line, "line b b %d p", mlr_limits.obj[0]);
	    }
	}

/* for each vertex reference */
	for ( vr = 0; vr < 2; vr++ )
	{
/* get the next vertex color */
	    if ( vertex_colors_present )
	    {
		next_color.r = vtx_color[v + vr].r;
		next_color.g = vtx_color[v + vr].g;
		next_color.b = vtx_color[v + vr].b;
	    }

/* if it is not the same as the previous color */
	    if ( (next_color.r != prev_color.r) || 
		 (next_color.g != prev_color.g) ||
		 (next_color.b != prev_color.b) )
	    {
/* make a new material (note that the dore attributes have not changed) */
		MATD_make_init();

		if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
		{
		    MATD_make_coeff( MAT_COEFF_AMB, next_color.r, next_color.g, 
				    next_color.b);
		    MATD_make_coeff( MAT_COEFF_DIFF, next_color.r, next_color.g,
				    next_color.b);
		    MATD_make_coeff( MAT_COEFF_SPEC);

/* if diffuse mapping is present */
		    if ( diffuse_map_on )
		    {
		        MATD_texmap_color( &diffuse_map);
		    }

/* if dissolve mapping is present */
		    if ( transp_map_on )
		    {
		        MATD_texmap_transp_intens( &transp_map);
		    }

/* if bump mapping is present */
		    if ( bump_map_on )
		    {
		        MATD_texmap_bump( bump_map);
		    }

/* if environment mapping is present */
		    if ( env_map_on )
		    {
		        MATD_rflmap( env_map);
		    }

		    MATD_make_complete();
		}

/* set the prev to be the next */
		prev_color.r = next_color.r;
		prev_color.g = next_color.g;
		prev_color.b = next_color.b;

/* increment the local material index */
		local_material_index++;
	    }

	    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
	    {
		if ( mlr_mode == MLR_MODE_FILE )
		{
		    if ( diffuse_map_on || bump_map_on || transp_map_on )
		    {
			sprintf( line_output_next, "  %d %d %d",
				 base_index + v + vr, local_material_index,
				 uv_base_index + v + vr);
		    }
		    else
		    {
			sprintf( line_output_next, "  %d %d",
				 base_index + v + vr, local_material_index);
		    }
	
		    strcat( line_output_line, line_output_next);
		}
		else
		{
		    vr_list[vr] = base_index + v + vr;
		    tr_list[vr] = uv_base_index + v + vr;
		    mr_list[vr] = local_material_index;
		}
	    }
	}

/* complete the line command */
	if ( mlr_mode == MLR_MODE_FILE )
	{
	    fprintf( mlr_files.object_fp, "%s\n", line_output_line);
	}
	else if ( mlr_mode == MLR_MODE_DIRECT )
	{
	    if ( diffuse_map_on || bump_map_on || transp_map_on )
	    {
#ifdef	RENDER_DIRECT
		MLR_linetex( mlr_limits.obj[0], USE_IN_BOTH, INTERP_BOTH, TRUE,
			     line_width, vr_list[0], mr_list[0], tr_list[0],
			     vr_list[1], mr_list[1], tr_list[1]);
#endif
	    }
	    else
	    {
#ifdef	RENDER_DIRECT
		MLR_line( mlr_limits.obj[0], USE_IN_BOTH, INTERP_BOTH, TRUE,
			  line_width, vr_list[0], mr_list[0], 
			  vr_list[1], mr_list[1]);
#endif
	    }
	}

/* increment line limit */
	(mlr_limits.line[0])++;

	if ( diffuse_map_on || bump_map_on || transp_map_on )
	{
	    (mlr_limits.line[1])++;
	}
    }

/* get the current list of lights which are off */
    routine = DDclass_InqMethod( DsInqClassId( "AttLightSwitch"), 
				  DcMethodInqGlbAttVal);

    (*routine)( &off_count, off_list, LIT_MAX);

/* reset all light to on, process the off list, and make the lto command */
    LIT_list_reset();
    LIT_list_process( off_count, off_list);
    LIT_list_lto( mlr_limits.obj[0]);

/* increment object count */
    (mlr_limits.obj[0])++;
}

/*[
 *$     GEOM_varline_list
 *
 *      Helga Thorvaldsdottir - Thu May 10, 1990
 *?
 *      Description:    render a varline list
 *
 *      Usage:
 *		GEOM_varline_list( object)
 *		DtObjectStructure	*object		(in)  object
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

GEOM_varline_list( object)
DtObjectStructure	*object;
{
struct varlinlst	*varline_list;
DtNPoint3	*vtx_loc;
DtNPoint3	*vtx_norm;
DtNColor3	*vtx_color;
DtMatrix4x4	vtx_mat;
DtPFI		routine, DDclass_InqMethod();

Simple_rgb	next_color;
Simple_rgb	prev_color;

Int		v, t, vr;
Int		base_index;
Int		local_material_index;
Int		vertex_colors_present;
Int		vertex_normals_present;

typedef DtObjectStructure *dot_object;

DtReal			width;
Float			varline_width;

Char		varline_output_varline[256];
Char		varline_output_next[256];

Int		vr_list[2];
Int		tr_list[2];
Int		mr_list[2];

DtInt		off_count;
DtObject	off_list[LIT_MAX];


#ifdef	DEBUG
printf( "GEOM_varline_list\n");
#endif



    varline_list = (struct varlinlst *) (object->data);

/* get the varline width */
    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
        routine = DDclass_InqMethod( DcTypeAttLineWidth,
				        DcMethodInqGlbAttVal);

        (*routine)( &width);
	varline_width = width;
    }

/* if we are not in the count phase of a direct render */
    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
/* get the transformation matrix for this set of control points */
        routine = DDclass_InqMethod( DcTypeAttLcstowcsmat,
				        DcMethodInqGlbAttVal);
    
        (*routine)( vtx_mat);


/* save base control point index */
        base_index = mlr_limits.cp;

/* write out all the vertecies */
        vtx_loc = (DtNPoint3 *) (varline_list->vertex_location);

        if ( (vtx_norm = (DtNPoint3 *) (varline_list->vertex_normal)) != NULL )
        {
	    vertex_normals_present = TRUE;
        }
        else
        {
	    vertex_normals_present = FALSE;
        }

/* print the matrix */
	if ( mlr_mode == MLR_MODE_FILE )
	{
            fprintf( mlr_files.object_fp, 
		    "mat %f %f %f %f %f %f %f %f %f %f %f %f\n",
	            vtx_mat[0][0], vtx_mat[0][1], vtx_mat[0][2], vtx_mat[0][3],
	            vtx_mat[1][0], vtx_mat[1][1], vtx_mat[1][2], vtx_mat[1][3],
	            vtx_mat[2][0], vtx_mat[2][1], vtx_mat[2][2], vtx_mat[2][3]);
	}
	else if ( mlr_mode == MLR_MODE_DIRECT )
	{
	    geom_xfm_cp( vtx_mat);
	}

#ifdef	DEBUG
{ DtReal *v;
  DtInt	  i;
printf( "vertex list\n");
v = (DtReal *) varline_list->vertex_location;
for ( i = 0; i < varline_list->varline_count; i++, v += 6 )
printf( " %lf %lf %lf %lf %lf %lf\n", v[0], v[1], v[2], v[3], v[4], v[5]);
}
#endif


	if ( mlr_mode == MLR_MODE_FILE )
	{
            fprintf( mlr_files.object_fp, "convex on\n");
	}
	else
	{
#ifdef	RENDER_DIRECT
	    MLR_convex( TRUE);
#endif
	}
    }

    for ( v = varline_list->line_count * 2; v-- > 0; vtx_loc++, vtx_norm++ )
    {
	if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
	{
	    if ( vertex_normals_present )
	    {
	        if ( mlr_mode == MLR_MODE_FILE )
	        {
		    fprintf( mlr_files.object_fp, "cn %f %f %f  %f %f %f\n",
			     vtx_loc->x, vtx_loc->y, vtx_loc->z, 
			     -vtx_norm->x, -vtx_norm->y, -vtx_norm->z);
		}
		else
		{
#ifdef	RENDER_DIRECT
		    MLR_cn( (Float) vtx_loc->x, (Float) vtx_loc->y, 
				(Float) vtx_loc->z, 
			    (Float) -vtx_norm->x, (Float) -vtx_norm->y, 
				(Float) -vtx_norm->z);
#endif
		}
	    }
	    else
	    {
	        if ( mlr_mode == MLR_MODE_FILE )
	        {
		    fprintf( mlr_files.object_fp, "cp %f %f %f\n",
			     vtx_loc->x, vtx_loc->y, vtx_loc->z);
		}
		else
		{
#ifdef	RENDER_DIRECT
		    MLR_cp( (Float) vtx_loc->x, (Float) vtx_loc->y, 
			    (Float) vtx_loc->z);
#endif
		}
	    }
	}
	
	(mlr_limits.cp)++;
    }

/* get the first color */
    if ( (vtx_color = (DtNColor3 *) varline_list->vertex_color) != NULL )
    {
	vertex_colors_present = TRUE;

	prev_color.r = vtx_color->r;
	prev_color.g = vtx_color->g;
	prev_color.b = vtx_color->b;
    }
    else
    {
	vertex_colors_present = FALSE;

	prev_color.r = 1.0;
	prev_color.g = 1.0;
	prev_color.b = 1.0;

	next_color.r = 1.0;
	next_color.g = 1.0;
	next_color.b = 1.0;
    }

    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
/* start a local material group */
        MATD_make_local();
    }

/* start a new material */
    MATD_make_init();

    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
/* get the material properties from the global attribute stack */
        MATD_make_from_dore();

/* set the ambient, diffuse and specular info */
        if ( vertex_colors_present )
        {
	    MATD_make_coeff( MAT_COEFF_AMB, 
			    prev_color.r, prev_color.g, prev_color.b);
	    MATD_make_coeff( MAT_COEFF_DIFF,
			    prev_color.r, prev_color.g, prev_color.b);
	    MATD_make_coeff( MAT_COEFF_SPEC);
        }

/* close the material definition */
        MATD_make_complete();
    }

/* we are on the first local material */
    local_material_index = 0;

/* write out all the lines */
#ifdef	DEBUG
printf( "line count %d\n", varline_list->line_count);
#endif

    v = 0;
    for ( t = varline_list->line_count; t-- > 0; v += 2 )
    {
	if ( mlr_mode == MLR_MODE_FILE )
	{
/* start the line command */
	    sprintf( varline_output_varline, "line b b %d n %f", 
			 mlr_limits.obj[0], varline_width);
	}

/* for each vertex reference */
	for ( vr = 0; vr < 2; vr++ )
	{
/* get the next vertex color */
	    if ( vertex_colors_present )
	    {
		next_color.r = vtx_color[v + vr].r;
		next_color.g = vtx_color[v + vr].g;
		next_color.b = vtx_color[v + vr].b;
	    }

/* if it is not the same as the previous color */
	    if ( (next_color.r != prev_color.r) || 
		 (next_color.g != prev_color.g) ||
		 (next_color.b != prev_color.b) )
	    {
/* make a new material (note that the dore attributes have not changed) */
		MATD_make_init();

		if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
		{
		    MATD_make_coeff( MAT_COEFF_AMB, next_color.r, next_color.g, 
				    next_color.b);
		    MATD_make_coeff( MAT_COEFF_DIFF, next_color.r, next_color.g,
				    next_color.b);
		    MATD_make_coeff( MAT_COEFF_SPEC);

		    MATD_make_complete();
		}

/* set the prev to be the next */
		prev_color.r = next_color.r;
		prev_color.g = next_color.g;
		prev_color.b = next_color.b;

/* increment the local material index */
		local_material_index++;
	    }

	    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
	    {
		if ( mlr_mode == MLR_MODE_FILE )
		{
		    sprintf( varline_output_next, "  %d %d",
				 base_index + v + vr, local_material_index);
	
		    strcat( varline_output_varline, varline_output_next);
		}
		else
		{
		    vr_list[vr] = base_index + v + vr;
		    mr_list[vr] = local_material_index;
		}
	    }
	}

/* complete the varline command */
	if ( mlr_mode == MLR_MODE_FILE )
	{
	    fprintf( mlr_files.object_fp, "%s\n", varline_output_varline);
	}
	else if ( mlr_mode == MLR_MODE_DIRECT )
	{
#ifdef	RENDER_DIRECT
		MLR_line( mlr_limits.obj[0], USE_IN_BOTH, INTERP_BOTH, FALSE,
			  varline_width, vr_list[0], mr_list[0], 
			  vr_list[1], mr_list[1]);
#endif
	}

/* increment varline limit */
	(mlr_limits.line[0])++;

    }

/* get the current list of lights which are off */
    routine = DDclass_InqMethod( DsInqClassId( "AttLightSwitch"), 
				    DcMethodInqGlbAttVal);

    (*routine)( &off_count, off_list, LIT_MAX);

/* reset all light to on, process the off list, and make the lto command */
    LIT_list_reset();
    LIT_list_process( off_count, off_list);
    LIT_list_lto( mlr_limits.obj[0]);

/* increment object count */
    (mlr_limits.obj[0])++;
}
