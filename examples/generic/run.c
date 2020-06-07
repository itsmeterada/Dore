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
 
/*
	This file contains a generic Dore main routine capable of generating
	dynamic sequences.
	It first initializes Dore and then calls the user supplied routine
	makeobjects() which is expected to create a studio group named
	"cameragroup" and a display group named "objectgroup".  Next a
	utility routine is executed to determine if the TITAN has
	a graphics expansion board, and an appropriate Dore device
	is created. It then sets up the rest of a reasonable Dore
	environment and calls out to another user supplied routine
	called run_application() which can generate dynamic sequences
	with repeated calls to DdUpdate, alternating with modifications
	to the camera or display groups.
*/


#include <dore/dore.h>

int run_application (DtObject);

int main (
    int argc,
    char *argv[])
{
	DtObject device, frame, view, camera_group, object_group;
	DtVolume volume;

	DsInitialize(0);

	/* Application specific call which creates camera and display groups */
	makeobjects(argc, argv);

	if (!(view = DoView())) {
		printf("can't create view\n");
		exit(1);
	}

	/* find camera and display groups built in makeobjects() */
	camera_group = DsInqObjViaString("cameragroup",DcTypeGroup);
	object_group = DsInqObjViaString("objectgroup",DcTypeGroup);

	if((camera_group == DcNullObject) || (object_group == DcNullObject)) {
		printf("can't find camera or object group\n");
		exit(1);
	}

	DgAddObjToGroup(DvInqDisplayGroup(view), object_group);

	DgAddObjToGroup(DvInqDefinitionGroup(view), camera_group);


	/* Uncomment the following line to use the previewer. */
	/* The previewer is a useful debugging utility.      */
	/* view_preview(view, 0, DcNullPtr); */

	device = DoDevice("stdx11", "-geometry 640x512");

	if( ! device) {
		printf("can't create device\n");
		exit(1);
	}

	DdInqExtent(device,&volume);

	if (!(frame = DoFrame())) {
		printf("can't create frame\n");
		exit(1);
	}

	DdSetFrame(device,frame);

	DfSetBoundary(frame,&volume);

	DgAddObjToGroup(DfInqViewGroup(frame), view);

	DvSetBoundary(view,&volume);

	/* Application specific call which does dynamic updates */
	run_application(view);

	printf("Goodbye!\n");

	DsReleaseObj(device);

	DsTerminate();
}
