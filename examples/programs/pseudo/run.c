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

int makeobjects  (DtShadeMode);
int run_application  (DtObject);
int create_color_table  (DtObject, DtShadeMode);

DtColorRGB background_col={0.2, 0.1, 0.6}; /* violet */

int main (void)
{
	DtObject device, frame, view, camera_group, object_group;
	DtVolume volume;
	DtShadeMode shademode;

	DsInitialize(0);

	printf("Pseudo color example\n");
	printf("do you wish range or component shademode? (r/c)\n");
	shademode = ('r' == getchar()) ? DcRange : DcComponent;
	while('\n' != getchar()) /* discard rest of input line */
		;
	printf("to see correct colors, move pointer into Dore window.\n");

	device = DoDevice("stdx11",
		"-visualtype DcPseudoColor -geometry 640x512+0+0");

	if( ! device) {
		printf("can't create device\n");
		exit(1);
	}

	DdSetShadeMode(device, shademode);
	create_color_table(device,shademode);

	makeobjects(shademode);

	DdInqExtent(device,&volume);

	if (!(frame = DoFrame())) {
		printf("can't create frame\n");
		exit(1);
	}

	if (DdInqColorTableSize(device) < 256) {
           printf("\nSorry. Display must support >= 256 pseudo");
           printf(" writeable color map entries\n");
           printf("(or else `create_color_table' must be modified)\n");
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
	DvSetBackgroundColor(view,DcRGB,background_col);

	camera_group = DsInqObjViaString("cameragroup",DcTypeGroup);
	object_group = DsInqObjViaString("objectgroup",DcTypeGroup);

	if((camera_group == DcNullObject) || (object_group == DcNullObject)) {
		printf("can't find camera or object group\n");
		exit(1);
	}

	DgAddObjToGroup(DvInqDisplayGroup(view), object_group);

	DgAddObjToGroup(DvInqDefinitionGroup(view), camera_group);

	run_application(view);

	printf("Goodbye!\n");

	DsReleaseObj(device);

	DsTerminate();
}




int create_color_table (
    DtObject device,
    DtShadeMode shademode)
{
	DtInt loc;
	DtReal entries[256*3];
	DtInt shaderanges[4];

	switch(shademode)
		{
		case DcComponent:
			for (loc=0;loc<256;loc++)
				{
				entries[loc*3] = (DtReal)(loc>>5) / 7.;
				entries[loc*3+1] = (DtReal)((loc>>2)&0x07) / 7.;
				entries[loc*3+2] = (DtReal)(loc&0x03) / 3.;
				}
			break;
		case DcRange:
			/*  Establish the "silver" shade range.  */
			for (loc=0;loc<128;loc++)
				{
				entries[loc*3] = (2.*(DtReal)(loc)) / 255.;
				entries[loc*3+1] = (2.*(DtReal)(loc)) / 255.;
				entries[loc*3+2] = (2.*(DtReal)(loc)) / 255.;
				}

			/*  Establish the "gold" shade range.  */
			for (loc=128;loc<256;loc++)
				{
				entries[loc*3] =
					(2.*((DtReal)(loc)-128.)) / 255. * 1.;
				entries[loc*3+1] =
					(2.*((DtReal)(loc)-128.)) / 255. * .7;
				entries[loc*3+2] =
					(2.*((DtReal)(loc)-128.)) / 255. * .3;
				}
			shaderanges[0] = 0;
			shaderanges[1] = 127;
			shaderanges[2] = 128;
			shaderanges[3] = 255;
			DdSetShadeRanges(device,0,2,shaderanges);
			break;
		default:
			printf("create_color_table: unknown shade mode\n");
			exit(1);

		}
	DdSetColorEntries(device,DcRGB,0,256,entries);
}
