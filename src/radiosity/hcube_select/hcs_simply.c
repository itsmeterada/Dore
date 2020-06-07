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
#include "dore/dore_develop/private/simply.h"

/*
 ======================================================================
 Functions:

    void dpr_radiosity_simply_hcube_select(object)

 ======================================================================
 */

void dpr_radiosity_simply_hcube_select (
    dot_object *object)
{
    dpt_rad_primitive_data *add_data, *head_add_data;
    struct simply *simply;
    static DtInt index = -1;
    DtRealTriple energy_per_channel;
    DtReal patch_energy;
    DtInt k;

    if (index == -1) 
        index = dpr_radiosity_get_additional_data_index("DoSimplePolygon");

    head_add_data = object->additional_data[index];

    add_data = dpr_radiosity_get_current_set_additional_data(head_add_data);

    simply = (struct simply *)object->data;

    energy_per_channel[0] = add_data->patchUnshotRad[0][0] *
                            add_data->patchArea[0];
    energy_per_channel[1] = add_data->patchUnshotRad[0][1] *
                            add_data->patchArea[0];
    energy_per_channel[2] = add_data->patchUnshotRad[0][2] *
                            add_data->patchArea[0];

    dpe_rad_energy.total_unshot_per_channel[0] +=
                                  energy_per_channel[0];
    dpe_rad_energy.total_unshot_per_channel[1] +=
                                  energy_per_channel[1];
    dpe_rad_energy.total_unshot_per_channel[2] +=
                                  energy_per_channel[2];

    patch_energy = energy_per_channel[0] +
                   energy_per_channel[1] +
                   energy_per_channel[2];

    dpe_rad_energy.total_unshot += patch_energy;

    if (patch_energy > dpe_rad_brightest_patch.energy) {

        dpe_rad_brightest_patch.intElemColor =
                          add_data->patchIntElemColor[0];

        dpe_rad_brightest_patch.energy = patch_energy;

        dpe_rad_brightest_patch.area = add_data->patchArea[0];

        /* update the pointer to the patch radiosity
           note : we do not copy the information */

         dpe_rad_brightest_patch.unshotRadPtr =
                   (DtReal *) add_data->patchUnshotRad[0];
   
         if (dpe_rad_brightest_patch.nVert != simply->vertex_count) {
            dpe_rad_brightest_patch.vertices = (DtRealTriple *)
                    DDspace_Reallocate(dpe_rad_brightest_patch.vertices,
                                 sizeof(DtRealTriple)*simply->vertex_count);

         }
         dpe_rad_brightest_patch.nVert = simply->vertex_count;

         for (k=0; k<simply->vertex_count; k++) {

            dpe_rad_brightest_patch.vertices[k][0] =
                             simply->vertex_location[k][0];
            dpe_rad_brightest_patch.vertices[k][1] =
                             simply->vertex_location[k][1];
            dpe_rad_brightest_patch.vertices[k][2] =
                             simply->vertex_location[k][2];
        
        dpr_radiosity_copy_matrix(dpe_rad_hcube_select_att.lcswcs,
                                      dpe_rad_brightest_patch.lcswcs);
        }
    }

}

/*
 ======================================================================
 */

