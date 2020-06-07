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
	int ddr_x11_XFDI_drom_light (light)
	ddr_x11_XFDI_drom_set_att_lgtatn ddr_x11_XFDI_drom_set_att_lgtatn (c1, c2)
	int ddr_x11_XFDI_drom_set_att_lgtclr (colormodel, color)
	int ddr_x11_XFDI_drom_set_att_lgtint (intensity)
	int ddr_x11_XFDI_drom_set_att_lgtspdang (total, delta)
	int ddr_x11_XFDI_drom_set_att_lgtspdexp (exponent)
	int ddr_x11_XFDI_drom_set_att_lgtswi (count, lights)
	int ddr_x11_XFDI_drom_set_att_lgttyp (type)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_light(light)
     DtObject light;
{
    DtPoint3 pos;
    DtVector3 dir;
    DtReal w;
    float color[3];
    float angle;
    int lightno;
    DtMatrix4x4 lcstowcsmat;
    int light_type;
    float z_val;

    if (dde_x11_XFDI_drom_curviwdat->total_lights >= 16) {
	DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_x11_XFDI_drom_light",
		    "Only support 16 lights, light ignored");
    } else {
	ddr_x11_XFDI_drom_get_lcstowcsmat(lcstowcsmat);

	lightno = (dde_x11_XFDI_drom_curviwdat->total_lights)++;

	dor_math_homo_trns(0.,0.,0.,1.,lcstowcsmat,
			   &pos[0], &pos[1], &pos[2], &w);
	if (w != 0.0) {
	    pos[0] /= w; pos[1] /= w; pos[2] /= w;
	}

	/* the light points in the negative Z direction but XFDI wants the
	   direction to the light which would be positive Z.
	   */

	if (dde_x11_XFDI_drom_data.light_type == DcLightAmbient) {
	    light_type = XFDIAmbLight;
	    z_val = -1.0;
	} else if (dde_x11_XFDI_drom_data.light_type == DcLightInfinite) {
	    light_type = XFDIDirLight;
	    z_val = 1.0;
	} else if (dde_x11_XFDI_drom_data.light_type == DcLightPoint ||
		   dde_x11_XFDI_drom_data.light_type == DcLightPointAttn) {
	    light_type = XFDIPointLight;
	    z_val = -1.0;
	} else if (dde_x11_XFDI_drom_data.light_type == DcLightSpot ||
		   dde_x11_XFDI_drom_data.light_type == DcLightSpotAttn) {
	    light_type = XFDISpotLight;
	    z_val = 1.0;
	} else {
	    DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_x11_XFDI_drom_light",
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

	color[0] = dde_x11_XFDI_drom_data.light_color[0] *
		dde_x11_XFDI_drom_data.light_intensity;
	color[1] = dde_x11_XFDI_drom_data.light_color[1] *
		dde_x11_XFDI_drom_data.light_intensity;
	color[2] = dde_x11_XFDI_drom_data.light_color[2] *
		dde_x11_XFDI_drom_data.light_intensity;

	/*  This is so that the XFDI shader will not wrap around.  */

	color[0] = min(color[0], .99);
	color[1] = min(color[1], .99);
	color[2] = min(color[2], .99);

	dde_x11_XFDI_drom_curviwdat->light[lightno] = light;

	angle = dde_x11_XFDI_drom_data.light_spread_angle_total * 57.29577951308;
#ifdef DEBUG
	printf ("light %d: %s\n\tat: (%g,%g,%g)\n\tdir: (%g,%g,%g)\n\tcolor (%g,%g,%g)\n\tangle %g\n",
		lightno, 
		light_type == XFDIAmbLight ? "XFDIAmbLight" :
		light_type == XFDIDirLight ? "XFDIDirLight" :
		light_type == XFDIPointLight ? "XFDIPointLight" :
		light_type == XFDISpotLight ? "XFDISpotLight" : "(UNKNOWN)",
		pos[0], pos[1], pos[2], dir[0], dir[1], dir[2], 
		color[0], color[1], color[2], angle);
#endif
	XFDISetLight(dde_x11_XFDI_drom_curdevdat->display,
		     dde_x11_XFDI_drom_curdevdat->gc,
		     lightno, light_type, pos, dir, color, 
		     angle);
	XFDISetLightStatus(dde_x11_XFDI_drom_curdevdat->display,
			   dde_x11_XFDI_drom_curdevdat->gc,
			   lightno, XFDIOn);
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_lgtatn (c1, c2)
     DtReal c1;
     DtReal c2;
{
    dde_x11_XFDI_drom_data.light_att_c1 = c1;
    dde_x11_XFDI_drom_data.light_att_c2 = c2;
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_lgtclr(colormodel, color)
DtColorModel colormodel;
DtReal color[];
{
    dde_x11_XFDI_drom_data.light_color[0] = color[0];
    dde_x11_XFDI_drom_data.light_color[1] = color[1];
    dde_x11_XFDI_drom_data.light_color[2] = color[2];
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_lgtint(intensity)
     DtReal intensity;
{
    dde_x11_XFDI_drom_data.light_intensity = intensity;
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_lgtspdang (total, delta)
     DtReal total;
     DtReal delta;
{
    dde_x11_XFDI_drom_data.light_spread_angle_total = total;
    dde_x11_XFDI_drom_data.light_spread_angle_delta = delta;
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_lgtspdexp (exponent)
     DtReal exponent;
{
    dde_x11_XFDI_drom_data.light_spread_exponent = exponent;
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_lgtswi (count, lights)
     DtInt count;
     DtObject *lights;
{
    DtInt i;
    DtInt lightidx;
    DtInt enabled;
   
    if (dde_x11_XFDI_drom_curviwdat == (ddt_x11_XFDI_drom_view *)0)
	    return;

    for (lightidx = 0; lightidx < dde_x11_XFDI_drom_curviwdat->total_lights; 
	 lightidx++) {
	enabled = DcTrue;
	for (i=0; i<count; i++) {
	    if (dde_x11_XFDI_drom_curviwdat->light[lightidx] == lights[i]) {
		enabled = DcFalse;
		break;
	    }
	}
	XFDISetLightStatus(dde_x11_XFDI_drom_curdevdat->display,
			   dde_x11_XFDI_drom_curdevdat->gc,
			   lightidx, enabled);

#ifdef DEBUG
	printf ("light %d turned %d\n", lightidx, enabled);
#endif
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_lgttyp(type)
     DtObject type;
{
    dde_x11_XFDI_drom_data.light_type = type;
}
