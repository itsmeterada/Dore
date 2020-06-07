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

int main (void)
{
	DtObject
		device, frame, 
		view1, view2, view3, view4,
		left_eye, right_eye, birds_eye, solar_system, light_group;

	static DtVolume 
		volume1 = {{0.0,0.0,0.0}, {0.5,0.5,1.0}},
		volume2 = {{0.0,0.5,0.0}, {0.5,1.0,1.0}},
		volume3 = {{0.5,0.0,0.0}, {1.0,0.5,1.0}},
		volume4 = {{0.5,0.5,0.0}, {1.0,1.0,1.0}};


	DsInitialize(0);

	makeobjects();

	device = DoDevice("stdx11", "");

	frame = DoFrame();

	DdSetFrame(device,frame);

	view1 = DoView();
	DgAddObjToGroup(DfInqViewGroup(frame), view1);
	DvSetBoundary(view1,&volume1);

	view2 = DoView();
	DgAddObjToGroup(DfInqViewGroup(frame), view2);
	DvSetBoundary(view2,&volume2);

	view3 = DoView();
	DgAddObjToGroup(DfInqViewGroup(frame), view3);
	DvSetBoundary(view3,&volume3);

	view4 = DoView();
	DgAddObjToGroup(DfInqViewGroup(frame), view4);
	DvSetBoundary(view4,&volume4);

	left_eye = DsInqObjViaString("left_eye",DcTypeGroup);
	right_eye = DsInqObjViaString("right_eye",DcTypeGroup);
	birds_eye = DsInqObjViaString("birds_eye",DcTypeGroup);
	light_group = DsInqObjViaString("lightgroup",DcTypeGroup);
	solar_system = DsInqObjViaString("solar_system",DcTypeGroup);
	if( ! (left_eye && right_eye && birds_eye && 
		light_group && solar_system) )
		{
		printf("couldn't find a group\n");
		exit(1);
		}

	DgAddObjToGroup(DvInqDisplayGroup(view1), solar_system);
	DgAddObjToGroup(DvInqDefinitionGroup(view1), left_eye);
	DgAddObjToGroup(DvInqDefinitionGroup(view1), light_group);

	DgAddObjToGroup(DvInqDisplayGroup(view2), solar_system);
	DgAddObjToGroup(DvInqDefinitionGroup(view2), solar_system);
	DgAddObjToGroup(DvInqDefinitionGroup(view2), light_group);

	DgAddObjToGroup(DvInqDisplayGroup(view3), solar_system);
	DgAddObjToGroup(DvInqDefinitionGroup(view3), right_eye);
	DgAddObjToGroup(DvInqDefinitionGroup(view3), light_group);

	DgAddObjToGroup(DvInqDisplayGroup(view4), solar_system);
	DgAddObjToGroup(DvInqDefinitionGroup(view4), birds_eye);
	DgAddObjToGroup(DvInqDefinitionGroup(view4), light_group);

	run_application(device);

	printf("Goodbye!\n");

	DsReleaseObj(device);

	DsTerminate();
}
