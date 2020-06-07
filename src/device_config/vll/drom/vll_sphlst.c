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
	ddt_vll_drom_sphlst * ddr_vll_drom_sphlst_create (object)
	int ddr_vll_drom_sphlst_print (object, data)
	int ddr_vll_drom_sphlst_delete (object, data)
	int ddr_vll_drom_sphlst_render (object)

  ======================================================================
 */
#include "drom.h"
#include "dore/dore_develop/private/sphlst.h"

/*
 ======================================================================
 */

typedef struct {
    DtReal *radius;
} ddt_vll_drom_sphlst;

static DtInt dataindex = -1;
static DtInt classid = -1;
static DtPFI altobj_query = (DtPFI)0;
static DtPFI gentexuv_query = (DtPFI)0;
static DtPFI gentexuvw_query = (DtPFI)0;
/*
  ======================================================================-
 */
ddt_vll_drom_sphlst *
ddr_vll_drom_sphlst_create(object)
	DtObjectStructure *object;
{
    ddt_vll_drom_sphlst *dynout_sphlst;

    dynout_sphlst =  (ddt_vll_drom_sphlst *)
	    DDspace_Allocate(sizeof(ddt_vll_drom_sphlst));

    dynout_sphlst->radius = (DtReal *)0;

    return dynout_sphlst;
}
/*
  ======================================================================
 */
ddr_vll_drom_sphlst_print(object, data)
	DtObjectStructure *object;
	ddt_vll_drom_sphlst *data;
{
}
/*
  ======================================================================
 */
ddr_vll_drom_sphlst_delete(object, data)
	DtObjectStructure *object;
	ddt_vll_drom_sphlst *data;
{
    if (data->radius != (DtReal *)0) {
	DDspace_Deallocate(data->radius);
    }
    DDspace_Deallocate(data);
}
/*
  ======================================================================
 */
ddr_vll_drom_sphlst_render(object)
	DtObjectStructure *object;
{
    struct sphlst *sphlst;
    ddt_vll_drom_sphlst *dynout_sphlst;
    DtObject alt_obj;
    DtInt i;
    DtSwitch uvswitchvalue, uvwswitchvalue;


    if (classid == -1) 
	    classid = DsInqClassId("DoSphereList");

    if (!DDcondex_QueryRender(classid))
	    return;

    if (dataindex == -1) {
	for (i=0; dde_vll_drom_class_data[i].name != 0; i++) {
	    if (dde_vll_drom_class_data[i].id == classid) {
		dataindex = dde_vll_drom_class_data[i].index;
		break;
	    }
	}
    }

    if (gentexuv_query == (DtPFI)0)
	    gentexuv_query =  DDclass_InqMethod
                 (DsInqClassId("AttCompTextureUVSwitch"), DcMethodInqGlbAttVal);
    (*gentexuv_query)(&uvswitchvalue);

    if (gentexuvw_query == (DtPFI)0)
	    gentexuvw_query =  DDclass_InqMethod
                 (DsInqClassId("AttCompTextureUVWSwitch"), DcMethodInqGlbAttVal);
    (*gentexuvw_query)(&uvwswitchvalue);

    sphlst = (struct sphlst *)(object->data);
    dynout_sphlst = (ddt_vll_drom_sphlst *)object->additional_data[dataindex];

    /* 
     * The user has specified that this object should be able to be texture mapped
     * so use the tesselated spheres not the hardware sphere
     * NOTE: HW spheres will only be used if there are NO texture mapped objects
     * in the view
     * If user has requested transparent spheres we use tesselated spheres.
     */

    if (dde_vll_drom_curdevdat->use_spheres &&
	(dde_vll_drom_data.reptype == DcSurface) &&
	!(((uvswitchvalue && dde_vll_drom_data.texture_2D_valid) ||
	  (uvwswitchvalue && dde_vll_drom_data.texture_3D_valid)) &&
	  dde_vll_drom_data.map_diffuse_color) &&
	!dde_vll_drom_curviwdat->have_textures &&
	(dde_vll_drom_data.traversal_type !=  ddd_vll_drom_transp_pass)) {

	if (!dde_vll_drom_data.have_spheres &&
	    !dde_vll_drom_curviwdat->have_spheres) {
	    /*
	     * This is the first sphere seen and didn't think there were any spheres
	     * so abort the traversal and indicate that we have spheres
	     */
	    dde_vll_drom_data.have_spheres = DcTrue;
	    DsExecutionAbort();
	    return;
	}
	dde_vll_drom_data.have_spheres = DcTrue;

	if (sphlst->radii == (DtReal *)0) {
	    if (dynout_sphlst->radius == (DtReal *)0) {
		dynout_sphlst->radius = (DtReal *)DDspace_Allocate 
			(sphlst->sphere_count*sizeof(DtReal));
		for (i=0; i<sphlst->sphere_count; i++) {
		    dynout_sphlst->radius[i] = 1.0;
		}
	    }
	    VLLsphere_list(sphlst->sphere_count, (void *)0,
			   (void *)sphlst->locations, 
			   (void *)dynout_sphlst->radius,
			   (void *)sphlst->colors, (void *)0);
	} else {
	    VLLsphere_list(sphlst->sphere_count, (void *)0,
			   (void *)sphlst->locations, 
			   (void *)sphlst->radii,
			   (void *)sphlst->colors, (void *)0);
	}
    } else {
	if (altobj_query == (DtPFI)0) {
	    altobj_query = DDobject_InqMethod(object, DcMethodUpdStdAltObj);
	}

	if (altobj_query != (DtPFI)0) {
	    alt_obj = (DtObject)(*altobj_query)(object);

	    if (alt_obj != (DtObject)0) {
		DDobject_InqCurrentMethod(alt_obj) (alt_obj);
	    } else {
		DDerror(ERR_CANT_CREATE_ALTOBJ,
			   "ddr_vll_drom_sphlst_render", "");
		return;
	    }
	}
    }
}
/*
 ======================================================================
 */
