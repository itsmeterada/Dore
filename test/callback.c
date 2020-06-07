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
 
/*  ======================================================================
    This file contains the definition of all the callback functions in 
    makeobject() and makestudio().  These callbacks allow the display
    and studio objects to be altered on the fly through user keyboard
    interaction.
    ====================================================================== */

#include <dore/dore.h>
#include <stdio.h>
#include <sys/types.h>

#if defined(sun) && !defined(SVR4)	/* ! Solaris 2 */
#include <suntool/sunview.h>
#include <suntool/canvas.h>
#endif

extern DtSwitch			backfacecullswitch;
extern DtInt   			interptype;
extern DtInt   			reptype;
extern DtReal  			rotatevalue[3];
extern DtReal  			scalevalue[3];
extern DtSwitch			specularswitch;
extern DtReal  			translatevalue[3];
extern DtReal  			subdivlevel;
extern DtFlag  			clearflag;
extern DtInt   			shadetype;
extern DtReal  			ambientintensity;
extern DtReal  			diffuseintensity;
extern DtSwitch  		depthcueswitch;
extern DtSwitch 		shadowswitch;
extern DtSwitch 		reflectionswitch;
extern DtSwitch 		transpswitch;
extern DtReal  			transpintensity;
extern DtExtendMode             umode;
extern DtExtendMode             vmode;
extern DtReal                   uscale;
extern DtReal                   vscale;
extern DtReal                   utranslate;
extern DtReal                   vtranslate;
extern DtTextureAntiAliasMode   aa_mode;
extern DtTextureOperator        tex_op;

/*
  ======================================================================
 */
void
light_ambient_callback()
{
   DsExecuteObj( DoLightType(DcLightAmbient) );
}
/*
  ======================================================================
 */
void
light_infinite_callback()
{
   DsExecuteObj( DoLightType(DcLightInfinite) );
}
/*
  ======================================================================
 */
void
ambient_intensity_callback()
{
   DsExecuteObj( DoLightIntens(ambientintensity) );
}
/*
  ======================================================================
 */
void
diffuse_intensity_callback()
{
   DsExecuteObj( DoLightIntens(diffuseintensity) );
}
/*
  ======================================================================
 */
void
interptype_callback()
{
   DsExecuteObj( DoInterpType(interptype) );
}
/*
  ======================================================================
 */
void
reptype_callback()
{
   DsExecuteObj( DoRepType(reptype) );
}
/*
  ======================================================================
 */
void
specular_callback()
{
   DsExecuteObj( DoSpecularSwitch(specularswitch) );
}
/*
  ======================================================================
 */
void
backface_callback()
{
   DsExecuteObj( DoBackfaceCullSwitch(backfacecullswitch) );
}
/*
  ======================================================================
 */
void
rotate_callback()
{
   DsExecuteObj( DoRotate(0,rotatevalue[0]) );
   DsExecuteObj( DoRotate(1,rotatevalue[1]) );
   DsExecuteObj( DoRotate(2,rotatevalue[2]) );
}
/*
  ======================================================================
 */
void
scale_callback()
{
   DsExecuteObj( DoScale(scalevalue[0],scalevalue[1],scalevalue[2]));
}
/*
  ======================================================================
 */
void
subdivspec_callback()
{
   DsExecuteObj( DoSubDivSpec(2,&subdivlevel) );
}
/*
  ======================================================================
 */
void
translate_callback()
{
   DsExecuteObj( DoTranslate(translatevalue[0],translatevalue[1],
			     translatevalue[2]));
}
/*
  ======================================================================
 */
void
depthcue_callback()
{
   DsExecuteObj( DoDepthCueSwitch(depthcueswitch) );
}
/*
  ======================================================================
 */
shadow_callback()
{
   DsExecuteObj( DoShadowSwitch(shadowswitch) );
}
/*
  ======================================================================
 */
reflection_callback()
{
   DsExecuteObj( DoReflectionSwitch(reflectionswitch) );
}
/*
  ======================================================================
 */
transp_callback()
{
   DsExecuteObj( DoTranspSwitch(transpswitch) );
}
/*
  ======================================================================
 */
transp_intensity_callback()
{
   DsExecuteObj( DoTranspIntens(transpintensity) );
}

/*
  ======================================================================
 */
texture_extend_callback()
{
   DsExecuteObj( DoTextureExtendUV(umode, vmode) );
}
/*
  ======================================================================
 */
texture_scale_callback()
{
   DsExecuteObj( DoTextureScaleUV(uscale, vscale) );
}
/*
  ======================================================================
 */
texture_translate_callback()
{
   DsExecuteObj( DoTextureTranslateUV(utranslate, vtranslate) );
}
/*
  ======================================================================
 */
texture_antialias_callback()
{
   DsExecuteObj( DoTextureAntiAlias(aa_mode) );
}
/*
  ======================================================================
 */
texture_op_callback()
{
   DsExecuteObj( DoTextureOp(tex_op) );
}
/*
  ======================================================================
 */

