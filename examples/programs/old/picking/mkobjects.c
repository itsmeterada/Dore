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
#include <stdio.h>


#define MAXOBJS 50

DtObject colors[MAXOBJS], highlight_color_obj;
DtInt obj_total = 0;


/* return all object colors to default */

clear_colors()
{
	int i;

	for(i=0; i<obj_total; i++)
		DgEmpty(colors[i]);
}



color_it(i)
{
	if((0 > i) || (i >= MAXOBJS))
		printf("color id %d out of range\n", i);
	else
		DgAddObjToGroup(colors[i], highlight_color_obj);
}




static DtPoint3 Pos = {0.,0.,0.};
static DtVector3 U = {1.,0.,0.}, V = {0.,1.,0.};

DtObject TextGroup()
{
	DtObject prim = DoText(Pos, U, V, "Pick me!");

	DsSetObjName(DoGroup(DcTrue),DcNameString,"text",DcFalse);
		add_color_inline();
		DgAddObj(DoTranslate(-.5,.0,0.));
		DgAddObj(DoScale(.1,.15,1.));
		DgAddObj(prim);
		test("text", prim);
	return(DgClose());
} 



DtObject BoxGroup()
{
	DtObject prim = DoPrimSurf(DcBox);

	DsSetObjName(DoGroup(DcTrue),DcNameString,"box",DcFalse);
		add_color_inline();
		DgAddObj(DoRotate(DcXAxis, .5));
		DgAddObj(DoRotate(DcYAxis, .2));
		DgAddObj(DoScale(.7,.7,.7));
		DgAddObj(DoTranslate(-.5,-.5,-.5));
		DgAddObj(prim);
		test("box", prim);
	return(DgClose());
}


DtObject CylinderGroup()
{
	static DtReal sds[] = {5.};
	DtObject prim = DoPrimSurf(DcCylinder);

	DsSetObjName(DoGroup(DcTrue),DcNameString,"cyl",DcFalse);
		add_color_inline();
		DgAddObj(DoRotate(DcXAxis, .5));
		DgAddObj(DoRotate(DcYAxis, .2));
		DgAddObj(DoScale(.4,.4,.4));
		DgAddObj(DoTranslate(0.,0.,-.5));
		DgAddObj(DoSubDivSpec(DcSubDivRelative, sds));
		DgAddObj(prim);
		test("cyl", prim);
	return(DgClose());
}


DtObject SphereGroup()
{
	static DtReal sds[] = {5.};
	DtObject prim = DoPrimSurf(DcSphere);

	DsSetObjName(DoGroup(DcTrue),DcNameString,"sphere",DcFalse);
		add_color_inline();
		DgAddObj(DoRotate(DcXAxis, .5));
		DgAddObj(DoRotate(DcYAxis, .2));
		DgAddObj(DoScale(.4,.4,.4));
		DgAddObj(DoSubDivSpec(DcSubDivRelative, sds));
		DgAddObj(prim);
		test("sphere", prim);
	return(DgClose());
}



DtObject SimplyGroup()
{
	static DtReal poly[] =
		{
			 0.,    1.,    0.,
			 -.951,  .309, 0.,
			 -.587, -.809, 0.,
			  .587, -.809, 0.,
			  .951,  .309, 0.,
		};

	DtObject prim = DoSimplePolygon(DcRGB, DcLoc, 5, poly, DcConvex);


	DsSetObjName(DoGroup(DcTrue),DcNameString,"simply",DcFalse);
		add_color_inline();
		DgAddObj(DoScale(.4,.4,.4));
		DgAddObj(prim);
		test("simply", prim);
	return(DgClose());
}




DtObject LineListGroup()
{
	static DtReal LineLoc[] =
		{
			.0,.0,.0, 1.,.0,.0,
			.0,.1,.0, 1.,.1,.0,
			.0,.2,.0, 1.,.2,.0,
			.0,.3,.0, 1.,.3,.0,
			.0,.4,.0, 1.,.4,.0,
			.0,.5,.0, 1.,.5,.0,
			.0,.6,.0, 1.,.6,.0,
			.0,.7,.0, 1.,.7,.0,
			.0,.8,.0, 1.,.8,.0,
			.0,.9,.0, 1.,.9,.0,
			.0,1.,.0, 1.,1.,.0,
		};

	DtObject prim = DoLineList(DcRGB, DcLoc, 11, LineLoc);


	DsSetObjName(DoGroup(DcTrue),DcNameString,"linlst",DcFalse);
		add_color_inline();
		DgAddObj(DoScale(.75, .75, .75));
		DgAddObj(DoTranslate(-.5,-.5,.0));
		DgAddObj(prim);
		test("linlst", prim);
	return(DgClose());
}




DtObject TriangleListGroup()
{
	static DtReal TriLoc[] =
		{
			 .0, .0, .0,   1., .0, .0,    .0,  .1, .0,
			1.,  .1, .0,   1., .2, .0,   0.,   .2, .0,
			 .0, .3, .0,   1., .3, .0,    .0,  .4, .0,
			1.,  .4, .0,   1., .5, .0,   0.,   .5, .0,
			 .0, .6, .0,   1., .6, .0,    .0,  .7, .0,
			1.,  .7, .0,   1., .8, .0,   0.,   .8, .0,
			 .0, .9, .0,   1., .9, .0,    .0, 1.,  .0
		};

	DtObject prim = DoTriangleList(DcRGB, DcLoc, 7, TriLoc);


	DsSetObjName(DoGroup(DcTrue),DcNameString,"trilst",DcFalse);
		add_color_inline();
		DgAddObj(DoScale(.75, .75, .75));
		DgAddObj(DoTranslate(-.5, -.5, .0));
		DgAddObj(prim);
		test("trilst", prim);
	return(DgClose());
}




DtObject TrimshGroup()
{
	static DtReal Verticies[] =
		{
			 0.,	 1.,	 1.618034,
			 0.,	 1.,	-1.618034,
			 0.,	-1.,	 1.618034,
			 0.,	-1.,	-1.618034,
			 1.,	 1.618034,	 0.,
			 1.,	-1.618034,	 0.,
			-1.,	 1.618034,	 0.,
			-1.,	-1.618034,	 0.,
			 1.618034,	 0.,	 1.,
			 1.618034,	 0.,	-1.,
			-1.618034,	 0.,	 1.,
			-1.618034,	 0.,	-1.
		};

	static DtInt Triangles[] =
		{
			  0,  4,  6,
			  6,  4,  1,
			  1,  4,  9,
			  9,  4,  8,
			  8,  4,  0,
			  8,  0,  2,
			  2,  0, 10,
			 10,  0,  6,
			 10,  6, 11,
			 11,  6,  1,
			 11,  1,  3,
			  3,  1,  9,
			  3,  9,  5,
			  5,  9,  8,
			  5,  8,  2,
			  5,  2,  7,
			  7,  2, 10,
			  7, 10, 11,
			  7, 11,  3,
			  7,  3,  5,
		};

	DtObject prim = DoTriangleMesh(DcRGB, DcLoc,
		12, Verticies, 20, Triangles, DcFalse);


	DsSetObjName(DoGroup(DcTrue),DcNameString,"trimsh",DcFalse);
		add_color_inline();
		DgAddObj(DoRotate(DcXAxis, .3));
		DgAddObj(DoRotate(DcYAxis, .1));
		DgAddObj(DoScale(.25, .25, .25));
		DgAddObj(prim);
		test("trimsh", prim);
	return(DgClose());
}




DtObject SimmshGroup()
{
	static DtReal Vertices[] =
		{
			 0.,  .618034,  1.618034,
			 0.,  .618034, -1.618034,
			 0., -.618034,  1.618034,
			 0., -.618034, -1.618034,
			  1.618034, 0.,  .618034,
			  1.618034, 0., -.618034,
			 -1.618034, 0.,  .618034,
			 -1.618034, 0., -.618034,
			  .618034,  1.618034, 0.,
			  .618034, -1.618034, 0.,
			 -.618034,  1.618034, 0.,
			 -.618034, -1.618034, 0.,
			  1.,  1.,  1.,
			  1.,  1., -1.,
			  1., -1.,  1.,
			  1., -1., -1.,
			 -1.,  1.,  1.,
			 -1.,  1., -1.,
			 -1., -1.,  1.,
			 -1., -1., -1.,
		};

	static DtInt VertexList[] = 
		{
			  0, 12,  8, 10, 16,
			 16, 10, 17,  7,  6,
			  6, 18,  2,  0, 16,
			  0,  2, 14,  4, 12,
			 12,  4,  5, 13,  8,
			  8, 13,  1, 17, 10,
			 17,  1,  3, 19,  7,
			  7, 19, 11, 18,  6,
			  1, 13,  5, 15,  3,
			 15,  5,  4, 14,  9,
			  9, 14,  2, 18, 11,
			 11, 19,  3, 15,  9,
		};
	
	static DtInt Contours[] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};

	DtObject prim = DoSimplePolygonMesh(DcRGB, DcLoc,
		20, Vertices, 12, Contours, VertexList, DcConvex, DcFalse);


	DsSetObjName(DoGroup(DcTrue),DcNameString,"simmsh",DcFalse);
		add_color_inline();
		DgAddObj(DoRotate(DcXAxis, .3));
		DgAddObj(DoRotate(DcYAxis, .1));
		DgAddObj(DoScale(.25, .25, .25));
		DgAddObj(prim);
		test("simmsh", prim);
	return(DgClose());
}




DtObject PlyGroup()
{
	static DtReal Vertices[] =
		{
			   3.,  3., 0.,
			  -3.,  3., 0.,
			  -3., -3., 0.,
			   3., -3., 0.,
			   5.,  5., 0.,
			  -5.,  5., 0.,
			  -5., -5., 0.,
			   5., -5., 0.,
			   7.,  7., 0.,
			  -7.,  7., 0.,
			  -7., -7., 0.,
			   7., -7., 0.,
			   9.,  9., 0.,
			  -9.,  9., 0.,
			  -9., -9., 0.,
			   9., -9., 0.,
			   1.,  1., 0.,
			  -1.,  1., 0.,
			   6.,-13., 0.,
			  -6.,-13., 0.,
		 };

	static DtInt Contours[] = {4, 4, 4, 4, 4, 4};

	DtObject prim = DoPolygon(DcRGB, DcLoc,
		5, Contours, Vertices, DcComplex);


	DsSetObjName(DoGroup(DcTrue),DcNameString,"ply",DcFalse);
		add_color_inline();
		DgAddObj(DoScale(.03, .03, .03));
		DgAddObj(prim);
		test("ply", prim);
	return(DgClose());
}





static DtReal Heart[] = 
	{
		 0.,    .2,  0.,
		  .07,  .3,  0.,
		  .16,  .34, 0.,
		  .23,  .33, 0.,
		  .3,   .26, 0.,
		  .32,  .14, 0.,
		  .28, 0.,   0.,
		  .12, -.23, 0.,
		 0.,   -.3,  0.,
		 -.12, -.23, 0.,
		 -.28, 0.,   0.,
		 -.32,  .14, 0.,
		 -.3,   .26, 0.,
		 -.23,  .33, 0.,
		 -.16,  .34, 0.,
		 -.07,  .3,  0.,
		 0.,    .2,  0.,
		  .1,   .1,  0.,
		  .1,  -.1,  0.,
		 -.1,  -.1,  0.,
		 -.1,   .1,  0.,
		  .1,   .1,  0.,
		 -.1,  -.1,  0.,
		 0.,   -.2,  0.,
		  .1,  -.1,  0.,
		 -.1,   .1,  0.,
	};


DtObject PolylineGroup()
{
	DtObject prim = DoPolyline(DcRGB, DcLoc, 26, Heart);

	DsSetObjName(DoGroup(DcTrue),DcNameString,"plylin",DcFalse);
		add_color_inline();
		DgAddObj(prim);
		test("plylin", prim);
	return(DgClose());
} 


DtObject PolymarkerGroup()
{
	DtObject prim = DoPolymarker(17, Heart);

	DsSetObjName(DoGroup(DcTrue),DcNameString,"plymkr",DcFalse);
		add_color_inline();
		DgAddObj(DoMarkerFont(DcGothicGerman));
		DgAddObj(DoMarkerGlyph((DtInt)'*')); /* asterick */
		DgAddObj(DoMarkerScale(30.0));
		DgAddObj(prim);
		test("plymkr", prim);
	return(DgClose());
} 


DtObject PointListGroup()
{
	DtObject prim = DoPointList(DcRGB, DcLoc, 26, Heart);

	DsSetObjName(DoGroup(DcTrue),DcNameString,"pntlst",DcFalse);
		add_color_inline();
		DgAddObj(prim);
		test("pntlst", prim);
	return(DgClose());
} 


DtObject TorusGroup()
{
	DtObject prim = DoTorus(.3, .15);

	DsSetObjName(DoGroup(DcTrue),DcNameString,"torus",DcFalse);
		add_color_inline();
		DgAddObj(DoRotate(DcXAxis, DTOR(40)));
		DgAddObj(prim);
		test("torus", prim);
	return(DgClose());
} 



DtObject PatchGroup()
{
	static DtReal ctlpts[] =
		{
		-.5,.5,0.,	0.,-.5,-.5,	0.,0.,-1.,	1.,0.,0.,
		0.,-.5,.5,	.5,.5,0.,	1.,0.,0.,	0.,0.,-1.,
		0.,0.,1.,	1.,0.,0.,	0.,0.,0.,	0.,0.,0.,
		1.,0.,0.,	0.,0.,1.,	0.,0.,0.,	0.,0.,0.,
		};
	static DtReal sds[] = {.03};

	DtObject prim = DoPatch(DcRGB, DcLoc, DcHermite4, ctlpts, DcHermite4);


	DsSetObjName(DoGroup(DcTrue),DcNameString,"patch",DcFalse);
		add_color_inline();
		DgAddObj(DoRotate(DcYAxis, .3));
		DgAddObj(DoRotate(DcXAxis, .4));
		DgAddObj(DoScale(.7, .7, .7));
		DgAddObj(DoSubDivSpec(DcSubDivRelative, sds));
		DgAddObj(prim);
		test("patch", prim);
	return(DgClose());
} 



DtObject PolygonMeshGroup()
{
	static DtReal plyverts[] = 
		{
		/* corners */
		1.,0.,0.,    /*  0 */
		1.,1.,0.,    /*  1 */
		0.,1.,0.,    /*  2 */
		0.,0.,0.,    /*  3 */
		0.,1.,-1.,   /*  4 */
		0.,0.,-1.,   /*  5 */
		1.,0.,-1.,   /*  6 */
		1.,1.,-1.,   /*  7 */

		/* front hole */
		.8,.2,0.,    /*  8 */
		.8,.8,0.,    /*  9 */
		.2,.8,0.,    /* 10 */
		.2,.2,0.,    /* 11 */

		/* left hole */
		0.,.2,-.2,   /* 12 */
		0.,.8,-.2,   /* 13 */
		0.,.8,-.8,   /* 14 */
		0.,.2,-.8,   /* 15 */

		/* back hole */
		.2,.2,-1.,   /* 16 */
		.2,.8,-1.,   /* 17 */
		.8,.8,-1.,   /* 18 */
		.8,.2,-1.,   /* 19 */

		/* right hole */
		1.,.2,-.8,   /* 20 */
		1.,.8,-.8,   /* 21 */
		1.,.8,-.2,   /* 22 */
		1.,.2,-.2,   /* 23 */

		/* bottom hole */
		.2,0.,-.2,   /* 24 */
		.2,0.,-.8,   /* 25 */
		.8,0.,-.8,   /* 26 */
		.8,0.,-.2,   /* 27 */

		/* top hole */
		.8,1.,-.2,   /* 28 */
		.8,1.,-.8,   /* 29 */
		.2,1.,-.8,   /* 30 */
		.2,1.,-.2,   /* 31 */
		};

	static DtInt
		polygons[] = {2,2,2,2,2,2},
		contours[] = {4,4,4,4,4,4,4,4,4,4,4,4},
		vlist[] =
			{
			/* front */
			0,1,2,3,
			8,9,10,11,

			/* left */
			3,2,4,5,
			12,13,14,15,

			/* back */
			5,4,7,6,
			16,17,18,19,

			/* right */
			6,7,1,0,
			20,21,22,23,

			/* bottom */
			0,3,5,6,
			24,25,26,27,

			/* top */
			1,7,4,2,
			28,29,30,31,
			};

	DtObject prim = DoPolygonMesh(DcRGB, DcLoc, 32, plyverts,
		6, polygons, contours, vlist, DcConcave, DcFalse);

	DsSetObjName(DoGroup(DcTrue),DcNameString,"plymsh",DcFalse);
		add_color_inline();
		DgAddObj(DoRotate(DcXAxis, DTOR(20)));
		DgAddObj(DoRotate(DcYAxis, DTOR(25)));
		DgAddObj(DoScale(.6, .6, .6));
		DgAddObj(DoTranslate(-.5, -.5, .5));
		DgAddObj(prim);
		test("plymsh", prim);
	return(DgClose());
} 



make_objects()
{
	DtObject box, cyl, sphere, simply, trimsh,
		text, simmsh, ply, linelist, trilst, plylin,
		plymkr, pntlst, torus, patch, plymsh;
	static DtPoint3
		origin = {0., 0., 0.},
		from   = {0., 0., 4.};
	static DtVector3 up = {0., 1., 0.};
	static DtReal highlight_color[3] = {.5,.7,.3};


	highlight_color_obj = DsHoldObj(DoDiffuseColor(DcRGB, highlight_color));

	DsSetObjName(DoGroup(DcTrue),DcNameString,"cam",DcFalse);
		DgAddObj(DoParallel(8.,-1.,-400.));  
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, from, up));
			DgAddObj(DoStereoSwitch(DcTrue));
			DgAddObj(DoStereo(0.03, from[2] -1.));
			DgAddObj(DoCamera());
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, from, up));
			DgAddObj(DoLightIntens(.8));
			DgAddObj(DoLight());
			DgAddObj(DoLightType(DcLightAmbient));
			DgAddObj(DoLightIntens(.2));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix());
	DgClose();

	box = BoxGroup();
	sphere = SphereGroup();
	cyl = CylinderGroup();
	simply = SimplyGroup();
	linelist = LineListGroup();
	trimsh = TrimshGroup();
	simmsh = SimmshGroup();
	ply = PlyGroup();
	text = TextGroup();
	plylin = PolylineGroup();
	plymkr = PolymarkerGroup();
	pntlst = PointListGroup();
	torus = TorusGroup();
	trilst = TriangleListGroup();
	patch = PatchGroup();
	plymsh = PolygonMeshGroup();

	DsSetObjName(DoGroup(DcTrue),DcNameString,"obj",DcFalse);
		DgAddObj(DoRepType(DcSurface));
		DgAddObj(DoPickSwitch(DcOn));

		DgAddObj(DoPushMatrix());
			DgAddObj(DoTranslate(-3.,0.,0.));
			DgAddObj(box);
		DgAddObj(DoPopMatrix());

		DgAddObj(DoPushMatrix());
			DgAddObj(DoTranslate(-2.,0.,0.));
			DgAddObj(simply);
		DgAddObj(DoPopMatrix());

		DgAddObj(DoPushMatrix());
			DgAddObj(DoTranslate(-1.,0.,0.));
			DgAddObj(linelist);
		DgAddObj(DoPopMatrix());

		DgAddObj(torus);

		DgAddObj(DoPushMatrix());
			DgAddObj(DoTranslate(1.,0.,0.));
			DgAddObj(trimsh);
		DgAddObj(DoPopMatrix());

		DgAddObj(DoPushMatrix());
			DgAddObj(DoTranslate(2.,0.,0.));
			DgAddObj(simmsh);
		DgAddObj(DoPopMatrix());

		DgAddObj(DoPushMatrix());
			DgAddObj(DoTranslate(-3.,1.,0.));
			DgAddObj(text);
		DgAddObj(DoPopMatrix());

		DgAddObj(DoPushMatrix());
			DgAddObj(DoTranslate(-2.,1.,0.));
			DgAddObj(plylin);
		DgAddObj(DoPopMatrix());

		DgAddObj(DoPushMatrix());
			DgAddObj(DoTranslate(-1.,1.,0.));
			DgAddObj(plymkr);
		DgAddObj(DoPopMatrix());

		DgAddObj(DoPushMatrix());
			DgAddObj(DoTranslate(0.,1.,0.));
			DgAddObj(pntlst);
		DgAddObj(DoPopMatrix());

		DgAddObj(DoPushMatrix());
			DgAddObj(DoTranslate(1.,1.,0.));
			DgAddObj(ply);
		DgAddObj(DoPopMatrix());

		DgAddObj(DoPushMatrix());
			DgAddObj(DoTranslate(2.,1.,0.));
			DgAddObj(trilst);
		DgAddObj(DoPopMatrix());

		DgAddObj(DoPushMatrix());
			DgAddObj(DoTranslate(-3.,-1.,0.));
			DgAddObj(sphere);
		DgAddObj(DoPopMatrix());

		DgAddObj(DoPushMatrix());
			DgAddObj(DoTranslate(-2.,-1.,0.));
			DgAddObj(cyl);
		DgAddObj(DoPopMatrix());

		DgAddObj(DoPushMatrix());
			DgAddObj(DoTranslate(-1.,-1.,0.));
			DgAddObj(patch);
		DgAddObj(DoPopMatrix());

		DgAddObj(DoPushMatrix());
			DgAddObj(DoTranslate(0.,-1.,0.));
			DgAddObj(plymsh);
		DgAddObj(DoPopMatrix());
	DgClose();
}




/* test an object's compute bounding volume method and it's print method. */

test(str, obj)
char *str;
DtObject obj;
{
	DtVolume bndvol;
	int i;

	printf("\n\n\n**** %s ****\n", str);
	DsCompBoundingVol(&bndvol, obj);
	printf("bounding volume:\n\t(%f, %f, %f) to\n\t(%f, %f, %f)\n\n", 
		bndvol.bll[0], bndvol.bll[1], bndvol.bll[2],
		bndvol.fur[0], bndvol.fur[1], bndvol.fur[2]);
	printf("executing print method:\n\n");
	DsPrintObj(obj);
	for(i=0; i<strlen(str) + 10; i++)
		putchar('*');
	putchar('\n');
}



/*
make and add an inline group to the currently open group.  These
inline groups will be used to hold color objects when the current
group is picked.  The pick id of the current object is set here to
be the same as the index into the colors array for easy access to
the correct inline group.
*/

add_color_inline()
{
	if(MAXOBJS == obj_total + 1)
		{
		printf("colors array overflow!\n");
		exit(1);
		}
	DgAddObj(colors[obj_total] = DoInLineGroup(DcFalse));
	DgAddObj(DoPickID(obj_total));
	obj_total++;
}
