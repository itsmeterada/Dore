/**************************************************************************
# Copyright (C) 1994 Kubota Graphics Corp.
# 
# Permission to use, copy, modify, and distribute this material for
# any purpose and without fee is hereby granted, provided that the
# above copyright notice and this permission notice appear in all
# copies, and that the name of Kubota Graphics not be used in
# advertising or publicity pertaining to this material.  Kubota
# Graphics Corporation MAKES NO REPRESENTATIONS ABOUT THE ACCURACY
# OR SUITABILITY OF THIS MATERIAL FOR ANY PURPOSE.  IT IS PROVIDED
# "AS IS", WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE AND KUBOTA GRAPHICS CORPORATION DISCLAIMS ALL WARRANTIES,
# EXPRESS OR IMPLIED.
**************************************************************************/

/*
  ======================================================================
  Functions:
	ddr_g4vll_drom_set_att_mapbmp
	ddr_g4vll_drom_set_att_mapbmpswi
	ddr_g4vll_drom_set_att_mapdifclr
	ddr_g4vll_drom_set_att_mapdifclrswi
	ddr_g4vll_drom_set_att_mapenv
	ddr_g4vll_drom_set_att_mapenvswi
	ddr_g4vll_drom_set_att_maptrnint
	ddr_g4vll_drom_set_att_maptrnintswi

  ======================================================================
 */

#include "drom.h"
#include <dore/internal/matrix.h>
#include <dore/dore_develop/private/mapdifclr.h>
#include <dore/dore_develop/private/stdtexatt.h>
#include <dore/dore_develop/private/rstr.h>


    /***  Macro Definitions  ***/

/* The PE's have only nine address lines, so every dimension of a texture is
** limited to 512 texels.  If an texture raster is larger than 512 in any
** dimension, then the texture will be tiled into separate pieces, and each
** tile will be rendered per pass.  For 3D textures, the maximum tile size is
** 64 x 64 x 64, and for environment maps, the maximum texture size is
** 204 x 204. */

#define MAX_2D_TILESIZE	  256
#define MAX_3D_TILESIZE	   64
#define MAX_ENV_TILESIZE  204


    /***  Function Declarations  ***/

static void ddr_g4vll_drom_uv_xform (DtMatrix3x3 M, DtMatrix4x4 T);
static void ddr_g4vll_drom_difclrmap_2d
		(struct rstr *, ddt_g4vll_drom_rstr *, dot_stdtexatt []);
static void ddr_g4vll_drom_difclrmap_3d
		(struct rstr *, ddt_g4vll_drom_rstr *, dot_stdtexatt []);
static int  ddr_g4vll_drom_vll_extend_mode (DtExtendMode dore_extend);



/*****************************************************************************
**  This function is called whenever the value of the bump-mapping
**  parameters changes.
*****************************************************************************/

void ddr_g4vll_drom_set_att_mapbmp (
    DtInt         count,	/* Number of Bump Maps to be Applied */
    DtObject      mapping[],	/* Type of Mapping to Perform */
    DtObject      raster[],	/* Map Data */
    dot_stdtexatt attributes[])	/* Texture Attributes */
{
}



/*****************************************************************************
**  This function enables/disables subsequent bump-mapping textures.
*****************************************************************************/

void ddr_g4vll_drom_set_att_mapbmpswi (DtSwitch switchvalue)
{
}



/*****************************************************************************
**  This routine enables/disables subsequent diffuse-color texture mapping.
*****************************************************************************/

void ddr_g4vll_drom_set_att_mapdifclrswi (DtSwitch switchvalue)
{
    dde_g4vll_drom_data.map_diffuse_color_switch = switchvalue;

    if (!dde_g4vll_drom_data.pre_init)
	ddr_g4vll_drom_set_prim_execute ();
}



/*****************************************************************************
**  This function sets the current diffuse-color mapping attribute, and is
**  called each time the diffuse-color texture mapping parameters change.
**  This implementation does not provide for multiple maps.
*****************************************************************************/

void ddr_g4vll_drom_set_att_mapdifclr (
    DtInt              count,
    DtObject           mapping[],
    DtObjectStructure *raster[],
    dot_stdtexatt      attributes[])
{
    auto     ddt_g4vll_drom_rstr *drom_rstr;
    static   DtInt                filerstr_classid = -1;
    static   DtInt                filerstr_dataindex = -1;
    register int                  ii;
    auto     DtObjectStructure   *raster_obj = DcNullPtr;
    auto     DtInt                rasterid;
    auto     struct rstr         *rstr;
    static   DtInt                rstr_classid = -1;
    static   DtInt                rstr_dataindex = -1;


    dde_g4vll_drom_data.texture_3D_valid = DcFalse;
    dde_g4vll_drom_data.texture_2D_valid = DcFalse;
    dde_g4vll_drom_data.map_extend_black = DcFalse;

    if (dde_g4vll_drom_data.pre_init) return;

    /* Return if the device does not support texture mapping.  Likewise,
    ** don't do anything if there aren't any maps.  */

    if (!dde_g4vll_drom_curdevdat->features->uv_texture_mapping &&
	!dde_g4vll_drom_curdevdat->features->uvw_texture_mapping)
	return;

    if (count <= 0) return;

    dde_g4vll_drom_data.map_diffuse_color_uv_index = attributes[0].uv.idx;
    dde_g4vll_drom_data.map_diffuse_color_uvw_index = attributes[0].uvw.idx;

    if (!raster[0]) return;

    rasterid = DsInqObjClassId(raster[0]);

    if (filerstr_classid == -1)
	filerstr_classid = DsInqClassId("DoFileRaster");

    /* If this is a fileraster, its additional data contains a raster object
    ** that in turns contains the actual raster data.  */

    if (rasterid == filerstr_classid) {
	if (filerstr_dataindex == -1)  {
	    for (ii=0; dde_g4vll_drom_class_data[ii].name != 0; ii++) {
		if (dde_g4vll_drom_class_data[ii].id == filerstr_classid) {
		    filerstr_dataindex = dde_g4vll_drom_class_data[ii].index;
		    break;
		}
	    }
	}

	ddr_g4vll_drom_filerstr_update (raster[0]);

	raster_obj = (DtObjectStructure *)
	    raster[0]->additional_data[filerstr_dataindex];
    }

    if (rstr_classid == -1) rstr_classid = DsInqClassId("DoRaster");

    if (rasterid == rstr_classid)
	raster_obj = raster[0];

    if (!raster_obj) return;

    if (rstr_dataindex == -1) {
	for (ii=0; dde_g4vll_drom_class_data[ii].name != 0; ii++) {
	    if (dde_g4vll_drom_class_data[ii].id == rstr_classid) {
		rstr_dataindex = dde_g4vll_drom_class_data[ii].index;
		break;
	    }
	}
    }

    rstr = (struct rstr *)(raster_obj->data);
    drom_rstr =
	(ddt_g4vll_drom_rstr *) raster_obj->additional_data[rstr_dataindex];

    /* Only allow operator of Multiply or Replace.  All other operators are
    ** set to Replace.  */

    if (attributes[0].operator == DcTextureMultiply)
	dde_g4vll_drom_data.map_operator = DcTextureMultiply;
    else
	dde_g4vll_drom_data.map_operator = DcTextureReplace;

    /* DcRasterRGB DcRasterABGR, and DcRasterRGBA are the only raster types
    ** supported by G4 VLL; exit if we encounter any other type.  */

    if (  (rstr->type != DcRasterRGB)
       && (rstr->type != DcRasterABGR)
       && (rstr->type != DcRasterRGBA)
       /* special for DEC (Rubik's cube): support for PSEUDO_8 texturing
          for small textures */
       && !(rstr->type == DcRasterSpecial &&
                          !strcmp(rstr->typestring,"PSEUDO_8"))
       )
	return;

    /* Set up for either a 2D texture map or a 3D texture map, as long as
    ** we're using some sort of standard table lookup texture.  */

    if ((mapping[0] == DcStdTableLookup) && (rstr->depth == 1)) {
    	dde_g4vll_drom_data.texture_2D_valid = DcTrue;
	ddr_g4vll_drom_difclrmap_2d (rstr, drom_rstr, attributes);
    } else if (mapping[0] == DcStd3dTableLookup && rstr->depth > 1) {
    	dde_g4vll_drom_data.texture_3D_valid = DcTrue;
	ddr_g4vll_drom_difclrmap_3d (rstr, drom_rstr, attributes);
    }
}



/*****************************************************************************
**  This subroutine loads the UV transformation matrix into a 4x4
**  transformation matrix.  The original UV transformation matrix is a 3x3.
*****************************************************************************/

static void ddr_g4vll_drom_uv_xform (
    DtMatrix3x3 M,		/* UV Matrix */
    DtMatrix4x4 T)		/* Transformation Matrix */
{
    T[0][0] = M[0][0];  T[0][1] = M[0][1];  T[0][2] = 0.0;  T[0][3] = M[0][2];
    T[1][0] = M[1][0];  T[1][1] = M[1][1];  T[1][2] = 0.0;  T[1][3] = M[1][2];
    T[2][0] =     0.0;  T[2][1] =     0.0;  T[2][2] = 1.0;  T[2][3] =     0.0;
    T[3][0] = M[2][0];  T[3][1] = M[2][1];  T[3][2] = 0.0;  T[3][3] = M[2][2];
}



/*****************************************************************************
**  This subroutine returns the corresponding VLL extend mode given the Dore'
**  texture-map extend mode.
*****************************************************************************/

static int ddr_g4vll_drom_vll_extend_mode (DtExtendMode dore_extend)
{
    register int vll_extend;	/* The VLL Extend Mode */

    switch (dore_extend)
    {   
	case DcExtendNone:    vll_extend = VLL_EXTEND_NONE;	break;
	case DcExtendBlack:   vll_extend = VLL_EXTEND_NONE;	break;
	case DcExtendClamp:   vll_extend = VLL_EXTEND_CLAMP;	break;
	case DcExtendRepeat:  vll_extend = VLL_EXTEND_REPEAT;	break;
	default:              vll_extend = VLL_EXTEND_SPECIAL;	break;
    }

    return vll_extend;
}



/*****************************************************************************
**  This subroutine computes and loads 2D texture maps.
*****************************************************************************/

static void ddr_g4vll_drom_difclrmap_2d (
             struct rstr         *rstr,
    register ddt_g4vll_drom_rstr *drom_rstr,
             dot_stdtexatt        attributes[])
{
    auto     int          aamode;
    auto     VLLtexture **cur_texture;
    register int          ii;
    auto     DtMatrix4x4  tile_matrix;
    auto     int          tile_number;
    auto     int          type;
    auto     int          umode, vmode, wmode;
    auto     DtMatrix4x4  usr_matrix;
    auto     int          width, height;
    auto     int          x_tile, y_tile;

    if (dde_g4vll_drom_data.cur_texture_pass < 0) return;

    /* Only point and bilinear sampling are supported when tiling. */

    if (attributes[0].aamode == DcTextureAABilinear)
    {   if (drom_rstr->aamode == DcTextureAANone)
	    drom_rstr->tiling_initialized = DcFalse;
	drom_rstr->aamode = DcTextureAABilinear;
    }
    else
    {   if (drom_rstr->aamode == DcTextureAABilinear) {
	    drom_rstr->tiling_initialized = DcFalse;
	}
	drom_rstr->aamode = DcTextureAANone;
    }

    if (drom_rstr->texture_sequence_number != rstr->sequence_number)
	drom_rstr->tiling_initialized = DcFalse;

    /* Determine whether the texture needs to be tiled (do this on the
    ** first pass and subsequently only when either the antialiasing mode
    ** changes or the sequence numbers of the texture and DROM don't
    ** match).  If tiling is necessary, break the texture into pieces no
    ** larger than MAX_2D_TILESIZE in any dimension, and store them away.  */

    if (!drom_rstr->tiling_initialized) {

	/* If any previous textures existed (if this is an update), then
	** delete them before continuing.  */

	if (drom_rstr->texture)
	{   for (ii=0;  ii < drom_rstr->num_tiles;  ++ii)
		VLLdelete_texture (drom_rstr->texture[ii]);
	    DDspace_Deallocate (drom_rstr->texture);
	    drom_rstr->texture = DcNullPtr;
	}

	/* Set the anti-aliasing mode and sequence number fields for the
	** current texture map. */

	dde_g4vll_drom_data.map_tile_aamode = drom_rstr->aamode;
	drom_rstr->texture_sequence_number  = rstr->sequence_number;

	/* If any raster dimension is larger than MAX_2D_TILESIZE, then we'll
	** need to break the raster up into separate tiles and render the
	** diffuse color map in several passes.  The following block breaks
	** the raster up into a list of texture tiles. */

	if ((rstr->width > MAX_2D_TILESIZE) || (rstr->height > MAX_2D_TILESIZE))
        {
	    auto int   ptr_offset;
	    auto int   byte_offset;
	    auto char *ptr;

	    if (rstr->type == DcRasterSpecial)
                /* at this point we know that typestring is PSEUDO-8 */
                DDerror(ERR_INVALID_OPERATION,
                "ddr_g4vll_drom_difclrmap_2d",
                "Can't use DcRasterSpecial/PSEUDO_8 with textures > 256x256!");

	    if (drom_rstr->aamode == DcTextureAANone) {

		drom_rstr->last_tile_width  = rstr->width  % MAX_2D_TILESIZE;
		drom_rstr->last_tile_height = rstr->height % MAX_2D_TILESIZE;

		drom_rstr->num_tiles_row = (rstr->width / MAX_2D_TILESIZE)
			 + (drom_rstr->last_tile_width == 0 ? 0 : 1);
		drom_rstr->num_tiles_col = (rstr->height / MAX_2D_TILESIZE)
			 + (drom_rstr->last_tile_height == 0 ? 0 : 1);

	    } else /* DcTextureAABilinear */ {

		/* When doing bilinear anti-aliasing, we need to overlap the
		** tiles by a texel so that the edge texels can still be
		** averaged.  */

		drom_rstr->last_tile_width =
		    (rstr->width - MAX_2D_TILESIZE) % (MAX_2D_TILESIZE - 1);

		drom_rstr->last_tile_height =
		    (rstr->height - MAX_2D_TILESIZE) % (MAX_2D_TILESIZE - 1);

		drom_rstr->num_tiles_row =
		    1 + ((rstr->width - MAX_2D_TILESIZE) / (MAX_2D_TILESIZE-1))
		      + ((drom_rstr->last_tile_width == 0) ? 0 : 1);
		drom_rstr->num_tiles_col =
		    1 + ((rstr->height - MAX_2D_TILESIZE) / (MAX_2D_TILESIZE-1))
                      + ((drom_rstr->last_tile_height == 0) ? 0 : 1);
	    }

	    /* If the remainder from the modulo operation for the last tile
	    ** is zero, it must be a full-width (height) tile.  */

	    if (drom_rstr->last_tile_width == 0)
		drom_rstr->last_tile_width = MAX_2D_TILESIZE;

	    if (drom_rstr->last_tile_height == 0)
		drom_rstr->last_tile_height = MAX_2D_TILESIZE;

	    drom_rstr->num_tiles = drom_rstr->num_tiles_row
				 * drom_rstr->num_tiles_col;

	    drom_rstr->width_scaler  = ((DtReal)MAX_2D_TILESIZE) / rstr->width;
	    drom_rstr->height_scaler = ((DtReal)MAX_2D_TILESIZE) / rstr->height;

	    drom_rstr->last_width_scaler  =
		((DtReal)drom_rstr->last_tile_width) / rstr->width;
	    drom_rstr->last_height_scaler =
		((DtReal)drom_rstr->last_tile_height) / rstr->height;

	    /* The supported raster types for 2D texture mapping are RGB,
	    ** ABGR and RGBA.  */

	    switch (rstr->type)
	    {
		case DcRasterRGB:
		    byte_offset = 3;
		    type = VLL_RASTER_RGB;
		    break;

		/* Hack for little-endian machines:  reverse the VLL raster
		** type instead of reversing the raster data.  */

		case DcRasterABGR:
		    byte_offset = 4;
		    type = VLL_RASTER_RGBA;
		    break;

		case DcRasterRGBA:
		    byte_offset = 4;
		    type = VLL_RASTER_ABGR;
		    break;

		default:  break;
	    }

	    drom_rstr->texture =
		DDspace_Allocate (sizeof(VLLtexture*) * drom_rstr->num_tiles);

	    cur_texture = drom_rstr->texture;

	    for (y_tile=0; y_tile < drom_rstr->num_tiles_col; y_tile++) {

		for (x_tile=0; x_tile < drom_rstr->num_tiles_row;x_tile++){

		    auto VLLtexture *new_tile;

		    if (drom_rstr->aamode == DcTextureAANone) {
			ptr_offset =
			    ((y_tile * rstr->width * MAX_2D_TILESIZE) +
			    (x_tile * MAX_2D_TILESIZE)) *
			    byte_offset;

		    } else /* DcTextureAABilinear */ {

			/* There is a one-texel overlap on each tile boundary
			** when bilinear sampling.  */

			ptr_offset =
			    ((y_tile * rstr->width * MAX_2D_TILESIZE) +
			    ((x_tile * MAX_2D_TILESIZE) - x_tile)) *
			    byte_offset;
		    }

		    ptr = (char *)rstr->data + ptr_offset;
		    if ((x_tile+1) == drom_rstr->num_tiles_row)
			width = drom_rstr->last_tile_width;
		    else
			width = MAX_2D_TILESIZE;

		    if ((y_tile+1) == drom_rstr->num_tiles_col)
			height = drom_rstr->last_tile_height;
		    else
			height = MAX_2D_TILESIZE;

		    new_tile = VLLcreate_texture
			       (width, height, rstr->width, type, (int*)ptr);

		    /* If the create-texture call failed, then set the
		    ** texture_2D_valid field false and abort.  */

    		    dde_g4vll_drom_data.texture_2D_valid = DcFalse;

		    if (!new_tile) {
	    		DDspace_Deallocate (drom_rstr->texture);
	    		drom_rstr->texture = DcNullPtr;
			return;
		    }
		    *cur_texture++ = new_tile;
		}
	    }

	} else /* Create nontiled 2D texture if necessary. */ {

	    /* Get the VLL raster type, based on the Dore' raster type.  Note
	    ** that for little-endian machines, we reverse the raster types
	    ** for VLL rather than swapping the bytes in the raster data.  */

	    switch (rstr->type) {
		case DcRasterRGB:   type = VLL_RASTER_RGB;   break;
		case DcRasterABGR:  type = VLL_RASTER_RGBA;  break;
		case DcRasterRGBA:  type = VLL_RASTER_ABGR;  break;
		case DcRasterSpecial: type = VLL_RASTER_PSEUDO_8; break;
		default:  break;
	    }

	    drom_rstr->texture_sequence_number = rstr->sequence_number;

	    /* Allocate & create the new VLL texture. */

	    drom_rstr->texture   = DDspace_Allocate (sizeof(VLLtexture*));
	    drom_rstr->num_tiles = 1;

	    drom_rstr->texture[0] = VLLcreate_texture
		(rstr->width, rstr->height, rstr->width, type, rstr->data);

	    /* If the VLL texture creation failed, then abort and set the
	    ** texture_2D_valid field at false.  */

    	    dde_g4vll_drom_data.texture_2D_valid = DcFalse;

	    if (!drom_rstr->texture[0]) {
		DDspace_Deallocate (drom_rstr->texture);
	    	drom_rstr->texture = DcNullPtr;
		return;
	    }
	}

	drom_rstr->tiling_initialized = DcTrue;
    }

    dde_g4vll_drom_data.texture_2D_valid = DcTrue;

    /* Multiply-tiled images need several texture passes.  However, we may
    ** have completed all of the passes necessary for this texture.  If the
    ** current texture pass is beyond the number needed for this texture,
    ** then set a flag to indicate that subsequent primitives with this
    ** texture map need not be rendered again, and return.  */

    dde_g4vll_drom_data.tex2D_finished
	= (dde_g4vll_drom_data.cur_texture_pass >= drom_rstr->num_tiles);

    if (dde_g4vll_drom_data.tex2D_finished) return;

    /* The following block handles 2D textures that do _not_ require tiling.
    ** These are 2D rasters that are less than or equal to 512 pixels in any
    ** dimension. */

    if (drom_rstr->num_tiles <= 1) {

	/* Set AntiAlias mode for textures. */

	switch (attributes[0].aamode) {
	    case DcTextureAABilinear:
	    case DcTextureAATrilinear:
		aamode = VLL_BI_LINEAR;		/* 2D Texture */
		break;

	    case DcTextureAASumArea:		/* Unsupported Modes */
	    case DcTextureAAMIPPoint:
	    case DcTextureAAMIP2Point:
	    case DcTextureAAMIPBilinear:
	    case DcTextureAAMIP2Bilinear:
	    case DcTextureAANone:
	    default:
		aamode = VLL_POINT_SAMPLE;
		break;
	}
	VLLset_texture_filter_mode(aamode);

	/* Determine and set the extend mode.  DcExtendNone has precedence over
	** DcExtendBlack so allow DcExtendBlack only if DcExtendNone is not
	** set.  */

	umode = ddr_g4vll_drom_vll_extend_mode (attributes[0].uv.extend[0]);
        dde_g4vll_drom_data.vll_extend_mode[0] = umode;
	vmode = ddr_g4vll_drom_vll_extend_mode (attributes[0].uv.extend[1]);
        dde_g4vll_drom_data.vll_extend_mode[1] = vmode;
	VLLset_texture_extend (umode, vmode, VLL_EXTEND_SPECIAL);

	if ((attributes[0].uv.extend[0] == DcExtendBlack ||
	     attributes[0].uv.extend[1] == DcExtendBlack) &&
	     attributes[0].uv.extend[0] != DcExtendNone &&
	     attributes[0].uv.extend[1] != DcExtendNone) {
	    dde_g4vll_drom_data.map_extend_black = DcTrue;
	} else if ((umode == VLL_EXTEND_NONE) && (vmode == VLL_EXTEND_NONE)) {
	    dde_g4vll_drom_data.map_extend_black = DcFalse;
	}

	/* Set the texture matrix.  */

	ddr_g4vll_drom_uv_xform (attributes[0].uv.matrix, usr_matrix);
	ddr_g4vll_drom_set_vll_matrix (VLL_TEXTURE_MATRIX, usr_matrix);
	VLLset_surface_texture (drom_rstr->texture[0]);

	return;
    }

    /* The following code handles 2D textures that must be tiled (i.e. are
    ** larger than MAX_2D_TILESIZE in any dimension).  Global to the drom, we
    ** need to know the maximum number of texture passes needed for all
    ** textures.  */

    dde_g4vll_drom_data.num_texture_passes =
	max (dde_g4vll_drom_data.num_texture_passes, drom_rstr->num_tiles);

    /* Set AntiAlias mode -- use the AA mode that the tiling was based on.  */

    if (drom_rstr->aamode == DcTextureAABilinear)
	VLLset_texture_filter_mode (VLL_BI_LINEAR);
    else
	VLLset_texture_filter_mode (VLL_POINT_SAMPLE);

    /* Set the extend mode.  Only DcExtendNoneNoUpdate and DcExtendNone are
    ** applicable modes when tiling.  The first tile is drawn in DcExtendNone
    ** mode so that the entire object and the first tile are drawn.  All
    ** subsequent tiles use DcExtendNoneNoUpdate so that only the tile itself
    ** is drawn.  */

    if (dde_g4vll_drom_data.cur_texture_pass != 0)
	umode = vmode = wmode = VLL_EXTEND_NONE_NU;
    else
	umode = vmode = wmode = VLL_EXTEND_NONE;

    /* ExtendNone has precedence over Black, so extend Black only if both
    ** U and V are not None.  */

    if ((attributes[0].uv.extend[0] == DcExtendBlack ||
	 attributes[0].uv.extend[1] == DcExtendBlack) &&
	 attributes[0].uv.extend[0] != DcExtendNone &&
	 attributes[0].uv.extend[1] != DcExtendNone) {
	dde_g4vll_drom_data.map_extend_black = DcTrue;
    } else if ((umode == VLL_EXTEND_NONE) && (vmode == VLL_EXTEND_NONE)) {
	dde_g4vll_drom_data.map_extend_black = DcFalse;
    }

    dde_g4vll_drom_data.vll_extend_mode[0] = umode;
    dde_g4vll_drom_data.vll_extend_mode[1] = vmode;
    dde_g4vll_drom_data.vll_extend_mode[2] = wmode;
    VLLset_texture_extend (umode, vmode, wmode);

    /* Construct & set the texture matrix such that the tile is scaled down
    ** & translated appropriately onto the primitive.  */

    tile_number = dde_g4vll_drom_data.cur_texture_pass % drom_rstr->num_tiles;

    x_tile = tile_number % drom_rstr->num_tiles_row;
    y_tile = tile_number / drom_rstr->num_tiles_row;

    dor_matrix_load_identity (tile_matrix);

    if (x_tile != (drom_rstr->num_tiles_row-1)) {
	tile_matrix[0][0] =  1.0 / drom_rstr->width_scaler;
	tile_matrix[0][3] = -1.0 * x_tile;
    } else {
	tile_matrix[0][0] =  1.0 /drom_rstr->last_width_scaler;
	tile_matrix[0][3] = -1.0 * x_tile *
	    drom_rstr->width_scaler / drom_rstr->last_width_scaler;
    }

    if (y_tile != (drom_rstr->num_tiles_col-1)) {
	tile_matrix[1][1] =  1.0 / drom_rstr->height_scaler;
	tile_matrix[1][3] = -1.0 * y_tile;
    } else {
	tile_matrix[1][1] =  1.0 / drom_rstr->last_height_scaler;
	tile_matrix[1][3] = -1.0 * y_tile *
	    drom_rstr->height_scaler / drom_rstr->last_height_scaler;
    }

    /* Also need to take into account the user-specified UV transformations. */

    ddr_g4vll_drom_uv_xform (attributes[0].uv.matrix, usr_matrix);
    dor_matrix_pre_concatenate (tile_matrix, usr_matrix);

    /* Set the VLL Texture Matrix */

    ddr_g4vll_drom_set_vll_matrix (VLL_TEXTURE_MATRIX, tile_matrix);

    /* Set the surface texture with the current tile.  */

    VLLset_surface_texture (drom_rstr->texture [tile_number]);
}



/*****************************************************************************
**  This subroutine computes and loads 3D texture maps.
*****************************************************************************/

static void ddr_g4vll_drom_difclrmap_3d (
    struct rstr         *rstr,
    ddt_g4vll_drom_rstr *drom_rstr,
    dot_stdtexatt        attributes[])
{
    auto     int          aamode;
    auto     VLLtexture **cur_texture;
    register int          ii;
    auto     DtMatrix4x4  tile_matrix;
    auto     int          tile_number;
    auto     int          tiles_per_depth;
    auto     int          type;
    auto     int          umode, vmode, wmode;
    auto     int          width, height, depth;
    auto     int          x_tile, y_tile, z_tile;

    /* The RGB raster data format is not supported for 3D texture mapping. */

    if (rstr->type == DcRasterRGB) return;

    /* If we're not doing a real texture pass, then return. */

    if (dde_g4vll_drom_data.cur_texture_pass < 0) return;

    /* Only point and bilinear sampling are supported when tiling.  */

    if (attributes[0].aamode == DcTextureAABilinear)
    {   if (drom_rstr->aamode == DcTextureAANone)
            drom_rstr->tiling_initialized = DcFalse;
        drom_rstr->aamode = DcTextureAABilinear;
    }
    else
    {   if (drom_rstr->aamode == DcTextureAABilinear) {
            drom_rstr->tiling_initialized = DcFalse;
        }
        drom_rstr->aamode = DcTextureAANone;
    }

    if (drom_rstr->texture_sequence_number != rstr->sequence_number)
	drom_rstr->tiling_initialized = DcFalse;

    /* Determine whether the texture needs to be tiled (do this on the first
    ** pass and subsequently only when either the antialiasing mode changes
    ** or the sequence numbers of the texture and DROM don't match).  If
    ** tiling is necessary, break the texture into pieces no larger than
    ** MAX_3D_TILESIZE in each dimension, and store them away.  */

    if (!drom_rstr->tiling_initialized) {

	/* If any previous textures exist (if this is an update), then delete
        ** them before continuing.  */

	if (drom_rstr->texture)
	{   for (ii=0;  ii < drom_rstr->num_tiles;  ++ii)
		VLLdelete_texture (drom_rstr->texture[ii]);
	    DDspace_Deallocate (drom_rstr->texture);
	    drom_rstr->texture = DcNullPtr;
	}

	dde_g4vll_drom_data.map_tile_aamode = drom_rstr->aamode;
	drom_rstr->texture_sequence_number  = rstr->sequence_number;

	if (  (rstr->width  > MAX_3D_TILESIZE)
	   || (rstr->height > MAX_3D_TILESIZE)
	   || (rstr->depth  > MAX_3D_TILESIZE)
	   )
        {
	    auto int   ptr_offset;
	    auto int   byte_offset;
	    auto char *ptr;

	    if (drom_rstr->aamode == DcTextureAANone) {

		drom_rstr->last_tile_width  = rstr->width  % MAX_3D_TILESIZE;
		drom_rstr->last_tile_height = rstr->height % MAX_3D_TILESIZE;
		drom_rstr->last_tile_depth  = rstr->depth  % MAX_3D_TILESIZE;

		drom_rstr->num_tiles_row = (rstr->width / MAX_3D_TILESIZE)
				 + (drom_rstr->last_tile_width == 0 ? 0 : 1);
		drom_rstr->num_tiles_col = (rstr->height / MAX_3D_TILESIZE)
				 + (drom_rstr->last_tile_height == 0 ? 0 : 1);
		drom_rstr->num_tiles_depth = (rstr->depth / MAX_3D_TILESIZE)
				 + (drom_rstr->last_tile_depth == 0 ? 0 : 1);

	    } else /* DcTextureAABilinear */ {

		/* When doing bilinear anti-aliasing, we need to overlap the
		** tiles by a texel so that the edge texels can still be
		** averaged.  */

		drom_rstr->last_tile_width =
		    (rstr->width - MAX_3D_TILESIZE) % (MAX_3D_TILESIZE - 1);
		drom_rstr->last_tile_height =
		    (rstr->height - MAX_3D_TILESIZE) % (MAX_3D_TILESIZE - 1);
		drom_rstr->last_tile_depth =
		    (rstr->depth - MAX_3D_TILESIZE) % (MAX_3D_TILESIZE - 1);

		drom_rstr->num_tiles_row =
		    1 + ((rstr->width - MAX_3D_TILESIZE) / (MAX_3D_TILESIZE-1))
		      + ((drom_rstr->last_tile_width == 0) ? 0 : 1);
		drom_rstr->num_tiles_col =
		    1 + ((rstr->height - MAX_3D_TILESIZE) / (MAX_3D_TILESIZE-1))
		      + ((drom_rstr->last_tile_height == 0) ? 0 : 1);
		drom_rstr->num_tiles_depth =
		    1 + ((rstr->depth - MAX_3D_TILESIZE) / (MAX_3D_TILESIZE-1))
		      + ((drom_rstr->last_tile_depth == 0) ? 0 : 1);
	    }

	    if (drom_rstr->last_tile_width == 0)
		drom_rstr->last_tile_width = MAX_3D_TILESIZE;

	    if (drom_rstr->last_tile_height == 0)
		drom_rstr->last_tile_height = MAX_3D_TILESIZE;

	    if (drom_rstr->last_tile_depth == 0)
		drom_rstr->last_tile_depth = MAX_3D_TILESIZE;


	    drom_rstr->num_tiles = drom_rstr->num_tiles_row
                                 * drom_rstr->num_tiles_col
				 * drom_rstr->num_tiles_depth;

	    drom_rstr->width_scaler  = ((DtReal)MAX_3D_TILESIZE) / rstr->width;
	    drom_rstr->height_scaler = ((DtReal)MAX_3D_TILESIZE) / rstr->height;
	    drom_rstr->depth_scaler  = ((DtReal)MAX_3D_TILESIZE) / rstr->depth;

	    drom_rstr->last_width_scaler  =
		    ((DtReal)drom_rstr->last_tile_width) / rstr->width;
	    drom_rstr->last_height_scaler =
		    ((DtReal)drom_rstr->last_tile_height) / rstr->height;
	    drom_rstr->last_depth_scaler =
		    ((DtReal)drom_rstr->last_tile_depth) / rstr->depth;

	    /* The currently supported raster types for 3D texture mapping are
	    ** ABGR and RGBA.  Rather than swapping the individual bytes in
	    ** the raster data for little-endian machines, we just specify
	    ** the opposite VLL raster type.  */

	    byte_offset = 4;

            if (rstr->type == DcRasterRGBA)
                type = VLL_RASTER_ABGR;
            else
                type = VLL_RASTER_RGBA;

	    drom_rstr->texture =
		DDspace_Allocate (sizeof(VLLtexture*) * drom_rstr->num_tiles);

	    cur_texture = drom_rstr->texture;

	    for (z_tile=0; z_tile < drom_rstr->num_tiles_depth; ++z_tile) {
		for (y_tile=0; y_tile < drom_rstr->num_tiles_col; ++y_tile) {
		    for (x_tile=0; x_tile < drom_rstr->num_tiles_row; ++x_tile){

			auto VLLtexture *new_tile;

			if (drom_rstr->aamode == DcTextureAANone) {
			    ptr_offset =
			    (   (z_tile * rstr->width * rstr->height
                                 * MAX_3D_TILESIZE)
			      + (y_tile * rstr->width * MAX_3D_TILESIZE)
                              + (x_tile * MAX_3D_TILESIZE)
			    ) * byte_offset;

			} else /* DcTextureAABilinear */ {
			    /* There is a one-texel overlap on each tile
			    **  boundary when bilinear sampling.  */

			    ptr_offset =
                            (   (z_tile * rstr->width * rstr->height
				 * MAX_3D_TILESIZE)
			      + (y_tile * rstr->width * MAX_3D_TILESIZE)
			      + ((x_tile * MAX_3D_TILESIZE) - x_tile)
			    ) * byte_offset;
			}

			ptr = (char *)rstr->data + ptr_offset;

			if ((x_tile + 1) == drom_rstr->num_tiles_row)
			    width = drom_rstr->last_tile_width;
			else
			    width = MAX_3D_TILESIZE;

			if ((y_tile + 1) == drom_rstr->num_tiles_col)
			    height = drom_rstr->last_tile_height;
			else
			    height = MAX_3D_TILESIZE;

			if ((z_tile + 1) == drom_rstr->num_tiles_depth)
			    depth = drom_rstr->last_tile_depth;
			else
			    depth = MAX_3D_TILESIZE;

			new_tile = VLLcreate_3d_texture
				   (   width, height, depth, rstr->width,
				       rstr->width * rstr->height,
				       type, (int*) ptr
				   );

			/* 
			 * If the create-texture call failed, then set the
			 * texture_3D_valid field at false, free the allocated
		         * texture data pointers, and then abort.  
			 */

    			dde_g4vll_drom_data.texture_3D_valid = DcFalse;

			if (!new_tile) {
	    		    DDspace_Deallocate (drom_rstr->texture);
	    		    drom_rstr->texture = DcNullPtr;
			    return;
			}
			*cur_texture++ = new_tile;
		    }
		}
	    }

	} else /* create nontiled 3D texture if necessary */ {

	    /* 3D texture maps support RGBA and ABGR raster types.  For little-
	    ** endian machines, we merely swap the raster types, rather than
	    ** swapping the bytes of the actual raster data.  */

	    if (rstr->type == DcRasterRGBA)
		type = VLL_RASTER_ABGR;
	    else
		type = VLL_RASTER_RGBA;

	    drom_rstr->texture = DDspace_Allocate (sizeof(VLLtexture*));
	    drom_rstr->texture[0] = VLLcreate_3d_texture
				    (   rstr->width, rstr->height, rstr->depth,
				        rstr->width, rstr->width*rstr->height,
				        type, rstr->data
				    );

	    /* 
	     * If the VLL texture creation failed, then set the 
	     * texture_3D_valid field at false, free the allocated texture 
	     * data pointers, and return.  
	     */

    	    dde_g4vll_drom_data.texture_3D_valid = DcFalse;

	    if (!drom_rstr->texture[0]) {
	    	DDspace_Deallocate (drom_rstr->texture);
	    	drom_rstr->texture = DcNullPtr;
		return;
	    }
	}

	drom_rstr->tiling_initialized = DcTrue;
    }

    dde_g4vll_drom_data.texture_3D_valid = DcTrue;

    /* Multiply-tiled images need several texture passes.  However, we may
    ** have completed all of the passes necessary for THIS texture (tiled or
    ** not).  If the current texture pass is beyond the number needed for this
    ** texture, then set a flag to indicate that subsequent primitives (with
    ** this texture map) need not be rendered again, and return.  */

    dde_g4vll_drom_data.tex3D_finished
	= (dde_g4vll_drom_data.cur_texture_pass >= drom_rstr->num_tiles);

    if (dde_g4vll_drom_data.tex3D_finished) return;

    /* The following block handles 3D textures that do NOT require tiling.
    ** These are 3D textures that are no larger than MAX_3D_TILESIZE in
    ** any dimension. */

    if (drom_rstr->num_tiles == 1) {

	/* Set AntiAlias mode for textures. */

	switch(attributes[0].aamode) {
	    case DcTextureAABilinear:
	    case DcTextureAATrilinear:
		aamode = VLL_TRI_LINEAR;	/* 3D Texture */
		break;

	    case DcTextureAASumArea:		/* Unsupported Modes */
	    case DcTextureAAMIPPoint:
	    case DcTextureAAMIP2Point:
	    case DcTextureAAMIPBilinear:
	    case DcTextureAAMIP2Bilinear:
	    case DcTextureAANone:
	    default:
		aamode = VLL_POINT_SAMPLE;
		break;
	}
	VLLset_texture_filter_mode(aamode);

	/* Determine and set the extend mode.  DcExtendNone has precedence
	** over DcExtendBlack so allow DcExtendBlack only if DcExtendNone
	** is not set.  */

	umode = ddr_g4vll_drom_vll_extend_mode (attributes[0].uvw.extend[0]);
	vmode = ddr_g4vll_drom_vll_extend_mode (attributes[0].uvw.extend[1]);
	wmode = ddr_g4vll_drom_vll_extend_mode (attributes[0].uvw.extend[2]);
    	dde_g4vll_drom_data.vll_extend_mode[0] = umode;
    	dde_g4vll_drom_data.vll_extend_mode[1] = vmode;
    	dde_g4vll_drom_data.vll_extend_mode[2] = wmode;
	VLLset_texture_extend (umode, vmode, wmode);

	if (  (  (attributes[0].uvw.extend[0] == DcExtendBlack)
	      || (attributes[0].uvw.extend[1] == DcExtendBlack)
	      || (attributes[0].uvw.extend[2] == DcExtendBlack)
	      )
	   &&
	      (  (attributes[0].uvw.extend[0] != DcExtendNone)
	      && (attributes[0].uvw.extend[1] != DcExtendNone)
	      && (attributes[0].uvw.extend[2] != DcExtendNone)
	      )
	   ) {
	    dde_g4vll_drom_data.map_extend_black = DcTrue;
	}
	else if (  (umode == VLL_EXTEND_NONE)
		&& (vmode == VLL_EXTEND_NONE)
		&& (wmode == VLL_EXTEND_NONE)
		) {
	    dde_g4vll_drom_data.map_extend_black = DcFalse;
	}

	/* Set the Texture Matrix */

	ddr_g4vll_drom_set_vll_matrix
	    (VLL_TEXTURE_MATRIX, attributes[0].uvw.matrix);
	VLLset_surface_texture (drom_rstr->texture[0]);

	return;
    }

    /* The following code handles 3D textures that must be tiled (i.e. are
    ** larger than MAX_3D_TILESIZE in any dimension).  Global to the drom, we
    ** need to know the maximum number of texture passes needed for all
    ** textures. */

    dde_g4vll_drom_data.num_texture_passes =
	max (dde_g4vll_drom_data.num_texture_passes, drom_rstr->num_tiles);

    /* Set AntiAlias mode -- use the AA mode that the tiling was based on.  */

    if (drom_rstr->aamode == DcTextureAABilinear)
	VLLset_texture_filter_mode (VLL_BI_LINEAR);
    else
	VLLset_texture_filter_mode (VLL_POINT_SAMPLE);

    /* Set the extend mode.  Only DcExtendNoneNoUpdate and DcExtendNone are
    ** applicable modes when tiling.  The first tile is drawn in DcExtendNone
    ** mode so that the entire object and the first tile are drawn.  All
    ** subsequent tiles use DcExtendNoneNoUpdate so that only the tile itself
    ** is drawn.  */

    if (dde_g4vll_drom_data.cur_texture_pass != 0)
	umode = vmode = wmode = VLL_EXTEND_NONE_NU;
    else
	umode = vmode = wmode = VLL_EXTEND_NONE;

    /* ExtendNone has precedence over Black, so extend Black only if all of
    ** U,V,W are not None.  */

    if (  (  (attributes[0].uvw.extend[0] == DcExtendBlack)
	  || (attributes[0].uvw.extend[1] == DcExtendBlack)
	  || (attributes[0].uvw.extend[2] == DcExtendBlack)
	  )
       &&
	  (  (attributes[0].uvw.extend[0] != DcExtendNone)
	  && (attributes[0].uvw.extend[1] != DcExtendNone)
	  && (attributes[0].uvw.extend[2] != DcExtendNone)
	  )
       ) {
	dde_g4vll_drom_data.map_extend_black = DcTrue;
    }
    else if (  (umode == VLL_EXTEND_NONE)
	    && (vmode == VLL_EXTEND_NONE)
	    && (wmode == VLL_EXTEND_NONE)
	    ) {
	dde_g4vll_drom_data.map_extend_black = DcFalse;
    }

    dde_g4vll_drom_data.vll_extend_mode[0] = umode;
    dde_g4vll_drom_data.vll_extend_mode[1] = vmode;
    dde_g4vll_drom_data.vll_extend_mode[2] = wmode;
    VLLset_texture_extend (umode,vmode, wmode);

    /* Construct & set the texture matrix such that the tile is scaled down
    ** & translated appropriately onto the primitive.  */

    tile_number = dde_g4vll_drom_data.cur_texture_pass % drom_rstr->num_tiles;

    tiles_per_depth = drom_rstr->num_tiles_row * drom_rstr->num_tiles_col;
    z_tile = tile_number / tiles_per_depth;
    x_tile = tile_number % drom_rstr->num_tiles_row;
    y_tile = (tile_number -(z_tile/tiles_per_depth))/ drom_rstr->num_tiles_row;

    dor_matrix_load_identity (tile_matrix);

    if (x_tile != (drom_rstr->num_tiles_row-1)) {
	tile_matrix[0][0] =  1.0 / drom_rstr->width_scaler;
	tile_matrix[0][3] = -1.0 * x_tile;
    } else {
	tile_matrix[0][0] =  1.0 / drom_rstr->last_width_scaler;
	tile_matrix[0][3] = -1.0 * x_tile *
	    drom_rstr->width_scaler / drom_rstr->last_width_scaler;
    }

    if (y_tile != (drom_rstr->num_tiles_col-1)) {
	tile_matrix[1][1] =  1.0 / drom_rstr->height_scaler;
	tile_matrix[1][3] = -1.0 * y_tile;
    } else {
	tile_matrix[1][1] =  1.0 / drom_rstr->last_height_scaler;
	tile_matrix[1][3] = -1.0 * y_tile *
	    drom_rstr->height_scaler / drom_rstr->last_height_scaler;
    }

    if (z_tile != (drom_rstr->num_tiles_depth-1)) {
	tile_matrix[2][2] =  1.0 / drom_rstr->depth_scaler;
	tile_matrix[2][3] = -1.0 * y_tile;
    } else {
	tile_matrix[2][2] =  1.0 / drom_rstr->last_depth_scaler;
	tile_matrix[2][3] = -1.0 * z_tile *
	    drom_rstr->depth_scaler / drom_rstr->last_depth_scaler;
    }

    /* Also need to take into account the user-specified uv transformations. */

    dor_matrix_pre_concatenate (tile_matrix, attributes[0].uvw.matrix);

    /* Set the VLL Texture Matrix.  */

    ddr_g4vll_drom_set_vll_matrix (VLL_TEXTURE_MATRIX, tile_matrix);

    /* Set the surface texture with the current tile.  */

    VLLset_surface_texture (drom_rstr->texture [tile_number]);
}



/*****************************************************************************
**  This procedure sets the environment-mapping attribute, and is called
**  whenever the environment-mapping parameters change.  If there are multiple
**  maps, then they are applied in order using the operator attribute in the
**  'attributes' parameter.
*****************************************************************************/

void ddr_g4vll_drom_set_att_mapenv (
    DtInt              count,		/* # of Environment Maps */
    DtObject           mapping[],	/* Spherical, Cubic, ... */
    DtObjectStructure *raster[],	/* Raster Map Data */
    dot_stdtexatt      attributes[])	/* Texture Attributes */
{
    static DtInt rstr_classid = -1;
    static DtInt filerstr_classid = -1;
    static DtInt rstr_dataindex = -1;
    static DtInt filerstr_dataindex = -1;
    DtInt rasterid;
    ddt_g4vll_drom_rstr *drom_rstr;
    DtInt ii;
    int aamode;
    DtObjectStructure *raster_obj = (DtObjectStructure *)0;
    struct rstr *rstr;

    dde_g4vll_drom_data.texture_env_valid = DcFalse;

    if (dde_g4vll_drom_data.pre_init) return;

    if (!dde_g4vll_drom_curdevdat->features->env_texture_mapping) return;

    if (count <= 0) return;

    if (!raster[0]) return;

    rasterid = DsInqObjClassId(raster[0]);

    /* If rasterid is a file raster, then create a memory raster as an
    ** alternate object.  */

    if (filerstr_classid == -1) filerstr_classid = DsInqClassId("DoFileRaster");
    if (rasterid == filerstr_classid) {
	if (filerstr_dataindex == -1) {
	    for (ii=0; dde_g4vll_drom_class_data[ii].name != 0; ii++) {
		if (dde_g4vll_drom_class_data[ii].id == filerstr_classid) {
		    filerstr_dataindex = dde_g4vll_drom_class_data[ii].index;
		    break;
		}
	    }
	}

	ddr_g4vll_drom_filerstr_update (raster[0]);

	raster_obj = (DtObjectStructure *)raster[0]->additional_data[filerstr_dataindex];
    }


    if (rstr_classid == -1) rstr_classid = DsInqClassId("DoRaster");

    if (rasterid == rstr_classid) {
	raster_obj = raster[0];
    }

    if (!raster_obj) return;

    if (rstr_dataindex == -1) {
	for (ii=0; dde_g4vll_drom_class_data[ii].name != 0; ii++) {
	    if (dde_g4vll_drom_class_data[ii].id == rstr_classid) {
		rstr_dataindex = dde_g4vll_drom_class_data[ii].index;
		break;
	    }
	}
    }

    ddr_g4vll_drom_rstr_update_env (raster_obj);

    rstr = (struct rstr *)(raster_obj->data);
    drom_rstr =
	    (ddt_g4vll_drom_rstr *)raster_obj->additional_data[rstr_dataindex];

    if (drom_rstr->env_texture != (VLLtexture *)0) {
	/*
	 * Set AntiAlias mode
	 */
	switch(attributes[0].aamode) {
	    case DcTextureAABilinear:
		aamode = VLL_BI_LINEAR;
		break;
	    case DcTextureAATrilinear:
		aamode = VLL_TRI_LINEAR;
		break;
	    case DcTextureAAMIPPoint:
	    case DcTextureAAMIP2Point:
	    case DcTextureAAMIPBilinear:
	    case DcTextureAAMIP2Bilinear:
	    case DcTextureAASumArea:
	    case DcTextureAANone:
	    default:
		aamode = VLL_POINT_SAMPLE;
		break;
	}
	VLLset_texture_filter_mode(aamode);

	if (mapping[0] == DcStdCubeEnvironMap && rstr->depth == 6) {
	    VLLset_surface_texture(drom_rstr->env_texture);

	    dde_g4vll_drom_data.texture_env_valid = DcTrue;
	}
    }
}



/*****************************************************************************
**  This routine enables/disables subsequent environment texture mapping.
*****************************************************************************/

void ddr_g4vll_drom_set_att_mapenvswi (DtSwitch switchvalue)
{
    dde_g4vll_drom_data.map_env_switch = switchvalue;

    if (!dde_g4vll_drom_data.pre_init)
	ddr_g4vll_drom_set_prim_execute();
}



/*****************************************************************************
**  These routines set the transparency-mapping attribute.  They are
**  unimplemented for now.
*****************************************************************************/

void ddr_g4vll_drom_set_att_maptrnintswi (DtSwitch switchvalue)
{
}

void ddr_g4vll_drom_set_att_maptrnint (
    DtInt count,
    DtObject mapping[],
    DtObject raster[],
    dot_stdtexatt attributes[])
{
}
