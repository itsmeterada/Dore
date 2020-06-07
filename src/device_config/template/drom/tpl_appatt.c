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
	void ddr_template_drom_set_att_ambint (intensity)
	void ddr_template_drom_set_att_ambswi (switchvalue)
	void ddr_template_drom_set_att_bacfacculble (switchvalue)
	void ddr_template_drom_set_att_bacfacculswi (switchvalue)
	void ddr_template_drom_set_att_depcue (zfront,zback,sfront,sback,colormodel,color)
	void ddr_template_drom_set_att_depcueswi (switchvalue)
	void ddr_template_drom_set_att_difclr (colormodel, color)
	void ddr_template_drom_set_att_difint (intensity)
	void ddr_template_drom_set_att_difswi (switchvalue)
	void ddr_template_drom_set_att_hidsrfswi (switchvalue)
	void ddr_template_drom_set_att_inttyp (interptype)
	void ddr_template_drom_set_att_lintyp (type)
	void ddr_template_drom_set_att_linwid (width)
	void ddr_template_drom_set_att_localaaswi (switchvalue)
	void ddr_template_drom_set_att_localaasty (style)
	void ddr_template_drom_set_att_refswi (switchvalue)
	void ddr_template_drom_set_att_reptyp (reptype)
	void ddr_template_drom_set_att_shaswi (switchvalue)
	void ddr_template_drom_set_att_shdidx (shadeindex)
	void ddr_template_drom_set_att_spcclr (colormodel, color)
	void ddr_template_drom_set_att_spcfct (factor)
	void ddr_template_drom_set_att_spcint (intensity)
	void ddr_template_drom_set_att_spcswi (switchvalue)
	void ddr_template_drom_set_att_srfedgclr (colormodel, color)
	void ddr_template_drom_set_att_srfshd (callbackobj)
	void ddr_template_drom_set_att_transpclr (colormodel, color)
	void ddr_template_drom_set_att_transpint (value)
	void ddr_template_drom_set_att_transpswi (switchvalue)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
      /*  Ambient Intensity  */
void ddr_template_drom_set_att_ambint (
    DtReal intensity)
{
    dde_template_drom_data.ambint = intensity;
}
/*
 ======================================================================
 */
	/*  Ambient switch */
void ddr_template_drom_set_att_ambswi (
    DtSwitch switchvalue)
{
    dde_template_drom_data.ambswi = switchvalue;
}
/*
 ======================================================================
 */
	/* Backface Cullable */
void ddr_template_drom_set_att_bacfacculble (
    DtSwitch switchvalue)
{
    dde_template_drom_data.bacfacculble = switchvalue;
}
/*
 ======================================================================
 */
	/* Backface Cull Switch */
void ddr_template_drom_set_att_bacfacculswi (
    DtSwitch switchvalue)
{
    dde_template_drom_data.bacfacculswi = switchvalue;
}
/*
 ======================================================================
 */
	/* Depth Cue */
void ddr_template_drom_set_att_depcue (
    DtReal zfront,
    DtReal zback,
    DtReal sfront,
    DtReal sback,
    DtColorModel colormodel,
    DtReal *color)
{
    dde_template_drom_data.depcue_zfront = zfront;
    dde_template_drom_data.depcue_zback  = zback;
    dde_template_drom_data.depcue_sfront = sfront;
    dde_template_drom_data.depcue_sback  = sback;
    dde_template_drom_data.depcue_colormodel = colormodel;
    if (colormodel == DcRGB) {
	dde_template_drom_data.depcue_color[0] = color[0];
	dde_template_drom_data.depcue_color[1] = color[1];
	dde_template_drom_data.depcue_color[2] = color[2];
    }
}
/*
 ======================================================================
 */
	/* Depth Cue Switch */
void ddr_template_drom_set_att_depcueswi (
    DtSwitch switchvalue)
{
    dde_template_drom_data.depcueswi = switchvalue;
}
/*
 ======================================================================
 */
	/*  Diffuse Color */
void ddr_template_drom_set_att_difclr (
    DtColorModel colormodel,
    DtReal color[])
{
    dde_template_drom_data.difclr[0] = color[0];
    dde_template_drom_data.difclr[1] = color[1];
    dde_template_drom_data.difclr[2] = color[2];
}
/*
 ======================================================================
 */
	/*  Diffuse Intensity */
void ddr_template_drom_set_att_difint (
    DtReal intensity)
{
    dde_template_drom_data.difint = intensity;
}
/*
 ======================================================================
 */
	/*  Diffuse Switch */
void ddr_template_drom_set_att_difswi (
    DtSwitch switchvalue)
{
    dde_template_drom_data.difswi = switchvalue;
}
/*
 ======================================================================
 */
	/*  Hidden Surface Switch */
void ddr_template_drom_set_att_hidsrfswi (
    DtSwitch switchvalue)
{
    dde_template_drom_data.hidsrfswi = switchvalue;
}
/*
 ======================================================================
 */
	/* InterType */
void ddr_template_drom_set_att_inttyp (
    DtInterpType interptype)
{
    dde_template_drom_data.inttyp = interptype;
}
/*
 ======================================================================
 */
      /* Line Type */
void ddr_template_drom_set_att_lintyp (
    DtLineType type)
{
    dde_template_drom_data.lintyp = type;
}
/*
 ======================================================================
 */
      /* Line Width */
void ddr_template_drom_set_att_linwid (
    DtReal width)
{
    dde_template_drom_data.linwid = width;
}
/*
 ======================================================================
 */
	/* Local Antialias Style */
void ddr_template_drom_set_att_localaasty (
    DtLocalAntiAliasStyle style)
{
    dde_template_drom_data.localaasty = style;
}
/*
 ======================================================================
 */
	/* Local Antialiasing Switch */
void ddr_template_drom_set_att_localaaswi (
    DtSwitch switchvalue)
{
    dde_template_drom_data.localaaswi = switchvalue;
}
/*
 ======================================================================
 */
      /* Reflection Switch */
void ddr_template_drom_set_att_refswi (
    DtSwitch switchvalue)
{
    dde_template_drom_data.refswi = switchvalue;
}
/*
 ======================================================================
 */
	/* RepType */
void ddr_template_drom_set_att_reptyp (
    DtRepType reptype)
{
    dde_template_drom_data.reptyp = reptype;
}
/*
 ======================================================================
 */
      /* Shadow Switch */
void ddr_template_drom_set_att_shaswi (
    DtSwitch switchvalue)
{
    dde_template_drom_data.shaswi = switchvalue;
}
/*
 ======================================================================
 */
	/* Shade Index */
void ddr_template_drom_set_att_shdidx (
    DtInt shadeindex)
{
    dde_template_drom_data.shdidx = shadeindex;
}
/*
 ======================================================================
 */
	/*  Specular Color  */
void ddr_template_drom_set_att_spcclr (
    DtColorModel colormodel,
    DtReal color[])
{
    dde_template_drom_data.spcclr[0] = color[0];
    dde_template_drom_data.spcclr[1] = color[1];
    dde_template_drom_data.spcclr[2] = color[2];
}
/*
 ======================================================================
 */
	/*  Specular Factor */
void ddr_template_drom_set_att_spcfct (
    DtReal factor)
{
    dde_template_drom_data.spcfct = factor;
}
/*
 ======================================================================
 */
	/*  Specular Intensity */
void ddr_template_drom_set_att_spcint (
    DtReal intensity)
{
    dde_template_drom_data.spcint = intensity;
}
/*
 ======================================================================
 */
	/*  Specular Switch */
void ddr_template_drom_set_att_spcswi (
    DtSwitch switchvalue)
{
    dde_template_drom_data.spcswi = switchvalue;
}
/*
 ======================================================================
 */
void ddr_template_drom_set_att_srfedgclr (
    DtColorModel colormodel,
    DtReal color[])
{
    dde_template_drom_data.srfedgclr[0] = color[0];
    dde_template_drom_data.srfedgclr[1] = color[1];
    dde_template_drom_data.srfedgclr[2] = color[2];
}
/*
 ======================================================================
 */
	/*  Surface Shader Callback Object  */
void ddr_template_drom_set_att_srfshd (
    DtObject callbackobj)
{
    dde_template_drom_data.srfshd = callbackobj;
}
/*
 ======================================================================
 */
	/*  Transparency color */
void ddr_template_drom_set_att_transpclr (
    DtColorModel colormodel,
    DtReal color[])
{
    dde_template_drom_data.transpclr[0] = color[0];
    dde_template_drom_data.transpclr[1] = color[1];
    dde_template_drom_data.transpclr[2] = color[2];
}
/*
 ======================================================================
 */
	/*  Transparency intensity */
void ddr_template_drom_set_att_transpint (
    DtReal value)
{
    dde_template_drom_data.transpint = value;
}
/*
 ======================================================================
 */
	/*  Transparency switch */
void ddr_template_drom_set_att_transpswi (
    DtSwitch switchvalue)
{
    dde_template_drom_data.transpswi = switchvalue;
}
/*
 ======================================================================
 */
