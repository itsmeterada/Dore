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

#define DataFile "Lpack.dat"

#define RED	.0
#define GREEN	.8
#define BLUE	.6

#define NMODELS 1
int nmodels = NMODELS;
int firstcycle = 0;
int lastcycle = 0;
	DtObject DoGroup();
	DtObject models[NMODELS];
	extern DtInt debug;

#define M 12
#define N 10
#define NV 3*N*N+4*N+1
#define NT 6*N*N
#define PI 3.1415926535897932384626433

	static DtReal V[NV][6],a[M],a0[M],mu[M];
	static DtInt T[NT][3];
	DtReal L1[NV],L2[NV],L3[NV],L4[NV],L5[NV],L6[NV];
	DtReal L7[NV],L8[NV],L9[NV],L10[NV],L11[NV],L12[NV];

	DtInt autocycle;
double dt = .05;
double fmode, height, phase;

void dothing();

geom_spec()
{
		static DtReal color[3] = { 1., 1., 1. };

	makething();

		models[0] = DoGroup(DcTrue);

			DgAddObj(DoDiffuseColor(DcRGB, color));
			DgAddObj(DoDiffuseIntens(.9));
			DgAddObj(DoSpecularColor(DcRGB, color));
			DgAddObj(DoSpecularFactor(100.));
			DgAddObj(DoSpecularIntens(1.));

			DgAddObj(DoRotate(DcXAxis, -PI*.4));
			DgAddObj(DoRotate(DcZAxis, -PI*.75));
			DgAddObj(DoCallback(dothing, DcNullObject));

		DgClose();
		DsHoldObj(models[0]);
}


makething() 
{
	FILE *fp;
	DtInt i,j,k,l,inc,jt;

	if ((fp = fopen(DataFile, "r")) == NULL) {
		perror(DataFile);
	exit(1);
	}

	loaddat(fp,M,mu);
	loaddat(fp,M,a0);
	loaddat(fp,NV,L1);
	loaddat(fp,NV,L2);
	loaddat(fp,NV,L3);
	loaddat(fp,NV,L4);
	loaddat(fp,NV,L5);
	loaddat(fp,NV,L6);
	loaddat(fp,NV,L7);
	loaddat(fp,NV,L8);
	loaddat(fp,NV,L9);
	loaddat(fp,NV,L10);
	loaddat(fp,NV,L11);
	loaddat(fp,NV,L12);

	printf("%d Vertices, %d Triangles\n",NV,NT);

	for (i=0; i<M; i++)
		a[i] = a0[i];

	k = 0;
	l = 0;
	for (i = -N; i<=N; i++) {
		jt = (i<=0 ? N : 0);
		inc = jt+N+1;
		for (j = -N; j<=jt; j++) {
				V[k][0] = (DtReal)i/(DtReal)N;
				V[k][1] = (DtReal)j/(DtReal)N;
			V[k][2] = 0.;
			V[k][3] = RED;
			V[k][4] = GREEN;
			V[k][5] = BLUE;
			if ((i < N) && (j < jt) && (!(i==0 && j>=0)) ) {
				T[l][0] = k;
				T[l][1] = k+inc;
				T[l][2] = k+inc+1;
				l++;
				T[l][0] = k;
				T[l][1] = k+inc+1;
				T[l][2] = k+1;
				l++;
			}
		k++;
		}
	}

}


void dothing()
{
	double c[M];
	static double t = 0.;
	int i,k,mode;

	if (!autocycle) {
	   if (t!=0) {
	       t = 0;
	       for (i=0; i<M; i++)
		   a[i] = 0;
	   }
	   mode = fmode;
	   a[mode-1] = height;
	}
	
	for (i=0; i<M; i++)
		c[i] = a[i]*cos(mu[i]*t);
	
	for (k=0; k<NV; k++) 
	   V[k][2] = 
		c[0]*L1[k] + c[1]*L2[k] + c[2]*L3[k] + c[3]*L4[k] + 
		c[4]*L5[k] + c[5]*L6[k] + c[6]*L7[k] + c[7]*L8[k] + 
		c[8]*L9[k] + c[9]*L10[k] + c[10]*L11[k] + c[11]*L12[k];

		DsExecuteObj(DoTriangleMesh(DcRGB, DcLocClr, NV, V, NT, T, DcTrue));

	/* current value of 'dt' is whatever was read in last */
	if (autocycle)
		t += dt;
}

loaddat(fp,n,A)
FILE *fp;
int n;
DtReal A[];
{
	int i;
	char skipname[3];
	double tmp;

	fscanf(fp,"%s ",skipname);
	for (i=0; i < n; i++) {
	    fscanf(fp,"%lf",&tmp);
	    A[i] = tmp;
	}
}
