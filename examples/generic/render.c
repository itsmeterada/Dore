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
	This file contains a generic Dore main routine which creates a single
	image.  It first initializes Dore and then calls the user supplied
	routine makeobjects() which is expected to create a studio group named
	"cameragroup" and a display group named "objectgroup". Next a
	reasonable Dore environment is created and a single image is displayed.
	It then waits for the user to hit the return key before exiting
	gracefully.
*/


#include <dore/dore.h>

int main (
    int argc,
    char *argv[])
{
	DtObject device, frame, view, camera_group, object_group;
	DtVolume volume;

	DsInitialize(0);

	/* Application specific call which creates camera and display groups */
	makeobjects(argc, argv);

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

	if (!(view = DoView())) {
		printf("can't create view\n");
		exit(1);
	}

	DgAddObjToGroup(DfInqViewGroup(frame), view);

	DvSetBoundary(view,&volume);

	/* find camera and display groups built in makeobjects() */
	camera_group = DsInqObjViaString("cameragroup",DcTypeGroup);
	object_group = DsInqObjViaString("objectgroup",DcTypeGroup);

	if((camera_group == DcNullObject) || (object_group == DcNullObject)) {
		printf("can't find camera or object group\n");
		exit(1);
	}

	DgAddObjToGroup(DvInqDisplayGroup(view), object_group);

	DgAddObjToGroup(DvInqDefinitionGroup(view), camera_group);

	DdUpdate(device); 

	getchar(); /* wait for user input */

	printf("Goodbye!\n");

	DsReleaseObj(device);

	DsTerminate();
}
