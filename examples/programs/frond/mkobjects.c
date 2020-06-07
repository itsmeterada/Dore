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
#include <math.h>

int depth, go=0;
DtReal a, b, c;
DtObject ingrp, segment;

/* prototype declarations */
int segment_callback  (void);
int makeobjects  (void);
DtObject make_camera_group  (void);
int run_application  (DtObject);


int segment_callback (void)
{
	if(depth-- && go) {
		DsExecuteObj(DoRotate(DcXAxis, a));
		DsExecuteObj(DoRotate(DcYAxis, b));
		DsExecuteObj(DoRotate(DcZAxis, c));
		DsExecuteObj(segment);
	}
}


int makeobjects (void)
{
	static DtReal sds[1]; /* sub division spec */
	static DtColorRGB cyan = {0.0, 1.0, 1.0};
	extern DtObject make_camera_group();

	sds[0] = .05;
	segment = DoGroup(DcTrue);
		DgAddObj(DoDiffuseColor(DcRGB, cyan));
		DgAddObj(DoScale(.9,.9,.9));
		DgAddObj(DoTranslate(0.,0.,-1.));
		DgAddObj(DoPushMatrix());
			DgAddObj(DoScale(.5,.5,1.));
			DgAddObj(DoPrimSurf(DcCylinder));
		DgAddObj(DoPopMatrix());
		DgAddObj(DoCallback(segment_callback, DcNullPtr));
	DgClose();

	DoGroup(DcTrue);
		DgAddObj(DoSubDivSpec(DcSubDivRelative, sds));
		DgAddObj(DoRepType(DcSurface));
		DgAddObj(DoInterpType(DcVertexShade));
		DgAddObj(DoTranslate(0.,-3.,0.));
		DgAddObj(DoRotate(DcXAxis, DTOR(90)));
		DgAddObj(segment);
	DsSetObjNameString(DgClose(), "objectgroup",DcFalse);

	DsSetObjNameString(make_camera_group(), "cameragroup",DcFalse);
}





DtObject make_camera_group (void)
{
	static DtPoint3
		origin = {0.,0.,0.},
		camera_from = {0.,0.,10.}, /* positive Z  */
		light0 = {0.,-8.,-2.},
		light1 = {8.,8.,8.};
	static DtVector3 up = {0.,1.,0.};

	DsSetObjNameString(DoGroup(DcTrue), "cameragroup", DcFalse);
		DgAddObj(DoPerspective(60.,-1.,-20.));  
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, camera_from, up));
			DgAddObj(DoCamera());
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, light0, up));
			DgAddObj(DoLightIntens(.7));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, light1, up));
			DgAddObj(DoLightIntens(.9));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix());
	return(DgClose());
}


int run_application (
    DtObject view)
{
	int time=200;

	DvUpdate(view);
	DvSetUpdateType(view, DcUpdateDisplay);
	go = 1;
	while(time--) {
		depth = 12;
		a= .3 * sin(.200000*time);
		b= .5 * sin(.221234*time);
		c= .7 * sin(.244098*time);
		DvUpdate(view);
	}
	printf("Hit return to exit.\n");
        getchar();
}
