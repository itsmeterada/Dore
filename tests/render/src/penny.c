/* ************************************************************************
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
#include "dore_proto.h"
#include "stdio.h"
#include <math.h>

#define IK 1
#define IJ 2
#define invert 1
#define noinvert 0
#define both 2
#define single 1
#define IMAX 512 
#define JMAX 512 
#define VERTMAX IMAX * JMAX 
#define TRIMAX  VERTMAX * 2 

FILE *fopen(), *fp, *fpout;
double z[IMAX+1][JMAX+1];
DtPoint3 vert0,vert1;

makeobjects()

{
   DtInt skipvalue;

   printf("enter skipvalue:  ");
   scanf("%d",&skipvalue);

   if ((fp = fopen("penny.data","r")) == NULL) {
      printf("Can't open penny.data\n");
      exit(1);
   }

   DuOpenGroupByName("penny");

   input(1,IMAX,1,JMAX); 
   maketriangles(1,IMAX,1,JMAX,skipvalue,invert);
	
   DgClose();

   fclose(fp);
}
/*
  ======================================================================
 */
maketriangles(i1,i2,j1,j2,skipvalue,side)
     int i1,i2,j1,j2,skipvalue,side;
{
   static DtPoint3 vertices[VERTMAX];
   static DtInt trilist[TRIMAX][3],temp;
   int n,width,i,j,i2prime,j2prime;
   float igridsize, jgridsize;
   DtInt vertexcount,trianglecount;

   igridsize = .01;
   jgridsize = .01;
   printf("Making triangles\n");
   n = 0;
   for (j = j1; j <= j2; j = j + skipvalue)
	 for (i = i1; i <= i2; i = i + skipvalue) {
	    vertices[n][0] = i * igridsize - IMAX*igridsize/2.0;
	    vertices[n][1] = j * jgridsize - JMAX*igridsize/2.0;
	    vertices[n][2] = z[i][j];
#ifdef DEBUG
	    printf("%4d  %4d  %7.3f %7.3f %7.3f\n",i,j,
		   vertices[n][0],
		   vertices[n][1], vertices[n][2]);
#endif
	    n++;
	 }
   vertexcount = n;
   printf("vertexcount: %d\n",vertexcount);
	
   n = 0;
   j2prime = 0;
   i2prime = 0;
   for (j = j1; j <= j2; j = j + skipvalue) i2prime++;
   for (i = i1; i <= i2; i = i + skipvalue) j2prime++;
		
   width = i2prime - i1 + 1;
   for (j = j1; j <  j2prime; j++)
	 for (i = i1; i <  i2prime; i++) {
	    trilist[n][0] = (j-j1) * width + (i-i1);
	    trilist[n][1] = trilist[n][0] + width + 1;
	    trilist[n][2] = trilist[n][0] + width ;
	    trilist[n+1][0] = trilist[n][0];
	    trilist[n+1][1] = trilist[n][0] + 1;
	    trilist[n+1][2] = trilist[n][1]; 
#ifdef DEBUG
	    printf("%4d %4d %4d %4d %4d %4d\n",
		   trilist[n][0],trilist[n][1],trilist[n][2],
		   trilist[n+1][0],trilist[n+1][1],
		   trilist[n+1][2]); 
#endif
	    n = n + 2;
	 }
	

   trianglecount = n;
   printf("trianglecount: %d\n",trianglecount);

   if (side == invert) 
	 for (i=0; i<=trianglecount; i++) {
	    temp = trilist[i][2];
	    trilist[i][2] = trilist[i][1];
	    trilist[i][1] = temp;
	 }

#ifndef stellar
   DgAddObj(DoVarTriangleMesh(DcRGB,vertexcount,vertices,DcNullPtr,
			      DcNullPtr,trianglecount,trilist,DcTrue));
#else
   DgAddObj(DoTriangleMesh(DcRGB,DcLoc,vertexcount,vertices,trianglecount,
			   trilist,DcTrue));
#endif
}
/*
  ======================================================================
 */
input(i1,i2,j1,j2)
     int i1,i2,j1,j2;
{
   int i,j;
   unsigned char inval;
   double q;

#ifdef DEBUG
   printf ("input  %3d %3d %3d %3d\n", i1,i2,j1,j2);
#endif
   for (j = j1; j <= j2; j++)
	 for (i = i1; i <= i2; i++) {
	    fscanf(fp,"%1c", &inval);
	    q = (double)inval * .002;
	    z[i][j] =  -q;
#ifdef DEBUG
	    printf("%lf\n",z[i][j]);
#endif
	 }
}
/*
  ======================================================================
 */
DuOpenGroupByName(groupname)
     char *groupname;

{
   DtObject groupobj;

   groupobj = DsInqObj(DcNameString,groupname,DcTypeGroup);

   if (groupobj == DcNullObject) {
#ifdef DEBUG
      printf("creating group %s\n",groupname);
#endif
      groupobj = DoGroup(DcTrue);

      DsSetObjName(groupobj,DcNameString,groupname,DcFalse);
   } else {
#ifdef DEBUG
      printf("opening group %s\n",groupname);
#endif
      DgOpen(groupobj, DcFalse);
   }
}
