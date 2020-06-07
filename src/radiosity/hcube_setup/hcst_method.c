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
 
#include "method.h"

/*
 ======================================================================
 Functions:

   dpr_radiosity_hcube_setup_method_initialize
   dpr_radiosity_hcube_setup_new_class
   dpr_radiosity_hcube_setup_att_initialize
   dpr_radiosity_hcube_setup_execute

 ======================================================================
 */

dpt_rad_method_rtn_list dpe_rad_hcube_setup_rtn[] = {

/*
 * We overwrite only the global attribute routine for those classes
 * that I need to query in this method. For the other ones, it is more
 * efficient to overwrite the attribute method routine. 
 *
 * For all geometry classes that radiosity does not currently "support",
 * and that do have an alternate representation that radiosity supports,
 * we need to install a null method routine here to avoid the alt. rep.
 * to be inserted in the hemi-cube group (since the object is going to be
 * rendered withoiut radiosity in the end)
 */

    { "AttRadiositySwitch", (DtMethodPtr) dpr_radiosity_radswi_hcube_setup },
    { "AttEmissionSwitch",  (DtMethodPtr) dpr_radiosity_emiswi_hcube_setup },
    { "AttEmissionFactor",  (DtMethodPtr) dpr_radiosity_emifct_hcube_setup },
    { "AttEmissionColor",   (DtMethodPtr) dpr_radiosity_emiclr_hcube_setup },
    { "AttDiffuseColor",    (DtMethodPtr) dpr_radiosity_difclr_hcube_setup },
    { "AttRepType",         (DtMethodPtr) dpr_radiosity_reptyp_hcube_setup },
    { "AttLcstowcsmat",     (DtMethodPtr) dpr_radiosity_lcswcs_hcube_setup },

    { "DoRotate",           (DtMethodPtr) dpr_radiosity_geoatt_hcube_setup },
    { "DoTranslate",        (DtMethodPtr) dpr_radiosity_geoatt_hcube_setup },
    { "DoShear",            (DtMethodPtr) dpr_radiosity_geoatt_hcube_setup },
    { "DoScale",            (DtMethodPtr) dpr_radiosity_geoatt_hcube_setup },
    { "DoTransformMatrix",  (DtMethodPtr) dpr_radiosity_geoatt_hcube_setup },
    { "DoLookAtFrom",       (DtMethodPtr) dpr_radiosity_geoatt_hcube_setup },
    { "DoPushMatrix",       (DtMethodPtr) dpr_radiosity_geoatt_hcube_setup },
    { "DoPopMatrix",        (DtMethodPtr) dpr_radiosity_geoatt_hcube_setup },
    { "DoPushAtts",         (DtMethodPtr) dpr_radiosity_geoatt_hcube_setup },
    { "DoPopAtts",          (DtMethodPtr) dpr_radiosity_geoatt_hcube_setup },

    { "DoTriangleMesh",     (DtMethodPtr) dpr_radiosity_trimsh_hcube_setup },
    { "DoTriangleList",     (DtMethodPtr) dpr_radiosity_trilst_hcube_setup },
    { "DoTriangleStrip",    (DtMethodPtr) dpr_radiosity_tristrip_hcube_setup },
    { "DoSimplePolygon",    (DtMethodPtr) dpr_radiosity_simply_hcube_setup },
    { "DoSimplePolygonMesh",(DtMethodPtr) dpr_radiosity_simplymsh_hcube_setup},
    { "DoVarTriangleMesh",  (DtMethodPtr) dpr_radiosity_vartrimsh_hcube_setup},
    { "DoVarTriangleStrip", (DtMethodPtr)dpr_radiosity_vartristrip_hcube_setup},
    { "DoVarSimplePolygonMesh" ,
			   (DtMethodPtr)dpr_radiosity_varsimplymsh_hcube_setup},

    { "DoPolygon",     (DtMethodPtr) dpr_radiosity_ply_hcube_setup },
    { "DoPolygonMesh", (DtMethodPtr) dpr_radiosity_plymsh_hcube_setup },
    { "DoPrimSurf",    (DtMethodPtr) dpr_radiosity_prisrf_hcube_setup },
    { "DoTorus",       (DtMethodPtr) dpr_radiosity_torus_hcube_setup },
    { "DoQuadGrid",    (DtMethodPtr) dpr_radiosity_quadgrid_hcube_setup },
    { "DoQuadList",    (DtMethodPtr) dpr_radiosity_quadlst_hcube_setup },
    { "DoQuadMesh",    (DtMethodPtr) dpr_radiosity_quadmsh_hcube_setup },
    { "DoSphereList",  (DtMethodPtr) dpr_radiosity_sphlst_hcube_setup },
    { "DoVarQuadGrid", (DtMethodPtr) dpr_radiosity_varquadgrid_hcube_setup },
    { "DoVarQuadMesh", (DtMethodPtr) dpr_radiosity_varquadmsh_hcube_setup },

    { "DoAnnoText",    (DtMethodPtr) DcNullPtr },
    { "DoNURBSurf",    (DtMethodPtr) DcNullPtr },
    { "DoPatch",       (DtMethodPtr) DcNullPtr },
    { "DoPolymarker",  (DtMethodPtr) DcNullPtr },
    { "DoText",        (DtMethodPtr) DcNullPtr },

    { 0, 0 }
};


dpt_rad_hcube_setup_att    dpe_rad_hcube_setup_att;

/*
 ======================================================================
 */

DtInt dpr_radiosity_hcube_setup_method_initialize (void)
{
    DtInt i;
    DtInt class_id, method_id;

    /* create methods to traverse original display group and build and
       initialize a group with only the radiosity objects and attributes
       for hemicube drawing */

    method_id = DDclass_CopyMethod(DsInqMethodId("StdRenderDisplay"),
                "HcubeSetup");

    for (i=0; dpe_rad_hcube_setup_rtn[i].name != 0; i++) {
        if ((class_id = DsInqClassId(dpe_rad_hcube_setup_rtn[i].name))
            != NOT_INSTALLED) {
                DDclass_SetMethod(class_id, method_id,
                    dpe_rad_hcube_setup_rtn[i].routine);
        }
    }

    dpr_radiosity_hcube_setup_att_initialize();

    return(method_id);
}

/*
 ======================================================================
 */

void dpr_radiosity_hcube_setup_new_class (
    DtPtr class_name,
    DtInt class_id)
{
    DtInt i;

    for (i=0; dpe_rad_hcube_setup_rtn[i].name != 0; i++) {
        if (!strcmp(dpe_rad_hcube_setup_rtn[i].name, class_name)) {
            DDclass_SetMethod(class_id,
                    dpe_rad_hcube_data.hcube_setup_method_id,
                    dpe_rad_hcube_setup_rtn[i].routine);
            break;
        }
    }
}

/*
 ======================================================================
 */

void dpr_radiosity_hcube_setup_att_initialize (void)
{
    DtMethodPtr query_rtn;
    DtColorModel colormodel;

    /* initialize every attribute used by this method */

    query_rtn = DDclass_InqMethod(DsInqClassId("AttRadiositySwitch"),
                DsInqMethodId("InqGlobalValue"));
    (*query_rtn) (&dpe_rad_hcube_setup_att.radswi);

    query_rtn = DDclass_InqMethod(DsInqClassId("AttRepType"),
                DsInqMethodId("InqGlobalValue"));
    (*query_rtn) (&dpe_rad_hcube_setup_att.reptyp);

    query_rtn = DDclass_InqMethod(DsInqClassId("AttDiffuseColor"),
                DsInqMethodId("InqGlobalValue"));
    (*query_rtn) (&colormodel, dpe_rad_hcube_setup_att.difclr);

    query_rtn = DDclass_InqMethod(DsInqClassId("AttEmissionColor"),
                DsInqMethodId("InqGlobalValue"));
    (*query_rtn) (&colormodel, dpe_rad_hcube_setup_att.emiclr);

    query_rtn = DDclass_InqMethod(DsInqClassId("AttEmissionFactor"),
                DsInqMethodId("InqGlobalValue"));
    (*query_rtn) (&dpe_rad_hcube_setup_att.emifct);

    query_rtn = DDclass_InqMethod(DsInqClassId("AttEmissionSwitch"),
                DsInqMethodId("InqGlobalValue"));
    (*query_rtn) (&dpe_rad_hcube_setup_att.emiswi);

    query_rtn = DDclass_InqMethod(DsInqClassId("AttLcstowcsmat"),
                DsInqMethodId("InqGlobalValue"));
    (*query_rtn) (dpe_rad_hcube_setup_att.lcswcs);


}

/* ====================================================================== */

/* The following will execute the hcube_setup method and create a
   new display group containing a subset of the initial display
   group containing only the primitves for which the radiosity
   switch is off + all geometric attributes. The additional data
   of each of the primitives is initialized and info about
   the total energy of the system is also computed */


void dpr_radiosity_hcube_setup_execute (void)
{
    DtMethodPtr executemethod;
    DtObject orig_display_group;

    orig_display_group = DvInqDisplayGroup(dpe_rad_show_data.view);

    /* the hemicube rendereing will have to be done with constant
       shading and surfaces. Force it. */

    dpe_rad_hcube_data.display_group = DoGroup(DcTrue);
        DgAddObj(DoRepType(DcSurface));
        DgAddObj(DoSurfaceShade(DcShaderConstant));
    DgClose();

    /* First patch will get color 1, then 2, 3, etc... Each patch
       must have a different color so that when we read the pixels
       back later we know which patch was projected onto the pixel.
       Color 0 is reserved for the background.  */

    dpe_rad_hcube_data.next_integer_color = 1;

    /* niitialization for ambient component */
    dpe_rad_ambient.sum_area = 0.0;
    dpe_rad_ambient.sum_reflecXarea[0] = 0.0;
    dpe_rad_ambient.sum_reflecXarea[1] = 0.0;
    dpe_rad_ambient.sum_reflecXarea[2] = 0.0;

    doe_system_current_method = dpe_rad_hcube_data.hcube_setup_method_id;

    executemethod = dor_object_get_current_method(orig_display_group);

    (*executemethod)(orig_display_group);

    doe_system_current_method = DcMethodNull;
}
