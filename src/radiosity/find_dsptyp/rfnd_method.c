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
   DtInt dpr_radiosity_find_dsptyp_method_initialize()
   void dpr_radiosity_find_dsptyp_new_class(class_name, class_id)
   void dpr_radiosity_find_intermediate_disp_type()

 ======================================================================
 */

dpt_rad_method_rtn_list dpe_rad_find_dsptyp_rtn[] = {
    { "DoCamera", (DtMethodPtr) dpr_radiosity_camera_find_dsptyp },
    { 0, 0 }
};

/*
 ======================================================================
 */

DtInt dpr_radiosity_find_dsptyp_method_initialize (void)
{
    DtInt i;
    DtInt class_id, method_id;

    /* create method to get the value of the DoIntermediateDispType
       attribute for the active camera */

    method_id = DDclass_CopyMethod(DsInqMethodId("StdRenderStudio"),
                "FindIntermDispType");

    for (i=0; dpe_rad_find_dsptyp_rtn[i].name != 0; i++) {
        if ((class_id = DsInqClassId(dpe_rad_find_dsptyp_rtn[i].name))
            != NOT_INSTALLED) {
                DDclass_SetMethod(class_id, method_id,
                    dpe_rad_find_dsptyp_rtn[i].routine);
        }
    }

    return(method_id);

}
/*
 ======================================================================
 */

void dpr_radiosity_find_dsptyp_new_class (
    DtPtr class_name,
    DtInt class_id)
{
    DtInt i;

    for (i=0; dpe_rad_find_dsptyp_rtn[i].name != 0; i++) {
        if (!strcmp(dpe_rad_find_dsptyp_rtn[i].name, class_name)) {
            DDclass_SetMethod(class_id,
                    dpe_rad_show_data.find_dsptyp_method_id,
                    dpe_rad_find_dsptyp_rtn[i].routine);
            break;
        }
    }

}

/*
 ======================================================================
 */

void dpr_radiosity_find_intermediate_disp_type (void)
{
    DtMethodPtr query_rtn;
    DtMethodPtr executemethod;
    DtObject orig_studio_group;
    DtInt actual_size;
    char errstring[128];

    orig_studio_group = DvInqDefinitionGroup(dpe_rad_show_data.view);

    dpe_rad_show_data.active_camera =
                  DvInqActiveCamera(dpe_rad_show_data.view);

    /* get default value of global attribute */

    query_rtn = DDclass_InqMethod
                (DsInqClassId("AttIntermediateDispType"),
                 DsInqMethodId("InqGlobalValue"));

    (*query_rtn) (&dpe_rad_show_data.interm_dsptyp,
                  dpe_rad_show_data.dsptyp_parms,
                  MAX_PARAM_SIZE, &actual_size);

    if (actual_size > MAX_PARAM_SIZE)  {
            sprintf(errstring,
              "Can't have more than %d params for DoIntermediateDispType",
              MAX_PARAM_SIZE);
            DDerror(ERR_VALUE_OUT_OF_RANGE,
              "dpr_radiosity_camera_find_dsptyp", errstring);
    }

    doe_system_current_method = dpe_rad_show_data.find_dsptyp_method_id;

    /* traversa original studio group */
    executemethod = dor_object_get_current_method(orig_studio_group);

    (*executemethod)(orig_studio_group);

    /* after this travesal, teh global variables
       dpe_rad_show_data.interm_dsptyp and dpe_rad_show_data.dsptyp_parms 
       hold the disp type of the active camera */

    doe_system_current_method = DcMethodNull;

}
