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
#include "dore/dore_develop/private/simplymsh.h"
#include "dore/dore_develop/private/trilst.h"

/*
 ======================================================================
 Functions:

   void dpr_radiosity_simplymsh_show_render(object)
   void dpr_radiosity_distribute_simple_simplymsh_triangle(simplymsh,
				 vertex1,vertex2,vertex3)
   void dpr_radiosity_distribute_complex_simplymsh_triangle(simplymsh, 
				  gpolydvtx1,gpolydvtx2,gpolydvtx3)

 ======================================================================
 */

extern DtInt doe_simplymsh_polygonno;

static dpt_rad_primitive_data *add_data;
static dpt_rad_primitive_data *alt_add_data;
static DtInt tri_alt_vtxcnt;

void dpr_radiosity_simplymsh_show_render (
    dot_object *object)
{
    DtObject current_srfshd;
    DtInterpType current_interptyp;
    DtRealTriple sumrad, ambient;
    DtInt i, k;
    static DtInt index = -1;
    static DtInt alt_index = -1;
    DtInt numply;
    DtSwitch radswi, radambswi;
    DtRealTriple *current_vertex_color, *alt_current_vertex_color;
    DtMethodPtr exec_rtn, obj_exec_rtn, query_rtn;
    DtVertexType current_vertextype, alt_current_vertextype;
    DtRepType reptype;
    DtFlag renderable, prim_disabled;
    struct simplymsh *simplymsh;
    struct trilst *alt_trilst;
    dpt_rad_primitive_data *head_add_data, *head_alt_add_data;
    DtInt vtx, vtxlstloc;
    DtInt polygonno;
    DtInt vertexno;
    DtInt contourvertexcount;
    DtInt contourvertexno;

    renderable = DDcondex_QueryRender(DsInqClassId("DoSimplePolygonMesh"));

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

    if (reptype == DcOutlines)  {
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
    if (!renderable && !prim_disabled) {
       /* invisible or out of ExecSet: object has no add data */
        dpr_radiosity_set_current_method_show_render();
        return;
    }

    if (index == -1)
        index = dpr_radiosity_get_additional_data_index("DoSimplePolygonMesh");

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

    simplymsh = (struct simplymsh *)object->data;

    alt_trilst = (struct trilst *) (simplymsh->alt_object_tris)->data;
    if (alt_index == -1)
          alt_index = dpr_radiosity_get_additional_data_index("DoTriangleList");
    head_alt_add_data =
             (simplymsh->alt_object_tris)->additional_data[alt_index];

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
       of polygon and average values at vertices. 
       We need to fill out both the object and alternate
       object's vertex colors, since we don't know in advance whether
       the renderer can reander the polygon directly or not */
    
    current_vertextype = simplymsh->vertextype;

    for (vtx=0; vtx<simplymsh->vertex_count; vtx++) {
        sumrad[0] = sumrad[1] = sumrad[2] = 0;
        numply = 0;
        vtxlstloc = -1;
        for (k=0; k<simplymsh->polygon_count; k++) {
            for (i=0; i<simplymsh->contours[k]; i++) {
                 vtxlstloc++;
                 if (simplymsh->vertexlist[vtxlstloc] == vtx) {
                     sumrad[0] += add_data->patchRad[k][0];
                     sumrad[1] += add_data->patchRad[k][1];
                     sumrad[2] += add_data->patchRad[k][2];
                     numply++;
                     break;
                 }
            }
        }

	if (numply != 0) {
            add_data->vertexRad[vtx][0] = sumrad[0]/numply + ambient[0];
            add_data->vertexRad[vtx][1] = sumrad[1]/numply + ambient[1];
            add_data->vertexRad[vtx][2] = sumrad[2]/numply + ambient[2];

            /* if vertex colors are present, blend with radiosities */
            if (current_vertextype == DcLocClr || 
                current_vertextype == DcLocNrmClr) {
                add_data->vertexRad[vtx][0] *= simplymsh->vertex_color[vtx][0];
                add_data->vertexRad[vtx][1] *= simplymsh->vertex_color[vtx][1];
                add_data->vertexRad[vtx][2] *= simplymsh->vertex_color[vtx][2];
            }
    
            /* clip final values */
            if (add_data->vertexRad[vtx][0] > 1.0) 
                add_data->vertexRad[vtx][0] = 1.0;
            if (add_data->vertexRad[vtx][1] > 1.0) 
                add_data->vertexRad[vtx][1] = 1.0;
            if (add_data->vertexRad[vtx][2] > 1.0) 
                add_data->vertexRad[vtx][2] = 1.0;
    
        } else { /* this vertex not used in the mesh */
            add_data->vertexRad[vtx][0] = 0;
            add_data->vertexRad[vtx][1] = 0;
            add_data->vertexRad[vtx][2] = 0;
        }

    } /* for vtx */

    alt_current_vertextype = alt_trilst->vertextype;

    /* distribute polygon vertex radiosities to alternate
       object triangle vertices */

    tri_alt_vtxcnt = 0;
    vtxlstloc = 0;
    for (polygonno=0;polygonno<simplymsh->polygon_count;polygonno++) {
        dor_gpolyd_create_polygon();
        dor_gpolyd_create_contour();

        contourvertexcount = simplymsh->contours[polygonno];

        for (contourvertexno=0; contourvertexno<contourvertexcount;
            contourvertexno++) {
            vertexno = simplymsh->vertexlist[vtxlstloc++];

            dor_gpolyd_create_contour_vertex(vertexno,
                     simplymsh->vertex_location[vertexno][0],
                     simplymsh->vertex_location[vertexno][1],
                     simplymsh->vertex_location[vertexno][2]);
        }

        dor_gpolyd_close_contour();
        dor_gpolyd_close_polygon();

        doe_simplymsh_polygonno = polygonno;

        dor_gpolyd_triangulate_polygon(simplymsh->shape,
               dpr_radiosity_distribute_simple_simplymsh_triangle,
               dpr_radiosity_distribute_complex_simplymsh_triangle,
               simplymsh);

    }

    for (i=0; i<alt_trilst->triangle_count*3; i++) {

        /* if vertex colors are present, blend with radiosities */
        if (alt_current_vertextype == DcLocClr || 
            alt_current_vertextype == DcLocNrmClr) {
            alt_add_data->vertexRad[i][0] *= 
                       alt_trilst->vertex_color[i][0];
            alt_add_data->vertexRad[i][1] *= 
                       alt_trilst->vertex_color[i][1];
            alt_add_data->vertexRad[i][2] *= 
                       alt_trilst->vertex_color[i][2];
        }

        /* clip final values */
        if (alt_add_data->vertexRad[i][0] > 1.0) 
                    add_data->vertexRad[i][0] = 1.0;
        if (alt_add_data->vertexRad[i][1] > 1.0) 
                    add_data->vertexRad[i][1] = 1.0;
        if (alt_add_data->vertexRad[i][2] > 1.0) 
                    add_data->vertexRad[i][2] = 1.0;

    }


    current_vertex_color = simplymsh->vertex_color;
    alt_current_vertex_color = alt_trilst->vertex_color;

    /* swap for the radiosity results */
    simplymsh->vertex_color = add_data->vertexRad;
    alt_trilst->vertex_color = alt_add_data->vertexRad;

    /* change vertex type to indicate presence of vertex colors */
    simplymsh->vertextype = simplymsh->vertextype | DcLocClr;
    alt_trilst->vertextype = alt_trilst->vertextype | DcLocClr;

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
    simplymsh->vertex_color = current_vertex_color;
    alt_trilst->vertex_color = alt_current_vertex_color;

    simplymsh->vertextype = current_vertextype;
    alt_trilst->vertextype = alt_current_vertextype;

    dpr_radiosity_set_current_method_show_render();
}
/*
 ======================================================================
 */

void dpr_radiosity_distribute_simple_simplymsh_triangle (
    struct simplymsh *simplymsh,
    DtInt vertex1,
    DtInt vertex2,
    DtInt vertex3)
{
    alt_add_data->vertexRad[tri_alt_vtxcnt][0] = 
					add_data->vertexRad[vertex1][0];
    alt_add_data->vertexRad[tri_alt_vtxcnt][1] = 
					add_data->vertexRad[vertex1][1];
    alt_add_data->vertexRad[tri_alt_vtxcnt][2] = 
					add_data->vertexRad[vertex1][2];
    tri_alt_vtxcnt++;

    alt_add_data->vertexRad[tri_alt_vtxcnt][0] = 
					add_data->vertexRad[vertex2][0];
    alt_add_data->vertexRad[tri_alt_vtxcnt][1] = 
					add_data->vertexRad[vertex2][1];
    alt_add_data->vertexRad[tri_alt_vtxcnt][2] = 
					add_data->vertexRad[vertex2][2];
    tri_alt_vtxcnt++;

    alt_add_data->vertexRad[tri_alt_vtxcnt][0] = 
					add_data->vertexRad[vertex3][0];
    alt_add_data->vertexRad[tri_alt_vtxcnt][1] = 
					add_data->vertexRad[vertex3][1];
    alt_add_data->vertexRad[tri_alt_vtxcnt][2] = 
					add_data->vertexRad[vertex3][2];
    tri_alt_vtxcnt++;
}

/*
 ======================================================================
 */

void dpr_radiosity_distribute_complex_simplymsh_triangle (
    struct simplymsh *simplymsh,
    dot_gpolyd_interpolated_vertex *gpolydvtx1,
    dot_gpolyd_interpolated_vertex *gpolydvtx2,
    dot_gpolyd_interpolated_vertex *gpolydvtx3)
{
    DtInt vertex1, vertex2;
    DtReal weight1, weight2;

    vertex1 = gpolydvtx1->vertex1;
    if (gpolydvtx1->interpolated) {
        vertex2 = gpolydvtx1->vertex2;

        weight1 = gpolydvtx1->weight1;
        weight2 = gpolydvtx1->weight2;

        alt_add_data->vertexRad[tri_alt_vtxcnt][0] = 
				weight1*add_data->vertexRad[vertex1][0] +
				weight2*add_data->vertexRad[vertex2][0];
        alt_add_data->vertexRad[tri_alt_vtxcnt][1] = 
				weight1*add_data->vertexRad[vertex1][1] +
				weight2*add_data->vertexRad[vertex2][1];
        alt_add_data->vertexRad[tri_alt_vtxcnt][2] = 
				weight1*add_data->vertexRad[vertex1][2] +
				weight2*add_data->vertexRad[vertex2][2];
    } else {
        alt_add_data->vertexRad[tri_alt_vtxcnt][0] = 
					add_data->vertexRad[vertex1][0];
        alt_add_data->vertexRad[tri_alt_vtxcnt][1] = 
					add_data->vertexRad[vertex1][1];
        alt_add_data->vertexRad[tri_alt_vtxcnt][2] = 
					add_data->vertexRad[vertex1][2];

    }
    tri_alt_vtxcnt++;

    vertex1 = gpolydvtx2->vertex1;
    if (gpolydvtx2->interpolated) {
        vertex2 = gpolydvtx2->vertex2;

        weight1 = gpolydvtx2->weight1;
        weight2 = gpolydvtx2->weight2;

        alt_add_data->vertexRad[tri_alt_vtxcnt][0] = 
				weight1*add_data->vertexRad[vertex1][0] +
				weight2*add_data->vertexRad[vertex2][0];
        alt_add_data->vertexRad[tri_alt_vtxcnt][1] = 
				weight1*add_data->vertexRad[vertex1][1] +
				weight2*add_data->vertexRad[vertex2][1];
        alt_add_data->vertexRad[tri_alt_vtxcnt][2] = 
				weight1*add_data->vertexRad[vertex1][2] +
				weight2*add_data->vertexRad[vertex2][2];
    } else {
        alt_add_data->vertexRad[tri_alt_vtxcnt][0] = 
					add_data->vertexRad[vertex1][0];
        alt_add_data->vertexRad[tri_alt_vtxcnt][1] = 
					add_data->vertexRad[vertex1][1];
        alt_add_data->vertexRad[tri_alt_vtxcnt][2] = 
					add_data->vertexRad[vertex1][2];

    }
    tri_alt_vtxcnt++;

    vertex1 = gpolydvtx3->vertex1;
    if (gpolydvtx3->interpolated) {
        vertex2 = gpolydvtx3->vertex2;

        weight1 = gpolydvtx3->weight1;
        weight2 = gpolydvtx3->weight2;

        alt_add_data->vertexRad[tri_alt_vtxcnt][0] = 
				weight1*add_data->vertexRad[vertex1][0] +
				weight2*add_data->vertexRad[vertex2][0];
        alt_add_data->vertexRad[tri_alt_vtxcnt][1] = 
				weight1*add_data->vertexRad[vertex1][1] +
				weight2*add_data->vertexRad[vertex2][1];
        alt_add_data->vertexRad[tri_alt_vtxcnt][2] = 
				weight1*add_data->vertexRad[vertex1][2] +
				weight2*add_data->vertexRad[vertex2][2];

    } else {
        alt_add_data->vertexRad[tri_alt_vtxcnt][0] = 
					add_data->vertexRad[vertex1][0];
        alt_add_data->vertexRad[tri_alt_vtxcnt][1] = 
					add_data->vertexRad[vertex1][1];
        alt_add_data->vertexRad[tri_alt_vtxcnt][2] = 
					add_data->vertexRad[vertex1][2];

    }
    tri_alt_vtxcnt++;
}

/*
 ======================================================================
 */

