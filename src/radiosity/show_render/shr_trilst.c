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
#include "dore/dore_develop/private/trilst.h"

/*
 ======================================================================
 Functions:

   void dpr_radiosity_trilst_show_render(object)

 ======================================================================
 */

void dpr_radiosity_trilst_show_render (
    dot_object *object)
{
    DtObject current_srfshd;
    DtRealTriple ambient;
    DtInt i, elem;
    static DtInt index = -1;
    DtRealTriple *current_vertex_color;
    DtMethodPtr exec_rtn, obj_exec_rtn, query_rtn;
    DtSwitch radswi, radambswi;
    DtVertexType current_vertextype;
    DtFlag renderable, prim_disabled;
    struct trilst *trilst;
    dpt_rad_primitive_data *add_data, *head_add_data;

    renderable = DDcondex_QueryRender(DsInqClassId("DoTriangleList"));

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

        if (index == -1)
            index = dpr_radiosity_get_additional_data_index("DoTriangleList");

        head_add_data = object->additional_data[index];
  
        if (head_add_data->usageCount == 0) {
            if (!renderable)
               return; 
             /* no radiosity available for this object (either created
                from a callback or radswi was off during last compute) */
             exec_rtn = DDobject_InqMethod(object, dpe_rad_base_method_id);
            (*exec_rtn)(object);
            return;
        }

        /* even if prims are disabled, we need to update add_data->usageCur */
        add_data =
           dpr_radiosity_get_current_set_additional_data(head_add_data);

        if (prim_disabled)
            /* a multipass technique is being used */
            return;

        trilst = (struct trilst *)object->data;

        /* find out if user want an ambient component added */

        query_rtn = DDclass_InqMethod
                    (DsInqClassId("AttRadiosityAmbientSwitch"),
                    DsInqMethodId("InqGlobalValue"));
        (*query_rtn) (&radambswi);

        if (radambswi == DcOn) {
           ambient[0] = dpe_rad_ambient.component[0] *
                                  add_data->objectReflectance[0];

           ambient[1] = dpe_rad_ambient.component[1] *
                                  add_data->objectReflectance[1];

           ambient[2] = dpe_rad_ambient.component[2] *
                                  add_data->objectReflectance[2];
       } else {
           ambient[0] = 0.0;
           ambient[1] = 0.0;
           ambient[2] = 0.0;
       }

        /* transfer patch radiosities to vertex radiosities */
    
        current_vertextype = trilst->vertextype;

        /* Distribute patch radiosities to vertex radiosities 
           For a list, we just duplicate the patch radiosity for
           the three triangle's vertices */
       
        elem = -1;
        for (i=0; i<trilst->triangle_count*3; i++) {
           if ((i%3) == 0)
               elem++;

           add_data->vertexRad[i][0] = add_data->patchRad[elem][0] + ambient[0];
           add_data->vertexRad[i][1] = add_data->patchRad[elem][1] + ambient[1];
           add_data->vertexRad[i][2] = add_data->patchRad[elem][2] + ambient[2];

           /* if veretx colors are present, blend with radiosities */
           if (current_vertextype == DcLocClr || 
               current_vertextype == DcLocNrmClr) {
               add_data->vertexRad[i][0] *= trilst->vertex_color[elem][0];
               add_data->vertexRad[i][1] *= trilst->vertex_color[elem][1];
               add_data->vertexRad[i][2] *= trilst->vertex_color[elem][2];
           }

           /* clip final values */

           if (add_data->vertexRad[i][0] > 1.0) 
                add_data->vertexRad[i][0] = 1.0;
           if (add_data->vertexRad[i][1] > 1.0) 
                add_data->vertexRad[i][1] = 1.0;
           if (add_data->vertexRad[i][2] > 1.0) 
                add_data->vertexRad[i][2] = 1.0;

        }

        current_vertex_color = trilst->vertex_color;

        /* swap for the radiosity results */
        trilst->vertex_color = add_data->vertexRad;

        /* change vertex type to indicate presence of vertex colors */
        trilst->vertextype = trilst->vertextype | DcLocClr;

        /* set current surface shader to constant shading 
           since radiosities include light information already */
  
        query_rtn = DDclass_InqMethod(DsInqClassId("AttSurfaceShade"),
                        DsInqMethodId("InqGlobalValue"));
        (*query_rtn) (&current_srfshd);

        /* set surface shade to constant if necessary */
        if (current_srfshd != DcShaderConstant) 
            DsExecuteObj(DoSurfaceShade(DcShaderConstant));

        obj_exec_rtn = DDobject_InqMethod(object, dpe_rad_base_method_id);
        (*obj_exec_rtn)(object);

        /* restore previous attribute values */

        if (current_srfshd != DcShaderConstant)
            DsExecuteObj(DoSurfaceShade(current_srfshd));

        /* swap original vertex colors back */
        trilst->vertex_color = current_vertex_color;

        trilst->vertextype = current_vertextype;
   }

}
/*
 ======================================================================
 */

