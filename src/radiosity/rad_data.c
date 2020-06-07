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
 
#include "rad.h" 

#define NO_ID  -2

/*
 ======================================================================
 Functions:

   dpr_radiosity_geom_check_already_installed 
   dpr_radiosity_request_additional_data
   dpr_radiosity_get_additional_data_index
   dpr_radiosity_get_id_additional_data_index

   dpr_radiosity_prim_create_data
   dpr_radiosity_print_null_rtn
   dpr_radiosity_prim_delete_data

 ======================================================================
*/

/* additional data will be used by several methods of the radiosity pkg */

/* we use the print routines only in debug mode: otherwise, since we link
   several sets of additional data to the same object if it is instanced
   several times, the user may not properly interpret the printed data
   correctly */

dpt_rad_additional_data   dpe_rad_additional_data[] = {
    { "DoTriangleMesh", -1,
          (DtMethodPtr) dpr_radiosity_prim_create_data,
          (DtMethodPtr) dpr_radiosity_print_null_rtn,
          (DtMethodPtr) dpr_radiosity_prim_delete_data, -1},

    { "DoTriangleList", -1,
          (DtMethodPtr) dpr_radiosity_prim_create_data,
          (DtMethodPtr) dpr_radiosity_print_null_rtn,
          (DtMethodPtr) dpr_radiosity_prim_delete_data, -1},

    { "DoTriangleStrip", -1,
          (DtMethodPtr) dpr_radiosity_prim_create_data,
          (DtMethodPtr) dpr_radiosity_print_null_rtn,
          (DtMethodPtr) dpr_radiosity_prim_delete_data, -1},

    { "DoSimplePolygon", -1,
          (DtMethodPtr) dpr_radiosity_prim_create_data,
          (DtMethodPtr) dpr_radiosity_print_null_rtn,
          (DtMethodPtr) dpr_radiosity_prim_delete_data, -1},

    { "DoSimplePolygonMesh", -1,
          (DtMethodPtr) dpr_radiosity_prim_create_data,
          (DtMethodPtr) dpr_radiosity_print_null_rtn,
          (DtMethodPtr) dpr_radiosity_prim_delete_data, -1},

    { "DoVarTriangleMesh", -1,
          (DtMethodPtr) dpr_radiosity_prim_create_data,
          (DtMethodPtr) dpr_radiosity_print_null_rtn,
          (DtMethodPtr) dpr_radiosity_prim_delete_data, -1},

    { "DoVarTriangleStrip", -1,
          (DtMethodPtr) dpr_radiosity_prim_create_data,
          (DtMethodPtr) dpr_radiosity_print_null_rtn,
          (DtMethodPtr) dpr_radiosity_prim_delete_data, -1},

    { "DoVarSimplePolygonMesh", -1,
          (DtMethodPtr) dpr_radiosity_prim_create_data,
          (DtMethodPtr) dpr_radiosity_print_null_rtn,
          (DtMethodPtr) dpr_radiosity_prim_delete_data, -1},

   { 0, NO_ID, 0, 0, 0, -1},
};

/* ====================================================================== */

DtFlag dpr_radiosity_geom_check_already_installed (void)
{
    DtInt i;
    /* returns true if at least one of the radiosity geometry classes
       is currently installed */

    for (i=0; dpe_rad_additional_data[i].name != 0; i++) {
         if (DsInqClassId(dpe_rad_additional_data[i].name)
             != NOT_INSTALLED) 
                   return(DcTrue);
    }

    return(DcFalse);
}

/* ====================================================================== */

void dpr_radiosity_request_additional_data (
    DtPtr class_name,
    DtInt class_id)
{
    DtInt i;

    /* request additional data for class if needed */

    for (i=0; dpe_rad_additional_data[i].name != 0; i++) {
        if (!strcmp(dpe_rad_additional_data[i].name, class_name)) {
            dpe_rad_additional_data[i].id = class_id;
            dpe_rad_additional_data[i].index =
                    DDclass_AddObjectData(class_id,
                            dpe_rad_additional_data[i].crt_rtn,
                            dpe_rad_additional_data[i].prt_rtn,
                            dpe_rad_additional_data[i].del_rtn);
            break;
        }
    }
}

/* ====================================================================== */

DtInt dpr_radiosity_get_additional_data_index (
    DtPtr class_name)
{
    DtInt i;
    DtInt index;

    index = -1;
    for (i=0; dpe_rad_additional_data[i].name != 0; i++) {
        if (!strcmp(dpe_rad_additional_data[i].name, class_name)) {
            index = dpe_rad_additional_data[i].index;
        }
    }

    if (index  < 0) {
        DDerror(ERR_RAD_NO_DATA_INDEX,
               "dpr_radiosity_get_additional_data_index",
                class_name);
    }

   return(index);
}

/* ====================================================================== */

DtInt dpr_radiosity_get_id_additional_data_index (
    DtInt class_id)
{
    DtInt i;
    DtInt index;

    index = -1;
    for (i=0; dpe_rad_additional_data[i].id != NO_ID; i++) {
        if (dpe_rad_additional_data[i].id ==  class_id) {
            index = dpe_rad_additional_data[i].index;
        }
    }

    if (index  < 0) {
        DDerror(ERR_RAD_NO_DATA_INDEX,
               "dpr_radiosity_get_id_additional_data_index",
                DcNullPtr);
    }

   return(index);

}

/* ====================================================================== */

DtPtr dpr_radiosity_prim_create_data (
    dot_object *object)
{
    dpt_rad_primitive_data *prim_data;

    prim_data = (dpt_rad_primitive_data *)DDspace_Allocate
                                (sizeof(dpt_rad_primitive_data));
    prim_data->usageCount = 0;

    prim_data->usageCur = 0;

    prim_data->patchRad = DcNullPtr;

    prim_data->patchUnshotRad = DcNullPtr;

    prim_data->patchIntElemColor = DcNullPtr;

    prim_data->patchElemColor = DcNullPtr;

    prim_data->patchArea = DcNullPtr;

    prim_data->vertexRad = DcNullPtr;

    prim_data->next = DcNullPtr;

    return((DtPtr)prim_data);
}

/* ====================================================================== */

void dpr_radiosity_print_null_rtn (
    dot_object *object,
    dpt_rad_primitive_data *data)
{
}

/* ====================================================================== */

void dpr_radiosity_prim_delete_data (
    dot_object *object,
    dpt_rad_primitive_data *data)
{
    if (data->patchRad != DcNullPtr)
        DDspace_Deallocate(data->patchRad);

    if (data->patchUnshotRad != DcNullPtr)
        DDspace_Deallocate(data->patchUnshotRad);

    if (data->patchIntElemColor != DcNullPtr)
        DDspace_Deallocate(data->patchIntElemColor);

    if (data->patchElemColor != DcNullPtr)
        DDspace_Deallocate(data->patchElemColor);

    if (data->patchArea != DcNullPtr)
        DDspace_Deallocate(data->patchArea);

    if (data->vertexRad != DcNullPtr)
        DDspace_Deallocate(data->vertexRad);

    DDspace_Deallocate(data);
}

/* ====================================================================== */
