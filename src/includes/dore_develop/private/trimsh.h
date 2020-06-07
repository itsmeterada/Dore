#ifndef DD_TRIMSH_H
#define DD_TRIMSH_H
 
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

/*****************************************************************************
The following structure provides information for a mesh of triangles.  The
vertex_location array is always filled with each vertex coordinate.  The
vertex_normal array will be filled if the vertex type is DcLocNrm or
DcLocNrmClr.  The vertex_color array is filled if the vertex type is DcLocClr
or DcLocNrmClr.

The element_normal array contains the calculated normal vectors for each
triangle in the mesh.

The compiled_triangle_vertexlist is an array of triangle_count indices that
are scaled by 3 (e.g., compiled_triangle_vertexlist[n][0] =
3 * triangle_vertexlist[n][0]).  This is useful when the vertex data arrays
need to be accessed as one-dimensional arrays.
*****************************************************************************/

typedef DtInt trimsh_vertexlist[3];

struct trimsh {
    DtColorModel       colormodel;		/* Color Format */
    DtVertexType       vertextype;		/* Vertex Info Present */
    DtInt              vertex_count;		/* # Vertices in Mesh */
    DtRealTriple      *vertex_location;		/* Vertex Location Array */
    DtRealTriple      *vertex_normal;		/* Vertex Normal Array */
    DtColorRGB        *vertex_color;		/* Vertex Color Array */
    DtReal            *vertex_alpha;		/* Vertex Alpha Array */
    DtInt              triangle_count;		/* # Triangles in Mesh */
    trimsh_vertexlist *triangle_vertexlist;	/* Triangle Vert Index Array */
    trimsh_vertexlist *compiled_triangle_vertexlist;
    DtRealTriple      *element_normal;		/* Triangle Normal Vec Array */
    DtFlag             smoothflag;		/* If true, smooth surface. */
    DtRealTriple       bounding_box_pts[8];	/* Bounding Box Corners */
    DtInt              uv_count;		/* # UV Values per Vertex */
    DtRealCouple     **uv_list;			/* Array of UV Value Arrays */
    DtInt              uvw_count;		/* # UVW Values per Vertex */
    DtRealTriple     **uvw_list;		/* Array of UVW Value Arrays */
    DtObjectStructure *alt_object_lines;	/* Wireframe Rep Object */
};

    /***  Function Prototypes  ***/

void        dor_trimsh_initialize (void);
dot_object *dor_trimsh_create (DtColorModel, DtVertexType, DtInt, DtReal [3],
			       DtInt, DtInt [][3], DtFlag);
void        dor_trimsh_destroy (dot_object *);
void        dor_trimsh_print (dot_object *);
dot_object *dor_trimsh_update_lines (dot_object *);
void        dor_trimsh_bounding_box (struct trimsh *);
dot_object *dor_trimsh_create_alternate_lines (struct trimsh *);
void        dor_trimsh_calculate_triangle_normals (struct trimsh *);
void        dor_trimsh_calculate_vertex_normals (struct trimsh *);
void        dor_trimsh_compute_bounding_volume (dot_object *);
void        dor_trimsh_pick (dot_object *);

#endif
