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

   void dpr_radiosity_camera_find_dsptyp(object)

 ======================================================================
 */


void dpr_radiosity_camera_find_dsptyp (
    dot_object *object)
{
    DtMethodPtr query_rtn;
    DtInt actual_size;
    char errstring[128];

    if (dpe_rad_show_data.active_camera == DcNullObject ||
        dpe_rad_show_data.active_camera == object) {

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
    }
}
