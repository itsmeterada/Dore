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
   void dpr_radiosity_simplymsh_hcube_setup(object)
   void dor_radiosity_count_simple_simplymsh_triangle()
   void dor_radiosity_count_complex_simplymsh_triangle()
   void dor_radiosity_simplymsh_get_tris_per_polygon(simplymsh,add_data)

 ======================================================================
 */

static DtInt numtri;

void dpr_radiosity_simplymsh_hcube_setup (
    dot_object *object)
{
    dpt_rad_primitive_data *add_data, *head_add_data;
    dpt_rad_primitive_data *alt_add_data, *head_alt_add_data;
    struct trilst *alt_trilst;
    struct simplymsh *simplymsh;
    static DtInt index = -1;
    static DtInt alt_index = -1;
    DtInt i, polytri, elem, v, k;
    DtInt next_integer_color, vertexno;
    DtInt size, triplesize,  max_vert;
    DtInt *tris_per_polygon;
    DtRealTriple next_normvec_color;
    DtRealTriple init_rad;
    DtRealTriple *transf_polyvertex;

    /* don't do anything if this object's radiosity is off */
    if (dpe_rad_hcube_setup_att.radswi == DcOff) 
       return;

    /* no radiosity allowed on alt. line representation */
    if (dpe_rad_hcube_setup_att.reptyp == DcOutlines) 
       return;

    /* return if object is invisible (DoInvisSwitch) or if the class
       is not executable (DoExecSet) */
    if (!DDcondex_QueryRender(DsInqClassId("DoSimplePolygonMesh")))
         return;

    DgAddObjToGroup(dpe_rad_hcube_data.display_group, object);

    if (index == -1) 
        index = dpr_radiosity_get_additional_data_index("DoSimplePolygonMesh");

    head_add_data = object->additional_data[index];

    add_data = dpr_radiosity_get_next_set_additional_data(head_add_data);

    simplymsh = (struct simplymsh *)object->data;

    if (add_data->patchRad == DcNullPtr) {
        /* this is the first time this object is used in radiosity */

        size = sizeof(DtReal)*simplymsh->polygon_count; 
        triplesize = sizeof(DtRealTriple)*simplymsh->polygon_count;

        add_data->patchRad = (DtRealTriple *)
                              DDspace_Allocate(triplesize); 

        add_data->patchUnshotRad = (DtRealTriple *)
                                    DDspace_Allocate(triplesize); 

        add_data->patchElemColor = (DtRealTriple *)
                                    DDspace_Allocate(triplesize); 

        size = sizeof(DtInt)*simplymsh->polygon_count;
        add_data->patchIntElemColor = (DtInt *)DDspace_Allocate(size); 

        size = sizeof(DtReal)*simplymsh->polygon_count;
        add_data->patchArea = (DtReal *) DDspace_Allocate(size);

        size =  sizeof(DtRealTriple)*simplymsh->vertex_count;
        add_data->vertexRad = (DtRealTriple *)DDspace_Allocate(size); 
    }

    /* store diffuse color of the object */
    add_data->objectReflectance[0] = dpe_rad_hcube_setup_att.difclr[0];
    add_data->objectReflectance[1] = dpe_rad_hcube_setup_att.difclr[1];
    add_data->objectReflectance[2] = dpe_rad_hcube_setup_att.difclr[2];

    /* initialize radiosity fields */

    if (dpe_rad_hcube_setup_att.emiswi == DcOn) {
        init_rad[0] = dpe_rad_hcube_setup_att.emiclr[0] *
                      dpe_rad_hcube_setup_att.emifct;
        init_rad[1] = dpe_rad_hcube_setup_att.emiclr[1] *
                      dpe_rad_hcube_setup_att.emifct;
        init_rad[2] = dpe_rad_hcube_setup_att.emiclr[2] *
                      dpe_rad_hcube_setup_att.emifct;
    } else {
        init_rad[0] = 0.0;
        init_rad[1] = 0.0;
        init_rad[2] = 0.0;
    }

    /* find the polygon with the most vertices in mesh */
    max_vert = 0;
    for (i=0; i<simplymsh->polygon_count; i++) {
         if (simplymsh->contours[i] > max_vert)
            max_vert = simplymsh->contours[i];
    }

    size = sizeof(DtRealTriple)*max_vert;
    transf_polyvertex = (DtRealTriple *)DDspace_Allocate(size);

    /* we need the triangle alternate representation for hcube drawing */
       
    if (simplymsh->alt_object_tris == DcNullObject) 
        simplymsh->alt_object_tris = dor_simplymsh_update_tris(object);

    /* we also need the number of triangles in which every polygon was
       decomposed. This is why we call our modified decomposition
       routine that keeps trck of those numbers without re-creating
       the alternate object */

    size = sizeof(DtInt)*simplymsh->polygon_count;
    tris_per_polygon = (DtInt *) DDspace_Allocate(size);

    dor_radiosity_simplymsh_get_tris_per_polygon(simplymsh, tris_per_polygon);

    alt_trilst = (struct trilst *) (simplymsh->alt_object_tris)->data;
    if (alt_index == -1)
        alt_index = dpr_radiosity_get_additional_data_index("DoTriangleList");
    head_alt_add_data = (simplymsh->alt_object_tris)->additional_data[alt_index];

    alt_add_data =
        dpr_radiosity_get_next_set_additional_data(head_alt_add_data);

    /* we want all the triangles of a same polygon to have the same
       element color since each polygon is considered as an entity
       by the radiosity algorithm */
        
    size = sizeof(DtRealTriple)*alt_trilst->triangle_count;
    alt_add_data->patchElemColor = (DtRealTriple *)
                                     DDspace_Allocate (size);

    size = sizeof(DtRealTriple)*alt_trilst->triangle_count*3;
    alt_add_data->vertexRad = (DtRealTriple *) DDspace_Allocate(size);

    for (i=0, elem=0, v=0; i<simplymsh->polygon_count; i++) {

        add_data->patchRad[i][0] = init_rad[0];  
        add_data->patchRad[i][1] = init_rad[1];  
        add_data->patchRad[i][2] = init_rad[2];  

        add_data->patchUnshotRad[i][0] = init_rad[0];  
        add_data->patchUnshotRad[i][1] = init_rad[1];  
        add_data->patchUnshotRad[i][2] = init_rad[2];  

        dpr_radiosity_compute_next_color(&next_integer_color,
                                      next_normvec_color);

        add_data->patchIntElemColor[i] = next_integer_color;

        for (polytri=0; polytri<tris_per_polygon[i]; polytri++) {
            alt_add_data->patchElemColor[elem][0] = next_normvec_color[0];
            alt_add_data->patchElemColor[elem][1] = next_normvec_color[1];
            alt_add_data->patchElemColor[elem][2] = next_normvec_color[2];
            elem++;
        }

        /* apply current transformation to find real area */

        for (k=0;k<simplymsh->contours[i]; k++) {
            vertexno = simplymsh->vertexlist[v];
            v++;
            dpr_radiosity_transform_vertices
	    (   simplymsh->vertex_location + vertexno,
                1, dpe_rad_hcube_setup_att.lcswcs,
                transf_polyvertex + k
	    );
        }

        add_data->patchArea[i] = dpr_radiosity_compute_polygon_area
                     (transf_polyvertex, simplymsh->contours[i]);

        /* compute following sums to use later in ambient component calc */

        dpe_rad_ambient.sum_reflecXarea[0] += add_data->objectReflectance[0] *
                                    add_data->patchArea[i];
        dpe_rad_ambient.sum_reflecXarea[1] += add_data->objectReflectance[1] *
                                    add_data->patchArea[i];
        dpe_rad_ambient.sum_reflecXarea[2] += add_data->objectReflectance[2] *
                                    add_data->patchArea[i];
        dpe_rad_ambient.sum_area += add_data->patchArea[i];

        /* note:vertex colors are set only at display time */
    }

    DDspace_Deallocate(transf_polyvertex);
    DDspace_Deallocate(tris_per_polygon);
}

/*
 ======================================================================
 */
void dor_radiosity_count_simple_simplymsh_triangle (void)
{
    numtri++;
}

/*
 ======================================================================
 */
void dor_radiosity_count_complex_simplymsh_triangle (void)
{
    numtri++;
}

/*
 ======================================================================
 */
extern DtInt doe_simplymsh_polygonno;

/* derived from dor_simplymsh_create_alternate_geom_tris */

void dor_radiosity_simplymsh_get_tris_per_polygon (
    struct simplymsh *simplymsh,
    DtInt *tris_per_polygon)
{
    DtInt vertexlistloc;
    DtInt polygonno;
    DtInt vertexno;
    DtInt contourvertexcount;
    DtInt contourvertexno;

    vertexlistloc = 0;

    for (polygonno=0;polygonno<simplymsh->polygon_count;polygonno++) {
        dor_gpolyd_create_polygon();
        dor_gpolyd_create_contour();

        numtri = 0;

        contourvertexcount = simplymsh->contours[polygonno];

        for (contourvertexno=0;
             contourvertexno<contourvertexcount;
             contourvertexno++) {
            vertexno = simplymsh->vertexlist[vertexlistloc++];

            dor_gpolyd_create_contour_vertex(vertexno,
                             simplymsh->vertex_location[vertexno][0],
                             simplymsh->vertex_location[vertexno][1],
                             simplymsh->vertex_location[vertexno][2]);
        }

        dor_gpolyd_close_contour();
        dor_gpolyd_close_polygon();

        doe_simplymsh_polygonno = polygonno;

        dor_gpolyd_triangulate_polygon(simplymsh->shape,
                 dor_radiosity_count_simple_simplymsh_triangle,
                 dor_radiosity_count_complex_simplymsh_triangle,
                 simplymsh);

        tris_per_polygon[polygonno] = numtri;
    }

}

/*
 ======================================================================
 */
