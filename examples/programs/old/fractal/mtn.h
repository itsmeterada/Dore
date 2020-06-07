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
 

/*  Header for the fractal triangle data file:
 *
 *	x0, y0		Coordinates for the initial triangle, in
 *	x1, y1		counter clockwise order; point 0 has the
 *	x2, y2		highest y value (with lowest x if tied)
 *
 *	gen		# of generations in file
 *
 *	magic		Data File magic #
 *
 *	flags		Various flags
 *
 *	zcount		# of z entries in file
 */

typedef struct {
	double x0, y0;
	double x1, y1;
	double x2, y2;
	long gen;
	long zcount;
	unsigned long magic;
	unsigned long flags;
} FractalHeader;

#define	FRACTALMAGIC	0x0da1bad1

/*  Flags for fractal header  */

#define	F_COLOR	0x01

/*  The fractal triangle is written in the following order:
 *
 *				0
 *			1		2
 *		3		4		5
 *	6		7		8		9
 *
 *  etc.  This can also be considered as a lower triangular matrix
 *  with a coordinate system as follows:
 *
 *	 0  1  2  3  ...
 *   0	 0
 *   1	 1  2
 *   2	 3  4  5
 *   3	 6  7  8  9
 *   .
 *   .
 */

/*  This macro converts from the x coordiante to the first node in the
 *  corresponding row.
 */

#define	XNODE(x)	(((x) * ((x) + 1)) >> 1)

/*  This macro converts from an x-y coordinate to a node index  */

#define	XY2NODE(x, y)	(XNODE(x) + (y))
