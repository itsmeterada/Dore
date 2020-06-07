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

/*
 ======================================================================
 Functions:
   dpr_radiosity_hcube_distrib_method_initialize
   dpr_radiosity_hcube_distrib_init_execution
   dpr_radiosity_hcube_distrib_update_form_factors
   dpr_radiosity_hcube_distrib_execute
   dpr_radiosity_compute_top_delta_form_factors
   dpr_radiosity_compute_side_delta_form_factors
   dpr_radiosty_update_form_factors

 ======================================================================
 */

#define INDEX_BUFFER(n)   ((n)<half_res? n : (xres-n-1))


dpt_rad_method_rtn_list dpe_rad_hcube_distrib_rtn[] = {
    { "DoTriangleMesh",
		    (DtMethodPtr) dpr_radiosity_trimsh_hcube_distrib },
    { "DoTriangleList",
		    (DtMethodPtr) dpr_radiosity_trilst_hcube_distrib },
    { "DoTriangleStrip",
		    (DtMethodPtr) dpr_radiosity_tristrip_hcube_distrib },
    { "DoSimplePolygon",
		    (DtMethodPtr) dpr_radiosity_simply_hcube_distrib },
    { "DoSimplePolygonMesh",
		    (DtMethodPtr) dpr_radiosity_simplymsh_hcube_distrib },
    { "DoVarTriangleMesh",
		    (DtMethodPtr) dpr_radiosity_vartrimsh_hcube_distrib },
    { "DoVarTriangleStrip",
		    (DtMethodPtr) dpr_radiosity_vartristrip_hcube_distrib },
    { "DoVarSimplePolygonMesh",
		    (DtMethodPtr) dpr_radiosity_varsimplymsh_hcube_distrib },
    { 0, 0 }
};

DtReal  *dpe_rad_form_factors = DcNullPtr;

static DtReal  *dpe_rad_top_delta_ff = DcNullPtr;
static DtReal  *dpe_rad_side_delta_ff = DcNullPtr;

static DtInt count;

/*
 ======================================================================
 */

DtInt dpr_radiosity_hcube_distrib_method_initialize (void)
{
    DtInt i;
    DtInt class_id, method_id;

    /* install method to distribute the radiosity to patches by
       processing one buffer obtained from DdInqPixelData */

    method_id = DDclass_CopyMethod(DsInqMethodId("StdRenderDisplay"),
                "HcubeDistributeRadiosity");

    for (i=0; dpe_rad_hcube_distrib_rtn[i].name != 0; i++) {
        if ((class_id = DsInqClassId(dpe_rad_hcube_distrib_rtn[i].name))
            != NOT_INSTALLED) {
                DDclass_SetMethod(class_id, method_id,
                    dpe_rad_hcube_distrib_rtn[i].routine);
        }
    }

    return(method_id);

}
/*
 ======================================================================
 */

void dpr_radiosity_hcube_distrib_new_class (
    DtPtr class_name,
    DtInt class_id)
{
    DtInt i;

    for (i=0; dpe_rad_hcube_distrib_rtn[i].name != 0; i++) {
        if (!strcmp(dpe_rad_hcube_distrib_rtn[i].name, class_name)) {
            DDclass_SetMethod(class_id,
                    dpe_rad_hcube_data.hcube_distrib_method_id,
                    dpe_rad_hcube_distrib_rtn[i].routine);
            break;
        }
    }

}

/*
 ======================================================================
 */

void dpr_radiosity_hcube_distrib_init_execution (void)
{
    static DtInt previous_res = 0;
    static DtInt previous_num_patches = 0;
    DtInt i, size, half_res;
    
    if (dpe_rad_hcube_data.next_integer_color != previous_num_patches) {

        /* the current color indicates how many patches there are
           in the system at the moment */

        dpe_rad_form_factors = (DtReal *) DDspace_Reallocate
                               (dpe_rad_form_factors,
                                dpe_rad_hcube_data.next_integer_color*
                                sizeof(DtReal));

       previous_num_patches = dpe_rad_hcube_data.next_integer_color;
    }
 
    /* initialize form factors to zero */
    for (i=0; i<dpe_rad_hcube_data.next_integer_color; i++)  
        dpe_rad_form_factors[i] = 0.0;

    if (dpe_rad_hcube_data.resolution != previous_res) {

        /* because of symmetry, we can get away with computing and
           storing ffactors for 1/4 of a face   */

        size = dpe_rad_hcube_data.resolution *
               dpe_rad_hcube_data.resolution / 4;

        dpe_rad_side_delta_ff = (DtReal *) DDspace_Reallocate
                                (dpe_rad_side_delta_ff,
                                size*sizeof(DtReal));

        dpe_rad_top_delta_ff = (DtReal *) DDspace_Reallocate
                               (dpe_rad_top_delta_ff,
                                size*sizeof(DtReal));

        half_res = dpe_rad_hcube_data.resolution / 2.0;

        dpr_radiosity_compute_top_delta_form_factors(half_res);
        dpr_radiosity_compute_side_delta_form_factors(half_res);
    
        previous_res = dpe_rad_hcube_data.resolution;
    }
           
}

/*
 ======================================================================
 */

void dpr_radiosity_hcube_distrib_update_form_factors (
    DtInt face,
    unsigned char *imgbuffer)
{
    DtInt res;

    /* process image buffer - distribute radiosity to patches */

    res = dpe_rad_hcube_data.resolution;

    /* increment the form factors using the latest buffer */
    if (face == 0)
        /* top view */
        dpr_radiosty_update_form_factors(imgbuffer, res, res,
                                         dpe_rad_top_delta_ff);
    else
        /* one of the four side views */
        dpr_radiosty_update_form_factors(imgbuffer, res, (res / 2),
                                         dpe_rad_side_delta_ff);
}
/*
 ======================================================================
 */

void dpr_radiosity_hcube_distrib_execute (void)
{
    DtMethodPtr executemethod;

    /* initiate traversal on the hcube display group */

    doe_system_current_method = dpe_rad_hcube_data.hcube_distrib_method_id;

    executemethod =
        dor_object_get_current_method(dpe_rad_hcube_data.display_group);

    (*executemethod)(dpe_rad_hcube_data.display_group);

    doe_system_current_method = DcMethodNull;
}

/*
 ======================================================================
 */

/* compute the delta form factors for the top of the hemicube */

void dpr_radiosity_compute_top_delta_form_factors (
    DtInt int_half_res)
{

    DtInt j,k;
    DtReal x, x_square ,y, y_square, sum, sum_square;
    DtReal factor;
    DtReal *delta_ff_ptr;
    DtReal half_res;

    DtReal totalsum;
     totalsum = 0.0;

    half_res = (DtReal) int_half_res;

    /* Create delta form-factors for the top face of hemi-cube 
       Need to compute only 1/4 of the form-factors because of the
       4-way symmetry */

    delta_ff_ptr = dpe_rad_top_delta_ff;
    count = 0;

    factor = 1.0 / (M_PI * half_res * half_res);

    for (j=0; j<int_half_res; j++) {
        y = (half_res - (j+0.5)) / half_res;
        y_square  = y*y;

        for (k=0 ; k<int_half_res ; k++ ) {
            x = (half_res - (k + 0.5)) / half_res;
            x_square = x*x;
            sum =  x_square + y_square + 1.0 ;
            sum_square = sum*sum;
            *delta_ff_ptr = factor / sum_square ;
            totalsum += *delta_ff_ptr;
            delta_ff_ptr++;
            count++;
        }
    }
}

/*
 ======================================================================
 */

/* compute the delta form factors for the sides of the hemicube */

void dpr_radiosity_compute_side_delta_form_factors (
    DtInt int_half_res)
{
    DtInt j,k;
    DtReal x, x_square, y, y_square, sum, sum_square;
    DtReal factor;
    DtReal *delta_ff_ptr;
    DtReal half_res;

    DtReal totalsum;
    totalsum = 0.0;

    half_res = (DtReal) int_half_res;

    delta_ff_ptr = dpe_rad_side_delta_ff;
    count = 0;

    factor = 1.0/(M_PI * half_res * half_res);

    /* Create delta form-factors for the side faces of hemi-cube 
       Need to compute only 1/2 of the form-factors because of the
       2-way symmetry (remeber it is a HEMI cube, so resolution
       on the sides is half what it is on the top) */

    for (j=0; j<int_half_res; j++) {
        y = (half_res - (j+0.5)) / half_res;
        y_square = y*y;

        for (k=0 ; k<int_half_res ; k++ ) {
            x = (half_res - (k + 0.5)) / half_res;
            x_square = x*x;
            sum =  x_square + y_square + 1.0 ;
            sum_square = sum*sum;
            *delta_ff_ptr = factor * y / sum_square;
            totalsum += *delta_ff_ptr;
            delta_ff_ptr++;
            count++;
        }
    }

}

/*
 ======================================================================
 */

void dpr_radiosty_update_form_factors (
    unsigned char *imgbuffer,
    DtInt xres,
    DtInt yres,
    DtReal *delta_ff)
{
    DtInt i, j, half_res;
    DtInt idx1, idx2;
    DtInt int_elem_color;
    unsigned char *buff_ptr;

   /* xres is the width of the buffer that we want to process
      (should be full buffer width). yres is the height of the
      buffer that we want to process: should be full height
      for top factors, and half of it for side factors:

            image plane
                 |
                 v

        +--------+     < = eye position
        |      / |      
        |     /  |
        |    /   |
      ------<--------  patch surface
             \   : 
              \  :   <- because viewing frustum = 90o, we also get 
               \ :      this unneeded half of the image

   */

    buff_ptr = imgbuffer;

    half_res = xres/2;

    /* note: INDEX_BUFFER assumes form factor matrix is same size in x and y */
    for (j=0; j<xres; j++) {
         idx1 = INDEX_BUFFER(j) * half_res;
         for (i=0; i<yres; i++) {

              /* reconstitute single color index */
              int_elem_color =
                    ((int)buff_ptr[0]) |
                    ((int)buff_ptr[1]<<8) |
                    ((int)buff_ptr[2]<<16);

              if (int_elem_color != dpe_rad_int_backg_col) {

                  if (int_elem_color <=0 || int_elem_color >
                      dpe_rad_hcube_data.next_integer_color) {
                      DDerror(ERR_RAD_BAD_PIXELVALUE, 
                         "dpr_radiosty_update_form_factors",
                          DcNullPtr);
                  } else {
                      idx2 = INDEX_BUFFER(i);
                      dpe_rad_form_factors[int_elem_color] += delta_ff[idx1+idx2];
                  }
              }
              /* skip to next pixel value */
              buff_ptr += dpe_rad_pixel_offset;
         }
    }

}

/*
 ======================================================================
 */
