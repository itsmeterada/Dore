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
*/

int makeobjects (
    DtShadeMode shademode)
{
	static DtReal redish[] = {.8,.2,.2};
	DtObject make_mesh();

	static DtPoint3
		origin = {0.,0.,0.},
		z_dir = {0.,0.,4.},
		light = {1.,1.,1.};
	static DtVector3 up = {0.,1.,0.};


	DsSetObjNameString(DoGroup(DcTrue), "cameragroup", DcFalse);
		DgAddObj(DoPerspective(60.,-1.,-10.));
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, z_dir, up));
			DgAddObj(DoCamera());
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, light, up));
			DgAddObj(DoLightIntens(1.));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix());
	DgClose();

	ingrp = DoInLineGroup(DcFalse);

	DsSetObjNameString(DoGroup(DcTrue), "objectgroup", DcFalse);
		DgAddObj(DoDiffuseColor(DcRGB, redish));
		DgAddObj(DoRepType(DcSurface));
		DgAddObj(DoPushMatrix());
			if(DcRange == shademode)
				DgAddObj(DoShadeIndex(0)); /* silver */
			DgAddObj(DoTranslate(1.25, 0., 0.));
			DgAddObj(ingrp);
			DgAddObj(DoRotate(DcYAxis, DTOR(-20)));
			DgAddObj(DoRotate(DcXAxis, DTOR(-50)));
			DgAddObj(DoTranslate(-.5,-.5,-.5)); /* center it */
			DgAddObj(DoPrimSurf(DcBox));
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix());
			if(DcRange == shademode)
				{
				DgAddObj(DoInterpType(DcVertexShade));
				DgAddObj(DoShadeIndex(1)); /* gold */
				}
			DgAddObj(DoTranslate(-1.25, 0., 0.));
			DgAddObj(ingrp);
			DgAddObj(DoRotate(DcYAxis, DTOR(-20)));
			DgAddObj(DoRotate(DcXAxis, DTOR(-50)));
			DgAddObj(DoPrimSurf(DcSphere));
		DgAddObj(DoPopMatrix());
	DgClose();
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
