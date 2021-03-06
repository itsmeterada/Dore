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
#include "dore/dore_develop/private/torus.h"

/*
 ======================================================================
 Functions:

   void dpr_radiosity_torus_show_render(object)

 ======================================================================
 */

void dpr_radiosity_torus_show_render (
    dot_object *object)
{
    DtMethodPtr exec_rtn, query_rtn;
    DtSwitch radswi;
    DtFlag renderable, prim_disabled;
    struct torus *torus;

    renderable = DDcondex_QueryRender(DsInqClassId("DoTorus"));

    query_rtn = DDclass_InqMethod(DsInqClassId("AttRadiositySwitch"),
                        DsInqMethodId("InqGlobalValue"));
    (*query_rtn) (&radswi);

    if (radswi == DcOff) {
        if (!renderable)
           return;

        /* do rendering as usual */
        exec_rtn = DDobject_InqMethod(object, dpe_rad_base_method_id);
        (*exec_rtn)(object);

    } else {
        prim_disabled = DDcondex_QueryPrimExecFlag();
        if (!renderable && !prim_disabled)
            /* invisible or out of ExecSet: object has no add data */
            return;

        /* force the use of the standard alternate representation  */
        torus = (struct torus *) object->data;

        if (torus->alternate_object == DcNullPtr) {
            /* object was created in callback. No rad allowed */
            exec_rtn =
                DDobject_InqMethod(object, dpe_rad_base_method_id);
            (*exec_rtn)(object);

        } else {

        exec_rtn = DDobject_InqMethod(torus->alternate_object,
                           dpe_rad_show_data.show_render_method_id);
        (*exec_rtn)(torus->alternate_object);
        }
    }
}
