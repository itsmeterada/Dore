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

/*
 ======================================================================
 Functions:

   dpr_radiosity_color_int_to_normvec
   dpr_radiosity_compute_next_color
   dpr_radiosity_compute_triangle_area
   dpr_radiosity_compute_polygon_area
   dpr_radiosity_compute_center_and_normal
   dpr_radiosity_copy_matrix
   dpr_radiosity_transform_vertices
   dpr_radiosity_get_next_set_additional_data
   dpr_radiosity_get_current_set_additional_data

 ======================================================================
 */

#define MAX_RANGE 16384.0  /* this is (2^15)/2 or 32768/2 */

/*
 ======================================================================
 */

static long randx=4321;

void dpr_radiosity_change_seed (
    unsigned x)
{
        randx = x;
}

/*
 ======================================================================
 */

/* returns a DtReal between  -1.0 and 1.0 */

DtReal dpr_radiosity_random (void)
{
   /* randx returns an integer between 0 and 32767 */
   randx = ((randx * 1103515245L + 12345)>>16) & 0x7fff;
   return((randx-MAX_RANGE)/MAX_RANGE);
}


/*
 ======================================================================
 */

/* separates a 32 int into r, g, and b values */

void dpr_radiosity_color_int_to_normvec (
    DtInt integer_color,
    DtReal normvec_color[3])
{
    /* extract red, green and blue */
    normvec_color[0] = (((integer_color)      & 0xff)/255.);
    normvec_color[1] = (((integer_color >>8 ) & 0xff)/255.);
    normvec_color[2] = (((integer_color >>16) & 0xff)/255.);

}

/*
 ======================================================================
 */

void dpr_radiosity_compute_next_color (
    DtInt *next_integer_color,
    DtReal next_normvec_color[3])
{
    char errstring[128];

    dpr_radiosity_color_int_to_normvec
          (dpe_rad_hcube_data.next_integer_color, next_normvec_color);

    *next_integer_color = dpe_rad_hcube_data.next_integer_color;

    dpe_rad_hcube_data.next_integer_color++;

    if (dpe_rad_hcube_data.next_integer_color > MAXIMUM_COLOR) {
       sprintf(errstring, "Maximum is %d", MAXIMUM_COLOR);
       DDerror(ERR_RAD_CAP_EXCEEDED, "dpr_radiosity_compute_next_color",
               errstring);
    }
}

/*
 ======================================================================
 */

DtReal dpr_radiosity_compute_triangle_area (
    DtRealTriple v0,
    DtRealTriple v1,
    DtRealTriple v2)
{
   DtReal length;
   DtRealTriple crossp0, crossp1, crossp2;
   DtRealTriple partialsum, sum;

   /* Compute area of 3D triangle 
      Source : Graphics Gems I p.20 */

   CROSS_VEC(v0, v1, crossp0);
   CROSS_VEC(v1, v2, crossp1);
   CROSS_VEC(v2, v0, crossp2);

   ADD_VEC(crossp0, crossp1, partialsum);
   ADD_VEC(partialsum, crossp2, sum);

   LENGTH_VEC(sum, length);

   return (length/2.0);
    
}

/*
 ======================================================================
 */

DtReal dpr_radiosity_compute_polygon_area (
    DtRealTriple vertices[],
    DtInt n)
{
   DtRealTriple plynormal, u, v;
   DtRealTriple crossp, sum;
   DtReal length;
   DtReal dotp;
   DtInt i;

   /* Compute area of arbitrary polygon
      Source : Graphics Gems II p.170 */

   if (n < 3) 
       DDerror(ERR_RAD_TOO_FEW_VERT,
          "dpr_radiosity_compute_polygon_area", DcNullPtr);

   SUB_VEC(vertices[1], vertices[0], u);
   SUB_VEC(vertices[2], vertices[0], v);

   CROSS_VEC(u, v, plynormal);

   /* make sure the normal is unit */
   NORMALIZE_VEC(plynormal, length);

  if (length == 0.) 
      DDerror(ERR_ZERO_LEN_VEC,
           "dpr_radiosity_compute_polygon_area", DcNullPtr);

   sum[0] = sum[1] = sum[2] = 0.0;

   /* it is important that "vertices" be DtRealTriple and not DtReal */

   for (i=0; i<n-1; i++) {
      CROSS_VEC(vertices[i], vertices[i+1], crossp);
      ADD_VEC(crossp, sum, sum);
   }

   CROSS_VEC(vertices[n-1], vertices[0], crossp);
   ADD_VEC(crossp, sum, sum);

   LENGTH_VEC(sum, length);

   dotp = DOT_PRODUCT(plynormal, sum);

   if (dotp < 0)
       dotp = 0 - dotp;

   return (dotp/2.0);

}


/*
 ======================================================================
 */

/* compute center of mass of polygon and also it's normal (non-unit)*/

void dpr_radiosity_compute_center_and_normal (
    DtRealTriple vertices[],
    DtInt n,
    DtRealTriple center,
    DtRealTriple normal)
{
   DtInt i;
   DtRealTriple sum, u, v;
   DtReal length;

   if (n < 3) 
       DDerror(ERR_RAD_TOO_FEW_VERT,
          "dpr_radiosity_compute_center_and_normal", DcNullPtr);

   sum[0] = vertices[0][0];
   sum[1] = vertices[0][1];
   sum[2] = vertices[0][2];

   for (i=1; i<n; i++) 
      ADD_VEC(vertices[i], sum, sum);

  center[0] = sum[0] / n;
  center[1] = sum[1] / n;
  center[2] = sum[2] / n;

  /* use first three vertices to get normal */

  SUB_VEC(vertices[1], vertices[0], u);
  SUB_VEC(vertices[2], vertices[0], v);

  CROSS_VEC(u, v, normal);

  NORMALIZE_VEC(normal, length);

  if (length == 0.) 
      DDerror(ERR_ZERO_LEN_VEC,
           "dpr_radiosity_compute_center_and_normal", DcNullPtr);

}

/*
 ======================================================================
 */

/* transform array of vertices using a given transform matrix */

void 
dpr_radiosity_transform_vertices (
    DtRealTriple *vertices,     /* each vertex is x, y, z */
    DtInt n,                    /* number of vertices to transform */
    DtMatrix4x4 matrix,
    DtRealTriple *transf_vertices
)
{
    DtInt i;

    for (i=0; i<n; i++) {

       /* compute new x */
       transf_vertices[i][0] = 

              vertices[i][0] * matrix[0][0] +
              vertices[i][1] * matrix[0][1] +
              vertices[i][2] * matrix[0][2] +
                               matrix[0][3];
       
       /* compute new y */
       transf_vertices[i][1] = 

              vertices[i][0] * matrix[1][0] +
              vertices[i][1] * matrix[1][1] +
              vertices[i][2] * matrix[1][2] +
                               matrix[1][3];
       
       /* compute new z */
       transf_vertices[i][2] = 

              vertices[i][0] * matrix[2][0] +
              vertices[i][1] * matrix[2][1] +
              vertices[i][2] * matrix[2][2] +
                               matrix[2][3];
   }
}

/*
 ======================================================================
 */

/* when the same object handle is instanced multiple times, we need
   to keep multiple sets of additional data for the object. This
   routine is called when a new data set is requested - if this
   is the first time, we just return since the object has already
   one set when it is created by Dore */

dpt_rad_primitive_data *dpr_radiosity_get_next_set_additional_data (
    dpt_rad_primitive_data *head_add_data)
{
    dpt_rad_primitive_data *last_add_data, *add_data;

    if (head_add_data->usageCount != 0) {
        add_data = (dpt_rad_primitive_data *)
                   dpr_radiosity_prim_create_data(DcNullObject);
        last_add_data = head_add_data;
        while (last_add_data->next != DcNullPtr)
              last_add_data = last_add_data->next;
        last_add_data->next = add_data;
    } else {

        add_data = head_add_data;
    }
    head_add_data->usageCount += 1;

    if (add_data == DcNullPtr)
        DDerror(ERR_RAD_BAD_ADD_DATASET,
                "dpr_radiosity_get_next_set_additional_data",
                "Next additional data set is NULL");

    return(add_data);
}

/*
 ======================================================================
 */

/* when the same object handle is instanced multiple times, we need
   to keep multiple sets of additional data for the object. This
   routine is called when a traversal uses the data sets one after
   the other. It returns the "current" data set and increments the
   current set count for the next request */

dpt_rad_primitive_data *dpr_radiosity_get_current_set_additional_data (
    dpt_rad_primitive_data *head_add_data)
{
    DtInt i;
    dpt_rad_primitive_data *cur_add_data;

    if (head_add_data->usageCur != 0) {
        cur_add_data = head_add_data;
        for (i=0; i<head_add_data->usageCur; i++)
              cur_add_data = cur_add_data->next;
    } else {

        cur_add_data = head_add_data;
    }

    if (head_add_data->usageCur >= head_add_data->usageCount ||
        cur_add_data == DcNullPtr) {
        if (cur_add_data == DcNullPtr)
           DDerror(ERR_RAD_BAD_ADD_DATASET,
                   "dpr_radiosity_get_current_set_additional_data",
                   "Current additional data set is NULL");
        if (head_add_data->usageCur >= head_add_data->usageCount)
           DDerror(ERR_RAD_BAD_ADD_DATASET,
                   "dpr_radiosity_get_current_set_additional_data",
                   "data->usageCur >= data->usageCount");
    }

    if (cur_add_data->next == DcNullPtr) 
        /* reset current pointer for next traversal */
        head_add_data->usageCur = 0;
    else
        head_add_data->usageCur += 1;

    return(cur_add_data);
}

/*
 ======================================================================
 */

void dpr_radiosity_copy_matrix (
    DtMatrix4x4 matrix_from,
    DtMatrix4x4 matrix_to)
{
    DtInt i,j;

    for (i=0; i<4; i++) 
        for (j=0; j<4; j++) 
            matrix_to[i][j] = matrix_from[i][j];
}

/*
 ======================================================================
 */

