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
#include <dore/dore_develop/private/simplymsh.h>
#include <dore/dore_develop/private/varsplymsh.h>

/*
 ======================================================================
 Functions:
   dpr_radiosity_del_datalsts
   dpr_radiosity_del_simply_datalsts
   dpr_radiosity_del_simplymsh_datalsts
   dpr_radiosity_del_varsimplymsh_datalsts

 ======================================================================
 */

void dpr_radiosity_del_datalsts (dot_object *object)
{
    dpt_rad_primitive_data *cur_add_data, *save;
    dpt_rad_primitive_data *head_add_data;
    DtInt index;

    index = dpr_radiosity_get_id_additional_data_index
                                  (DsInqObjClassId(object));
    head_add_data = object->additional_data[index];

    if (head_add_data->usageCount > 1) {

        /* delete all sets of additional data except the head set */
        cur_add_data = head_add_data->next;

        while (cur_add_data != DcNullPtr) {
            save = cur_add_data->next;
            dpr_radiosity_prim_delete_data(object, cur_add_data);
            cur_add_data = save;
        }
    }

    head_add_data->usageCount = 0;
    head_add_data->usageCur = 0;
    head_add_data->next = DcNullPtr;
}

/* ====================================================================== */

void dpr_radiosity_del_simply_datalsts (dot_object *object)
{
    struct simply *simply;

    /* delete additional data of DoSimplePolygon object */
    dpr_radiosity_del_datalsts(object);

    simply = (struct simply *) object->data;

    /* delete additional data of DoSimplePolygon's alternate  object */
    if (simply->alt_object_tris != DcNullObject)
        dpr_radiosity_del_datalsts(simply->alt_object_tris);
}

/* ====================================================================== */

void dpr_radiosity_del_simplymsh_datalsts (dot_object *object)
{
    struct simplymsh *simplymsh;

    /* delete additional data of DoSimplePolygonMesh object */
    dpr_radiosity_del_datalsts(object);

    simplymsh = (struct simplymsh *) object->data;

    /* delete additional data of DoSimplePolygon's alternate  object */
    if (simplymsh->alt_object_tris != DcNullObject)
        dpr_radiosity_del_datalsts(simplymsh->alt_object_tris);
}

/* ====================================================================== */

void dpr_radiosity_del_varsimplymsh_datalsts (dot_object *object)
{
    struct varsimplymsh *varsimplymsh;

    /* delete additional data of DoSimplePolygonMesh object */
    dpr_radiosity_del_datalsts(object);

    varsimplymsh = (struct varsimplymsh *) object->data;

    /* delete additional data of DoVarSimplePolygon's alternate  object */
    if (varsimplymsh->alt_object_tris != DcNullObject)
        dpr_radiosity_del_datalsts(varsimplymsh->alt_object_tris);
}
