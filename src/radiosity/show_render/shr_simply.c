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
#include <dore/dore_develop/private/simply.h>
#include <dore/dore_develop/private/trilst.h>
/*
 ======================================================================
 Functions:

   void dpr_radiosity_simply_show_render(object)

 ======================================================================
 */

void dpr_radiosity_simply_show_render (
    dot_object *object)
{
    DtObject current_srfshd;
    DtRealTriple ambient;
    DtInt i;
    static DtInt index = -1;
    static DtInt alt_index = -1;
    DtSwitch radswi, radambswi;
    DtRealTriple *current_vertex_color, *alt_current_vertex_color;
    DtMethodPtr exec_rtn, obj_exec_rtn, query_rtn;
    DtVertexType current_vertextype, alt_current_vertextype;
    DtRepType reptype;
    DtFlag renderable, prim_disabled;
    struct simply *simply;
    struct trilst *alt_trilst;
    dpt_rad_primitive_data *add_data, *head_add_data;
    dpt_rad_primitive_data *alt_add_data, *head_alt_add_data;

    renderable = DDcondex_QueryRender(DsInqClassId("DoSimplePolygon"));

    /* if we are in a callback, current method is show_render: change it
       to make sure the base method will be executed on the alternate object,
       if it is used */
    dpr_radiosity_set_current_method_base();

    query_rtn = DDclass_InqMethod(DsInqClassId("AttRadiositySwitch"),
                        DsInqMethodId("InqGlobalValue"));
    (*query_rtn) (&radswi);

    if (radswi == DcOff) {
        if (!renderable) {
           dpr_radiosity_set_current_method_show_render();
           return;
        }

        /* do rendering as usual */
        exec_rtn = DDobject_InqMethod(object, dpe_rad_base_method_id);
        (*exec_rtn)(object);
        dpr_radiosity_set_current_method_show_render();
        return;
    }

    query_rtn = DDclass_InqMethod(DsInqClassId("AttRepType"),
                    DsInqMethodId("InqGlobalValue"));
    (*query_rtn) (&reptype);

    if (reptype == DcOutlines) {
        if (!renderable) {
           dpr_radiosity_set_current_method_show_render();
           return;
        }
        /* we ignore radiosity if outlines are requested */
        obj_exec_rtn = DDobject_InqMethod(object, dpe_rad_base_method_id);
        (*obj_exec_rtn)(object);
        dpr_radiosity_set_current_method_show_render();
        return;
    }

    prim_disabled = DDcondex_QueryPrimExecFlag();
    if (!renderable && !prim_disabled){
        /* invisible or out of ExecSet: object has no add data */
        dpr_radiosity_set_current_method_show_render();
        return;
    }

    if (index == -1)
        index = dpr_radiosity_get_additional_data_index("DoSimplePolygon");

    head_add_data = object->additional_data[index];

    if (head_add_data->usageCount == 0) {
        if (!renderable) {
             dpr_radiosity_set_current_method_show_render();
             return;
        }
        /* no radiosity available for this object (either created
           from a callback or radswi was off during last compute) */
        exec_rtn = DDobject_InqMethod(object, dpe_rad_base_method_id);
        (*exec_rtn)(object);
        dpr_radiosity_set_current_method_show_render();
        return;
    }

    /* even if prims are disabled, we need to update add_data->usageCur */
    add_data =
       dpr_radiosity_get_current_set_additional_data(head_add_data);

    simply = (struct simply *)object->data;

    alt_trilst = (struct trilst *) (simply->alt_object_tris)->data;
    if (alt_index == -1)
      alt_index = dpr_radiosity_get_additional_data_index("DoTriangleList");
    head_alt_add_data =
          (simply->alt_object_tris)->additional_data[alt_index];

    /* even if prims are disabled, we need to update add_data->usageCur */
    alt_add_data =
       dpr_radiosity_get_current_set_additional_data(head_alt_add_data);

    if (prim_disabled) {
        /* a multipass technique is being used */
        dpr_radiosity_set_current_method_show_render();
        return;
    }

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

    /* transfer patch radiosities of alt object to vertex radiosities 
       of polygon. We need to fill out both the object and alternate
       object's vertex colors, since we don't know in advance whether
       the renderer can render the polygon directly or not */
    
    current_vertextype = simply->vertextype;

    for (i=0; i<simply->vertex_count; i++) {
        add_data->vertexRad[i][0] = add_data->patchRad[0][0] + ambient[0];
        add_data->vertexRad[i][1] = add_data->patchRad[0][1] + ambient[1];
        add_data->vertexRad[i][2] = add_data->patchRad[0][2] + ambient[2];

        /* if vertex colors are present, blend with radiosities */
        if (current_vertextype == DcLocClr || 
           current_vertextype == DcLocNrmClr) {
           add_data->vertexRad[i][0] *= simply->vertex_color[i][0];
           add_data->vertexRad[i][1] *= simply->vertex_color[i][1];
           add_data->vertexRad[i][2] *= simply->vertex_color[i][2];
        }

        /* clip final values */
        if (add_data->vertexRad[i][0] > 1.0) 
                add_data->vertexRad[i][0] = 1.0;
        if (add_data->vertexRad[i][1] > 1.0) 
                add_data->vertexRad[i][1] = 1.0;
        if (add_data->vertexRad[i][2] > 1.0) 
                add_data->vertexRad[i][2] = 1.0;

    }

    alt_current_vertextype = alt_trilst->vertextype;

    for (i=0; i<alt_trilst->triangle_count*3; i++) {
            alt_add_data->vertexRad[i][0] =
                          add_data->patchRad[0][0] + ambient[0];
            alt_add_data->vertexRad[i][1] =
                          add_data->patchRad[0][1] + ambient[1];
            alt_add_data->vertexRad[i][2] =
                          add_data->patchRad[0][2] + ambient[2];

            /* if vertex colors are present, blend with radiosities */
            if (alt_current_vertextype == DcLocClr || 
               alt_current_vertextype == DcLocNrmClr) {
               alt_add_data->vertexRad[i][0] *= alt_trilst->vertex_color[i][0];
               alt_add_data->vertexRad[i][1] *= alt_trilst->vertex_color[i][1];
               alt_add_data->vertexRad[i][2] *= alt_trilst->vertex_color[i][2];
            }

            /* clip final values */
            if (alt_add_data->vertexRad[i][0] > 1.0) 
                    add_data->vertexRad[i][0] = 1.0;
            if (alt_add_data->vertexRad[i][1] > 1.0) 
                    add_data->vertexRad[i][1] = 1.0;
            if (alt_add_data->vertexRad[i][2] > 1.0) 
                    add_data->vertexRad[i][2] = 1.0;

        }

    current_vertex_color = simply->vertex_color;
    alt_current_vertex_color = alt_trilst->vertex_color;

    /* swap for the radiosity results */
    simply->vertex_color = add_data->vertexRad;
    alt_trilst->vertex_color = alt_add_data->vertexRad;

    /* change vertex type to indicate presence of vertex colors */
    simply->vertextype = simply->vertextype | DcLocClr;
    alt_trilst->vertextype = alt_trilst->vertextype | DcLocClr;

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
    simply->vertex_color = current_vertex_color;
    alt_trilst->vertex_color = alt_current_vertex_color;

    simply->vertextype = current_vertextype;
    alt_trilst->vertextype = alt_current_vertextype;
  
    dpr_radiosity_set_current_method_show_render();
}
/*
 ======================================================================
 */

