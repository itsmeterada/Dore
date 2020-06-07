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
	void ddr_template_drom_light (light)
	void ddr_template_drom_set_att_lgtatn (c1, c2)
	void ddr_template_drom_set_att_lgtclr (colormodel, color)
	void ddr_template_drom_set_att_lgtint (intensity)
	void ddr_template_drom_set_att_lgtspdang (total, delta)
	void ddr_template_drom_set_att_lgtspdexp (exp)
	void ddr_template_drom_set_att_lgtswi (count, lights)
	void ddr_template_drom_set_att_lgttyp (type)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
void ddr_template_drom_light (
    DtObject light)
{
    static DtReal lcspos[4] = { 0., 0.,  0., 1.};
    static DtReal lcsdir[4] = { 0., 0., -1., 1.};
    DtReal pos[4];
    DtReal dir[4];
    DtColorRGB color;
    int lightno;
    DtMatrix4x4 lcstowcsmat;

    ddr_template_drom_get_lcstowcsmat(lcstowcsmat);

    lightno = (dde_template_drom_curviwdat->total_lights)++;

    if (dde_template_drom_curviwdat->total_lights > 
	dde_template_drom_curviwdat->max_lights) {
	dde_template_drom_curviwdat->light = (ddt_template_drom_light *)
		DDspace_Reallocate
			(dde_template_drom_curviwdat->light,
			 (dde_template_drom_curviwdat->max_lights+10)*
			 sizeof(ddt_template_drom_light));
	dde_template_drom_curviwdat->max_lights += 10;
    }

    ddr_template_math_pnttrns(lcspos,lcstowcsmat, pos);
    if (pos[3] != 0.0) {
	pos[0] /= pos[3];
	pos[1] /= pos[3];
	pos[2] /= pos[3];
    }

    ddr_template_math_pnttrns(lcsdir, lcstowcsmat, dir);
    if (dir[3] != 0.0) {
	dir[0] /= dir[3];
	dir[1] /= dir[3]; 
	dir[2] /= dir[3];
    }

    dir[0] -= pos[0];
    dir[1] -= pos[1];
    dir[2] -= pos[2];

    ddr_template_math_renormalize(dir);

    color[0] = dde_template_drom_data.light_color[0] *
	    dde_template_drom_data.light_intensity;
    color[1] = dde_template_drom_data.light_color[1] *
	    dde_template_drom_data.light_intensity;
    color[2] = dde_template_drom_data.light_color[2] *
	    dde_template_drom_data.light_intensity;

    dde_template_drom_curviwdat->light[lightno].light = light;

    dde_template_drom_curviwdat->light[lightno].type = 
	    dde_template_drom_data.light_type;

    dde_template_drom_curviwdat->light[lightno].position[0] = pos[0];
    dde_template_drom_curviwdat->light[lightno].position[1] = pos[1];
    dde_template_drom_curviwdat->light[lightno].position[2] = pos[2];

    dde_template_drom_curviwdat->light[lightno].direction[0] = dir[0];
    dde_template_drom_curviwdat->light[lightno].direction[1] = dir[1];
    dde_template_drom_curviwdat->light[lightno].direction[2] = dir[2];

    dde_template_drom_curviwdat->light[lightno].color[0] = color[0];
    dde_template_drom_curviwdat->light[lightno].color[1] = color[1];
    dde_template_drom_curviwdat->light[lightno].color[2] = color[2];

    dde_template_drom_curviwdat->light[lightno].angle_total =
	    dde_template_drom_data.light_spread_angle_total;
    dde_template_drom_curviwdat->light[lightno].angle_delta =
	    dde_template_drom_data.light_spread_angle_delta;
    dde_template_drom_curviwdat->light[lightno].exponent =
	    dde_template_drom_data.light_spread_exponent;
    dde_template_drom_curviwdat->light[lightno].c1 = 
	    dde_template_drom_data.light_att_c1;
    dde_template_drom_curviwdat->light[lightno].c2 = 
	    dde_template_drom_data.light_att_c2;
    dde_template_drom_curviwdat->light[lightno].status = DcOn;
}
/*
 ======================================================================
 */
void ddr_template_drom_set_att_lgtatn (
    DtReal c1,
    DtReal c2)
{
    dde_template_drom_data.light_att_c1 = c1;
    dde_template_drom_data.light_att_c2 = c2;
}
/*
 ======================================================================
 */
void ddr_template_drom_set_att_lgtclr (
    DtColorModel colormodel,
    DtReal color[])
{
    dde_template_drom_data.light_color[0] = color[0];
    dde_template_drom_data.light_color[1] = color[1];
    dde_template_drom_data.light_color[2] = color[2];
}
/*
 ======================================================================
 */
void ddr_template_drom_set_att_lgtint (
    DtReal intensity)
{
    dde_template_drom_data.light_intensity = intensity;
}
/*
 ======================================================================
 */
void ddr_template_drom_set_att_lgtspdang (
    DtReal total,
    DtReal delta)
{
    dde_template_drom_data.light_spread_angle_total = total;
    dde_template_drom_data.light_spread_angle_delta = delta;
}
/*
 ======================================================================
 */
void ddr_template_drom_set_att_lgtspdexp (
    DtReal exp)
{
    dde_template_drom_data.light_spread_exponent = exp;
}
/*
 ======================================================================
 */
void ddr_template_drom_set_att_lgtswi (
    DtInt count,
    DtObject *lights)
{
    DtInt i;
    DtInt lightidx;
    DtInt enabled;
   
    if (dde_template_drom_pre_init) 
	    return;

    if (dde_template_drom_curviwdat == (ddt_template_drom_view *)0)
	    return;

    for (lightidx = 0; lightidx < dde_template_drom_curviwdat->total_lights; 
	 lightidx++) {
	enabled = DcTrue;
	for (i=0; i<count; i++) {
	    if (dde_template_drom_curviwdat->light[lightidx].light == lights[i]) {
		enabled = DcFalse;
		break;
	    }
	}
	dde_template_drom_curviwdat->light[lightidx].status = enabled;
    }
}
/*
 ======================================================================
 */
void ddr_template_drom_set_att_lgttyp (
    DtObject type)
{
    dde_template_drom_data.light_type = type;
}
/*
 ======================================================================
 */
