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
	ddr_gl_drom_set_att_ambint
	ddr_gl_drom_set_att_ambswi
	ddr_gl_drom_set_att_bacfacculble
	ddr_gl_drom_set_att_bacfacculswi
	ddr_gl_drom_set_att_depcue
	ddr_gl_drom_set_att_depcueswi
	ddr_gl_drom_set_att_difswi
	ddr_gl_drom_set_att_difclr
	ddr_gl_drom_set_att_difint
	ddr_gl_drom_set_att_hidsrfswi
	ddr_gl_drom_set_att_inttyp
	ddr_gl_drom_set_att_lintyp
	ddr_gl_drom_set_att_linwid
	ddr_gl_drom_set_att_localaaswi
	ddr_gl_drom_set_att_localaasty
	ddr_gl_drom_set_att_refswi
	ddr_gl_drom_set_att_reptyp
	ddr_gl_drom_set_att_shaswi
	ddr_gl_drom_set_att_shdidx
	ddr_gl_drom_set_att_spcclr
	ddr_gl_drom_set_att_spcfct
	ddr_gl_drom_set_att_spcint
	ddr_gl_drom_set_att_srfedgclr
	ddr_gl_drom_set_att_spcswi
	ddr_gl_drom_set_att_srfshd
	ddr_gl_drom_set_att_transpclr
	ddr_gl_drom_set_att_transpint
	ddr_gl_drom_set_att_transpswi

  ======================================================================
 */
#include "../gl_driver.h"


#define DEBUGP  0


/*======================================================================
==  This routine sets the ambient intensity by calling lmdef() to alter
==  the current material property.
======================================================================*/

void ddr_gl_drom_set_att_ambint (
    DtReal intensity)		/* Ambient Intensity */
{
    auto     float props[20];	/* Attribute Properties */
    register int   prop_index;	/* Property List Index  */

#   if DEBUGP
	printf ("* Ambient_intensity(%f)\n", intensity);
#   endif

    DevStatus.ambientintens = intensity;

    prop_index = 0;

    props[prop_index++] = AMBIENT;
    props[prop_index++] = props[prop_index++] = props[prop_index++] =
	(DevStatus.ambientswitch) ? DevStatus.ambientintens : 0;
    props[prop_index++] = LMNULL;

    lmdef (DEFMATERIAL, 1, 0, props);
}



/* ======================================================================== */

void ddr_gl_drom_set_att_ambswi (DtSwitch switchvalue)
{
#   if DEBUGP
	printf("* Set_ambient_switch(%d)\n", switchvalue);
#   endif

    DevStatus.ambientswitch = switchvalue;

    ddr_gl_drom_set_att_ambint (DevStatus.ambientintens);
}



/* ======================================================================== */

void ddr_gl_drom_set_att_bacfacculble (DtSwitch switchvalue)
{
#   if DEBUGP
	printf("* Backface_cullable_switch(%d)\n",switchvalue);
#   endif

    if ((DevStatus.cullable=switchvalue) && DevStatus.cull)
	backface(TRUE);
    else
	backface(FALSE);
}



/* ======================================================================== */

void ddr_gl_drom_set_att_bacfacculswi (DtSwitch switchvalue)
{
#   if DEBUGP
	printf("* Backface_cull_switch(%d)\n",switchvalue);
#   endif

    if ((DevStatus.cull=switchvalue) && DevStatus.cullable)
	backface(TRUE);
    else
	backface(FALSE);
}



/* ======================================================================== */

void ddr_gl_drom_set_att_depcue (
    DtReal zfront,
    DtReal zback,
    DtReal sfront,
    DtReal sback,
    DtColorModel colormodel,
    DtReal *color)
{
#   if DEBUGP
	printf("* Depth_cue(zfront=%f,zback=%f,...)\n",zfront,zback);
#   endif

    DevStatus.depcuecolor[0] = color[0];
    DevStatus.depcuecolor[1] = color[1];
    DevStatus.depcuecolor[2] = color[2];
    DevStatus.depcuezfront   = zfront;
    DevStatus.depcuezback    = zback;
    DevStatus.depcuesfront   = sfront;
    DevStatus.depcuesback    = sback;

    if (DevStatus.depcueswitch)
	ddr_iris_restore_depcue();
}



/* ======================================================================== */

void ddr_gl_drom_set_att_depcueswi (DtSwitch switchvalue)
{
#   if DEBUGP
	printf("* Depth_cue_switch(%d)\n",switchvalue);
#   endif

    depthcue(switchvalue);

    DevStatus.depcueswitch = switchvalue;

    ddr_iris_restore_diffuse_color();
}



/*======================================================================
==  This routine sets the diffuse intensity switch.  If the switch is
==  DcOff, then the diffuse intensity is set to 0 (black).
======================================================================*/

void ddr_gl_drom_set_att_difswi (DtSwitch switchvalue)
{
#   if DEBUGP
	printf ("* Set_Diffuse_switch(%d)\n", switchvalue);
#   endif

    DevStatus.diffuseswitch = switchvalue;

    ddr_gl_drom_set_att_difint (DevStatus.diffuseintens);
}



/* ======================================================================== */

void ddr_gl_drom_set_att_difclr (
    DtColorModel colormodel,
    DtReal color[])
{
#   if DEBUGP
	printf("* Set_diffuse_color (%f,%f,%f)\n",
	    color[0],color[1],color[2]);
#   endif

    DevStatus.diffusecolor[0] = color[0];
    DevStatus.diffusecolor[1] = color[1];
    DevStatus.diffusecolor[2] = color[2];

    ddr_iris_restore_diffuse_color();
}



/*======================================================================
==  This procedure sets the diffuse intensity.  It uses lmdef() to
==  alter the current material property.
======================================================================*/

void ddr_gl_drom_set_att_difint (
    DtReal intensity)	/* The Material's Diffuse Intensity */
{
    auto     float props[20];	/* Attribute Properties */
    register int   prop_index;	/* Property List Index */

#   if DEBUGP
	printf ("* Diffuse_intens(%f)\n", intensity);
#   endif

    DevStatus.diffuseintens = intensity;

    prop_index = 0;

    props[prop_index++] = DIFFUSE;
    props[prop_index++] = props[prop_index++] = props[prop_index++] =
	(DevStatus.diffuseswitch) ? DevStatus.diffuseintens : 0;
    props[prop_index++] = LMNULL;

    lmdef (DEFMATERIAL, 1, 0, props);
}



/* ======================================================================== */

void ddr_gl_drom_set_att_hidsrfswi (DtSwitch switchvalue)
{
#   if DEBUGP
	printf("* Hidden_surface_switch(%d)\n",switchvalue);
#   endif

    DDdevice_SetDepthBufferEnable(DevStatus.dore_device, switchvalue);
}



/* ======================================================================== */

void ddr_gl_drom_set_att_inttyp (DtInterpType interptype)
{
    if (interptype == DcConstantShade) {
	/* Uncommented for now -- srh. */
	    /* Unfortunately, the iris does not dither in FLAT shaded mode,
	    ** which causes the colors to be off in the 16 bit RGB mode.  */
	    /* shademodel(FLAT); */
	shademodel (FLAT);
	DevStatus.smoothswitch = 0;
    } else {
	shademodel (GOURAUD);
	DevStatus.smoothswitch = 1;
    }

#   if DEBUGP
	printf("* Set_interptype(%s)\n",
	    DevStatus.smoothswitch ? "Smooth" : "Flat");
#   endif
}



/* ======================================================================== */

void ddr_gl_drom_set_att_lintyp (DtLineType type)
{
#   if DEBUGP
	printf("* Set_Linetype(%d)\n",type);
#   endif

    if (type == DcLineTypeSolid) {
	setlinestyle(0);
	return;
    }

    switch (type) {

	case DcLineTypeDash:
	    deflinestyle (1, 0x3F00);
	    break;

	case DcLineTypeDot:
	    deflinestyle (1, 0x0303);
	    break;

	case DcLineTypeDotDash:
	    deflinestyle (1, 0xF060);
	    break;

	default:
#           if DEBUGP
		printf("   ... INVALID STYLE!!!\n");
#           endif

	    setlinestyle (0);
	    return;
    }

    setlinestyle (1);
    return;
}



/* ======================================================================== */

void ddr_gl_drom_set_att_linwid (DtReal width)
{
#   if DEBUGP
	printf("* Set_line_width(%f)\n",width);
#   endif

    if (width < 1.0) width = 1.0;

    linewidth ((short)width);
}



/* ======================================================================== */

void ddr_gl_drom_set_att_localaasty (DtLocalAntiAliasStyle style)
{
}



/* ======================================================================== */

void ddr_gl_drom_set_att_localaaswi (DtSwitch switchvalue)
{
}



/* ======================================================================== */

void ddr_gl_drom_set_att_refswi (DtSwitch switchvalue)
{
}



/* ======================================================================== */

void ddr_gl_drom_set_att_reptyp (DtRepType reptype)
{
#   if DEBUGP
	printf("* Set_reptype(%d)\n",reptype);
#   endif

    DevStatus.reptype = reptype;
}



/* ======================================================================== */

void ddr_gl_drom_set_att_shaswi (DtSwitch switchvalue)
{
}



/* ======================================================================== */

void ddr_gl_drom_set_att_shdidx (DtInt shadeindex)
{
    DtInt shdrange[2];

#   if DEBUGP
	printf("* Set_shade_index (%d)\n",shadeindex);
#   endif

    DevStatus.shadeindex = shadeindex;

    DDdevice_InqShadeRange(DevStatus.dore_device, shadeindex,
			   &(shdrange[0]), &(shdrange[1]));

    DevStatus.rampmin = shdrange[0];
    DevStatus.rampmax = shdrange[1];

#   if DEBUGP
	printf("Indices set to min=%d, max=%d\n", shdrange[0], shdrange[1]);
#   endif

    /* ddr_set_iris_material_color_range(shdrange[0],shdrange[1]); */

    ddr_iris_restore_diffuse_color();

    /* This should take into account the Z depths given to the other depthcue
    ** calls: */

    if (DevStatus.visualtype == DcPseudoColor)
	lshaderange
	    (shdrange[0], shdrange[1], getgdesc(GD_ZMIN), getgdesc(GD_ZMAX));
}



/* ======================================================================== */

void ddr_gl_drom_set_att_spcclr (
    DtColorModel colormodel,
    DtReal color[])
{
#   if DEBUGP
	printf("* Specular_color(%f,%f,%f)\n",color[0],color[1],color[2]);
#   endif

    DevStatus.specularcolor[0] = color[0];
    DevStatus.specularcolor[1] = color[1];
    DevStatus.specularcolor[2] = color[2];

    ddr_iris_restore_specular_color();
}



/* ======================================================================== */

void ddr_gl_drom_set_att_spcfct (DtReal factor)
{
#   if DEBUGP
	printf("* Specular_factor(%f)\n",factor);
#   endif
    DevStatus.specularfactor = factor;

    ddr_iris_restore_specular_factor();
}



/* ======================================================================== */

void ddr_gl_drom_set_att_spcint (DtReal intensity)
{
#   if DEBUGP
	printf("* Specular_intensity(%f)\n",intensity);
#   endif

    DevStatus.specularintens = intensity;

    ddr_iris_restore_specular_color();
}



/* ======================================================================== */

void ddr_gl_drom_set_att_spcswi (DtSwitch switchvalue)
{
#   if DEBUGP
	printf("* Specular_switch(%d)\n",switchvalue);
#   endif

    DevStatus.specularswitch = switchvalue;

    ddr_iris_restore_specular_factor();
}



/* ======================================================================== */

void ddr_gl_drom_set_att_srfedgclr (
    DtColorModel colormodel,
    DtReal color[])
{
}



/* ======================================================================== */

void ddr_gl_drom_set_att_srfshd (DtObject callbackobj)
{
#   if DEBUGP
	printf("* Set_surface_shader(%s)\n",
	    (callbackobj==DcShaderConstant) ? "Off" : "On");
#   endif

    if (callbackobj == DcShaderConstant) {
	lmbind (LMODEL, 0);
	lmbind (MATERIAL, 0);
	lmcolor (LMC_COLOR);
	DevStatus.lighting = 0;
    } else {
	lmbind (LMODEL, 1);
	lmbind (MATERIAL, 1);
	lmcolor (LMC_AD);
	DevStatus.lighting = 1;
    }

    ddr_iris_restore_diffuse_color();
}



/* ======================================================================== */

void ddr_gl_drom_set_att_transpclr (
    DtColorModel colormodel,
    DtReal color[])
{
}



/* ======================================================================== */

void ddr_gl_drom_set_att_transpint (DtReal value)
{
#   if DEBUGP
	printf("* Transp_intensity(%f)\n",value);
#   endif

    DevStatus.transpintens = value;

    ddr_iris_restore_transparency();
}



/* ======================================================================== */

void ddr_gl_drom_set_att_transpswi (DtSwitch switchvalue)
{
#   if DEBUGP
	printf("* Transp_switch(%d)\n",switchvalue);
#   endif

    DevStatus.transpswitch = switchvalue;

    ddr_iris_restore_transparency();
}
