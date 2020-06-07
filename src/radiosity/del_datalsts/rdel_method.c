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

   dpr_radiosity_del_datalsts_method_initialize
   dpr_radiosity_del_datalsts_new_class
   dpr_radiosity_del_datalsts_execute

 ======================================================================
 */


dpt_rad_method_rtn_list dpe_rad_del_datalsts_rtn[] = {

    { "DoTriangleMesh",
		    (DtMethodPtr) dpr_radiosity_del_datalsts },
    { "DoTriangleList",
		    (DtMethodPtr) dpr_radiosity_del_datalsts },
    { "DoTriangleStrip",
		    (DtMethodPtr) dpr_radiosity_del_datalsts },
    { "DoVarTriangleMesh",
		    (DtMethodPtr) dpr_radiosity_del_datalsts },
    { "DoVarTriangleStrip",
		    (DtMethodPtr) dpr_radiosity_del_datalsts },
    { "DoSimplePolygon",
		    (DtMethodPtr) dpr_radiosity_del_simply_datalsts },
    { "DoSimplePolygonMesh",
		    (DtMethodPtr) dpr_radiosity_del_simplymsh_datalsts },
    { "DoVarSimplePolygonMesh",
		    (DtMethodPtr) dpr_radiosity_del_varsimplymsh_datalsts },
    { 0, 0 }
};


/*
 ======================================================================
 */

DtInt dpr_radiosity_del_datalsts_method_initialize (void)
{
    DtInt i;
    DtInt class_id, method_id;

    /* create method to traverse display group and delete
       multiple additional data sets for geometry, if any */

    method_id = DDclass_CopyMethod(DsInqMethodId("StdRenderDisplay"),
                  "Delete_DataLists");

    for (i=0; dpe_rad_del_datalsts_rtn[i].name != 0; i++) {
        if ((class_id = DsInqClassId(dpe_rad_del_datalsts_rtn[i].name))
            != NOT_INSTALLED) {
                DDclass_SetMethod(class_id, method_id,
                    dpe_rad_del_datalsts_rtn[i].routine);

        }
    }

    return(method_id);
}

/*
 ======================================================================
 */

void dpr_radiosity_del_datalsts_new_class (
    DtPtr class_name,
    DtInt class_id)
{
    DtInt i;

    for (i=0; dpe_rad_del_datalsts_rtn[i].name != 0; i++) {
        if (!strcmp(dpe_rad_del_datalsts_rtn[i].name, class_name)) {
            DDclass_SetMethod(class_id,
                    dpe_rad_hcube_data.del_datalsts_method_id,
                    dpe_rad_del_datalsts_rtn[i].routine);
            break;
        }
    }
}

/*
 ======================================================================
 */

void dpr_radiosity_del_datalsts_execute (void)
{
    DtMethodPtr executemethod;
    DtObject orig_display_group;

    /* traverse database and delete multiple sets of add. data */
    orig_display_group = DvInqDisplayGroup(dpe_rad_show_data.view);

    doe_system_current_method = dpe_rad_hcube_data.del_datalsts_method_id;

    executemethod = dor_object_get_current_method(orig_display_group);

    (*executemethod)(orig_display_group);

    doe_system_current_method = DcMethodNull;
}
