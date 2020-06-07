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
	ddr_gl_drom_light
	ddr_gl_drom_set_att_lgtatn
	ddr_gl_drom_set_att_lgtclr
	ddr_gl_drom_set_att_lgtint
	ddr_gl_drom_set_att_lgtspdang
	ddr_gl_drom_set_att_lgtspdexp
	ddr_gl_drom_set_att_lgtswi
	ddr_gl_drom_set_att_lgttyp
	ddr_gl_drom_update_display_light
	ddr_gl_drom_update_display_lights
	ddr_gl_drom_reset_lights

  ======================================================================
 */
#include "../gl_driver.h"
#include <math.h>

/* ====================================================================== */

void ddr_gl_drom_light (DtObject light)
{
    light_data *lt;

#   ifdef DEBUGP
	printf ("Init_light() called\n");
#   endif

    if (DevStatus.view_data->numlights >= MAXLIGHTS) {
#       ifdef DEBUGP
	    printf ("WARNING!!! More than %d lights (too many) declared\n",
		MAXLIGHTS);
#       endif
	return;
    }

    lt = &(DevStatus.view_data->light[DevStatus.view_data->numlights++]);

    mmode(MVIEWING);
    getmatrix(lt->light_to_world_matrix);

    lt->light = light;
    lt->enabled = DcTrue;
    lt->type = DevStatus.lighttype;

    lt->position[0] = 0.;
    lt->position[1] = 0.;
    lt->position[2] = 0.;

    lt->direction[0] = 0.;
    lt->direction[1] = 0.;
    lt->direction[2] = -1.;

    lt->color[0] = DevStatus.lightcolor[0]*DevStatus.lightintens;
    lt->color[1] = DevStatus.lightcolor[1]*DevStatus.lightintens;
    lt->color[2] = DevStatus.lightcolor[2]*DevStatus.lightintens;

    lt->spread_angle_total = DevStatus.light_spread_angle_total;
    lt->spread_angle_delta = DevStatus.light_spread_angle_delta;
    lt->spread_exponent    = DevStatus.light_spread_exponent;

    lt->att_c1    = DevStatus.light_att_c1;
    lt->att_c2    = DevStatus.light_att_c2;
}



/* ====================================================================== */

void ddr_gl_drom_set_att_lgtatn (
    DtReal c1,
    DtReal c2)
{
    DevStatus.light_att_c1 = c1;
    DevStatus.light_att_c2 = c2;
}



/* ====================================================================== */

void ddr_gl_drom_set_att_lgtclr (
    DtColorModel colormodel,
    DtReal color[])
{
#   ifdef DEBUGP
	printf("Set_light_color(%f,%f,%f) called\n",color[0],color[1],color[2]);
#   endif

    DevStatus.lightcolor[0] = color[0];
    DevStatus.lightcolor[1] = color[1];
    DevStatus.lightcolor[2] = color[2];
}



/* ====================================================================== */

void ddr_gl_drom_set_att_lgtint (DtReal intensity)
{
#   ifdef DEBUGP
	printf("Set_light_intensity(%f) called\n",intensity);
#   endif

    DevStatus.lightintens = intensity;
}



/* ====================================================================== */

void ddr_gl_drom_set_att_lgtspdang (
    DtReal total,
    DtReal delta)
{
    DevStatus.light_spread_angle_total = total;
    DevStatus.light_spread_angle_delta = delta;
}



/* ====================================================================== */

void ddr_gl_drom_set_att_lgtspdexp (DtReal exp)
{
    DevStatus.light_spread_exponent = exp;
}



/* ====================================================================== */

void ddr_gl_drom_set_att_lgtswi (
    DtInt count,
    DtObject *lights)
{
    DtInt i;
    DtInt lightidx;
    DtInt enabled;
   
    if (!DevStatus.view_data)
	return;

    pushmatrix();
    loadmatrix(DevStatus.view_data->world_to_cam_matrix);

    for (lightidx = 0; lightidx < DevStatus.view_data->numlights;
	 lightidx++) {
	enabled = DcTrue;
	for (i=0; i<count; i++) {
	    if (DevStatus.view_data->light[lightidx].light == lights[i]) {
		enabled = DcFalse;
		break;
	    }
	}
	DevStatus.view_data->light[lightidx].enabled = enabled;
	pushmatrix();
	multmatrix(DevStatus.view_data->light[lightidx].light_to_world_matrix);
	if (enabled) {
	    lmbind(IrisLightNo(lightidx),IrisLightDefnNo(lightidx));
	} else {
	    lmbind(IrisLightNo(lightidx), 0);
	}
	popmatrix();
    }
    popmatrix();
}



/* ======================================================================== */

void ddr_gl_drom_set_att_lgttyp (DtObject type)
{
#   ifdef DEBUGP
	printf("Set_light_type(%d) called\n",type);
#   endif

    DevStatus.lighttype = type;
}



/* ======================================================================== */

#define RadToDegrees(rad) ((rad)*57.2958)

void ddr_gl_drom_update_display_light (
    light_data *lt,
    int ln)
{
    float props[40];
    int prop;

    /*
     * Hardware can't handle ambient lights in pseudocolor mode:
     */
    if (lt->type==DcLightAmbient && DevStatus.visualtype==DcPseudoColor) {

	lmbind(IrisLightNo(ln),0);
	return;
    }

    prop=0;

    if (lt->type == DcLightAmbient)
	    props[prop++] = AMBIENT;
    else
	    props[prop++] = LCOLOR;

    props[prop++] = lt->color[0];
    props[prop++] = lt->color[1];
    props[prop++] = lt->color[2];

    if (lt->type == DcLightAmbient)
	    props[prop++] = LCOLOR;
    else
	    props[prop++] = AMBIENT;

    props[prop++] = 0.;
    props[prop++] = 0.;
    props[prop++] = 0.;

    props[prop++] = POSITION;

    if (lt->type == DcLightInfinite || lt->type == DcLightAmbient) {
	props[prop++] = -lt->direction[0];
	props[prop++] = -lt->direction[1];
	props[prop++] = -lt->direction[2];		/* light at inifinity */
	props[prop++] = 0.;
    } else {
	props[prop++] = lt->position[0];
	props[prop++] = lt->position[1];
	props[prop++] = lt->position[2];		/* light in scene */
	props[prop++] = 1.;
    }


    if (lt->type == DcLightSpot ||
	lt->type == DcLightSpotAttn) {
	props[prop++] = SPOTDIRECTION;
	props[prop++] = lt->direction[0];
	props[prop++] = lt->direction[1];
	props[prop++] = lt->direction[2];
    }

    props[prop++] = SPOTLIGHT;

    if (lt->type == DcLightSpot ||
	lt->type == DcLightSpotAttn) {
	props[prop++] = lt->spread_exponent > 128. ? 128. : lt->spread_exponent;
	props[prop++] = RadToDegrees(lt->spread_angle_total);
    } else {
	props[prop++] = 0.0;
	props[prop++] = 180.0;
    }

    props[prop++] = LMNULL;

    pushmatrix();
    multmatrix(lt->light_to_world_matrix);

    lmdef (DEFLIGHT, IrisLightDefnNo(ln), 0, props);
    lmbind (IrisLightNo(ln), IrisLightDefnNo(ln));

    popmatrix();
}



/* ====================================================================
** The viewing matrix MUST be Identity when this
** is called.
** ==================================================================== */

void ddr_gl_drom_update_display_lights (void)
{
    int l;

    mmode(MVIEWING);
    pushmatrix();
    loadmatrix(DevStatus.view_data->world_to_cam_matrix);

    for (l=0; l<DevStatus.view_data->numlights; l++) {
	ddr_gl_drom_update_display_light(&(DevStatus.view_data->light[l]),l);
    }

    /* Turn the rest of the lights off: */

    for (; l<MAXLIGHTS; l++)
	    lmbind(IrisLightNo(l),0);

    popmatrix();
}

/* ====================================================================== */

void ddr_gl_drom_reset_lights (void)
{
    int l;

    mmode(MVIEWING);
    pushmatrix();
    loadmatrix(DevStatus.view_data->world_to_cam_matrix);

    for (l=0; l<DevStatus.view_data->numlights; l++) {
	pushmatrix();
	multmatrix(DevStatus.view_data->light[l].light_to_world_matrix);
	if (DevStatus.view_data->light[l].enabled) {
	    lmbind(IrisLightNo(l),IrisLightDefnNo(l));
	}
	popmatrix();
    }

    /* Turn the rest of the lights off: */

    for (; l<MAXLIGHTS; l++)
	lmbind (IrisLightNo(l), 0);

    popmatrix();
}
