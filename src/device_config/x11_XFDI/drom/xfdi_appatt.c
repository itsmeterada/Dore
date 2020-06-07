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
	int ddr_x11_XFDI_drom_update_surface_properties ()
	long ddr_x11_XFDI_drom_get_pixel_value (color)
	int ddr_x11_XFDI_drom_set_att_ambint (intensity)
	int ddr_x11_XFDI_drom_set_att_ambswi (switchvalue)
	int ddr_x11_XFDI_drom_set_att_bacfacculble (switchvalue)
	int ddr_x11_XFDI_drom_set_att_bacfacculswi (switchvalue)
	int ddr_x11_XFDI_drom_set_att_depcue (zfront,zback,sfront,sback,colormodel,color)
	int ddr_x11_XFDI_drom_set_att_depcueswi (switchvalue)
	int ddr_x11_XFDI_drom_set_att_difswi (switchvalue)
	int ddr_x11_XFDI_drom_set_att_difclr (colormodel, color)
	int ddr_x11_XFDI_drom_set_att_difint (intensity)
	int ddr_x11_XFDI_drom_set_att_hidsrfswi (switchvalue)
	int ddr_x11_XFDI_drom_set_att_inttyp (interptype)
	int ddr_x11_XFDI_drom_set_att_lintyp (type)
	int ddr_x11_XFDI_drom_set_att_linwid (width)
	int ddr_x11_XFDI_drom_set_att_refswi (switchvalue)
	int ddr_x11_XFDI_drom_set_att_reptyp (reptype)
	int ddr_x11_XFDI_drom_set_att_shaswi (switchvalue)
	int ddr_x11_XFDI_drom_set_att_shdidx (shadeindex)
	int ddr_x11_XFDI_drom_set_att_spcclr (colormodel, color)
	int ddr_x11_XFDI_drom_set_att_spcfct (factor)
	int ddr_x11_XFDI_drom_set_att_spcint (intensity)
	int ddr_x11_XFDI_drom_set_att_spcswi (switchvalue)
	int ddr_x11_XFDI_drom_set_att_srfshd (callbackobj)
	int ddr_x11_XFDI_drom_set_att_transpclr (colormodel, color)
	int ddr_x11_XFDI_drom_set_att_transpint (intensity)
	int ddr_x11_XFDI_drom_set_att_transpswi (switchvalue)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_update_surface_properties()

{
    DtReal ambintens, difintens, spcintens;

    /*  This is so that the XFDI shader will not wrap around.  */

    if (dde_x11_XFDI_drom_data.ambient_switch) {
	ambintens = min(dde_x11_XFDI_drom_data.ambient_intensity,.99);
    } else {
	ambintens = 0.;
    }
    if (dde_x11_XFDI_drom_data.diffuse_switch) {
	difintens = min(dde_x11_XFDI_drom_data.diffuse_intensity,.99);
    } else {
	difintens = 0.;
    }
    if (dde_x11_XFDI_drom_data.specular_switch) {
	spcintens = min(dde_x11_XFDI_drom_data.specular_intensity,.99);
    } else {
	spcintens = 0.;
    }

    XFDISetSurfaceMaterial(dde_x11_XFDI_drom_curdevdat->display,
			   dde_x11_XFDI_drom_curdevdat->gc,
			   ambintens,
			   difintens,
			   spcintens,
			   dde_x11_XFDI_drom_data.specular_factor);

    if (dde_x11_XFDI_drom_data.constant_shading_switch) {
	XFDISetSurfaceLightingModel(dde_x11_XFDI_drom_curdevdat->display,
				    dde_x11_XFDI_drom_curdevdat->gc,
				    XFDINoLighting);
    } else if (!dde_x11_XFDI_drom_data.specular_switch) {
	XFDISetSurfaceLightingModel(dde_x11_XFDI_drom_curdevdat->display,
				    dde_x11_XFDI_drom_curdevdat->gc,
				    XFDIDiffuse);
    } else {
	XFDISetSurfaceLightingModel(dde_x11_XFDI_drom_curdevdat->display,
				    dde_x11_XFDI_drom_curdevdat->gc,
				    XFDISpecular);
    }
}
/*
 ======================================================================
 */
unsigned long
ddr_x11_XFDI_drom_get_pixel_value (color)
     DtColorRGB color;
{
    unsigned long c;

    if (dde_x11_XFDI_drom_curdevdat->visual_type == DcPseudoColor) {
	c = dde_x11_XFDI_drom_curdevdat->current_shade_range_min + 
		(color[0] * dde_x11_XFDI_drom_curdevdat->cwtavgpsweight[0] +
		 color[1] * dde_x11_XFDI_drom_curdevdat->cwtavgpsweight[1] +
		 color[2] * dde_x11_XFDI_drom_curdevdat->cwtavgpsweight[2]) *
			 dde_x11_XFDI_drom_curdevdat->shade_range_scale;

	c = c > dde_x11_XFDI_drom_curdevdat->current_shade_range_max ?
		dde_x11_XFDI_drom_curdevdat->current_shade_range_max : c;
    } else {
	c = (256*256* (255 & (int)(255.*color[0]))) | 
		(256* (255 & (int)(255.*color[1]))) |
			(255 & (int)(255.*color[2]));
    }
    return c;
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_ambint(intensity)
     DtReal intensity;
{
    dde_x11_XFDI_drom_data.ambient_intensity = intensity;

    if (!dde_x11_XFDI_drom_pre_init) 
	    ddr_x11_XFDI_drom_update_surface_properties();
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_ambswi(switchvalue)
     DtSwitch switchvalue;
{
    dde_x11_XFDI_drom_data.ambient_switch = switchvalue;

    if (!dde_x11_XFDI_drom_pre_init) 
	    ddr_x11_XFDI_drom_update_surface_properties();
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_bacfacculble(switchvalue)
     DtSwitch switchvalue;
{
    dde_x11_XFDI_drom_data.backface_cullable_switch = switchvalue;

    if (!dde_x11_XFDI_drom_pre_init) {
	if (dde_x11_XFDI_drom_data.backface_cullable_switch &&
	    dde_x11_XFDI_drom_data.backface_cull_switch) {
	    XFDISetBackfaceEliminateFlag(dde_x11_XFDI_drom_curdevdat->display,
					 dde_x11_XFDI_drom_curdevdat->gc,
					 XFDICullClockwise);
	} else {
	    XFDISetBackfaceEliminateFlag(dde_x11_XFDI_drom_curdevdat->display,
					 dde_x11_XFDI_drom_curdevdat->gc,
					 XFDICullOff);
	}
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_bacfacculswi(switchvalue)
     DtSwitch switchvalue;
{
    dde_x11_XFDI_drom_data.backface_cull_switch = switchvalue;

    if (!dde_x11_XFDI_drom_pre_init) {
	if (dde_x11_XFDI_drom_data.backface_cullable_switch &&
	    dde_x11_XFDI_drom_data.backface_cull_switch) {
	    XFDISetBackfaceEliminateFlag(dde_x11_XFDI_drom_curdevdat->display,
					 dde_x11_XFDI_drom_curdevdat->gc,
					 XFDICullClockwise);
	} else {
	    XFDISetBackfaceEliminateFlag(dde_x11_XFDI_drom_curdevdat->display,
					 dde_x11_XFDI_drom_curdevdat->gc,
					 XFDICullOff);
	}
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_depcue(zfront,zback,sfront,sback,colormodel,color)
     DtReal zfront, zback;
     DtReal sfront, sback;
     DtColorModel colormodel;
     DtReal *color;
{
    DtColorRGB localcolor;
    unsigned long pixelvalue;
    extern unsigned long ddr_x11_XFDI_drom_get_pixel_value();

    /* Dore' depthcueing range is 1 at Zfront and 0 at Zback to XFDI's
     *  1 at zfront and -1 at zback, thus the conversion is necessary.
     */

    localcolor[0] = min(color[0], .99);
    localcolor[1] = min(color[1], .99);
    localcolor[2] = min(color[2], .99);

    pixelvalue = ddr_x11_XFDI_drom_get_pixel_value (localcolor);

    XFDISetDepthCueing(dde_x11_XFDI_drom_curdevdat->display,
		       dde_x11_XFDI_drom_curdevdat->gc,
		       2.*zfront - 1.,
		       2.*zback - 1,
		       sfront, sback);

    XFDISetMarkerBackground (dde_x11_XFDI_drom_curdevdat->display,
			     dde_x11_XFDI_drom_curdevdat->gc,
			     pixelvalue);

    XFDISetLineBackground (dde_x11_XFDI_drom_curdevdat->display,
			   dde_x11_XFDI_drom_curdevdat->gc,
			   pixelvalue);

    XFDISetSurfaceBackground (dde_x11_XFDI_drom_curdevdat->display,
			      dde_x11_XFDI_drom_curdevdat->gc,
			      pixelvalue);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_depcueswi(switchvalue)
     DtSwitch switchvalue;
{
    XFDISetDepthCueingFlag(dde_x11_XFDI_drom_curdevdat->display,
			   dde_x11_XFDI_drom_curdevdat->gc,
			   switchvalue == DcTrue ? 1 : 0);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_difswi(switchvalue)
     DtSwitch switchvalue;
{
    dde_x11_XFDI_drom_data.diffuse_switch = switchvalue;

    if (!dde_x11_XFDI_drom_pre_init) 
	    ddr_x11_XFDI_drom_update_surface_properties();
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_difclr(colormodel, color)
DtColorModel colormodel;
DtReal color[];
{
    DtColorRGB localcolor;
    unsigned long pixelvalue;
    extern unsigned long ddr_x11_XFDI_drom_get_pixel_value();

    dde_x11_XFDI_drom_data.diffuse_color[0] = min(color[0], .99);
    dde_x11_XFDI_drom_data.diffuse_color[1] = min(color[1], .99);
    dde_x11_XFDI_drom_data.diffuse_color[2] = min(color[2], .99);

    pixelvalue = ddr_x11_XFDI_drom_get_pixel_value 
	    (dde_x11_XFDI_drom_data.diffuse_color);

    XFDISetMarkerColor(dde_x11_XFDI_drom_curdevdat->display,
		       dde_x11_XFDI_drom_curdevdat->gc,
		       pixelvalue);

    XFDISetLineColor(dde_x11_XFDI_drom_curdevdat->display,
		     dde_x11_XFDI_drom_curdevdat->gc,
		     pixelvalue);

    XFDISetSurfaceColor(dde_x11_XFDI_drom_curdevdat->display,
			dde_x11_XFDI_drom_curdevdat->gc,
			pixelvalue);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_difint(intensity)
     DtReal intensity;
{
    dde_x11_XFDI_drom_data.diffuse_intensity = intensity;

    if (!dde_x11_XFDI_drom_pre_init) 
	    ddr_x11_XFDI_drom_update_surface_properties();
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_hidsrfswi(switchvalue)
     DtSwitch switchvalue;
{
    dde_x11_XFDI_drom_data.hidden_surf_switch = switchvalue;

    ddr_x11_XFDI_dcm_set_depth_buffer_write(switchvalue);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_inttyp(interptype)
     DtInterpType interptype;
{
    dde_x11_XFDI_drom_data.interptype = interptype;

    switch (interptype) {
    case DcConstantShade:
	/*
	 * NOTE: Flat shading and transparency do not work togegher so
	 * use Gouraud shading instead.
	 */
	if (dde_x11_XFDI_drom_data.transparent_surface_pass) {
	    XFDISetSurfaceShading(dde_x11_XFDI_drom_curdevdat->display,
				  dde_x11_XFDI_drom_curdevdat->gc,
				  XFDIShadeColor);
	} else {
	    XFDISetSurfaceShading(dde_x11_XFDI_drom_curdevdat->display,
				  dde_x11_XFDI_drom_curdevdat->gc,
				  XFDIShadeNone);
	}
	break;
    case DcVertexShade:
	XFDISetSurfaceShading(dde_x11_XFDI_drom_curdevdat->display,
			      dde_x11_XFDI_drom_curdevdat->gc,
			      XFDIShadeColor);
	break;
    case DcSurfaceShade:
	XFDISetSurfaceShading(dde_x11_XFDI_drom_curdevdat->display,
			      dde_x11_XFDI_drom_curdevdat->gc,
			      XFDIShadeNormal);
	break;
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_lintyp (type)
     DtLineType type;
{
    switch (type) {
    case DcLineTypeDash:
	XFDISetLineStyle(dde_x11_XFDI_drom_curdevdat->display,
			 dde_x11_XFDI_drom_curdevdat->gc,
			 0xf0f0f0f0, 1.0);
	break;

    case DcLineTypeDot:
	XFDISetLineStyle(dde_x11_XFDI_drom_curdevdat->display,
			 dde_x11_XFDI_drom_curdevdat->gc,
			 0x88888888, 1.0);
	break;
      
    case DcLineTypeDotDash:
	XFDISetLineStyle(dde_x11_XFDI_drom_curdevdat->display,
			 dde_x11_XFDI_drom_curdevdat->gc,
			 0x83e083e0, 1.0);
	break;
    case DcLineTypeSolid:
    default:
	XFDISetLineStyle(dde_x11_XFDI_drom_curdevdat->display,
			 dde_x11_XFDI_drom_curdevdat->gc,
			 0xffffffff, 1.0);
	break;
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_linwid (width)
     DtReal width;
{
    XFDISetLineWidth(dde_x11_XFDI_drom_curdevdat->display,
		     dde_x11_XFDI_drom_curdevdat->gc,
		     width);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_refswi(switchvalue)
     DtSwitch switchvalue;
{
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_reptyp(reptype)
     DtRepType reptype;
{
    dde_x11_XFDI_drom_data.reptype = reptype;
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_shaswi(switchvalue)
     DtSwitch switchvalue;
{
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_shdidx(shadeindex)
     DtInt shadeindex;

{
    DtInt shaderange[2];
    unsigned long pixelvalue;

    dde_x11_XFDI_drom_data.shade_index = shadeindex;

    shaderange[0] = -10;
    shaderange[1] = -10;
    DdInqShadeRanges (dde_x11_XFDI_drom_data.current_device,
				dde_x11_XFDI_drom_data.shade_index,
				1,shaderange);
    if (shaderange[0] != -10 && shaderange[1] != -10) {
	dde_x11_XFDI_drom_curdevdat->current_shade_range_min = shaderange[0];
	dde_x11_XFDI_drom_curdevdat->current_shade_range_max = shaderange[1];
   
	dde_x11_XFDI_drom_curdevdat->shade_range_scale = 
		(shaderange[1] - shaderange[0]);
    }

    if (dde_x11_XFDI_drom_curdevdat->visual_type == DcPseudoColor) {
	pixelvalue = ddr_x11_XFDI_drom_get_pixel_value 
		(dde_x11_XFDI_drom_data.diffuse_color);

	XFDISetMarkerColor(dde_x11_XFDI_drom_curdevdat->display,
			   dde_x11_XFDI_drom_curdevdat->gc,
			   pixelvalue);

	XFDISetLineColor(dde_x11_XFDI_drom_curdevdat->display,
			 dde_x11_XFDI_drom_curdevdat->gc,
			 pixelvalue);

	XFDISetSurfaceColor(dde_x11_XFDI_drom_curdevdat->display,
			    dde_x11_XFDI_drom_curdevdat->gc,
			    pixelvalue);

	pixelvalue = ddr_x11_XFDI_drom_get_pixel_value 
		(dde_x11_XFDI_drom_data.specular_color);

	XFDISetSurfaceSpecularColor(dde_x11_XFDI_drom_curdevdat->display,
				    dde_x11_XFDI_drom_curdevdat->gc,
				    pixelvalue);

	XFDISetMarkerBackground (dde_x11_XFDI_drom_curdevdat->display,
				 dde_x11_XFDI_drom_curdevdat->gc,
				 shaderange[0]);

	XFDISetLineBackground (dde_x11_XFDI_drom_curdevdat->display,
			       dde_x11_XFDI_drom_curdevdat->gc,
			       shaderange[0]);

	XFDISetSurfaceBackground (dde_x11_XFDI_drom_curdevdat->display,
				  dde_x11_XFDI_drom_curdevdat->gc,
				  shaderange[0]);
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_spcclr(colormodel, color)
DtColorModel colormodel;
DtReal color[];
{
    DtColorRGB localcolor;
    unsigned long pixelvalue;
    extern unsigned long ddr_x11_XFDI_drom_get_pixel_value();

    dde_x11_XFDI_drom_data.specular_color[0] = min(color[0], .99);
    dde_x11_XFDI_drom_data.specular_color[1] = min(color[1], .99);
    dde_x11_XFDI_drom_data.specular_color[2] = min(color[2], .99);

    pixelvalue = ddr_x11_XFDI_drom_get_pixel_value 
	    (dde_x11_XFDI_drom_data.specular_color);

    XFDISetSurfaceSpecularColor(dde_x11_XFDI_drom_curdevdat->display,
				dde_x11_XFDI_drom_curdevdat->gc,
				pixelvalue);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_spcfct(factor)
     DtReal factor;
{
    dde_x11_XFDI_drom_data.specular_factor = factor;

    if (!dde_x11_XFDI_drom_pre_init)
	    ddr_x11_XFDI_drom_update_surface_properties();
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_spcint(intensity)
     DtReal intensity;
{
    dde_x11_XFDI_drom_data.specular_intensity = intensity;

    if (!dde_x11_XFDI_drom_pre_init) 
	    ddr_x11_XFDI_drom_update_surface_properties();
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_spcswi(switchvalue)
     DtSwitch switchvalue;
{
    dde_x11_XFDI_drom_data.specular_switch = switchvalue;

    if (!dde_x11_XFDI_drom_pre_init) 
	    ddr_x11_XFDI_drom_update_surface_properties();
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_srfshd(callbackobj)
     DtObject callbackobj;
{
    dde_x11_XFDI_drom_data.surface_shader = callbackobj;

    dde_x11_XFDI_drom_data.constant_shading_switch = 
	    (callbackobj == DcShaderConstant);

    ddr_x11_XFDI_drom_update_surface_properties();
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_transpclr(colormodel, color)
DtColorModel colormodel;
DtReal color[];
{
    dde_x11_XFDI_drom_data.transparent_color[0] = color[0];
    dde_x11_XFDI_drom_data.transparent_color[1] = color[1];
    dde_x11_XFDI_drom_data.transparent_color[2] = color[2];
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_transpint(intensity)
     DtReal intensity;
{
    DtReal intens;

    dde_x11_XFDI_drom_data.transparent_intensity = intensity;

    if (dde_x11_XFDI_drom_data.transparent_surface_pass) {
	intens = 1.0 - intensity;
	intens = max(0.0001, intens);
	intens = min(0.9999, intens);
	XFDISetSurfaceTransparency(dde_x11_XFDI_drom_curdevdat->display,
				   dde_x11_XFDI_drom_curdevdat->gc,
				   intens);

    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_transpswi(switchvalue)
     DtSwitch switchvalue;
{
    dde_x11_XFDI_drom_data.transparent_switch = switchvalue;

    if (dde_x11_XFDI_drom_pre_init) return;

    if (!dde_x11_XFDI_drom_data.transparent_surface_pass) {
	if (dde_x11_XFDI_drom_data.transparent_switch) {
	    dde_x11_XFDI_drom_data.transparent_surface_flag = DcTrue;

	    DDcondex_SetDisablePrimExecFlag(DcTrue);
	} else {
	    DDcondex_SetDisablePrimExecFlag(DcFalse);
	}
    } else {
	if (dde_x11_XFDI_drom_data.transparent_switch) {
	    DDcondex_SetDisablePrimExecFlag(DcFalse);
	} else {
	    DDcondex_SetDisablePrimExecFlag(DcTrue);
	}
    }
}
