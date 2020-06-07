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
	void ddr_g4vll_drom_update_surface_properties ()
	void ddr_g4vll_drom_set_att_ambint (intensity)
	void ddr_g4vll_drom_set_att_ambswi (switchvalue)
	void ddr_g4vll_drom_set_att_bacfacculble (switchvalue)
	void ddr_g4vll_drom_set_att_bacfacculswi (switchvalue)
	void ddr_g4vll_drom_set_att_depcue (zfront,zback, sfront,sback,
			colormodel,color)
	void ddr_g4vll_drom_set_att_depcueswi (switchvalue)
	void ddr_g4vll_drom_set_att_difswi (switchvalue)
	void ddr_g4vll_drom_set_att_difclr (colormodel, color)
	void ddr_g4vll_drom_set_att_difint (intensity)
	void ddr_g4vll_drom_set_att_hidsrfswi (switchvalue)
	void ddr_g4vll_drom_set_att_inttyp (interptype)
	void ddr_g4vll_drom_set_att_lintyp (type)
	void ddr_g4vll_drom_set_att_linwid (width)
	void ddr_g4vll_drom_set_att_localaasty (style)
	void ddr_g4vll_drom_set_att_localaaswi (switchvalue)
	void ddr_g4vll_drom_set_att_refswi (switchvalue)
	void ddr_g4vll_drom_set_att_reptyp (reptype)
	void ddr_g4vll_drom_set_att_shaswi (switchvalue)
	void ddr_g4vll_drom_set_att_shdidx (shadeindex)
	void ddr_g4vll_drom_set_att_spcclr (colormodel, color)
	void ddr_g4vll_drom_set_att_spcfct (factor)
	void ddr_g4vll_drom_set_att_spcint (intensity)
	void ddr_g4vll_drom_set_att_spcswi (switchvalue)
	void ddr_g4vll_drom_set_att_srfedgclr (colormodel, color)
	void ddr_g4vll_drom_set_att_srfshd (callbackobj)
	void ddr_g4vll_drom_set_att_transpclr (colormodel, color)
	void ddr_g4vll_drom_set_att_transpint (intensity)
	void ddr_g4vll_drom_set_att_transpswi (switchvalue)
	void ddr_g4vll_drom_set_att_transporientclr (colormodel, color)
	void ddr_g4vll_drom_set_att_transporientexp (exponent)
	void ddr_g4vll_drom_set_att_transporientint (intensity)
	void ddr_g4vll_drom_set_att_transporientswi (switchvalue)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
void ddr_g4vll_drom_update_surface_properties (void)
{
    float ambintens, difintens, spcintens;

    if (dde_g4vll_drom_data.ambient_switch) {
	ambintens = min(dde_g4vll_drom_data.ambient_intensity,.99);
    } else {
	ambintens = 0.;
    }
    if (dde_g4vll_drom_data.diffuse_switch) {
	difintens = min(dde_g4vll_drom_data.diffuse_intensity,.99);
    } else {
	difintens = 0.;
    }
    if (dde_g4vll_drom_data.specular_switch) {
	spcintens = min(dde_g4vll_drom_data.specular_intensity,.99);
    } else {
	spcintens = 0.;
    }

    VLLset_material(ambintens, difintens, spcintens, 
		    dde_g4vll_drom_data.specular_factor);

    if (dde_g4vll_drom_data.constant_shading_switch) {
	VLLset_lighting_model(VLL_NO_LIGHTING);
#ifdef DEBUG
	printf ("VLLset_lighting_model(VLL_NO_LIGHTING)\n");
#endif
    } else if (!dde_g4vll_drom_data.specular_switch) {
	VLLset_lighting_model(VLL_DIFFUSE);
#ifdef DEBUG
	printf ("VLLset_lighting_model(VLL_DIFFUSE)\n");
#endif
    } else {
	VLLset_lighting_model(VLL_SPECULAR);
#ifdef DEBUG
	printf ("VLLset_lighting_model(VLL_SPECULAR)\n");
#endif
    }
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_ambint (
    DtReal intensity)
{
    dde_g4vll_drom_data.ambient_intensity = intensity;

    if (!dde_g4vll_drom_data.pre_init)
	    ddr_g4vll_drom_update_surface_properties();
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_ambswi (
    DtSwitch switchvalue)
{
    dde_g4vll_drom_data.ambient_switch = switchvalue;

    if (!dde_g4vll_drom_data.pre_init) 
	    ddr_g4vll_drom_update_surface_properties();
}
/*
 ======================================================================
 */

void ddr_g4vll_drom_set_att_bacfacculble (
    DtSwitch switchvalue)
{
    dde_g4vll_drom_data.backface_cullable_switch = switchvalue;

    if (!dde_g4vll_drom_data.pre_init) {
	if (dde_g4vll_drom_data.backface_cullable_switch &&
	    dde_g4vll_drom_data.backface_cull_switch) {
	    VLLset_cull_mode(VLL_CULL_BACK);
	} else {
	    VLLset_cull_mode(VLL_CULL_NONE);
	}
	VLLset_backface_eliminate_flag(VLL_BACK_CLOCKWISE);
	VLLset_backface_distinguish(VLL_DISABLE);
	VLLset_normal_invert(VLL_ENABLE);
    }
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_bacfacculswi (
    DtSwitch switchvalue)
{
    dde_g4vll_drom_data.backface_cull_switch = switchvalue;

    if (!dde_g4vll_drom_data.pre_init) {
	if (dde_g4vll_drom_data.backface_cullable_switch &&
	    dde_g4vll_drom_data.backface_cull_switch) {
	    VLLset_cull_mode(VLL_CULL_BACK);
	} else {
	    VLLset_cull_mode(VLL_CULL_NONE);
	}
	VLLset_backface_eliminate_flag(VLL_BACK_CLOCKWISE);
	VLLset_backface_distinguish(VLL_DISABLE);
	VLLset_normal_invert(VLL_ENABLE);
    }
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_depcue (
    DtReal zfront,
    DtReal zback,
    DtReal sfront,
    DtReal sback,
    DtColorModel colormodel,
    DtReal *color)
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
void ddr_g4vll_drom_set_att_depcueswi (
    DtSwitch switchvalue)
{
    VLLset_depth_cueing_flag(switchvalue == DcTrue ? 1 : 0);
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_difswi (
    DtSwitch switchvalue)
{
    dde_g4vll_drom_data.diffuse_switch = switchvalue;

    if (!dde_g4vll_drom_data.pre_init)
	ddr_g4vll_drom_update_surface_properties();
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_difclr (
    DtColorModel colormodel,
    DtReal color[])
{
    dde_g4vll_drom_data.diffuse_color[0] = min(color[0], .99);
    dde_g4vll_drom_data.diffuse_color[1] = min(color[1], .99);
    dde_g4vll_drom_data.diffuse_color[2] = min(color[2], .99);

    VLLset_diffuse_color(dde_g4vll_drom_data.diffuse_color);
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_difint (
    DtReal intensity)
{
    dde_g4vll_drom_data.diffuse_intensity = intensity;

    if (!dde_g4vll_drom_data.pre_init)
	ddr_g4vll_drom_update_surface_properties();
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_hidsrfswi (
    DtSwitch switchvalue)
{
    dde_g4vll_drom_data.hidden_surf_switch = switchvalue;

    ddr_g4vll_dcm_set_depth_buffer_write(switchvalue);
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_inttyp (
    DtInterpType interptype)
{
    dde_g4vll_drom_data.interptype = interptype;

    switch (interptype) {
    case DcConstantShade:
#ifdef DEBUG
	printf ("VLLset_shading(VLL_SHADE_CONSTANT)\n");
#endif
	VLLset_shading(VLL_SHADE_CONSTANT);
	break;
    case DcVertexShade:
#ifdef DEBUG
	printf ("VLLset_shading(VLL_SHADE_VERTEX)\n");
#endif
	VLLset_shading(VLL_SHADE_VERTEX);
	break;
    case DcSurfaceShade:
#ifdef DEBUG
	printf ("VLLset_shading(VLL_SHADE_NORMAL)\n");
#endif
	VLLset_shading(VLL_SHADE_NORMAL);
	break;
    }
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_lintyp (
    DtLineType type)
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
void ddr_g4vll_drom_set_att_linwid (
    DtReal width)
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
void ddr_g4vll_drom_set_att_localaasty (
    DtLocalAntiAliasStyle style)
{
    dde_g4vll_drom_data.localaa_style = style;

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
void ddr_g4vll_drom_set_att_localaaswi (
    DtSwitch switchvalue)
{
    dde_g4vll_drom_data.localaa_switch = switchvalue;

    VLLset_line_filter(switchvalue ? 1 : 0);
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_refswi (
    DtSwitch switchvalue)
{
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_reptyp (
    DtRepType reptype)
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

    dde_g4vll_drom_data.reptype = reptype;
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_shaswi (
    DtSwitch switchvalue)
{
    dde_g4vll_drom_data.shadow_switch = switchvalue;

    if (dde_g4vll_drom_data.pre_init) return;

    if ((dde_g4vll_drom_curdevdat->features->shadows) &&
	(dde_g4vll_drom_data.traversal_type != ddd_g4vll_drom_shadow_pass) &&
	(dde_g4vll_drom_data.shadow_switch)) {
	dde_g4vll_drom_data.have_shadows = DcTrue;
    }
      
    ddr_g4vll_drom_set_prim_execute();
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_shdidx (
    DtInt shadeindex)

{
    DtInt shaderange[2];
    DtInt ncolors;

    dde_g4vll_drom_data.shade_index = shadeindex;

    ddr_g4vll_dcm_inquire_ncolors(&ncolors);

    shaderange[0] = -10;
    shaderange[1] = -10;
    DDdevice_InqShadeRange (dde_g4vll_drom_data.current_device,
			    dde_g4vll_drom_data.shade_index,
			    &shaderange[0], &shaderange[1]);
    if (shaderange[0] != -10 && shaderange[1] != -10) {
	dde_g4vll_drom_curdevdat->pseudo_range[0] = shaderange[0];
	dde_g4vll_drom_curdevdat->pseudo_range[1] = shaderange[1];
    }
    if (dde_g4vll_drom_curdevdat->visual_type == DcPseudoColor) {
	if (ncolors > 256) {
	    VLLset_pseudo_range(dde_g4vll_drom_curdevdat->pseudo_range_weight,
		dde_g4vll_drom_curdevdat->pseudo_range, VLL_PSEUDO_TYPE_12);
	} else {
	    VLLset_pseudo_range(dde_g4vll_drom_curdevdat->pseudo_range_weight,
		dde_g4vll_drom_curdevdat->pseudo_range, VLL_PSEUDO_TYPE_RAMDAC);
	}
    }
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_spcclr (
    DtColorModel colormodel,
    DtReal color[])
{
    dde_g4vll_drom_data.specular_color[0] = min(color[0], .99);
    dde_g4vll_drom_data.specular_color[1] = min(color[1], .99);
    dde_g4vll_drom_data.specular_color[2] = min(color[2], .99);

    VLLset_specular_color(dde_g4vll_drom_data.specular_color);
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_spcfct (
    DtReal factor)
{
    dde_g4vll_drom_data.specular_factor = factor;

    if (!dde_g4vll_drom_data.pre_init)
	    ddr_g4vll_drom_update_surface_properties();
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_spcint (
    DtReal intensity)
{
    dde_g4vll_drom_data.specular_intensity = intensity;

    if (!dde_g4vll_drom_data.pre_init)
	    ddr_g4vll_drom_update_surface_properties();
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_spcswi (
    DtSwitch switchvalue)
{
    dde_g4vll_drom_data.specular_switch = switchvalue;

    if (!dde_g4vll_drom_data.pre_init)
	    ddr_g4vll_drom_update_surface_properties();
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_srfedgclr (
    DtColorModel colormodel,
    DtReal color[])
{
    dde_g4vll_drom_data.surface_edge_color[0] = min(color[0], .99);
    dde_g4vll_drom_data.surface_edge_color[1] = min(color[1], .99);
    dde_g4vll_drom_data.surface_edge_color[2] = min(color[2], .99);

    VLLset_edge_color(dde_g4vll_drom_data.surface_edge_color);
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_srfshd (
    DtObject callbackobj)
{
    dde_g4vll_drom_data.surface_shader = callbackobj;

    dde_g4vll_drom_data.constant_shading_switch = 
	    (callbackobj == DcShaderConstant);

    ddr_g4vll_drom_update_surface_properties();
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_transpclr (
    DtColorModel colormodel,
    DtReal color[])
{
    dde_g4vll_drom_data.transparent_color[0] = color[0];
    dde_g4vll_drom_data.transparent_color[1] = color[1];
    dde_g4vll_drom_data.transparent_color[2] = color[2];
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_transpint (
    DtReal intensity)
{
    dde_g4vll_drom_data.transparent_intensity = intensity;

    if (dde_g4vll_drom_data.traversal_type == ddd_g4vll_drom_transp_pass) {
	VLLset_transparency(dde_g4vll_drom_data.transparent_intensity);
    }
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_transpswi (
    DtSwitch switchvalue)
{
    dde_g4vll_drom_data.transparent_switch = switchvalue;

    if (dde_g4vll_drom_data.pre_init) return;

    if ((dde_g4vll_drom_data.traversal_type != ddd_g4vll_drom_transp_pass) &&
	(dde_g4vll_drom_data.transparent_switch)) {
#ifdef DEBUG_TRANSP
printf ("\tJust found Transparency to do next in transp_pass\n");
#endif
	dde_g4vll_drom_data.have_transparency = DcTrue;
    }

    ddr_g4vll_drom_set_prim_execute();
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_transporientclr (
    DtColorModel colormodel,
    DtReal color[])
{
    dde_g4vll_drom_data.transporient_color[0] = color[0];
    dde_g4vll_drom_data.transporient_color[1] = color[1];
    dde_g4vll_drom_data.transporient_color[2] = color[2];
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_transporientexp (
    DtReal exponent)
{
    dde_g4vll_drom_data.transporient_exp = exponent;

    if (dde_g4vll_drom_data.traversal_type == ddd_g4vll_drom_transp_pass) {
	VLLset_orient_dep_transp(dde_g4vll_drom_data.transporient_intensity,
		dde_g4vll_drom_data.transporient_exp);
    }
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_transporientint (
    DtReal intensity)
{
    dde_g4vll_drom_data.transporient_intensity = intensity;

    if (dde_g4vll_drom_data.traversal_type == ddd_g4vll_drom_transp_pass) {
	VLLset_orient_dep_transp(dde_g4vll_drom_data.transporient_intensity,
		dde_g4vll_drom_data.transporient_exp);
    }
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_set_att_transporientswi (
    DtSwitch switchvalue)
{
    if (dde_g4vll_drom_data.traversal_type == ddd_g4vll_drom_transp_pass) {
	VLLset_orient_dep_transp_enable(switchvalue == DcTrue ? 1 : 0);
    }
}
/*
 ======================================================================
 */
