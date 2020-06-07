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

   dpr_radiosity_hcube_render_method_initialize
   dpr_radiosity_hcube_render_new_class
   dpr_radiosity_generic_hcube_render_rtn
   
 ======================================================================
 */

dpt_rad_method_rtn_list dpe_rad_hcube_render_rtn[] = {
    { "DoTriangleMesh",
		(DtMethodPtr) dpr_radiosity_trimsh_hcube_render },
    { "DoTriangleList",
		(DtMethodPtr) dpr_radiosity_trilst_hcube_render },
    { "DoTriangleList",
		(DtMethodPtr) dpr_radiosity_tristrip_hcube_render },
    { "DoSimplePolygon",
		(DtMethodPtr) dpr_radiosity_simply_hcube_render },
    { "DoSimplePolygonMesh",
		(DtMethodPtr) dpr_radiosity_simplymsh_hcube_render },
    { "DoVarTriangleMesh",
		(DtMethodPtr) dpr_radiosity_vartrimsh_hcube_render },
    { "DoVarTriangleStrip",
		(DtMethodPtr) dpr_radiosity_vartristrip_hcube_render },
    { "DoVarSimplePolygonMesh",
		(DtMethodPtr) dpr_radiosity_varsimplymsh_hcube_render },
    { "DoGroup",
		(DtMethodPtr) dpr_radiosity_group_execute_current_hcube_render},
    { "DoInLineGroup",
		(DtMethodPtr) dpr_radiosity_group_execute_current_hcube_render},
    { 0, 0 }
};

/*
 ======================================================================
 */

DtInt dpr_radiosity_hcube_render_method_initialize (void)
{
    DtInt class_id, method_id;
    DtInt i;

    /* create method to traverse hemicube display group and
      render from point of view of hemicube in offscreen device  */

    method_id = DDclass_AddInitializedMethod("HcubeRender",
                    (DtMethodPtr) dpr_radiosity_generic_hcube_render_rtn);

    for (i=0; dpe_rad_hcube_render_rtn[i].name != 0; i++) {
        if ((class_id = DsInqClassId(dpe_rad_hcube_render_rtn[i].name))
            != NOT_INSTALLED) {
                DDclass_SetMethod(class_id, method_id,
                    dpe_rad_hcube_render_rtn[i].routine);
        }
    }

    return(method_id);

}

/*
 ======================================================================
 */

void dpr_radiosity_hcube_render_new_class (
    char  *class_name,
    DtInt  class_id)
{
    DtFlag set_rtn;
    DtInt i;

    set_rtn = DcFalse;
    for (i=0; dpe_rad_hcube_render_rtn[i].name != 0; i++) {
        if (!strcmp(dpe_rad_hcube_render_rtn[i].name, class_name)) {
            DDclass_SetMethod(class_id,
                    dpe_rad_hcube_data.hcube_render_method_id,
                    dpe_rad_hcube_render_rtn[i].routine);
            set_rtn = DcTrue;
            break;
        }
    }

    /* if this new class does not need a special routine, use
       generic display routine */

    if (!set_rtn) {
        DDclass_SetMethod(class_id,
                    dpe_rad_hcube_data.hcube_render_method_id,
                    (DtMethodPtr) dpr_radiosity_generic_hcube_render_rtn);
    }

}
/*
 ======================================================================
 */

void dpr_radiosity_generic_hcube_render_rtn (
    dot_object *object)
{
    DtMethodPtr routine;

    routine = DDobject_InqMethod(object, dpe_rad_base_method_id);
    (*routine)(object);
}
