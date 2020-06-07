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
 
#include <stdio.h>
#include "dore.h"
#include "trnsfrmr.h"
#include "dore/dore_develop/develop.h"

#define max(a,b)	(((a) > (b)) ? (a) : (b))
#define min(a,b)	(((a) < (b)) ? (a) : (b))

DtSwitch	superswi;
DtSwitch	hidsrfswi;
DtReal		stereo_separation;
DtReal		stereo_distance;
DtInt		interptype;
DtInt		reptype;
DtInt		linetype;
DtInt		specularswitch;
DtInt		backfacecullswitch;
DtReal		subdivlevel;
DtFlag		depthcueswitch;
DtFlag          transpswitch;
DtFlag          localaaswitch;
DtReal		ambientintensity;
DtReal		diffuseintensity;
DtFlag		constantshader;
DtInt		geom_select;
DtInt		geom_max;
DtReal		rotatevalue[9][3];
DtReal		scalevalue[9][3];
DtReal		translatevalue[9][3];
DtFlag                   textureswitch;
DtExtendMode             umode;
DtExtendMode             vmode;
DtExtendMode             wmode;
DtReal                   uscale;
DtReal                   vscale;
DtReal                   wscale;
DtReal                   utranslate;
DtReal                   vtranslate;
DtReal                   wtranslate;
DtTextureAntiAliasMode   aa_mode;
DtTextureOperator        tex_op;

DtInt	tf_rot = -1;
DtObject transformer = DcNullObject;
DtReal rotx_val = 0.;
DtReal roty_val = 0.;
DtReal rotz_val = 0.;

char		rotate_name[9][15]	=
		{"rotate", "rotate1", "rotate2", 
		 "rotate3", "rotate4", "rotate5",
		 "rotate6", "rotate7", "rotate8" };
char		scale_name[9][15]	=
		{"scale", "scale1", "scale2", 
		 "scale3", "scale4", "scale5",
		 "scale6", "scale7", "scale8" };
char		trans_name[9][15]	=
		{"translate", "translate1", "translate2", 
		 "translate3", "translate4", "translate5",
		 "translate6", "translate7", "translate8" };

/*
  ======================================================================
 */
int reset_callbacks (void)
{
    DtInt i, j;
    extern DtInt step_translate, step_rotate, step_scale;

    superswi		= DcFalse;
    hidsrfswi		= DcTrue;
    stereo_separation	= 0.08;
    stereo_distance	= 10.0;
    interptype		= 1;
    reptype		= 2;
    linetype		= 0;
    specularswitch	= 0;
    backfacecullswitch 	= 0;
    subdivlevel		= 0.08;
    depthcueswitch	= DcFalse;
    transpswitch        = DcFalse;
    localaaswitch       = DcFalse;
    ambientintensity	= 0.0;
    diffuseintensity	= 1.;
    constantshader	= 0;
    step_translate = step_scale = step_rotate = 1;
    
    geom_select		= 0;
    for (i=0; i<9; i++) {
	for (j=0; j<3; j++) {
	    rotatevalue[i][j] = 0.;
	    scalevalue[i][j] = 1.;
	    translatevalue[i][j] = 0.;
	}
    }
    if (tf_rot != -1) {
	DsReleaseObj(transformer);
	transformer = DsHoldObj(change_transformer(tf_rot,Reset,0.,
						   DcPostConcatenate,DcTrue));
    }
    rotx_val = 0.;
    roty_val = 0.;
    rotz_val = 0.;

    textureswitch = DcTrue;
    umode = vmode = wmode = DcExtendNone;
    uscale = vscale = wscale = 1.0;
    utranslate = vtranslate = wtranslate = 0.0;
    aa_mode = DcTextureAANone;
    tex_op = DcTextureReplace;

    printf ("callback values reset\n");
}
/*
  ======================================================================
 */
stdmap_callback ()        { printf ("standard map callback\n"); }
super_sample_callback ()  { DsExecuteObj( DoSampleSuperSwitch(superswi)); }
hidsurf_callback ()       { DsExecuteObj( DoHiddenSurfSwitch(hidsrfswi)); }
light_ambient_callback()  { DsExecuteObj( DoLightType(DcLightAmbient) ); }
light_infinite_callback() { DsExecuteObj( DoLightType(DcLightInfinite) ); }
stereo_callback()         { DsExecuteObj( DoStereo(stereo_separation,stereo_distance) ); }
interptype_callback()     { DsExecuteObj( DoInterpType(interptype) ); }
reptype_callback()        { DsExecuteObj( DoRepType(reptype) ); }
linetype_callback()       { DsExecuteObj( DoLineType(linetype) ); }
specular_callback()       { DsExecuteObj( DoSpecularSwitch(specularswitch) ); }
backface_callback()       { DsExecuteObj( DoBackfaceCullSwitch(backfacecullswitch) ); }
subdivspec_callback()     { DsExecuteObj( DoSubDivSpec(2,&subdivlevel) ); }
depthcue_callback()       { DsExecuteObj( DoDepthCueSwitch(depthcueswitch) ); }
transp_callback()         { DsExecuteObj( DoTranspSwitch(transpswitch) ); }
localaa_callback()        { DsExecuteObj( DoLocalAntiAliasSwitch(localaaswitch) ); }
/*
  ======================================================================
 */
int error_callback (void)
{
    extern DtObject device;

    DdUpdate(device);
}
/*
  ======================================================================
 */
int ambient_intensity_callback (void)
{
   extern DtSwitch multiply_lgt_intens;
   DtReal currentvalue;

   if (multiply_lgt_intens) {
       DDclass_InqMethod (DcTypeAttLightIntens,DcMethodInqGlbAttVal)
	       (&currentvalue);
   } else {
       currentvalue = 1.0;
   }
   DsExecuteObj( DoLightIntens(ambientintensity*currentvalue) );
}
/*
  ======================================================================
 */
int diffuse_intensity_callback (void)
{
   extern DtSwitch multiply_lgt_intens;
   DtReal currentvalue;

   if (multiply_lgt_intens) {
       DDclass_InqMethod (DcTypeAttLightIntens,DcMethodInqGlbAttVal)
	       (&currentvalue);
   } else {
       currentvalue = 1.0;
   }

   DsExecuteObj( DoLightIntens(diffuseintensity*currentvalue) );
}
/*
  ======================================================================
 */
int shader_callback (void)
{
    if (constantshader) 
	    DsExecuteObj(DoSurfaceShade(DcShaderConstant));
    else
	    DsExecuteObj(DoSurfaceShade(DcShaderLightSource));
}
/*
  ======================================================================
 */
int rotate_callback (Dt32Bits val)
{
    DtReal rot;

    geom_max = max(geom_max,val);
    if (val <= 7) {
	DsExecuteObj( DoRotate(0,rotatevalue[val][0]) );
	DsExecuteObj( DoRotate(1,rotatevalue[val][1]) );
	DsExecuteObj( DoRotate(2,rotatevalue[val][2]) );
    } else {
	if (tf_rot == -1) {
	    tf_rot = make_transformer(DcPostConcatenate);
	    transformer = DsHoldObj(change_transformer(tf_rot,RotX,0.,
						       DcPostConcatenate,DcTrue));

	}
	rot = rotatevalue[val][0] - rotx_val;
	if (rot != 0.) {
	    DsReleaseObj(transformer);
	    transformer = DsHoldObj(change_transformer(tf_rot,RotX,-rot,
						       DcPostConcatenate,DcTrue));
	    rotx_val = rotatevalue[val][0];
	}
	rot = rotatevalue[val][1] - roty_val;
	if (rot != 0.) {
	    DsReleaseObj(transformer);
	    transformer = DsHoldObj(change_transformer(tf_rot,RotY,-rot,
						       DcPostConcatenate,DcTrue));
	    roty_val = rotatevalue[val][1];
	}
	rot = rotatevalue[val][2] - rotz_val;
	if (rot != 0.) {
	    DsReleaseObj(transformer);
	    transformer = DsHoldObj(change_transformer(tf_rot,RotZ,-rot,
						       DcPostConcatenate,DcTrue));
	    rotz_val = rotatevalue[val][2];
	}
	DsExecuteObj (transformer);
    }
}
/*
  ======================================================================
 */
int scale_callback (Dt32Bits val)
{
   geom_max = max(geom_max,val);
   DsExecuteObj( DoScale(scalevalue[val][0],
			 scalevalue[val][1],
			 scalevalue[val][2]));
}
/*
  ======================================================================
 */
int trans_callback (Dt32Bits val)
{

   geom_max = max(geom_max,val);
   DsExecuteObj( DoTranslate(translatevalue[val][0],
			     translatevalue[val][1],
			     translatevalue[val][2]));
}
/*
  ======================================================================
 */
int stereo_test_callback (void)
{
   extern DtSwitch stereoflag;

   static int buffer_number = 1;
   static int stereo_left = 1;
   static DtObject text1 = DcNullObject;
   static DtObject text2 = DcNullObject;
   static DtObject stereol = DcNullObject;
   static DtObject stereor = DcNullObject;
   DtPoint3 pos;
   static DtVector3 u = {1., 0., 0.};
   static DtVector3 v = {0., 1., 0.};

   if (buffer_number) {
      if (text2 == DcNullObject) {
	 text2 = DoGroup(DcTrue);
	 DgAddObj(DoTextFont(DcHelvetica));
	 pos[0] = -4.0;
	 pos[1] = 1.0;
	 pos[2] = 0.0;
	 DgAddObj(DoText(pos, u, v, "Buffer 1"));
	 DgClose();
	 DsHoldObj (text2);
      }
      DsExecuteObj(text2);
   } else {
      if (text1 == DcNullObject) {
	 text1 = DoGroup(DcTrue);
	 DgAddObj(DoTextFont(DcHelvetica));
	 pos[0] = -4.0;
	 pos[1] = 1.0;
	 pos[2] = 0.0;
	 DgAddObj(DoText(pos, u, v, "Buffer 0"));
	 DgClose();
	 DsHoldObj (text1);
      }
      DsExecuteObj(text1);
   }

   if (stereoflag) {
      if (stereo_left) {
	 if (stereol == DcNullObject) {
	    stereol = DoGroup(DcTrue);
	    DgAddObj(DoTextFont(DcHelvetica));
	    pos[0] = -4.0;
	    pos[1] = -1.0;
	    pos[2] = 0.0;
	    DgAddObj(DoText(pos, u, v, "Left"));
	    DgClose();
	    DsHoldObj (stereol);
	 }
	 DsExecuteObj(stereol);
      } else {
	 if (stereor == DcNullObject) {
	    stereor = DoGroup(DcTrue);
	    DgAddObj(DoTextFont(DcHelvetica));
	    pos[0] = 0.0;
	    pos[1] = -1.0;
	    pos[2] = 0.0;
	    DgAddObj(DoText(pos, u, v, "Right"));
	    DgClose();
	    DsHoldObj (stereor);
	 }
	 DsExecuteObj(stereor);

	 buffer_number = !buffer_number;
      }
      stereo_left = !stereo_left;
   } else {
      if (stereol == DcNullObject) {
	 stereol = DoGroup(DcTrue);
	 DgAddObj(DoTextFont(DcHelvetica));
	 pos[0] = -4.0;
	 pos[1] = -1.0;
	 pos[2] = 0.0;
	 DgAddObj(DoText(pos, u, v, "Non-Stereo"));
	 DgClose();
	 DsHoldObj (stereol);
      }
      DsExecuteObj(stereol);
      buffer_number = !buffer_number;
   }
}
/*
  ======================================================================
 */
int dataval_callback (Dt32Bits data) 
{
   printf ("dataval_callback %d\n", data);
}
/*
  ======================================================================
 */
int dataptr_callback (DtPtr data)
{
   printf ("dataptr_callback\n");
   printf ("data = 0x%x\n", data);
   printf ("end dataval_callback\n");
}
/*
  ======================================================================
 */
int texture_switch_callback (void)
{
    DsExecuteObj(DoTextureMapDiffuseColorSwitch(textureswitch));
}
/*
  ======================================================================
 */
int texture_extend_callback (void)
{
    DsExecuteObj(DoTextureExtendUV(umode, vmode));
}
/*
  ======================================================================
 */
int texture_scale_callback (void)
{
    DsExecuteObj(DoTextureScaleUV(uscale, vscale));
}
/*
  ======================================================================
 */
int texture_translate_callback (void)
{
    DsExecuteObj(DoTextureTranslateUV(utranslate, vtranslate));
}
/*
  ======================================================================
 */
int texture_antialias_callback (void)
{
    DsExecuteObj(DoTextureAntiAlias(aa_mode));
}
/*
  ======================================================================
 */
int texture_operator_callback (void)
{
    DsExecuteObj(DoTextureOp(tex_op));
}
/*
  ======================================================================
 */
int texture_3d_extend_callback (void)
{
    DsExecuteObj(DoTextureExtendUVW(umode, vmode, wmode));
}
/*
  ======================================================================
 */
int texture_3d_scale_callback (void)
{
    DsExecuteObj(DoTextureScaleUVW(uscale, vscale, wscale));
}
/*
  ======================================================================
 */
int texture_3d_translate_callback (void)
{
    DsExecuteObj(DoTextureTranslateUVW(utranslate, vtranslate, wtranslate));
}
/*
  ======================================================================
 */
int create_callback_functions (void)
{
   DtObject datavalobject;
   DtInt i;
#ifdef OFFSET_OUTLINE
   extern set_offset();
   extern clr_offset();

   DsSetObjNameString
      (DsHoldObj(DoCallback(set_offset, DcNullObject)), "setoffset", DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(clr_offset, DcNullObject)), "clroffset",DcFalse);

   if (use_outline_offset)
	 fixup_device();
#endif

   DsSetObjNameString
      (DsHoldObj(DoCallback (super_sample_callback, DcNullObject)),
      "superswi", DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback (hidsurf_callback, DcNullObject)),
      "hidsurf", DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(error_callback, DcNullObject)), "error",DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(dataval_callback, DoDataVal(187))),
      "dataval",DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(dataptr_callback, DoDataPtr(DoPickID(1)))),
      "dataptr",DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(light_ambient_callback, DcNullObject)),
      "ambientlight",DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(light_infinite_callback, DcNullObject)),
      "infinitelight",DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(ambient_intensity_callback, DcNullObject)),
      "ambientintensity",DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(diffuse_intensity_callback, DcNullObject)),
      "diffuseintensity",DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(stereo_callback, DcNullObject)), "stereo",DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(backface_callback,DcNullObject)),
      "backface",DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(interptype_callback,DcNullObject)),
      "interptype",DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(reptype_callback,DcNullObject)), "reptype",DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(linetype_callback,DcNullObject)),
      "linetype",DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(specular_callback,DcNullObject)),
      "specular",DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(subdivspec_callback,DcNullObject)),
      "subdivspec",DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(depthcue_callback,DcNullObject)),
      "depthcue", DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(transp_callback,DcNullObject)), "transp", DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(localaa_callback,DcNullObject)), "localaa",DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(shader_callback,DcNullObject)), "shader", DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(stereo_test_callback,DcNullObject)),
      "teststereo", DcFalse);
    DsSetObjNameString
       (DsHoldObj(DoCallback(texture_switch_callback,DcNullObject)),
       "texture_switch", DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(texture_extend_callback,DcNullObject)),
      "texture_extend", DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(texture_scale_callback,DcNullObject)),
      "texture_scale", DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(texture_translate_callback,DcNullObject)),
      "texture_translate", DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(texture_3d_extend_callback,DcNullObject)),
      "texture_3d_extend", DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(texture_3d_scale_callback,DcNullObject)),
      "texture_3d_scale", DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(texture_3d_translate_callback,DcNullObject)),
      "texture_3d_translate", DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(texture_antialias_callback,DcNullObject)),
      "texture_antialias", DcFalse);
   DsSetObjNameString
      (DsHoldObj(DoCallback(texture_operator_callback,DcNullObject)),
      "texture_operator", DcFalse);

   for (i=0; i<9; i++) {
      datavalobject = DoDataVal((Dt32Bits)i);

      DsSetObjNameString
	 (DsHoldObj(DoCallback(rotate_callback,datavalobject)),
	 rotate_name[i],DcFalse);
      DsSetObjNameString
	 (DsHoldObj(DoCallback(scale_callback,datavalobject)),
	 scale_name[i],DcFalse);
      DsSetObjNameString
	 (DsHoldObj(DoCallback(trans_callback,datavalobject)),
	 trans_name[i],DcFalse);
   }

   reset_callbacks();
}
/*
  ======================================================================
 */
int process_callback (char controlchar)
/* NOTE:  the texture callbacks are processed in process_texture_callback */
{
    int retval = 0;
    extern DtInt step_translate, step_rotate, step_scale;

    /* return values:
       0 - ok 
       1 - match - full update
       2 - match - no update
     */

    switch (controlchar) {
    case '/':
	 retval = 1;
	/* super sampling is a filter technique aka "general antialiasing" on G3 */
	 superswi = !superswi;
	 printf ("Super Sample Switch is now %s\n",
		 superswi ? "on" : "off");
	 break;

      case 'z':
	 hidsrfswi = !hidsrfswi;
	 printf ("Hidden Surface is now %s\n", hidsrfswi ? "on" : "off");
	 break;

      case '1':
	 backfacecullswitch = !backfacecullswitch;
	 printf ("culling is now %s\n", backfacecullswitch ? "on" : "off");
	 break;

      case '2':
	 reptype++;
	 if (reptype > 5) reptype = 0;
	 printf ("representation type is now %s\n", 
		 reptype == 0 ? "Points" : 
		 reptype == 1 ? "Wireframe" :
		 reptype == 2 ? "Surfaces" : 
		 reptype == 3 ? "Outlines" :
		 reptype == 4 ? "SurfaceEdges" :
		 "SurfaceOutlineEdges" );
	 break;
					
      case '@':
	 reptype--;
	 if (reptype < 0) reptype = 5;
	 printf ("representation type is now %s\n", 
		 reptype == 0 ? "Points" : 
		 reptype == 1 ? "Wireframe" :
		 reptype == 2 ? "Surfaces" : 
		 reptype == 3 ? "Outlines" :
		 reptype == 4 ? "SurfaceEdges" :
		 "SurfaceOutlineEdges" );
	 break;
					
      case '3':
	 interptype++;
	 if (interptype > 2) interptype = 0;
	 printf ("interpolation type is now %s\n", 
		 interptype == 0 ? "ConstantShade" : 
		 interptype == 1 ? "VertexShade" : "SurfaceShade");
	 break;
      
      case '#':
	 interptype--;
	 if (interptype < 0) interptype = 2;
	 printf ("interpolation type is now %s\n", 
		 interptype == 0 ? "ConstantShade" : 
		 interptype == 1 ? "VertexShade" : "SurfaceShade");
	 break;
      
      case '4':
	 retval = 1;
	 specularswitch = !specularswitch;
	 printf ("specular switch is now %s\n", specularswitch ? "on" : "off");
	 break;

      case '7':
	 linetype++;
	 if (linetype > 3) linetype = 0;
	 printf ("line type is now %s\n", 
		 linetype == 0 ? "Solid" : 
		 linetype == 1 ? "Dash" :
		 linetype == 2 ? "Dot" : 
		 "Dot-Dash" );
	 break;

      case '&':
	 linetype--;
	 if (linetype < 0) linetype = 3;
	 printf ("line type is now %s\n", 
		 linetype == 0 ? "Solid" : 
		 linetype == 1 ? "Dash" :
		 linetype == 2 ? "Dot" : 
		 "Dot-Dash" );
	 break;
      case '8':
         transpswitch = !transpswitch;
         printf("transpswitch is now %s\n",transpswitch ? "on" : "off");
         break;
 
      case '9':
         localaaswitch = !localaaswitch;
         printf("localaaswitch is now %s\n",localaaswitch ? "on" : "off");
         break;

      case 'c':
	 depthcueswitch = !depthcueswitch;
	 printf("depthcueswitch is now %s\n",depthcueswitch ? "on" : "off");
	 break;

      case 'd':
	 retval = 1;
	 diffuseintensity += .1;
	 diffuseintensity = min(diffuseintensity,1.);
	 printf("diffuse intensity is now : %g\n", diffuseintensity);
	 break;

      case 'D':
	 retval = 1;
	 diffuseintensity -= .1;
	 diffuseintensity = max(diffuseintensity,0.);
	 printf("diffuse intensity is now : %g\n", diffuseintensity);
	 break;

      case 'a':
	 retval = 1;
	 ambientintensity += .1;
	 ambientintensity = min(ambientintensity,1.);
	 printf("ambient intensity is now : %g\n", ambientintensity);
	 break;

      case 'A':
	 retval = 1;
	 ambientintensity -= .1;
	 ambientintensity = max(ambientintensity,0.);
	 printf("ambient intensity is now : %g\n", ambientintensity);
	 break;

      case 'e':
	 retval = 1;
	 stereo_separation += .01;
	 printf ("stereo eye separation = %g\n", stereo_separation);
	 break;

      case 'E':
	 retval = 1;
	 stereo_separation -= .01;
	 printf ("stereo eye separation = %g\n", stereo_separation);
	 break;

      case 'f':
	 retval = 1;
	 stereo_distance += 1.0;
	 printf ("stereo distance = %g\n", stereo_distance);
	 break;

      case 'F':
	 retval = 1;
	 stereo_distance -= 1.0;
	 printf ("stereo distance = %g\n", stereo_distance);
	 break;

      case 'h':
	 constantshader = !constantshader;
	 printf("shader is now %s\n",constantshader ? "constant" : "lightsource");
	 break;

      case 'm':
	 scalevalue[geom_select][0] += (.1 * step_scale);
	 break;

      case 'M':
	 scalevalue[geom_select][0] -= (.1 * step_scale);
	 scalevalue[geom_select][0] = max(scalevalue[geom_select][0],.1);
	 break;

      case ',':
	 scalevalue[geom_select][1] += (.1 * step_scale);
	 break;

      case '<':
	 scalevalue[geom_select][1] -= (.1 * step_scale);
	 scalevalue[geom_select][1] = max(scalevalue[geom_select][1],.1);
	 break;

      case '.':
	 scalevalue[geom_select][2] += (.1 * step_scale);
	 break;

      case '>':
	 scalevalue[geom_select][2] -= (.1 * step_scale);
	 scalevalue[geom_select][2] = max(scalevalue[geom_select][2],.1);
	 break;

      case 'j':	rotatevalue[geom_select][0] += (.1 * step_rotate);	break;
      case 'J':	rotatevalue[geom_select][0] -= (.1 * step_rotate);	break;
      case 'k':	rotatevalue[geom_select][1] += (.1 * step_rotate);	break;
      case 'K':	rotatevalue[geom_select][1] -= (.1 * step_rotate);	break;
      case 'l':	rotatevalue[geom_select][2] += (.1 * step_rotate);	break;
      case 'L':	rotatevalue[geom_select][2] -= (.1 * step_rotate);	break;

      case 'u':	translatevalue[geom_select][0] += (.1 * step_translate);break;
      case 'U':	translatevalue[geom_select][0] -= (.1 * step_translate);break;
      case 'i':	translatevalue[geom_select][1] += (.1 * step_translate);break;
      case 'I':	translatevalue[geom_select][1] -= (.1 * step_translate);break;
      case 'o':	translatevalue[geom_select][2] += (.1 * step_translate);break;
      case 'O':	translatevalue[geom_select][2] -= (.1 * step_translate);break;

      case '=':
	 retval = 2;
	 geom_select++;
	 if (geom_select > geom_max) geom_select = 0;
	 if (geom_max > 0) 
	       printf ("callbacks %s %s %s now on control keys\n",
		       rotate_name[geom_select], scale_name[geom_select],
		       trans_name[geom_select]);
	 break;
      case '+':
	 retval = 2;
	 geom_select--;
	 if (geom_select < 0) geom_select = geom_max;
	 if (geom_max > 0)
	       printf ("callbacks %s %s %s now on control keys\n",
		       rotate_name[geom_select], scale_name[geom_select],
		       trans_name[geom_select]);
	 break;

      case 's':
	 subdivlevel += .1;
	 printf("Relative subdivision level is now %g\n", subdivlevel);
	 break;

      case 'S':
	 subdivlevel -= .1;
	 if (subdivlevel <= 0.0) {
             subdivlevel = 0.01;
         }
	 printf("Relative subdivision level is now %g\n", subdivlevel);
	 break;

     default:
	 retval = 0;
     } 

    return retval;
}
/*
  ======================================================================
 */
int process_texture_callback (char controlchar)
{
    int retval;

    /* return values:
       0 - ok 
       1 - match - full update
       2 - match - no update
     */

    switch (controlchar) {
        case 's':
            textureswitch = !textureswitch;
            printf("textureswitch is now %s\n",textureswitch ? "on" : "off");
            break;
	case 'm':
	    umode++;
	    if (umode > 3) umode = 0;
	    switch (umode) {
		case DcExtendNone:
		    printf("texture U extend mode is DcExtendNone\n");
		    break;
		case DcExtendBlack:
		    printf("texture U extend mode is DcExtendBlack\n");
		    break;
		case DcExtendClamp:
		    printf("texture U extend mode is DcExtendClamp\n");
		    break;
		case DcExtendRepeat:
		    printf("texture U extend mode is DcExtendRepeat\n");
		    break;
	    }
	    break;
	case 'M':
	    umode--;
	    if (umode < 0) umode = 3;
	    switch (umode) {
		case DcExtendNone:
		    printf("texture U extend mode is DcExtendNone\n");
		    break;
		case DcExtendBlack:
		    printf("texture U extend mode is DcExtendBlack\n");
		    break;
		case DcExtendClamp:
		    printf("texture U extend mode is DcExtendClamp\n");
		    break;
		case DcExtendRepeat:
		    printf("texture U extend mode is DcExtendRepeat\n");
		    break;
	    }
	    break;
	case ',':
	    vmode++;
	    if (vmode > 3) vmode = 0;
	    switch (vmode) {
		case DcExtendNone:
		    printf("texture V extend mode is DcExtendNone\n");
		    break;
		case DcExtendBlack:
		    printf("texture V extend mode is DcExtendBlack\n");
		    break;
		case DcExtendClamp:
		    printf("texture V extend mode is DcExtendClamp\n");
		    break;
		case DcExtendRepeat:
		    printf("texture V extend mode is DcExtendRepeat\n");
		    break;
	    }
	    break;
	case '<':
	    vmode--;
	    if (vmode < 0) vmode = 3;
	    switch (vmode) {
		case DcExtendNone:
		    printf("texture V extend mode is DcExtendNone\n");
		    break;
		case DcExtendBlack:
		    printf("texture V extend mode is DcExtendBlack\n");
		    break;
		case DcExtendClamp:
		    printf("texture V extend mode is DcExtendClamp\n");
		    break;
		case DcExtendRepeat:
		    printf("texture V extend mode is DcExtendRepeat\n");
		    break;
	    }
	    break;
	case '.':
	    wmode++;
	    if (wmode > 3) wmode = 0;
	    switch (wmode) {
		case DcExtendNone:
		    printf("texture W extend mode is DcExtendNone\n");
		    break;
		case DcExtendBlack:
		    printf("texture W extend mode is DcExtendBlack\n");
		    break;
		case DcExtendClamp:
		    printf("texture W extend mode is DcExtendClamp\n");
		    break;
		case DcExtendRepeat:
		    printf("texture W extend mode is DcExtendRepeat\n");
		    break;
	    }
	    break;
	case '>':
	    wmode--;
	    if (wmode < 0) wmode = 3;
	    switch (wmode) {
		case DcExtendNone:
		    printf("texture W extend mode is DcExtendNone\n");
		    break;
		case DcExtendBlack:
		    printf("texture W extend mode is DcExtendBlack\n");
		    break;
		case DcExtendClamp:
		    printf("texture W extend mode is DcExtendClamp\n");
		    break;
		case DcExtendRepeat:
		    printf("texture W extend mode is DcExtendRepeat\n");
		    break;
	    }
	    break;
	case 'h':
	    aa_mode++;
	    if (aa_mode > 8) aa_mode = 0;
	    switch (aa_mode) {
		case DcTextureAANone:
		    printf("texture AA mode is DcTextureAANone\n");
		    break;
		case DcTextureAAMIPPoint:
		    printf("texture AA mode is DcTextureAAMIPPoint\n");
		    break;
		case DcTextureAAAdaptive:
		    printf("texture AA mode is DcTextureAAAdaptive\n");
		    break;
		case DcTextureAABilinear:
		    printf("texture AA mode is DcTextureAABilinear\n");
		    break;
		case DcTextureAASumArea:
		    printf("texture AA mode is DcTextureAASumArea\n");
		    break;
		case DcTextureAATrilinear:
		    printf("texture AA mode is DcTextureAATrilinear\n");
		    break;
		case DcTextureAAMIP2Point:
		    printf("texture AA mode is DcTextureAAMIP2Point\n");
		    break;
		case DcTextureAAMIPBilinear:
		    printf("texture AA mode is DcTextureAAMIPBilinear\n");
		    break;
		case DcTextureAAMIP2Bilinear:
		    printf("texture AA mode is DcTextureAAMIP2Bilinear\n");
		    break;
	    }
	    break;
	case 'H':
	    aa_mode--;
	    if (aa_mode < 0) aa_mode = 8;
	    switch (aa_mode) {
		case DcTextureAANone:
		    printf("texture AA mode is DcTextureAANone\n");
		    break;
		case DcTextureAAMIPPoint:
		    printf("texture AA mode is DcTextureAAMIPPoint\n");
		    break;
		case DcTextureAAAdaptive:
		    printf("texture AA mode is DcTextureAAAdaptive\n");
		    break;
		case DcTextureAABilinear:
		    printf("texture AA mode is DcTextureAABilinear\n");
		    break;
		case DcTextureAASumArea:
		    printf("texture AA mode is DcTextureAASumArea\n");
		    break;
                case DcTextureAATrilinear:
                    printf("texture AA mode is DcTextureAATrilinear\n");
                    break;
                case DcTextureAAMIP2Point:
                    printf("texture AA mode is DcTextureAAMIP2Point\n");
                    break;
                case DcTextureAAMIPBilinear:
                    printf("texture AA mode is DcTextureAAMIPBilinear\n");
                    break;
                case DcTextureAAMIP2Bilinear:
                    printf("texture AA mode is DcTextureAAMIP2Bilinear\n");
                    break;
	    }
	    break;
	case 'n':
	    tex_op++;
	    if (tex_op > 3) tex_op = 0;
	    switch (tex_op) {
		case DcTextureReplace:
		    printf("texture operator is DcTextureReplace\n");
		    break;
		case DcTextureMultiply:
		    printf("texture operator is DcTextureMultiply\n");
		    break;
		case DcTextureBlend:
		    printf("texture operator is DcTextureBlend\n");
		    break;
		case DcTextureAdd:
		    printf("texture operator is DcTextureAdd\n");
		    break;
	    }
      	    break;
	case 'N':
	    tex_op--;
	    if (tex_op < 0) tex_op = 3;
	    switch (tex_op) {
		case DcTextureReplace:
		    printf("texture operator is DcTextureReplace\n");
		    break;
		case DcTextureMultiply:
		    printf("texture operator is DcTextureMultiply\n");
		    break;
		case DcTextureBlend:
		    printf("texture operator is DcTextureBlend\n");
		    break;
		case DcTextureAdd:
		    printf("texture operator is DcTextureAdd\n");
		    break;
	    }
      	    break;
	case 'u':
	    utranslate += 0.1;
	    utranslate = min(utranslate, 1.);
	    printf("texture U translate value is now %3.1f\n",utranslate);
	    break;
	case 'U':
	    utranslate -= 0.1;
	    utranslate = max(utranslate, -1.);
	    printf("texture U translate value is now %3.1f\n",utranslate);
	    break;
	case 'i':
	    vtranslate += 0.1;
	    vtranslate = min(vtranslate, 1.);
	    printf("texture V translate value is now %3.1f\n",vtranslate);
	    break;
	case 'I':
	    vtranslate -= 0.1;
	    vtranslate = max(vtranslate, -1.);
	    printf("texture V translate value is now %3.1f\n",vtranslate);
	    break;
	case 'o':
	    wtranslate += 0.1;
	    wtranslate = min(wtranslate, 1.);
	    printf("texture W translate value is now %3.1f\n",wtranslate);
	    break;
	case 'O':
	    wtranslate -= 0.1;
	    wtranslate = max(wtranslate, -1.);
	    printf("texture W translate value is now %3.1f\n",wtranslate);
	    break;
   	case 'j':
      	    uscale += 0.1;
      	    uscale = min(uscale, 2.);
      	    printf("texture U scale value is now %3.1f\n",uscale);
      	    break;
   	case 'J':
      	    uscale -= 0.1;
      	    uscale = max(uscale, 0.);
      	    printf("texture U scale value is now %3.1f\n",uscale);
      	    break;
   	case 'k':
      	    vscale += 0.1;
      	    vscale = min(vscale, 2.);
      	    printf("texture V scale value is now %3.1f\n",vscale);
      	    break;
   	case 'K':
      	    vscale -= 0.1;
      	    vscale = max(vscale, 0.);
      	    printf("texture V scale value is now %3.1f\n",vscale);
      	    break;
   	case 'l':
      	    wscale += 0.1;
      	    wscale = min(wscale, 2.);
      	    printf("texture W scale value is now %3.1f\n",wscale);
      	    break;
   	case 'L':
      	    wscale -= 0.1;
      	    wscale = max(wscale, 0.);
      	    printf("texture W scale value is now %3.1f\n",wscale);
      	    break;
	case '?':
	    print_texture_help();
	    break;
	default:
            printf ("character %c (0x%x) ignored\n", controlchar, controlchar);
    }
    return(retval = 0);
}
/*
  ======================================================================
 */
int read_values (void)
{
   extern DtInt shadetype;
   FILE *valuesfile;
   DtInt i;
   double x, y, z;

   if((valuesfile = fopen("values.dat", "r")) == NULL) {
      perror ("values.dat");
      return;
   }

   fscanf(valuesfile,"backfacecullswitch %d\n",&backfacecullswitch);
   fscanf(valuesfile,"interptype %d\n",&interptype);
   fscanf(valuesfile,"reptype %d\n",&reptype);
   fscanf(valuesfile,"linetype %d\n",&linetype);
   for (i=0; i<9; i++) {
      fscanf(valuesfile,"rotatevalue %lf %lf %lf\n", &x, &y, &z);
      rotatevalue[i][0] = x;
      rotatevalue[i][1] = y;
      rotatevalue[i][2] = z;
      fscanf(valuesfile,"scalevalue %lf %lf %lf\n", &x, &y, &z);
      scalevalue[i][0] = x;
      scalevalue[i][1] = y;
      scalevalue[i][2] = z;
      fscanf(valuesfile,"translatevalue %lf %lf %lf\n", &x, &y, &z);
      translatevalue[i][0] = x;
      translatevalue[i][1] = y;
      translatevalue[i][2] = z;
   }
   fscanf(valuesfile,"specularswitch %d\n",&specularswitch);

   fscanf(valuesfile,"subdivlevel %lf\n", &x); subdivlevel = x;
   fscanf(valuesfile,"shadetype %d\n", &shadetype);
   fscanf(valuesfile,"ambientintensity %lf\n", &x); ambientintensity = x;
   fscanf(valuesfile,"diffuseintensity %lf\n", &x); diffuseintensity = x;
   fscanf(valuesfile,"depthcueswitch %d\n", &depthcueswitch);
   fscanf(valuesfile,"transpswitch %d\n", &transpswitch);
   fscanf(valuesfile,"localaaswitch %d\n", &localaaswitch);
   fscanf(valuesfile,"stereo_separation %lf\n", &x); stereo_separation = x;
   fscanf(valuesfile,"stereo_distance %lf\n", &x); stereo_distance = x;

   fclose(valuesfile);
}
/*
  ======================================================================
 */
int write_values (void)
{
   extern DtInt shadetype;
   FILE *valuesfile;
   DtInt i;

   if ((valuesfile = fopen("values.dat", "w")) == NULL) {
      perror ("values.dat");
      return;
   }

   fprintf(valuesfile,"backfacecullswitch %d\n",backfacecullswitch);
   fprintf(valuesfile,"interptype %d\n",interptype);
   fprintf(valuesfile,"reptype %d\n",reptype);
   fprintf(valuesfile,"linetype %d\n",linetype);
   for (i=0; i<9; i++) {
      fprintf(valuesfile,"rotatevalue %lf %lf %lf\n",
	      rotatevalue[i][0],rotatevalue[i][1],rotatevalue[i][2]);
      fprintf(valuesfile,"scalevalue %lf %lf %lf\n",
	      scalevalue[i][0],scalevalue[i][1],scalevalue[i][2]);
      fprintf(valuesfile,"translatevalue %lf %lf %lf\n",
	      translatevalue[i][0],translatevalue[i][1],translatevalue[i][2]);
   }
   fprintf(valuesfile,"specularswitch %d\n",specularswitch);

   fprintf(valuesfile,"subdivlevel %lf\n", subdivlevel);
   fprintf(valuesfile,"shadetype %d\n", shadetype);
   fprintf(valuesfile,"ambientintensity %lf\n", ambientintensity);
   fprintf(valuesfile,"diffuseintensity %lf\n", diffuseintensity);
   fprintf(valuesfile,"depthcueswitch %d\n", depthcueswitch);
   fprintf(valuesfile,"transpswitch %d\n", transpswitch);
   fprintf(valuesfile,"localaaswitch %d\n", localaaswitch);
   fprintf(valuesfile,"stereo_separation %lf\n", stereo_separation);
   fprintf(valuesfile,"stereo_distance %lf\n", stereo_distance);

   fclose(valuesfile);
}
/*
  ======================================================================
 */
int print_texture_help (void)
{
   printf ("Texture Mapping Commands:\n");
   printf ("\t!u/!U   - U Translation\n");
   printf ("\t!i/!I   - V Translation\n");
   printf ("\t!o/!O   - W Translation\n");
   printf ("\t!j/!J   - U Scale\n");
   printf ("\t!k/!K   - V Scale\n");
   printf ("\t!l/!L   - W Scale\n");
   printf ("\t!m/!M   - cycle through U extend modes\n");
   printf ("\t!,/!<   - cycle through V extend modes\n");
   printf ("\t!./!>   - cycle through W extend modes\n");
   printf ("\t!h/!H   - cycle through texture antialias modes\n");
   printf ("\t!n/!N   - cycle through texture operators\n");
}
