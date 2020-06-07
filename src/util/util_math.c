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
 
/*============================================================================
  Functions:
	dor_math_acos
	dor_math_angle
	dor_math_asin
	dor_math_atan
	dor_math_atan2
	dor_math_ceil
	dor_math_cos
	dor_math_cross_cosines
	dor_math_crossproduct
	dor_math_distance
	dor_math_distance_squared
	dor_math_dotproduct
	dor_math_fabs
	dor_math_fast_cross_cosines
	dor_math_fast_normal
	dor_math_fast_sqrt
	dor_math_floor
	dor_math_homo_trns
	dor_math_homo_trns_fast
	dor_math_line_intersect
	dor_math_line_intersect_point
	dor_math_log
	dor_math_normal
	dor_math_pixel_window
	dor_math_plane_equation
	dor_math_pnttrns
	dor_math_pnttrns_fast
	dor_math_point_line_distance
	dor_math_polygon_normal_fast
	dor_math_polygon_plane_equation
	dor_math_pow
	dor_math_projected_polygon_area
	dor_math_projected_polygon_area_fast
	dor_math_projected_triangle_area_squared
	dor_math_projected_triangle_area_squared_fast
	dor_math_renormalize
	dor_math_renormalize_fast
	dor_math_renormalize_vector
	dor_math_same_side
	dor_math_sin
	dor_math_sqrt
	dor_math_vectrns
	dor_math_vectrns_fast

============================================================================*/

#include <math.h>
#include <dore/internal/dogen.h>
#include <dore/internal/matrix.h>



/*============================================================================
**  Calculate the plane equation of a plane described by three non-colinear
**  points.
============================================================================*/

void dor_math_cross_cosines (
    DtReal x1, DtReal y1, DtReal z1,	/* First Point Coordinates */
    DtReal x2, DtReal y2, DtReal z2,	/* Second Point Coordinates */
    DtReal x3, DtReal y3, DtReal z3,	/* Third Point Coordinates */
    DtReal *cosinex, DtReal *cosiney, DtReal *cosinez )		/* Return */
{
    DtReal p1x, p1y, p1z, p2x, p2y, p2z;

    /*  Make the first and third endpoint relative to the second.  */

    p1x = x1 - x2;
    p1y = y1 - y2;
    p1z = z1 - z2;

    p2x = x3 - x2;
    p2y = y3 - y2;
    p2z = z3 - z2;

    *cosinex = -p1y*p2z + p1z*p2y; 
    *cosiney = -p1z*p2x + p1x*p2z;
    *cosinez = -p1x*p2y + p1y*p2x;
}



/*==========================================================================*/

void dor_math_crossproduct (
    DtReal px, DtReal py, DtReal pz,			/* Vector 1 */
    DtReal vx, DtReal vy, DtReal vz,			/* Vector 2 */
    DtReal *resultx, DtReal *resulty, DtReal *resultz )	/* Result Vector */
{
    *resultx = py*vz - pz*vy; 
    *resulty = pz*vx - px*vz;
    *resultz = px*vy - py*vx;
}



/*==========================================================================*/

DtReal dor_math_distance (
    DtReal x1, DtReal y1, DtReal z1,	/* First Point */
    DtReal x2, DtReal y2, DtReal z2 )	/* Second Point */
{
    return dor_math_sqrt ((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)+(z2-z1)*(z2-z1));
}



/*==========================================================================*/

DtReal dor_math_distance_squared (
    DtReal x1, DtReal y1, DtReal z1,	/* First Point */
    DtReal x2, DtReal y2, DtReal z2 )	/* Second Point */
{
    return ((x2-x1) * (x2-x1)) + ((y2-y1) * (y2-y1)) + ((z2-z1) * (z2-z1));
}



/*==========================================================================*/

DtReal dor_math_dotproduct (
    DtReal x1, DtReal y1, DtReal z1,	/* First Vector */
    DtReal x2, DtReal y2, DtReal z2)	/* Second Vector */
{
    return (x1 * x2) + (y1 * y2) + (z1 * z2);
}



/*============================================================================
**  Calculate the cross product of the vectors between three non-colinear 
**  points (usually making up a triangle).  The result gives a non-normalized 
**  direction vector for the surface normal of the triangle described by the 
**  three points, in the right handed coordinate system.
============================================================================*/

void dor_math_fast_cross_cosines (
    DtReal point1[3], DtReal point2[3], DtReal point3[3],
    DtReal cosine[3])
{
    DtReal p1x, p1y, p1z, p2x, p2y, p2z;

    /*  Make the first and third endpoint relative to the second.  */

    p1x = point1[0] - point2[0];
    p1y = point1[1] - point2[1];
    p1z = point1[2] - point2[2];

    p2x = point3[0] - point2[0];
    p2y = point3[1] - point2[1];
    p2z = point3[2] - point2[2];

    cosine[0] = -p1y*p2z + p1z*p2y; 
    cosine[1] = -p1z*p2x + p1x*p2z;
    cosine[2] = -p1x*p2y + p1y*p2x;
}



/*==========================================================================*/

DtFlag dor_math_fast_normal (
    DtReal point1[3],
    DtReal point2[3],
    DtReal normal[3])
{
    DtReal diff[3];
    DtReal length;

    diff[0] = point2[0] - point1[0];
    diff[1] = point2[1] - point1[1];
    diff[2] = point2[2] - point1[2];

    length = diff[0]*diff[0]+diff[1]*diff[1]+diff[2]*diff[2];

    if (length == 0.){
	normal[0] = 0.;
	normal[1] = 1.;
	normal[2] = 0.;

	return(DcFalse);
    }

    if (length == 1.){
	normal[0] = diff[0];
	normal[1] = diff[1];
	normal[2] = diff[2];

	return(DcTrue);
    }

    length = 1. / dor_math_fast_sqrt(length);

    normal[0] = diff[0] * length;
    normal[1] = diff[1] * length;
    normal[2] = diff[2] * length;

    return(DcTrue);
}



/*==========================================================================*/

void dor_math_homo_trns (
    DtReal      origx, DtReal origy, DtReal origz, DtReal origw,
    DtMatrix4x4 matrix,
    DtReal     *transx, DtReal *transy, DtReal *transz, DtReal *transw)
{
    DtVector4 invec, outvec;

    invec[0] = origx;
    invec[1] = origy;
    invec[2] = origz;
    invec[3] = origw;

    dor_matrix_new_vectormultiply(invec,matrix,outvec);

    *transx = outvec[0];
    *transy = outvec[1];
    *transz = outvec[2];
    *transw = outvec[3];
}



/*==========================================================================*/

PRAGMA(OPT_LEVEL 2)

void dor_math_homo_trns_fast (
    DtReal      origpoint[4],
    DtMatrix4x4 matrix,
    DtReal      transpoint[4] )
{
    DtInt row, column;
    DtReal newpoint[4];

PRAGMA(IVDEP)
    for (row=0;row<4;row++){
	newpoint[row] = 0.;

	for (column=0;column<4;column++){
	    newpoint[row] += matrix[row][column]*origpoint[column];
	}

	transpoint[row] = newpoint[row];
    }
}



/*==========================================================================*/

DtFlag dor_math_line_intersect (
    DtReal line1x1, DtReal line1y1,	/* Line 1, Endpoint 1 */
    DtReal line1x2, DtReal line1y2,	/* Line 1, Endpoint 2 */
    DtReal line2x1, DtReal line2y1,	/* Line 2, Endpoint 1 */
    DtReal line2x2, DtReal line2y2)	/* Line 2, Endpoint 2 */
{
    if (  (dor_math_same_side (line1x1,line1y1,line1x2,line1y2,
			       line2x1,line2y1,line2x2,line2y2) <= 0)
       && (dor_math_same_side (line2x1,line2y1,line2x2,line2y2,
			       line1x1,line1y1,line1x2,line1y2) <= 0)
       )
    {
	return DcTrue;
    }

    return DcFalse;
}



/*==========================================================================*/

void dor_math_line_intersect_point (
    DtReal line1x1, DtReal line1y1,		/* Line 1, endpoint 1 */
    DtReal line1x2, DtReal line1y2,		/* Line 1, endpoint 2 */
    DtReal line2x1, DtReal line2y1,		/* Line 2, endpoint 1 */
    DtReal line2x2, DtReal line2y2,		/* Line 2, endpoint 2 */
    DtReal *intersectx, DtReal *intersecty)	/* Intersection Point */
{
    DtReal a1, b1, c1;
    DtReal a2, b2, c2;
    DtReal denominator;

    a1 = (line1y2-line1y1)/(line1x2-line1x1);
    b1 = -1.;
    c1 = line1y1 - a1*line1x1;

    a2 = (line2y2-line2y1)/(line2x2-line2x1);
    b2 = -1.;
    c2 = line2y1 - a2*line2x1;

    denominator = a1*b2 - a2*b1;

    *intersectx = (b1*c2-b2*c1)/denominator;

    *intersecty = (c1*a2-c2*a1)/denominator;
}



/*==========================================================================*/

DtFlag dor_math_normal (
    DtReal  a1, DtReal  b1, DtReal  c1,
    DtReal  a2, DtReal  b2, DtReal  c2,
    DtReal *an, DtReal *bn, DtReal *cn)
{
    DtReal dx, dy, dz, length;

    dx = a2 - a1;
    dy = b2 - b1;
    dz = c2 - c1;

    length = dx*dx+dy*dy+dz*dz;

    if (length == 0.){
	*an = 0.;
	*bn = 1.;
	*cn = 0.;

	return(DcFalse);
    }

    if (length == 1.){
	*an = dx;
	*bn = dy;
	*cn = dz;

	return(DcTrue);
    }

    length = 1. / dor_math_sqrt(length);

    *an = dx * length;
    *bn = dy * length;
    *cn = dz * length;

    return(DcTrue);
}



/*============================================================================
**  Calculate the plane equation of a plane described by three
**  non-colinear points.
============================================================================*/

DtFlag dor_math_plane_equation (
    DtReal x1, DtReal y1, DtReal z1,			/* Point 1 */
    DtReal x2, DtReal y2, DtReal z2,			/* Point 2 */
    DtReal x3, DtReal y3, DtReal z3,			/* Point 3 */
    DtReal *cosinex, DtReal *cosiney, DtReal *cosinez,
    DtReal *D)
{
    DtReal p1x, p1y, p1z, p2x, p2y, p2z;

    /*  Make the first and third endpoint relative to the second.  */

    p1x = x1 - x2;
    p1y = y1 - y2;
    p1z = z1 - z2;

    p2x = x3 - x2;
    p2y = y3 - y2;
    p2z = z3 - z2;

    /*
     *cosinex = -p1y*p2z + p1z*p2y; 
     *cosiney = -p1z*p2x + p1x*p2z;
     *cosinez = -p1x*p2y + p1y*p2x;
     */
    *cosinex = p1y*p2z - p1z*p2y; 
    *cosiney = p1z*p2x - p1x*p2z;
    *cosinez = p1x*p2y - p1y*p2x;

    if (!dor_math_renormalize(cosinex,cosiney,cosinez)){
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "dor_math_plane_equation",
		    "invalid vector normal");
#ifdef debug
	printf("original points:  (%lg,%lg,%lg), (%lg,%lg,%lg), (%lg,%lg,%lg)\n",
	       x1,y1,z1,x2,y2,z2,x3,y3,z3);

	printf("p1, p2:  (%lg,%lg,%lg), (%lg,%lg,%lg)\n",p1x,p1y,p1z,p2x,p2y,p2z);

	printf("normal :  (%lg,%lg,%lg)\n",cosinex,cosiney,cosinez);
#endif
	*D = 0.;

	return(DcFalse);
    }

    *D = - (*cosinex*x2+*cosiney*y2+*cosinez*z2);

    return(DcTrue);
}



/*==========================================================================*/

void dor_math_polygon_normal_fast (
    DtInt         vertexcount,
    DtRealTriple *vertices,
    DtReal        normal[3])
{
    normal[0] = -dor_math_projected_polygon_area_fast(vertexcount,vertices,1,2);
    normal[1] =  dor_math_projected_polygon_area_fast(vertexcount,vertices,0,2);
    normal[2] = -dor_math_projected_polygon_area_fast(vertexcount,vertices,0,1);
}



/*==========================================================================*/

DtReal dor_math_projected_polygon_area_fast (
    DtInt         vertexcount,
    DtRealTriple *vertices,
    DtInt         axis1,
    DtInt         axis2)
{
    auto DtInt vertexno;
    auto DtReal area = 0.0;

    for (vertexno=1;  vertexno < (vertexcount-1);  vertexno++) {
	area = dor_math_projected_triangle_area_squared_fast(
			vertices[0][axis1], vertices[0][axis2],
			vertices[vertexno][axis1], vertices[vertexno][axis2],
			vertices[vertexno+1][axis1],
			vertices[vertexno+1][axis2]);
    }

    return (area * 0.5);
}



/*==========================================================================*/

DtReal dor_math_projected_triangle_area_squared_fast (
    DtReal x1, DtReal y1, DtReal x2,
    DtReal y2, DtReal x3, DtReal y3)
{
    return (x1*(y3-y2) + x2*(y1-y3) + x3*(y2-y1));
}



/*==========================================================================*/

void dor_math_polygon_plane_equation (
    DtInt         vertexcount,
    DtRealTriple *vertices,
    DtReal       *resulta,
    DtReal       *resultb,
    DtReal       *resultc,
    DtReal       *resultd)
{
    DtReal a, b, c, d; 

    a = -dor_math_projected_polygon_area(vertexcount,vertices,1,2);

    b = dor_math_projected_polygon_area(vertexcount,vertices,0,2);

    c = -dor_math_projected_polygon_area(vertexcount,vertices,0,1);

    d = -(a*vertices[0][0]+b*vertices[0][1]+c*vertices[0][2]);

    *resulta = a;
    *resultb = b;
    *resultc = c;
    *resultd = d;
}



/*==========================================================================*/

DtReal dor_math_projected_polygon_area (
    DtInt         vertexcount,
    DtRealTriple *vertices,
    DtInt         axis1,
    DtInt         axis2)
{
    DtInt vertexno;
    DtReal area;

    area = 0.;

    for (vertexno=1;vertexno<(vertexcount-1);vertexno++){
	area = dor_math_projected_triangle_area_squared(
		    vertices[0][axis1],vertices[0][axis2],
		    vertices[vertexno][axis1],vertices[vertexno][axis2],
		    vertices[vertexno+1][axis1],vertices[vertexno+1][axis2]);
    }

    area = area * .5;

    return(area);
}



/*==========================================================================*/

DtReal dor_math_projected_triangle_area_squared (
    DtReal x1, DtReal y1,
    DtReal x2, DtReal y2,
    DtReal x3, DtReal y3)
{
    return (x1*(y3-y2) + x2*(y1-y3) + x3*(y2-y1));
}



/*==========================================================================*/

DtReal dor_math_point_line_distance (
    DtReal pointx, DtReal pointy, DtReal pointz,
    DtReal linex1, DtReal liney1, DtReal linez1,
    DtReal linex2, DtReal liney2, DtReal linez2)
{
    DtReal vx, vy, vz;
    DtReal crossx, crossy, crossz;
    DtReal distance;

    /*  Compute the direction vector for the line.  */

    vx = linex2 - linex1;
    vy = liney2 - liney1;
    vz = linez2 - linez1;

    if (vx==0 && vy==0 && vz==0) {

	/* The two endpoints of the line are the same point, so just return
	** the distance from that point to the input point */

	vx = pointx - linex1;
	vy = pointy - liney1;
	vz = pointz - linez1;
	distance = dor_math_sqrt(vx*vx+vy*vy+vz*vz);

    } else {

	dor_math_crossproduct
	(   pointx-linex1, pointy-liney1, pointz-linez1, vx, vy, vz,
	    &crossx, &crossy, &crossz
	);

	distance = dor_math_sqrt (
	(   crossx*crossx + crossy*crossy + crossz*crossz) /
	       (vx*vx     +     vy*vy     +     vz*vz)
	);
    }

    return distance;
}



/*==========================================================================*/

void dor_math_pnttrns (
    DtReal      origx,
    DtReal      origy,
    DtReal      origz,
    DtMatrix4x4 matrix,
    DtReal     *transx,
    DtReal     *transy,
    DtReal     *transz)
{
    DtVector4 invec, outvec;

    invec[0] = origx;
    invec[1] = origy;
    invec[2] = origz;
    invec[3] = 1.;

    dor_matrix_vectormultiply(invec,matrix,outvec);

    *transx = outvec[0];
    *transy = outvec[1];
    *transz = outvec[2];
}



/*============================================================================
**  Transform a point (3 reals) into another point (3 reals) through a 
**  4x4 transformation matrix.  Note that we assume that the 4th component 
**  of the input vector is 1., and that we don't compute the 4th component 
**  of the output vector.
============================================================================*/

PRAGMA(OPT_LEVEL 2)

void dor_math_pnttrns_fast (
    DtReal      origpoint[3],
    DtMatrix4x4 matrix,
    DtReal      transpoint[3])
{
    DtInt  row, column;
    DtReal newpoint[3];

PRAGMA(IVDEP)
    for (row=0;  row < 3;  ++row){
	newpoint[row] = 0.;

	for (column=0;column<3;column++){
	    newpoint[row] += matrix[row][column]*origpoint[column];
	}

	newpoint[row] += matrix[row][3];

	transpoint[row] = newpoint[row];
    }
}



/*==========================================================================*/

DtFlag dor_math_renormalize (
    DtReal *normalx, DtReal *normaly, DtReal *normalz)
{
    DtReal length;

    length = (*normalx)*(*normalx)+(*normaly)*(*normaly)+(*normalz)*(*normalz);

    if (length == 0.){
	*normalx = 0.;
	*normaly = 0.;
	*normalz = 0.;

	return(DcFalse);
    }

    if (length == 1.){
	return(DcTrue);
    }

    length = 1. / dor_math_sqrt(length);

    *normalx *= length;
    *normaly *= length;
    *normalz *= length;

    return(DcTrue);
}



/*==========================================================================*/

DtFlag dor_math_renormalize_fast (DtReal normal[3])
{
    DtReal length;

    length = (normal[0])*(normal[0])+(normal[1])*(normal[1])+
	    (normal[2])*(normal[2]);

    if (length == 0.){
	normal[0] = 0.;
	normal[1] = 0.;
	normal[2] = 0.;

	return(DcFalse);
    }

    if (length == 1.){
	return(DcTrue);
    }

    length = 1. / dor_math_sqrt(length);

    normal[0] *= length;
    normal[1] *= length;
    normal[2] *= length;

    return(DcTrue);
}



/*==========================================================================*/

DtFlag dor_math_renormalize_vector (DtVector3 vector)
{
    DtReal length;

    length = (vector[0]*vector[0])+(vector[1]*vector[1])+(vector[2]*vector[2]);

    if (length == 0.){
	return(DcFalse);
    }

    if (length == 1.){
	return(DcTrue);
    }

    length = 1. / dor_math_sqrt(length);

    vector[0] *= length;
    vector[1] *= length;
    vector[2] *= length;

    return(DcTrue);
}



/*==========================================================================*/

DtShort dor_math_same_side (
    DtReal linex1,  DtReal liney1,
    DtReal linex2,  DtReal liney2,
    DtReal point1x, DtReal point1y,
    DtReal point2x, DtReal point2y)
{
    DtReal dx, dy, dx1, dx2, dy1, dy2;
    DtReal samevalue;

    dx = linex2 - linex1;
    dy = liney2 - liney1;
    dx1 = point1x - linex1;
    dy1 = point1y - liney1;
    dx2 = point2x - linex2;
    dy2 = point2y - liney2;

    samevalue = (dx*dy1-dy*dx1) * (dx*dy2-dy*dx2);

    if (samevalue > 0.)
	return(1);
    else if (samevalue < 0.)
	return(-1);
    else
	return(0);
}



/*==========================================================================*/

void dor_math_vectrns (
    DtReal origx, DtReal origy, DtReal origz,
    DtMatrix4x4 matrix,
    DtReal *transx, DtReal *transy, DtReal *transz)
{
    DtVector4 invec, outvec;

    invec[0] = origx;
    invec[1] = origy;
    invec[2] = origz;
    invec[3] = 0.;

    dor_matrix_vectormultiply(invec,matrix,outvec);

    *transx = outvec[0];
    *transy = outvec[1];
    *transz = outvec[2];
}



/*============================================================================
**  Transform a vector (3 reals) into another vector (3 reals) through a 
**  4x4 transformation matrix.  Note that we assume that the 4th component 
**  of the input vector is 0., and that we don't compute the 4th component 
**  of the output vector.
============================================================================*/

PRAGMA(OPT_LEVEL 2)

void dor_math_vectrns_fast (
    DtReal      origvector[3],
    DtMatrix4x4 matrix,
    DtReal      transvector[3])
{
    DtInt row, column;
    DtReal newvector[3];

PRAGMA(IVDEP)
    for (row=0;row<3;row++){
	newvector[row] = 0.;

	for (column=0;column<3;column++){
	    newvector[row] += matrix[row][column]*origvector[column];
	}

	transvector[row] = newvector[row];
    }
}



/*==========================================================================*/

DtReal dor_math_angle (DtReal x, DtReal y)
{
    DtReal angle;

    if (y < 0)
	angle = 2.*3.1415943 + dor_math_atan2(y,x);
    else
	angle = dor_math_atan2(y,x);

    return angle;
}



/*==========================================================================*/

DtReal dor_math_acos (DtReal value)
{
    return (DtReal) acos(value);
}



/*==========================================================================*/

DtReal dor_math_asin (DtReal value)
{
    return (DtReal) asin(value);
}



/*==========================================================================*/

DtReal dor_math_atan (DtReal value)
{
    return (DtReal) atan(value);
}



/*==========================================================================*/

DtReal dor_math_atan2 (DtReal x, DtReal y)
{
    return (DtReal) atan2(x,y);
}



/*==========================================================================*/

DtReal dor_math_cos (DtReal value)
{
    return (DtReal) cos(value);
}



/*==========================================================================*/

DtReal dor_math_sin (DtReal value)
{
    return (DtReal) sin(value);
}



/*==========================================================================*/

DtReal dor_math_ceil (DtReal value)
{
    return (DtReal) ceil(value);
}



/*==========================================================================*/

DtReal dor_math_fabs (DtReal arg)
{
    return (arg < 0) ? -arg : arg;
}



/*==========================================================================*/

DtReal dor_math_floor (DtReal value)
{
    return (DtReal) floor(value);
}



/*============================================================================
**  FASTROOT -- Fast, limited accuracy, DSQRT
**  Initial approximation is "magic" table lookup taken from the Berkeley 4.3
**  routine by Kahan and Ng.  Followed by two steps of Newton's method.  This
**  provides enough accuracy so that conversion to single precision almost
**  always gives the correctly rounded result.
**  -- Cleve Moler, Ardent, 1/13/88.
============================================================================*/

#ifdef FAST_SQRT
    static unsigned long table[] = {
	    0,  1204,  3062,  5746,  9193, 13348, 18162, 23592,
	29598, 36145, 43202, 50740, 58733, 67158, 75992, 85215,
	83599, 71378, 60428, 50647, 41945, 34246, 27478, 21581,
	16499, 12183,  8588,  5674,  3403,  1742,   661,   130 };
#endif

DtReal dor_math_fast_sqrt (DtReal x)
{
#ifndef FAST_SQRT
    return sqrt (x);
#else
    /*
    float zero = 0.0;
    */
    double y,t;
    unsigned long *py=(unsigned long *) &y;

    /* Exceptions */
#   if 0
      if (x!=x || x==0.0) return x;	/* sqrt(NaN) is NaN, sqrt(+-0) = +-0 */
      if (x<0) return (zero/zero);	/* sqrt(negative) is invalid */
      if ((x+x)==x) return x;		/* sqrt(+INF) is +INF */
#   endif

    /* Magic initial approximation to almost 8 sig. bits */

    y = x;

    py[0] = (py[0]>>1)+0x1ff80000;
    py[0] = py[0]-table[(py[0]>>15)&31];

    /* Heron's rule twice to improve y to almost 36 sig. bits */

    t = x/y + y;
    y = x/t + 0.25*t;

    return ((DtReal)y);
#endif
}



/*==========================================================================*/

DtReal dor_math_log (DtReal value)
{
    return (DtReal) log(value);
}



/*==========================================================================*/

DtFlag dor_math_pixel_window (
    DtReal worldxmin, DtReal worldymin,
    DtReal worldxmax, DtReal worldymax,
    DtInt  pixelresolution,
    DtReal windowxmin, DtReal windowymin,
    DtReal windowxmax, DtReal windowymax,
    DtInt *pixelwinxmin, DtInt *pixelwinymin,
    DtInt *pixelwinxmax, DtInt *pixelwinymax)
{
    DtReal pixelsizex;
    DtReal halfpixelsizex;
    DtInt basepixelnumber;
    DtReal basepixelloc;
    DtReal pixeloffset;
    DtReal pixelsizey;
    DtReal halfpixelsizey;

    pixelsizex = (worldxmax-worldxmin) / (DtReal)pixelresolution;
    halfpixelsizex = .5 * pixelsizex;

    basepixelnumber = (int)((windowxmin-worldxmin)/pixelsizex);

    if (basepixelnumber == pixelresolution)
	    basepixelnumber = pixelresolution-1;

    basepixelloc = worldxmin + (DtReal)basepixelnumber*pixelsizex;

    pixeloffset = windowxmin - basepixelloc;

    if (pixeloffset < halfpixelsizex)
	    *pixelwinxmin = basepixelnumber;
    else
	    *pixelwinxmin = basepixelnumber+1;

    if (*pixelwinxmin == pixelresolution)
	    return(DcFalse);

    pixelsizey = (worldymax-worldymin) / (DtReal)pixelresolution;
    halfpixelsizey = .5 * pixelsizey;

    basepixelnumber = (int)((windowymin-worldymin)/pixelsizey);

    if (basepixelnumber == pixelresolution)
	    basepixelnumber = pixelresolution-1;

    basepixelloc = worldymin + (DtReal)basepixelnumber*pixelsizey;

    pixeloffset = windowymin - basepixelloc;

    if (pixeloffset >= halfpixelsizey)
	    *pixelwinymin = basepixelnumber+1;
    else
	    *pixelwinymin = basepixelnumber;

    if (*pixelwinymin == pixelresolution)
	    return(DcFalse);

    basepixelnumber = (int)((windowxmax-worldxmin)/pixelsizex);

    if (basepixelnumber == pixelresolution)
	    basepixelnumber = pixelresolution-1;

    basepixelloc = worldxmin + (DtReal)basepixelnumber*pixelsizex;

    pixeloffset = windowxmax - basepixelloc;

    if (pixeloffset >= halfpixelsizex)
	    *pixelwinxmax = basepixelnumber;
    else
	    *pixelwinxmax = basepixelnumber-1;

    if (*pixelwinxmax == (-1))
	    return(DcFalse);

    basepixelnumber = (int)((windowymax-worldymin)/pixelsizey);

    if (basepixelnumber == pixelresolution)
	    basepixelnumber = pixelresolution-1;

    basepixelloc = worldymin + (DtReal)basepixelnumber*pixelsizey;

    pixeloffset = windowymax - basepixelloc;

    if (pixeloffset >= halfpixelsizey)
	    *pixelwinymax = basepixelnumber;
    else
	    *pixelwinymax = basepixelnumber-1;

    if (*pixelwinymax == (-1))
	    return(DcFalse);

    return(DcTrue);
}



/*==========================================================================*/

DtReal dor_math_pow (DtReal x, DtReal y)
{
    return (DtReal) pow(x,y);
}



/*==========================================================================*/

DtReal dor_math_sqrt (DtReal value)
{
    return (DtReal) sqrt(value);
}
