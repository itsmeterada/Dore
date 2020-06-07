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
	void ddr_vll_drom_update_surface_properties ()
	void ddr_vll_drom_set_att_ambint (intensity)
	void ddr_vll_drom_set_att_ambswi (switchvalue)
	void ddr_vll_drom_set_att_bacfacculble (switchvalue)
	void ddr_vll_drom_set_att_bacfacculswi (switchvalue)
	void ddr_vll_drom_set_att_depcue (zfront,zback,sfront,sback,colormodel,color)
	void ddr_vll_drom_set_att_depcueswi (switchvalue)
	void ddr_vll_drom_set_att_difswi (switchvalue)
	void ddr_vll_drom_set_att_difclr (colormodel, color)
	void ddr_vll_drom_set_att_difint (intensity)
	void ddr_vll_drom_set_att_hidsrfswi (switchvalue)
	void ddr_vll_drom_set_att_inttyp (interptype)
	void ddr_vll_drom_set_att_lintyp (type)
	void ddr_vll_drom_set_att_linwid (width)
	void ddr_vll_drom_set_att_localaasty (style)
	void ddr_vll_drom_set_att_localaaswi (switchvalue)
	void ddr_vll_drom_set_att_refswi (switchvalue)
	void ddr_vll_drom_set_att_reptyp (reptype)
	void ddr_vll_drom_set_att_shaswi (switchvalue)
	void ddr_vll_drom_set_att_shdidx (shadeindex)
	void ddr_vll_drom_set_att_spcclr (colormodel, color)
	void ddr_vll_drom_set_att_spcfct (factor)
	void ddr_vll_drom_set_att_spcint (intensity)
	void ddr_vll_drom_set_att_spcswi (switchvalue)
	void ddr_vll_drom_set_att_srfedgclr (colormodel, color)
	void ddr_vll_drom_set_att_srfshd (callbackobj)
	void ddr_vll_drom_set_att_transpclr (colormodel, color)
	void ddr_vll_drom_set_att_transpint (intensity)
	void ddr_vll_drom_set_att_transpswi (switchvalue)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
void
ddr_vll_drom_update_surface_properties()
{
    float ambintens, difintens, spcintens;

    if (dde_vll_drom_data.ambient_switch) {
	ambintens = min(dde_vll_drom_data.ambient_intensity,.99);
    } else {
	ambintens = 0.;
    }
    if (dde_vll_drom_data.diffuse_switch) {
	difintens = min(dde_vll_drom_data.diffuse_intensity,.99);
    } else {
	difintens = 0.;
    }
    if (dde_vll_drom_data.specular_switch) {
	spcintens = min(dde_vll_drom_data.specular_intensity,.99);
    } else {
	spcintens = 0.;
    }

    VLLset_material(ambintens, difintens, spcintens, 
		    dde_vll_drom_data.specular_factor);

    if (dde_vll_drom_data.constant_shading_switch) {
	VLLset_lighting_model(VLL_NO_LIGHTING);
    } else if (!dde_vll_drom_data.specular_switch) {
	VLLset_lighting_model(VLL_DIFFUSE);
    } else {
	VLLset_lighting_model(VLL_SPECULAR);
    }
}
/*
 ======================================================================
 */
void
ddr_vll_drom_set_att_ambint(intensity)
	DtReal intensity;
{
    dde_vll_drom_data.ambient_intensity = intensity;

    if (!dde_vll_drom_pre_init) 
	    ddr_vll_drom_update_surface_properties();
}
/*
 ======================================================================
 */
void
ddr_vll_drom_set_att_ambswi(switchvalue)
	DtSwitch switchvalue;
{
    dde_vll_drom_data.ambient_switch = switchvalue;

    if (!dde_vll_drom_pre_init) 
	    ddr_vll_drom_update_surface_properties();
}
/*
 ======================================================================
 */
void
ddr_vll_drom_set_att_bacfacculble(switchvalue)
	DtSwitch switchvalue;
{
    dde_vll_drom_data.backface_cullable_switch = switchvalue;

    if (!dde_vll_drom_pre_init) {
	if (dde_vll_drom_data.backface_cullable_switch &&
	    dde_vll_drom_data.backface_cull_switch) {
	    VLLset_backface_eliminate_flag(VLL_BACK_COUNTERCLOCKWISE);
	    VLLset_backface_style(VLL_BACK_CULL);
	} else {
	    VLLset_backface_style(VLL_BACK_FLIP);
	    VLLset_backface_eliminate_flag(VLL_BACK_COUNTERCLOCKWISE);
	}
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_bacfacculswi(switchvalue)
	DtSwitch switchvalue;
{
    dde_vll_drom_data.backface_cull_switch = switchvalue;

    if (!dde_vll_drom_pre_init) {
	if (dde_vll_drom_data.backface_cullable_switch &&
	    dde_vll_drom_data.backface_cull_switch) {
	    VLLset_backface_eliminate_flag(VLL_BACK_COUNTERCLOCKWISE);
	    VLLset_backface_style(VLL_BACK_CULL);
	} else {
	    VLLset_backface_style(VLL_BACK_FLIP);
	    VLLset_backface_eliminate_flag(VLL_BACK_COUNTERCLOCKWISE);
	}
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_depcue(zfront,zback,sfront,sback,colormodel,color)
	DtReal zfront, zback;
	DtReal sfront, sback;
	DtColorModel colormodel;
	DtReal *color;
{
    float localcolor[3];

    localcolor[0] = min(color[0], .99);
    localcolor[1] = min(color[1], .99);
    localcolor[2] = min(color[2], .99);

    VLLset_depth_cueing ((float)zfront, (float)zback, 
			 (float)sfront, (float)sback, localcolor);
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_depcueswi(switchvalue)
	DtSwitch switchvalue;
{
    VLLset_depth_cueing_flag(switchvalue == DcTrue ? 1 : 0);
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_difswi(switchvalue)
	DtSwitch switchvalue;
{
    dde_vll_drom_data.diffuse_switch = switchvalue;

    if (!dde_vll_drom_pre_init) 
	    ddr_vll_drom_update_surface_properties();
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_difclr(colormodel, color)
DtColorModel colormodel;
DtReal color[];
{
    dde_vll_drom_data.diffuse_color[0] = min(color[0], .99);
    dde_vll_drom_data.diffuse_color[1] = min(color[1], .99);
    dde_vll_drom_data.diffuse_color[2] = min(color[2], .99);

    VLLset_diffuse_color(dde_vll_drom_data.diffuse_color);
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_difint(intensity)
	DtReal intensity;
{
    dde_vll_drom_data.diffuse_intensity = intensity;

    if (!dde_vll_drom_pre_init) 
	    ddr_vll_drom_update_surface_properties();
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_hidsrfswi(switchvalue)
	DtSwitch switchvalue;
{
    dde_vll_drom_data.hidden_surf_switch = switchvalue;

    ddr_vll_dcm_set_depth_buffer_write(switchvalue);
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_inttyp(interptype)
	DtInterpType interptype;
{
    dde_vll_drom_data.interptype = interptype;

    switch (interptype) {
    case DcConstantShade:
	VLLset_shading(VLL_SHADE_CONSTANT);
	break;
    case DcVertexShade:
	VLLset_shading(VLL_SHADE_VERTEX);
	break;
    case DcSurfaceShade:
	VLLset_shading(VLL_SHADE_NORMAL);
	break;
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_lintyp (type)
	DtLineType type;
{
    switch (type) {
    case DcLineTypeDash:	break;
    case DcLineTypeDot:		break;
    case DcLineTypeDotDash:	break;
    case DcLineTypeSolid:	break;
    default:			break;
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_linwid (width)
	DtReal width;
{
    int iwidth;

    if (width < 1.5) {
	iwidth = 1;
    } else if (width < 3.0) {
	iwidth = 2;
    } else {
	iwidth = 4;
    }
    VLLset_line_width(iwidth, 1);
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_localaasty(style)
	DtLocalAntiAliasStyle style;
{
    dde_vll_drom_data.localaa_style = style;

    switch (style) {
    case DcLocalAntiAliasFast:
    case DcLocalAntiAliasIntermediate:
	VLLset_line_filter_width(VLL_LINE_FILTER_SMALL);
	break;

    case DcLocalAntiAliasBest:
	VLLset_line_filter_width(VLL_LINE_FILTER_LARGE);
	break;
    }
}
/*
 ======================================================================
 */
void
ddr_vll_drom_set_att_localaaswi(switchvalue)
	DtSwitch switchvalue;
{
    dde_vll_drom_data.localaa_switch = switchvalue;

    VLLset_line_filter(switchvalue ? 1 : 0);
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_refswi(switchvalue)
	DtSwitch switchvalue;
{
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_reptyp(reptype)
	DtRepType reptype;
{
    switch (reptype) {
    case DcPoints:
	VLLset_reptype(VLL_REPTYPE_POINT);
	break;
    case DcWireframe:
    case DcOutlines:
	VLLset_reptype(VLL_REPTYPE_LINE);
	break;
    case DcSurface:
	VLLset_reptype(VLL_REPTYPE_SURFACE);
	break;
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
	VLLset_reptype(VLL_REPTYPE_SURFACE_W_OUTLINE);
	break;
    }

    dde_vll_drom_data.reptype = reptype;
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_shaswi(switchvalue)
	DtSwitch switchvalue;
{
    dde_vll_drom_data.shadow_switch = switchvalue;

    if (dde_vll_drom_pre_init) return;

    if ((dde_vll_drom_curdevdat->features->shadows) &&
	(dde_vll_drom_data.traversal_type != ddd_vll_drom_shadow_pass) &&
	(dde_vll_drom_data.shadow_switch)) {
	dde_vll_drom_data.have_shadows = DcTrue;
    }
      
    ddr_vll_drom_set_prim_execute();
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_shdidx(shadeindex)
	DtInt shadeindex;

{
    DtInt shaderange[2];
    DtInt ncolors;

    dde_vll_drom_data.shade_index = shadeindex;

    ddr_vll_dcm_inquire_ncolors(&ncolors);

    shaderange[0] = -10;
    shaderange[1] = -10;
    DDdevice_InqShadeRange (dde_vll_drom_data.current_device,
			    dde_vll_drom_data.shade_index,
			    &shaderange[0], &shaderange[1]);
    if (shaderange[0] != -10 && shaderange[1] != -10) {
	dde_vll_drom_curdevdat->pseudo_range[0] = shaderange[0];
	dde_vll_drom_curdevdat->pseudo_range[1] = shaderange[1];
    }
    if (dde_vll_drom_curdevdat->visual_type == DcPseudoColor) {
	VLLset_pseudo_range(dde_vll_drom_curdevdat->pseudo_range_weight,
			    dde_vll_drom_curdevdat->pseudo_range,
			    (unsigned)ncolors);
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_spcclr(colormodel, color)
DtColorModel colormodel;
DtReal color[];
{
    DtColorRGB localcolor;
    extern unsigned long ddr_vll_drom_get_pixel_value();

    dde_vll_drom_data.specular_color[0] = min(color[0], .99);
    dde_vll_drom_data.specular_color[1] = min(color[1], .99);
    dde_vll_drom_data.specular_color[2] = min(color[2], .99);

    VLLset_specular_color(dde_vll_drom_data.specular_color);
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_spcfct(factor)
	DtReal factor;
{
    dde_vll_drom_data.specular_factor = factor;

    if (!dde_vll_drom_pre_init)
	    ddr_vll_drom_update_surface_properties();
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_spcint(intensity)
	DtReal intensity;
{
    dde_vll_drom_data.specular_intensity = intensity;

    if (!dde_vll_drom_pre_init) 
	    ddr_vll_drom_update_surface_properties();
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_spcswi(switchvalue)
	DtSwitch switchvalue;
{
    dde_vll_drom_data.specular_switch = switchvalue;

    if (!dde_vll_drom_pre_init) 
	    ddr_vll_drom_update_surface_properties();
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_srfedgclr(colormodel, color)
DtColorModel colormodel;
DtReal color[];
{
    DtColorRGB localcolor;

    dde_vll_drom_data.surface_edge_color[0] = min(color[0], .99);
    dde_vll_drom_data.surface_edge_color[1] = min(color[1], .99);
    dde_vll_drom_data.surface_edge_color[2] = min(color[2], .99);

    VLLset_edge_color(dde_vll_drom_data.surface_edge_color);
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_srfshd(callbackobj)
	DtObject callbackobj;
{
    dde_vll_drom_data.surface_shader = callbackobj;

    dde_vll_drom_data.constant_shading_switch = 
	    (callbackobj == DcShaderConstant);

    ddr_vll_drom_update_surface_properties();
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_transpclr(colormodel, color)
DtColorModel colormodel;
DtReal color[];
{
    dde_vll_drom_data.transparent_color[0] = color[0];
    dde_vll_drom_data.transparent_color[1] = color[1];
    dde_vll_drom_data.transparent_color[2] = color[2];
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_transpint(intensity)
     DtReal intensity;
{
    dde_vll_drom_data.transparent_intensity = intensity;

    if (dde_vll_drom_data.traversal_type == ddd_vll_drom_transp_pass) {
	VLLset_transparency(dde_vll_drom_data.transparent_intensity);
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_transpswi(switchvalue)
	DtSwitch switchvalue;
{
    dde_vll_drom_data.transparent_switch = switchvalue;

    if (dde_vll_drom_pre_init) return;

    if ((dde_vll_drom_data.traversal_type != ddd_vll_drom_transp_pass) &&
	(dde_vll_drom_data.transparent_switch)) {
	dde_vll_drom_data.have_transparency = DcTrue;
    }

    ddr_vll_drom_set_prim_execute();
}
/*
 ======================================================================
 */
