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
#include "dore/dore_develop/private/vartrimsh.h"

/*
 ======================================================================
 Functions:

   void dpr_radiosity_vartrimsh_show_render(object)

 ======================================================================
 */

void dpr_radiosity_vartrimsh_show_render (
    dot_object *object)
{
    DtObject current_srfshd;
    DtInterpType current_interptyp;
    DtRealTriple sumrad, ambient;
    DtInt j, k, vtx;
    DtInt numtri;
    DtSwitch radswi, radambswi;
    static DtInt index = -1;
    DtRealTriple *current_vertex_color;
    DtMethodPtr exec_rtn, obj_exec_rtn, query_rtn;
    DtFlag renderable, prim_disabled;
    struct vartrimsh *vartrimsh;
    dpt_rad_primitive_data *add_data, *head_add_data;

    renderable = DDcondex_QueryRender(DsInqClassId("DoVarTriangleMesh"));

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
            index = dpr_radiosity_get_additional_data_index("DoVarTriangleMesh");

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
        add_data = dpr_radiosity_get_current_set_additional_data(head_add_data);

        if (prim_disabled) 
            /* a multipass technique is being used */
            return;
           
        vartrimsh = (struct vartrimsh *)object->data;

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
    
        for (vtx=0; vtx<vartrimsh->vertex_count; vtx++) {
           sumrad[0] = sumrad[1] = sumrad[2] = 0.0;
           numtri = 0;
           for (j=0; j<vartrimsh->triangle_count; j++) {
               for (k=0; k<3; k++) {
                   if (vartrimsh->triangle_vertexlist[j][k] == vtx) {
                      /* triangle j uses vertex vtx 
                      add rad of triangle j to vertex */
                   sumrad[0] += add_data->patchRad[j][0];
                   sumrad[1] += add_data->patchRad[j][1];
                   sumrad[2] += add_data->patchRad[j][2];
                   numtri++;
                   }
               }
            }

            if (numtri != 0) {
                add_data->vertexRad[vtx][0] = sumrad[0] / numtri + ambient[0];
                add_data->vertexRad[vtx][1] = sumrad[1] / numtri + ambient[1];
                add_data->vertexRad[vtx][2] = sumrad[2] / numtri + ambient[2];

               /* if vertex colors are present, blend with radiosities */
               if (vartrimsh->vertex_color != DcNullPtr) {
                   add_data->vertexRad[vtx][0] *= vartrimsh->vertex_color[vtx][0];
                   add_data->vertexRad[vtx][1] *= vartrimsh->vertex_color[vtx][1];
                   add_data->vertexRad[vtx][2] *= vartrimsh->vertex_color[vtx][2];
               }

               /* clip final values */

               if (add_data->vertexRad[vtx][0] > 1.0) 
                    add_data->vertexRad[vtx][0] = 1.0;
               if (add_data->vertexRad[vtx][1] > 1.0) 
                    add_data->vertexRad[vtx][1] = 1.0;
               if (add_data->vertexRad[vtx][2] > 1.0) 
                    add_data->vertexRad[vtx][2] = 1.0;

            } else {
               /* this vertex is not referenced in the mesh */
               add_data->vertexRad[vtx][0] = 0.0;
               add_data->vertexRad[vtx][1] = 0.0;
               add_data->vertexRad[vtx][2] = 0.0;
            }
        }

        current_vertex_color = vartrimsh->vertex_color;

        /* swap for the radiosity results */
        vartrimsh->vertex_color = add_data->vertexRad;

        /* set current surface shader to constant shading 
           since radiosities include light information already */
  
        query_rtn = DDclass_InqMethod(DsInqClassId("AttSurfaceShade"),
                        DsInqMethodId("InqGlobalValue"));
        (*query_rtn) (&current_srfshd);

        /* set surface shade to constant if necessary */
        if (current_srfshd != DcShaderConstant)  
            DsExecuteObj(DoSurfaceShade(DcShaderConstant));

        query_rtn = DDclass_InqMethod(DsInqClassId("AttInterpType"),
                        DsInqMethodId("InqGlobalValue"));
        (*query_rtn) (&current_interptyp);

        /* set interp type to vertex shade to interpolate v radiosities*/
        if (current_interptyp != DcVertexShade) 
            DsExecuteObj(DoInterpType(DcVertexShade));

        obj_exec_rtn = DDobject_InqMethod(object, dpe_rad_base_method_id);
        (*obj_exec_rtn)(object);

        /* restore previous attribute values */

        if (current_srfshd != DcShaderConstant) 
            DsExecuteObj(DoSurfaceShade(current_srfshd));

        if (current_interptyp != DcVertexShade)
            DsExecuteObj(DoInterpType(current_interptyp));

        /* swap original vertex colors back */
        vartrimsh->vertex_color = current_vertex_color;

   }

}
/*
 ======================================================================
 */

