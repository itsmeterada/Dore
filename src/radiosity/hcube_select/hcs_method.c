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
   dpr_radiosity_hcube_select_method_initialize
   dpr_radiosity_hcube_select_att_initialize
   dpr_radiosity_hcube_select_execute
   dpr_radiosity_hcube_select_compute_ambient
   dpr_radiosity_hcube_select_test_convergence

 ======================================================================
 */

dpt_rad_method_rtn_list    dpe_rad_hcube_select_rtn[] = {
    { "AttLcstowcsmat",   (DtMethodPtr) dpr_radiosity_lcswcs_hcube_select },
    { "DoTriangleMesh",   (DtMethodPtr) dpr_radiosity_trimsh_hcube_select },
    { "DoTriangleStrip",  (DtMethodPtr) dpr_radiosity_tristrip_hcube_select },
    { "DoTriangleList",   (DtMethodPtr) dpr_radiosity_trilst_hcube_select },
    { "DoSimplePolygon",  (DtMethodPtr) dpr_radiosity_simply_hcube_select },
    { "DoSimplePolygonMesh",
			(DtMethodPtr) dpr_radiosity_simplymsh_hcube_select },
    { "DoVarTriangleMesh",(DtMethodPtr) dpr_radiosity_vartrimsh_hcube_select },
    { "DoVarTriangleStrip",(DtMethodPtr)dpr_radiosity_vartristrip_hcube_select},
    { "DoVarSimplePolygonMesh",
			(DtMethodPtr) dpr_radiosity_varsimplymsh_hcube_select },
    { 0, 0 }
};

dpt_rad_hcube_select_att   dpe_rad_hcube_select_att;

/*
 ======================================================================
 */

DtInt dpr_radiosity_hcube_select_method_initialize (void)
{
    DtInt i;
    DtInt class_id, method_id;

    /* create method to select the brightest patch */

    method_id = DDclass_CopyMethod(DsInqMethodId("StdRenderDisplay"),
                "HcubeSelectBrightestPatch");

    for (i=0; dpe_rad_hcube_select_rtn[i].name != 0; i++) {
        if ((class_id = DsInqClassId(dpe_rad_hcube_select_rtn[i].name))
            != NOT_INSTALLED) {
                DDclass_SetMethod(class_id, method_id,
                    dpe_rad_hcube_select_rtn[i].routine);
        }
    }

    dpr_radiosity_hcube_select_att_initialize();

    return(method_id);

}
/*
 ======================================================================
 */

void dpr_radiosity_hcube_select_new_class (
    DtPtr class_name,
    DtInt class_id)
{
    DtInt i;

    for (i=0; dpe_rad_hcube_select_rtn[i].name != 0; i++) {
        if (!strcmp(dpe_rad_hcube_select_rtn[i].name, class_name)) {
            DDclass_SetMethod(class_id,
                    dpe_rad_hcube_data.hcube_select_method_id,
                    dpe_rad_hcube_select_rtn[i].routine);
            break;
        }
    }

}

/*
 ======================================================================
 */

void dpr_radiosity_hcube_select_att_initialize (void)
{
    DtMethodPtr query_rtn;

    /* initialize every attribute used by this method */

    query_rtn = DDclass_InqMethod(DsInqClassId("AttLcstowcsmat"),
                DsInqMethodId("InqGlobalValue"));
    (*query_rtn) (dpe_rad_hcube_select_att.lcswcs);
}

/*
 ======================================================================
 */

void dpr_radiosity_hcube_select_execute (void)
{
    DtMethodPtr executemethod;

    /* reset energy to find next brightest patch */
    dpe_rad_brightest_patch.energy = -1.0;
    dpe_rad_brightest_patch.nVert = 0;
    dpe_rad_brightest_patch.vertices = DcNullPtr;

    dpe_rad_energy.total_unshot_per_channel[0] = 0.0;
    dpe_rad_energy.total_unshot_per_channel[1] = 0.0;
    dpe_rad_energy.total_unshot_per_channel[2] = 0.0;
    dpe_rad_energy.total_unshot = 0.0;

    doe_system_current_method = dpe_rad_hcube_data.hcube_select_method_id;

    executemethod =
        dor_object_get_current_method(dpe_rad_hcube_data.display_group);

    (*executemethod)(dpe_rad_hcube_data.display_group);

    doe_system_current_method = DcMethodNull;

    /* if dpe_rad_brightest_patch.energy is still -1, then there are
       no objects in the radiosity group  */

}

/*
 ======================================================================
 */

void dpr_radiosity_hcube_select_compute_ambient (void)
{
    /* estimate ambient component */
    /* using technique discussed in Chen's article, Siggraph 88 */

    /* avoid division by zero */
    if (dpe_rad_ambient.sum_area != dpe_rad_ambient.sum_reflecXarea[0])
        dpe_rad_ambient.component[0] =
            dpe_rad_energy.total_unshot_per_channel[0] /
            (dpe_rad_ambient.sum_area - dpe_rad_ambient.sum_reflecXarea[0]);
    else if (dpe_rad_energy.total_unshot_per_channel[0] == 0.0)
        dpe_rad_ambient.component[0] = 0.0;
    else
        dpe_rad_ambient.component[0] = 1.0;

    if (dpe_rad_ambient.sum_area != dpe_rad_ambient.sum_reflecXarea[1])
        dpe_rad_ambient.component[1] =
            dpe_rad_energy.total_unshot_per_channel[1] /
            (dpe_rad_ambient.sum_area - dpe_rad_ambient.sum_reflecXarea[1]);
    else if (dpe_rad_energy.total_unshot_per_channel[1] == 0.0)
        dpe_rad_ambient.component[1] = 0.0;
    else
        dpe_rad_ambient.component[1] = 1.0;

    if (dpe_rad_ambient.sum_area != dpe_rad_ambient.sum_reflecXarea[2])
        dpe_rad_ambient.component[2] =
            dpe_rad_energy.total_unshot_per_channel[2] /
            (dpe_rad_ambient.sum_area - dpe_rad_ambient.sum_reflecXarea[2]);
    else if (dpe_rad_energy.total_unshot_per_channel[2] == 0.0)
        dpe_rad_ambient.component[2] = 0.0;
    else
        dpe_rad_ambient.component[2] = 1.0;

    if (dpe_rad_ambient.component[0] > 1.0)
        dpe_rad_ambient.component[0] = 1.0;
    if (dpe_rad_ambient.component[1] > 1.0)
        dpe_rad_ambient.component[1] = 1.0;
    if (dpe_rad_ambient.component[2] > 1.0)
        dpe_rad_ambient.component[2] = 1.0;

}


/*
 ======================================================================
 */

/* returns true if convergence was met */

DtFlag dpr_radiosity_hcube_select_test_convergence (void)
{
    /* if there is practically no energy remaining unshot, terminate */
    if (dpe_rad_energy.total_unshot < ENERGY_EPSILON)
        return(DcTrue);

    switch(dpe_rad_hcube_data.conv_type) {
        case DcConvFixedIterations:
            if (dpe_rad_iteration_number >=
                 dpe_rad_hcube_data.conv_parms[0])
                   return(DcTrue);
            break;
        case DcConvGlobalRel:
            if (dpe_rad_energy.total_unshot / dpe_rad_energy.total <=
                dpe_rad_hcube_data.conv_parms[0])
                   return(DcTrue);
            break;
        case DcConvGlobalAbs:
            if (dpe_rad_energy.total_unshot <=
                dpe_rad_hcube_data.conv_parms[0])
                   return(DcTrue);
            break;
        case DcConvPatchRel:
            if (dpe_rad_brightest_patch.energy / dpe_rad_energy.total <=
                dpe_rad_hcube_data.conv_parms[0])
                   return(DcTrue);
            break;
        case DcConvPatchAbs:
            if (dpe_rad_brightest_patch.energy <=
                dpe_rad_hcube_data.conv_parms[0])
                   return(DcTrue);
            break;
    }

#ifdef DEBUG
       printf("Testing convergence:\n");
       printf("Total energy is %g\n", dpe_rad_energy.total);
       printf("Total unshot energy is %g\n", dpe_rad_energy.total_unshot);
       printf("Energy of brightest patch %g\n\n", dpe_rad_brightest_patch.energy);
#endif

    return(DcFalse);
}

/*
 ======================================================================
 */
