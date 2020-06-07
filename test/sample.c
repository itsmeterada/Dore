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
 
/*  ======================================================================
    This file contains the source code for the sample program from 
    Chapter 2 of the Dore Programmer's Guide.  The generated image contains 
    a blue cylinder, a magenta box, and a wireframe magenta sphere.
    ====================================================================== */

#include <stdio.h>
/* #include <windows.h> */
#include <dore/dore.h>     /* standard Dore include */



#ifdef WIN32
int APIENTRY
WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
#else
main()
#endif
{
    

    DtObject device, frame, view;
    DtObject post, base, def_group, obj_group;
    static DtPoint3
    	origin     = {  0.0,  0.0,  0.0 },
    	eye_point  = { 10.0, 10.0, 10.0 },
    	light      = {  1.0,  1.0,  0.5 };
    static DtVector3
    	up         = {  0.0,  1.0,  0.0 };
    static DtReal
    	magenta[]  = {  1.0,  0.0,  1.0 },   /* R,G,B */
    	grey[]     = {  0.4,  0.4,  0.4 },
    	sky_blue[] = {  0.3,  0.3,  1.0 },
    	sds[]      = { 28.0 };  /* for DoSubDivSpec */

    /* initialize Dore */
    DsInitialize(0);                

    /* create device */
#ifdef WIN32
    device = DoDevice ("opengl", 
                       "-filename dore_out");
#else
#if defined(sun) && !defined(NO_SUNVIEW) 
    device = DoDevice("sunview", 
                      "-visualtype DcPseudoColor -zbuffer");
#else
#ifdef sun 
    device = DoDevice("stdx11", 
                      "-visualtype DcPseudoColor -zbuffer");
#else
#ifdef sgi 
    device = DoDevice("gl", 
                      "-visualtype DcTrueColor");
#else
#if defined (titan) && defined (__P3__)
    device = DoDevice("stdx11", 
                      "-visualtype DcTrueColor -zbuffer");
#else
    device = DoDevice("stdx11", 
                      "-visualtype DcPseudoColor -zbuffer");
#endif
#endif
#endif
#endif
#endif /* ifdef WIN32 */


    /* create frame and add to the device */
    frame = DoFrame();
    DdSetFrame(device, frame);

    /* create view and add to frame */
    view = DoView();
    DvSetBackgroundColor(view, DcRGB, grey);
    DgAddObjToGroup(DfInqViewGroup(frame), view);

    /* use degrees for all angles */

    DsSetAngleUnits(DcAngleDegrees);


    /* create group with a parallel camera and a light */
    def_group = DoGroup(DcTrue);
    	DgAddObj(DoParallel(10.0, -0.1, -20.0));
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

    /* define post group */
    post = DoGroup(DcTrue);
    	DgAddObj(DoDiffuseColor(DcRGB, sky_blue));
    	DgAddObj(DoTranslate(0.0, 1.5, 0.0));
    	DgAddObj(DoRotate(DcXAxis, 90.));
    	DgAddObj(DoScale(1.2, 1.2, 2.4));
    	DgAddObj(DoPrimSurf(DcCylinder));
    DgClose();

    /* define base group */
    base = DoGroup(DcTrue);
    	DgAddObj(DoTranslate(0.0, -2.2, 0.0));
    	DgAddObj(DoScale(2.5, 2.5, 2.5));
    	DgAddObj(DoTranslate(-0.5, -0.5, -0.5));
    	DgAddObj(DoPrimSurf(DcBox));
    DgClose();

    /* obj_group includes post & base groups, and sphere */
    obj_group = DoGroup(DcTrue);
    	DgAddObj(DoRepType(DcSurface));
    	DgAddObj(DoSubDivSpec(DcSubDivSegments, sds));
    	DgAddObj(DoDiffuseColor(DcRGB, magenta));
    	DgAddObj(post);
    	DgAddObj(base);
    	DgAddObj(DoTranslate(0.0, 3.2, 0.0));
    	DgAddObj(DoScale(1.5, 1.5, 1.5));
    	DgAddObj(DoRepType(DcWireframe));
    	DgAddObj(DoPrimSurf(DcSphere));
    DgClose();

    /* add objects to view */
    DgAddObjToGroup(DvInqDefinitionGroup(view), def_group);
    DgAddObjToGroup(DvInqDisplayGroup(view), obj_group);

    /* render the image */
    DdUpdate(device);

    printf("Hit return to continue.");
    getchar();
     
    /* clean up */
    DsReleaseObj(device); 

    /* shut down */
    DsTerminate();
}
