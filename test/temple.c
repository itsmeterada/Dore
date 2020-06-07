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

	pro_temple
	dyn_temple

    The display group is many different primitive surfaces, including a
    cylinder with spheres inside.  The cylinder can be made transparent
    on some platforms.
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
int shadow_callback();
int reflection_callback();
int transp_callback();
int transp_intensity_callback();

/*
  ======================================================================
 */
DtObject makeobject()
{
   DtObject pillar;
   DtObject exhibit;
   DtObject temple;
   DtObject object;
   DtReal	color[3];

   pillar = DoGroup(DcTrue);

	DgAddObj( DoTranspSwitch(DcOff));

	DgAddObj( DoDiffuseIntens(.8) );

	DgAddObj( DoSpecularIntens(.5) );
	color[0] = 1.;
	color[1] = .8;
	color[2] = 1.;
	DgAddObj( DoSpecularColor(DcRGB, color) );
	DgAddObj( DoSpecularFactor(150.) );

	DgAddObj( DoPushMatrix() );
		DgAddObj( DoRotate(0,-1.57) );
		DgAddObj( DoScale(.2,.2,3.) );
		DgAddObj( DoTranslate(0.,0.,-.5) );
		DgAddObj( DoPrimSurf(DcCylinder) );
	DgAddObj( DoPopMatrix() );

   DgClose();

   exhibit = DoGroup(DcTrue);

	/*  Glass cylinder.  */

	DgAddObj( DoShadowSwitch(DcOff) );
	DgAddObj( DoReflectionSwitch(DcOff) );

	DgAddObj( DoDiffuseIntens(.8) );
	color[0] = 1.;
	color[1] = 1.;
	color[2] = 1.;
	DgAddObj( DoDiffuseColor(DcRGB, color) );
	DgAddObj( DoSpecularIntens(.5) );
	color[0] = 1.;
	color[1] = 1.;
	color[2] = 1.;
	DgAddObj( DoSpecularColor(DcRGB, color) );
	DgAddObj( DoSpecularFactor(160.) );

	DgAddObj( DoCallback(transp_intensity_callback, DcNullObject) );
	color[0] = 1.;
	color[1] = 1.;
	color[2] = 1.;
	DgAddObj( DoTranspColor(DcRGB, color) );
	DgAddObj( DoCallback(transp_callback, DcNullObject) );

	DgAddObj( DoPushMatrix() );
		DgAddObj( DoRotate(0,1.57) );
		DgAddObj( DoScale(.55,.55,3.) );
		DgAddObj( DoTranslate(0.,0.,-.5) );
		DgAddObj( DoPrimSurf(DcCylinder) );
	DgAddObj( DoPopMatrix() );

	DgAddObj( DoDiffuseIntens(.5) );
	color[0] = 1.;
	color[1] = 0.;
	color[2] = 1.;
	DgAddObj( DoDiffuseColor(DcRGB, color) );

	DgAddObj( DoSpecularIntens(.5) );
	color[0] = 1.;
	color[1] = 0.;
	color[2] = 1.;
	DgAddObj( DoSpecularColor(DcRGB, color) );
	DgAddObj( DoSpecularFactor(150.) );

	DgAddObj( DoTranspSwitch(DcOff) );

	DgAddObj( DoPushMatrix() );
		DgAddObj( DoTranslate(0.,-1.,0.) );
		DgAddObj( DoScale(.5,.5,.5) );
		DgAddObj( DoPrimSurf(DcSphere) );
	DgAddObj( DoPopMatrix() );

	DgAddObj( DoDiffuseIntens(.5) );
	color[0] = 0.;
	color[1] = 1.;
	color[2] = 0.;
	DgAddObj( DoDiffuseColor(DcRGB, color) );

	DgAddObj( DoSpecularIntens(.5) );
	color[0] = 0.;
	color[1] = 1.;
	color[2] = 0.;
	DgAddObj( DoSpecularColor(DcRGB, color) );
	DgAddObj( DoSpecularFactor(150.) );

	DgAddObj( DoPushMatrix() );
		DgAddObj( DoScale(.5,.5,.5) );
		DgAddObj( DoPrimSurf(DcSphere) );
	DgAddObj( DoPopMatrix() );

	DgAddObj( DoDiffuseIntens(.5) );
	color[0] = 1.;
	color[1] = .0;
	color[2] = .3;
	DgAddObj( DoDiffuseColor(DcRGB, color) );

	DgAddObj( DoSpecularIntens(.5) );
	color[0] = 1.;
	color[1] = .0;
	color[2] = .3;
	DgAddObj( DoSpecularColor(DcRGB, color) );
	DgAddObj( DoSpecularFactor(150.) );

	DgAddObj( DoPushMatrix() );
		DgAddObj( DoTranslate(0.,1.,0.) );
		DgAddObj( DoScale(.45,.45,.45) );
		DgAddObj( DoPrimSurf(DcSphere) );
	DgAddObj( DoPopMatrix() );
   DgClose();

   temple = DoGroup(DcTrue);

	DgAddObj( DoRotate(1,.3) );
	DgAddObj( DoScale(.8,.8,.8) );

	DgAddObj( DoDiffuseIntens(.7) );
	color[0] = 1.;
	color[1] = .8;
	color[2] = 1.;
	DgAddObj( DoDiffuseColor(DcRGB, color) );

	DgAddObj( DoSpecularIntens(.5) );
	color[0] = 1.;
	color[1] = .8;
	color[2] = 1.;
	DgAddObj( DoSpecularColor(DcRGB, color) );
	DgAddObj( DoSpecularFactor(150.) );

	DgAddObj( DoPushMatrix() );
		DgAddObj( DoTranslate(0.,1.5,0.) );
		DgAddObj( DoScale(3.,.3,3.) );
		DgAddObj( DoTranslate(-.5,-.5,-.5) );
		DgAddObj( DoPrimSurf(DcBox) );
	DgAddObj( DoPopMatrix() );

	DgAddObj( DoPushMatrix() );
		DgAddObj( DoTranslate(-1.,0.,-1.) );
		color[0] = 1.;
		color[1] = 0.;
		color[2] = 0.;
		DgAddObj( DoDiffuseColor(DcRGB, color) );
		DgAddObj ( pillar );
	DgAddObj( DoPopMatrix() );

	DgAddObj( DoPushMatrix() );
		DgAddObj( DoTranslate(1.,0.,-1.) );
		color[0] = 0.;
		color[1] = 1.;
		color[2] = 0.;
		DgAddObj( DoDiffuseColor(DcRGB, color) );
		DgAddObj ( pillar );
	DgAddObj( DoPopMatrix() );

	DgAddObj( DoPushMatrix() );
		DgAddObj( DoTranslate(1.,0.,1.) );
		color[0] = 0.;
		color[1] = 0.;
		color[2] = 1.;
		DgAddObj( DoDiffuseColor(DcRGB, color) );
		DgAddObj ( pillar );
	DgAddObj( DoPopMatrix() );

	DgAddObj( DoPushMatrix() );
	DgAddObj( DoTranslate(-1.,0.,1.) );
		color[0] = 1.;
		color[1] = 1.;
		color[2] = 1.;
		DgAddObj( DoDiffuseColor(DcRGB, color) );
		DgAddObj ( pillar );
	DgAddObj( DoPopMatrix() );

	DgAddObj( DoPushMatrix() );
		DgAddObj( DoRotate(2,.45) );
		DgAddObj( DoScale(.7,.7,.7) );
		DgAddObj ( exhibit );
	DgAddObj( DoPopMatrix() );

	DgAddObj( DoDiffuseIntens(.1) );
	color[0] = .9;
	color[1] = .9;
	color[2] = 1.;
	DgAddObj( DoDiffuseColor(DcRGB, color) );

	DgAddObj( DoSpecularIntens(.8) );
	color[0] = .9;
	color[1] = .9;
	color[2] = 1.;
	DgAddObj( DoSpecularColor(DcRGB, color) );
	DgAddObj( DoSpecularFactor(150.) );

	DgAddObj( DoDiffuseIntens(.7) );
	color[0] = 1.;
	color[1] = .8;
	color[2] = 1.;
	DgAddObj( DoDiffuseColor(DcRGB, color) );

	DgAddObj( DoSpecularIntens(.5) );
	color[0] = 1.;
	color[1] = .8;
	color[2] = 1.;
	DgAddObj( DoSpecularColor(DcRGB, color) );
	DgAddObj( DoSpecularFactor(150.) );

	DgAddObj( DoPushMatrix() );
	DgAddObj( DoTranslate(0.,-1.5,0.) );
	DgAddObj( DoScale(3.,.3,3.) );
	DgAddObj( DoTranslate(-.5,-.5,-.5) );
	DgAddObj( DoPrimSurf(2) );
	DgAddObj( DoPopMatrix() );

   DgClose();

   object = DoGroup(DcTrue);

	DgAddObj(DoPickSwitch(DcOn));

	DgAddObj(DoCallback(backface_callback, DcNullObject));
	DgAddObj(DoBackfaceCullable(DcOn));

	DgAddObj(DoCallback(reptype_callback, DcNullObject));
	DgAddObj(DoCallback(interptype_callback, DcNullObject));

	DgAddObj(DoCallback(specular_callback, DcNullObject));

	DgAddObj(DoCallback(translate_callback, DcNullObject));

	DgAddObj(DoCallback(rotate_callback, DcNullObject));

	DgAddObj(DoCallback(scale_callback, DcNullObject));

	DgAddObj(DoCallback(subdivspec_callback, DcNullObject));

	DgAddObj(DoCallback(shadow_callback, DcNullObject));

	DgAddObj(DoCallback(reflection_callback, DcNullObject));

	DgAddObj( DoRotate(DcXAxis,2.6) );

	DgAddObj ( temple );

   DgClose();

   return object;
}
/*
  ======================================================================
 */
DtObject makestudio()
{
   DtObject studio;
   static DtPoint3	at		= {0., 0., 0.};
   static DtPoint3	camfrom		= {0., 6., 8.};
   static DtPoint3	lightfrom	= {1., 1., 1.};
   static DtVector3	up		= {0., 1., 0.};
   static DtReal	white[3]	= {1., 1., 1.};

   studio = DoGroup(DcTrue);

	DgAddObj(DoPerspective(30.,-1.,-100.));

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
