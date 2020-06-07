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
#include "dore_util.h"

/*
	Returns Dore primitive object that represents a rectangular
	grid surface.  The "vertices" array is a logically two dimensional
	array of Dore vertices.  The dimensions are given by "rows" and
	"cols".  rect_mesh() creates a connectivity array to describe the
	triangles in the mesh, and returns a Dore triangle mesh object
	created from this array and the vertices given.
*/


typedef DtInt int3[3];


DtObject rect_mesh (
    DtColorModel colormodel,
    DtVertexType vertextype,
    DtInt rows,
    DtInt cols,
    DtReal vertices[],
    DtFlag smoothflag)
{
	DtInt i, j, tricount;
	DtObject trimsh;
	int3 *tris;

	if(colormodel != DcRGB)
		{
		printf("rect_mesh: unknown colormodel %d\n", colormodel);
		return(DcNullObject);
		}
	tris = (int3 *)malloc((rows - 1) * (cols - 1) * 6 * sizeof(DtInt));
	if(NULL == tris)
		{
		printf("rect_mesh: malloc failed!\n");
		return(DcNullObject);
		}
	tricount = 0;
	for(i=0; i<rows-1; i++)
		for(j=0; j<cols-1; j++)
			{
			tris[tricount][0] = cols*i + j;
			tris[tricount][1] = cols*i + (j+1);
			tris[tricount][2] = cols*(i+1) + (j+1);
			tricount++;
			tris[tricount][0] = cols*i + j;
			tris[tricount][1] = cols*(i+1) + (j+1);
			tris[tricount][2] = cols*(i+1) + j;
			tricount++;
			}
	trimsh = DoTriangleMesh(colormodel, vertextype,
		rows*cols, vertices, tricount, (DtInt *)tris, smoothflag);
	free(tris);
	return(trimsh);
}




DtObject RCTMSH (
    DtInt *colormodel,
    DtInt *vertextype,
    DtInt *rows,
    DtInt *cols,
    DtReal vertices[],
    DtInt *smoothflag)
{
	return(rect_mesh(*colormodel, *vertextype,
		*rows, *cols, vertices, *smoothflag));
}
