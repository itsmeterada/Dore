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
	void ddr_g4vll_drom_light (light)
	void ddr_g4vll_drom_set_att_lgtatn (c1, c2)
	void ddr_g4vll_drom_set_att_lgtclr (colormodel, color)
	void ddr_g4vll_drom_set_att_lgtint (intensity)
	void ddr_g4vll_drom_set_att_lgtspdang (total, delta)
	void ddr_g4vll_drom_set_att_lgtspdexp (exponent)
	void ddr_g4vll_drom_set_att_lgtswi (count, lights)
	void ddr_g4vll_drom_set_att_lgttyp (type)

  ======================================================================
 */
#include "drom.h"

/*
 ======================================================================
 */
void ddr_g4vll_drom_light (
    DtObject light)
{
    DtPoint3 pos;
    DtVector3 dir;
    DtReal w;
    float color[3];
    int lightno;
    DtMatrix4x4 lcstowcsmat;
    int light_type;
    float z_val;

    ddr_g4vll_drom_get_lcstowcsmat(lcstowcsmat);

    lightno = (dde_g4vll_drom_curviwdat->total_lights)++;

    if (dde_g4vll_drom_curviwdat->total_lights > dde_g4vll_drom_curviwdat->max_lights) {
	dde_g4vll_drom_curviwdat->light = (DtObject *)DDspace_Reallocate
		(dde_g4vll_drom_curviwdat->light,
		 (dde_g4vll_drom_curviwdat->max_lights+10)*sizeof(DtObject));
	dde_g4vll_drom_curviwdat->max_lights += 10;
    }

    dor_math_homo_trns(0.,0.,0.,1.,lcstowcsmat,
		       &pos[0], &pos[1], &pos[2], &w);
    if (w != 0.0) {
	pos[0] /= w; pos[1] /= w; pos[2] /= w;
    }

    z_val = -1.0;

    if (dde_g4vll_drom_data.light_type == DcLightAmbient) {
	light_type = VLL_AMB_LIGHT;
    } else if (dde_g4vll_drom_data.light_type == DcLightInfinite) {
	light_type = VLL_DIR_LIGHT;
    } else if (dde_g4vll_drom_data.light_type == DcLightPoint) {
	light_type = VLL_POINT_LIGHT;
    } else if (dde_g4vll_drom_data.light_type == DcLightPointAttn) {
	light_type = VLL_ATTN_POINT_LIGHT;
    } else if (dde_g4vll_drom_data.light_type == DcLightSpot) {
	light_type = VLL_SPOT_LIGHT;
    } else if (dde_g4vll_drom_data.light_type == DcLightSpotAttn) {
	light_type = VLL_ATTN_SPOT_LIGHT;
    } else {
	DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_g4vll_drom_light",
		    "Unknown light type, light ignored");
	return;
    }

    dor_math_homo_trns(0.,0.,z_val,1., lcstowcsmat,
		       &dir[0], &dir[1], &dir[2],&w);
    if (w != 0.0) {
	dir[0] /= w; dir[1] /= w; dir[2] /= w;
    }

    dir[0] -= pos[0];
    dir[1] -= pos[1];
    dir[2] -= pos[2];

    dor_math_renormalize_fast(dir);

    color[0] = dde_g4vll_drom_data.light_color[0] *
	    dde_g4vll_drom_data.light_intensity;
    color[1] = dde_g4vll_drom_data.light_color[1] *
	    dde_g4vll_drom_data.light_intensity;
    color[2] = dde_g4vll_drom_data.light_color[2] *
	    dde_g4vll_drom_data.light_intensity;

    /*  This is so that the XFDI shader will not wrap around.  */

    color[0] = min(color[0], .99);
    color[1] = min(color[1], .99);
    color[2] = min(color[2], .99);

    dde_g4vll_drom_curviwdat->light[lightno] = light;

    if (sizeof(float) != sizeof(DtReal)) {
	int i;
	float pos1[3], dir1[3];

	for (i=0; i<3; i++) {
	    pos1[i] = pos[i];
	    dir1[i] = dir[i];
	}

	VLLset_light(lightno, light_type, pos1, dir1, color, 
		     dde_g4vll_drom_data.light_spread_angle_total,
		     dde_g4vll_drom_data.light_spread_angle_delta,
		     dde_g4vll_drom_data.light_spread_exponent,
		     dde_g4vll_drom_data.light_att_c1,
		     dde_g4vll_drom_data.light_att_c2);
    } else {
	VLLset_light(lightno, light_type, (float *)pos, (float *)dir, color, 
		     dde_g4vll_drom_data.light_spread_angle_total,
		     dde_g4vll_drom_data.light_spread_angle_delta,
		     dde_g4vll_drom_data.light_spread_exponent,
		     dde_g4vll_drom_data.light_att_c1,
		     dde_g4vll_drom_data.light_att_c2);
    }
    VLLset_light_status(lightno, 1);
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_lgtatn (
    DtReal c1,
    DtReal c2)
{
    dde_g4vll_drom_data.light_att_c1 = c1;
    dde_g4vll_drom_data.light_att_c2 = c2;
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_lgtclr (
    DtColorModel colormodel,
    DtReal color[])
{
    dde_g4vll_drom_data.light_color[0] = color[0];
    dde_g4vll_drom_data.light_color[1] = color[1];
    dde_g4vll_drom_data.light_color[2] = color[2];
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_lgtint (
    DtReal intensity)
{
    dde_g4vll_drom_data.light_intensity = intensity;
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_lgtspdang (
    DtReal total,
    DtReal delta)
{
    dde_g4vll_drom_data.light_spread_angle_total = total;
    dde_g4vll_drom_data.light_spread_angle_delta = delta;
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_lgtspdexp (
    DtReal exponent)
{
    dde_g4vll_drom_data.light_spread_exponent = exponent;
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_lgtswi (
    DtInt count,
    DtObject *lights)
{
    DtInt i;
    DtInt lightidx;
    DtInt enabled;
   
    if (dde_g4vll_drom_data.pre_init)
	    return;

    if (dde_g4vll_drom_curviwdat == (ddt_g4vll_drom_view *)0)
	    return;

    for (lightidx = 0; lightidx < dde_g4vll_drom_curviwdat->total_lights; 
	 lightidx++) {
	enabled = DcTrue;
	for (i=0; i<count; i++) {
	    if (dde_g4vll_drom_curviwdat->light[lightidx] == lights[i]) {
		enabled = DcFalse;
		break;
	    }
	}
	VLLset_light_status(lightidx, enabled);

#ifdef DEBUG
	printf ("light %d turned %d\n", lightidx, enabled);
#endif
    }
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_lgttyp (
    DtObject type)
{
    dde_g4vll_drom_data.light_type = type;
}
/*
 ======================================================================
 */
