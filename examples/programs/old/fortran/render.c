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

#define DTOR(angle) ((angle) * (3.14159265358979 / 180.0))



int main (void)
{
	DtObject device, frame, view;
	DtObject post, base, def_group, obj_group;
	static DtPoint3
		origin     = { 0.0,  0.0,  0.0 },
		eye_point  = { 0.0,  0.0, 10.0 },
		light      = { 1.0,  1.0,  0.5 };
	static DtVector3
		up         = { 0.0,  1.0,  0.0 };
	static DtReal
		magenta[]  = { 1.0,  0.0,  1.0 },
		yellow[]   = { 0.8,  0.8,  0.0 },
		sky_blue[] = { 0.3,  0.3,  1.0 },
		sds[]      = { 0.8 };


	DsInitialize(0);
	device = DoDevice("display", "-singlebuffered");
	frame = DoFrame();
	DdSetFrame(device, frame);
	view = DoView();
	DvSetBackgroundColor(view, DcRGB, sky_blue);
	DgAddObjToGroup(DfInqViewGroup(frame), view);

	def_group = DoGroup(DcTrue);
		DgAddObj(DoParallel(10., -1., -20.));
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, eye_point, up));
			DgAddObj(DoCamera());
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, light, up));
			DgAddObj(DoLightIntens(1.0));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix());
	DgClose();

	post = DoGroup(DcTrue);
		DgAddObj(DoDiffuseColor(DcRGB, yellow));
		DgAddObj(DoSurfaceShade(DcShaderConstant));
		DgAddObj(DoRepType(DcWireframe));
		DgAddObj(DoRotate(DcXAxis, DTOR(90)));
		DgAddObj(DoScale(0.4, 0.4, 2.1));
		DgAddObj(DoPrimSurf(DcCylinder));
	DgClose();

	base = DoGroup(DcTrue);
		DgAddObj(DoTranslate(0.0, -3.0, 0.0));
		DgAddObj(DoScale(2.0, 2.0, 2.0));
		DgAddObj(DoTranslate(-0.5, -0.5, -0.5));
		DgAddObj(DoPrimSurf(DcBox));
	DgClose();

	obj_group = DoGroup(DcTrue);
		DgAddObj(DoRepType(DcSurface));
		DgAddObj(DoSubDivSpec(DcSubDivRelative, sds));
		DgAddObj(DoDiffuseColor(DcRGB, magenta));
		DgAddObj(post);
		DgAddObj(base);
		DgAddObj(DoPrimSurf(DcSphere));
	DgClose();

	DgAddObjToGroup(DvInqDefinitionGroup(view), def_group);
	DgAddObjToGroup(DvInqDisplayGroup(view), obj_group);

	DdUpdate(device);
	printf("Hit return to exit.\n");
	getchar();
	DsReleaseObj(device);
	DsTerminate();
}
