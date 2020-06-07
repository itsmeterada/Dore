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
	int ddr_sunview_cxp_drom_light ()
	int ddr_sunview_cxp_drom_initialize_specular_table ()
	int ddr_sunview_cxp_drom_update_lights ()
	int ddr_sunview_cxp_drom_update_lights_specular ()
	int ddr_sunview_cxp_drom_h_normal (viwdir,srftolgtnrm,hnrm)
	DtFlag ddr_sunview_cxp_drom_renormalize (normal)
	int ddr_sunview_cxp_drom_set_att_lgtatn (c1, c2)
	int ddr_sunview_cxp_drom_set_att_lgtclr (colormodel, color)
	int ddr_sunview_cxp_drom_set_att_lgtint (intensity)
	int ddr_sunview_cxp_drom_set_att_lgtspdang (total, delta)
	int ddr_sunview_cxp_drom_set_att_lgtspdexp (exp)
	int ddr_sunview_cxp_drom_set_att_lgtswi (count, lights)
	int ddr_sunview_cxp_drom_set_att_lgttyp (type)

  ======================================================================
 */
#include "drom.h"

extern DtFlag ddr_sunview_cxp_drom_renormalize();
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_light()
{
    DtReal lightcosine[3];
    DtInt lightindex;
    DtInt count;

    if (dde_sunview_cxp_drom_data.light_type == DcLightAmbient) {

	dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[0] += 
		dde_sunview_cxp_drom_data.light_intensity * 
			dde_sunview_cxp_drom_data.light_color[0];
	dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[1] += 
		dde_sunview_cxp_drom_data.light_intensity * 
			dde_sunview_cxp_drom_data.light_color[1];
	dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[2] += 
		dde_sunview_cxp_drom_data.light_intensity * 
			dde_sunview_cxp_drom_data.light_color[2];

    } else if (dde_sunview_cxp_drom_data.light_type == DcLightInfinite) {
	/*  Determine the direction to the light in the world
	    coordinate system.  */

	dor_math_vectrns(0.,0.,1.,
			  dde_sunview_cxp_drom_data.object_lcstowcsmat,
			  &lightcosine[0],&lightcosine[1],&lightcosine[2]);

	ddr_sunview_cxp_drom_renormalize(lightcosine);

	dde_sunview_cxp_drom_curlgtdat->count++;

	if (dde_sunview_cxp_drom_curlgtdat->count >
	    dde_sunview_cxp_drom_curlgtdat->max_count) {
	    count = 5 + dde_sunview_cxp_drom_curlgtdat->max_count;
	    if (dde_sunview_cxp_drom_curlgtdat->max_count == 0) {
		dde_sunview_cxp_drom_curlgtdat->cosine = (DtRealTriple *)
			DDspace_Allocate (count*sizeof(DtRealTriple));
		dde_sunview_cxp_drom_curlgtdat->intens = (DtReal *)
			DDspace_Allocate (count*sizeof(DtReal));
		dde_sunview_cxp_drom_curlgtdat->color = (DtColorRGB *)
			DDspace_Allocate (count*sizeof(DtColorRGB));
		dde_sunview_cxp_drom_curlgtdat->h = (DtRealTriple *)
			DDspace_Allocate (count*sizeof(DtRealTriple));
		dde_sunview_cxp_drom_curlgtdat->lcscosine = (DtRealTriple *)
			DDspace_Allocate (count*sizeof(DtRealTriple));
		dde_sunview_cxp_drom_curlgtdat->scaledcolor = (DtColorRGB *)
			DDspace_Allocate (count*sizeof(DtColorRGB));
	    } else {
		dde_sunview_cxp_drom_curlgtdat->cosine = (DtRealTriple *)
			DDspace_Reallocate (dde_sunview_cxp_drom_curlgtdat->cosine,
					    count*sizeof(DtRealTriple));
		dde_sunview_cxp_drom_curlgtdat->intens = (DtReal *)
			DDspace_Reallocate (dde_sunview_cxp_drom_curlgtdat->intens,
					    count*sizeof(DtReal));
		dde_sunview_cxp_drom_curlgtdat->color = (DtColorRGB *)
			DDspace_Reallocate (dde_sunview_cxp_drom_curlgtdat->color,
					    count*sizeof(DtColorRGB));
		dde_sunview_cxp_drom_curlgtdat->h = (DtRealTriple *)
			DDspace_Reallocate (dde_sunview_cxp_drom_curlgtdat->h,
					    count*sizeof(DtRealTriple));
		dde_sunview_cxp_drom_curlgtdat->lcscosine = (DtRealTriple *)
			DDspace_Reallocate (dde_sunview_cxp_drom_curlgtdat->lcscosine,
					    count*sizeof(DtRealTriple));
		dde_sunview_cxp_drom_curlgtdat->scaledcolor = (DtColorRGB *)
			DDspace_Reallocate (dde_sunview_cxp_drom_curlgtdat->scaledcolor,
					    count*sizeof(DtColorRGB));
	    }
	    dde_sunview_cxp_drom_curlgtdat->max_count = count;
	}

	lightindex = dde_sunview_cxp_drom_curlgtdat->count - 1;

	dde_sunview_cxp_drom_curlgtdat->cosine[lightindex][0] = lightcosine[0];
	dde_sunview_cxp_drom_curlgtdat->cosine[lightindex][1] = lightcosine[1];
	dde_sunview_cxp_drom_curlgtdat->cosine[lightindex][2] = lightcosine[2];

	dde_sunview_cxp_drom_curlgtdat->intens[lightindex] = 
		dde_sunview_cxp_drom_data.light_intensity;

	dde_sunview_cxp_drom_curlgtdat->color[lightindex][0] =
		dde_sunview_cxp_drom_data.light_color[0];
	dde_sunview_cxp_drom_curlgtdat->color[lightindex][1] =
		dde_sunview_cxp_drom_data.light_color[1];
	dde_sunview_cxp_drom_curlgtdat->color[lightindex][2] =
		dde_sunview_cxp_drom_data.light_color[2];

	dde_sunview_cxp_drom_curlgtdat->scaledcolor[lightindex][0] =
		dde_sunview_cxp_drom_data.light_intensity * 
			dde_sunview_cxp_drom_data.light_color[0];
	dde_sunview_cxp_drom_curlgtdat->scaledcolor[lightindex][1] =
		dde_sunview_cxp_drom_data.light_intensity * 
			dde_sunview_cxp_drom_data.light_color[1];
	dde_sunview_cxp_drom_curlgtdat->scaledcolor[lightindex][2] =
		dde_sunview_cxp_drom_data.light_intensity * 
			dde_sunview_cxp_drom_data.light_color[2];

    } else {
	printf("ddr_sunview_cxp_drom_initialize_light:  unknown light type\n");
	return;
    }
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_initialize_specular_table()

{
    DtInt index;
    DtReal dor_math_pow();

    for (index=0;index<256;index++) {
	dde_sunview_cxp_drom_data.specular_factor_table[index] =
		dor_math_pow(((double)index)/255.,140.);
    }
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_update_lights()

{
    DtInt lightno;
    DtRealTriple lcscamnrm;

    for (lightno=0;lightno<dde_sunview_cxp_drom_curlgtdat->count;lightno++) {
	dor_math_vectrns_fast(
			       dde_sunview_cxp_drom_curlgtdat->cosine[lightno],
			       dde_sunview_cxp_drom_data.nrmwcstolcsmat,
			       dde_sunview_cxp_drom_curlgtdat->lcscosine[lightno]);

	dor_math_renormalize_fast(dde_sunview_cxp_drom_curlgtdat->lcscosine[lightno]);
    }

    if (dde_sunview_cxp_drom_data.object_specular_switch) {
	dor_math_vectrns_fast(
			       dde_sunview_cxp_drom_curcamdat->wcscamnorm,
			       dde_sunview_cxp_drom_data.nrmwcstolcsmat,
			       lcscamnrm);

	for (lightno=0;lightno<dde_sunview_cxp_drom_curlgtdat->count;lightno++) {
	    ddr_sunview_cxp_drom_h_normal(lcscamnrm,
					   dde_sunview_cxp_drom_curlgtdat->lcscosine[lightno],
					   dde_sunview_cxp_drom_curlgtdat->h[lightno]);
	}
    }
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_update_lights_specular()

{
    DtInt lightno;
    DtRealTriple lcscamnrm;

    if (dde_sunview_cxp_drom_data.object_specular_switch) {
	dor_math_vectrns_fast(
			       dde_sunview_cxp_drom_curcamdat->wcscamnorm,
			       dde_sunview_cxp_drom_data.nrmwcstolcsmat,
			       lcscamnrm);

	for (lightno=0;lightno<dde_sunview_cxp_drom_curlgtdat->count;lightno++) {
	    ddr_sunview_cxp_drom_h_normal(lcscamnrm,
					   dde_sunview_cxp_drom_curlgtdat->lcscosine[lightno],
					   dde_sunview_cxp_drom_curlgtdat->h[lightno]);
	}
    }
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_h_normal(viwdir,srftolgtnrm,hnrm)
     DtReal viwdir[3];
     DtReal srftolgtnrm[3];
     DtReal hnrm[3];

{

    hnrm[0] = srftolgtnrm[0] + viwdir[0];
    hnrm[1] = srftolgtnrm[1] + viwdir[1];
    hnrm[2] = srftolgtnrm[2] + viwdir[2];

    ddr_sunview_cxp_drom_renormalize(hnrm);
}
/*
 ======================================================================
 */

DtFlag
ddr_sunview_cxp_drom_renormalize(normal)
     DtReal normal[3];

{
    DtReal length;
    DtReal dor_math_sqrt();

    length = normal[0]*normal[0]+normal[1]*normal[1]+normal[2]*normal[2];

    if (length == 0.) {
	normal[0] = 0.;
	normal[1] = 0.;
	normal[2] = 0.;

	return(DcFalse);
    }

    if (length == 1.) {
	return(DcTrue);
    }

    length = 1. / dor_math_sqrt(length);

    normal[0] *= length;
    normal[1] *= length;
    normal[2] *= length;

    return(DcTrue);
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_set_att_lgtatn (c1, c2)
     DtReal c1;
     DtReal c2;
{
}
/*
 ======================================================================
 */

	/*  light color  */

ddr_sunview_cxp_drom_set_att_lgtclr(colormodel, color)
DtColorModel colormodel;
DtReal color[];
{
    dde_sunview_cxp_drom_data.light_color[0] = color[0];
    dde_sunview_cxp_drom_data.light_color[1] = color[1];
    dde_sunview_cxp_drom_data.light_color[2] = color[2];
}
/*
 ======================================================================
 */

	/*  light intensity  */

ddr_sunview_cxp_drom_set_att_lgtint(intensity)
     DtReal intensity;
{
    dde_sunview_cxp_drom_data.light_intensity = intensity;
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_set_att_lgtspdang (total, delta)
     DtReal total;
     DtReal delta;
{
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_set_att_lgtspdexp (exp)
     DtReal exp;
{
}

/*
 ======================================================================
 */

ddr_sunview_cxp_drom_set_att_lgtswi (count, lights)
     DtInt count;
     DtObject *lights;
{
}

/*
 ======================================================================
 */

	/*  light type  */

ddr_sunview_cxp_drom_set_att_lgttyp(type)
     DtObject type;
{
    dde_sunview_cxp_drom_data.light_type = type;
}
