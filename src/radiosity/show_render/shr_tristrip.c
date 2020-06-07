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
#include "dore/dore_develop/private/tristrip.h"

/*
 ======================================================================
 Functions:

   void dpr_radiosity_tristrip_show_render(object)

 ======================================================================
 */

void dpr_radiosity_tristrip_show_render (
    dot_object *object)
{
    DtObject current_srfshd;
    DtInterpType current_interptyp;
    DtRealTriple sumrad, ambient;
    DtInt i, tricount;
    DtSwitch radswi, radambswi;
    static DtInt index = -1;
    DtRealTriple *current_vertex_color;
    DtMethodPtr exec_rtn, obj_exec_rtn, query_rtn;
    DtVertexType current_vertextype;
    DtFlag renderable, prim_disabled;
    struct tristrip *tristrip;
    dpt_rad_primitive_data *add_data, *head_add_data;

    renderable = DDcondex_QueryRender(DsInqClassId("DoTriangleStrip"));

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
            index = dpr_radiosity_get_additional_data_index("DoTriangleStrip");

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
           
        tristrip = (struct tristrip *)object->data;

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
    
        current_vertextype = tristrip->vertextype;
		tricount = tristrip->vertex_count - 2;

		/* first vertex (only belongs to one triangle) */
		add_data->vertexRad[0][0] = ambient[0] + add_data->patchRad[0][0];
		add_data->vertexRad[0][1] = ambient[1] + add_data->patchRad[0][1];
		add_data->vertexRad[0][2] = ambient[2] + add_data->patchRad[0][2];

		if (tricount == 1) {
			add_data->vertexRad[1][0] = add_data->vertexRad[2][0] =
										add_data->vertexRad[0][0];
			add_data->vertexRad[1][1] = add_data->vertexRad[2][1] =
										add_data->vertexRad[0][1];
			add_data->vertexRad[1][2] = add_data->vertexRad[2][2] =
										add_data->vertexRad[0][2];
		} else {

			/* second vertex (belongs to two triangles) */
			add_data->vertexRad[1][0] = ambient[0] +
                 (add_data->patchRad[0][0] + add_data->patchRad[1][0])/2.0; 
			add_data->vertexRad[1][1] = ambient[1] +
                 (add_data->patchRad[0][1] + add_data->patchRad[1][1])/2.0; 
			add_data->vertexRad[1][2] = ambient[2] +
                 (add_data->patchRad[0][2] + add_data->patchRad[1][2])/2.0; 

		    /* second to last vertex (belongs to two triangles) */
		    add_data->vertexRad[tristrip->vertex_count-2][0] = ambient[0] +
                  (add_data->patchRad[tricount-1][0] +
		           add_data->patchRad[tricount-2][0])/2.0;
		    add_data->vertexRad[tristrip->vertex_count-2][1] = ambient[1] +
                  (add_data->patchRad[tricount-1][1] +
		           add_data->patchRad[tricount-2][1])/2.0;
		    add_data->vertexRad[tristrip->vertex_count-2][2] = ambient[2] +
                  (add_data->patchRad[tricount-1][2] +
		           add_data->patchRad[tricount-2][2])/2.0;

		    /* last vertex (belongs to only one triangle) */
		    add_data->vertexRad[tristrip->vertex_count-1][0] = 
		          ambient[0] + add_data->patchRad[tricount-1][0];
		    add_data->vertexRad[tristrip->vertex_count-1][1] = 
		          ambient[1] + add_data->patchRad[tricount-1][1];
		    add_data->vertexRad[tristrip->vertex_count-1][2] = 
		          ambient[2] + add_data->patchRad[tricount-1][2];
		}

		if (tricount > 2) {
		    /* all other vertices are shared by three triangles */
		    for (i=2; i<tristrip->vertex_count-2; i++) {
			    add_data->vertexRad[i][0] = ambient[0] +
		                 (add_data->patchRad[i-2][0] +
			              add_data->patchRad[i-1][0] +
			              add_data->patchRad[i][0]) / 3.0;
			    add_data->vertexRad[i][1] = ambient[1] +
		                 (add_data->patchRad[i-2][1] +
			              add_data->patchRad[i-1][1] +
			              add_data->patchRad[i][1]) / 3.0;
			    add_data->vertexRad[i][2] = ambient[2] +
		                 (add_data->patchRad[i-2][2] +
			              add_data->patchRad[i-1][2] +
			              add_data->patchRad[i][2]) / 3.0;
		    }
		}

		for (i=0; i<tristrip->vertex_count; i++) {

            /* if vertex colors are present, blend with radiosities */
            if (current_vertextype == DcLocClr || 
                current_vertextype == DcLocNrmClr) {
            	add_data->vertexRad[i][0] *= tristrip->vertex_color[i][0];
            	add_data->vertexRad[i][1] *= tristrip->vertex_color[i][1];
                add_data->vertexRad[i][2] *= tristrip->vertex_color[i][2];
            }

            /* clip final values */
            if (add_data->vertexRad[i][0] > 1.0) 
                    add_data->vertexRad[i][0] = 1.0;
            if (add_data->vertexRad[i][1] > 1.0) 
                    add_data->vertexRad[i][1] = 1.0;
            if (add_data->vertexRad[i][2] > 1.0) 
                    add_data->vertexRad[i][2] = 1.0;
        }

        current_vertex_color = tristrip->vertex_color;

        /* swap for the radiosity results */
        tristrip->vertex_color = add_data->vertexRad;

        /* change vertex type to indicate presence of vertex colors */
        tristrip->vertextype = tristrip->vertextype | DcLocClr;

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

        /* set interp type to vertex shade to interpolate vtx radiosities*/
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
        tristrip->vertex_color = current_vertex_color;

        tristrip->vertextype = current_vertextype;
   }

}
/*
 ======================================================================
 */

