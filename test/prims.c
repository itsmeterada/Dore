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
    This file contains the display and studio objects for the executables:

	pro_prims
	dyn_prims

    The display group consists of a sphere, cylinder, box, and torus.
    ====================================================================== */

#include <dore/dore.h>

int light_ambient_callback();
int light_infinite_callback();
int ambient_intensity_callback();
int diffuse_intensity_callback();
int interptype_callback();
int reptype_callback();
int specular_callback();
int backface_callback();
int rotate_callback();
int scale_callback();
int subdivspec_callback();
int translate_callback();
int depthcue_callback();
extern DtVisualType visualtype;

/*
  ======================================================================
 */
DtObject makeobject()
{
   DtObject object;
   DtReal color[4];

   object = DoGroup(DcTrue);
   
	DgAddObj(DoPickSwitch(DcOn));

	DgAddObj(DoCallback(backface_callback, DcNullObject));
	DgAddObj(DoBackfaceCullable(DcOn));

	DgAddObj(DoCallback(reptype_callback, DcNullObject));
	DgAddObj(DoCallback(interptype_callback, DcNullObject));

	DgAddObj(DoDiffuseIntens(1.));
	DgAddObj(DoSpecularIntens(1.));

	DgAddObj(DoCallback(specular_callback, DcNullObject));

	DgAddObj(DoCallback(translate_callback, DcNullObject));

	DgAddObj(DoCallback(rotate_callback, DcNullObject));

	DgAddObj(DoCallback(scale_callback, DcNullObject));

	DgAddObj(DoCallback(subdivspec_callback, DcNullObject));

	DgAddObj(DoPickID (1));
	if (visualtype == DcPseudoColor) {
	   DgAddObj(DoShadeIndex(7));
	} else {
	   color[0] = 1.0;
	   color[1] = .75;
	   color[2] = 0.5;
	   DgAddObj(DoDiffuseColor(DcRGB, color));
	}
	DgAddObj(DoPushMatrix());
		DgAddObj(DoTranslate (2.,0.,0.));
		DgAddObj(DoPrimSurf(DcSphere));	
	DgAddObj(DoPopMatrix());

	DgAddObj(DoPickID (2));
	if (visualtype == DcPseudoColor) {
	   DgAddObj(DoShadeIndex(3));
	} else {
	   color[0] = 1.0;
	   color[1] = 0.0;
	   color[2] = 0.0;
	   DgAddObj(DoDiffuseColor(DcRGB, color));
	}
	DgAddObj(DoPushMatrix());
		DgAddObj(DoTranslate (0.,2.,0.));
		DgAddObj(DoRotate(0, .785));
		DgAddObj(DoRotate(1, .785));
		DgAddObj(DoScale (.5,.5,2.));
		DgAddObj(DoPrimSurf(DcCylinder));	
	DgAddObj(DoPopMatrix());

	DgAddObj(DoPickID (3));
	if (visualtype == DcPseudoColor) {
	   DgAddObj(DoShadeIndex(1));
	} else {
	   color[0] = 0.0;
	   color[1] = 1.0;
	   color[2] = 0.0;
	   DgAddObj(DoDiffuseColor(DcRGB, color));
	}
	DgAddObj(DoPushMatrix());
		DgAddObj(DoTranslate (-2.,0.,0.));
		DgAddObj(DoPrimSurf(DcBox));
	DgAddObj(DoPopMatrix());

	DgAddObj(DoPickID (4));
	if (visualtype == DcPseudoColor) {
	   DgAddObj(DoShadeIndex(4));
	} else {
	   color[0] = 1.0;
	   color[1] = 0.0;
	   color[2] = 1.0;
	   DgAddObj(DoDiffuseColor(DcRGB, color));
	}
	DgAddObj(DoPushMatrix());
		DgAddObj(DoTranslate (0.,-2.,0.));
		DgAddObj(DoTorus(1.0,0.5));
	DgAddObj(DoPopMatrix());
 
   DgClose();

   return(object);
}
/*
  ======================================================================
 */
DtObject makestudio()
{
   DtObject studio;
   static DtPoint3	at		= {0., 0., 0.};
   static DtPoint3	camfrom		= {0., 5., 10.};
   static DtPoint3	lightfrom	= {1., 1., 1.};
   static DtVector3	up		= {0., 1., 0.};
   static DtReal	white[3]	= {1., 1., 1.};


   studio = DoGroup(DcTrue);

	DgAddObj(DoPerspective(30., -1.0, -100.0));

        DgAddObj(DoGlbRendMaxObjs(4));
        DgAddObj(DoGlbRendMaxSub(150));

	DgAddObj(DoPushMatrix());
		DgAddObj(DoLookAtFrom(at, camfrom, up));
		DgAddObj(DoCamera());
	DgAddObj(DoPopMatrix());

	DgAddObj(DoLightColor(DcRGB, white));
	DgAddObj(DoCallback(ambient_intensity_callback, DcNullObject));
	DgAddObj(DoCallback(light_ambient_callback, DcNullObject));
	DgAddObj(DoLight());

	DgAddObj(DoPushMatrix());
		DgAddObj(DoLookAtFrom(at, lightfrom, up));
		DgAddObj(DoLightColor(DcRGB, white));
		DgAddObj(DoCallback(diffuse_intensity_callback, DcNullObject));
		DgAddObj(DoCallback(light_infinite_callback, DcNullObject));
		DgAddObj(DoLight());
	DgAddObj(DoPopMatrix());

   DgClose();

   return(studio);
}
/*
  ======================================================================
 */
