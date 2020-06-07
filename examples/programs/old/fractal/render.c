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

main(argc,argv)
int argc;
char *argv[];
{
	DtObject device, frame, view;
	DtObject camera_group, object_group;
	DtVolume volume;
	static DtReal skyblue[] = {.5,.5,.8};


	DsInitialize(0);

	makeobjects(argc, argv);

	prsarg_check_bad_arguments(argc,argv);

	if(inq_expansion_board())
		device = DoDevice("stdx11", "");
	else
		device = DoDevice("stdx11", "-singlebuffered");
	if( ! device) {
		printf("can't create device\n");
		exit(1);
	}

	DdInqExtent(device,&volume);

	if (!(frame = DoFrame()))
		{
		printf("can't create frame\n");
		exit(1);
		}

	DdSetFrame(device,frame);

	DfSetBoundary(frame,&volume);

	if (!(view = DoView()))
		{
		printf("can't create view\n");
		exit(1);
		}

	DvSetBackgroundColor(view, DcRGB, skyblue);

	DgAddObjToGroup(DfInqViewGroup(frame), view);

	DvSetBoundary(view,&volume);

	camera_group = DsInqObj(DcNameString,"cameragroup",DcTypeGroup);
	object_group = DsInqObj(DcNameString,"objectgroup",DcTypeGroup);

	if((camera_group == DcNullObject) || (object_group == DcNullObject))
		{
		printf("can't find camera or object group\n");
		exit(1);
		}

	DgAddObjToGroup(DvInqDisplayGroup(view), object_group);

	DgAddObjToGroup(DvInqDefinitionGroup(view), camera_group);

	DdUpdate(device,DcFalse); 

	run_application(device);

	DsReleaseObj(device);

	DsTerminate();
}
