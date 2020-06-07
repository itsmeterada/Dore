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
				SUPER_MESH
				by Daniel Green

	super_mesh creates a simple polygon mesh with edges.
	It takes the same arguments as DoSimplePolygonMesh but
	also takes an edge color and a pick flag argument.
	The edge color argument is just what you think - it is used
	as the color of the wireframe edges drawn on top of the mesh.
	The pick flag argument specifies whether to also add pickable
	point objects at each vertex given by the user. If this flag
	is DcTrue, the pick ID for a point will be the index into the
	vertices array for that point.  The color of the point
	will be the same as that of the edges and so will not be seen.
	Note that this happens regardless of whether that vertex is
	used in the mesh (in which case it might be seen).
*/

#include <dore/dore.h>
#include "dore_util.h"


#define RENDERABLE 0
#define PICKABLE   1



/*
 * This callback function will be passed an array of two different
 * representations of the mesh object.  The first one is the renderable
 * version, and the second is the pickable object (if it was created).
 * This function selects which of the two versions to execute depending
 * upon the current method.  This is faster than just using a single group
 * to hold both representations because the pickable version is not executed
 * during rendering, and the renderable version is ignored during picking.
 * Care is taken to free the space used by the objects and callback data
 * when the callback object is being destroyed.
 */
static mesh_callback_func (
    DtObject representations[2])
{
	switch(DsInqCurrentMethod()) {
		case DcMethodPick:  /* pick on pickable version if it exists */
			if(representations[PICKABLE] != DcNullObject)
				DsExecuteObj(representations[PICKABLE]);
			break;
		case DcMethodDestroy:  /* distroy both representations */
			DsExecuteObj(representations[RENDERABLE]);
			if(representations[PICKABLE] != DcNullObject)
				DsExecuteObj(representations[PICKABLE]);
			free(representations); /* finished with the array */
			break;
		default:  /* render (or whatever) the main object */
			DsExecuteObj(representations[RENDERABLE]);
	}
}




/*
 * super_mesh() works by creating several different representations
 * of a simple polygon mesh object.  One is simply the mesh object
 * itself, another is a line list object that defines the edges of
 * the mesh, and an optional third is a set of pickable point objects
 * that represent the vertices.
 *
 * The line list representation is scaled slightly larger than the
 * mesh representation, and the two objects are put in a group.  This group
 * makes up the visual representation.  The optional group of point
 * objects is the pickable representation.
 *
 * The handles to these two groups are stored in an array which becomes
 * the data for a callback object.  This callback object is the return
 * value of super_mesh().  When executed, this callback object behaves
 * like the pickable representation during picking, and like the the
 * renderable representation otherwise.  The reason for this is so that
 * the renderer does not spend the time to render the points representation
 * during rendering.
 */

DtObject super_mesh (
    DtColorModel colormodel,
    DtVertexType vertextype,
    DtInt vertexcount,
    DtReal vertices[],
    DtInt polygoncount,
    DtInt contours[],
    DtInt vertexlist[],
    DtShapeType shape,
    DtFlag smoothflag,
    DtReal edgecolor[],
    DtFlag pickflag)
{
	DtReal *new_verts;
	int
		bytes_needed, reals_in_vert, edges, contourstart,
		vertno, i, j, p;
	DtObject *objects;

	switch(vertextype)
		{
		case DcLoc:
			reals_in_vert = 3;
			break;
		case DcLocClr:
			reals_in_vert = 6;
			break;
		case DcLocNrm:
			reals_in_vert = 6;
			break;
		case DcLocNrmClr:
			reals_in_vert = 9;
			break;
		default:
			printf("unknown vertex type\n");
			return(DcNullObject);
		}

	/* sum the total number of edges in the mesh */
	for(i=0, edges=0; i<polygoncount; i++)
		edges += contours[i];

	/* calculate space needed for the new line list vertices */
	bytes_needed = 3 * sizeof(DtReal) * edges * 2;

	/* get the space for the line list vertices */
	new_verts = (DtReal *)malloc(bytes_needed);

	if( ! new_verts)
		{
		printf("ERROR couldn't malloc %d bytes\n", bytes_needed);
		exit(1);
		}
	
	/* load the vertex array for the new line list */
	for(i=0, contourstart=0, p=0; i<polygoncount; i++)
		{
		/* add a contour to the new list */
		for(j=0; j<contours[i]; j++)
			{
			/* copy a vertex */
			vertno = vertexlist[contourstart + j];
			memcpy(
				new_verts + p,
				&vertices[reals_in_vert * vertno],
				3 * sizeof(DtReal));

			p += 3;

			/* if last vert in a contour, append first vert,
			   otherwise append next vert in contour */
			if(j + 1 == contours[i])
				vertno = vertexlist[contourstart];
			else
				vertno = vertexlist[contourstart + j + 1];

			memcpy(
				new_verts + p,
				&vertices[reals_in_vert * vertno],
				3 * sizeof(DtReal));

			p += 3;
			}
		contourstart += contours[i];
		}

	/* get space for the handles for the two representations */
	objects = (DtObject *)malloc(2 * sizeof(DtObject));

	/* define the renderable mesh object with edges */
	objects[RENDERABLE] = DoGroup(DcTrue);
		/* make the simple polygon primitive specified by the user */
		DgAddObj(DoSimplePolygonMesh(colormodel, vertextype,
			vertexcount, vertices, polygoncount, contours,
			vertexlist, shape, smoothflag));

		/* add the edges represented by a line list primitive */
		DgAddObj(DoSurfaceShade(DcShaderConstant));
		DgAddObj(DoDiffuseColor(colormodel, edgecolor));
		DgAddObj(DoScale(1.01, 1.01, 1.01));
		DgAddObj(DoLineList(colormodel, DcLoc /* no color */,
			edges, new_verts));
	DsHoldObj(DgClose());

	/* done with the line list vertices */
	free(new_verts);

	/* if the user wants pickable vertices, then define a pickable  */
	/* version (just point objects at the vertices).                */
	if(pickflag)
		{
		objects[PICKABLE] = DoGroup(DcTrue);
			{
			DgAddObj(DoPickSwitch(DcOn));
			for(i=0; i<vertexcount; i++)
				{
				DgAddObj(DoPickID(i));
				DgAddObj(DoPointList(colormodel, DcLoc,
					1, &vertices[reals_in_vert * i]));
				}
			}
		DsHoldObj(DgClose());
		}
	else
		objects[PICKABLE] = DcNullObject;

	return(DoCallback(mesh_callback_func, DoDataPtr(&objects[0])));
}
