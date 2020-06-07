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
#include "dore/dore_develop/private/trilst.h"

/*
 ======================================================================
 Functions:
   void dpr_radiosity_simply_hcube_setup(object)

 ======================================================================
 */

void dpr_radiosity_simply_hcube_setup (
    dot_object *object)
{
    dpt_rad_primitive_data *add_data, *head_add_data;
    dpt_rad_primitive_data *alt_add_data, *head_alt_add_data;
    struct trilst *alt_trilst;
    struct simply *simply;
    static DtInt index = -1;
    static DtInt alt_index = -1;
    DtInt i;
    DtInt next_integer_color;
    DtInt size, triplesize;
    DtRealTriple next_normvec_color;
    DtRealTriple init_rad;
    DtRealTriple *transf_vertices;

    /* don't do anything if this object's radiosity is off */
    if (dpe_rad_hcube_setup_att.radswi == DcOff) 
       return;

    /* no radiosity allowed on alt. line representation */
    if (dpe_rad_hcube_setup_att.reptyp == DcOutlines) 
       return;

    /* return if object is invisible (DoInvisSwitch) or if the class
       is not executable (DoExecSet) */
    if (!DDcondex_QueryRender(DsInqClassId("DoSimplePolygon")))
         return;

    DgAddObjToGroup(dpe_rad_hcube_data.display_group, object);

    if (index == -1) 
        index = dpr_radiosity_get_additional_data_index("DoSimplePolygon");

    head_add_data = object->additional_data[index];

    add_data = dpr_radiosity_get_next_set_additional_data(head_add_data);

    simply = (struct simply *)object->data;

    /* this is a single polygon, so we allocate space only for one element */

    if (add_data->patchRad == DcNullPtr) {
       /* this is the first time this object is used in radiosity */

        size = sizeof(DtReal); 
        triplesize = sizeof(DtRealTriple);

        add_data->patchRad = (DtRealTriple *)
                              DDspace_Allocate(triplesize); 

        add_data->patchUnshotRad = (DtRealTriple *)
                                    DDspace_Allocate(triplesize); 

        add_data->patchElemColor = (DtRealTriple *)
                                    DDspace_Allocate(triplesize); 

        add_data->patchIntElemColor = (DtInt *)
                  DDspace_Allocate(sizeof(DtInt)); 

        add_data->patchArea = (DtReal *) DDspace_Allocate(size);

        add_data->vertexRad = (DtRealTriple *)
                               DDspace_Allocate(triplesize*simply->vertex_count); 
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

    /* apply current transformation to find real area */

    size = sizeof(DtRealTriple)*simply->vertex_count;
    transf_vertices = (DtRealTriple *)DDspace_Allocate (size);

    dpr_radiosity_transform_vertices(simply->vertex_location,
                       simply->vertex_count, 
                       dpe_rad_hcube_setup_att.lcswcs,
                       transf_vertices);

    add_data->patchRad[0][0] = init_rad[0];  
    add_data->patchRad[0][1] = init_rad[1];  
    add_data->patchRad[0][2] = init_rad[2];  

    add_data->patchUnshotRad[0][0] = init_rad[0];  
    add_data->patchUnshotRad[0][1] = init_rad[1];  
    add_data->patchUnshotRad[0][2] = init_rad[2];  

    add_data->patchElemColor[0][0] = next_normvec_color[0];  
    add_data->patchElemColor[0][1] = next_normvec_color[1];  
    add_data->patchElemColor[0][2] = next_normvec_color[2];  

    /* we need the triangle alternate representation for hcube drawing */

    if (simply->alt_object_tris == DcNullObject) 
        simply->alt_object_tris = dor_simply_update_tris(object);

    alt_trilst = (struct trilst *) (simply->alt_object_tris)->data;
    if (alt_index == -1)
      alt_index = dpr_radiosity_get_additional_data_index("DoTriangleList");

    head_alt_add_data = (simply->alt_object_tris)->additional_data[alt_index];

    alt_add_data =
         dpr_radiosity_get_next_set_additional_data(head_alt_add_data);

    /* we need to set the element colors of the alternate object since the
       alt.obj. is going to be used for hcube drawing (hcube_render) */

    size = sizeof(DtRealTriple)*alt_trilst->triangle_count;
    alt_add_data->patchElemColor = (DtRealTriple *) DDspace_Allocate(size);

    size = sizeof(DtRealTriple)*alt_trilst->triangle_count*3;
    alt_add_data->vertexRad = (DtRealTriple *) DDspace_Allocate(size);

    dpr_radiosity_compute_next_color(&next_integer_color,
                                         next_normvec_color);
         
    /* store that color for the entire polygon, but also store it
       as the element color of every triangle of the alt rep */

    add_data->patchIntElemColor[0] = next_integer_color;

    for (i=0; i<alt_trilst->triangle_count; i++) {
        alt_add_data->patchElemColor[i][0] = next_normvec_color[0];
        alt_add_data->patchElemColor[i][1] = next_normvec_color[1];
        alt_add_data->patchElemColor[i][2] = next_normvec_color[2];
    }
    
    add_data->patchArea[0] =
                dpr_radiosity_compute_polygon_area
                     (transf_vertices, simply->vertex_count);

    /* compute following sums to use later in ambient component calc */

    dpe_rad_ambient.sum_reflecXarea[0] += add_data->objectReflectance[0] *
                                    add_data->patchArea[0];
    dpe_rad_ambient.sum_reflecXarea[1] += add_data->objectReflectance[1] *
                                    add_data->patchArea[0];
    dpe_rad_ambient.sum_reflecXarea[2] += add_data->objectReflectance[2] *
                                    add_data->patchArea[0];
    dpe_rad_ambient.sum_area += add_data->patchArea[0];

    /* note:vertex colors are set only at display time */

    DDspace_Deallocate(transf_vertices);
}
