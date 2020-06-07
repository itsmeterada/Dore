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
makes and returns a tiled surface object.
It takes an object to use as the tile foreground pattern and replecates
it in a checkerboard pattern (rows by cols) using the given foreground
color. A rectangular polygon of the given background color is placed
just behind the pattern array to fill in the gaps.
The given tile object is assumed to be planer and to lie in the xy plane.
It should be roughly one unit wide and high.
The object returned will be a rectangle in the xy plane with one corner
at the origin and extending into positive x and y. It's longest edge
will be one unit long and the ratio of the sides will be the same as
the ratio of rows/cols.
*/

#include <dore/dore.h>
#include "util.h"

DtObject make_tiles(color_model, fgnd_color, bgnd_color, tile_obj, rows, cols)
DtColorModel color_model;
DtReal fgnd_color[], bgnd_color[];
DtObject tile_obj;
DtInt rows, cols;
{
	DtObject DoSimplePolygon();
	DtObject base;
	DtInt i, j, max_edge;
	DtHalfSpace clippers[4];
	static DtReal baseverts[][3] =
		{0.,0.,0.,
		 0.,0.,0.,
		 0.,0.,0.,
		 0.,0.,0.,};

	if((0 >= rows) || (0 >= cols))
		{
		printf("make_floor: bad params rows=%d, cols=%d\n",
			rows, cols);
		return(DcNullObject);
		}

	/* construct the base rectangle */
	baseverts[1][0] = baseverts[2][0] = (DtReal)rows;
	baseverts[2][1] = baseverts[3][1] = (DtReal)cols;
	base = DoSimplePolygon(DcRGB, DcLoc, 4, baseverts, DcConvex);

	clippers[0].point[0] = 0.;
	clippers[0].point[1] = 0.;
	clippers[0].point[2] = 0.;
	clippers[0].vector[0] = 0.;
	clippers[0].vector[1] = 0.;
	clippers[0].vector[2] = -1.;

	clippers[1].point[0] = 0.;
	clippers[1].point[1] = 0.;
	clippers[1].point[2] = 0.;
	clippers[1].vector[0] = 1.;
	clippers[1].vector[1] = 0.;
	clippers[1].vector[2] = 0.;

	clippers[2].point[0] = 0.;
	clippers[2].point[1] = 0.;
	clippers[2].point[2] = (DtReal)rows;
	clippers[2].vector[0] = 0.;
	clippers[2].vector[1] = 0.;
	clippers[2].vector[2] = 1.;

	clippers[3].point[0] = (DtReal)cols;
	clippers[3].point[1] = 0.;
	clippers[3].point[2] = 0.;
	clippers[3].vector[0] = -1.;
	clippers[3].vector[1] = 0.;
	clippers[3].vector[2] = 0.;

	stacked_group(DcTrue, DcFalse);
		max_edge = rows > cols ? rows : cols;
		DgAddObj(DoScale(1.0/max_edge, 1.0/max_edge, 1.0/max_edge));
		DgAddObj(DoRepType(DcSurface));

		/* put down the base (background) sheet */
		DgAddObj(DoDiffuseColor(color_model, bgnd_color));
		DgAddObj(base);

		/* trim the edges of the pattern */
/* commented out until modeling clipping works
		DgAddObj(DoClipSwitch(DcOn));
		DgAddObj(DoClipVol(DcClipAnd, 4, clippers));
*/

		/* move to cut away first row and column, and to raise
		   the tiles slightly above the base plane. */
		DgAddObj(DoTranslate(-1., -1., .01));

		/* instance the tiles in a checkerboard pattern */
		DgAddObj(DoDiffuseColor(color_model, fgnd_color));
		for(i=0; i<rows+2; i++)
			{
			DgAddObj(DoPushMatrix());
				for(j=0; j<cols+2; j++)
					{
					if(0 == (i+j) % 2) /* even */
						DgAddObj(tile_obj);
					DgAddObj(DoTranslate(0., 1., 0.));
					}
			DgAddObj(DoPopMatrix());
			DgAddObj(DoTranslate(1., 0., 0.));
			}
	return(close_stacked_group());
}
