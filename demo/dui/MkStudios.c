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
#include <stdio.h>

extern DtInt debug;
DtReal origin[3];
DtReal camera[3];

DtObject studio_group;



/*******************************************************************************
 ************************  m a k e _ s t u d i o s  ****************************
 *******************************************************************************
 * Module Name: make_studios
 * Module Type: c
 *     Purpose: Define the a "studio" environment (lights, cameras,...) and
 *              place it in a group object for later recall.
 *   Arguments: None
 * ReturnValue: None
 ******************************************************************************/
make_studios()
{
/*******************************************************************************
 * Declare and initialize variables.
 ******************************************************************************/
	extern zoom_callback();
	extern liteintens_callback();
	extern DtInt debug;
	extern DtReal camera[3];

	static DtPoint3 At	= { 0., 0., 0.};
	static DtVector3 Up  = { 0., 1., 0. };
	static DtPoint3 Light1 = { .15, .15, 1. };
	static DtPoint3 Light2 = { -1., -.1, 0.};
	static DtPoint3 Light3 = { .5, 1., 1. };
	static DtReal White[3] = { 1., 1., 1. };
	static DtReal Red[3]	= { 1., 0., 0. };
	static DtReal Green[3] = { 0., 1., 0. };
	static DtReal Blue[3]  = { 0., 0., 1. };

	DtObject DoGroup();

	if(debug) 
		printf("camera position = %lf, %lf, %lf\n",
			camera[0], camera[1], camera[2]);

/*******************************************************************************
 * Define the camera position, lights, & anything else that relates to how
 *  the model itself is viewed by the observer.  Place all of these in one
 *  group (object) for easy access later.  ('****_callback' routines may be
 *  found in Render.c)
 ******************************************************************************/
	studio_group = DoGroup(DcTrue);

		/* Set the viewing style (parallel/perspective) and front and
		 *  back clipping planes */
		DgAddObj(DoCallback(zoom_callback,DcNullObject));

		/* Set up ray tracing parameters */
		DgAddObj(DoGlbRendMaxObjs(1));
		DgAddObj(DoGlbRendMaxSub(5));

		/* Create and position the camera */
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(At, camera, Up));
			DgAddObj(DoCamera());
		DgAddObj(DoPopMatrix());

		/* create and position light source #1 */
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(At, Light1, Up));
			DgAddObj(DoLightColor(DcRGB,White));
			DgAddObj(DoCallback(liteintens_callback,DcNullObject));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix()); 

		/* create and position light source #2 */
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(At, Light2, Up));
			DgAddObj(DoLightColor(DcRGB,White));
			DgAddObj(DoCallback(liteintens_callback,DcNullObject));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix()); 

		/* create and position light source #3 */
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(At, Light3, Up));
			DgAddObj(DoLightColor(DcRGB,White));
			DgAddObj(DoCallback(liteintens_callback,DcNullObject));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix());

	DgClose();

}  /* End of make_studios function */

