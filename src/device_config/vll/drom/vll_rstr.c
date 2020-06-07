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
	ddt_vll_drom_rstr * ddr_vll_drom_rstr_create (object)
	void ddr_vll_drom_rstr_print (object, data)
	void ddr_vll_drom_rstr_delete (object, data)
	void ddr_vll_drom_rstr_update (object)
	void ddr_vll_drom_rstr_update_env (object)
	void ddr_vll_drom_rstr_update_background (object)

  ======================================================================
 */
#include "drom.h"
#include "dore/dore_develop/private/rstr.h"

/*
 ======================================================================
 */
static DtInt rstr_dataindex = -1;
static DtInt rstr_classid = -1;
/*
  ======================================================================
 */
ddt_vll_drom_rstr *
ddr_vll_drom_rstr_create(object)
DtObject object;
{
    ddt_vll_drom_rstr *dynout_rstr;

    dynout_rstr =  (ddt_vll_drom_rstr *)
	    DDspace_Allocate(sizeof(ddt_vll_drom_rstr));

    dynout_rstr->texture_sequence_number = -1;
    dynout_rstr->texture = 0;
    dynout_rstr->texture_data = DcNullPtr;

    dynout_rstr->env_texture_sequence_number = -1;
    dynout_rstr->env_texture = 0;
    dynout_rstr->env_texture_data = DcNullPtr;

    dynout_rstr->background_data = 0;
    dynout_rstr->background_mode = 0;
    dynout_rstr->background_sequence_number = -1;
    return dynout_rstr;
}
/*
  ======================================================================
 */
void
ddr_vll_drom_rstr_print(object, data)
DtObject object;
ddt_vll_drom_rstr *data;
{
}
/*
  ======================================================================
 */
void
ddr_vll_drom_rstr_delete(object, data)
DtObject object;
ddt_vll_drom_rstr *data;
{
    if  (data != (ddt_vll_drom_rstr *)0) {
	if (data->texture != (VLLtexture *)0)
		VLLdelete_texture(data->texture);

	if (data->texture_data != DcNullPtr) {
	    DDspace_Deallocate(data->texture_data);
	}

	if ((!(data->background_mode) & ddd_vll_drom_rstr_user_data) &&
	    data->background_data != DcNullPtr) {
	    DDspace_Deallocate(data->background_data);
	}
	DDspace_Deallocate(data);
    }
}
/*
  ======================================================================
 */
void
ddr_vll_drom_rstr_update (object)
DtObjectStructure *object;
{
    struct rstr *rstr;
    ddt_vll_drom_rstr *dynout_rstr;
    int type;
    int i;
    int mode;

    if (rstr_classid == -1) {
	rstr_classid = DsInqClassId("DoRaster");
    }

    if (rstr_dataindex == -1) {
	for (i=0; dde_vll_drom_class_data[i].name != 0; i++) {
	    if (dde_vll_drom_class_data[i].id == rstr_classid) {
		rstr_dataindex = dde_vll_drom_class_data[i].index;
		break;
	    }
	}
    }

    rstr = (struct rstr *)(object->data);
    dynout_rstr = (ddt_vll_drom_rstr *)object->additional_data[rstr_dataindex];

    switch (rstr->type) {
    case DcRasterRGB:	type = VLL_RASTER_RGB;		break;
    case DcRasterRGBA:	type = VLL_RASTER_RGBA;		break;
    case DcRasterRGBZ:	type = VLL_RASTER_RGBZ;		break;
    case DcRasterRGBAZ:	type = VLL_RASTER_RGBAZ;	break;
    case DcRasterABGR:	type = VLL_RASTER_ABGR;		break;
    default:		return;
    }

    /*
    /* If we don't have any raster data yet or if the raster data has been
     * altered since it was last used, update the texture.
     */
    if ((dynout_rstr->texture_sequence_number != rstr->sequence_number) ||
	dynout_rstr->texture == 0) {

	if (dynout_rstr->texture != (VLLtexture *)0) {
	    VLLdelete_texture(dynout_rstr->texture);
	}
	if (dynout_rstr->texture_data != DcNullPtr) {
	    DDspace_Deallocate (dynout_rstr->texture_data);
	    dynout_rstr->texture_data = DcNullPtr;
	}

	if (rstr->depth > 1) {
	    dynout_rstr->texture = VLLcreate_3d_texture(rstr->width, rstr->height,
			rstr->depth, type, rstr->data);
	} else if (rstr->width <= 256 && rstr->height <= 256 &&
		   (rstr->width < 256 || rstr->height < 256)) {
	    /* 
	     * Expand the raster data to be 256x256 ABGR if the data is 
	     * smaller than 256 in width and/or height.
	     */
	    if ((dynout_rstr->texture_data = (unsigned char *)
		 DDspace_Allocate(256*256*4)) == NULL) {
		return;
	    }
	    mode = ddr_vll_dcm_inq_texture_expand();
	    if (mode == ddd_vll_expand_default) {
		if ((rstr->width <= 64 || rstr->height <= 64)) {
		    mode = ddd_vll_expand_replicate;
		} else {
		    mode = ddd_vll_expand_bilinear;
		}
	    }
	    if (mode == ddd_vll_expand_replicate) {
		VLLexpand_image(VLL_IMAGE_EXPAND_REPLICATE,
				type, rstr->data, rstr->width, rstr->height,
				VLL_RASTER_ABGR, dynout_rstr->texture_data,
				256, 256);
	    } else {
		VLLexpand_image(VLL_IMAGE_EXPAND_BILINEAR,
				type, rstr->data, rstr->width, rstr->height,
				VLL_RASTER_ABGR, dynout_rstr->texture_data,
				256, 256);
	    }
	    dynout_rstr->texture = VLLcreate_texture(256, 256, VLL_RASTER_ABGR,
						     dynout_rstr->texture_data);

	} else {
	    dynout_rstr->texture = VLLcreate_texture(rstr->width, rstr->height,
						     type, rstr->data);
	}
	dynout_rstr->texture_sequence_number = rstr->sequence_number;
    }
}
/*
  ======================================================================
 */
void
ddr_vll_drom_rstr_update_env (object)
DtObjectStructure *object;
{
    struct rstr *rstr;
    ddt_vll_drom_rstr *dynout_rstr;
    int type;
    int i;
    int mode;

    if (rstr_classid == -1) {
	rstr_classid = DsInqClassId("DoRaster");
    }

    if (rstr_dataindex == -1) {
	for (i=0; dde_vll_drom_class_data[i].name != 0; i++) {
	    if (dde_vll_drom_class_data[i].id == rstr_classid) {
		rstr_dataindex = dde_vll_drom_class_data[i].index;
		break;
	    }
	}
    }

    rstr = (struct rstr *)(object->data);
    dynout_rstr = (ddt_vll_drom_rstr *)object->additional_data[rstr_dataindex];

    switch (rstr->type) {
    case DcRasterRGB:	type = VLL_RASTER_RGB;		break;
    case DcRasterRGBA:	type = VLL_RASTER_RGBA;		break;
    case DcRasterRGBZ:	type = VLL_RASTER_RGBZ;		break;
    case DcRasterRGBAZ:	type = VLL_RASTER_RGBAZ;	break;
    case DcRasterABGR:	type = VLL_RASTER_ABGR;		break;
    default:		return;
    }

    if ((dynout_rstr->env_texture_sequence_number != rstr->sequence_number) ||
	dynout_rstr->env_texture == 0) {

	if (dynout_rstr->env_texture != (VLLtexture *)0) {
	    VLLdelete_texture(dynout_rstr->env_texture);
	}
	if (dynout_rstr->env_texture_data != DcNullPtr) {
	    DDspace_Deallocate (dynout_rstr->env_texture_data);
	    dynout_rstr->env_texture_data = DcNullPtr;
	}

	if (rstr->depth == 6) {
	    dynout_rstr->env_texture = VLLcreate_env_texture(rstr->width, 
							 rstr->height,
							 type, rstr->data);
	} 
	dynout_rstr->env_texture_sequence_number = rstr->sequence_number;
    }
}
/*
  ======================================================================
 */
void
ddr_vll_drom_rstr_update_background (object)
DtObjectStructure *object;
{
    struct rstr *rstr;
    ddt_vll_drom_rstr *dynout_rstr;
    int i;
    int w, h, skip, have_z, have_alpha;
    unsigned char *d1;
    unsigned char *d2;

    if (rstr_classid == -1) {
	rstr_classid = DsInqClassId("DoRaster");
    }

    if (rstr_dataindex == -1) {
	for (i=0; dde_vll_drom_class_data[i].name != 0; i++) {
	    if (dde_vll_drom_class_data[i].id == rstr_classid) {
		rstr_dataindex = dde_vll_drom_class_data[i].index;
		break;
	    }
	}
    }

    rstr = (struct rstr *)(object->data);
    dynout_rstr = (ddt_vll_drom_rstr *)object->additional_data[rstr_dataindex];

    if ((dynout_rstr->background_sequence_number != rstr->sequence_number) ||
	(dynout_rstr->background_data == 0)) {

	switch (rstr->type) {
	case DcRasterRGB:		skip = 3; have_alpha = 0; have_z = 0; break;
	case DcRasterRGBA:	skip = 4; have_alpha = 1; have_z = 0; break;
	case DcRasterRGBZ:	skip = 7; have_alpha = 0; have_z = 3; break;
	case DcRasterRGBAZ:	skip = 8; have_alpha = 1; have_z = 4; break;
	case DcRasterABGR:
	    dynout_rstr->background_mode = ddd_vll_drom_rstr_user_data;
	    dynout_rstr->background_data = rstr->data;
	    return;
	default:
	    return;
	}
      
	if (dynout_rstr->background_data != 0 && 
	    (!(dynout_rstr->background_mode & ddd_vll_drom_rstr_user_data))) {
	    dor_space_deallocate(dynout_rstr->background_data);
	}

	if (have_z) {
	    dynout_rstr->background_mode |= ddd_vll_drom_rstr_with_z;
	    if ((dynout_rstr->background_data = (unsigned char *)
		 dor_space_allocate(rstr->width*rstr->height*8)) == NULL) {
		return;
	    }
	} else {
	    if ((dynout_rstr->background_data = (unsigned char *)
		 dor_space_allocate(rstr->width*rstr->height*4)) == NULL) {
		return;
	    }
	}
	d1 = (unsigned char *)rstr->data;
	d2 = (unsigned char *)dynout_rstr->background_data;

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
      
	dynout_rstr->background_sequence_number = rstr->sequence_number;
    }
}
/*
  ======================================================================
 */
