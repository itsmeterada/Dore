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
	int ddr_x11_titang2_drom_update_state ()
	int ddr_x11_titang2_drom_set_att_ambint (intensity)
	int ddr_x11_titang2_drom_set_att_ambswi (switchvalue)
	int ddr_x11_titang2_drom_set_att_bacfacculble (switchvalue)
	int ddr_x11_titang2_drom_set_att_bacfacculswi (switchvalue)
	int ddr_x11_titang2_drom_set_att_depcue (zfront,zback,sfront,sback,colormodel,color)
	int ddr_x11_titang2_drom_set_att_depcueswi (switchvalue)
	int ddr_x11_titang2_drom_set_att_difswi (switchvalue)
	int ddr_x11_titang2_drom_set_att_difclr (colormodel, color)
	int ddr_x11_titang2_drom_set_att_difint (intensity)
	int ddr_x11_titang2_drom_set_att_hidsrfswi (switchvalue)
	int ddr_x11_titang2_drom_set_att_inttyp (interptype)
	int ddr_x11_titang2_drom_set_att_lintyp (type)
	int ddr_x11_titang2_drom_set_att_linwid (width)
	int ddr_x11_titang2_drom_set_att_refswi (switchvalue)
	int ddr_x11_titang2_drom_set_att_reptyp (reptype)
	int ddr_x11_titang2_drom_set_att_shaswi (switchvalue)
	int ddr_x11_titang2_drom_set_att_shdidx (shadeindex)
	int ddr_x11_titang2_drom_set_att_spcclr (colormodel, color)
	int ddr_x11_titang2_drom_set_att_spcfct (factor)
	int ddr_x11_titang2_drom_set_att_spcint (intensity)
	int ddr_x11_titang2_drom_set_att_spcswi (switchvalue)
	int ddr_x11_titang2_drom_set_att_srfshd (callbackobj)
	int ddr_x11_titang2_drom_set_att_transpclr (colormodel, color)
	int ddr_x11_titang2_drom_set_att_transpint (value)
	int ddr_x11_titang2_drom_set_att_transpswi (switchvalue)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */

ddr_x11_titang2_drom_update_state()

{

    if (!dde_x11_titang2_drom_pre_init) {
	dor_matrix_load(dde_x11_titang2_drom_curcamdat->lcstofcsmat,
			dde_x11_titang2_drom_curcamdat->wcstofcsmat);
	dor_matrix_concat(dde_x11_titang2_drom_curcamdat->lcstofcsmat,
			  dde_x11_titang2_drom_data.object_lcstowcsmat,
			  DcPreConcatenate);
    }

    dde_x11_titang2_drom_data.draw_switch = 
	    (dde_x11_titang2_drom_data.object_surface_shader == DcShaderConstant);

    dde_x11_titang2_drom_data.diffuse_reflectance[0] =
	    dde_x11_titang2_drom_data.object_diffuse_intensity*
		    dde_x11_titang2_drom_data.object_diffuse_color[0];
    dde_x11_titang2_drom_data.diffuse_reflectance[1] =
	    dde_x11_titang2_drom_data.object_diffuse_intensity*
		    dde_x11_titang2_drom_data.object_diffuse_color[1];
    dde_x11_titang2_drom_data.diffuse_reflectance[2] =
	    dde_x11_titang2_drom_data.object_diffuse_intensity*
		    dde_x11_titang2_drom_data.object_diffuse_color[2];

    dde_x11_titang2_drom_data.ambient_reflectance[0] =
	    dde_x11_titang2_drom_data.object_ambient_intensity*
		    dde_x11_titang2_drom_data.object_diffuse_color[0];
    dde_x11_titang2_drom_data.ambient_reflectance[1] =
	    dde_x11_titang2_drom_data.object_ambient_intensity*
		    dde_x11_titang2_drom_data.object_diffuse_color[1];
    dde_x11_titang2_drom_data.ambient_reflectance[2] =
	    dde_x11_titang2_drom_data.object_ambient_intensity*
		    dde_x11_titang2_drom_data.object_diffuse_color[2];

    dde_x11_titang2_drom_data.specular_reflectance[0] =
	    dde_x11_titang2_drom_data.object_specular_intensity*
		    dde_x11_titang2_drom_data.object_specular_color[0];
    dde_x11_titang2_drom_data.specular_reflectance[1] =
	    dde_x11_titang2_drom_data.object_specular_intensity*
		    dde_x11_titang2_drom_data.object_specular_color[1];
    dde_x11_titang2_drom_data.specular_reflectance[2] =
	    dde_x11_titang2_drom_data.object_specular_intensity*
		    dde_x11_titang2_drom_data.object_specular_color[2];

    dde_x11_titang2_drom_data.transparent_reflectance[0] =
	    dde_x11_titang2_drom_data.object_transparent_intensity*
		    dde_x11_titang2_drom_data.object_transparent_color[0];
    dde_x11_titang2_drom_data.transparent_reflectance[1] =
	    dde_x11_titang2_drom_data.object_transparent_intensity*
		    dde_x11_titang2_drom_data.object_transparent_color[1];
    dde_x11_titang2_drom_data.transparent_reflectance[2] =
	    dde_x11_titang2_drom_data.object_transparent_intensity*
		    dde_x11_titang2_drom_data.object_transparent_color[2];

    ddr_x11_titang2_drom_initialize_parms();
}
/*
 ======================================================================
 */

	/*  Ambient Intensity.  */

ddr_x11_titang2_drom_set_att_ambint(intensity)
     DtReal intensity;

{

    dde_x11_titang2_drom_data.object_ambient_intensity = intensity;

    dde_x11_titang2_drom_data.ambient_reflectance[0] =
	    dde_x11_titang2_drom_data.object_ambient_intensity *
		    dde_x11_titang2_drom_data.object_diffuse_color[0];
    dde_x11_titang2_drom_data.ambient_reflectance[1] =
	    dde_x11_titang2_drom_data.object_ambient_intensity *
		    dde_x11_titang2_drom_data.object_diffuse_color[1];
    dde_x11_titang2_drom_data.ambient_reflectance[2] =
	    dde_x11_titang2_drom_data.object_ambient_intensity *
		    dde_x11_titang2_drom_data.object_diffuse_color[2];
}
/*
 ======================================================================
 */

	/*  Ambient switch.  */

ddr_x11_titang2_drom_set_att_ambswi(switchvalue)
     DtSwitch switchvalue;

{
    dde_x11_titang2_drom_data.object_ambient_switch = switchvalue;

}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_set_att_bacfacculble(switchvalue)
     DtSwitch switchvalue;

{
    dde_x11_titang2_drom_data.object_cullable_switch = switchvalue;

    dde_x11_titang2_drom_cg_sparms.cg_ctl &= ~(CG_mCULL);

    if (dde_x11_titang2_drom_data.object_cull_switch &&
	dde_x11_titang2_drom_data.object_cullable_switch) {
	dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_BFCULL;
    } else {
	dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_NOCULL;
    }
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_set_att_bacfacculswi(switchvalue)
     DtSwitch switchvalue;

{
    dde_x11_titang2_drom_data.object_cull_switch = switchvalue;

    dde_x11_titang2_drom_cg_sparms.cg_ctl &= ~(CG_mCULL);

    if (dde_x11_titang2_drom_data.object_cull_switch &&
	dde_x11_titang2_drom_data.object_cullable_switch) {
	dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_BFCULL;
    } else {
	dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_NOCULL;
    }
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_set_att_depcue(zfront,zback,sfront,sback,colormodel,color)
     DtReal zfront, zback;
     DtReal sfront, sback;
     DtColorModel colormodel;
     DtReal *color;

{
    static DtReal dprange[3];
    static DtReal dpfrac[2];

    dde_x11_titang2_drom_data.object_depthcue_zfront = zfront * 65535.;
    dde_x11_titang2_drom_data.object_depthcue_zback = zback * 65535.;
    dde_x11_titang2_drom_data.object_depthcue_sfront = sfront;
    dde_x11_titang2_drom_data.object_depthcue_sback = sback;

    dde_x11_titang2_drom_data.object_depthcue_color[0] = color[0];
    dde_x11_titang2_drom_data.object_depthcue_color[1] = color[1];
    dde_x11_titang2_drom_data.object_depthcue_color[2] = color[2];

    dprange[0] = 1. / (dde_x11_titang2_drom_data.object_depthcue_zfront-
		       dde_x11_titang2_drom_data.object_depthcue_zback);
    dprange[1] = dde_x11_titang2_drom_data.object_depthcue_zfront;
    dprange[2] = dde_x11_titang2_drom_data.object_depthcue_zback;;

    dde_x11_titang2_drom_cg_sparms.cg_dprange = dprange;

    dpfrac[0] = dde_x11_titang2_drom_data.object_depthcue_sfront;
    dpfrac[1] = dde_x11_titang2_drom_data.object_depthcue_sback;

    dde_x11_titang2_drom_cg_sparms.cg_dpfracs = dpfrac;

    dde_x11_titang2_drom_cg_sparms.cg_dpcolor = 
	    dde_x11_titang2_drom_data.object_depthcue_color;

    dde_x11_titang2_drom_cg_sparms.cg_ctl &= ~(CG_mDPCUE);

    if (dde_x11_titang2_drom_data.object_depthcue_switch) {
	if (dde_x11_titang2_drom_data.object_depthcue_zfront == 1.*65535. &&
	    dde_x11_titang2_drom_data.object_depthcue_zback == 0. &&
	    dde_x11_titang2_drom_data.object_depthcue_sfront == 1. &&
	    dde_x11_titang2_drom_data.object_depthcue_sback == 0.) {
	    if (dde_x11_titang2_drom_data.object_depthcue_color[0] == 0. &&
		dde_x11_titang2_drom_data.object_depthcue_color[1] == 0. &&
		dde_x11_titang2_drom_data.object_depthcue_color[2] == 0.) {
		dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_DPCUEBB;
	    } else {
		dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_DPCUECB;
	    }
	} else{
	    dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_DPCUEFULL;
	}
    } else {
	dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_NODPCUE;
    }

#ifdef DEBUG
    ddr_x11_titang2_drom_print_fixed_parms();
#endif
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_set_att_depcueswi(switchvalue)
     DtSwitch switchvalue;

{
    dde_x11_titang2_drom_data.object_depthcue_switch = switchvalue;
    dde_x11_titang2_drom_cg_sparms.cg_ctl &= ~(CG_mDPCUE);

    if (dde_x11_titang2_drom_data.object_depthcue_switch) {
	if (dde_x11_titang2_drom_data.object_depthcue_zfront == 1.*65535. &&
	    dde_x11_titang2_drom_data.object_depthcue_zback == 0. &&
	    dde_x11_titang2_drom_data.object_depthcue_sfront == 1. &&
	    dde_x11_titang2_drom_data.object_depthcue_sback == 0.) {
	    if (dde_x11_titang2_drom_data.object_depthcue_color[0] == 0. &&
		dde_x11_titang2_drom_data.object_depthcue_color[1] == 0. &&
		dde_x11_titang2_drom_data.object_depthcue_color[2] == 0.) {
		dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_DPCUEBB;
	    } else {
		dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_DPCUECB;
	    }
	} else {
	    dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_DPCUEFULL;
	}
    } else {
	dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_NODPCUE;
    }
}
/*
 ======================================================================
 */

	/*  Diffuse Switch.  */

ddr_x11_titang2_drom_set_att_difswi(switchvalue)
     DtSwitch switchvalue;

{
    dde_x11_titang2_drom_data.object_diffuse_switch = switchvalue;
}
/*
 ======================================================================
 */

	/*  Diffuse Color.   */

ddr_x11_titang2_drom_set_att_difclr(colormodel, color)
DtColorModel colormodel;
DtReal color[];

{

    dde_x11_titang2_drom_data.object_diffuse_color[0] = color[0];
    dde_x11_titang2_drom_data.object_diffuse_color[1] = color[1];
    dde_x11_titang2_drom_data.object_diffuse_color[2] = color[2];

    dde_x11_titang2_drom_data.diffuse_reflectance[0] =
	    dde_x11_titang2_drom_data.object_diffuse_intensity *
		    dde_x11_titang2_drom_data.object_diffuse_color[0];
    dde_x11_titang2_drom_data.diffuse_reflectance[1] =
	    dde_x11_titang2_drom_data.object_diffuse_intensity *
		    dde_x11_titang2_drom_data.object_diffuse_color[1];
    dde_x11_titang2_drom_data.diffuse_reflectance[2] =
	    dde_x11_titang2_drom_data.object_diffuse_intensity *
		    dde_x11_titang2_drom_data.object_diffuse_color[2];

    dde_x11_titang2_drom_data.ambient_reflectance[0] =
	    dde_x11_titang2_drom_data.object_ambient_intensity *
		    dde_x11_titang2_drom_data.object_diffuse_color[0];
    dde_x11_titang2_drom_data.ambient_reflectance[1] =
	    dde_x11_titang2_drom_data.object_ambient_intensity *
		    dde_x11_titang2_drom_data.object_diffuse_color[1];
    dde_x11_titang2_drom_data.ambient_reflectance[2] =
	    dde_x11_titang2_drom_data.object_ambient_intensity *
		    dde_x11_titang2_drom_data.object_diffuse_color[2];
}
/*
 ======================================================================
 */

	/*  Diffuse Intensity.  */

ddr_x11_titang2_drom_set_att_difint(intensity)
     DtReal intensity;

{

    dde_x11_titang2_drom_data.object_diffuse_intensity = intensity;

    dde_x11_titang2_drom_data.diffuse_reflectance[0] =
	    dde_x11_titang2_drom_data.object_diffuse_intensity*
		    dde_x11_titang2_drom_data.object_diffuse_color[0];
    dde_x11_titang2_drom_data.diffuse_reflectance[1] =
	    dde_x11_titang2_drom_data.object_diffuse_intensity*
		    dde_x11_titang2_drom_data.object_diffuse_color[1];
    dde_x11_titang2_drom_data.diffuse_reflectance[2] =
	    dde_x11_titang2_drom_data.object_diffuse_intensity*
		    dde_x11_titang2_drom_data.object_diffuse_color[2];

}
/*
 ======================================================================
 */

	/*  Hidden Surface Switch.  */

ddr_x11_titang2_drom_set_att_hidsrfswi(switchvalue)
     DtSwitch switchvalue;

{
    dde_x11_titang2_drom_data.object_hidsurf_switch = switchvalue;

    if (dde_x11_titang2_drom_pre_init) return;

    if (dde_x11_titang2_drom_data.object_hidsurf_switch) {
	ddr_x11_titang2_dcm_set_z_function(1);
    } else {
	ddr_x11_titang2_dcm_set_z_function(9);
    }
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_set_att_inttyp(interptype)
     DtInterpType interptype;

{
    dde_x11_titang2_drom_data.object_interpolation_type = interptype;

    dde_x11_titang2_drom_cg_sparms.cg_ctl &= ~(CG_mSHDAVG);

    if (dde_x11_titang2_drom_data.object_interpolation_type == DcConstantShade) {
	dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_SHDAVG;
    } else {
	dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_NOSHDAVG;
    }
}
/*
 ======================================================================
 */

      /* Line Type */

ddr_x11_titang2_drom_set_att_lintyp (type)
     DtLineType type;
{
}
/*
 ======================================================================
 */

      /* Line Width */

ddr_x11_titang2_drom_set_att_linwid (width)
     DtReal width;
{
}
/*
 ======================================================================
 */

      /* Reflection Switch */

ddr_x11_titang2_drom_set_att_refswi(switchvalue)
     DtSwitch switchvalue;
{
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_set_att_reptyp(reptype)
     DtRepType reptype;

{
    dde_x11_titang2_drom_data.object_representation_type = reptype;
}
/*
 ======================================================================
 */

      /* Shadow Switch */

ddr_x11_titang2_drom_set_att_shaswi(switchvalue)
     DtSwitch switchvalue;
{
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_set_att_shdidx(shadeindex)
     DtInt shadeindex;

{
    DtInt shaderange[2];
    static DtReal cwtavgpsweight[] = { .33, .33, .33 };
    static DtReal cwtavgpslim[3];

    dde_x11_titang2_drom_data.object_shade_index = shadeindex;

    if (dde_x11_titang2_drom_pre_init) {
	return;
    }

    DdInqShadeRanges (dde_x11_titang2_drom_data.current_device,
				dde_x11_titang2_drom_data.object_shade_index,1,shaderange);
    dde_x11_titang2_drom_curdevdat->current_shade_range_min = (DtReal)shaderange[0];
    dde_x11_titang2_drom_curdevdat->current_shade_range_max = (DtReal)shaderange[1];
   
    if (dde_x11_titang2_drom_data.pseudocolor_switch) {
	if (dde_x11_titang2_drom_curdevdat->shade_mode == DcRange) {
	    dde_x11_titang2_drom_cg_sparms.cg_psweight = cwtavgpsweight;

	    cwtavgpslim[0] = dde_x11_titang2_drom_curdevdat->current_shade_range_min;
	    cwtavgpslim[1] = dde_x11_titang2_drom_curdevdat->current_shade_range_max;
	    cwtavgpslim[2] = (cwtavgpslim[1]-cwtavgpslim[0]) / 255.;

	    dde_x11_titang2_drom_cg_sparms.cg_pscol.cg_pslim = cwtavgpslim;
	}
    }
}
/*
 ======================================================================
 */

	/*  Specular Color.   */

ddr_x11_titang2_drom_set_att_spcclr(colormodel, color)
DtColorModel colormodel;
DtReal color[];

{

    dde_x11_titang2_drom_data.object_specular_color[0] = color[0];
    dde_x11_titang2_drom_data.object_specular_color[1] = color[1];
    dde_x11_titang2_drom_data.object_specular_color[2] = color[2];

    dde_x11_titang2_drom_data.specular_reflectance[0] =
	    dde_x11_titang2_drom_data.object_specular_intensity*
		    dde_x11_titang2_drom_data.object_specular_color[0];
    dde_x11_titang2_drom_data.specular_reflectance[1] =
	    dde_x11_titang2_drom_data.object_specular_intensity*
		    dde_x11_titang2_drom_data.object_specular_color[1];
    dde_x11_titang2_drom_data.specular_reflectance[2] =
	    dde_x11_titang2_drom_data.object_specular_intensity*
		    dde_x11_titang2_drom_data.object_specular_color[2];
}
/*
 ======================================================================
 */

	/*  Specular Factor.  */

ddr_x11_titang2_drom_set_att_spcfct(factor)
     DtReal factor;

{
    dde_x11_titang2_drom_data.object_specular_factor = factor;
}
/*
 ======================================================================
 */

	/*  Specular Intensity.  */

ddr_x11_titang2_drom_set_att_spcint(intensity)
     DtReal intensity;

{

    dde_x11_titang2_drom_data.object_specular_intensity = intensity;

    dde_x11_titang2_drom_data.specular_reflectance[0] =
	    dde_x11_titang2_drom_data.object_specular_intensity*
		    dde_x11_titang2_drom_data.object_specular_color[0];
    dde_x11_titang2_drom_data.specular_reflectance[1] =
	    dde_x11_titang2_drom_data.object_specular_intensity*
		    dde_x11_titang2_drom_data.object_specular_color[1];
    dde_x11_titang2_drom_data.specular_reflectance[2] =
	    dde_x11_titang2_drom_data.object_specular_intensity*
		    dde_x11_titang2_drom_data.object_specular_color[2];
}
/*
 ======================================================================
 */

	/*  Specular Switch.  */

ddr_x11_titang2_drom_set_att_spcswi(switchvalue)
     DtSwitch switchvalue;

{
    DtSwitch previousvalue;

    previousvalue = dde_x11_titang2_drom_data.object_specular_switch;

    dde_x11_titang2_drom_data.object_specular_switch = switchvalue;

    /*  If the switch was previously off, and has now been set on, then
	make sure any changes in the lcstowcsmat are taken into account.  */

    if (!previousvalue && switchvalue) {
	ddr_x11_titang2_drom_update_lights_specular();
    }
    dde_x11_titang2_drom_cg_sparms.cg_ctl &= ~(CG_mSPEC);

    if (!dde_x11_titang2_drom_data.object_specular_switch) {
	dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_NOSPEC;
    } else {
	dde_x11_titang2_drom_cg_sparms.cg_ctl |= CG_SOBJ;
    }
}
/*
 ======================================================================
 */

	/*  Surface Shader Callback Object.  */

ddr_x11_titang2_drom_set_att_srfshd(callbackobj)
     DtObject callbackobj;

{
    DtSwitch previousvalue;

    dde_x11_titang2_drom_data.object_surface_shader = callbackobj;

    previousvalue = dde_x11_titang2_drom_data.draw_switch;

    dde_x11_titang2_drom_data.draw_switch = 
	    (dde_x11_titang2_drom_data.object_surface_shader == DcShaderConstant);

    /*  If the draw switch was previously on, and has now been turned off,
	update the lighting stuff.  */

    if (previousvalue  && !dde_x11_titang2_drom_data.draw_switch) {
	/*  World to local normal transformation matrix is the transpose of
	    the inverse of the wcstolcsmat.  Since lcstowcsmat is already the
	    inverse of wcstolcsmat, we merely need to take the transpose of this
	    matrix.  */

	dor_matrix_load(dde_x11_titang2_drom_data.nrmwcstolcsmat,
			dde_x11_titang2_drom_data.object_lcstowcsmat);
	dor_matrix_transpose(dde_x11_titang2_drom_data.nrmwcstolcsmat);

	ddr_x11_titang2_drom_update_lights();
    }
}
/*
 ======================================================================
 */

	/*  Transparency color.  */

ddr_x11_titang2_drom_set_att_transpclr(colormodel, color)
DtColorModel colormodel;
DtReal color[];

{

    dde_x11_titang2_drom_data.object_transparent_color[0] = color[0];
    dde_x11_titang2_drom_data.object_transparent_color[1] = color[1];
    dde_x11_titang2_drom_data.object_transparent_color[2] = color[2];

    dde_x11_titang2_drom_data.transparent_reflectance[0] =
	    dde_x11_titang2_drom_data.object_transparent_intensity*
		    dde_x11_titang2_drom_data.object_transparent_color[0];
    dde_x11_titang2_drom_data.transparent_reflectance[1] =
	    dde_x11_titang2_drom_data.object_transparent_intensity*
		    dde_x11_titang2_drom_data.object_transparent_color[1];
    dde_x11_titang2_drom_data.transparent_reflectance[2] =
	    dde_x11_titang2_drom_data.object_transparent_intensity*
		    dde_x11_titang2_drom_data.object_transparent_color[2];
}
/*
 ======================================================================
 */

	/*  Transparency intensity.  */

ddr_x11_titang2_drom_set_att_transpint(value)
     DtReal value;

{

    dde_x11_titang2_drom_data.object_transparent_intensity = value;
   
    dde_x11_titang2_drom_data.transparent_reflectance[0] =
	    dde_x11_titang2_drom_data.object_transparent_intensity*
		    dde_x11_titang2_drom_data.object_transparent_color[0];
    dde_x11_titang2_drom_data.transparent_reflectance[1] =
	    dde_x11_titang2_drom_data.object_transparent_intensity*
		    dde_x11_titang2_drom_data.object_transparent_color[1];
    dde_x11_titang2_drom_data.transparent_reflectance[2] =
	    dde_x11_titang2_drom_data.object_transparent_intensity*
		    dde_x11_titang2_drom_data.object_transparent_color[2];
}
/*
 ======================================================================
 */

	/*  Transparency switch.  */

ddr_x11_titang2_drom_set_att_transpswi(switchvalue)
     DtSwitch switchvalue;

{
    /*  If we are not in the transparent surface rendering pass, then
	just indicate that at least one transparent surface was encountered,
	and must be rendered in the next (transparent surface) pass.  */

    dde_x11_titang2_drom_data.object_transparent_switch = switchvalue;

    if (dde_x11_titang2_drom_pre_init) return;

    if (!dde_x11_titang2_drom_data.transparent_surface_pass) {
	if (dde_x11_titang2_drom_data.object_transparent_switch) {
	    dde_x11_titang2_drom_data.transparent_surface_flag = DcTrue;

	    DDcondex_SetDisablePrimExecFlag(DcTrue);
	} else {
	    DDcondex_SetDisablePrimExecFlag(DcFalse);
	}
    } else {
	if (dde_x11_titang2_drom_data.object_transparent_switch) {
	    DDcondex_SetDisablePrimExecFlag(DcFalse);
	} else {
	    DDcondex_SetDisablePrimExecFlag(DcTrue);
	}
    }
}
