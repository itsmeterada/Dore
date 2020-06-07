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
	ddt_g4vll_drom_sphlst * ddr_g4vll_drom_sphlst_create (object)
	int ddr_g4vll_drom_sphlst_print (object, data)
	int ddr_g4vll_drom_sphlst_delete (object, data)
	int ddr_g4vll_drom_sphlst_render (object)
	void ddr_g4vll_drom_sphlst_render_firmware (sphlst, drom_sphlst)
	void ddr_g4vll_drom_sphlst_render_software (object)

  ======================================================================
 */
#include "drom.h"
#include "dore/internal/system.h"

/* ====================================================================== */

static DtInt dataindex = -1;
static DtInt classid = -1;
static DtMethodPtr altobj_query    = DcNullPtr;
static DtMethodPtr gentexuv_query  = DcNullPtr;
static DtMethodPtr gentexuvw_query = DcNullPtr;

/* ======================================================================= */

ddt_g4vll_drom_sphlst *ddr_g4vll_drom_sphlst_create (
    DtObjectStructure *object)
{
    ddt_g4vll_drom_sphlst *drom_sphlst;

    drom_sphlst =  (ddt_g4vll_drom_sphlst *)
	    DDspace_Allocate(sizeof(ddt_g4vll_drom_sphlst));

    drom_sphlst->radius = (DtReal *)0;

    return drom_sphlst;
}

/* ====================================================================== */

void ddr_g4vll_drom_sphlst_print (
    DtObjectStructure *object,
    ddt_g4vll_drom_sphlst *data)
{
}

/* ====================================================================== */

void ddr_g4vll_drom_sphlst_delete (
    DtObjectStructure *object,
    ddt_g4vll_drom_sphlst *data)
{
    if (data->radius != (DtReal *)0) {
	DDspace_Deallocate(data->radius);
    }
    DDspace_Deallocate(data);
}

/* ====================================================================== */

void ddr_g4vll_drom_sphlst_render (
    DtObjectStructure *object)
{
    struct sphlst *sphlst;
    ddt_g4vll_drom_sphlst *drom_sphlst;
    DtInt i;
    DtSwitch uvswitchvalue, uvwswitchvalue;
    auto DtFlag texture_mapping = DcFalse;

    if (classid == -1) 
	    classid = DsInqClassId("DoSphereList");

    if (!DDcondex_QueryRender(classid))
	    return;

    if (dataindex == -1) {
	for (i=0; dde_g4vll_drom_class_data[i].name != 0; i++) {
	    if (dde_g4vll_drom_class_data[i].id == classid) {
		dataindex = dde_g4vll_drom_class_data[i].index;
		break;
	    }
	}
    }

    if (!gentexuv_query)
	gentexuv_query =  DDclass_InqMethod 
		(DsInqClassId("AttCompTextureUVSwitch"), DcMethodInqGlbAttVal);
    (*gentexuv_query)(&uvswitchvalue);

    if (!gentexuvw_query)
	gentexuvw_query =  DDclass_InqMethod 
		(DsInqClassId("AttCompTextureUVWSwitch"), DcMethodInqGlbAttVal);
    (*gentexuvw_query)(&uvwswitchvalue);

    sphlst = (struct sphlst *)(object->data);
    drom_sphlst = (ddt_g4vll_drom_sphlst *)object->additional_data[dataindex];

    /* 
     * Use the 29050 spheres only if the user has requested them specifically.
     * The alternate representation will be used instead of the firmware if 
     * any of the following conditions exist:
     *   -the spheres do not have a surface representation type
     * 	 -the spheres are texture mapped (texture mapping wins over transparency
     *    when both are present)
     * Note: only up to three _directional_ light sources will be applied by VLL.
     */
	
    if (dde_g4vll_drom_curdevdat->use_spheres &&
	(dde_g4vll_drom_data.reptype == DcSurface)) {

	if (dde_g4vll_drom_data.map_diffuse_color_switch &&
		((uvswitchvalue && dde_g4vll_drom_data.texture_2D_valid) ||
		(uvwswitchvalue && dde_g4vll_drom_data.texture_3D_valid))) {
	    texture_mapping = DcTrue;
	}

	if (dde_g4vll_drom_data.traversal_type == ddd_g4vll_drom_standard_pass) {
	    if (texture_mapping) {
		dde_g4vll_drom_data.have_textures = DcTrue;
		return;
	    }
	    if (dde_g4vll_drom_data.transparent_switch) {
		dde_g4vll_drom_data.have_transparency = DcTrue;
		return;
	    }
	    ddr_g4vll_drom_sphlst_render_firmware (sphlst, drom_sphlst);
	} else 
	if (dde_g4vll_drom_data.traversal_type == ddd_g4vll_drom_texture_pass) {
	    if (texture_mapping) {
		ddr_g4vll_drom_sphlst_render_software (object);
	    }
	} else 
	if (dde_g4vll_drom_data.traversal_type == ddd_g4vll_drom_transp_pass) {
	    if (dde_g4vll_drom_data.transparent_switch && !texture_mapping) {
		ddr_g4vll_drom_sphlst_render_firmware(sphlst, drom_sphlst);
	    }
	} else
	if (dde_g4vll_drom_data.traversal_type ==
		ddd_g4vll_drom_transp_texture_pass) {
	    if ((dde_g4vll_drom_data.transparent_switch &&
		    (ddr_g4vll_dcm_inq_transp_mode() ==
		    ddd_g4vll_transp_multi_pass)) && texture_mapping) {
		ddr_g4vll_drom_sphlst_render_software (object);
	    }
	}
    } else {
	ddr_g4vll_drom_sphlst_render_software (object);
    }
}

/* ====================================================================== */

void ddr_g4vll_drom_sphlst_render_firmware (
    struct sphlst *sphlst,
    ddt_g4vll_drom_sphlst *drom_sphlst)
{
    DtInt i;

    if (sphlst->radii == (DtReal *)0) {
	if (drom_sphlst->radius == (DtReal *)0) {
	    drom_sphlst->radius = (DtReal *)DDspace_Allocate 
			(sphlst->sphere_count*sizeof(DtReal));
	    for (i=0; i<sphlst->sphere_count; i++) {
		drom_sphlst->radius[i] = 1.0;
	    }
	}
	VLLset_clip_status(0);
	VLLsphere_list(sphlst->sphere_count,
			   (void *)sphlst->locations, 
			   (void *)drom_sphlst->radius,
			   (void *)sphlst->colors, (void *)0);
    } else {
	VLLset_clip_status(0);
	VLLsphere_list(sphlst->sphere_count,
			   (void *)sphlst->locations, 
			   (void *)sphlst->radii,
			   (void *)sphlst->colors, (void *)0);
    }

    if (doe_system_timing_display_traverse_count == 1)
        doe_system_timing_firmware_spheres_drawn += sphlst->sphere_count;
}

/* ====================================================================== */

void ddr_g4vll_drom_sphlst_render_software (
    DtObjectStructure *object)
{
    DtObject alt_obj;

    if (!altobj_query) {
	altobj_query = DDobject_InqMethod(object, DcMethodUpdStdAltObj);
    }

    if (altobj_query) {
	alt_obj = (DtObject)(*altobj_query)(object);

	if (alt_obj != (DtObject)0) {
	    DDobject_InqCurrentMethod(alt_obj) (alt_obj);
	} else {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
			   "ddr_g4vll_drom_sphlst_render", "");
	    return;
	}
    }
}
