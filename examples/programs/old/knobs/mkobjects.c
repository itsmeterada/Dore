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
#include <stdio.h>

DtObject rotate_ingrp, translate_ingrp, view_handle;


/* 
   Create the display and definition groups.
   makeobjects() expects to be called once near the beginning of the program.
*/

makeobjects()
{
	static DtPoint3
		origin = {0.,0.,0.},
		camera_from = {0.,0.,2.}, /* positive Z  */
		light0 = {-1.,1.,1.},     /* upper left  */
		light1 = {1.,1.,1.};      /* upper right */
	static DtVector3 up = {0.,1.,0.};
	static DtReal yellow[] = {1.,1.,0.};
	static DtReal sds[] = {.03};
	static DtReal ctlpts[] =
		{
		-.3,.3,0.,	0.,-.3,-.3,	0.,0.,-1.,	1.,0.,0.,
		0.,-.3,.3,	.3,.3,0.,	1.,0.,0.,	0.,0.,-1.,
		0.,0.,1.,	1.,0.,0.,	0.,0.,0.,	0.,0.,0.,
		1.,0.,0.,	0.,0.,1.,	0.,0.,0.,	0.,0.,0.,
		};

	rotate_ingrp = DoInLineGroup(DcFalse);
	translate_ingrp = DoInLineGroup(DcFalse);

	DsSetObjName(DoGroup(DcTrue), DcNameString, "cameragroup", DcFalse);
		DgAddObj(DoPerspective(60.,-1.,-400.));  
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
			DgAddObj(DoLightIntens(.7));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix());
	DgClose();

	DsSetObjName(DoGroup(DcTrue), DcNameString, "objectgroup", DcFalse);
		DgAddObj(DoDiffuseColor(DcRGB, yellow));
		DgAddObj(DoInterpType(DcVertexShade));
		DgAddObj(DoRotate(DcYAxis, DTOR(-20)));
		DgAddObj(DoRotate(DcXAxis, DTOR(-50)));
		DgAddObj(DoRepType(DcSurface));
		DgAddObj(rotate_ingrp);
		DgAddObj(translate_ingrp);
		DgAddObj(DoSubDivSpec(DcSubDivRelative, sds));
		DgAddObj(DoPatch(DcRGB, DcLoc, DcHermite4, ctlpts, DcHermite4));
	DgClose();
}




int rotate_transformer;  /* a rotation transformer */
int translate_transformer;  /* a translation transformer */
int done=0;  /* quitting time? */


transform_callback_function(data, slot, val)
Dt32Bits data;
DtObject slot;
DtReal val;
{
	if(DcRotXSlot == slot) {
		DgReplaceObjInGroup(rotate_ingrp, change_transformer(
			rotate_transformer, RotX, val,
			DcPostConcatenate, DcTrue));
	}
	else if(DcRotYSlot == slot) {
		DgReplaceObjInGroup(rotate_ingrp, change_transformer(
			rotate_transformer, RotY, -val,
			DcPostConcatenate, DcTrue));
	}
	else if(DcRotZSlot == slot) {
		DgReplaceObjInGroup(rotate_ingrp, change_transformer(
			rotate_transformer, RotZ, -val,
			DcPostConcatenate, DcTrue));
	}
	else if(DcTransXSlot == slot) {
		DgReplaceObjInGroup(translate_ingrp, change_transformer(
			translate_transformer, TransX, val,
			DcPostConcatenate, DcTrue));
	}
	else if(DcTransYSlot == slot) {
		DgReplaceObjInGroup(translate_ingrp, change_transformer(
			translate_transformer, TransY, val,
			DcPostConcatenate, DcTrue));
	}
	else if(DcTransZSlot == slot) {
		DgReplaceObjInGroup(translate_ingrp, change_transformer(
			translate_transformer, TransZ, val,
			DcPostConcatenate, DcTrue));
	}
}


update_callback_function()
{
	DvUpdate(view_handle);
}


quit_callback_function()
{
	done = 1;
}





run_application(view)
DtObject view;
{
	DtObject
		transform_callback_object,
		update_callback_object,
		quit_callback_object,
		val_group, quit_slot;

	view_handle = view;
	rotate_transformer = make_transformer(DcPostConcatenate);
	translate_transformer = make_transformer(DcPostConcatenate);
	transform_callback_object =
		DoCallback(transform_callback_function, DcNullObject);
	update_callback_object =
		DoCallback(update_callback_function, DcNullObject);
	quit_callback_object =
		DoCallback(quit_callback_function, DcNullObject);
	quit_slot = DoInputSlot();
	DgAddObjToGroup(DsInqValuatorGroup(quit_slot), quit_callback_object);

	val_group = DsInqValuatorGroup(DcRotXSlot);
	DgAddObjToGroup(val_group, transform_callback_object);
	DgAddObjToGroup(val_group, update_callback_object);

	val_group = DsInqValuatorGroup(DcRotYSlot);
	DgAddObjToGroup(val_group, transform_callback_object);
	DgAddObjToGroup(val_group, update_callback_object);

	val_group = DsInqValuatorGroup(DcRotZSlot);
	DgAddObjToGroup(val_group, transform_callback_object);
	DgAddObjToGroup(val_group, update_callback_object);

	val_group = DsInqValuatorGroup(DcTransXSlot);
	DgAddObjToGroup(val_group, transform_callback_object);
	DgAddObjToGroup(val_group, update_callback_object);

	val_group = DsInqValuatorGroup(DcTransYSlot);
	DgAddObjToGroup(val_group, transform_callback_object);
	DgAddObjToGroup(val_group, update_callback_object);

	val_group = DsInqValuatorGroup(DcTransZSlot);
	DgAddObjToGroup(val_group, transform_callback_object);
	DgAddObjToGroup(val_group, update_callback_object);

	DvUpdate(view_handle);
	DvSetUpdateType(view_handle, DcUpdateDisplay);
	set_knobs(DcRotXSlot, DcRotYSlot, DcRotZSlot, DoInputSlot(),
		DcTransXSlot, DcTransYSlot, DcTransZSlot, quit_slot);

	printf("\n\n\t\tKNOBS\n\n");
	printf("top three left knobs rotate x, y, and z\n");
	printf("top three right knobs translate x, y, and z\n");
	printf("bottom right knob exits program\n\n");
	printf("knob reads may use one of the following modes:\n");
	printf("\t1 - blocking\n\t2 - non-blocking\n\t3 - signal driven\n\n");
	printf("choose knob mode -> ");
	switch(getchar()) {
		case '1': blocking_knobs(); break;
		case '2': non_blocking_knobs(); break;
		case '3': signaling_knobs(); break;
	}
}



/*
			BLOCKING KNOBS

	Start knobbing using blocking synchronous reads of the knob box.
	The main loop calls the utility function poll_knobs() directly and
	blocks until it returns.
	Advantages:
		- simple
		- does not burn cycles when nothing is happening
	Disadvantages:
		- program can do nothing else until poll_knobs() returns
		- input can easily pile up faster than it can be dealt with
*/

blocking_knobs()
{
	int knobcount=0;

	printf("using blocking knobs mode\n\n");
	start_knobs(KnobBlock, 1, 10);
	while( ! done) {
		poll_knobs();
		knobcount++;
	}
	printf("made %d calls to poll_knobs()\n\n", knobcount);
}





/*
			NON-BLOCKING KNOBS

	Start knobbing using non-blocking synchronous reads of the knob box.
	The main loop calls the utility function poll_knobs() directly but
	does not block.
	Advantages:
		- fairly simple
		- program is free to do other tasks while polling occasionally
	Disadvantages:
		- burns cycles when nothing is happening
		- main routine must poll often to get good interactivity
*/

non_blocking_knobs()
{
	static int yea=0, nea=0, pollval;

	printf("using non-blocking knobs mode\n\n");
	start_knobs(KnobNonBlock, 1, 2);
	while( ! done) {
		pollval = poll_knobs();
		switch(pollval)
			{
			case -1:
				printf("poll_knobs returned error\n");
				break;
			case  0:
				nea++;
				break;
			default:
				yea++;
				break;
			}
	}
	printf("made %d calls to poll_knobs() ",  yea + nea);
	printf("%d of which found input\n\n",  yea);
}






/*
			SIGNAL DRIVEN KNOBS

	Sets a signal handler to handle knob input asynchronously.

	Advantages:
		- knob input is dealt with as it occures
		- the main program is freed from needing to check knob status
	Disadvantages:
		- tricky to use correctly because the main routine must worry
		  about making non-reentrent library calls safely
		- installs a signal handler which is a scarce resource.
*/

signaling_knobs()
{
	printf("using signal driven knobs mode\n\n");
	start_knobs(KnobSig, 1, 2);
	enable_knobs();
	while( ! done)
		{
		pause();
		}
}
