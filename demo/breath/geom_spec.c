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

#define NMODELS 1        /* number of discrete models available for display */

int model_type;
int nmodels = NMODELS;
int firstcycle = 0;
int lastcycle = 0;
DtObject models[NMODELS];
extern DtInt debug;
extern int autocycle;
extern int autorotate, autox, autoy, autoz;

#define MoveMax 2.
#define SphereSize 1.414
#define CubeSize .9
#define NFacets 8
#define NTris (NFacets-1)*(NFacets-1)*2
#define NVerts NFacets*NFacets
#define FacetSize 2.*CubeSize/(NFacets-1.)
#define NCubes 20
#define CLim (MoveMax+SphereSize)

int cube_enable = 1;
int cube_state = 0;
int cube_step = 1;
int cube_lolim = 0;
int cube_hilim = 19;

int bounce_enable = 1;
int cage_enable = 1;

void cube_callback();
DtObject cubes[NCubes];
DtObject cage;

DtReal xpos, ypos, zpos, dx, dy, dz;

static int tottri = 0;

geom_spec(bounds)
	DtVolume *bounds;
{
	srand48(time(0));
	make_cube();
	make_cage();

	for (model_type=0; model_type<nmodels; model_type++)
		DsHoldObj( models[model_type] =
			(DtObject)DoCallback(cube_callback, DcNullObject) );

	xpos = ypos = zpos = 0.;
	dx = (drand48()*2. - 1.) * .5;
	dy = (drand48()*2. - 1.) * .5;
	dz = (drand48()*2. - 1.) * .5;

	bounds->bll[0] = bounds->bll[1] = bounds->bll[2] = -CLim;
	bounds->fur[0] = bounds->fur[1] = bounds->fur[2] =  CLim;

	autorotate = autox = autoy = autoz = 1;
}

make_cage()
{
	DtObject DoLineList();

	static DtReal verts[12][2][3] = {
		{ { -CLim, -CLim, -CLim }, { -CLim, -CLim,  CLim } },
		{ { -CLim, -CLim,  CLim }, { -CLim,  CLim,  CLim } },
		{ { -CLim,  CLim,  CLim }, { -CLim,  CLim, -CLim } },
		{ { -CLim,  CLim, -CLim }, { -CLim, -CLim, -CLim } },
		{ {  CLim, -CLim, -CLim }, {  CLim, -CLim,  CLim } },
		{ {  CLim, -CLim,  CLim }, {  CLim,  CLim,  CLim } },
		{ {  CLim,  CLim,  CLim }, {  CLim,  CLim, -CLim } },
		{ {  CLim,  CLim, -CLim }, {  CLim, -CLim, -CLim } },
		{ { -CLim, -CLim, -CLim }, {  CLim, -CLim, -CLim } },
		{ { -CLim, -CLim,  CLim }, {  CLim, -CLim,  CLim } },
		{ { -CLim,  CLim, -CLim }, {  CLim,  CLim, -CLim } },
		{ { -CLim,  CLim,  CLim }, {  CLim,  CLim,  CLim } }
	};
	cage = DoLineList(DcRGB, DcLoc, 12, verts);
	DsHoldObj(cage);
}

make_cube()
{
	int c, i, j, k;
	float ix, iy;
	float len;
	DtReal cface[NFacets][NFacets][6], sface[NFacets][NFacets][6];
	DtReal face[NFacets][NFacets][6];
	DtReal tris[3*NTris][9];
	int t;
	float ic, dc, cfrac, sfrac;

	/* make cube and sphere faces */
	ix = -CubeSize;
	for (i=0; i<NFacets; i++) {
		iy = -CubeSize;
		for (j=0; j<NFacets; j++) {
			cface[i][j][0] = ix;
			cface[i][j][1] = iy;
			cface[i][j][2] = CubeSize;
			cface[i][j][3] = 0.;
			cface[i][j][4] = 0.;
			cface[i][j][5] = 1.;

			len = sqrt(ix*ix+iy*iy+CubeSize*CubeSize);
			sface[i][j][0] = SphereSize*ix/len;
			sface[i][j][1] = SphereSize*iy/len;
			sface[i][j][2] = SphereSize*CubeSize/len;
			sface[i][j][3] = ix/len;
			sface[i][j][4] = iy/len;
			sface[i][j][5] = CubeSize/len;

			iy += FacetSize;
		}
		ix += FacetSize;
	}

	/* make cube objects */
	ic = -3.1415927/2.;
	dc = 3.1415927/(NCubes-1);
	for (c=0; c<NCubes; c++) {

		sfrac = (sin(ic)+1.)/2.;
		sfrac = sfrac*1.5-.5;
		cfrac = 1. - sfrac;
		for (i=0; i<NFacets; i++) {
			for (j=0; j<NFacets; j++) {
				for (k=0; k<6; k++)
					face[i][j][k] = cface[i][j][k]*cfrac +
							sface[i][j][k]*sfrac;
				len = sqrt(face[i][j][3]*face[i][j][3] +
					   face[i][j][4]*face[i][j][4] +
					   face[i][j][5]*face[i][j][5]);
				face[i][j][3] /= len;
				face[i][j][4] /= len;
				face[i][j][5] /= len;
			}
		}

		t = 0;
		for (i=0; i<NFacets-1; i++) {
			for (j=0; j<NFacets-1; j++) {
				for (k=0; k<3; k++) {
					tris[t+k][6] = 1.;
					tris[t+k][7] = ((i+j)&1 ? 1. : 0.);
					tris[t+k][8] = 0.;
				}
				for (k=0; k<6; k++)
					tris[t][k] = face[i][j][k];
				t++;
				for (k=0; k<6; k++)
					tris[t][k] = face[i+1][j][k];
				t++;
				for (k=0; k<6; k++)
					tris[t][k] = face[i][j+1][k];
				t++;

				for (k=0; k<3; k++) {
					tris[t+k][6] = 1.;
					tris[t+k][7] = ((i+j)&1 ? 0. : 1.);
					tris[t+k][8] = 0.;
				}
				for (k=0; k<6; k++)
					tris[t][k] = face[i+1][j][k];
				t++;
				for (k=0; k<6; k++)
					tris[t][k] = face[i+1][j+1][k];
				t++;
				for (k=0; k<6; k++)
					tris[t][k] = face[i][j+1][k];
				t++;

			}
		}

		cubes[c] = (DtObject)DoGroup(DcTrue);

			DgAddObj(DoTriangleList(DcRGB, DcLocNrmClr, NTris, tris));

			DgAddObj(DoPushMatrix());
				DgAddObj(DoRotate(DcZAxis, 3.1415927));
				DgAddObj(DoRotate(DcXAxis, 3.1415927/2.));
				DgAddObj(DoTriangleList(DcRGB, DcLocNrmClr, NTris, tris));
			DgAddObj(DoPopMatrix());

			DgAddObj(DoPushMatrix());
				DgAddObj(DoRotate(DcZAxis, 3.1415927/2.));
				DgAddObj(DoRotate(DcXAxis, -3.1415927/2.));
				DgAddObj(DoTriangleList(DcRGB, DcLocNrmClr, NTris, tris));
			DgAddObj(DoPopMatrix());

			DgAddObj(DoPushMatrix());
				DgAddObj(DoRotate(DcZAxis, -3.1415927/2.));
				DgAddObj(DoRotate(DcYAxis, 3.1415927/2.));
				DgAddObj(DoTriangleList(DcRGB, DcLocNrmClr, NTris, tris));
			DgAddObj(DoPopMatrix());

			DgAddObj(DoPushMatrix());
				DgAddObj(DoRotate(DcZAxis, 3.1415927));
				DgAddObj(DoRotate(DcYAxis, -3.1415927/2.));
				DgAddObj(DoTriangleList(DcRGB, DcLocNrmClr, NTris, tris));
			DgAddObj(DoPopMatrix());

			DgAddObj(DoPushMatrix());
				DgAddObj(DoRotate(DcZAxis, 3.1415927/2.));
				DgAddObj(DoRotate(DcYAxis, 3.1415927));
				DgAddObj(DoTriangleList(DcRGB, DcLocNrmClr, NTris, tris));
			DgAddObj(DoPopMatrix());

		DgClose();
		DsHoldObj(cubes[c]);

		tottri += 6 * NTris;
		
		ic += dc;
	}

	printf("Model contains %d triangles\n",tottri);
}

void cube_callback()
{
	autocycle = 1;

	DsExecuteObj(DoBackfaceCullSwitch(DcTrue));
	if (cage_enable)
		DsExecuteObj(cage);
	DsExecuteObj(DoTranslate(xpos, ypos, zpos));
	DsExecuteObj(cubes[cube_state]);

	if (autocycle) {

		cube_state += cube_step;

		if (cube_state < cube_lolim) {
			cube_state = cube_lolim;
			cube_step = 0 - cube_step;
		}
		else if (cube_state > cube_hilim) {
			cube_state = cube_hilim;
			cube_step = 0 - cube_step;
		}
	}

	if (bounce_enable) {

		xpos += dx;
		if (xpos < -MoveMax) { xpos = -MoveMax; dx *= -1.; }
		if (xpos >  MoveMax) { xpos =  MoveMax; dx *= -1.; }
		ypos += dy;
		if (ypos < -MoveMax) { ypos = -MoveMax; dy *= -1.; }
		if (ypos >  MoveMax) { ypos =  MoveMax; dy *= -1.; }
		zpos += dz;
		if (zpos < -MoveMax) { zpos = -MoveMax; dz *= -1.; }
		if (zpos >  MoveMax) { zpos =  MoveMax; dz *= -1.; }
	}
}
