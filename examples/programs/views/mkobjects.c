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
 
#include <dore/dore.h>
#include <dore/dore_util.h>


DtObject ingrp;
DtObject solar_sys;
DtObject sun;
DtObject earth;
DtObject moon;
DtObject earth_moon;


/* 
   Create the display and definition groups.
   makeobjects() expects to be called once near the beginning of the program.
*/

int makeobjects (void)
{
	static DtPoint3
		origin = {0.,0.,0.},
		camera_from = {0.,0.,8.}, /* positive Z  */
		from1 = {-0.2, 0.0, 8.0}, /* left eye */
		from2 = {0.2, 0.0, 8.0},  /* right eye */
		from3 = {0.0, 10.0, 0.0}, /* birds eye */
		light1 = { 1., 0.,  1.},
		light2 = { 1., 0., -1.},
		light3 = {-1., 0.,  1.},
		light4 = {-1., 0., -1.};
	static DtVector3
		y_dir = {0.,1.,0.},
		z_dir = {0.,0.,1.};
	static DtReal yellow[] = {.8,.8,.2};
	static DtReal green[] = {.2,.8,.2};
	static DtReal white[] = {.8,.8,.8};
	static DtReal red[] = {1.,.0,.0};
	static DtPoint3 nzdir = {0.,0.-1.};
	static DtPoint3 moonbase = {1.5,0.,0.};
	static DtVector3 x_axis = {1.0,0.0,0.0};
	static DtReal param[] = {0.75};
	static DtReal pole[] = 
		{0., -1.2, 0.,
		 0., 1.2, 0.};
	DtObject sphere_obj;



	DsSetObjNameString(DoGroup(DcTrue), "left_eye", DcFalse);
		DgAddObj(DoPerspective(45.,-1.,-30.));  
		DgAddObj(DoLookAtFrom(origin, from1, y_dir));
		DgAddObj(DoCamera());
	DgClose();


	DsSetObjNameString(DoGroup(DcTrue), "right_eye", DcFalse);
		DgAddObj(DoPerspective(45.,-1.,-30.));  
		DgAddObj(DoLookAtFrom(origin, from2, y_dir));
		DgAddObj(DoCamera());
	DgClose();


	DsSetObjNameString(DoGroup(DcTrue), "birds_eye", DcFalse);
		DgAddObj(DoPerspective(60.,-1.,-30.));  
		DgAddObj(DoLookAtFrom(origin, from3, z_dir));
		DgAddObj(DoCamera());
	DgClose();


	DsSetObjNameString(DoGroup(DcTrue), "lightgroup", DcFalse);
		DgAddObj(DoLightIntens(.7));
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, light1, y_dir));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, light2, y_dir));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, light3, y_dir));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, light4, y_dir));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix());
	DgClose();


	/* makes a basic planetoid object with poles */
	DoGroup(DcTrue);
		DgAddObj(DoRepType(DcSurface));
		DgAddObj(DoInterpType(DcVertexShade));
		DgAddObj(DoSubDivSpec(DcSubDivRelative, param));
		DgAddObj(DoPrimSurf(DcSphere));
		DgAddObj(DoLineList(DcRGB, DcLoc, 1, pole));
	sphere_obj = DgClose();


	/* moon object (Note: does not spin relative to earth) */
	DoGroup(DcTrue);
		DgAddObj(DoDiffuseColor(DcRGB, white));
		DgAddObj(DoScale(.25,.25,.25));
		DgAddObj(sphere_obj);
	moon = DgClose();


	/*
	Here is where we define the earth and fix a camera on it's surface.
	the camera is positioned at 47 degrees north latitude (over San
	Francisco) facing west.
	*/
	DoGroup(DcTrue);
		DgAddObj(DoDiffuseColor(DcRGB, green));
		DgAddObj(DoScale(.5,.5,.5));
		DgAddObj(DoLabel(0));   /* spin earth here */
		DgAddObj(sphere_obj);
		DgAddObj(DoRotate(DcZAxis, DTOR(90-38))); /* rotate to S.F.  */
		DgAddObj(DoRotate(DcXAxis, DTOR(-10))); /* look down a bit */
		DgAddObj(DoTranslate(0., 1.1, 0.));    /* over north pole */
		DgAddObj(DoPerspective(60.,-.1,-30.)); 
		DgAddObj(DoCamera());
		DgAddObj(DoScale(.1,.1,.1));
		DgAddObj(DoDiffuseColor(DcRGB, red));
		DgAddObj(DoPrimSurf(DcBox));
		DgSetElePtr(0, DcBeginning);
		DgSetElePtrRelLabel(0, 0);
	earth = DgClose();


	/* earth/moon system */
	DoGroup(DcTrue);
		DgAddObj(DoPushMatrix());
			DgAddObj(DoRotate(DcXAxis,DTOR(-23))); /* earth tilt */
			DgAddObj(earth);
		DgAddObj(DoPopMatrix());
		DgAddObj(DoLabel(0));   /* moon orbits here */
		DgAddObj(DoTranslate(1.,0.,0.));
		DgAddObj(moon);
		DgSetElePtr(0, DcBeginning);
		DgSetElePtrRelLabel(0, 0);
	earth_moon = DgClose();


	/* defines the sun object (Note: doesn't spin) */
	DoGroup(DcTrue);
		DgAddObj(DoDiffuseColor(DcRGB, yellow));
		DgAddObj(sphere_obj);
	sun = DgClose();


	/* makes the solar system model */
	DsSetObjNameString(DoGroup(DcTrue), "solar_system", DcFalse);
		DgAddObj(sun);
		DgAddObj(DoLabel(0));  /* earth/moon system orbits sun here */
		DgAddObj(DoTranslate(3.,0.,0.));
		DgAddObj(earth_moon);
		DgSetElePtr(0, DcBeginning);
		DgSetElePtrRelLabel(0, 0);
	solar_sys = DgClose();
}



#define FRAMES_PER_YEAR		200.0
#define FRAMES_PER_MONTH	100.0
#define FRAMES_PER_DAY		40.0

#define DEG_YEARS_PER_FRAME	(360. / FRAMES_PER_YEAR)
#define DEG_MONTHS_PER_FRAME	(360. / FRAMES_PER_MONTH)
#define DEG_DAYS_PER_FRAME	(360. / FRAMES_PER_DAY)

int run_application (
    DtObject device)
{
	DtReal earth_rotate=0, moon_orbit=0, em_orbit=0;

	printf("Running solar system example. Hit <control c> to exit.\n");
	while(1)
	{
		DdUpdate(device);

		em_orbit     += DEG_YEARS_PER_FRAME;
		moon_orbit   += DEG_MONTHS_PER_FRAME;
		earth_rotate += DEG_DAYS_PER_FRAME;
	  
		DgReplaceObjInGroup(earth_moon,
			DoRotate(DcYAxis, DTOR(moon_orbit)));
		DgReplaceObjInGroup(solar_sys,
			DoRotate(DcYAxis, DTOR(em_orbit)));
		DgReplaceObjInGroup(earth,
			DoRotate(DcYAxis, DTOR(earth_rotate)));
	}
}

