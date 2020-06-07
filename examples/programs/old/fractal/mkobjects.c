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
 
#include <fcntl.h>
#include <dore/dore.h>
#include <dore/dore_util.h>
#include "mtn.h"

DtReal lake_level;

makeobjects(argc, argv)
	int argc;
	char *argv[];
{
	static DtPoint3
		camera_at = {0.,0.,-8.}, /* negative Z */
		camera_from = {0.,3.0,0.0}, /* near origin */
		origin = {0.,0.,0.}, /* origin */
		light0 = {8.,8.,8.},
		light1 = {-8.,8.,8.};
	static DtVector3 up = {0.,1.,0.};
	static DtReal blue[] = {.0,.0,1.};
	static DtReal brown[] = {.6,.4,.1};
	DtObject make_mesh();
	DtObject make_lakes();
	char infilename[200];
	int input;
	DtInt smooth;

	prsarg_get_keyword_string(argc,argv,"-input","NoNe",infilename);
	prsarg_get_keyword_int(argc,argv,"-smooth",1,&smooth);
	prsarg_get_keyword_real(argc,argv,"-l",-.75,&lake_level);
	
	if ((input = open(infilename, O_RDONLY)) < 0) {
                if (!strcmp(infilename, "NoNe"))
		   printf("Must specify an input file. See README file.\n");
		else
		   printf("Cannot open '%s'\n", infilename);
		exit (1);
	}

	DsSetObjName(DoGroup(DcTrue), DcNameString, "cameragroup", DcFalse);
		DgAddObj(DoPerspective(40.,-1.,-10.));  
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(camera_at, camera_from, up));
			DgAddObj(DoCamera());
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, light0, up));
			DgAddObj(DoLightIntens(.5));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, light1, up));
			DgAddObj(DoLightIntens(.5));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix());
	DgClose();

	DsSetObjName(DoGroup(DcTrue), DcNameString, "objectgroup", DcFalse);
		DgAddObj(DoDiffuseColor(DcRGB, brown));
		DgAddObj(DoRotate(DcXAxis, DTOR(-80)));
		DgAddObj(DoRotate(DcZAxis, DTOR(60)));
		DgAddObj(DoRepType(DcSurface));
		DgAddObj(DoInterpType(DcVertexShade));
		DgAddObj(make_mesh(input, smooth));
		DgAddObj(DoTranslate(0., 0., lake_level));
		DgAddObj(DoDiffuseColor(DcRGB, blue));
		DgAddObj(make_lakes());
		DgSetElePtr(-3, DcCurrent);
}

#include <stdio.h>

run_application(view)
DtObject view;
{
	DtReal level;
	double d;
	static DtReal skyblue[] = {.5,.5,.8};

	DvSetBackgroundColor(view, DcRGB, skyblue);
	DvUpdate(view);
	DvSetUpdateType(view, DcUpdateDisplay);
	printf("\nInitial water level is %g\n", lake_level);
	while (1)
	{
		printf("\nEnter new water level (or `q' to quit): ");
		if (scanf("%lf", &d) <= 0)
			break;
		level = d;
		DgReplaceObj(DoTranslate(0.,0.,level));
		DvUpdate(view);
	}
}

#include <sys/types.h>
#include <sys/stat.h>

#define	ODDBITS	0xaaaaaaaa

/*  Read in the triangle list from the file.  */

DtObject
make_mesh(f, smooth)
	int f;
	DtInt smooth;
{
	DtReal *vspace, *zspace;
	DtInt *mspace;
	DtObject tmp;
	struct stat sb;
	FractalHeader fhdr;
	char *malloc();

	if (fstat(f, &sb) != 0) {
		printf("Cannot stat() input file\n");
		exit(2);
	}

	if ((sb.st_mode & S_IFMT) != S_IFREG) {
		printf("Input must be a regular file\n");
		exit(2);
	}

	if (read(f, (char *) &fhdr, sizeof(fhdr)) != sizeof(fhdr) ||
		fhdr.magic != FRACTALMAGIC ||
		fhdr.zcount != gencount(fhdr.gen) ||
		(fhdr.flags & F_COLOR) && sb.st_size !=
			sizeof(fhdr) + fhdr.zcount * 4 * sizeof(double) ||
		(fhdr.flags & F_COLOR) == 0 && sb.st_size !=
			sizeof(fhdr) + fhdr.zcount * sizeof(double)) {
		printf("Invalid input data file\n");
		exit(2);
	}

	if (fhdr.flags & F_COLOR)
		vspace = (DtReal *) malloc(6 * sizeof(DtReal) * fhdr.zcount);
	else
		vspace = (DtReal *) malloc(3 * sizeof(DtReal) * fhdr.zcount);

	zspace = (DtReal *) malloc(sb.st_size - sizeof(fhdr));
	mspace = (DtInt *) malloc((1L << (fhdr.gen * 2)) * 3 * sizeof(DtReal));

	if (vspace == (DtReal *) 0 ||
			zspace == (DtReal *) 0 || mspace == (DtInt *) 0) {
		printf("Cannot get memory for data\n");
		exit(2);
	}
	
	if (read(f, (char *) zspace, sb.st_size - sizeof(fhdr)) !=
						sb.st_size - sizeof(fhdr)) {
		printf("Unexpected read failure\n");
		exit(2);
	}

	dist_xy(&fhdr, zspace, vspace);
	genmesh(fhdr.gen, mspace);

#if 0
	if (fhdr.flags & F_COLOR) {
		int i;

		printf("num verts = %d\n, \tverts:\n", fhdr.zcount);
		for(i=0; i<fhdr.zcount; i++)
			printf("%d: %f, %f, %f <%f, %f, %f>\n",
				i, vspace[6*i], vspace[6*i+1], vspace[6*i+2],
				vspace[6*i+3], vspace[6*i+4], vspace[6*i+5]);
		printf("num tris = %d\n, \ttris:\n", 1L << (fhdr.gen * 2));
		for(i=0; i<1L << (fhdr.gen * 2); i++)
			printf("%d: %d %d %d\n",
				i, mspace[3*i], mspace[3*i+1], mspace[3*i+2]);
	} else {
		int i;

		printf("num verts = %d\n, \tverts:\n", fhdr.zcount);
		for(i=0; i<fhdr.zcount; i++)
			printf("%d: %f, %f, %f\n",
				i, vspace[3*i], vspace[3*i+1], vspace[3*i+2]);
		printf("num tris = %d\n, \ttris:\n", 1L << (fhdr.gen * 2));
		for(i=0; i<1L << (fhdr.gen * 2); i++)
			printf("%d: %d %d %d\n",
				i, mspace[3*i], mspace[3*i+1], mspace[3*i+2]);
	}
#endif

	if (fhdr.flags & F_COLOR)
		tmp = DoTriangleMesh(DcRGB, DcLocClr, fhdr.zcount, vspace,
				1L << (fhdr.gen * 2), mspace, smooth);
	else
		tmp = DoTriangleMesh(DcRGB, DcLoc, fhdr.zcount, vspace,
				1L << (fhdr.gen * 2), mspace, smooth);

	free(vspace);
	free(zspace);
	free(mspace);
	return (tmp);
}


DtObject
make_lakes()
{
	static DtReal coords[] =
		{5.0, 8.660254, 0.,
		 0., 0., 0.,
		 10.0, 0., 0.};

	return(DoTriangleList(DcRGB, DcLoc, 1, coords));
}
