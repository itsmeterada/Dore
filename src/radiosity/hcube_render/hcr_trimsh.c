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
#include <dore/dore_develop/private/trimsh.h>

/*
 ======================================================================
 Functions:
    void dpr_radiosity_trimsh_hcube_render(object)

 ======================================================================
 */


extern DtRealTriple  *dre_dynrnd_drom_element_colors;


/*
 ======================================================================
 */

void dpr_radiosity_trimsh_hcube_render (
    dot_object *object)
{
    static DtMethodPtr exec_rtn = DcNullPtr;
    static DtInt index = -1;
    struct trimsh *trimsh;
    DtRealTriple *current_vertexcolors;
    dpt_rad_primitive_data *add_data, *head_add_data;

    if (index == -1) 
        index = dpr_radiosity_get_additional_data_index("DoTriangleMesh");

    head_add_data = object->additional_data[index];

    add_data = dpr_radiosity_get_current_set_additional_data(head_add_data);

    trimsh = (struct trimsh *)object->data;

    /* vertex colors have precedence over element colors, so make
       hardware ignore vertex colors if there are any */

    current_vertexcolors = trimsh->vertex_color;
    trimsh->vertex_color = DcNullPtr;

    /* set global dynamic renderer array for element colors*/
    dre_dynrnd_drom_element_colors = add_data->patchElemColor;

    if (exec_rtn == DcNullPtr)
       exec_rtn =
            DDobject_InqMethod(object, dpe_rad_base_method_id);
    (*exec_rtn)(object);

    dre_dynrnd_drom_element_colors = (void *)DcNullPtr;

   /* restore vertex colors, if necessary */

   trimsh->vertex_color = current_vertexcolors;

}
