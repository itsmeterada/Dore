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
#include "dore/dore_develop/private/trimsh.h"

/*
 ======================================================================
 Functions:
   void dpr_radiosity_trimsh_hcube_setup(object)

 ======================================================================
 */

void
dpr_radiosity_trimsh_hcube_setup(object)
dot_object *object;
{
    dpt_rad_primitive_data *add_data;
    dpt_rad_primitive_data *head_add_data;
    struct trimsh *trimsh;
    static DtInt index = -1;
    DtInt i;
    DtInt vertex0, vertex1, vertex2;
    DtInt next_integer_color;
    DtInt size, triplesize;
    DtRealTriple next_normvec_color;
    DtRealTriple init_rad;
    DtRealTriple *transf_vertices;

    /* don't do anything if this object's radiosity is off */
    if (dpe_rad_hcube_setup_att.radswi == DcOff) 
       return;

    /* return if object is invisible (DoInvisSwitch) or if the class
       is not executable (DoExecSet) */
    if (!DDcondex_QueryRender(DsInqClassId("DoTriangleMesh")))
         return;

    DgAddObjToGroup(dpe_rad_hcube_data.display_group, object);

    if (index == -1) 
        index = dpr_radiosity_get_additional_data_index("DoTriangleMesh");

    /* if index null here this will crash ! */

    head_add_data = object->additional_data[index];

    add_data = dpr_radiosity_get_next_set_additional_data(head_add_data);

    trimsh = (struct trimsh *)object->data;

    if (add_data->patchRad == DcNullPtr) {
        /* this is the first time this object is used in radiosity */

        size = sizeof(DtReal)*trimsh->triangle_count; 
        triplesize = sizeof(DtRealTriple)*trimsh->triangle_count;

        add_data->patchRad = (DtRealTriple *)
                              DDspace_Allocate(triplesize); 

        add_data->patchUnshotRad = (DtRealTriple *)
                                    DDspace_Allocate(triplesize); 

        add_data->patchElemColor = (DtRealTriple *)
                                    DDspace_Allocate(triplesize); 

        size = sizeof(DtInt)*trimsh->triangle_count;
        add_data->patchIntElemColor = (DtInt *) DDspace_Allocate(size); 

        size = sizeof(DtReal)*trimsh->triangle_count;
        add_data->patchArea = (DtReal *) DDspace_Allocate(size);

        size = trimsh->vertex_count*sizeof(DtRealTriple);
        add_data->vertexRad = (DtRealTriple *) DDspace_Allocate(size); 
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

    size = sizeof(DtRealTriple)*trimsh->vertex_count;
    transf_vertices = (DtRealTriple *)DDspace_Allocate(size);

    dpr_radiosity_transform_vertices(trimsh->vertex_location,
                       trimsh->vertex_count, 
                       dpe_rad_hcube_setup_att.lcswcs,
                       transf_vertices);

    for (i=0; i<trimsh->triangle_count; i++) {

         add_data->patchRad[i][0] = init_rad[0];  
         add_data->patchRad[i][1] = init_rad[1];  
         add_data->patchRad[i][2] = init_rad[2];  

         add_data->patchUnshotRad[i][0] = init_rad[0];  
         add_data->patchUnshotRad[i][1] = init_rad[1];  
         add_data->patchUnshotRad[i][2] = init_rad[2];  

         dpr_radiosity_compute_next_color(&next_integer_color,
                                         next_normvec_color);
         
         add_data->patchElemColor[i][0] = next_normvec_color[0];  
         add_data->patchElemColor[i][1] = next_normvec_color[1];  
         add_data->patchElemColor[i][2] = next_normvec_color[2];  

         add_data->patchIntElemColor[i] = next_integer_color;

         vertex0 = trimsh->triangle_vertexlist[i][0];
         vertex1 = trimsh->triangle_vertexlist[i][1];
         vertex2 = trimsh->triangle_vertexlist[i][2];

         add_data->patchArea[i] =
                dpr_radiosity_compute_triangle_area
                     (transf_vertices[vertex0],
                      transf_vertices[vertex1],
                      transf_vertices[vertex2]);

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

    DDspace_Deallocate(transf_vertices);

}
