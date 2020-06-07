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
 
/*****************************************************************************
**  This file contains functions that relate to appearance attributes.  They
**  are called in between the pre-initialization and the post-initialization
**  of a specific device to set the default attribute values, and then are
**  called subsequently for specific objects or groups of objects.
*****************************************************************************/

#include "../pex_driver.h"



/*****************************************************************************
**  The renderer calls this function whenever the ambient intensity changes.
**  `intensity' is the new ambient intensity in the range of [0,1].
**  Typically, the diffuse color and ambient intensity are multiplied to get
**  the ambient reflectance.  This reflectance is then used in the shading
**  calculations.
*****************************************************************************/

void  ddr_pex_drom_set_att_ambint  (DtReal intensity)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_ambint (%f)\n", intensity);
	fflush (stdout);
#   endif

    DROM.ambient_intens = intensity;

    if (dde_pex_drom_pre_init) return;

    PEX.pc_attrs.reflection_attr.ambient = DROM.ambient_switch ? intensity : 0;
    PEXSetReflectionAttributes
	(DCM->display, PEX.renderer,PEXOCRender, &PEX.pc_attrs.reflection_attr);
}



/*****************************************************************************
**  This routine sets the ambient light enable/disable flag.  If the parameter
**  is DcOn, then ambient intensity is factored into the illumination
**  equation, otherwise the ambient light contribution is ignored.
*****************************************************************************/

void  ddr_pex_drom_set_att_ambswi  (DtSwitch switchvalue)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_ambswi (%d)\n", switchvalue);
	fflush (stdout);
#   endif

    DROM.ambient_switch = switchvalue;

    if (dde_pex_drom_pre_init) return;

    PEX.pc_attrs.reflection_attr.ambient = switchvalue ? DROM.ambient_intens :0;
    PEXSetReflectionAttributes
	(DCM->display, PEX.renderer,PEXOCRender, &PEX.pc_attrs.reflection_attr);
}



/*****************************************************************************
**  This routine sets the backface-cullable switch.  In order for backface
**  culling to take place, both this switch and the backface-cull switch (see
**  ddr_pex_drom_set_att_bacfacculswi()) must be set to DcOn.
*****************************************************************************/

void  ddr_pex_drom_set_att_bacfacculble  (DtSwitch switchvalue)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_bacfacculble (%d)\n", switchvalue);
	fflush (stdout);
#   endif

    DROM.bfcullable = switchvalue;

    if (dde_pex_drom_pre_init) return;

    PEXSetFacetCullingMode
    (   DCM->display,
	PEX.renderer, PEXOCRender,
	(DROM.bfcull_switch && DROM.bfcullable) ? PEXBackFaces : PEXNone
    );
}



/*****************************************************************************
**  This routine sets the backface-cull switch.  In order for backface culling
**  to take place, both this switch and the backface-cullable switch (see
**  ddr_pex_drom_set_att_bacfacculble()) must be set to DcOn.
*****************************************************************************/

void  ddr_pex_drom_set_att_bacfacculswi  (DtSwitch switchvalue)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_bacfacculswi (%d)\n", switchvalue);
	fflush (stdout);
#   endif

    DROM.bfcull_switch = switchvalue;

    if (dde_pex_drom_pre_init) return;

    PEXSetFacetCullingMode
    (   DCM->display,
	PEX.renderer, PEXOCRender,
	(DROM.bfcull_switch && DROM.bfcullable) ? PEXBackFaces : PEXNone
    );
}



/*****************************************************************************
**  This routine sets the depth-cueing attributes.  `zfront' and `zback'
**  define the Z-values (in frame coordinates) of two planes of constant Z.
**  The `sfront' and `sback' values are in the range [0,1], and specify the
**  portion between the primitive's color and the depth cue color `color' to
**  be used at each plane.  The values for Z-values between `sfront' and
**  `sback' are linearly interpolated.  The parameter `colormodel' establishes
**  the color model of the color parameter.
*****************************************************************************/

void  ddr_pex_drom_set_att_depcue (
    DtReal        zfront,	/* Min & Max Z Values */
    DtReal        zback,
    DtReal        sfront,	/* Object/Depthcue Color Ratio */
    DtReal        sback,
    DtColorModel  colormodel,
    DtReal        color[])	/* Depthcue Color */
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_depcue (%g,%g, %g,%g, %d, %lx)\n", zfront,zback, sfront,sback, colormodel, color);
	fflush (stdout);
#   endif

    if (colormodel != DcRGB) return;

    DROM.depthcue.mode          = PEXOn;
    DROM.depthcue.front_plane   = zfront;
    DROM.depthcue.back_plane    = zback;
    DROM.depthcue.front_scaling = sfront;
    DROM.depthcue.back_scaling  = sback;

    SETCOLOR (DROM.depthcue.color, color[0], color[1], color[2]);

    if (!dde_pex_drom_pre_init)
    {   PEXSetTableEntries
	(   DCM->display, DROM.depthcue_LUT, 0, 1,
	    PEXLUTDepthCue, (PEXPointer)(&DROM.depthcue)
	);
    }
}



/*****************************************************************************
**  This routine enables/disables depthcueing.  If `switchvalue' is DcOn, then
**  depthcueing is enabled, otherwise it's disabled.
*****************************************************************************/

void  ddr_pex_drom_set_att_depcueswi (DtSwitch switchvalue)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_depcueswi (%d)\n", switchvalue);
	fflush (stdout);
#   endif

    DROM.depthcue.mode = switchvalue ? PEXOn : PEXOff;

    if (!dde_pex_drom_pre_init)
    {   PEXSetTableEntries
	(   DCM->display, DROM.depthcue_LUT, 0, 1,
	    PEXLUTDepthCue, (PEXPointer)(&DROM.depthcue)
	);
    }
}



/*****************************************************************************
**  This procedure sets the diffuse color attribute.  `colormodel' defines the
**  color model used; `color' defines the diffuse color of subsequent objects.
**  Typically, the diffuse color and diffuse intensity are multiplied together
**  to get the diffuse reflectance, which is used in the shading calculations.
**  The diffuse color is also used when computing the ambient reflectance.
*****************************************************************************/

void  ddr_pex_drom_set_att_difclr  (
    DtColorModel colormodel,	/* Type of Color (RGB, HSV, et cetera) */
    DtReal       color[])	/* Diffuse Color Components */
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_difclr (%d, [%f,%f,%f])\n",
	    colormodel, color[0], color[1], color[2]);
	fflush (stdout);
#   endif

    if (colormodel != DcRGB) return;

    SETCOLOR (PEX.pc_attrs.surface_color, color[0], color[1], color[2]);

    if (dde_pex_drom_pre_init)
    {   PEXSetPCAttributeMask (PEX.pc_value_mask, PEXPCSurfaceColor);
	return;
    }

    PEXSetSurfaceColor (DCM->display, PEX.renderer,
			PEXOCRender, PEXColorTypeRGB,
			&PEX.pc_attrs.surface_color.value);
}



/*****************************************************************************
**  This subroutine sets the diffuse intensity.  `intensity' defines a
**  weighting factor for the object's response to diffuse light.  Typically
**  the diffuse color and diffuse intensity are multiplied toether to get the
**  diffuse reflectance.  The reflectance is then used in the shading
**  calculations.
*****************************************************************************/

void  ddr_pex_drom_set_att_difint  (DtReal intensity)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_difint (%f)\n", intensity);
	fflush (stdout);
#   endif

    DROM.diffuse_intens = intensity;

    if (dde_pex_drom_pre_init) return;

    PEX.pc_attrs.reflection_attr.diffuse = DROM.diffuse_switch ? intensity : 0;
    PEXSetReflectionAttributes
	(DCM->display, PEX.renderer,PEXOCRender, &PEX.pc_attrs.reflection_attr);
}



/*****************************************************************************
**  This procedure sets the diffuse switch.  If `switchvalue' is DcOn, the
**  diffuse component of the shading equations is enabled, otherwise diffuse
**  shading is disabled.
*****************************************************************************/

void  ddr_pex_drom_set_att_difswi  (DtSwitch switchvalue)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_difswi (%d)\n", switchvalue);
	fflush (stdout);
#   endif

    DROM.diffuse_switch = switchvalue;

    if (dde_pex_drom_pre_init) return;

    PEX.pc_attrs.reflection_attr.diffuse = switchvalue ? DROM.diffuse_intens :0;
    PEXSetReflectionAttributes
	(DCM->display, PEX.renderer,PEXOCRender, &PEX.pc_attrs.reflection_attr);
}



/*****************************************************************************
**  This routine sets the hidden-surface switch.  `switchvalue' specifies
**  whether hidden surface removal is enabled or disabled.  If `switchvalue'
**  is DcOn, then an object will be obscured by other objects that are between
**  it and the viewer.  If `switchvalue' is off, then the object will not be
**  obscured.  This switch typically corresponds to enabling or disabling the
**  depth buffer.
*****************************************************************************/

void  ddr_pex_drom_set_att_hidsrfswi  (DtSwitch switchvalue)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_hidsrfswi (%d)\n", switchvalue);
	fflush (stdout);
#   endif

    /* The Hlhsr (Hidden Line / Hidden Surface Renderer) mode cannot be
    ** changed during a PEX rendering pass.  In order to implement changes
    ** in the hidden-surface setting, we'd have to save the current attribute
    ** state, end the current rendering pass, change the HlhsrMode setting,
    ** unset the z-clear flag, begin a new rendering pass, restore the
    ** attribute state, and set the z-clear flag.  */
}



/*****************************************************************************
**  This subroutine sets the interpolation type.  `interptype' specifies the
**  type of shading interpolation for subsequent objects.
*****************************************************************************/

void  ddr_pex_drom_set_att_inttyp  (DtInterpType interptype)
{
    auto int pex_surf_interp;	/* PEX Surface Interpolation Method */
    auto int pex_line_interp;	/* PEX Line Interpolation Method */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_inttyp (%d)\n", interptype);
	fflush (stdout);
#   endif

    /* Get the PEX surface interpolation method that most closely matches
    ** the requested Dore' interpolation method. */

    switch (interptype)
    {   default:
	case DcConstantShade: pex_surf_interp = PEXSurfaceInterpNone;
			      pex_line_interp = PEXPolylineInterpNone;
			      break;

	case DcVertexShade:   pex_surf_interp = pex_modes.surfinterp_color;
			      pex_line_interp = PEXPolylineInterpColor;
			      break;

	case DcSurfaceShade:  pex_surf_interp = pex_modes.surfinterp_normal;
			      pex_line_interp = PEXPolylineInterpColor;
			      break;
    }

    if (dde_pex_drom_pre_init)
    {   PEX.pc_attrs.surface_interp = pex_surf_interp;
        PEXSetPCAttributeMask (PEX.pc_value_mask, PEXPCSurfaceInterp);
	PEX.pc_attrs.polyline_interp = pex_line_interp;
	PEXSetPCAttributeMask (PEX.pc_value_mask, PEXPCPolylineInterp);
	return;
    }

    PEXSetSurfaceInterpMethod
	(DCM->display, PEX.renderer, PEXOCRender, pex_surf_interp);
    PEXSetPolylineInterpMethod
	(DCM->display, PEX.renderer, PEXOCRender, pex_line_interp);
}



/*****************************************************************************
**  The line type defines the style of lines to be drawn.  The line style
**  affects all objects drawn with lines.
*****************************************************************************/

void  ddr_pex_drom_set_att_lintyp  (DtLineType linetype)
{
    auto int pex_linetype;	/* Nearest Corresponding PEX Line Type */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_lintyp (%d)\n", linetype);
	fflush (stdout);
#   endif

    switch (linetype)
    {   default:
	case DcLineTypeSolid:    pex_linetype = PEXLineTypeSolid;       break;
        case DcLineTypeDash:     pex_linetype = pex_modes.line_dashed;  break;
        case DcLineTypeDot:      pex_linetype = pex_modes.line_dotted;  break;
        case DcLineTypeDotDash:  pex_linetype = pex_modes.line_dashdot; break;
    }

    if (!dde_pex_drom_pre_init)
	PEXSetLineType (DCM->display, PEX.renderer, PEXOCRender, pex_linetype);
    else
    {   PEX.pc_attrs.line_type = pex_linetype;
	PEXSetPCAttributeMask (PEX.pc_value_mask, PEXPCLineType);
    }
}



/*****************************************************************************
**  This procedure sets the line width.  `width' defines the width (in pixels)
**  of the lines to be drawn.  The line width affects all objects drawn with
**  lines.  Note that for the PEX driver, this attribute will also set the
**  width of surface edges.
*****************************************************************************/

void  ddr_pex_drom_set_att_linwid  (DtReal width)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_linwid (%f)\n", width);
	fflush (stdout);
#   endif

    if (dde_pex_drom_pre_init)
    {   PEX.pc_attrs.line_width = width;
        PEXSetPCAttributeMask (PEX.pc_value_mask, PEXPCLineWidth);
	return;
    }

    PEXSetLineWidth
	(DCM->display, PEX.renderer, PEXOCRender, (double)(width));
    PEXSetSurfaceEdgeWidth
	(DCM->display, PEX.renderer, PEXOCRender, (double)(width));
}



/*****************************************************************************
**  Set the object representation type.
*****************************************************************************/

void  ddr_pex_drom_set_att_reptyp  (DtRepType reptype)
{
    auto int pex_interior_style;
    auto int pex_surfedge_flag;

#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_reptyp (%d)\n", reptype);
	fflush (stdout);
#   endif

    DROM.rep_type = reptype;

    switch (reptype)
    {
	case DcPoints:
	case DcWireframe:
	case DcOutlines:
	    pex_interior_style = PEXInteriorStyleHollow;
	    pex_surfedge_flag  = PEXOff;
	    break;

	default:
	case DcSurface:
	    pex_interior_style = PEXInteriorStyleSolid;
	    pex_surfedge_flag  = PEXOff;
	    break;

	case DcSurfaceWireframeEdges:
	case DcSurfaceOutlineEdges:
	    pex_interior_style = PEXInteriorStyleSolid;
	    pex_surfedge_flag  = PEXOn;
	    break;
    }

    if (dde_pex_drom_pre_init)
    {   PEX.pc_attrs.interior_style = pex_interior_style;
	PEX.pc_attrs.surface_edges  = pex_surfedge_flag;
        PEXSetPCAttributeMask (PEX.pc_value_mask, PEXPCInteriorStyle);
        PEXSetPCAttributeMask (PEX.pc_value_mask, PEXPCSurfaceEdgeFlag);
	return;
    }

    PEXSetInteriorStyle
	(DCM->display, PEX.renderer, PEXOCRender, pex_interior_style);
    PEXSetSurfaceEdgeFlag
	(DCM->display, PEX.renderer, PEXOCRender, pex_surfedge_flag);
}



/*****************************************************************************
**  Set the shade index.  The shade index `shadeindex' specifies the color
**  shade range to use for subsequent primitives when the device visual type
**  is DcPseudoColor and the shade mode is DcRange.
*****************************************************************************/

void  ddr_pex_drom_set_att_shdidx  (DtInt shadeindex)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_shdidx (%d)\n", shadeindex);
	fflush (stdout);
#   endif
}



/*****************************************************************************
**  Set the specular color.  `colormodel' is the color model of the specified
**  color.  `color' defines the specular color of subsequent objects.
**  Typically the specular color and specular intensity are multiplied
**  together to get the specular reflectance.  The reflectance is then used in
**  the shading calculations.
*****************************************************************************/

void  ddr_pex_drom_set_att_spcclr  (
    DtColorModel colormodel,	/* Color Model of the Color Parameter */
    DtReal       color[])	/* Specular Color */
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_spcclr (%d, [%f,%f,%f])\n",
	    colormodel, color[0], color[1], color[2]);
	fflush (stdout);
#   endif

    if (colormodel != DcRGB) return;

    SETCOLOR (PEX.pc_attrs.reflection_attr.specular_color,
	      color[0], color[1], color[2]);

    if (dde_pex_drom_pre_init)
    {   PEXSetPCAttributeMask (PEX.pc_value_mask, PEXPCReflectionAttr);
	return;
    }

    PEXSetReflectionAttributes
	(DCM->display, PEX.renderer,PEXOCRender, &PEX.pc_attrs.reflection_attr);
}





/*****************************************************************************
**  Set the specular factor.  `factor' defines an exponent that controls the
**  shape of the specular highlight.  Typically, a cosine raised to the power
**  `factor' is used to determine the shape of the highlight.
*****************************************************************************/

void  ddr_pex_drom_set_att_spcfct  (DtReal factor)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_spcfct (%f)\n", factor);
	fflush (stdout);
#   endif

    PEX.pc_attrs.reflection_attr.specular_conc = factor;

    if (dde_pex_drom_pre_init)
    {   PEXSetPCAttributeMask (PEX.pc_value_mask, PEXPCReflectionAttr);
	return;
    }

    PEXSetReflectionAttributes
	(DCM->display, PEX.renderer,PEXOCRender, &PEX.pc_attrs.reflection_attr);
}



/*****************************************************************************
**  Set the specular intensity.  `intensity' defines a weighting factor for an
**  object's response to specular light.  Typically, the specular color and
**  specular intensity are multiplied to get the specular reflectance.  The
**  reflectance is then used in the shading calculations.
*****************************************************************************/

void  ddr_pex_drom_set_att_spcint  (DtReal intensity)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_spcint (%f)\n", intensity);
	fflush (stdout);
#   endif

    DROM.specular_intens = intensity;

    if (dde_pex_drom_pre_init) return;

    PEX.pc_attrs.reflection_attr.specular = DROM.specular_switch ? intensity :0;
    PEXSetReflectionAttributes
	(DCM->display, PEX.renderer,PEXOCRender, &PEX.pc_attrs.reflection_attr);
}



/*****************************************************************************
**  Enable/Disable specular lighting.  If `switchvalue' is set to DcOn, then
**  the DROM applies specular lighting to subsequent objects, otherwise it
**  ignores specular light.
*****************************************************************************/

void  ddr_pex_drom_set_att_spcswi  (DtSwitch switchvalue)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_spcswi (%d)\n", switchvalue);
	fflush (stdout);
#   endif

    DROM.specular_switch = switchvalue;

    if (dde_pex_drom_pre_init) return;

    PEX.pc_attrs.reflection_attr.specular =
	switchvalue ? DROM.specular_intens : 0;

    PEXSetReflectionAttributes
	(DCM->display, PEX.renderer,PEXOCRender, &PEX.pc_attrs.reflection_attr);
}



/*****************************************************************************
**  Set the surface-edge color.  `colormodel' is the color model of the color
**  parameter.  `color' specifies the color to be used when drawing the edges
**  of subsequent objects when the representation type is
**  DcSurfaceWireframeEdges or DcSurfaceOutlineEdges.
*****************************************************************************/

void  ddr_pex_drom_set_att_srfedgclr  (
    DtColorModel colormodel,	/* Color Model of the Color Parameter */
    DtReal       color[])	/* Surface Edge Color */
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_srfedgclr (%d, [%f,%f,%f])\n",
	    colormodel, color[0], color[1], color[2]);
	fflush (stdout);
#   endif

    if (colormodel != DcRGB) return;

    SETCOLOR (PEX.pc_attrs.surface_edge_color, color[0], color[1], color[2]);

    if (dde_pex_drom_pre_init)
        PEXSetPCAttributeMask (PEX.pc_value_mask, PEXPCSurfaceEdgeColor);
    else
    {   PEXSetSurfaceEdgeColor
	(   DCM->display,
	    PEX.renderer, PEXOCRender,
	    PEXColorTypeRGB,
	    &PEX.pc_attrs.surface_edge_color.value
	);
    }
}



/*****************************************************************************
**  Set the surface shader callback object.  `callbackobj' specifies the type
**  of shader to use when performing shading calculations for subsequent
**  objects.  Two types of shader callbacks are currently defined.
**  DcShaderConstant means that objects are shaded using just their diffuse
**  colors, while DcShaderLightSource means that light sources are used for
**  the shading.
*****************************************************************************/

void  ddr_pex_drom_set_att_srfshd  (DtObject callbackobj)
{
    auto int model;	/* PEX Reflection Model */

#   if DEBUG_CALL
    {   auto char buff[40];
	printf
	(   "ddr_pex_drom_set_att_srfshd (%s)\n",
	    (callbackobj == DcShaderConstant)    ? "DcShaderConstant"    :
	    (callbackobj == DcShaderLightSource) ? "DcShaderLightSource" :
	    (sprintf(buff,"%lx",callbackobj), buff)
	);
	fflush (stdout);
    }
#   endif

    if (callbackobj == DcShaderConstant)
	model = PEXReflectionNone;
    else
	model = PEXReflectionSpecular;

    DROM.shading_model = model;

    if (!dde_pex_drom_pre_init)
	PEXSetReflectionModel (DCM->display, PEX.renderer, PEXOCRender, model);
    else
    {   PEX.pc_attrs.reflection_model = model;
	PEXSetPCAttributeMask (PEX.pc_value_mask, PEXPCReflectionModel);
    }
}



/*****************************************************************************
**  Set the transparent color.  The `color' parameter defines the amount of
**  each color component that shines through subsequent transparent objects.
**  The transparency color and transparency intensity together define the
**  amount of light that passes through transparent objects.
*****************************************************************************/

void  ddr_pex_drom_set_att_transpclr  (
    DtColorModel colormodel,	/* Color Model of the Color Parameter */
    DtReal       color[])	/* Transparency Color */
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_transpclr (%d, [%f,%f,%f]\n",
	    colormodel, color[0], color[1], color[2]);
	fflush (stdout);
#   endif

    /* Not supported in PEX */
}



/*****************************************************************************
**  Set the transparent intensity.  The transparency color and transparency
**  intensity together define the amount of light that passes through
**  transparent objects, but transparency color is not supported in PEX yet.
**  `value' is the fraction of the light that shines through subsequent
**  objects.  An intensity of 0 means that objects are completely opaque;
**  none of the light penetrates.  An intensity of 1 indicates that objects
**  are completely transparent; the light shines through the object at full
**  intensity.
*****************************************************************************/

void  ddr_pex_drom_set_att_transpint  (DtReal intensity)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_transpint (%f)\n", intensity);
	fflush (stdout);
#   endif

    DROM.transp_intens = intensity;

    if (dde_pex_drom_pre_init || !DROM.transp_switch) return;

    PEX.pc_attrs.reflection_attr.transmission = intensity;
    PEXSetReflectionAttributes
	(DCM->display, PEX.renderer,PEXOCRender, &PEX.pc_attrs.reflection_attr);
}



/*****************************************************************************
**  Enable/Disable transparent objects.  If `switchvalue' is DcOn, then
**  subsequent objects are transparent, otherwise subsequent objects are
**  opaque.
*****************************************************************************/

void  ddr_pex_drom_set_att_transpswi  (DtSwitch switchvalue)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_transpswi (%d)\n", switchvalue);
	fflush (stdout);
#   endif

    DROM.transp_switch = switchvalue;

    if (dde_pex_drom_pre_init) return;

    PEX.pc_attrs.reflection_attr.transmission =
	DROM.transp_switch ? DROM.transp_intens : 0;
    PEXSetReflectionAttributes
	(DCM->display, PEX.renderer,PEXOCRender, &PEX.pc_attrs.reflection_attr);
}
