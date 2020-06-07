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
	int ddr_x11_DGL_drom_update_state ()
	int ddr_x11_DGL_drom_set_att_ambint (intensity)
	int ddr_x11_DGL_drom_set_att_ambswi (switchvalue)
	int ddr_x11_DGL_drom_set_att_bacfacculble (switchvalue)
	int ddr_x11_DGL_drom_set_att_bacfacculswi (switchvalue)
	int ddr_x11_DGL_drom_set_att_depcue (zfront,zback,sfront,sback,colormodel,color)
	int ddr_x11_DGL_drom_set_att_depcueswi (switchvalue)
	int ddr_x11_DGL_drom_set_att_difswi (switchvalue)
	int ddr_x11_DGL_drom_set_att_difclr (colormodel, color)
	int ddr_x11_DGL_drom_set_att_difint (intensity)
	int ddr_x11_DGL_drom_set_att_hidsrfswi (switchvalue)
	int ddr_x11_DGL_drom_set_att_inttyp (interptype)
	int ddr_x11_DGL_drom_set_att_lintyp (type)
	int ddr_x11_DGL_drom_set_att_linwid (width)
	int ddr_x11_DGL_drom_set_att_refswi (switchvalue)
	int ddr_x11_DGL_drom_set_att_reptyp (reptype)
	int ddr_x11_DGL_drom_set_att_shaswi (switchvalue)
	int ddr_x11_DGL_drom_set_att_shdidx (shadeindex)
	int ddr_x11_DGL_drom_set_att_spcclr (colormodel, color)
	int ddr_x11_DGL_drom_set_att_spcfct (factor)
	int ddr_x11_DGL_drom_set_att_spcint (intensity)
	int ddr_x11_DGL_drom_set_att_spcswi (switchvalue)
	int ddr_x11_DGL_drom_set_att_srfshd (callbackobj)
	int ddr_x11_DGL_drom_set_att_transpclr (colormodel, color)
	int ddr_x11_DGL_drom_set_att_transpint (value)
	int ddr_x11_DGL_drom_set_att_transpswi (switchvalue)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
ddr_x11_DGL_drom_update_state()

{
    XdSetTransformationMode(dde_x11_DGL_drom_curdevdat->pGC,
			    XdTransform4x4);

    XdSetProjectionMatrix(dde_x11_DGL_drom_curdevdat->pGC,
			  dde_x11_DGL_drom_curcamdat->wcstofcsmat);
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_ambint(intensity)
     DtReal intensity;

{
    XdReal intens;

    dde_x11_DGL_drom_data.object_ambient_intensity = intensity;

    if (dde_x11_DGL_drom_data.transparent_surface_pass) {
	intens = dde_x11_DGL_drom_data.object_ambient_intensity*
		(1.0 - dde_x11_DGL_drom_data.object_transparent_intensity);
    } else {
	intens = dde_x11_DGL_drom_data.object_ambient_intensity;
    }

    XdSetAmbientIntens(dde_x11_DGL_drom_curdevdat->pGC,
		       intens);
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_ambswi(switchvalue)
     DtSwitch switchvalue;

{
    XdSetAmbientSwitch(dde_x11_DGL_drom_curdevdat->pGC, 
		       (XdSwitch)switchvalue);
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_bacfacculble(switchvalue)
     DtSwitch switchvalue;
{
    dde_x11_DGL_drom_data.object_cullable_switch = switchvalue;

    if (dde_x11_DGL_drom_data.object_cull_switch &&
	dde_x11_DGL_drom_data.object_cullable_switch) {
	XdSetCullMode(dde_x11_DGL_drom_curdevdat->pGC, XdCullBack);
    } else {
	XdSetCullMode(dde_x11_DGL_drom_curdevdat->pGC, XdCullNone);
    }
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_bacfacculswi(switchvalue)
     DtSwitch switchvalue;
{
    dde_x11_DGL_drom_data.object_cull_switch = switchvalue;

    if (dde_x11_DGL_drom_data.object_cull_switch &&
	dde_x11_DGL_drom_data.object_cullable_switch) {
	XdSetCullMode(dde_x11_DGL_drom_curdevdat->pGC, XdCullBack);
    } else {
	XdSetCullMode(dde_x11_DGL_drom_curdevdat->pGC, XdCullNone);
    }
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_depcue(zfront,zback,sfront,sback,colormodel,color)
     DtReal zfront, zback;
     DtReal sfront, sback;
     DtColorModel colormodel;
     DtReal *color;

{
    XdColor Xcolor;

    if (colormodel == DcRGB) {
	Xcolor.r = color[0];
	Xcolor.g = color[1];
	Xcolor.b = color[2];

	XdSetDepthCueing(dde_x11_DGL_drom_curdevdat->pGC,
			 (XdReal)zfront, (XdReal)zback, 
			 (XdReal)sfront, (XdReal)sback, &Xcolor);
    }
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_depcueswi(switchvalue)
     DtSwitch switchvalue;

{
    XdSetDepthCueSwitch(dde_x11_DGL_drom_curdevdat->pGC,
			(XdSwitch)switchvalue);
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_difswi(switchvalue)
     DtSwitch switchvalue;

{
    XdSetDiffuseSwitch(dde_x11_DGL_drom_curdevdat->pGC,
		       (XdSwitch)switchvalue);
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_difclr(colormodel, color)
DtColorModel colormodel;
DtReal color[];

{

    dde_x11_DGL_drom_data.object_diffuse_color.r = color[0];
    dde_x11_DGL_drom_data.object_diffuse_color.g = color[1];
    dde_x11_DGL_drom_data.object_diffuse_color.b = color[2];

    XdSetDiffuseColor(dde_x11_DGL_drom_curdevdat->pGC,
		      &(dde_x11_DGL_drom_data.object_diffuse_color));
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_difint(intensity)
     DtReal intensity;

{
    XdReal intens;

    dde_x11_DGL_drom_data.object_diffuse_intensity = intensity;

    if (dde_x11_DGL_drom_data.transparent_surface_pass) {
	intens = dde_x11_DGL_drom_data.object_diffuse_intensity*
		(1.0 - dde_x11_DGL_drom_data.object_transparent_intensity);
    } else {
	intens = dde_x11_DGL_drom_data.object_diffuse_intensity;
    }

    XdSetDiffuseIntens(dde_x11_DGL_drom_curdevdat->pGC,
		       intens);
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_hidsrfswi(switchvalue)
     DtSwitch switchvalue;

{
    dde_x11_DGL_drom_data.object_hidsurf_switch = switchvalue;

    ddr_x11_DGL_dcm_set_depth_buffer_enable(switchvalue);
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_inttyp(interptype)
     DtInterpType interptype;

{
    if (interptype == DcConstantShade) {
	XdSetShadeType(dde_x11_DGL_drom_curdevdat->pGC, XdConstantShade);
    } else {
	XdSetShadeType(dde_x11_DGL_drom_curdevdat->pGC, XdVertexShade);
    }
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_lintyp (type)
     DtLineType type;
{
    static char dot_list[] = {1, 1};
    static char dash_list[] = {4, 4};
    static char dotdash_list[] = {1, 5, 5, 5};

    switch (type) {
    case DcLineTypeSolid:
	XdSetLineStyle(dde_x11_DGL_drom_curdevdat->pGC,
		       LineSolid);
	break;

    case DcLineTypeDash:
	XdSetLineStyle(dde_x11_DGL_drom_curdevdat->pGC,
		       LineOnOffDash);
	XdSetDashes(dde_x11_DGL_drom_curdevdat->pGC,
		    0, dash_list, 2);
	break;

    case DcLineTypeDot:
	XdSetLineStyle(dde_x11_DGL_drom_curdevdat->pGC,
		       LineOnOffDash);
	XdSetDashes(dde_x11_DGL_drom_curdevdat->pGC,
		    0, dot_list, 2);
	break;

    case DcLineTypeDotDash:
	XdSetLineStyle(dde_x11_DGL_drom_curdevdat->pGC,
		       LineOnOffDash);
	XdSetDashes(dde_x11_DGL_drom_curdevdat->pGC,
		    0, dotdash_list, 4);
	break;
    }
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_linwid (width)
     DtReal width;
{
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_refswi(switchvalue)
     DtSwitch switchvalue;
{
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_reptyp(reptype)
     DtRepType reptype;

{
    switch (reptype) {
    case DcPoints:
	XdSetRepType (dde_x11_DGL_drom_curdevdat->pGC,
		      XdPoints);
	break;

    case DcWireframe:
    case DcOutlines:
	XdSetRepType (dde_x11_DGL_drom_curdevdat->pGC,
		      XdWireframe);
	break;
	/*
	  case DcOutlines:
	  XdSetRepType (dde_x11_DGL_drom_curdevdat->pGC,
	  XdOutline);
	  break;
	  */
    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
	XdSetRepType (dde_x11_DGL_drom_curdevdat->pGC,
		      XdSurface);
	break;
    }
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_shaswi(switchvalue)
     DtSwitch switchvalue;
{
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_shdidx(shadeindex)
     DtInt shadeindex;

{
    DtInt shaderange[2];
    XdReal *cwtavgpsweight;

    dde_x11_DGL_drom_data.object_shade_index = shadeindex;
   
    ddr_x11_DGL_dcm_inq_pseudocolor_weighted_average_weights(&cwtavgpsweight);

#ifdef DEBUG
    printf ("ddr_x11_DGL_drom_set_att_shdidx (%d)\n", 
	    dde_x11_DGL_drom_data.object_shade_index);
#endif

    if (dde_x11_DGL_drom_pre_init) {
	return;
    }

    DdInqShadeRanges (dde_x11_DGL_drom_data.current_device,
				dde_x11_DGL_drom_data.object_shade_index,
				1,shaderange);
    dde_x11_DGL_drom_curdevdat->current_shade_range_min = shaderange[0];
    dde_x11_DGL_drom_curdevdat->current_shade_range_max = shaderange[1];
   
    if (dde_x11_DGL_drom_data.pseudocolor_switch) {
	if (dde_x11_DGL_drom_curdevdat->shade_mode == DcRange) {
	    XdSetShadeRange(dde_x11_DGL_drom_curdevdat->pGC,
			    cwtavgpsweight, 
			    (int)shaderange[0], (int)shaderange[1]);
	}
    }
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_spcclr(colormodel, color)
DtColorModel colormodel;
DtReal color[];

{
    XdColor Xcolor;

    Xcolor.r = color[0];
    Xcolor.g = color[1];
    Xcolor.b = color[2];

    XdSetSpecularColor(dde_x11_DGL_drom_curdevdat->pGC,
		       &Xcolor);
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_spcfct(factor)
     DtReal factor;

{
    static XdReal last_factor;
    static DtFlag first_time = DcTrue;

    dde_x11_DGL_drom_data.object_specular_factor = factor;

    if (first_time || (XdReal)factor != last_factor) {
	first_time = DcFalse;
	last_factor = factor;
	XdSetSpecularExponent(dde_x11_DGL_drom_curdevdat->pGC, (XdReal)factor);
    }
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_spcint(intensity)
     DtReal intensity;

{
    XdSetSpecularIntens(dde_x11_DGL_drom_curdevdat->pGC,
			(XdReal)intensity);
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_spcswi(switchvalue)
     DtSwitch switchvalue;

{
    XdSetSpecularSwitch(dde_x11_DGL_drom_curdevdat->pGC,
			(XdSwitch)switchvalue);
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_srfshd(callbackobj)
     DtObject callbackobj;

{
    if (callbackobj == DcShaderConstant) {
	XdSetLightSwitch(dde_x11_DGL_drom_curdevdat->pGC,
			 XdOff);
    } else {
	XdSetLightSwitch(dde_x11_DGL_drom_curdevdat->pGC,
			 XdOn);
    }

    dde_x11_DGL_drom_data.object_surface_shader = callbackobj;
    dde_x11_DGL_drom_data.draw_switch = 
	    (dde_x11_DGL_drom_data.object_surface_shader == DcShaderConstant);

}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_transpclr(colormodel, color)
DtColorModel colormodel;
DtReal color[];

{
    dde_x11_DGL_drom_data.object_transparent_color.r = color[0];
    dde_x11_DGL_drom_data.object_transparent_color.g = color[1];
    dde_x11_DGL_drom_data.object_transparent_color.b = color[2];
}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_transpint(value)
     DtReal value;

{
    dde_x11_DGL_drom_data.object_transparent_intensity = value;
   
    /* We can't send down the altered ambient and diffuse components
     * to DGL yet as we don't know whether the transparency switch 
     * will be set.  So just compute these values and store'em away
     * for now ...
     */
    if (dde_x11_DGL_drom_data.transparent_surface_pass) {
	dde_x11_DGL_drom_data.object_transparent_diffuse_intensity = 
		dde_x11_DGL_drom_data.object_diffuse_intensity *
		(1.0 - dde_x11_DGL_drom_data.object_transparent_intensity);
    } 

    if (dde_x11_DGL_drom_data.transparent_surface_pass) {
	dde_x11_DGL_drom_data.object_transparent_ambient_intensity = 
		dde_x11_DGL_drom_data.object_ambient_intensity *
		(1.0 - dde_x11_DGL_drom_data.object_transparent_intensity);
    } 

}
/*
 ======================================================================
 */
ddr_x11_DGL_drom_set_att_transpswi(switchvalue)
     DtSwitch switchvalue;

{
    /* If we are not in the transparent surface rendering pass, then
     * indicate that at least one transparent surface was encountered
     * that needs be rendered in the next (transparent surface) pass.  
     */

    dde_x11_DGL_drom_data.object_transparent_switch = switchvalue;

    if (dde_x11_DGL_drom_pre_init) return;

    if (!dde_x11_DGL_drom_data.transparent_surface_pass) {
	if (dde_x11_DGL_drom_data.object_transparent_switch) {
	    dde_x11_DGL_drom_data.transparent_surface_flag = DcTrue;
	    DDcondex_SetDisablePrimExecFlag(DcTrue);
	} else {
	    DDcondex_SetDisablePrimExecFlag(DcFalse);
	}
    } else {
	if (dde_x11_DGL_drom_data.object_transparent_switch) {
	    DDcondex_SetDisablePrimExecFlag(DcFalse);
	} else {
	    DDcondex_SetDisablePrimExecFlag(DcTrue);
	}
    }
}
