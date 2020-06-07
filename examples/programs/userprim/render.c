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
#include "lbracket.h"

int main (
    int argc,
    char *argv[])
{
	DtObject device, frame, view;
	DtObject obj_group, def_group;
	static DtPoint3
		origin       = { 0.0,  0.0,  0.0 },
		eye_point    = { 10.0,  10.0, 20.0 };
	static DtVector3 up  = { 0.0,  1.0,  0.0 };
	char devicetype[25];
	char argstring[75];
	int argno;
	static DtRealTriple red = { 1.0,  0.0,  0.0 };
	static DtRealTriple green = { 0.0,  1.0,  0.0 };

	DsInitializeSystem(0);

    strcpy(devicetype, "display");
    strcpy(argstring, "-geometry 512x512+1+1");
	argno = 1;
	while(argno < argc) {
		if (strcmp("-dt",argv[argno])==0) {
			argno++;
			strcpy(devicetype, argv[argno]);
			argno++;
		}
		else if (strcmp("-as",argv[argno])==0) {
			argno++;
			strcpy(argstring, argv[argno]);
			argno++;
		}
	}
	device = DoDevice(devicetype, argstring);

	frame = DoFrame();
	DdSetFrame(device, frame);
	view = DoView();
	DgAddObjToGroup(DfInqViewGroup(frame), view);

	def_group = DoGroup(DcTrue);
		DgAddObj(DoPerspective(30., -.1, -50.));
		DgAddObj(DoLookAtFrom(origin, eye_point, up));
		DgAddObj(DoCamera());
		DgAddObj(DoLight());
	DgClose();

	obj_group = DoGroup(DcTrue);
		DgAddObj(DoBackfaceCullSwitch(DcTrue));
		DgAddObj(DoBackfaceCullable(DcTrue));
		DgAddObj(DoPushMatrix());
			DgAddObj(DoRepType(DcSurface));
			DgAddObj(DoTranslate(-2., -2., -2.));
			DgAddObj(DoRotate(DcXAxis, 1.6));
			DgAddObj(DUoLbracket(3., 5., 2., .2));
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix());
			DgAddObj(DoRepType(DcSurface));
			DgAddObj(DoTranslate(-3., -2., -2.));
			DgAddObj(DoRotate(DcXAxis, -1.6));
			DgAddObj(DoRotate(DcYAxis, -1.6));
			DgAddObj(DoDiffuseColor(DcRGB, red));
			DgAddObj(DUoLbracket(3., 5., 2., .5));
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix());
			DgAddObj(DoRepType(DcWireframe));
			DgAddObj(DoTranslate(3., 3., 3.));
			DgAddObj(DoScale(.5, .5, .5));
			DgAddObj(DoDiffuseColor(DcRGB, red));
			DgAddObj(DUoLbracket(6., 3., 1., 1.));
		DgAddObj(DoPopMatrix());
	DgClose();

	DgAddObjToGroup(DvInqDefinitionGroup(view), def_group);
	DgAddObjToGroup(DvInqDisplayGroup(view), obj_group);

	DdUpdate(device);
	printf("Hit return to exit.\n");
	getchar();
	DsReleaseObj(device);
	DsTerminateSystem();
}

