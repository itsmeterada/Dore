/* vi:set ts=8 sw=4 ai:
 *[
 *$     GEOM_tri.c
 *
 *      Steve Gray - Sat Jul  8 16:44:53 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	geometry conversion: triangle list, mesh and varmesh
 *
 *      Usage:
 *		GEOM_tri_list()		convert list
 *		GEOM_tri_mesh()		convert mesh
 *		GEOM_vartri_mesh()	convert varmesh
 *
 *      Notes:
 *
 *
 *?
 *!     Changes History
 *
 *	01/18/90	fix triangle texture map limit computation	gray
 *]
 */

/* #define	DEBUG  */

/* system includes */
#include <stdio.h>

/* dore includes */
/*#include "dogen.h"*/
#include "dore.h"
#include "dore/dore_develop/private/trimsh.h"
#include "dore/dore_develop/private/trilst.h"
#include "dore/dore_develop/private/vartrimsh.h"
#include "dore/dore_develop/private/datptr.h"

/* Render includes */
#include "primitives.h"

/* MLR Dore interface includes */
#include "global.h"
#include "geom.h"


/*[
 *$     GEOM_tri_list
 *
 *      Steve Gray - Sat Jul  8 16:48:47 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    render a triangle list
 *
 *      Usage:
 *		GEOM_tri_list( object)
 *		DtObjectStructure	*object		(in)  object
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *	01/18/90	fix triangle texture map limit computation	gray
 *
 *]
 */

GEOM_tri_list( object)
DtObjectStructure	*object;
{
struct trilst	*tri_list;
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

DtSwitch	backface_on;
DtSwitch	backface_switch;

typedef DtObjectStructure *dot_object;

DtInt			uv_count, uv_set, uv;

Char		tri_output_line[256];
Char		tri_output_next[256];

Float		u_list[50];
Float		v_list[50];

Int		vr_list[3];
Int		tr_list[3];
Int		mr_list[3];

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
printf( "GEOM_tri_list\n");
#endif


    if ( !DDcondex_QueryRender( DcTypeTriangleList) )
    {
	ERROR( "GEOM_tri_list", "invalid object type");
    }

    tri_list = (struct trilst *) (object->data);

/* check the texture map switch */
    if ( tri_list->uv_count != 0 )
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

/* get the backfacing culling information */
        routine = DDclass_InqMethod( DcTypeAttBackfaceCullSwitch,
				      DcMethodInqGlbAttVal);
    
        (*routine)( &backface_switch);

        routine = DDclass_InqMethod( DcTypeAttBackfaceCullable,
				      DcMethodInqGlbAttVal);
    
        (*routine)( &backface_on);

#ifdef	DEBUG
printf( "backface on/switch %d %d\n", backface_on, backface_switch);
#endif

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
printf( "transpintens map on\n");
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
        vtx_loc = (DtNPoint3 *) (tri_list->vertex_location);

        if ( (vtx_norm = (DtNPoint3 *) (tri_list->vertex_normal)) != NULL )
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
v = (DtReal *) tri_list->vertex_location;
for ( i = 0; i < 3*tri_list->triangle_count; i++, v += 3 )
printf( " %lf %lf %lf\n", v[0], v[1], v[2]);
}
#endif


/* if we are NOT culling backfaces, turn on rendering of both sides */
        if ( !(backface_on && backface_switch) )
        {
	    if ( mlr_mode == MLR_MODE_FILE )
	    {
	        fprintf( mlr_files.object_fp, "both on\n");
	        fprintf( mlr_files.object_fp, "coherent off\n");
	    }
	    else
	    {
#ifdef	RENDER_DIRECT
		MLR_both( TRUE);
		MLR_coherent( FALSE);
#endif
	    }
        }
    }

    for ( v = tri_list->triangle_count * 3; v-- > 0; vtx_loc++, vtx_norm++ )
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
	uv_count = tri_list->uv_count;

	for ( uv = 0; uv < tri_list->triangle_count * 3; uv++ )
	{
	    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
	    {
		if ( mlr_mode == MLR_MODE_FILE )
		{
		    fprintf( mlr_files.object_fp, "tex");

		    for ( uv_set = 0; uv_set < uv_count; uv_set++ )
		    {
			fprintf( mlr_files.object_fp, " %f %f",
				 (tri_list->uv_list)[uv_set][uv][0],
				 (tri_list->uv_list)[uv_set][uv][1]);
		    }

		    fprintf( mlr_files.object_fp, "\n");
		}
		else
		{
		    for ( uv_set = 0; uv_set < uv_count; uv_set++ )
		    {
			u_list[uv_set] = (tri_list->uv_list)[uv_set][uv][0];
			v_list[uv_set] = (tri_list->uv_list)[uv_set][uv][1];
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
    if ( (vtx_color = (DtNColor3 *) tri_list->vertex_color) != NULL )
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

/* write out all the triangles */
    v = 0;
    for ( t = tri_list->triangle_count; t-- > 0; v += 3 )
    {
	if ( mlr_mode == MLR_MODE_FILE )
	{
/* start the triangle command */
	    if ( diffuse_map_on || bump_map_on || transp_map_on )
	    {
		sprintf( tri_output_line, "tritex b b %d", mlr_limits.obj[0]);
	    }
	    else
	    {
		sprintf( tri_output_line, "tri b b %d", mlr_limits.obj[0]);
	    }
	}

/* for each vertex reference */
	for ( vr = 0; vr < 3; vr++ )
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
		        MATD_texmap_bump( &bump_map);
		    }

/* if environment mapping is present */
		    if ( env_map_on )
		    {
		        MATD_rflmap( &env_map);
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
			sprintf( tri_output_next, "  %d %d %d",
				 base_index + v + vr, local_material_index,
				 uv_base_index + v + vr);
		    }
		    else
		    {
			sprintf( tri_output_next, "  %d %d",
				 base_index + v + vr, local_material_index);
		    }
	
		    strcat( tri_output_line, tri_output_next);
		}
		else
		{
		    vr_list[vr] = base_index + v + vr;
		    tr_list[vr] = uv_base_index + v + vr;
		    mr_list[vr] = local_material_index;
		}
	    }
	}

/* complete the triangle command */
	if ( mlr_mode == MLR_MODE_FILE )
	{
	    fprintf( mlr_files.object_fp, "%s\n", tri_output_line);
	}
	else if ( mlr_mode == MLR_MODE_DIRECT )
	{
	    if ( diffuse_map_on || bump_map_on || transp_map_on )
	    {
#ifdef	RENDER_DIRECT
		MLR_tritex( mlr_limits.obj[0], USE_IN_BOTH, INTERP_BOTH,
			    vr_list[0], mr_list[0], tr_list[0],
			    vr_list[1], mr_list[1], tr_list[1],
			    vr_list[2], mr_list[2], tr_list[2]);
#endif
	    }
	    else
	    {
#ifdef	RENDER_DIRECT
		MLR_tri( mlr_limits.obj[0], USE_IN_BOTH, INTERP_BOTH,
			 vr_list[0], mr_list[0], vr_list[1], mr_list[1],
			 vr_list[2], mr_list[2]);
#endif
	    }
	}

/* increment triangle limit */
	(mlr_limits.tri[0])++;

	if ( diffuse_map_on || bump_map_on || transp_map_on )
	{
	    (mlr_limits.tri[1]) += uv_count;
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
 *$     GEOM_tri_mesh
 *
 *      Steve Gray - Sat Jul  8 16:48:47 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    render a triangle mesh
 *
 *      Usage:
 *		GEOM_tri_mesh( object)
 *		DtObjectStructure	*object		(in)  object
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *	01/18/90	fix triangle texture map limit computation	gray
 *
 *]
 */

GEOM_tri_mesh( object)
DtObjectStructure	*object;
{
struct trimsh	*tri_mesh;
DtNPoint3	*vtx_loc;
DtNPoint3	*vtx_norm;
DtNColor3	*vtx_color;
DtInt		*vtx_ref;
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

DtSwitch	backface_on;
DtSwitch	backface_switch;

DtInt			uv_count, uv_set, uv;

Char		tri_output_line[256];
Char		tri_output_next[256];

Float		u_list[50];
Float		v_list[50];

Int		mr_list[3];
Int		vr_list[3];
Int		tr_list[3];

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
printf( "GEOM_tri_mesh\n");
#endif


    if ( !DDcondex_QueryRender( DcTypeTriangleMesh) )
    {
	ERROR( "GEOM_tri_mesh", "invalid object type");
    }

    tri_mesh = (struct trimsh *) (object->data);

    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
/* get the transformation matrix for this set of control points */
        routine = DDclass_InqMethod( DcTypeAttLcstowcsmat,
				        DcMethodInqGlbAttVal);
    
        (*routine)( vtx_mat);

/* get the backfacing culling information */
        routine = DDclass_InqMethod( DcTypeAttBackfaceCullSwitch,
				      DcMethodInqGlbAttVal);
    
         (*routine)( &backface_switch);

        routine = DDclass_InqMethod( DcTypeAttBackfaceCullable,
				      DcMethodInqGlbAttVal);
    
        (*routine)( &backface_on);

#ifdef	DEBUG
printf( "backface on/switch %d %d\n", backface_on, backface_switch);
#endif
    }

/* check the texture map switch */
    if ( tri_mesh->uv_count != 0 )
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

    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
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
        vtx_loc = (DtNPoint3 *) (tri_mesh->vertex_location);

/*
        if ( ((vtx_norm = (DtNPoint3 *) (tri_mesh->vertex_normal)) != NULL) &&
	     (tri_mesh->smoothflag) && ((tri_mesh->vertextype == DcLocNrm) ||
				        (tri_mesh->vertextype == DcLocNrmClr)) )
*/
        if ( ((vtx_norm = (DtNPoint3 *) (tri_mesh->vertex_normal)) != NULL) )
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

/* if we are NOT culling backfaces, turn on rendering of both sides */
        if ( !(backface_on && backface_switch) )
        {
	    if ( mlr_mode == MLR_MODE_FILE )
	    {
	        fprintf( mlr_files.object_fp, "both on\n");
	        fprintf( mlr_files.object_fp, "coherent off\n");
	    }
	    else
	    {
#ifdef	RENDER_DIRECT
		MLR_both( TRUE);
		MLR_coherent( FALSE);
#endif
	    }
        }
    }

#ifdef	DEBUG
{ DtReal *v;
  DtInt	  i;
printf( "vertex list\n");
v = (DtReal *) tri_mesh->vertex_location;
for ( i = 0; i < tri_mesh->vertex_count; i++, v += 3 )
printf( " %lf %lf %lf\n", v[0], v[1], v[2]);
}
#endif

    for ( v = tri_mesh->vertex_count; v-- > 0; vtx_loc++, vtx_norm++ )
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
	uv_count = tri_mesh->uv_count;

	for ( uv = 0; uv < tri_mesh->vertex_count; uv++ )
	{
	    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
	    {
		if ( mlr_mode == MLR_MODE_FILE )
		{
		    fprintf( mlr_files.object_fp, "tex");

		    for ( uv_set = 0; uv_set < uv_count; uv_set++ )
		    {
			fprintf( mlr_files.object_fp, " %f %f",
				 (tri_mesh->uv_list)[uv_set][uv][0],
				 (tri_mesh->uv_list)[uv_set][uv][1]);
		    }

		    fprintf( mlr_files.object_fp, "\n");
		}
		else
		{
		    for ( uv_set = 0; uv_set < uv_count; uv_set++ )
		    {
			u_list[uv_set] = (tri_mesh->uv_list)[uv_set][uv][0];
			v_list[uv_set] = (tri_mesh->uv_list)[uv_set][uv][1];
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

/* get the vertex reference list */
    vtx_ref = (DtInt *) (tri_mesh->triangle_vertexlist);

/* get the first color */
    if ( (vtx_color = (DtNColor3 *) tri_mesh->vertex_color) != NULL )
    {
	vertex_colors_present = TRUE;

	prev_color.r = vtx_color[*vtx_ref].r;
	prev_color.g = vtx_color[*vtx_ref].g;
	prev_color.b = vtx_color[*vtx_ref].b;
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

/* we are on the first local material */
        local_material_index = 0;
    }

#ifdef	DEBUG
printf( "vertex reference list\n");
{ DtInt *vref;
  DtInt	i;
vref = (DtInt *) tri_mesh->triangle_vertexlist;
for ( i = 0; i < tri_mesh->triangle_count; i++, vref += 3 )
printf( " %d %d %d\n", vref[0], vref[1], vref[2]);
}
#endif
  

/* write out all the triangles */
    for ( t = tri_mesh->triangle_count; t-- > 0; vtx_ref += 3 )
    {
	if ( mlr_mode == MLR_MODE_FILE )
	{
/* start the triangle command */
	    if ( diffuse_map_on || bump_map_on || transp_map_on )
	    {
		sprintf( tri_output_line, "tritex b b %d", mlr_limits.obj[0]);
	    }
	    else
	    {
		sprintf( tri_output_line, "tri b b %d", mlr_limits.obj[0]);
	    }
	}

/* for each vertex reference */
	for ( vr = 0; vr < 3; vr++ )
	{
/* get the next vertex color */
	    if ( vertex_colors_present )
	    {
	        next_color.r = vtx_color[vtx_ref[vr]].r;
	        next_color.g = vtx_color[vtx_ref[vr]].g;
	        next_color.b = vtx_color[vtx_ref[vr]].b;
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
		        MATD_texmap_bump( &bump_map);
		    }

/* if environment mapping is present */
		    if ( env_map_on )
		    {
		        MATD_rflmap( &env_map);
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
		        sprintf( tri_output_next, "  %d %d %d",
			         base_index + vtx_ref[vr], local_material_index,
			         uv_base_index + vtx_ref[vr]);
		    }
		    else
		    {
		        sprintf(tri_output_next, "  %d %d",
			        base_index + vtx_ref[vr], local_material_index);
		    }

		    strcat( tri_output_line, tri_output_next);
		}
		else
		{
		    vr_list[vr] = base_index + vtx_ref[vr];
		    mr_list[vr] = local_material_index;
		    tr_list[vr] = uv_base_index + vtx_ref[vr];
		}
	    }
	}

/* complete the triangle command */
	if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
	{
	    if ( mlr_mode == MLR_MODE_FILE )
	    {
	        fprintf( mlr_files.object_fp, "%s\n", tri_output_line);
	    }
	    else if ( mlr_mode == MLR_MODE_DIRECT )
	    {
		if ( diffuse_map_on || bump_map_on || transp_map_on )
		{
#ifdef	RENDER_DIRECT
		    MLR_tritex( mlr_limits.obj[0], USE_IN_BOTH, INTERP_BOTH,
			        vr_list[0], mr_list[0], tr_list[0],
			        vr_list[1], mr_list[1], tr_list[1],
			        vr_list[2], mr_list[2], tr_list[2]);
#endif
		}
		else
		{
#ifdef	RENDER_DIRECT
		    MLR_tri( mlr_limits.obj[0], USE_IN_BOTH, INTERP_BOTH,
			     vr_list[0], mr_list[0], vr_list[1], mr_list[1],
			     vr_list[2], mr_list[2]);
#endif
		}
	    }
	}

/* increment triangle limit */
	(mlr_limits.tri[0])++;

	if ( diffuse_map_on || bump_map_on || transp_map_on )
	{
	    (mlr_limits.tri[1]) += uv_count;
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

/* increment the object count */
    (mlr_limits.obj[0])++;
}


/*[
 *$     GEOM_vartri_mesh
 *
 *      Helga Thorvaldsdottir - Thu May 10, 1990
 *?
 *      Description:    render a vartriangle mesh
 *
 *      Usage:
 *		GEOM_vartri_mesh( object)
 *		DtObjectStructure	*object		(in)  object
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *	vartriangle meshes can now have texture coordinates -helga 9/27/90
 *
 *]
 */

GEOM_vartri_mesh( object)
DtObjectStructure	*object;

{
struct trimsh	*vartri_mesh;
DtNPoint3	*vtx_loc;
DtNPoint3	*vtx_norm;
DtNColor3	*vtx_color;
DtInt		*vtx_ref;
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

DtSwitch	backface_on;
DtSwitch	backface_switch;

DtInt			uv_count, uv_set, uv;

Char		vartri_output_line[256];
Char		vartri_output_next[256];

Float		u_list[50];
Float		v_list[50];

Int		mr_list[3];
Int		vr_list[3];
Int		tr_list[3];

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
printf( "GEOM_vartri_mesh\n");
#endif


    if ( !DDcondex_QueryRender( DcTypeVarTriangleMesh) )
    {
	ERROR( "GEOM_vartri_mesh", "invalid object type");
    }

    vartri_mesh = (struct trimsh *) (object->data);

    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
/* get the transformation matrix for this set of control points */
        routine = DDclass_InqMethod( DcTypeAttLcstowcsmat,
				        DcMethodInqGlbAttVal);
    
        (*routine)( vtx_mat);

/* get the backfacing culling information */
        routine = DDclass_InqMethod( DcTypeAttBackfaceCullSwitch,
				      DcMethodInqGlbAttVal);
    
         (*routine)( &backface_switch);

        routine = DDclass_InqMethod( DcTypeAttBackfaceCullable,
				      DcMethodInqGlbAttVal);
    
        (*routine)( &backface_on);

#ifdef	DEBUG
printf( "backface on/switch %d %d\n", backface_on, backface_switch);
#endif
    }

/* check the texture map switch */
    if ( vartri_mesh->uv_count != 0 )
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

    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
    {
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
        vtx_loc = (DtNPoint3 *) (vartri_mesh->vertex_location);

/*
        if ( ((vtx_norm = (DtNPoint3 *) (vartri_mesh->vertex_normal)) != NULL) &&
	     (vartri_mesh->smoothflag) && ((vartri_mesh->vertextype == DcLocNrm) ||
				        (vartri_mesh->vertextype == DcLocNrmClr)) )
*/
        if ( ((vtx_norm = (DtNPoint3 *) (vartri_mesh->vertex_normal)) != NULL) )
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

/* if we are NOT culling backfaces, turn on rendering of both sides */
        if ( !(backface_on && backface_switch) )
        {
	    if ( mlr_mode == MLR_MODE_FILE )
	    {
	        fprintf( mlr_files.object_fp, "both on\n");
	        fprintf( mlr_files.object_fp, "coherent off\n");
	    }
	    else
	    {
#ifdef	RENDER_DIRECT
		MLR_both( TRUE);
		MLR_coherent( FALSE);
#endif
	    }
        }
    }

#ifdef	DEBUG
{ DtReal *v;
  DtInt	  i;
printf( "vertex list\n");
v = (DtReal *) vartri_mesh->vertex_location;
for ( i = 0; i < vartri_mesh->vertex_count; i++, v += 3 )
printf( " %lf %lf %lf\n", v[0], v[1], v[2]);
}
#endif

    for ( v = vartri_mesh->vertex_count; v-- > 0; vtx_loc++, vtx_norm++ )
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
	uv_count = vartri_mesh->uv_count;

	for ( uv = 0; uv < vartri_mesh->vertex_count; uv++ )
	{
	    if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
	    {
		if ( mlr_mode == MLR_MODE_FILE )
		{
		    fprintf( mlr_files.object_fp, "tex");

		    for ( uv_set = 0; uv_set < uv_count; uv_set++ )
		    {
			fprintf( mlr_files.object_fp, " %f %f",
				 (vartri_mesh->uv_list)[uv_set][uv][0],
				 (vartri_mesh->uv_list)[uv_set][uv][1]);
		    }

		    fprintf( mlr_files.object_fp, "\n");
		}
		else
		{
		    for ( uv_set = 0; uv_set < uv_count; uv_set++ )
		    {
			u_list[uv_set] = (vartri_mesh->uv_list)[uv_set][uv][0];
			v_list[uv_set] = (vartri_mesh->uv_list)[uv_set][uv][1];
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

/* get the vertex reference list */
    vtx_ref = (DtInt *) (vartri_mesh->triangle_vertexlist);

/* get the first color */
    if ( (vtx_color = (DtNColor3 *) vartri_mesh->vertex_color) != NULL )
    {
	vertex_colors_present = TRUE;

	prev_color.r = vtx_color[*vtx_ref].r;
	prev_color.g = vtx_color[*vtx_ref].g;
	prev_color.b = vtx_color[*vtx_ref].b;
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

/* we are on the first local material */
        local_material_index = 0;
    }

#ifdef	DEBUG
printf( "vertex reference list\n");
{ DtInt *vref;
  DtInt	i;
vref = (DtInt *) vartri_mesh->triangle_vertexlist;
for ( i = 0; i < vartri_mesh->triangle_count; i++, vref += 3 )
printf( " %d %d %d\n", vref[0], vref[1], vref[2]);
}
#endif
  

/* write out all the triangles */
    for ( t = vartri_mesh->triangle_count; t-- > 0; vtx_ref += 3 )
    {
	if ( mlr_mode == MLR_MODE_FILE )
	{
/* start the triangle command */
	    if ( diffuse_map_on || bump_map_on || transp_map_on )
	    {
		sprintf( vartri_output_line, "tritex b b %d", mlr_limits.obj[0]);
	    }
	    else
	    {
		sprintf( vartri_output_line, "tri b b %d", mlr_limits.obj[0]);
	    }
	}

/* for each vertex reference */
	for ( vr = 0; vr < 3; vr++ )
	{
/* get the next vertex color */
	    if ( vertex_colors_present )
	    {
	        next_color.r = vtx_color[vtx_ref[vr]].r;
	        next_color.g = vtx_color[vtx_ref[vr]].g;
	        next_color.b = vtx_color[vtx_ref[vr]].b;
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
		        MATD_texmap_bump( &bump_map);
		    }

/* if environment mapping is present */
		    if ( env_map_on )
		    {
		        MATD_rflmap( &env_map);
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
		        sprintf( vartri_output_next, "  %d %d %d",
			         base_index + vtx_ref[vr], local_material_index,
			         uv_base_index + vtx_ref[vr]);
		    }
		    else
		    {
		        sprintf(vartri_output_next, "  %d %d",
			        base_index + vtx_ref[vr], local_material_index);
		    }

		    strcat( vartri_output_line, vartri_output_next);
		}
		else
		{
		    vr_list[vr] = base_index + vtx_ref[vr];
		    mr_list[vr] = local_material_index;
		    tr_list[vr] = uv_base_index + vtx_ref[vr];
		}
	    }
	}

/* complete the triangle command */
	if ( mlr_mode != MLR_MODE_DIRECT_COUNT )
	{
	    if ( mlr_mode == MLR_MODE_FILE )
	    {
	        fprintf( mlr_files.object_fp, "%s\n", vartri_output_line);
	    }
	    else if ( mlr_mode == MLR_MODE_DIRECT )
	    {
		if ( diffuse_map_on || bump_map_on || transp_map_on )
		{
#ifdef	RENDER_DIRECT
		    MLR_tritex( mlr_limits.obj[0], USE_IN_BOTH, INTERP_BOTH,
			        vr_list[0], mr_list[0], tr_list[0],
			        vr_list[1], mr_list[1], tr_list[1],
			        vr_list[2], mr_list[2], tr_list[2]);
#endif
		}
		else
		{
#ifdef	RENDER_DIRECT
		    MLR_tri( mlr_limits.obj[0], USE_IN_BOTH, INTERP_BOTH,
			     vr_list[0], mr_list[0], vr_list[1], mr_list[1],
			     vr_list[2], mr_list[2]);
#endif
		}
	    }
	}

/* increment triangle limit */
	(mlr_limits.tri[0])++;

	if ( diffuse_map_on || bump_map_on || transp_map_on )
	{
	    (mlr_limits.tri[1]) += uv_count;
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

/* increment the object count */
    (mlr_limits.obj[0])++;
}
