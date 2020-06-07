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
	ddr_g4vll_drom_rstr_create
	ddr_g4vll_drom_rstr_print
	ddr_g4vll_drom_rstr_delete
	ddr_g4vll_drom_rstr_update
	ddr_g4vll_drom_rstr_update_env
	ddr_g4vll_drom_rstr_update_background

  ======================================================================
 */
#include "drom.h"
#include <dore/dore_develop/private/rstr.h>

/*
 ======================================================================
 */
static DtInt rstr_dataindex = -1;
static DtInt rstr_classid = -1;
/*
  ======================================================================
 */

ddt_g4vll_drom_rstr *ddr_g4vll_drom_rstr_create (DtObject object)
{
    register ddt_g4vll_drom_rstr *drom_rstr;

    drom_rstr = DDspace_Allocate (sizeof(ddt_g4vll_drom_rstr));

    drom_rstr->texture_sequence_number = -1;
    drom_rstr->texture_data = DcNullPtr;

    drom_rstr->env_texture_sequence_number = -1;
    drom_rstr->env_texture = 0;
    drom_rstr->env_texture_data = DcNullPtr;

    drom_rstr->background_data = 0;
    drom_rstr->background_mode = 0;
    drom_rstr->background_sequence_number = -1;

    drom_rstr->tiling_initialized = DcFalse;
    drom_rstr->num_tiles          = 1;
    drom_rstr->num_tiles_row      = 1;
    drom_rstr->num_tiles_col      = 1;
    drom_rstr->num_tiles_depth    = 1;
    drom_rstr->last_tile_width    = 0;
    drom_rstr->last_tile_height   = 0;
    drom_rstr->last_tile_depth    = 0;
    drom_rstr->width_scaler       = 0.0;
    drom_rstr->height_scaler      = 0.0;
    drom_rstr->depth_scaler       = 0.0;
    drom_rstr->last_width_scaler  = 0.0;
    drom_rstr->last_height_scaler = 0.0;
    drom_rstr->last_depth_scaler  = 0.0;
    drom_rstr->aamode             = DcTextureAANone;
    drom_rstr->texture            = 0;

    return drom_rstr;
}
/*
  ======================================================================
 */
void ddr_g4vll_drom_rstr_print (
    DtObject object,
    ddt_g4vll_drom_rstr *data)
{
}
/*
  ======================================================================
 */
void ddr_g4vll_drom_rstr_delete (
    DtObject object,
    ddt_g4vll_drom_rstr *data)
{
    register int ii;	/* Tile Index */

    /* If there's nothing to do, then we just finished. */

    if (!data) return;

    /* Delete all texture tiles created for the texture and deallocate the
    ** list of pointers. */

    if (data->texture)
    {   for (ii=0;  ii < data->num_tiles;  ++ii)
	    VLLdelete_texture (data->texture[ii]);
	DDspace_Deallocate (data->texture);
    }

    if (data->texture_data)
	DDspace_Deallocate (data->texture_data);

    /* Delete the background raster data as long as we have data to begin
    ** with, and it doesn't belong to the user.  */

    if (  data->background_data
       && !(data->background_mode & ddd_g4vll_drom_rstr_user_data)
       ) {
	DDspace_Deallocate (data->background_data);
    }

    DDspace_Deallocate(data);
}
/*
  ======================================================================
 */
void ddr_g4vll_drom_rstr_update (
    DtObjectStructure *object)
{
    struct rstr *rstr;
    ddt_g4vll_drom_rstr *drom_rstr;
    int type;
    int i;

    if (rstr_classid == -1) {
	rstr_classid = DsInqClassId("DoRaster");
    }

    if (rstr_dataindex == -1) {
	for (i=0; dde_g4vll_drom_class_data[i].name != 0; i++) {
	    if (dde_g4vll_drom_class_data[i].id == rstr_classid) {
		rstr_dataindex = dde_g4vll_drom_class_data[i].index;
		break;
	    }
	}
    }

    rstr = (struct rstr *)(object->data);
    drom_rstr = (ddt_g4vll_drom_rstr *)object->additional_data[rstr_dataindex];

    switch (rstr->type) {
	case DcRasterRGB:	type = VLL_RASTER_RGB;		break;
	case DcRasterRGBA:	type = VLL_RASTER_RGBA;		break;
	default:		return;
    }
}
/*
  ======================================================================
 */
void ddr_g4vll_drom_rstr_update_env (
    DtObjectStructure *object)
{
    struct rstr *rstr;
    ddt_g4vll_drom_rstr *drom_rstr;
    int type;
    int i;

    if (rstr_classid == -1) {
        rstr_classid = DsInqClassId("DoRaster");
    }

    if (rstr_dataindex == -1) {
        for (i=0; dde_g4vll_drom_class_data[i].name != 0; i++) {
            if (dde_g4vll_drom_class_data[i].id == rstr_classid) {
                rstr_dataindex = dde_g4vll_drom_class_data[i].index;
                break;
            }
        }
    }

    rstr = (struct rstr *)(object->data);
    drom_rstr = (ddt_g4vll_drom_rstr *)object->additional_data[rstr_dataindex];

    if (dor_util_bigendian_word_byte_order()) {
        switch (rstr->type) {
            case DcRasterRGBA:  type = VLL_RASTER_RGBA; break;
            case DcRasterABGR:  type = VLL_RASTER_ABGR; break;
            default:            return;
        }
    } else {
        /* this is a hack: we are on a little-endian machine and
           we don't want to swap the bytes for vll so we reverse
           the vll raster type */
        switch (rstr->type) {
            case DcRasterRGBA:  type = VLL_RASTER_ABGR; break;
            case DcRasterABGR:  type = VLL_RASTER_RGBA; break;
            default:            return;
        }
    }

    if ((drom_rstr->env_texture_sequence_number != rstr->sequence_number) ||
        drom_rstr->env_texture == 0) {

        if (drom_rstr->env_texture != (VLLtexture *)0) {
            VLLdelete_texture(drom_rstr->env_texture);
        }
        if (drom_rstr->env_texture_data != DcNullPtr) {
            DDspace_Deallocate (drom_rstr->env_texture_data);
            drom_rstr->env_texture_data = DcNullPtr;
        }

        if (rstr->depth == 6) {
#ifdef DEBUG_TEXTURE
printf ("\tcreate an environment map texture\n");
#endif
            drom_rstr->env_texture = VLLcreate_env_texture(rstr->width,
			rstr->height, rstr->width, type, rstr->data);
        }
        drom_rstr->env_texture_sequence_number = rstr->sequence_number;
    }
}
/*
  ======================================================================
 */
void ddr_g4vll_drom_rstr_update_background (
    DtObjectStructure *object)
{
    struct rstr *rstr;
    ddt_g4vll_drom_rstr *drom_rstr;
    int i;
    int w, h, skip, have_z, have_alpha;
    unsigned char *d1;
    unsigned char *d2;

    if (rstr_classid == -1) {
	rstr_classid = DsInqClassId("DoRaster");
    }


    if (rstr_dataindex == -1) {
	for (i=0; dde_g4vll_drom_class_data[i].name != 0; i++) {
	    if (dde_g4vll_drom_class_data[i].id == rstr_classid) {
		rstr_dataindex = dde_g4vll_drom_class_data[i].index;
		break;
	    }
	}
    }

    rstr = (struct rstr *)(object->data);
    drom_rstr = (ddt_g4vll_drom_rstr *)object->additional_data[rstr_dataindex];

    /* the VLLput_iamge call will later be made with VLL_RASTER_ABGR (or ZABGR
       if data has Z). So we need to pack the data in that format. Note
       that the packing is done differently if this is a little endian machine */

    if ((drom_rstr->background_sequence_number != rstr->sequence_number) ||
	(drom_rstr->background_data == 0)) {

	switch (rstr->type) {
	case DcRasterRGB:
	     skip = 3; have_alpha = 0; have_z = 0;
	     break;
	case DcRasterRGBA:
	     if (dor_util_bigendian_word_byte_order()) {
	         skip = 4; have_alpha = 1; have_z = 0;
	         break;
	     } else {
	         drom_rstr->background_mode = ddd_g4vll_drom_rstr_user_data;
	         drom_rstr->background_data = rstr->data;
	         return;
	         }
	case DcRasterRGBZ:
	     skip = 7; have_alpha = 0; have_z = 3; 
	     break;
	case DcRasterRGBAZ:
	     skip = 8; have_alpha = 1; have_z = 4;
	     break;
	case DcRasterABGR:
	     if (dor_util_bigendian_word_byte_order()) {
	         drom_rstr->background_mode = ddd_g4vll_drom_rstr_user_data;
	         drom_rstr->background_data = rstr->data;
	         return;
	     } else {
	         skip = 4; have_alpha = 1; have_z = 0;
	        break;
	    }
	default:
	     return;
	}
      
	if (drom_rstr->background_data != 0 && 
	    (!(drom_rstr->background_mode & ddd_g4vll_drom_rstr_user_data))) {
	    dor_space_deallocate(drom_rstr->background_data);
	}

	if (have_z) {
	    drom_rstr->background_mode |= ddd_g4vll_drom_rstr_with_z;
	    if ((drom_rstr->background_data = (unsigned char *)
		 dor_space_allocate(rstr->width*rstr->height*8)) == NULL) {
		return;
	    }
	} else {
	    if ((drom_rstr->background_data = (unsigned char *)
		 dor_space_allocate(rstr->width*rstr->height*4)) == NULL) {
		return;
	    }
	}
	d1 = (unsigned char *)rstr->data;
	d2 = (unsigned char *)drom_rstr->background_data;

	if (dor_util_bigendian_word_byte_order()) {
	    for (h=rstr->height; h--;) {
	        for (w=rstr->width; w--;) {
		    if (have_z) {
		        d2[0] = d1[have_z+0];
		        d2[1] = d1[have_z+1];
		        d2[2] = d1[have_z+2];
		        d2[3] = d1[have_z+3];
		        d2 += 4;
		    }
	       
		    if (have_alpha) d2[0] = d1[3];
		    d2[1] = d1[2];
		    d2[2] = d1[1];
		    d2[3] = d1[0];

		    d1 += skip;
		    d2 += 4;
	        }
	    }
	} else {
	   /* this is a little endian machine: we need to pack the data
              in byte ordering RGBA (or ZRGBA) */

	    for (h=rstr->height; h--;) {
	        for (w=rstr->width; w--;) {
		    if (have_z) {
		        d2[0] = d1[have_z+0];
		        d2[1] = d1[have_z+1];
		        d2[2] = d1[have_z+2];
		        d2[3] = d1[have_z+3];
		        d2 += 4;
		    }
	       
	            if (rstr->type == DcRasterABGR) {
		        d2[0] = d1[3];
		        d2[1] = d1[2];
		        d2[2] = d1[1];
		        d2[3] = d1[0];
	            } else {
                        /* RGB bytes are in the right place */
		        d2[0] = d1[0];
		        d2[1] = d1[1];
		        d2[2] = d1[2];
		        if (have_alpha) d2[3] = d1[3];
	            }

		    d1 += skip;
		    d2 += 4;
	        }
	    }
	}
      
	drom_rstr->background_sequence_number = rstr->sequence_number;
    }
}
