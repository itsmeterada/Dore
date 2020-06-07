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
#include <math.h>

#define NMODELS 1       

int model_type;
int nmodels = NMODELS;
int firstcycle = 0;
int lastcycle = 0;
DtObject models[NMODELS];
extern DtInt debug;

#define MaxDepth 12

DtObject segment;
DtObject colors[MaxDepth];

float XBend = .3;
float YBend = .5;
float ZBend = .7;
float Speed = 1.;

geom_spec()
{
	int segment_callback();
	DtObject DoInLineGroup(), DoGroup();

	segment = DoInLineGroup(DcTrue);
		DgAddObj(DoScale(.9,.9,.9));
		DgAddObj(DoTranslate(0.,0.,-1.));
		DgAddObj(DoPushMatrix());
			DgAddObj(DoScale(.5,.5,1.));
			DgAddObj(DoPrimSurf(DcCylinder));
		DgAddObj(DoPopMatrix());
	DgClose();
	DsHoldObj(segment);

	init_colors();

	models[0] = DoGroup(DcTrue);

		DgAddObj(DoRotate(DcXAxis, 3.1415927/2.));
		DgAddObj(DoCallback(segment_callback, DcNullPtr));

	DgClose();
	DsHoldObj(models[0]);

}

init_colors()
{
	int depth;
	DtReal color[3];
	DtReal len;
	DtObject DoDiffuseColor();

	color[2] = 0.;
	for (depth=0; depth<MaxDepth; depth++) {

		color[0] = (DtReal)depth/(DtReal)(MaxDepth-1);
		color[1] = 1. - (DtReal)depth/(DtReal)(MaxDepth-1);
		len = sqrt(color[0]*color[0] + color[1]*color[1]);
		color[0] /= len;
		color[1] /= len;

		colors[depth]=DoDiffuseColor(DcRGB, color);
		DsHoldObj(colors[depth]);

	}

}


int segment_callback(obj)
DtObject obj;
{
	int depth;
	static float time = 0.;
	static DtReal a=0., b=0., c=0.;

	for (depth=0; depth<MaxDepth; depth++) {

		DsExecuteObj(DoRotate(DcXAxis, a));
		DsExecuteObj(DoRotate(DcYAxis, b));
		DsExecuteObj(DoRotate(DcZAxis, c));

		DsExecuteObj(colors[depth]);

		DsExecuteObj(segment);
	}

	a= XBend * sin(.200000*time);
	b= YBend * sin(.221234*time);
	c= ZBend * sin(.244098*time);
	time += Speed;
}

