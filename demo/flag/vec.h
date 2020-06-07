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
 
vecmulc(a,v,n) 
double a,*v;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (v)[qxg]*=(a);
}

vecaddc(a,v,n) 
double a,*v;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (v)[qxg]+=(a);
}

vecsubc(a,v,n) 
double a,*v;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (v)[qxg]-=(a);
}

vecdivc(a,v,n) 
double a,*v;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (v)[qxg]/=(a);
}

veccopyc(a,v,n) 
double a,*v;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (v)[qxg]=(a);
}


vecmul(u,v,n) 
double *u,*v;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (v)[qxg]*=(u)[qxg];
}

vecadd(u,v,n) 
double *u,*v;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (v)[qxg]+=(u)[qxg];
}

vecsub(u,v,n) 
double *u,*v;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (v)[qxg]-=(u)[qxg];
}

vecdiv(u,v,n) 
double *u,*v;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (v)[qxg]/=(u)[qxg];
}

veccopy(u,v,n) 
double *u,*v;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (v)[qxg]=(u)[qxg];
}


vecmulc3(a,v,o,n) 
double a,*v,*o;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (o)[qxg]=(v)[qxg]*(a);
}

vecaddc3(a,v,o,n) 
double a,*v,*o;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (o)[qxg]=(v)[qxg]+(a);
}

vecsubc3(a,v,o,n) 
double a,*v,*o;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (o)[qxg]=(v)[qxg]-(a);
}

vecdivc3(a,v,o,n) 
double a,*v,*o;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (o)[qxg]=(v)[qxg]/(a);
}


vecmul3(u,v,o,n) 
double *u,*v,*o;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (o)[qxg]=(v)[qxg]*(u)[qxg];
}

vecadd3(u,v,o,n) 
double *u,*v,*o;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (o)[qxg]=(v)[qxg]+(u)[qxg];
}

vecsub3(u,v,o,n) 
double *u,*v,*o;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (o)[qxg]=(v)[qxg]-(u)[qxg];
}

vecdiv3(u,v,o,n) 
double *u,*v,*o;
int n;
{
	int qxg;
	
	for (qxg=0; qxg<n; qxg++) (o)[qxg]=(v)[qxg]/(u)[qxg];
}


