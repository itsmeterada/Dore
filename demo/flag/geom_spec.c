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
 
#include <stdio.h>
#include <math.h>
#include <dore/dore.h>
#include "vec.h"

/*
 * Number of steps between graphing:
 */
#define skip 6

/*
 * Search for string 'NOTE' for efficiency notes.
 *
 * NOTE: Many divs can be omitted on horizontal and
 * vertical (but not diagonal) links if we assume a tight 
 * enough system that 'length d' will always be close to 1.
 */

/*
 * Dimensions of flag
 */
#define sizex 20
#define sizey 12

/*
 * Weight of a point is always 1. !!!
 * Prefered length of horizontal or vertical spring is always 1. !!!
 * Strength of springs: ( force = strength * displacement )
 * Friction: ( force = fric * vel )
 * Number of seconds per sim step:
 */
#define timestep (1./10.)
#define strength 20.
#define fric 1.
/* NOTE: statements involving fric have been
 * commented out on the assumption that fric is 1.!!!!
 */

/*
 * Gravity
 */
#define G 0.05

/*
 * Resolution of the squaroot lookup table.
 */
#define sqrtres 4096

/*
 * Arrays are indexed with 0 in the 'lower left'.
 * X increments by 1, y increments by sizex.
 * (Scanline order, bottom to top.)
 */
#define len1 (sizex*sizey)
#define len3 (len1*3)
#define xoff 0
#define yoff len1
#define zoff (len1*2)
#define veci(lim) for (i=0; i < (lim); i++)

	/*
	 * Point location, velocity, force (= acceleration, as mass is 1.).
	 */
	double pxyz[len3];
	double vxyz[len3];
	double fxyz[len3];

	/*
	 * Temporary variables.
	 */
	double dxyz[len3];
	double txyz[len3];

	double ld[len1];	/* Length of 'd'	*/
	double sf[len1];	/* Spring force		*/
	double ff[len1];	/* Friction force.	*/

	/*
	 * The assumption is that the point displacement
	 * from spring neutral (a distance of 1.) will
	 * never exceed 1.
	 * Therefore, the sqrt must produce values from 0
	 * to 2., leaving an input range from
	 * 0 to 4.
	 *
	 * So, sqrt(r) = sqrtl[(int)(r*((sqrtres-1)/4.))]
	 *             = sqrtl[(int)(r*sqrtscale)]
	 *
	 * NOTE: with the addition of diagonal connections,
	 * the range should be increased to accomodate the
	 * added length.
	 */
	double sqrtl[sqrtres];

/*
 * The divisor here should be greater than the square of the
 * greatest stretched connection length expected.
 */
#define sqrtscale (sqrtres/8.)

DtReal Vertices[sizex][sizey][6];
DtReal oldVert[sizex][sizey][3];
DtInt  Triangles[len3][3];
DtInt nTris;

#define NMODELS 1        /* number of discrete models available for display */

int model_type;
int nmodels = NMODELS;
int firstcycle = 0;
int lastcycle = 0;
DtObject models[NMODELS];
extern DtInt debug;
extern int autocycle;

DtSwitch corner1 = DcTrue, corner2 = DcTrue;
int framenumber = 0;

int flagstep;
int updateflag();

geom_spec()
{
	DtObject DoGroup();
	DtReal color[3];
	DtReal subdivval = .4;
	
	init_sqrt();
	init_flag();
	init_wind();
	init_tris();

	color[0]=.9;
	color[1]=0.;
	color[2]=.2;
	models[0]=DoGroup(DcTrue);
		DgAddObj(DoDiffuseColor(DcRGB,color));	
		DgAddObj(DoDiffuseIntens(10.0));	
		DgAddObj(DoSubDivSpec(DcSubDivRelative, &subdivval));
		DgAddObj(DoPushMatrix());
			DgAddObj(DoRotate(DcXAxis,-3.14159/2.0));
			DgAddObj(DoTranslate(0.,0.,-10.));
			DgAddObj(DoScale(.2,.2,22.));
			DgAddObj(DoPrimSurf(DcCylinder));	
		DgAddObj(DoPopMatrix());
		color[0]=.3;
		color[1]=.3;
		color[2]=.8;
		DgAddObj(DoDiffuseColor(DcRGB,color));	
		DgAddObj(DoDiffuseIntens(1.0));	
		DgAddObj(DoPushMatrix());
			DgAddObj(DoTranslate(0.,12.,0.));
			DgAddObj(DoScale(.4,.4,.4));
			DgAddObj(DoPrimSurf(DcSphere));	
		DgAddObj(DoPopMatrix());
		DgAddObj(DoDiffuseIntens(1.0));	
		DgAddObj(DoCallback(updateflag,DcNullObject));
	DgClose();
	DsHoldObj(models[0]);


}

updateflag()
{
	int i;

	if (autocycle) {
		for (i=0; i<skip; i++) {
			forceflag();
			externalforces();
			moveflag();
		}
	}
	drawflag();
}

forceflag()
{

	/*
	 * Start with 0 force.
	 */
	veccopyc(0.,fxyz,len3);
	
	/*
	 * Essentially, we are calculating
	 *
	 * (strength * (l-1.)) + (fric * (v dot d)/l)
	 * ------------------------------------------ * d
	 *                     l
	 *
	 * where d is the vector between the
	 * two points, l is the length of d,
	 * and v is the difference between their
	 * velocities.  This handles spring force Fs = -k(x-x0),
	 * and friction force Ff = -Rv, where k = strength,
	 * R = fric, and x0 = 1.
	 *
	 * Each of the following functions is specialized
	 * to calculate the indicated forces by the above
	 * method in as vectorizable a fassion as possible.
	 */

	force_horizontal();
	force_vertical();
	force_diagonal();
}

force_horizontal()
{
	int i;

	vecsub3(pxyz,pxyz+1,dxyz,len3-1);/* d = p1-p (vec from p to p1)	*/
	vecmul3(dxyz,dxyz,txyz,len3-1);	 /* t = d*d			*/
	vecadd(txyz+yoff,txyz,len1-1);	 /* tx += ty			*/
	vecadd(txyz+zoff,txyz,len1-1);	 /* tx += tz (tx=(length d)^2)	*/

	/*
	 * No interaction between ends of flag:
	 */
	for (i=sizex-1; i<len1; i+=sizex)
		txyz[i] = 1.;

	vecmulc(sqrtscale,txyz,len1-1);	/* t *= sqrtscale		*/

	veci(len1-1) {			/* ld = length(d)		*/
		if ((int)txyz[i] >= sqrtres) {
		    printf("force_horizontal: (int)txyz[%d] = %d\n",
							i, (int)txyz[i]);
		    txyz[i] = sqrtres - 1;
		}
		ld[i] = sqrtl[(int)txyz[i]];
	}

	/*
	 * Now, ld = length(d), and we are done with txyz.
	 */

	/*
	 * Using ld, find spring force numberator.  Leave
	 * result in sf.
	 */
	veccopy(ld,sf,len1-1);		/* sf = ld			*/
	vecsubc(1.,sf,len1-1);		/* sf = ld-1.			*/
	vecmulc(strength,sf,len1-1);	/* sf = strength * (ld-1.)	*/

	/*
	 * Using ld, d, and v, find friction
	 * force numberator.  Leave result in txyz.
	 */
	vecsub3(vxyz,vxyz+1,txyz,len3-1); /* t = v (rel velocity)	*/
	vecmul(dxyz,txyz,len3-1);	  /* t = v * d			*/
	vecadd(txyz+yoff,txyz,len1-1);	  /* ...			*/
	vecadd(txyz+zoff,txyz,len1-1);	  /* t = v dot d		*/
/*NOTE	vecmulc(fric,txyz,len1-1);	  /* t = fric * (v dot d)	*/
/*NOTE	vecdiv(ld,txyz,len1-1);		  /* t = fric * (v dot d) / l	*/

	/*
	 * Nullify any effects at the wrap around
	 * points. (sides of flag)
	 */
	for (i=sizex-1; i<len1; i+=sizex)
		txyz[i] = 0.;

	/*
	 * Add forces together...
	 */
	vecadd(txyz,sf,len1-1);

/*NOTE	vecdiv(ld,sf,len1-1);		/* sf = (lots of stuff)/ld	*/

	vecmul(sf,dxyz+xoff,len1-1);	/* d  = force vector.		*/
	vecmul(sf,dxyz+yoff,len1-1);	/* d  = force vector.		*/
	vecmul(sf,dxyz+zoff,len1-1);	/* d  = force vector.		*/

	/*
	 * Remember, when going from a len1 to
	 * a len3, we must pay special attention to
	 * what happens at the wrap around points.
	 */
	
	dxyz[len1-1] = 0.;
	dxyz[yoff+len1-1] = 0.;

	vecadd(dxyz,fxyz,len3-1);	/* Force pulling right		*/
	vecsub(dxyz,fxyz+1,len3-1);	/* Force pulling left		*/
}

/*
 * See force_horizontal for in code comments.
 */
force_vertical()
{
	int i;

	vecsub3(pxyz,pxyz+sizex,dxyz,len3-sizex);
	vecmul3(dxyz,dxyz,txyz,len3-sizex);
	vecadd(txyz+yoff,txyz,len1-sizex);
	vecadd(txyz+zoff,txyz,len1-sizex);
	vecmulc(sqrtscale,txyz,len1-sizex);	/* These two lines just set */
	veci(len1-sizex) {			/* ld = sqrt(txyz)   -NOTE- */
		if ((int)txyz[i] >= sqrtres) {
		    printf("force_vertical: (int)txyz[%d] = %d\n",
							i, (int)txyz[i]);
		    txyz[i] = sqrtres - 1;
		}
		ld[i] = sqrtl[(int)txyz[i]];
	}
	veccopy(ld,sf,len1-sizex);
	vecsubc(1.,sf,len1-sizex);
	vecmulc(strength,sf,len1-sizex);
	vecsub3(vxyz,vxyz+sizex,txyz,len3-sizex);
	vecmul(dxyz,txyz,len3-sizex);
	vecadd(txyz+yoff,txyz,len1-sizex);
	vecadd(txyz+zoff,txyz,len1-sizex);
/*NOTE	vecmulc(fric,txyz,len1-sizex); */
/*NOTE	vecdiv(ld,txyz,len1-sizex);		/* NOTE: divide by 1. */
	vecadd(txyz,sf,len1-sizex);
/*NOTE	vecdiv(ld,sf,len1-sizex);		/* NOTE: divide by 1. */
	vecmul(sf,dxyz+xoff,len1-sizex);
	vecmul(sf,dxyz+yoff,len1-sizex);
	vecmul(sf,dxyz+zoff,len1-sizex);
	veccopyc(0.,dxyz+(len1-sizex),sizex);
	veccopyc(0.,dxyz+(yoff+(len1-sizex)),sizex);
	vecadd(dxyz,fxyz,len3-sizex);
	vecsub(dxyz,fxyz+sizex,len3-sizex);
}

/*
 * Length of a diagonal is NOT 1.
 */
#define dlen 1.414214

/*
 * See force_horizontal for in code comments.
 */
force_diagonal()
{
	int i;

	vecsub3(pxyz,pxyz+(sizex+1),dxyz,len3-(sizex+1));
	vecmul3(dxyz,dxyz,txyz,len3-(sizex+1));
	vecadd(txyz+yoff,txyz,len1-(sizex+1));
	vecadd(txyz+zoff,txyz,len1-(sizex+1));
	for (i=sizex-1; i<len1-(sizex+1); i+=sizex)
		txyz[i] = 1.;			/* Anything safe for sqrt */
	vecmulc(sqrtscale,txyz,len1-(sizex+1));	/* These two lines just set */
	veci(len1-(sizex+1)) {			/* ld = sqrt(txyz)   -NOTE- */
		if ((int)txyz[i] >= sqrtres) {
		    printf("force_diagonal: (int)txyz[%d] = %d\n",
							i, (int)txyz[i]);
		    txyz[i] = sqrtres - 1;
		}
		ld[i] = sqrtl[(int)txyz[i]];
	}
	veccopy(ld,sf,len1-(sizex+1));
	vecsubc(dlen,sf,len1-(sizex+1));
	vecmulc(strength,sf,len1-(sizex+1));
	vecsub3(vxyz,vxyz+(sizex+1),txyz,len3-(sizex+1));
	vecmul(dxyz,txyz,len3-(sizex+1));
	vecadd(txyz+yoff,txyz,len1-(sizex+1));
	vecadd(txyz+zoff,txyz,len1-(sizex+1));
/*NOTE	vecmulc(fric,txyz,len1-(sizex+1)); */
/*NOTE	vecdiv(ld,txyz,len1-(sizex+1));		/* NOTE: divide by dlen */
/*NOTE*/vecmulc((1./dlen),txyz,len1-(sizex+1));	/* NOTE: Substitute for above*/
	vecadd(txyz,sf,len1-(sizex+1));
/*NOTE	vecdiv(ld,sf,len1-(sizex+1));		/* NOTE: divide by dlen */
/*NOTE*/vecmulc((1./dlen),sf,len1-(sizex+1));	/* NOTE: Substitute for above*/
	vecmul(sf,dxyz+xoff,len1-(sizex+1));
	vecmul(sf,dxyz+yoff,len1-(sizex+1));
	vecmul(sf,dxyz+zoff,len1-(sizex+1));
	veccopyc(0.,dxyz+(len1-sizex),sizex-1);
	veccopyc(0.,dxyz+(yoff+(len1-sizex)),sizex-1);
	for (i=sizex-1; i<len3-(sizex+1); i+=sizex)
		dxyz[i] = 0.;
	vecadd(dxyz,fxyz,len3-(sizex+1));
	vecsub(dxyz,fxyz+(sizex+1),len3-(sizex+1));
}

externalforces()
{

	/*
	 * Gravity
	 */
	vecaddc((-G),fxyz+yoff,len1);

	/*
	 * Wind!!!
	 */
	force_wind();

	/*
	 * Zero total force on attatchment points:
	 *
	 * To attatch all pole-side points:
	 *
	 * for (i=0; i<len3; i+=sizex)
	 *	fxyz[i] = 0.;
	 *
	 * To attatch only the corners:
	 */
	if(corner2 == DcTrue) {
		fxyz[0] = 0.;
		fxyz[yoff] = 0.;
		fxyz[zoff] = 0.;
	}
	if(corner1 == DcTrue) {
		fxyz[0+(sizex*(sizey-1))] = 0.;
		fxyz[yoff+(sizex*(sizey-1))] = 0.;
		fxyz[zoff+(sizex*(sizey-1))] = 0.; 
	}
}
/*
 * This takes the cross product of the connection
 * to the right with the connection upward, to get
 * the 'normal' of that node, which is then dotted
 * with the relative wind.  (Relative wind is the
 * absolute wind less the motion of that node.)
 * That dot product is multiplied
 * by the original normal vector, and added as force
 * on that node, and perhaps the connected nodes.
 * (I haven't decided yet.)
 * It should be noted that again assuming a connection
 * length of 1., and a 90 degree connection, saves much hassle.
 *
 * NOTE that the wind-friction factor is assumed 1., which is
 * to say, the force generated by a wind of velocity 1. (in the
 * same units as vxyz) hitting a square dead on (perpendicular)
 * will generate a perpendicular force of 1..  This can make
 * the flag look dead, but it also helps make it look light (very
 * affected by the wind.)  If it looks to weightless, throw in a
 * vecmulc of some less than 1. number just before adding this
 * wind force to the global fxyz.
 */
double windx,windy,windz;

force_wind()
{
	static double norm[len3],rel[len3];
	static double a[len3],b[len1];
	int i;

	vecsub3(pxyz,pxyz+1,dxyz,len3-(sizex+1)); /* Rel vectors to right, */
	vecsub3(pxyz,pxyz+sizex,txyz,len3-(sizex+1)); /* and up. */

	/*
	 * Cross product: norm = dxyz X txyz
	 */
	vecmul3(dxyz+yoff,txyz+zoff,a,len1-(sizex+1));
	vecmul3(dxyz+zoff,txyz+yoff,b,len1-(sizex+1));
	vecsub3(b,a,norm,len1-(sizex+1));
	vecmul3(dxyz+zoff,txyz+xoff,a,len1-(sizex+1));
	vecmul3(dxyz+xoff,txyz+zoff,b,len1-(sizex+1));
	vecsub3(b,a,norm+yoff,len1-(sizex+1));
	vecmul3(dxyz+xoff,txyz+yoff,a,len1-(sizex+1));
	vecmul3(dxyz+yoff,txyz+xoff,b,len1-(sizex+1));
	vecsub3(b,a,norm+zoff,len1-(sizex+1));

	/*
	 * Calculate rel wind/4.
	 * (Divide by four cause we're gonna
	 * add it to all four nodes on that
	 * square.)
	 */
	vecsubc3(windx,vxyz+xoff,rel+xoff,len1-(sizex+1));
	vecsubc3(windy,vxyz+yoff,rel+yoff,len1-(sizex+1));
	vecsubc3(windz,vxyz+zoff,rel+zoff,len1-(sizex+1));
	vecmulc(-0.25,rel,len3-(sizex+1));

	/*
	 * Dot product: a = norm dot rel wind/4.
	 */
	vecmul3(norm,rel,a,len3-(sizex+1));
	vecadd(a+yoff,a,len1-(sizex+1));
	vecadd(a+zoff,a,len1-(sizex+1));

	/*
	 * Multiply norm by a.
	 */
	vecmul(a,norm+xoff,len1-(sizex+1));
	vecmul(a,norm+yoff,len1-(sizex+1));
	vecmul(a,norm+zoff,len1-(sizex+1));

	/*
	 * Clean up the wrap stuff
	 */
	veccopyc(0.,norm+(len1-sizex),sizex-1);
	veccopyc(0.,norm+(yoff+(len1-sizex)),sizex-1);
	for (i=sizex-1; i<len3-(sizex+1); i+=sizex)
		norm[i] = 0.;

	/*
	 * Add to the force vector.
	 */
	vecadd(norm,fxyz,len3-(sizex+1));
	vecadd(norm,fxyz+1,len3-(sizex+1));
	vecadd(norm,fxyz+sizex,len3-(sizex+1));
	vecadd(norm,fxyz+sizex+1,len3-(sizex+1));
}


moveflag()
{

	vecmulc(timestep,fxyz,len3);
	vecadd(fxyz,vxyz,len3);

	vecmulc3(timestep,vxyz,dxyz,len3);
	vecadd(dxyz,pxyz,len3);
}

init_sqrt()
{
	int i;

	printf("Initializing sqrt table...");
	fflush(stdout);
	veci(sqrtres)
		sqrtl[i] = sqrt(i/sqrtscale);
	printf("done\n");
}


init_wind()
{
	windx = 1.; 
	windy = 0.;
	windz = 0.;
}

init_flag()
{
	int i,xc;
	double x,y;

	xc = 0;
	x = 0.;
	y = 0.;

	veccopyc(0.,vxyz,len3);

	veci(len1) {

		pxyz[xoff+i] = x;
		pxyz[yoff+i] = y;
		pxyz[zoff+i] = y*x*0.01/(sizex*sizey)+x*0.01/sizex;

		if ((++xc)>=sizex) {
			x = 0.;
			xc = 0;
			y++;
		} else
			x++;
	}
}

test_exit()
{
	/*return (getbutton(MIDDLEMOUSE));*/
}

init_graphics()
{
/*
	ginit();
	doublebuffer();
	gconfig();

	perspective(400,1.3,.1,5000.);
	lookat(0.,sizey*0.5,sizex*2.5,0.,sizey*0.5,0.,0);
	color(BLACK);
	clear();
*/
}


#define ColorScale 1.
#define clamp(x) (x>1. ? 1. : (x<0. ? 0. : x))

drawflag()
{
	int ix,iy,j;
	DtReal col;

	j=0;

        for(iy = 0; iy < sizey; iy++)
	    for(ix = 0; ix < sizex; ix++) {

                Vertices[ix][iy][0] = pxyz[j];
                Vertices[ix][iy][1] = pxyz[j+yoff];
                Vertices[ix][iy][2] = pxyz[j+zoff];

		col = ColorScale *(Vertices[ix][iy][0] - oldVert[ix][iy][0]);
		if (col < 0.)
			col *= -1.;
		col += .5;
                Vertices[ix][iy][3] = clamp(col);
		col = ColorScale *(Vertices[ix][iy][1] - oldVert[ix][iy][1]);
		if (col < 0.)
			col *= -1.;
		col += .5;
                Vertices[ix][iy][4] = clamp(col);
		col = ColorScale *(Vertices[ix][iy][2] - oldVert[ix][iy][2]);
		if (col < 0.)
			col *= -1.;
		col += .5;
                Vertices[ix][iy][5] = clamp(col);

                oldVert[ix][iy][0] = Vertices[ix][iy][0] ;
                oldVert[ix][iy][1] = Vertices[ix][iy][1] ;
                oldVert[ix][iy][2] = Vertices[ix][iy][2] ;

		j++;
                }
	framenumber++;
	DsExecuteObj(DoTriangleMesh(DcRGB,
		DcLocClr,len1,Vertices,nTris,Triangles,DcTrue));

}

init_tris()
{
        int ix,iy,i;
        i=0;

        for(iy = 0; iy < sizey; iy++)
                for(ix = 0; ix < sizex; ix++) {
                        if((ix < sizex-2) && (iy != sizey-1)) {
                                Triangles[i][2] = sizey * ix + iy;
                                Triangles[i][1] = sizey * ix + (iy+1);
                                Triangles[i][0] = sizey * (ix+1) + (iy+1);
                                i++;
                                Triangles[i][2] = sizey * ix + iy;
                                Triangles[i][1] = sizey * (ix+1) + (iy+1);
                                Triangles[i][0] = sizey * (ix+1) + iy;
                                i++;
                                }
                        }
                        nTris=i;
	printf("Flag contains %d triangles\n",nTris);
}
