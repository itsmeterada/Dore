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
 
/*
  ======================================================================
  Functions:
	drr_glbshd_initialize_subsystem
	drr_glbshd_initialize_render
	drr_glbshd_initialize_before_environment
	drr_glbshd_initialize_after_environment
	drr_glbshd_initialize_camera
	drr_glbshd_initialize_light
	drr_glbshd_create_object_data

  ======================================================================
 */
#include "glbshd.h"
#include <dore/internal/glbatt.h>

struct glbshd_static_data dre_glbshd_static_data;
struct glbshd_light_data dre_glbshd_light_data;
struct glbshd_object_data *dre_glbshd_object_data;
struct glbshd_surface_data *dre_glbshd_surface_data;

/* ====================================================================== */

void drr_glbshd_initialize_subsystem (void)
{
    dre_glbshd_static_data.max_lightcount = 0;
    dre_glbshd_light_data.cosine = NULL;
    dre_glbshd_light_data.intens = NULL;
    dre_glbshd_light_data.color = NULL;
    dre_glbshd_light_data.h = NULL;
}



/* ====================================================================== */

void drr_glbshd_initialize_render (
    DtColorModel colormodel,
    DtColorRGB   background_color)
{
    dre_glbshd_static_data.background_color[0] = background_color[0];
    dre_glbshd_static_data.background_color[1] = background_color[1];
    dre_glbshd_static_data.background_color[2] = background_color[2];

    dre_glbshd_static_data.raylvl = 1;
}



/* ====================================================================== */

void drr_glbshd_initialize_before_environment (void)
{
    dre_glbshd_static_data.lightcount = 0;
    dre_glbshd_light_data.ambient_reflectance[0] = 0.0;
    dre_glbshd_light_data.ambient_reflectance[1] = 0.0;
    dre_glbshd_light_data.ambient_reflectance[2] = 0.0;
}



/* ====================================================================== */

void drr_glbshd_initialize_after_environment (void)
{
}



/* ====================================================================== */

void drr_glbshd_initialize_camera (void)
{
    dor_global_glbrndraylvl_get_value(&(dre_glbshd_static_data.maxraylvl));
}



/* ====================================================================== */

void drr_glbshd_initialize_light (dot_object *object)
{
    DtMatrix4x4 lcstowcsmat;
    DtReal lightcosine[3];
    DtReal lightint;
    DtColorRGB lightclr;
    DtInt lightindex;
    DtObject lighttype;
    DtInt count;
    DtColorModel colormodel;

    /*  Determine the direction to the light in the world
	coordinate system.  */

    dor_global_lcstowcsmat_get_value(lcstowcsmat);
    dor_global_lgttyp_get_value(&lighttype);
    dor_global_lgtint_get_value(&lightint);
    dor_global_lgtclr_get_value(&colormodel, lightclr);

    if (lighttype == DcLightAmbient) {
	dre_glbshd_light_data.ambient_reflectance[0] += 
						lightint*lightclr[0];
	dre_glbshd_light_data.ambient_reflectance[1] += 
						lightint*lightclr[1];
	dre_glbshd_light_data.ambient_reflectance[2] += 
						lightint*lightclr[2];

    } else if (lighttype == DcLightInfinite) {

	dor_math_vectrns(
		0.,0.,1.,
		lcstowcsmat,
		&lightcosine[0],&lightcosine[1],&lightcosine[2]);

	dor_math_renormalize_fast(lightcosine);

	dre_glbshd_static_data.lightcount++;

	if (dre_glbshd_static_data.lightcount >
	    dre_glbshd_static_data.max_lightcount) {
	    count = 5 + dre_glbshd_static_data.max_lightcount;

	    if (dre_glbshd_static_data.max_lightcount == 0) {
		dre_glbshd_light_data.cosine = (DtRealTriple *)
			DDspace_Allocate (count*sizeof(DtRealTriple));
		dre_glbshd_light_data.intens = (DtReal *)
			DDspace_Allocate (count*sizeof(DtReal));
		dre_glbshd_light_data.color = (DtColorRGB *)
			DDspace_Allocate (count*sizeof(DtColorRGB));
		dre_glbshd_light_data.h = (DtRealTriple *)
			DDspace_Allocate (count*sizeof(DtRealTriple));

	    } else {
		dre_glbshd_light_data.cosine = (DtRealTriple *)
			DDspace_Reallocate (dre_glbshd_light_data.cosine,
					    count*sizeof(DtRealTriple));
		dre_glbshd_light_data.intens = (DtReal *)
			DDspace_Reallocate (dre_glbshd_light_data.intens,
					    count*sizeof(DtReal));
		dre_glbshd_light_data.color = (DtColorRGB *)
			DDspace_Reallocate (dre_glbshd_light_data.color,
					    count*sizeof(DtColorRGB));
		dre_glbshd_light_data.h = (DtRealTriple *)
			DDspace_Reallocate (dre_glbshd_light_data.h,
					    count*sizeof(DtRealTriple));
	    }
	    dre_glbshd_static_data.max_lightcount = count;
	}
	   
	lightindex = dre_glbshd_static_data.lightcount - 1;

	dre_glbshd_light_data.cosine[lightindex][0] = lightcosine[0];
	dre_glbshd_light_data.cosine[lightindex][1] = lightcosine[1];
	dre_glbshd_light_data.cosine[lightindex][2] = lightcosine[2];

	dre_glbshd_light_data.intens[lightindex] = lightint;

	dre_glbshd_light_data.color[lightindex][0] = lightclr[0];
	dre_glbshd_light_data.color[lightindex][1] = lightclr[1];
	dre_glbshd_light_data.color[lightindex][2] = lightclr[2];
    }
}



/* ====================================================================== */

struct glbshd_object_data * drr_glbshd_create_object_data (void)
{
    DtColorModel colormodel;
    struct glbshd_object_data *data;

    data = (struct glbshd_object_data *)
		DDspace_Allocate(sizeof *data);

    dor_global_ambswi_get_value(&(data->ambswi));
    dor_global_difswi_get_value(&(data->difswi));
    dor_global_spcswi_get_value(&(data->spcswi));
    dor_global_transpswi_get_value(&(data->transpswi));
    dor_global_shadowswi_get_value(&(data->shadowswi));
    dor_global_reflecswi_get_value(&(data->reflectswi));

    if (data->ambswi){
	dor_global_ambint_get_value(&(data->ambint));
    }

    if (data->difswi){
	dor_global_difclr_get_value(&colormodel, data->difclr);

	dor_global_difint_get_value(&(data->difint));
    }

    if (data->spcswi || data->reflectswi){
	dor_global_spcclr_get_value(&colormodel, data->spcclr);

	dor_global_spcint_get_value(&(data->spcint));

	dor_global_spcfct_get_value(&(data->spcfct));
    }

    if (data->transpswi){
	dor_global_transpclr_get_value(&colormodel, data->transpclr);

	dor_global_transpint_get_value(&(data->transpint));
    }

    return(data);
}
