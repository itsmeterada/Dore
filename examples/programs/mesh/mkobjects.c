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


DtObject ingrp;


/* 
   Create the display and definition groups.
   makeobjects() expects to be called once near the beginning of the program.
   Note that the command line arguments are passed to makeobjects()
   though his version does not use them.
*/

int makeobjects (void)
{
	static DtPoint3
		origin = {0.,0.,0.},
		camera_from = {0.,0.,8.}, /* positive Z  */
		light0 = {-8.,8.,8.},     /* upper left  */
		light1 = {8.,8.,8.};      /* upper right */
	static DtVector3 up = {0.,1.,0.};
	static DtReal redish[] = {.8,.2,.2};
	DtObject make_mesh();

	DsSetObjNameString(DoGroup(DcTrue), "cameragroup", DcFalse);
		DgAddObj(DoParallel(10.,-1.,-400.));  
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

	DsSetObjNameString(DoGroup(DcTrue), "objectgroup", DcFalse);
		DgAddObj(DoDiffuseColor(DcRGB, redish));
		DgAddObj(DoRotate(DcYAxis, DTOR(-20)));
		DgAddObj(DoRotate(DcXAxis, DTOR(-50)));
		DgAddObj(DoInterpType(DcVertexShade));
		DgAddObj(DoRepType(DcSurface));
		DgAddObj(ingrp = DoInLineGroup(DcFalse));
		DgAddObj(make_mesh());
	DgClose();
}




/* 
   Returns a triangle mesh object with exactly 1000 triangles
   representing the function z = sin(x) * sin(3y).
*/

#define XDIM 26
#define YDIM 21

DtObject make_mesh (void)
{
	DtReal verts[XDIM][YDIM][3], xval, yval, zval;
	DtInt  x, y;

	for(x=0; x<XDIM; x++)
		for(y=0; y<YDIM; y++)
			{
			xval = (x - XDIM/2.)/4.0;
			yval = (y - YDIM/2.)/4.0;
			zval = sin(xval) * sin(3*yval);
			verts[x][y][0] = xval;
			verts[x][y][1] = yval;
			verts[x][y][2] = zval;
			}
	return(rect_mesh(DcRGB, DcLoc, XDIM, YDIM, (DtReal *)verts, DcTrue));
}




int run_application (
    DtObject view)
{
	DtReal yrot=0;
	int i=200;

	DvUpdate(view);
	DvSetUpdateType(view, DcUpdateDisplay);
	while(i--) {
		yrot+=5.;
		DgReplaceObjInGroup(ingrp, DoRotate(DcYAxis, DTOR(yrot)));
		DvUpdate(view);
	}
        printf("Hit return to exit.\n");
	getchar();
}
