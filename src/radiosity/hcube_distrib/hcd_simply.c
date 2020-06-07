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

    void dpr_radiosity_simply_hcube_distrib(object)

 ======================================================================
 */

void dpr_radiosity_simply_hcube_distrib (
    dot_object *object)
{
    dpt_rad_primitive_data *add_data, *head_add_data;
    static DtInt index = -1;
    DtRealTriple deltaRad;
    DtReal ff;
    DtInt int_elem_color;

    if (index == -1) 
        index = dpr_radiosity_get_additional_data_index("DoSimplePolygon");

    head_add_data = object->additional_data[index];

    add_data = dpr_radiosity_get_current_set_additional_data(head_add_data);

    int_elem_color = add_data->patchIntElemColor[0];

    /* ignore if the shooting patch or if form factor is zero */
    if ((int_elem_color != dpe_rad_brightest_patch.intElemColor) &&
        (dpe_rad_form_factors[int_elem_color] != 0.)) {

        /* What we have stored in the ff array at this point is
        F(i,j). What we want is F(j,i) so we multiply by
        the ratio if the areas (using reciprocity) */

        if (add_data->patchArea[0] != 0.)
            ff = dpe_rad_form_factors[int_elem_color] *
                 dpe_rad_brightest_patch.area / add_data->patchArea[0];
       else
           DDerror(ERR_RAD_ZERO_AREA,
                  "dpr_radiosity_simply_hcube_distrib", DcNullPtr);

        if (ff > 1.0)
            /* if this is above one, indicates more subdivision needed */
            ff = 1.0;

        deltaRad[0] = dpe_rad_brightest_patch.unshotRadPtr[0] *
                          ff * add_data->objectReflectance[0];

        deltaRad[1] = dpe_rad_brightest_patch.unshotRadPtr[1] * 
                          ff * add_data->objectReflectance[1];

        deltaRad[2] = dpe_rad_brightest_patch.unshotRadPtr[2] * 
                          ff * add_data->objectReflectance[2];

        add_data->patchRad[0][0] += deltaRad[0];
        add_data->patchRad[0][1] += deltaRad[1];
        add_data->patchRad[0][2] += deltaRad[2];

        add_data->patchUnshotRad[0][0] += deltaRad[0];
        add_data->patchUnshotRad[0][1] += deltaRad[1];
        add_data->patchUnshotRad[0][2] += deltaRad[2];

    }
}

/*
 ======================================================================
 */
