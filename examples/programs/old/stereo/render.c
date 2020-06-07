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



DtObject rotate_ingrp;

main()
{
	DtVolume        volume;
	DtObject        device, frame, view;
	DtObject        create_display_group(), create_camera_group();
	DtInt           i;
	DtReal          rotatevalue=0.;
	static DtReal   backgroundcolor[] = {.3,.3,.3};


	DsInitialize(0);

	/* create X11+ stereo window */
	if (!(device = DoDevice("display", "-geometry 512x256 -stereo"))) {
		printf("can't create stereo device\n");
		exit(1);
	}

	/* set up Dore device, frame, and view */
	DdInqExtent(device,&volume);
	frame = DoFrame();
	DdSetFrame(device,frame);
	DfSetBoundary(frame,&volume);
	view = DoView();
	DgAddObjToGroup(DfInqViewGroup(frame),view);
	DvSetBackgroundColor(view,DcRGB,backgroundcolor);
	DvSetBoundary(view,&volume);

	/* set Dore display group */
	DgAddObjToGroup(DvInqDisplayGroup(view), create_display_group());

	/* set Dore camera group including all Dore stereo calls*/
	DgAddObjToGroup(DvInqDefinitionGroup(view), create_camera_group());

	/* show the objects on the screen */
	DdUpdate(device);

	/* rotate the objects on the screen */
	printf("Hit return to rotate objects\n");
	getchar();
	DvSetUpdateType(view,DcUpdateDisplay);
	for (i=0; i<200; i++) {
		rotatevalue += .1;
		DgReplaceObjInGroup(rotate_ingrp,
			DoRotate(DcYAxis, rotatevalue));
		DdUpdate(device);
	}

	/* wait for user to indicate program exit */
	printf("Hit return to exit\n");
	getchar();

	/* Dore cleanup */
	DsReleaseObj(device);
	DsTerminate();
}







/*
  Create a Dore stereo camera group.
*/

DtObject
create_camera_group()
{
	static DtPoint3 at              = {0., 0., 0.};
	static DtPoint3 camerafrom      = {0., 5., 10.};
	static DtPoint3 lightfrom       = { 1., 1., 1.};
	static DtVector3 up             = {0., 1., 0.};

	DoGroup(DcTrue);
		DgAddObj(DoStereoSwitch(DcTrue)); /* turn on stereo */
		DgAddObj(DoStereo(0.05, 10.0)); /* set stereo attributes */
		DgAddObj(DoPerspective (30., -1., -100.));
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(at, camerafrom, up));
			DgAddObj(DoCamera());
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(at, lightfrom, up));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix());
	return(DgClose());
}







/*
  Creates a Dore display group.
*/

DtObject
create_display_group()
{
	static DtReal color1[3] = {1., 0., 0.};
	static DtReal color2[3] = {1., 1., 0.};

	DoGroup(DcTrue);
		DgAddObj(rotate_ingrp = DoInLineGroup(DcFalse));
		DgAddObj(DoInterpType(DcVertexShade));
		DgAddObj(DoDiffuseColor(DcRGB, color1));
		DgAddObj(DoPushMatrix());
			DgAddObj(DoRepType(DcWireframe));
			DgAddObj(DoTranslate (2.,0.,0.));
			DgAddObj(DoPrimSurf(DcSphere));
		DgAddObj(DoPopMatrix());
		DgAddObj(DoDiffuseColor(DcRGB, color2));
		DgAddObj(DoPushMatrix());
			DgAddObj(DoRepType(DcSurface));
			DgAddObj(DoTranslate (-2.,0.,0.));
			DgAddObj(DoPrimSurf(DcBox));
		DgAddObj(DoPopMatrix());
	return(DgClose());
}
