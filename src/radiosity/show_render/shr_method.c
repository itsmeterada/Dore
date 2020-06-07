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

   DtInt dpr_radiosity_show_render_method_initialize
   void dpr_radiosity_show_render_new_class
   void dpr_radiosity_generic_show_render_rtn
   void dpr_radiosity_set_current_method_base
   void dpr_radiosity_set_current_method_show_render

 ======================================================================
 */

dpt_rad_method_rtn_list    dpe_rad_show_render_rtn[] = {
    { "DoTriangleMesh",     (DtMethodPtr)dpr_radiosity_trimsh_show_render },
    { "DoTriangleList",     (DtMethodPtr)dpr_radiosity_trilst_show_render },
    { "DoTriangleStrip",    (DtMethodPtr)dpr_radiosity_tristrip_show_render },
    { "DoSimplePolygon",    (DtMethodPtr)dpr_radiosity_simply_show_render },
    { "DoSimplePolygonMesh",(DtMethodPtr)dpr_radiosity_simplymsh_show_render },
    { "DoVarTriangleMesh",  (DtMethodPtr)dpr_radiosity_vartrimsh_show_render },
    { "DoVarTriangleMesh",  (DtMethodPtr)dpr_radiosity_vartrimsh_show_render },
    { "DoVarTriangleStrip", (DtMethodPtr)dpr_radiosity_vartristrip_show_render},
    { "DoVarSimplePolygonMesh" ,
			  (DtMethodPtr) dpr_radiosity_varsimplymsh_show_render},
    { "DoPrimSurf",       (DtMethodPtr) dpr_radiosity_prisrf_show_render },
    { "DoTorus",          (DtMethodPtr) dpr_radiosity_torus_show_render },
    { "DoPolygon",        (DtMethodPtr) dpr_radiosity_ply_show_render },
    { "DoPolygonMesh",    (DtMethodPtr) dpr_radiosity_plymsh_show_render },
    { "DoQuadGrid",       (DtMethodPtr) dpr_radiosity_quadgrid_show_render },
    { "DoQuadList",       (DtMethodPtr) dpr_radiosity_quadlst_show_render },
    { "DoQuadMesh",       (DtMethodPtr) dpr_radiosity_quadmsh_show_render },
    { "DoSphereList",     (DtMethodPtr) dpr_radiosity_sphlst_show_render },
    { "DoVarQuadGrid",    (DtMethodPtr) dpr_radiosity_varquadgrid_show_render},
    { "DoVarQuadMesh",    (DtMethodPtr) dpr_radiosity_varquadmsh_show_render},

    { "DoCallback",       (DtMethodPtr) dpr_radiosity_callback_show_render },
    { "DoGroup", (DtMethodPtr) dpr_radiosity_group_execute_current_show_render},
    { "DoInLineGroup" ,
		 (DtMethodPtr) dpr_radiosity_group_execute_current_show_render},
    { 0, 0 }
};


/*
 ======================================================================
 */

DtInt dpr_radiosity_show_render_method_initialize (void)
{
    DtInt class_id, method_id;
    DtInt i;

   /* create a method to traverse the original display group and use
     the computed radiosities for the geometry if necessary */
  
    method_id = DDclass_AddInitializedMethod("ShowRender",
                (DtMethodPtr) dpr_radiosity_generic_show_render_rtn);

    for (i=0; dpe_rad_show_render_rtn[i].name != 0; i++) {
        if ((class_id = DsInqClassId(dpe_rad_show_render_rtn[i].name))
            != NOT_INSTALLED) {
                DDclass_SetMethod(class_id, method_id,
                    dpe_rad_show_render_rtn[i].routine);
        }
    }

    return(method_id);
}

/*
 ======================================================================
 */

void dpr_radiosity_show_render_new_class (
    DtPtr class_name,
    DtInt class_id)
{
    DtFlag set_rtn;
    DtInt i;

    set_rtn = DcFalse;
    for (i=0; dpe_rad_show_render_rtn[i].name != 0; i++) {
        if (!strcmp(dpe_rad_show_render_rtn[i].name, class_name)) {
            DDclass_SetMethod(class_id, 
                    dpe_rad_show_data.show_render_method_id,
                    dpe_rad_show_render_rtn[i].routine);
            set_rtn = DcTrue;
            break;
        }
    }

    /* if this new class does not need a special routine, use
       generic display routine */
    if (!set_rtn) {
        DDclass_SetMethod(class_id,
                    dpe_rad_show_data.show_render_method_id,
                    (DtMethodPtr) dpr_radiosity_generic_show_render_rtn);
    }

}

/*
 ======================================================================
 */
void dpr_radiosity_generic_show_render_rtn (
    dot_object *object)
{
    DtMethodPtr routine;

    /* set current method to base method to make sure attributes
       will call the right global attributes */
    dpr_radiosity_set_current_method_base();

    routine = DDobject_InqMethod(object, dpe_rad_base_method_id);
    (*routine)(object);

    /* reset current method in case we are in a callback */
    dpr_radiosity_set_current_method_show_render();
}

/*
 ======================================================================
 */

void dpr_radiosity_set_current_method_base (void)
{
   doe_system_current_method = dpe_rad_base_method_id;
}
/*
 ======================================================================
 */

void dpr_radiosity_set_current_method_show_render (void)
{
   doe_system_current_method = dpe_rad_show_data.show_render_method_id;
}

/*
 ======================================================================
 */
