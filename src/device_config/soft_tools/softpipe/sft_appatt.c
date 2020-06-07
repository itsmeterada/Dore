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
	ddr_softpipe_drom_update_state
	ddr_softpipe_drom_set_att_ambint
	ddr_softpipe_drom_set_att_ambswi
	ddr_softpipe_drom_set_att_bacfacculble
	ddr_softpipe_drom_set_att_bacfacculswi
	ddr_softpipe_drom_set_att_depcue
	ddr_softpipe_drom_set_att_depcueswi
	ddr_softpipe_drom_set_att_difclr
	ddr_softpipe_drom_set_att_difint
	ddr_softpipe_drom_set_att_difswi
	ddr_softpipe_drom_set_att_hidsrfswi
	ddr_softpipe_drom_set_att_inttyp
	ddr_softpipe_drom_set_att_lintyp
	ddr_softpipe_drom_set_att_linwid
	ddr_softpipe_drom_set_att_refswi
	ddr_softpipe_drom_set_att_reptyp
	ddr_softpipe_drom_set_att_shaswi
	ddr_softpipe_drom_set_att_shdidx
	ddr_softpipe_drom_set_att_spcclr
	ddr_softpipe_drom_set_att_spcfct
	ddr_softpipe_drom_set_att_spcint
	ddr_softpipe_drom_set_att_spcswi
	ddr_softpipe_drom_set_att_srfshd
	ddr_softpipe_drom_set_att_transpclr
	ddr_softpipe_drom_set_att_transpint
	ddr_softpipe_drom_set_att_transpswi

  ======================================================================
 */
#include "drom.h"
#include <dore/internal/matrix.h>

/*
 ======================================================================
 */
void ddr_softpipe_drom_update_state (void)
{
#ifdef DEBUG
   printf("...processing dynout update state \n");
   fflush(stdout);
#endif

   if(!dde_softpipe_drom_pre_init) {
      dor_matrix_load(dde_softpipe_drom_curcamdat->lcstofcsmat,
		      dde_softpipe_drom_curcamdat->wcstofcsmat);
      dor_matrix_concat(dde_softpipe_drom_curcamdat->lcstofcsmat,
			dde_softpipe_drom_data.object_lcstowcsmat,
			DcPreConcatenate);
   }

   dde_softpipe_drom_data.draw_switch =
	 (dde_softpipe_drom_data.object_surface_shader ==
	  DcShaderConstant);

   dde_softpipe_drom_data.diffuse_reflectance[0] =
	 dde_softpipe_drom_data.object_diffuse_intensity*
	       dde_softpipe_drom_data.object_diffuse_color[0];
   dde_softpipe_drom_data.diffuse_reflectance[1] =
	 dde_softpipe_drom_data.object_diffuse_intensity*
	       dde_softpipe_drom_data.object_diffuse_color[1];
   dde_softpipe_drom_data.diffuse_reflectance[2] =
	 dde_softpipe_drom_data.object_diffuse_intensity*
	       dde_softpipe_drom_data.object_diffuse_color[2];

   dde_softpipe_drom_data.ambient_reflectance[0] =
	 dde_softpipe_drom_data.object_ambient_intensity*
	       dde_softpipe_drom_data.object_diffuse_color[0];
   dde_softpipe_drom_data.ambient_reflectance[1] =
	 dde_softpipe_drom_data.object_ambient_intensity*
	       dde_softpipe_drom_data.object_diffuse_color[1];
   dde_softpipe_drom_data.ambient_reflectance[2] =
	 dde_softpipe_drom_data.object_ambient_intensity*
	       dde_softpipe_drom_data.object_diffuse_color[2];

   dde_softpipe_drom_data.specular_reflectance[0] =
	 dde_softpipe_drom_data.object_specular_intensity*
	       dde_softpipe_drom_data.object_specular_color[0];
   dde_softpipe_drom_data.specular_reflectance[1] =
	 dde_softpipe_drom_data.object_specular_intensity*
	       dde_softpipe_drom_data.object_specular_color[1];
   dde_softpipe_drom_data.specular_reflectance[2] =
	 dde_softpipe_drom_data.object_specular_intensity*
	       dde_softpipe_drom_data.object_specular_color[2];

   dde_softpipe_drom_data.transparent_reflectance[0] =
	 dde_softpipe_drom_data.object_transparent_intensity*
	       dde_softpipe_drom_data.object_transparent_color[0];
   dde_softpipe_drom_data.transparent_reflectance[1] =
	 dde_softpipe_drom_data.object_transparent_intensity*
	       dde_softpipe_drom_data.object_transparent_color[1];
   dde_softpipe_drom_data.transparent_reflectance[2] =
	 dde_softpipe_drom_data.object_transparent_intensity*
	       dde_softpipe_drom_data.object_transparent_color[2];
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_ambint (DtReal intensity)
{
#   ifdef DEBUG
       printf("...processing dynout set att ambient intensity\n");
       printf("ambient intensity = %lf\n",intensity);
       fflush(stdout);
#    endif

    dde_softpipe_drom_data.object_ambient_intensity = intensity;

    dde_softpipe_drom_data.ambient_reflectance[0]
        = dde_softpipe_drom_data.object_ambient_intensity
        * dde_softpipe_drom_data.object_diffuse_color[0];

    dde_softpipe_drom_data.ambient_reflectance[1]
        = dde_softpipe_drom_data.object_ambient_intensity
        * dde_softpipe_drom_data.object_diffuse_color[1];

    dde_softpipe_drom_data.ambient_reflectance[2]
        = dde_softpipe_drom_data.object_ambient_intensity
	* dde_softpipe_drom_data.object_diffuse_color[2];
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_ambswi (DtSwitch switchvalue)
{
#ifdef DEBUG
   printf("...processing dynout set att ambswi\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_ambient_switch = switchvalue;
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_bacfacculble (DtSwitch switchvalue)
{
#ifdef DEBUG
   printf("...processing dynout set att bacfacculble\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_cullable_switch = switchvalue;
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_bacfacculswi (DtSwitch switchvalue)
{
#ifdef DEBUG
   printf("...processing dynout set att backfacculswi\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_cull_switch = switchvalue;
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_depcue (
    DtReal        zfront,
    DtReal        zback,
    DtReal        sfront,
    DtReal        sback,
    DtColorModel  colormodel,
    DtReal       *color)
{
   static DtReal dprange[3];
   static DtReal dpfrac[2];

#ifdef DEBUG
   printf("...processing dynout set att depcue\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_depthcue_zfront = zfront * 65535.;
   dde_softpipe_drom_data.object_depthcue_zback = zback * 65535.;
   dde_softpipe_drom_data.object_depthcue_sfront = sfront;
   dde_softpipe_drom_data.object_depthcue_sback = sback;

   dde_softpipe_drom_data.object_depthcue_color[0] = 255. * color[0];
   dde_softpipe_drom_data.object_depthcue_color[1] = 255. * color[1];
   dde_softpipe_drom_data.object_depthcue_color[2] = 255. * color[2];

   dprange[0] = 1. / (dde_softpipe_drom_data.object_depthcue_zfront -
		      dde_softpipe_drom_data.object_depthcue_zback);
   dprange[1] = dde_softpipe_drom_data.object_depthcue_zfront;
   dprange[2] = dde_softpipe_drom_data.object_depthcue_zback;

   dpfrac[0] = dde_softpipe_drom_data.object_depthcue_sfront;
   dpfrac[1] = dde_softpipe_drom_data.object_depthcue_sback;
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_depcueswi (DtSwitch switchvalue)
{
#ifdef DEBUG
   printf("...processing dynout set att depcueswi\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_depthcue_switch = switchvalue;
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_difclr (
    DtColorModel colormodel,
    DtReal       color[])
{
#ifdef DEBUG
   printf("...processing dynout set att difclr\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_diffuse_color[0] = color[0];
   dde_softpipe_drom_data.object_diffuse_color[1] = color[1];
   dde_softpipe_drom_data.object_diffuse_color[2] = color[2];

   dde_softpipe_drom_data.diffuse_reflectance[0] =
	 dde_softpipe_drom_data.object_diffuse_intensity *
	       dde_softpipe_drom_data.object_diffuse_color[0];
   dde_softpipe_drom_data.diffuse_reflectance[1] =
	 dde_softpipe_drom_data.object_diffuse_intensity *
	       dde_softpipe_drom_data.object_diffuse_color[1];
   dde_softpipe_drom_data.diffuse_reflectance[2] =
	 dde_softpipe_drom_data.object_diffuse_intensity *
	       dde_softpipe_drom_data.object_diffuse_color[2];

   dde_softpipe_drom_data.ambient_reflectance[0] =
	 dde_softpipe_drom_data.object_ambient_intensity *
	       dde_softpipe_drom_data.object_diffuse_color[0];
   dde_softpipe_drom_data.ambient_reflectance[1] =
	 dde_softpipe_drom_data.object_ambient_intensity *
	       dde_softpipe_drom_data.object_diffuse_color[1];
   dde_softpipe_drom_data.ambient_reflectance[2] =
	 dde_softpipe_drom_data.object_ambient_intensity *
	       dde_softpipe_drom_data.object_diffuse_color[2];
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_difint (DtReal intensity)
{
#ifdef DEBUG
   printf("...processing dynout set att difint\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_diffuse_intensity = intensity;

   dde_softpipe_drom_data.diffuse_reflectance[0] =
	 dde_softpipe_drom_data.object_diffuse_intensity *
	       dde_softpipe_drom_data.object_diffuse_color[0];
   dde_softpipe_drom_data.diffuse_reflectance[1] =
	 dde_softpipe_drom_data.object_diffuse_intensity *
	       dde_softpipe_drom_data.object_diffuse_color[1];
   dde_softpipe_drom_data.diffuse_reflectance[2] =
	 dde_softpipe_drom_data.object_diffuse_intensity *
	       dde_softpipe_drom_data.object_diffuse_color[2];
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_difswi (DtSwitch switchvalue)
{
#ifdef DEBUG
   printf("...processing dynout set att difswi\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_diffuse_switch = switchvalue;
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_hidsrfswi (DtSwitch switchvalue)
{
#ifdef DEBUG
   printf("...processing dynout set att hidsrfswi\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_hidsurf_switch = switchvalue;

   if (dde_softpipe_drom_pre_init)
	   return;

   DDdevice_SetDepthBufferEnable (dde_softpipe_drom_data.current_device,
				      switchvalue == DcOn ? DcTrue : DcFalse);
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_inttyp (DtInterpType interptype)
{
#ifdef DEBUG
   printf("...processing dynout set att inttyp\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_interpolation_type = interptype;
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_lintyp (DtLineType type)
{
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_linwid (DtReal width)
{
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_refswi (DtSwitch switchvalue)
{
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_reptyp (DtRepType reptype)
{
#ifdef DEBUG
   printf("...processing dynout set att reptyp\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_representation_type = reptype;
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_shaswi (DtSwitch switchvalue)
{
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_shdidx (DtInt shadeindex)
{
   DtInt shaderange[2];
   static DtReal cwtavgpsweight[] = {0.33, 0.33, 0.33};
   static DtReal cwtavgpslim[3];

#ifdef DEBUG
   printf("...processing dynout set att shdidx\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_shade_index = shadeindex;

   if(dde_softpipe_drom_pre_init) {
      return;
   }

   DDdevice_SetShadeIndex(dde_softpipe_drom_data.current_device, shadeindex);
   DdInqShadeRanges(dde_softpipe_drom_data.current_device,
			      dde_softpipe_drom_data.object_shade_index,
			      1,shaderange);
   dde_softpipe_drom_curdevdat->current_shade_range_min =
	 (DtReal)shaderange[0];
   dde_softpipe_drom_curdevdat->current_shade_range_max =
	 (DtReal)shaderange[1];
#ifdef DEBUG
   printf("shade index = %d\n",shadeindex);
   printf("shade range %d - %d\n",shaderange[0],shaderange[1]);
#endif
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_spcclr (
    DtColorModel colormodel,
    DtReal       color[])
{
#ifdef DEBUG
   printf("...processing dynout set att spcclr\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_specular_color[0] = color[0];
   dde_softpipe_drom_data.object_specular_color[1] = color[1];
   dde_softpipe_drom_data.object_specular_color[2] = color[2];

   dde_softpipe_drom_data.specular_reflectance[0] =
	 dde_softpipe_drom_data.object_specular_intensity *
	       dde_softpipe_drom_data.object_specular_color[0];
   dde_softpipe_drom_data.specular_reflectance[1] =
	 dde_softpipe_drom_data.object_specular_intensity *
	       dde_softpipe_drom_data.object_specular_color[1];
   dde_softpipe_drom_data.specular_reflectance[2] =
	 dde_softpipe_drom_data.object_specular_intensity *
	       dde_softpipe_drom_data.object_specular_color[2];
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_spcfct (DtReal factor)
{
#ifdef DEBUG
   printf("...processing dynout set att spcfct\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_specular_factor = factor;
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_spcint (DtReal intensity)
{
#ifdef DEBUG
   printf("...processing dynout set att spcint\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_specular_intensity = intensity;

   dde_softpipe_drom_data.specular_reflectance[0] =
	 dde_softpipe_drom_data.object_specular_intensity *
	       dde_softpipe_drom_data.object_specular_color[0];
   dde_softpipe_drom_data.specular_reflectance[1] =
	 dde_softpipe_drom_data.object_specular_intensity *
	       dde_softpipe_drom_data.object_specular_color[1];
   dde_softpipe_drom_data.specular_reflectance[2] =
	 dde_softpipe_drom_data.object_specular_intensity *
	       dde_softpipe_drom_data.object_specular_color[2];
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_spcswi (DtSwitch switchvalue)
{
   DtSwitch previousvalue;

#ifdef DEBUG
   printf("...processing dynout set att spcswi\n");
   fflush(stdout);
#endif

   previousvalue = dde_softpipe_drom_data.object_specular_switch;

   dde_softpipe_drom_data.object_specular_switch = switchvalue;

   if(!previousvalue && switchvalue) {
      ddr_softpipe_drom_update_lights_specular();
   }
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_srfshd (DtObject callbackobj)
{
   DtSwitch previousvalue;

#ifdef DEBUG
   printf("...processing dynout set att srfshd\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_surface_shader = callbackobj;

   previousvalue = dde_softpipe_drom_data.draw_switch;

   dde_softpipe_drom_data.draw_switch =
	 (dde_softpipe_drom_data.object_surface_shader ==
	  DcShaderConstant);

   if(previousvalue && !dde_softpipe_drom_data.draw_switch) {
      dor_matrix_load(dde_softpipe_drom_data.nrmwcstolcsmat,
		      dde_softpipe_drom_data.object_lcstowcsmat);
      dor_matrix_transpose(dde_softpipe_drom_data.nrmwcstolcsmat);

      ddr_softpipe_drom_update_lights();
   }
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_transpclr (
    DtColorModel colormodel,
    DtReal       color[])
{
#ifdef DEBUG
   printf("...processing dynout set att transpclr\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_transparent_color[0] = color[0];
   dde_softpipe_drom_data.object_transparent_color[1] = color[1];
   dde_softpipe_drom_data.object_transparent_color[2] = color[2];

   dde_softpipe_drom_data.transparent_reflectance[0] =
	 dde_softpipe_drom_data.object_transparent_intensity *
	       dde_softpipe_drom_data.object_transparent_color[0];
   dde_softpipe_drom_data.transparent_reflectance[1] =
	 dde_softpipe_drom_data.object_transparent_intensity *
	       dde_softpipe_drom_data.object_transparent_color[1];
   dde_softpipe_drom_data.transparent_reflectance[2] =
	 dde_softpipe_drom_data.object_transparent_intensity *
	       dde_softpipe_drom_data.object_transparent_color[2];
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_transpint (DtReal value)
{
#ifdef DEBUG
   printf("...processing dynout set att transpint\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_transparent_intensity = value;

   dde_softpipe_drom_data.transparent_reflectance[0] =
	 dde_softpipe_drom_data.object_transparent_intensity *
	       dde_softpipe_drom_data.object_transparent_color[0];
   dde_softpipe_drom_data.transparent_reflectance[1] =
	 dde_softpipe_drom_data.object_transparent_intensity *
	       dde_softpipe_drom_data.object_transparent_color[1];
   dde_softpipe_drom_data.transparent_reflectance[2] =
	 dde_softpipe_drom_data.object_transparent_intensity *
	       dde_softpipe_drom_data.object_transparent_color[2];
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_set_att_transpswi (DtSwitch switchvalue)
{
#ifdef DEBUG
   printf("...processing dynout set att transpswi\n");
   fflush(stdout);
#endif

   dde_softpipe_drom_data.object_transparent_switch = switchvalue;
}
