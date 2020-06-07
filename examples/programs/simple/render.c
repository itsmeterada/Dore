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

int main (void)
{
	DtObject device, frame, view, studio_group, object_group;
	static DtColorRGB red_col = {1.0, 0.0, 0.0};

	DsInitialize(0);
	device = DoDevice("stdx11", "");
	frame = DoFrame();
	DdSetFrame(device, frame);
	view = DoView();
	DgAddObjToGroup(DfInqViewGroup(frame), view);

	studio_group = DoGroup(DcTrue);
		DgAddObj(DoTranslate(0.0, 0.0, 4.0));
		DgAddObj(DoPerspective(40.0, -0.1, -5.0));
		DgAddObj(DoCamera());
		DgAddObj(DoLight());
	DgClose();

	object_group = DoGroup(DcTrue);
		DgAddObj(DoRepType(DcSurface));
		DgAddObj(DoDiffuseColor(DcRGB, red_col));
		DgAddObj(DoPrimSurf(DcSphere));
	DgClose();

	DgAddObjToGroup(DvInqDefinitionGroup(view), studio_group);
	DgAddObjToGroup(DvInqDisplayGroup(view), object_group);

	DdUpdate(device);
	printf("Hit return to exit.\n");
	getchar();

	DsTerminate();
}
