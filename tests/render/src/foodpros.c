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
 
#include "dore.h"
#include <stdio.h>

#define MAX_PATCHES 500
#define MAX_GRID_POINTS 1000

typedef struct {
	DtReal X[4][4], Y[4][4], Z[4][4];	
	} patch;

typedef struct {
	DtReal X, Y, Z;	
	} point;

patch patches[MAX_PATCHES];		/* patch data		*/
DtInt patchcount = 0;
DtInt partcount = 0;

point Gridpoints[MAX_GRID_POINTS]; /* grid point data */

DtObject object;
DtObject patchobj[2000];

reset_objects() {} 



	makeobjects()
	{
    	create_foodpros_patches();
	create_checkerb_counter();
	}


	create_foodpros_patches()
	{
	FILE *fp, *fopen();
	int IT, id, IV, KC, N1, N2, N3, N4, N5;
	int i;

	/*  Scan the file for patches, creating each one as it is read in.  */

	if (NULL == (fp = fopen("foodpros.dat","r")))
		{
		fprintf(stderr,"couldn't open read file '%s'\n","foodpros.dat");
		exit(1);
		}

	printf("scanning data file for patches:\n");

	while(fscanf(fp,"%d%d%d%d%d%d%d%d%d",&IT,&id,&IV,&KC,&N1,&N2,&N3,&N4,&N5))
		{
		switch(IT)
			{
			case 33:
				makepatch(id,fp);
				break;

			case 99: break;

			/*  Ignore all other types.  */

			default:
				for(i=0; i<KC; i++)
					discardline(fp);
			}

		if (IT == 99)
			break;

		discardline(fp);
		}

	printf("\tcreated %d patches.\n",patchcount);

	fclose(fp);

	/*  Scan the file for parts, creating each one as it is read in from the
	already created patches.  */

	if (NULL == (fp = fopen("foodpros.dat","r")))
		{
		fprintf(stderr,"couldn't open read file '%s'\n","foodpros.dat");
		exit(1);
		}

	printf("scanning data file for parts:\n");

	while(fscanf(fp,"%d%d%d%d%d%d%d%d%d",&IT,&id,&IV,&KC,&N1,&N2,&N3,&N4,&N5))
		{
		switch(IT)
			{
			case 21:
				makepart(fp,IV);
				break;

			/*  Don't need gridpoints now.  */

			/*
			case 31:
				fscanf(fp,"%f%f%f",
					&Gridpoints[id].X,&Gridpoints[id].Y,&Gridpoints[id].Z);
				break;
			*/

			case 99: break;

			/*  Ignore all other types.  */

			default:
				for(i=0; i<KC; i++)
					discardline(fp);
			}

		if (IT == 99)
			break;

		discardline(fp);
		}

	printf("\tcreated %d parts.\n",partcount);

	fclose(fp);
	}


	discardline(fp)
	FILE *fp;

	{
	char c;

	while(! ((c=fgetc(fp) == '\n') || (c==EOF)));
	}


	makepart(fp,numbercount)
	FILE *fp;
	int numbercount;

	{
	char partname[100];
	int subpartcount;
	int subpart;
	int subparttype;
	int subpartno;

	subpartcount = numbercount / 2;

	fscanf(fp,"%s",partname);

	if (strcmp(partname,"TEMP") == 0)
		{
		/*
		printf("Part:  skipping part 'TEMP'\n");
		*/
		return;
		}
	else
		{
		/*
		printf("Part:  constructing part '%s' from %d subparts\n",partname,
			subpartcount);
		*/
		}

	object = DsInqObj(DcNameString,partname,DcTypeGroup);

	if (object != DcNullObject)
		{
		printf("create part:  part %s already exists\n",partname);

		return;
		}

	/*  Create and open the new group.  */

	object = DoGroup(DcTrue);

	/*  Assign the new group the correct name.  */

	DsSetObjName(object,DcNameString,partname,DcFalse);

	for (subpart=1;subpart<=subpartcount;subpart++)
		{
		fscanf(fp,"%d %d",&subparttype,&subpartno);

		if (subparttype != 3)
			continue;

		DgAddObj(patchobj[subpartno]);
		}

/*
	printf("\n");
*/

	partcount++;

	DgClose();
	}


	makepatch(patchno,fp)
	int patchno;
	FILE *fp;

	{
	DtInt i, j;
	DtNPoint3 cpt[4][4];
	extern DtObject DcBezier4;
	DtObject DoPatch();
	double x;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++) {
		   fscanf (fp, "%lf", &x);
		   patches[patchcount].X[j][i] = x;
		}
	for(i=0; i<4; i++)
		for(j=0; j<4; j++) {
		   fscanf (fp, "%lf", &x);
		   patches[patchcount].Y[j][i] = x;
		}
	for(i=0; i<4; i++)
		for(j=0; j<4; j++) {
		   fscanf (fp, "%lf", &x);
		   patches[patchcount].Z[j][i] = x;
		}
	HermiteToBezier(&patches[patchcount]);

	for (i=0;i<4;i++)
		{
		for (j=0;j<4;j++)
			{
			cpt[i][j].x = patches[patchcount].X[i][j];
			cpt[i][j].y = patches[patchcount].Y[i][j];
			cpt[i][j].z = patches[patchcount].Z[i][j];
			}
		}

	patchobj[patchno] = DoPatch(DcRGB,DcCtr,DcBezier4,cpt,DcBezier4);

	patchcount++;
	}

	HermiteToBezier(p)
	patch *p;

	{
	patch Tmp;
	DtInt i, j;
	static DtReal point3 = 1. / 3.;
	
	/* calculate second and third rows. Store in temp patch p */

	Tmp.X[1][0] = p->X[0][0] + p->X[2][0]*point3;
	Tmp.X[1][1] = p->X[0][1] + p->X[2][1]*point3;
	Tmp.X[1][2] = p->X[0][2] + p->X[2][2]*point3;
	Tmp.X[1][3] = p->X[0][3] + p->X[2][3]*point3;

	Tmp.X[2][0] = p->X[1][0] - p->X[3][0]*point3;
	Tmp.X[2][1] = p->X[1][1] - p->X[3][1]*point3;
	Tmp.X[2][2] = p->X[1][2] - p->X[3][2]*point3;
	Tmp.X[2][3] = p->X[1][3] - p->X[3][3]*point3;


	Tmp.Y[1][0] = p->Y[0][0] + p->Y[2][0]*point3;
	Tmp.Y[1][1] = p->Y[0][1] + p->Y[2][1]*point3;
	Tmp.Y[1][2] = p->Y[0][2] + p->Y[2][2]*point3;
	Tmp.Y[1][3] = p->Y[0][3] + p->Y[2][3]*point3;

	Tmp.Y[2][0] = p->Y[1][0] - p->Y[3][0]*point3;
	Tmp.Y[2][1] = p->Y[1][1] - p->Y[3][1]*point3;
	Tmp.Y[2][2] = p->Y[1][2] - p->Y[3][2]*point3;
	Tmp.Y[2][3] = p->Y[1][3] - p->Y[3][3]*point3;


	Tmp.Z[1][0] = p->Z[0][0] + p->Z[2][0]*point3;
	Tmp.Z[1][1] = p->Z[0][1] + p->Z[2][1]*point3;
	Tmp.Z[1][2] = p->Z[0][2] + p->Z[2][2]*point3;
	Tmp.Z[1][3] = p->Z[0][3] + p->Z[2][3]*point3;

	Tmp.Z[2][0] = p->Z[1][0] - p->Z[3][0]*point3;
	Tmp.Z[2][1] = p->Z[1][1] - p->Z[3][1]*point3;
	Tmp.Z[2][2] = p->Z[1][2] - p->Z[3][2]*point3;
	Tmp.Z[2][3] = p->Z[1][3] - p->Z[3][3]*point3;

	/* copy original second row into fourth row. */ 

	p->X[3][0] = p->X[1][0]; p->Y[3][0] = p->Y[1][0]; p->Z[3][0] = p->Z[1][0];
	p->X[3][1] = p->X[1][1]; p->Y[3][1] = p->Y[1][1]; p->Z[3][1] = p->Z[1][1];
	p->X[3][2] = p->X[1][2]; p->Y[3][2] = p->Y[1][2]; p->Z[3][2] = p->Z[1][2];
	p->X[3][3] = p->X[1][3]; p->Y[3][3] = p->Y[1][3]; p->Z[3][3] = p->Z[1][3];

	/* copy calculated new second and third rows back */

	for(i=1; i<3; i++)
		for(j=0; j<4; j++) 
			{
			p->X[i][j] = Tmp.X[i][j];
			p->Y[i][j] = Tmp.Y[i][j];
			p->Z[i][j] = Tmp.Z[i][j];
			}

	/* calculate second and third columns. Store in temp patch p */

	Tmp.X[0][1] = p->X[0][0] + p->X[0][2]*point3;
	Tmp.X[1][1] = p->X[1][0] + p->X[1][2]*point3;
	Tmp.X[2][1] = p->X[2][0] + p->X[2][2]*point3;
	Tmp.X[3][1] = p->X[3][0] + p->X[3][2]*point3;

	Tmp.X[0][2] = p->X[0][1] - p->X[0][3]*point3;
	Tmp.X[1][2] = p->X[1][1] - p->X[1][3]*point3;
	Tmp.X[2][2] = p->X[2][1] - p->X[2][3]*point3;
	Tmp.X[3][2] = p->X[3][1] - p->X[3][3]*point3;


	Tmp.Y[0][1] = p->Y[0][0] + p->Y[0][2]*point3;
	Tmp.Y[1][1] = p->Y[1][0] + p->Y[1][2]*point3;
	Tmp.Y[2][1] = p->Y[2][0] + p->Y[2][2]*point3;
	Tmp.Y[3][1] = p->Y[3][0] + p->Y[3][2]*point3;

	Tmp.Y[0][2] = p->Y[0][1] - p->Y[0][3]*point3;
	Tmp.Y[1][2] = p->Y[1][1] - p->Y[1][3]*point3;
	Tmp.Y[2][2] = p->Y[2][1] - p->Y[2][3]*point3;
	Tmp.Y[3][2] = p->Y[3][1] - p->Y[3][3]*point3;


	Tmp.Z[0][1] = p->Z[0][0] + p->Z[0][2]*point3;
	Tmp.Z[1][1] = p->Z[1][0] + p->Z[1][2]*point3;
	Tmp.Z[2][1] = p->Z[2][0] + p->Z[2][2]*point3;
	Tmp.Z[3][1] = p->Z[3][0] + p->Z[3][2]*point3;

	Tmp.Z[0][2] = p->Z[0][1] - p->Z[0][3]*point3;
	Tmp.Z[1][2] = p->Z[1][1] - p->Z[1][3]*point3;
	Tmp.Z[2][2] = p->Z[2][1] - p->Z[2][3]*point3;
	Tmp.Z[3][2] = p->Z[3][1] - p->Z[3][3]*point3;


	/* copy second column into fourth column. */ 

	p->X[0][3] = p->X[0][1]; p->Y[0][3] = p->Y[0][1]; p->Z[0][3] = p->Z[0][1];
	p->X[1][3] = p->X[1][1]; p->Y[1][3] = p->Y[1][1]; p->Z[1][3] = p->Z[1][1];
	p->X[2][3] = p->X[2][1]; p->Y[2][3] = p->Y[2][1]; p->Z[2][3] = p->Z[2][1];
	p->X[3][3] = p->X[3][1]; p->Y[3][3] = p->Y[3][1]; p->Z[3][3] = p->Z[3][1];

	/* copy calculated new second and third columns back */

	for(i=1; i<3; i++)
		for(j=0; j<4; j++)
			{
			p->X[j][i] = Tmp.X[j][i];
			p->Y[j][i] = Tmp.Y[j][i];
			p->Z[j][i] = Tmp.Z[j][i];
			}
	}



	printpatch(N)
	int N;

	{
	int j, k;

	putchar('\n');
	for(j=0; j<4; j++)
		{
		for(k=0; k<4; k++)
		printf("%2.2f,%2.2f,%2.2f",
			patches[N].X[j][k],patches[N].Y[j][k],patches[N].Z[j][k]);
		putchar('\n');
		}
	}

	create_checkerb_counter()
	{
	DtObject square, checkerb_counter, make_tiles();
	static DtReal pinkish[]={1.0, 0.8, 0.5};
	static DtReal black[]={0., 0., 0.};
	static DtReal white[]={1., 1., 1.};
	static DtReal square_verts[] =
         {
         0., 0., 0.,
         1., 0., 0.,
         1., 1., 0.,
         0., 1., 0.,
         };

	square = DoGroup(DcTrue);
		DgAddObj(DoPushMatrix());
/***
		DgAddObj(DoDiffuseIntens(.5));
		DgAddObj(DoSpecularSwitch(DcOn));
		DgAddObj(DoSpecularIntens(.5));
		DgAddObj(DoSpecularColor(DcRGB, white));
		DgAddObj(DoSpecularFactor(.5));
**/
		DgAddObj(DoSimplePolygon(DcRGB, DcLoc, 4, square_verts, DcConvex));
		DgAddObj(DoPopMatrix());
	DgClose();
	checkerb_counter =  DoGroup(DcTrue);
		DgAddObj(DoPushMatrix());
		DgAddObj(make_tiles(DcRGB, white, black, square, 10, 10));
		DgAddObj(DoPopMatrix());
	DgClose();
	DsSetObjName(checkerb_counter,DcNameString,"checkerbCounter",DcFalse);
	}
