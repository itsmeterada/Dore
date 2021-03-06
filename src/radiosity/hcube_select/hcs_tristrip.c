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

    dpr_radiosity_tristrip_hcube_select

 ======================================================================
 */

void dpr_radiosity_tristrip_hcube_select (
    dot_object *object)
{
    dpt_rad_primitive_data *add_data, *head_add_data;
    struct tristrip *tristrip;
    static DtInt index = -1;
    DtRealTriple energy_per_channel;
    DtReal patch_energy;
    DtInt i, tricount, index1, index2;

    if (index == -1) 
        index = dpr_radiosity_get_additional_data_index("DoTriangleStrip");

    head_add_data = object->additional_data[index];

    add_data = dpr_radiosity_get_current_set_additional_data(head_add_data); 
    
    tristrip = (struct tristrip *)object->data;

	tricount = tristrip->vertex_count - 2;
    for (i=0; i<tricount; i++){

        energy_per_channel[0] = add_data->patchUnshotRad[i][0] *
                               add_data->patchArea[i];
        energy_per_channel[1] = add_data->patchUnshotRad[i][1] *
                                 add_data->patchArea[i];
        energy_per_channel[2] = add_data->patchUnshotRad[i][2] *
                                 add_data->patchArea[i];

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
                              add_data->patchIntElemColor[i];

            dpe_rad_brightest_patch.energy = patch_energy;

            dpe_rad_brightest_patch.area = add_data->patchArea[i];

            /* update the pointer to the patch radiosity
               note : we do not copy the information */

            dpe_rad_brightest_patch.unshotRadPtr =
                       (DtReal *) add_data->patchUnshotRad[i];
   
            if (dpe_rad_brightest_patch.nVert != 3) {
               dpe_rad_brightest_patch.vertices = (DtRealTriple *)
                    DDspace_Reallocate(dpe_rad_brightest_patch.vertices,
                                       sizeof(DtRealTriple)*3);
            }
            dpe_rad_brightest_patch.nVert = 3;

            dpe_rad_brightest_patch.vertices[0][0] =
                          tristrip->vertex_location[i][0];
            dpe_rad_brightest_patch.vertices[0][1] =
                          tristrip->vertex_location[i][1];
            dpe_rad_brightest_patch.vertices[0][2] =
                          tristrip->vertex_location[i][2];

			/* store vertices in order to compute correct normal later */
			if ((i%2) == 0) {
				/* even triangle: uses right hand rule */
				index1 = 1;
				index2 = 2;
			} else {
				/* odd triangle: uses left hand rule */
				index1 = 2;
				index2 = 1;
			}

            dpe_rad_brightest_patch.vertices[index1][0] =
                          tristrip->vertex_location[i+1][0];
            dpe_rad_brightest_patch.vertices[index1][1] =
                          tristrip->vertex_location[i+1][1];
            dpe_rad_brightest_patch.vertices[index1][2] =
                          tristrip->vertex_location[i+1][2];

            dpe_rad_brightest_patch.vertices[index2][0] =
                          tristrip->vertex_location[i+2][0];
            dpe_rad_brightest_patch.vertices[index2][1] =
                          tristrip->vertex_location[i+2][1];
            dpe_rad_brightest_patch.vertices[index2][2] =
                          tristrip->vertex_location[i+2][2];

            dpr_radiosity_copy_matrix(dpe_rad_hcube_select_att.lcswcs,
                                      dpe_rad_brightest_patch.lcswcs);

        }
                
    }
}

/*
 ======================================================================
 */
