/* vi:set ts=8 sw=4 ai:
 *[
 *$     MATD_map.c
 *
 *      Steve Gray - Mon Nov 13 17:01:28 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	map file management
 *
 *      Usage:
 *		MATD_map_init()			init list
 *
 *		MATD_map_add_raster()		add raster to map list
 *		MATD_map_add_file()		add file to map list
 *
 *		matd_map_find_raster()		find a map by object
 *		matd_map_find_file()		find a map by file name
 *
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
#include "dore/dore_develop/private/rstr.h"
#include "dore/dore_develop/private/filerstr.h"

/* Render includes */
#include "primitives.h"

/* MLR Dore interface includes */
#include "global.h"
#include "matd.h"


/*[
 *$     MATD_map_init
 *
 *      Steve Gray - Mon Nov 13 17:03:30 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    initialize map list
 *
 *      Usage:
 *		MATD_map_init()
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MATD_map_init()
{
    matd_map_count = 0;
}



/*[
 *$     MATD_map_add_raster
 *
 *      Steve Gray - Mon Nov 13 17:04:20 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    add a raster object to the map list
 *
 *      Usage:
 *		MATD_map_add_raster( object)
 *		DtObject	object
 *
 *      Notes:  
 *		returns the Media Logic mapfile index
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MATD_map_add_raster( object)
DtObject	object;
{
Int		index;


/* see if the raster has already been converted */
    if ( (index = matd_map_find_raster( object)) >= 0 )
    {
	return( index);
    }

/* see if there would be too many textures */
    if ( (matd_map_count + 1) == MAT_MAX_MAPFILES )
    {
	DDerror( ERR_FATAL_INTERNAL_ERROR, "MATD_map_add_raster", 
		    "too many maps");
    }

/* make a tmp file name */
    sprintf( matd_map_file[matd_map_count].map_name, "%s.%d", 
	     mlr_files.file_prefix, matd_map_count);

/* write the raster data to a DORE raster file */
    DsRasterWrite( object, matd_map_file[matd_map_count].map_name);

/* add it to the list */
    matd_map_file[matd_map_count].dore_object = object;
    matd_map_file[matd_map_count].type = MAT_MAP_TYPE_ART;

    if ( mlr_mode == MLR_MODE_DIRECT )
    {
	switch ( matd_map_file[matd_map_count].type ) {
	case MAT_MAP_TYPE_ART:
#ifdef	RENDER_DIRECT
	    MLR_mapfile( matd_map_file[matd_map_count].map_name, 
			 FILE_TYPE_DORE);
#endif
	    break;
	
	default:
	    DDerror( ERR_VALUE_OUT_OF_RANGE, "MATD_map_exec", 
		        "internal error: unimplemented map type");
	}
    }
    else if ( mlr_mode == MLR_MODE_FILE )
    {
	switch ( matd_map_file[matd_map_count].type ) {
	case MAT_MAP_TYPE_ART:
	    fprintf( mlr_files.camera_fp, "mapfile %s dore\n", 
		     matd_map_file[matd_map_count].map_name);
	    break;
	
	default:
	    DDerror( ERR_VALUE_OUT_OF_RANGE, "MATD_map_exec", 
		        "internal error: unimplemented map type");
	}
    }

    return( matd_map_count++);
}


/*[
 *$     MATD_map_add_file
 *
 *      Steve Gray - Mon Nov 13 17:14:24 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    add file raster to map list
 *
 *      Usage:
 *		MATD_map_add_file( object)
 *		DtObject	object
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MATD_map_add_file( object)
DtObject	object;
{
struct filerstr	*file_raster;
Int		index;


/* get the fileraster info */
    file_raster = (struct filerstr *) (((DtObjectStructure *) object)->data);

/* see if the file has already been converted */
    if ( (index = matd_map_find_file( file_raster->filename)) >= 0 )
    {
	return( index);
    }

/* see if there would be too many textures */
    if ( (matd_map_count + 1) == MAT_MAX_MAPFILES )
    {
	DDerror( ERR_FATAL_INTERNAL_ERROR, "MATD_map_add_file", 
		    "too many maps");
    }

/* add the file too the list */
    strcpy( matd_map_file[matd_map_count].map_name, file_raster->filename);
    matd_map_file[matd_map_count].dore_object = object;

    if ( file_raster->type == DcNullPtr )
    {
        matd_map_file[matd_map_count].type = MAT_MAP_TYPE_DORE;
    }
    else
    {
	if ( strcmp( file_raster->type, "MLR art") == 0 )
	{
	    matd_map_file[matd_map_count].type = MAT_MAP_TYPE_ART;
	}
	else if ( strcmp( file_raster->type, "MLR rle") == 0 )
	{
	    matd_map_file[matd_map_count].type = MAT_MAP_TYPE_RLE;
	}
	else if ( strcmp( file_raster->type, "MLR tmap") == 0 )
	{
	    matd_map_file[matd_map_count].type = MAT_MAP_TYPE_TMAP;
	}
	else if ( strcmp( file_raster->type, "MLR rmap") == 0 )
	{
	    matd_map_file[matd_map_count].type = MAT_MAP_TYPE_RMAP;
	}
	else
	{
	    DDerror( ERR_VALUE_OUT_OF_RANGE, "MATD_map_add_file", 
			"unknown special file type");
	}
    }


    if ( mlr_mode == MLR_MODE_DIRECT )
    {
#ifdef	RENDER_DIRECT
	switch ( matd_map_file[matd_map_count].type ) {
	case MAT_MAP_TYPE_DORE:
	    MLR_mapfile( matd_map_file[matd_map_count].map_name, 
			 FILE_TYPE_DORE);
	    break;
	
	case MAT_MAP_TYPE_ART:
	    MLR_mapfile( matd_map_file[matd_map_count].map_name, 
			 FILE_TYPE_ART);
	    break;
	
	case MAT_MAP_TYPE_RLE:
	    MLR_mapfile( matd_map_file[matd_map_count].map_name, 
			 FILE_TYPE_ART_RLE);
	    break;
	
	case MAT_MAP_TYPE_TMAP:
	    MLR_mapfile( matd_map_file[matd_map_count].map_name, 
			 FILE_TYPE_TMAP);
	    break;
	
	case MAT_MAP_TYPE_RMAP:
	    MLR_mapfile( matd_map_file[matd_map_count].map_name, 
			 FILE_TYPE_RMAP);
	    break;
	
	default:
	    DDerror( ERR_FATAL_INTERNAL_ERROR, "MATD_map_add_file", 
		        "internal error: unimplemented map type");
	}
#endif
    }
    else if ( mlr_mode == MLR_MODE_FILE )
    {
	switch ( matd_map_file[matd_map_count].type ) {
	case MAT_MAP_TYPE_DORE:
	    fprintf( mlr_files.camera_fp, "mapfile %s dore\n", 
		     matd_map_file[matd_map_count].map_name);
	    break;
	
	case MAT_MAP_TYPE_ART:
	    fprintf( mlr_files.camera_fp, "mapfile %s art\n", 
		     matd_map_file[matd_map_count].map_name);
	    break;
	
	case MAT_MAP_TYPE_RLE:
	    fprintf( mlr_files.camera_fp, "mapfile %s rle\n", 
		     matd_map_file[matd_map_count].map_name);
	    break;
	
	case MAT_MAP_TYPE_TMAP:
	    fprintf( mlr_files.camera_fp, "mapfile %s tmap\n", 
		     matd_map_file[matd_map_count].map_name);
	    break;
	
	case MAT_MAP_TYPE_RMAP:
	    fprintf( mlr_files.camera_fp, "mapfile %s rmap\n", 
		     matd_map_file[matd_map_count].map_name);
	    break;
	
	default:
	    DDerror( ERR_FATAL_INTERNAL_ERROR, "MATD_map_add_file", 
		        "internal error: unimplemented map type");
	}
    }
    return( matd_map_count++);
}


/*[
 *$     matd_map_find_raster
 *
 *      Steve Gray - Mon Nov 13 17:21:56 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    find a file by Dore object
 *
 *      Usage:
 *		matd_map_find_raster( object)
 *		DtObject	object		(in)  object to find
 *
 *      Notes:  
 *		returns -1	if not found
 *			index	if found
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

matd_map_find_raster( object)
DtObject	object;
{
Int	i;


    for ( i = 0; i < matd_map_count; i++ )
    {
	if ( matd_map_file[i].dore_object == object )
	{
	    return( i);
	}
    }

    return( -1);
}


/*[
 *$     matd_map_find_file
 *
 *      Steve Gray - Mon Nov 13 17:21:56 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    find a file by file name
 *
 *      Usage:
 *		matd_map_find_file( name)
 *		Char	*name		(in) file name
 *
 *      Notes:  
 *		returns -1	if not found
 *			index	if found
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

matd_map_find_file( name)
Char	*name;
{
Int	i;


    for ( i = 0; i < matd_map_count; i++ )
    {
	if ( strcmp( matd_map_file[i].map_name, name) == 0 )
	{
	    return( i);
	}
    }

    return( -1);
}



