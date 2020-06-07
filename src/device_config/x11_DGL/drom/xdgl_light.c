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
	int ddr_x11_DGL_drom_light (light_obj)
	static print_lights ()
	int ddr_x11_DGL_drom_initialize_specular_table ()
	DtFlag ddr_x11_DGL_drom_renormalize (normal)
	int ddr_x11_DGL_drom_set_att_lgtatn (c1, c2)
	int ddr_x11_DGL_drom_set_att_lgtclr (colormodel, color)
	int ddr_x11_DGL_drom_set_att_lgtint (intensity)
	int ddr_x11_DGL_drom_set_att_lgtspdang (total, delta)
	int ddr_x11_DGL_drom_set_att_lgtspdexp (exp)
	int ddr_x11_DGL_drom_set_att_lgtswi (count, lights)
	int ddr_x11_DGL_drom_set_att_lgttyp (type)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */

#define TABLE_SIZE 1000

ddr_x11_DGL_drom_light(light_obj)
     DtObject light_obj;
{
    XdReal lightcosine[4];
    XdReal lightpos[4];
    DtInt lightindex;
    DtInt count;
    XdReal *table;
    DtReal exp;
    DtReal total;
    DtReal delta;
    DtReal inner;
    DtInt i;
    DtReal theta;
    DtReal a, cosine;
    DtReal dor_math_pow();
    DtReal dor_math_acos();
    DtFlag zeroed;

    /*  Determine the direction to the light in the world
	coordinate system.  */

    /*
     * The light is defined to be located at (0,0,0) in the local coordinate 
     * system with the light direction along the -Z axis.
     * To compute the position and direction of the light in world coordinates
     * first the position is determined by transforming the point (0,0,0) by the
     * current transformation matrix. Second the point (0,0,-1), which lies 
     * along the light vector in local space is transformed by the current 
     * transformation matrix.  These two points in world space define the light
     * vector.  This light vector is the second point minus the first point.
     */

    XdMathHomoTrns(0.,0.,0.,1.,
		   dde_x11_DGL_drom_data.object_lcstowcsmat,
		   &lightpos[0],&lightpos[1],&lightpos[2],&lightpos[3]);

    if (lightpos[3] != 0.0) {
	lightpos[0] /= lightpos[3];
	lightpos[1] /= lightpos[3];
	lightpos[2] /= lightpos[3];
    }

    XdMathHomoTrns(0.,0.,-1.,1.,
		   dde_x11_DGL_drom_data.object_lcstowcsmat,
		   &lightcosine[0],&lightcosine[1],&lightcosine[2],&lightcosine[3]);

    if (lightcosine[3] != 0.0) {
	lightcosine[0] /= lightcosine[3];
	lightcosine[1] /= lightcosine[3];
	lightcosine[2] /= lightcosine[3];
    }

#ifdef DEBUG
    printf ("light position: (%g,%g,%g,%g)\n", lightpos[0], lightpos[1], 
	    lightpos[2],lightpos[3]);
#endif

    lightcosine[0] -= lightpos[0];
    lightcosine[1] -= lightpos[1];
    lightcosine[2] -= lightpos[2];

    XdMathFastRenormalize(lightcosine);

#ifdef DEBUG
    printf ("light direction: (%g,%g,%g)\n", lightcosine[0], lightcosine[1], 
	    lightcosine[2]);
#endif

    dde_x11_DGL_drom_curlgtdat->count++;

    if (dde_x11_DGL_drom_curlgtdat->count >
	dde_x11_DGL_drom_curlgtdat->max_count) {
	count = 5 + dde_x11_DGL_drom_curlgtdat->max_count;

	dde_x11_DGL_drom_curlgtdat->lights = (XdLightPtr)
		DDspace_Reallocate (dde_x11_DGL_drom_curlgtdat->lights,
				    count*sizeof(XdLight));
	dde_x11_DGL_drom_curlgtdat->light_objs = (DtObject *)
		DDspace_Reallocate (dde_x11_DGL_drom_curlgtdat->light_objs,
				    count*sizeof(DtObject));
	dde_x11_DGL_drom_curlgtdat->max_count = count;
    }

    lightindex = dde_x11_DGL_drom_curlgtdat->count - 1;

    dde_x11_DGL_drom_curlgtdat->light_objs[lightindex] = light_obj;

    dde_x11_DGL_drom_curlgtdat->lights[lightindex].type = 
	    dde_x11_DGL_drom_data.light_type;
    dde_x11_DGL_drom_curlgtdat->lights[lightindex].mask =
	    dde_x11_DGL_drom_data.light_mask;
    dde_x11_DGL_drom_curlgtdat->lights[lightindex].enabled = XdTrue;

    dde_x11_DGL_drom_curlgtdat->lights[lightindex].color.r = 
	    dde_x11_DGL_drom_data.light_intensity * 
		    dde_x11_DGL_drom_data.light_color.r;
    dde_x11_DGL_drom_curlgtdat->lights[lightindex].color.g = 
	    dde_x11_DGL_drom_data.light_intensity * 
		    dde_x11_DGL_drom_data.light_color.g;
    dde_x11_DGL_drom_curlgtdat->lights[lightindex].color.b = 
	    dde_x11_DGL_drom_data.light_intensity * 
		    dde_x11_DGL_drom_data.light_color.b;

    switch (dde_x11_DGL_drom_data.light_type) {
    case XdAmbientLight:
#ifdef DEBUG
	printf ("XdAmbientLight\n");
#endif
	break;

    case XdDirectionalLight:
#ifdef DEBUG
	printf ("XdDirectionalLight\n");
#endif
	dde_x11_DGL_drom_curlgtdat->lights[lightindex].direction.u =
		lightcosine[0];
	dde_x11_DGL_drom_curlgtdat->lights[lightindex].direction.v =
		lightcosine[1];
	dde_x11_DGL_drom_curlgtdat->lights[lightindex].direction.n =
		lightcosine[2];
	break;

    case XdPositionalLight:
#ifdef DEBUG
	printf ("XdPositionallLight\n");
#endif
	dde_x11_DGL_drom_curlgtdat->lights[lightindex].position.x =
		lightpos[0];
	dde_x11_DGL_drom_curlgtdat->lights[lightindex].position.y =
		lightpos[1];
	dde_x11_DGL_drom_curlgtdat->lights[lightindex].position.z =
		lightpos[2];

	dde_x11_DGL_drom_curlgtdat->lights[lightindex].direction.u =
		lightcosine[0];
	dde_x11_DGL_drom_curlgtdat->lights[lightindex].direction.v =
		lightcosine[1];
	dde_x11_DGL_drom_curlgtdat->lights[lightindex].direction.n =
		lightcosine[2];

	if (dde_x11_DGL_drom_data.light_mask & XdLightAtten) {
#ifdef DEBUG
	    printf ("Attenuation: %g %g\n",
		    dde_x11_DGL_drom_data.light_atten_c1,
		    dde_x11_DGL_drom_data.light_atten_c2);
#endif
	    dde_x11_DGL_drom_curlgtdat->lights[lightindex].atten_c1 =
		    dde_x11_DGL_drom_data.light_atten_c1;
	    dde_x11_DGL_drom_curlgtdat->lights[lightindex].atten_c2 =
		    dde_x11_DGL_drom_data.light_atten_c2;
	}

	if (dde_x11_DGL_drom_data.light_mask & XdLightDist) {
	 
	    table = (XdReal *)DDspace_Allocate (TABLE_SIZE*sizeof(XdReal));

	    exp = dde_x11_DGL_drom_data.light_spread_exponent;
	    total = dde_x11_DGL_drom_data.light_total_angle;
	    delta = dde_x11_DGL_drom_data.light_delta_angle;
	    inner = total - delta;

#ifdef DEBUG
	    printf ("Spotlight: exp %g, total %g, delta %g, inner %g\n",
		    exp, total, delta, inner);
#endif

	    for (i=0, zeroed = DcFalse; i<TABLE_SIZE; i++) {
		if (zeroed) {
		    table[i] = 0.0;
		    continue;
		}

		cosine = 1. - 2.*i/(TABLE_SIZE-1.);
		theta = dor_math_acos(cosine);

#ifdef DEBUG
		printf ("   table[%d = cos(%g <%g>)] = ", i, theta, theta*180./3.1415);
#endif
		if (theta >= total) {
#ifdef DEBUG
		    printf ("0.0\n");
#endif
		    zeroed = DcTrue;
		    table[i] = 0.0;
		} else {
		    table[i] = 1.0;
		    if (theta > inner) {
			table[i] -= (theta - inner)/delta;
		    }
		    a = dor_math_pow (cosine, exp);
		    a = a < 0.0 ? 0.0 : a;
#ifdef DEBUG
		    printf ("%g * %g\n", table[i], a);
#endif
		    table[i] *= a;
		}
	    }
	    dde_x11_DGL_drom_curlgtdat->lights[lightindex].distribution = table;
	    dde_x11_DGL_drom_curlgtdat->lights[lightindex].table_size = TABLE_SIZE;
	}

	break;

    default:
	printf("ddr_x11_DGL_drom_initialize_light:  unknown light type\n");
    }
}


/*
 ======================================================================
 */
static print_lights()
{
    DtInt i, j;
    XdLightPtr lights;
   
    printf ("curlgtdat 0x%x\n", dde_x11_DGL_drom_curlgtdat);

    lights = dde_x11_DGL_drom_curlgtdat->lights;

    printf ("\t# lights: %d  (%d)\n", dde_x11_DGL_drom_curlgtdat->count,
	    dde_x11_DGL_drom_curlgtdat->max_count);
    for (i=0; i<dde_x11_DGL_drom_curlgtdat->count; i++) {
	printf ("\tlight %d  (%s%s%s)", i, 
		lights[i].type == XdAmbientLight ? "Ambient" :
		lights[i].type == XdDirectionalLight ? "Directional" :
		lights[i].type == XdPositionalLight ? "Positional" :
		"Unknown",
		lights[i].mask & XdLightAtten ? " Attenuated" : "",
		lights[i].mask & XdLightDist ? " Distributed" : "");
	printf ("\t  %s\n", lights[i].enabled ? "On" : "Off");
	printf ("\t  color (%g %g %g)\n",
		lights[i].color.r, lights[i].color.g, lights[i].color.b);
	printf ("\t  dir   (%g %g %g)\n",
		lights[i].direction.u, lights[i].direction.v, lights[i].direction.n);
	printf ("\t  pos   (%g %g %g)\n",
		lights[i].position.x, lights[i].position.y, lights[i].position.z);
	if (lights[i].mask & XdLightAtten) {
	    printf ("\t  atten (%g %g)\n",
		    lights[i].atten_c1, lights[i].atten_c2);
	}
	if (lights[i].mask & XdLightDist) {
	    printf ("\t  tbl   %d\n", lights[i].table_size);
	    for (j=0; j < lights[i].table_size; j++) {
		printf ("\t     %d - %g\n", j, lights[i].distribution[j]);
	    }
	}
    }
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_initialize_specular_table()

{
    DtInt index;
    DtReal dor_math_pow();

    for (index=0;index<256;index++) {
	dde_x11_DGL_drom_data.specular_factor_table[index] =
		dor_math_pow(((double)index)/255.,140.);
    }
}
/*
 ======================================================================
 */

DtFlag
ddr_x11_DGL_drom_renormalize(normal)
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

ddr_x11_DGL_drom_set_att_lgtatn (c1, c2)
     DtReal c1;
     DtReal c2;
{
    dde_x11_DGL_drom_data.light_atten_c1 = c1;
    dde_x11_DGL_drom_data.light_atten_c2 = c2;
}
/*
 ======================================================================
 */

	/*  light color  */

ddr_x11_DGL_drom_set_att_lgtclr(colormodel, color)
DtColorModel colormodel;
DtReal color[];
{
    dde_x11_DGL_drom_data.light_color.r = color[0];
    dde_x11_DGL_drom_data.light_color.g = color[1];
    dde_x11_DGL_drom_data.light_color.b = color[2];
}
/*
 ======================================================================
 */

	/*  light intensity  */

ddr_x11_DGL_drom_set_att_lgtint(intensity)
     DtReal intensity;
{
    dde_x11_DGL_drom_data.light_intensity = intensity;
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_set_att_lgtspdang (total, delta)
     DtReal total;
     DtReal delta;
{
    dde_x11_DGL_drom_data.light_total_angle = total;
    dde_x11_DGL_drom_data.light_delta_angle = delta;
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_set_att_lgtspdexp (exp)
     DtReal exp;
{
    dde_x11_DGL_drom_data.light_spread_exponent = exp;
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_set_att_lgtswi (count, lights)
     DtInt count;
     DtObject *lights;
{
    DtInt i;
    DtInt lightidx;
   
    if (dde_x11_DGL_drom_pre_init) return;

    for (lightidx = 0; lightidx < dde_x11_DGL_drom_curlgtdat->count; lightidx++) {
	dde_x11_DGL_drom_curlgtdat->lights[lightidx].enabled = XdTrue;
	for (i=0; i<count; i++) {
	    if (dde_x11_DGL_drom_curlgtdat->light_objs[lightidx] == lights[i]) {
		dde_x11_DGL_drom_curlgtdat->lights[lightidx].enabled = XdFalse;
		break;
	    }
	}
#ifdef DEBUG
	printf ("light %d turned %d\n", lightidx,
		dde_x11_DGL_drom_curlgtdat->lights[lightidx].enabled);
#endif
    }
    XdSetLights(dde_x11_DGL_drom_curdevdat->pGC,
		dde_x11_DGL_drom_curlgtdat->count,
		dde_x11_DGL_drom_curlgtdat->lights);
}

/*
 ======================================================================
 */

	/*  light type  */

ddr_x11_DGL_drom_set_att_lgttyp(type)
     DtObject type;
{
#ifndef DORE_2.2_TITANG2
#ifndef __P3__
    static DtFlag pntlgterr = DcFalse;
    static DtFlag sptlgterr = DcFalse;
    static DtFlag attpntlgterr = DcFalse;
    static DtFlag attsptlgterr = DcFalse;
#endif
#endif

    if (type == DcLightInfinite) {
	dde_x11_DGL_drom_data.light_type = XdDirectionalLight;
	dde_x11_DGL_drom_data.light_mask = 0;
    } else if (type == DcLightAmbient) {
	dde_x11_DGL_drom_data.light_type = XdAmbientLight;
	dde_x11_DGL_drom_data.light_mask = 0;
#ifndef DORE_2.2_TITANG2
    } else if (type == DcLightPoint) {
#ifndef __P3__
	dde_x11_DGL_drom_data.light_type = XdAmbientLight;
	dde_x11_DGL_drom_data.light_mask = 0;
	if (!pntlgterr) {
	    DDerror (ERR_UNIMPLEMENTED_WARNING, "ddr_x11_DGL_drom_set_att_lgttyp",
		     "DcLightPoint being approximated with DcLightAmbient");
	    pntlgterr = DcTrue;
	}
#else
	dde_x11_DGL_drom_data.light_type = XdPositionalLight;
	dde_x11_DGL_drom_data.light_mask = 0;
#endif
    } else if (type == DcLightPointAttn) {
#ifndef __P3__
	dde_x11_DGL_drom_data.light_type = XdAmbientLight;
	dde_x11_DGL_drom_data.light_mask = 0;
	if (!attpntlgterr) {
	    DDerror (ERR_UNIMPLEMENTED_WARNING, "ddr_x11_DGL_drom_set_att_lgttyp",
		     "DcLightPointAttn being approximated with DcLightAmbient");
	    attpntlgterr = DcTrue;
	}
#else
	dde_x11_DGL_drom_data.light_type = XdPositionalLight;
	dde_x11_DGL_drom_data.light_mask = XdLightAtten;
#endif
    } else if (type == DcLightSpot) {
#ifndef __P3__
	dde_x11_DGL_drom_data.light_type = XdDirectionalLight;
	dde_x11_DGL_drom_data.light_mask = 0;
	if (!sptlgterr) {
	    DDerror (ERR_UNIMPLEMENTED_WARNING, "ddr_x11_DGL_drom_set_att_lgttyp",
		     "DcLightSpot being approximated with DcLightInfinite");
	    sptlgterr = DcTrue;
	}
#else
	dde_x11_DGL_drom_data.light_type = XdPositionalLight;
	dde_x11_DGL_drom_data.light_mask = XdLightDist;
#endif
    } else if (type == DcLightSpotAttn) {
#ifndef __P3__
	dde_x11_DGL_drom_data.light_type = XdDirectionalLight;
	dde_x11_DGL_drom_data.light_mask = 0;
	if (!attsptlgterr) {
	    DDerror (ERR_UNIMPLEMENTED_WARNING, "ddr_x11_DGL_drom_set_att_lgttyp",
		     "DcLightSpotAttn being approximated with DcLightInfinite");
	    attsptlgterr = DcTrue;
	}
#else
	dde_x11_DGL_drom_data.light_type = XdPositionalLight;
	dde_x11_DGL_drom_data.light_mask = XdLightDist | XdLightAtten;
#endif
#endif
    } else {
	dde_x11_DGL_drom_data.light_type = XdDirectionalLight;
	dde_x11_DGL_drom_data.light_mask = 0;
    }
}
