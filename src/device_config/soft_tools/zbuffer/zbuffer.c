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
 
/*
  ======================================================================
  Functions:
	ddr_zbuffer_Clear
	ddr_zbuffer_ClearRect
	ddr_zbuffer_DrawPoint
	ddr_zbuffer_DrawLine
	ddr_zbuffer_DrawTriangle
	ddr_zbuffer_TopFlatTri
	ddr_zbuffer_BottomFlatTri
	ddr_zbuffer_LeftFaceTri
	ddr_zbuffer_RightFaceTri

  ======================================================================
 */
#include <memory.h>
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/zbuffer.h>

typedef struct {
   DtUShort *depth_buf;
   DtInt width;
   DtInt height;
   void (*put_pixel) (DtInt, DtInt, DtColorRGB);
} ddt_zbuffer_data;

static ddt_zbuffer_data dde_zbuffer_data;

static void ddr_zbuffer_TopFlatTri     (DtInt*, DtInt*, DtInt*,
					DtColorRGB, DtColorRGB, DtColorRGB);
static void ddr_zbuffer_BottomFlatTri  (DtInt*, DtInt*, DtInt*,
					DtColorRGB, DtColorRGB, DtColorRGB);
static void ddr_zbuffer_LeftFaceTri    (DtInt*, DtInt*, DtInt*,
					DtColorRGB, DtColorRGB, DtColorRGB);
static void ddr_zbuffer_RightFaceTri   (DtInt*, DtInt*, DtInt*,
					DtColorRGB, DtColorRGB, DtColorRGB);

/*
 ======================================================================
 */
void ddr_zbuffer_SetBuffer (
    DtInt width,
    DtInt height,
    DtUShort *depth_buf,
    void (*put_pixel)(DtInt, DtInt, DtColorRGB))
{
#ifdef DEBUG
    printf("...processing ddr_zbuffer_SetBuffer\n");
    fflush(stdout);
#endif

    dde_zbuffer_data.width     = width;
    dde_zbuffer_data.height    = height;
    dde_zbuffer_data.depth_buf = depth_buf;
    dde_zbuffer_data.put_pixel = put_pixel;
}
/*
 ======================================================================
 */
void ddr_zbuffer_Clear (void)
{
    DtInt size;

#ifdef DEBUG
    printf("...processing ddr_zbuffer_Clear\n");
    fflush(stdout);
#endif

    size = dde_zbuffer_data.width * dde_zbuffer_data.height;

#ifdef DEBUG
    printf("size = %d\n",size);
#endif
    memset(dde_zbuffer_data.depth_buf,0,size*sizeof(DtUShort));
}
/*
 ======================================================================
 */
void ddr_zbuffer_ClearRect (
    int x,
    int y,
    int width,
    int height)
{
    DtInt i;
    DtUShort *depth_buf;

#ifdef DEBUG
    printf("...processing ddr_zbuffer_ClearRect\n");
    printf("x(%d),y(%d),width(%d),height(%d)\n",x,y,width,height);
    fflush(stdout);
#endif

    /* Accelerate the case where we clear the whole thing */
    if (height == dde_zbuffer_data.height && 
	width == dde_zbuffer_data.width) {
	memset(dde_zbuffer_data.depth_buf,0,width*height*sizeof(DtUShort));
	return;
    }
	
    for(i=0; i<height; i++) {
	depth_buf = dde_zbuffer_data.depth_buf + dde_zbuffer_data.width*(y+i) + x;

	/* This will clear using long words and should be faster */
	memset(depth_buf,0,width*sizeof(DtUShort));
    }
}
/*
 ======================================================================
 */
void ddr_zbuffer_DrawPoint (
    DtInt *point,
    DtColorRGB color)
{
    DtUShort *depth;
    DtInt width, height;
    DtInt address;

#ifdef DEBUG
    printf("...processing ddr_zbuffer_DrawPoint\n");
    fflush(stdout);
#endif

    depth  = dde_zbuffer_data.depth_buf;
    width  = dde_zbuffer_data.width;
    height = dde_zbuffer_data.height;

    address = point[1]*width + point[0];
    if((address < 0) || (address >= width*height)) {
#ifdef DEBUG
	printf("point = (%d,%d,%d) width = %d height = %d\n",
	       point[0],point[1],point[2],width,height);
	printf("Address = %d\n",address);
	DDerror(ERR_DEVICE_SYSTEM_ACCESS,"ddr_zbuffer_DrawPoint",
		"Bad buffer address required");
#else
	return;
#endif
    }

    if((DtUShort)point[2] >= depth[address]) {
	depth[address] = (DtUShort)point[2];

	(*dde_zbuffer_data.put_pixel)(point[0], point[1],color);
    }
}
/*
 ======================================================================
 */
void ddr_zbuffer_DrawLine (
    DtInt *point1,
    DtInt *point2,
    DtColorRGB color1,
    DtColorRGB color2)
{
    int error;
    int deltax,deltay, tmpdelta;
    double deltaz;
    int outpnt[3];
    DtColorRGB outcolor, delcolor;
    int x, y;
    double z;
    int i;
    int flag = 0;
    int flagx = 1;
    int flagy = 1;
    int tmpflag;

#ifdef DEBUG
    printf("...processing ddr_zbuffer_DrawLine\n");
    printf("points (%d,%d)-(%d,%d)\n", point1[0],point1[1],point2[0],point2[1]);
    fflush(stdout);
#endif

    /* this routine uses the midpoint line scan-conversion
       algorithm, as described in Foley and Van Dam */

    z = point1[2]+0.5;
    outcolor[0] = color1[0];
    outcolor[1] = color1[1];
    outcolor[2] = color1[2];

    if (point1[0] > point2[0]) flagx = -1;
    if (point1[1] > point2[1]) flagy = -1;

    deltax = abs (point2[0] - point1[0]);
    deltay = abs (point2[1] - point1[1]);

    if (deltax > deltay) {
        if (!deltax) {
	    deltaz      = 0;
	    delcolor[0] = 0;
	    delcolor[1] = 0;
	    delcolor[2] = 0;
	} else {
	    deltaz      = (double)(point2[2]-point1[2]) / deltax;
	    delcolor[0] = (color2[0]-color1[0]) / deltax;
	    delcolor[1] = (color2[1]-color1[1]) / deltax;
	    delcolor[2] = (color2[2]-color1[2]) / deltax;
	}
	x = point1[0];
	y = point1[1];
    } else {
	if (!deltay) {
	    deltaz      = 0;
	    delcolor[0] = 0;
	    delcolor[1] = 0;
	    delcolor[2] = 0;
	} else {
	    deltaz      = (double)(point2[2]-point1[2]) / deltay;
	    delcolor[0] = (color2[0]-color1[0]) / deltay;
	    delcolor[1] = (color2[1]-color1[1]) / deltay;
	    delcolor[2] = (color2[2]-color1[2]) / deltay;
	}
	x = point1[1];
	y = point1[0];
	flag = 1;
	tmpflag = flagx;
	flagx = flagy;
	flagy = tmpflag;
        tmpdelta = deltax;
	deltax = deltay; 
	deltay = tmpdelta; 
    }

    error = 2*deltay - deltax;
    for(i=0; i<= deltax; i++) {
	if(flag) {
	    outpnt[0] = y;
	    outpnt[1] = x;
	} else {
	    outpnt[0] = x;
	    outpnt[1] = y;
	}
	outpnt[2] = (int)z;
	ddr_zbuffer_DrawPoint(outpnt,outcolor);


	if(error > 0) {
	    y += flagy;
	    error += (2*deltay - 2*deltax);
	} else {
	    error += 2*deltay;
	}
	x += flagx;
	z += deltaz;
	outcolor[0] += delcolor[0];
	outcolor[1] += delcolor[1];
	outcolor[2] += delcolor[2];
    }
#ifdef DEBUG
    printf("End of ddr_zbuffer_DrawLine\n");
    fflush(stdout);
#endif
}
/*
 ======================================================================
 */
void ddr_zbuffer_DrawTriangle (
    DtInt *point1,
    DtInt *point2,
    DtInt *point3,
    DtColorRGB color1,
    DtColorRGB color2,
    DtColorRGB color3)
{
    DtInt *top, *left, *right;
    DtReal *top_color, *left_color, *right_color;

#ifdef DEBUG
    printf("...processing ddr_zbuffer_DrawTriangle\n");
    fflush(stdout);
#endif
    /* 
     * Throw away zero area triangles -- they currently cause visual
     * artifacts.  Probably the better way to fix this would be to
     * change the algorithm so that they aren't drawn at all.
     */
    if ((point1[0] == point2[0] && point1[1] == point2[1]) ||
	(point1[0] == point3[0] && point1[1] == point3[1]) ||
	(point2[0] == point3[0] && point2[1] == point3[1]))
	    return;

    if((point1[1] <= point2[1]) && (point1[1] <= point3[1])) {
	top = point1;
	top_color = color1;
	if(point2[0] < point3[0]) {
	    left = point2;
	    right = point3;
	    left_color = color2;
	    right_color = color3;
	} else {
	    left = point3;
	    right = point2;
	    left_color = color3;
	    right_color = color2;
	}
    } else if((point2[1] <= point1[1]) && (point2[1] <= point3[1])) {
	top = point2;
	top_color = color2;
	if(point1[0] < point3[0]) {
	    left = point1;
	    right = point3;
	    left_color = color1;
	    right_color = color3;
	} else {
	    left = point3;
	    right = point1;
	    left_color = color3;
	    right_color = color1;
	}
    } else {
	top = point3;
	top_color = color3;
	if(point1[0] < point2[0]) {
	    left = point1;
	    right = point2;
	    left_color = color1;
	    right_color = color2;
	} else {
	    left = point2;
	    right = point1;
	    left_color = color2;
	    right_color = color1;
	}
    }

    if(top[1] == left[1])
	    ddr_zbuffer_TopFlatTri(left,top,right,left_color,top_color,right_color);
    else if(top[1] == right[1])
	    ddr_zbuffer_TopFlatTri(top,right,left,top_color,right_color,left_color);
    else if(left[1] == right[1])
	    ddr_zbuffer_BottomFlatTri(top,left,right,top_color,left_color,right_color);
    else if(right[1] > left[1])
	    ddr_zbuffer_RightFaceTri(top,left,right,top_color,left_color,right_color);
    else
	    ddr_zbuffer_LeftFaceTri(top,left,right,top_color,left_color,right_color);

}
/*
 ======================================================================
 */
static void ddr_zbuffer_TopFlatTri (
    DtInt *left,
    DtInt *right,
    DtInt *bottom,
    DtColorRGB color1,
    DtColorRGB color2,
    DtColorRGB color3)
{
    DtReal     xl,zl, xr,zr;
    DtReal     deltaxl,deltazl, deltaxr, deltazr;
    DtInt      y;
    DtInt      point1[3], point2[3];
    DtColorRGB lcolor, rcolor, delclr_right, delclr_left;
    DtInt      bottom_left, bottom_right;

#ifdef DEBUG
    printf("...processing ddr_zbuffer_TopFlatTri\n");
    fflush(stdout);
#endif

    bottom_left  = bottom[1] - left[1];
    bottom_right = bottom[1] - right[1];

    xl = left[0]+0.5;
    xr = right[0]+0.5;
    zl = left[2]+0.5;
    zr = right[2]+0.5;
    if (!bottom_left) {
	deltaxl = 0;
	deltazl = 0;
    } else {
	deltaxl = (DtReal)(bottom[0] - left[0]) / bottom_left;
	deltazl = (DtReal)(bottom[2] - left[2]) / bottom_left;
    }
    if (!bottom_right) {
	deltaxr = 0;
	deltazr = 0;
    } else {
	deltaxr = (DtReal)(bottom[0] - right[0]) / bottom_right;
	deltazr = (DtReal)(bottom[2] - right[2]) / bottom_right;
    }

    lcolor[0] = color1[0];
    lcolor[1] = color1[1];
    lcolor[2] = color1[2];
    rcolor[0] = color2[0];
    rcolor[1] = color2[1];
    rcolor[2] = color2[2];
    if (!bottom_left) {
	delclr_left[0] = 0;
	delclr_left[1] = 0;
	delclr_left[2] = 0;
    } else {
	delclr_left[0] = (color3[0]-color1[0]) / bottom_left;
	delclr_left[1] = (color3[1]-color1[1]) / bottom_left;
	delclr_left[2] = (color3[2]-color1[2]) / bottom_left;
    }
    if (!bottom_right) {
	delclr_right[0] = 0;
	delclr_right[1] = 0;
	delclr_right[2] = 0;
    } else {
	delclr_right[0] = (color3[0]-color2[0]) / bottom_right;
	delclr_right[1] = (color3[1]-color2[1]) / bottom_right;
	delclr_right[2] = (color3[2]-color2[2]) / bottom_right;
    }

    for(y=left[1]; y<=bottom[1]; y++) {
	point1[0] = (DtInt)xl;
	point1[1] = y;
	point1[2] = (DtInt)zl;
	point2[0] = (DtInt)xr;
	point2[1] = y;
	point2[2] = (DtInt)zr;
	ddr_zbuffer_DrawLine(point1,point2,lcolor,rcolor);

	xl += deltaxl;
	zl += deltazl;
	xr += deltaxr;
	zr += deltazr;
	lcolor[0] += delclr_left[0];
	lcolor[1] += delclr_left[1];
	lcolor[2] += delclr_left[2];
	rcolor[0] += delclr_right[0];
	rcolor[1] += delclr_right[1];
	rcolor[2] += delclr_right[2];
    }
}
/*
 ======================================================================
 */
static void ddr_zbuffer_BottomFlatTri (
    DtInt *top,
    DtInt *left,
    DtInt *right,
    DtColorRGB color1,
    DtColorRGB color2,
    DtColorRGB color3)
{
    DtReal xl,zl, xr,zr;
    DtReal deltaxl,deltazl, deltaxr, deltazr;
    DtInt y;
    DtInt point1[3], point2[3];
    DtColorRGB lcolor, rcolor, delclr_left, delclr_right;
    DtInt      left_top, right_top;

#ifdef DEBUG
    printf("...processing ddr_zbuffer_BottomFlatTri\n");
    fflush(stdout);
#endif

    left_top  = left[1] - top[1];
    right_top = right[1] - top[1];

    xl = xr = top[0]+0.5;
    zl = zr = top[2]+0.5;
    if (!left_top) {
	deltaxl = 0;
	deltazl = 0;
	delclr_left[0] = 0;
	delclr_left[1] = 0;
	delclr_left[2] = 0;
    } else {
	deltaxl = (DtReal)(left[0]-top[0]) / left_top;
	deltazl = (DtReal)(left[2]-top[2]) / left_top;
	delclr_left[0] = (color2[0]-color1[0]) / left_top;
	delclr_left[1] = (color2[1]-color1[1]) / left_top;
	delclr_left[2] = (color2[2]-color1[2]) / left_top;
    }
    if (!right_top) {
	deltaxr = 0;
	deltazr = 0;
	delclr_right[0] = 0;
	delclr_right[1] = 0;
	delclr_right[2] = 0;
    } else {
	deltaxr = (DtReal)(right[0]-top[0]) / right_top;
	deltazr = (DtReal)(right[2]-top[2]) / right_top;
	delclr_right[0] = (color3[0]-color1[0]) / right_top;
	delclr_right[1] = (color3[1]-color1[1]) / right_top;
	delclr_right[2] = (color3[2]-color1[2]) / right_top;
    }

    lcolor[0] = rcolor[0] = color1[0];
    lcolor[1] = rcolor[1] = color1[1];
    lcolor[2] = rcolor[2] = color1[2];

#ifdef DEBUG
    printf("xl(%lf),zl(%lf),xr(%lf),zr(%lf)\n",xl,zl,xr,zr);
    printf("deltaxl(%lf),deltazl(%lf),deltaxr(%lf),deltazr(%lf)\n",
	   deltaxl,deltazl,deltaxr,deltazr);
    fflush(stdout);
#endif
    for(y=top[1]; y<=left[1]; y++) {
	point1[0] = (DtInt)xl;
	point1[1] = y;
	point1[2] = (DtInt)zl;
	point2[0] = (DtInt)xr;
	point2[1] = y;
	point2[2] = (DtInt)zr;
	ddr_zbuffer_DrawLine(point1,point2,lcolor,rcolor);

	xl += deltaxl;
	zl += deltazl;
	xr += deltaxr;
	zr += deltazr;

	lcolor[0] += delclr_left[0];
	lcolor[1] += delclr_left[1];
	lcolor[2] += delclr_left[2];
	rcolor[0] += delclr_right[0];
	rcolor[1] += delclr_right[1];
	rcolor[2] += delclr_right[2];
    }
}
/*
 ======================================================================
 */
static void ddr_zbuffer_LeftFaceTri (
    DtInt *top,
    DtInt *left,
    DtInt *right,
    DtColorRGB color1,
    DtColorRGB color2,
    DtColorRGB color3)
{
    DtReal xl,zl, xr,zr;
    DtReal deltaxl,deltazl, deltaxr, deltazr;
    DtInt y;
    DtInt point1[3], point2[3];
    DtColorRGB lcolor, rcolor, delclr_left, delclr_right;
    DtInt      left_top, right_top, left_right;

#ifdef DEBUG
    printf("...processing ddr_zbuffer_LeftFaceTri\n");
    fflush(stdout);
#endif

    left_top  = left[1]  - top[1];
    right_top = right[1] - top[1];

    xl = xr = top[0]+0.5;
    zl = zr = top[2]+0.5;

    if (!left_top) {
	deltaxl = 0;
	deltazl = 0;
	delclr_left[0] = 0;
	delclr_left[1] = 0;
	delclr_left[2] = 0;
    } else {
	deltaxl = (DtReal)(left[0]-top[0]) / left_top;
	deltazl = (DtReal)(left[2]-top[2]) / left_top;
	delclr_left[0] = (color2[0]-color1[0]) / left_top;
	delclr_left[1] = (color2[1]-color1[1]) / left_top;
	delclr_left[2] = (color2[2]-color1[2]) / left_top;
    }

    if (!right_top) {
	deltaxr = 0;
	deltazr = 0;
	delclr_right[0] = 0;
	delclr_right[1] = 0;
	delclr_right[2] = 0;
    } else {
	deltaxr = (DtReal)(right[0]-top[0]) / right_top;
	deltazr = (DtReal)(right[2]-top[2]) / right_top;
	delclr_right[0] = (color3[0]-color1[0]) / right_top;
	delclr_right[1] = (color3[1]-color1[1]) / right_top;
	delclr_right[2] = (color3[2]-color1[2]) / right_top;
    }

    lcolor[0] = rcolor[0] = color1[0];
    lcolor[1] = rcolor[1] = color1[1];
    lcolor[2] = rcolor[2] = color1[2];

    for(y=top[1]; y<right[1]; y++) {
	point1[0] = (DtInt)xl;
	point1[1] = y;
	point1[2] = (DtInt)zl;
	point2[0] = (DtInt)xr;
	point2[1] = y;
	point2[2] = (DtInt)zr;
	ddr_zbuffer_DrawLine(point1,point2,lcolor,rcolor);

	xl += deltaxl;
	zl += deltazl;
	xr += deltaxr;
	zr += deltazr;

	lcolor[0] += delclr_left[0];
	lcolor[1] += delclr_left[1];
	lcolor[2] += delclr_left[2];
	rcolor[0] += delclr_right[0];
	rcolor[1] += delclr_right[1];
	rcolor[2] += delclr_right[2];
    }

    left_right = left[1] - right[1];

    if (!left_right) {
	deltaxr = 0;
	deltazr = 0;
	delclr_right[0] = 0;
	delclr_right[1] = 0;
	delclr_right[2] = 0;
    } else {
	deltaxr = (DtReal)(left[0]-right[0]) / left_right;
	deltazr = (DtReal)(left[2]-right[2]) / left_right;
	delclr_right[0] = (color2[0]-color3[0]) / left_right;
	delclr_right[1] = (color2[1]-color3[1]) / left_right;
	delclr_right[2] = (color2[2]-color3[2]) / left_right;
    }

    for(; y<=left[1]; y++) {
	point1[0] = (DtInt)xl;
	point1[1] = y;
	point1[2] = (DtInt)zl;
	point2[0] = (DtInt)xr;
	point2[1] = y;
	point2[2] = (DtInt)zr;
	ddr_zbuffer_DrawLine(point1,point2,lcolor,rcolor);

	xl += deltaxl;
	zl += deltazl;
	xr += deltaxr;
	zr += deltazr;

	lcolor[0] += delclr_left[0];
	lcolor[1] += delclr_left[1];
	lcolor[2] += delclr_left[2];
	rcolor[0] += delclr_right[0];
	rcolor[1] += delclr_right[1];
	rcolor[2] += delclr_right[2];
    }
}
/*
 ======================================================================
 */
static void ddr_zbuffer_RightFaceTri (
    DtInt *top,
    DtInt *left,
    DtInt *right,
    DtColorRGB color1,
    DtColorRGB color2,
    DtColorRGB color3)
{
    DtReal xl,zl, xr,zr;
    DtReal deltaxl,deltazl, deltaxr, deltazr;
    DtInt y;
    DtInt point1[3], point2[3];
    DtColorRGB lcolor, rcolor, delclr_left, delclr_right;
    DtInt      left_top, right_top, right_left;

#ifdef DEBUG
    printf("...processing ddr_zbuffer_RightFaceTri\n");
    fflush(stdout);
#endif

    left_top  = left[1] - top[1];
    right_top = right[1] - top[1];

    xl = xr = top[0]+0.5;
    zl = zr = top[2]+0.5;

    if (!left_top) {
	deltaxl = 0;
	deltazl = 0;
	delclr_left[0] = 0;
	delclr_left[1] = 0;
	delclr_left[2] = 0;
    } else {
	deltaxl = (DtReal)(left[0]-top[0]) / left_top;
	deltazl = (DtReal)(left[2]-top[2]) / left_top;
	delclr_left[0] = (color2[0]-color1[0]) / left_top;
	delclr_left[1] = (color2[1]-color1[1]) / left_top;
	delclr_left[2] = (color2[2]-color1[2]) / left_top;
    }

    if (!right_top) {
	deltaxr = 0;
	deltazr = 0;
	delclr_right[0] = 0;
	delclr_right[1] = 0;
	delclr_right[2] = 0;
    } else {
	deltaxr = (DtReal)(right[0]-top[0]) / right_top;
	deltazr = (DtReal)(right[2]-top[2]) / right_top;
	delclr_right[0] = (color3[0]-color1[0]) / right_top;
	delclr_right[1] = (color3[1]-color1[1]) / right_top;
	delclr_right[2] = (color3[2]-color1[2]) / right_top;
    }

    lcolor[0] = rcolor[0] = color1[0];
    lcolor[1] = rcolor[1] = color1[1];
    lcolor[2] = rcolor[2] = color1[2];

    for(y=top[1]; y<left[1]; y++) {
	point1[0] = (DtInt)xl;
	point1[1] = y;
	point1[2] = (DtInt)zl;
	point2[0] = (DtInt)xr;
	point2[1] = y;
	point2[2] = (DtInt)zr;
	ddr_zbuffer_DrawLine(point1,point2,lcolor,rcolor);

	xl += deltaxl;
	zl += deltazl;
	xr += deltaxr;
	zr += deltazr;

	lcolor[0] += delclr_left[0];
	lcolor[1] += delclr_left[1];
	lcolor[2] += delclr_left[2];
	rcolor[0] += delclr_right[0];
	rcolor[1] += delclr_right[1];
	rcolor[2] += delclr_right[2];
    }

    right_left = right[1] - left[1];

    if (!right_left) {
	deltaxl = 0;
	deltazl = 0;
	delclr_left[0] = 0;
	delclr_left[1] = 0;
	delclr_left[2] = 0;
    } else {
	deltaxl = (DtReal)(right[0]-left[0]) / right_left;
	deltazl = (DtReal)(right[2]-left[2]) / right_left;
	delclr_left[0] = (color3[0]-color2[0]) / right_left;
	delclr_left[1] = (color3[1]-color2[1]) / right_left;
	delclr_left[2] = (color3[2]-color2[2]) / right_left;
    }

    for(; y<=right[1]; y++) {
	point1[0] = (DtInt)xl;
	point1[1] = y;
	point1[2] = (DtInt)zl;
	point2[0] = (DtInt)xr;
	point2[1] = y;
	point2[2] = (DtInt)zr;
	ddr_zbuffer_DrawLine(point1,point2,lcolor,rcolor);

	xl += deltaxl;
	zl += deltazl;
	xr += deltaxr;
	zr += deltazr;

	lcolor[0] += delclr_left[0];
	lcolor[1] += delclr_left[1];
	lcolor[2] += delclr_left[2];
	rcolor[0] += delclr_right[0];
	rcolor[1] += delclr_right[1];
	rcolor[2] += delclr_right[2];
    }
}
