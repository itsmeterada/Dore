#ifndef DD_TRILST_H
#define DD_TRILST_H
 
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
The following structure provides information pertaining to triangle lists.
The number of triangles in the list is given by the `triangle_count' field.
The location array is always filled with vertex locations, and the normal and
color fields may also be filled; the vertextype determines which (this can
be DcLoc, or DcLocNrm, and so on).  The element_normal array is the calculated
normal array for each triangle in the list -- it provides the normal to each
triangle.

Triangle lists can be built via dor_trilst_open(), dor_trilst_close(), and
dor_trilst_add() (plus other associated functions) in
dore/src/objs/pri/trilst.c.  These functions are used to generate lists of
triangles (e.g. for NURBs, subdivisions and so on).  To facilitate this, the
triangle_space field contains the number of triangles that the associated
arrays have space for, and space_estimate is the a priori estimate of the
total space needed.

edge_enable is an array of integers, one for each triangle.  The
least significant 3 bits of each integer indicate whether the corresponding
edge of that triangle is to be rendered.  If the bit is 0, then the edge is
not to be rendered; if the bit is 1, then the edge is to be rendered.  If this
array is not available, it will be set to NULL.
*****************************************************************************/

struct trilst {
    DtColorModel       colormodel;	     /* Color Information Format */
    DtInt              triangle_count;	     /* # Triangles in List */
    DtVertexType       vertextype;	     /* Vertex Information Given */
    DtRealTriple      *vertex_location;	     /* Array of Locations */
    DtRealTriple      *vertex_normal;	     /* Array of Normal Vectors */
    DtColorRGB        *vertex_color;	     /* Array of Colors */
    DtReal            *vertex_alpha;         /* Array of Alpha values */
    DtRealTriple      *element_normal;	     /* Array of Triangle Normals */
    DtInt              triangle_space;	     /* # Tri's We Have Space For */
    DtInt              space_estimate;	     /* # Tri's We Think We'll Need */
    DtRealTriple       bounding_box_pts[8];  /* Enclosing Bounding Box */
    DtInt              uv_count;	     /* # UV Values per Vertex */
    DtRealCouple     **uv_list;		     /* Array of UV Values */
    DtInt              uvw_count;	     /* # UVW Values per Vertex */
    DtRealTriple     **uvw_list;	     /* Array of UVW Values */
    DtObjectStructure *alt_object_lines;     /* Wireframe Object */
    DtInt             *edge_enable;	     /* Edge Flags Array */
};

    /***  Function Prototypes  ***/

void        dor_trilst_initialize (void);
dot_object *dor_trilst_create (DtColorModel, DtVertexType, DtInt, DtReal [3]);
void        dor_trilst_destroy (dot_object *);
void        dor_trilst_print (dot_object *);
dot_object *dor_trilst_update_lines (dot_object *);
dot_object *dor_trilst_open (DtColorModel, DtVertexType, DtInt);
void        dor_trilst_close (dot_object *);
void        dor_trilst_bounding_box (struct trilst *);
dot_object *dor_trilst_create_alternate_lines (struct trilst *);
void        dor_trilst_calculate_triangle_normals (struct trilst *);
void        dor_trilst_compute_bounding_volume (dot_object *);
void        dor_trilst_pick (dot_object *);
void        dor_trilst_add_triangle (dot_object *, DtReal *);
void        dor_trilst_add_triangle_space (struct trilst *);
void        dor_trilst_add_triangle_with_edges (dot_object *, DtReal *, DtInt);
void        dor_trilst_add_triangle_with_edges_space (struct trilst *);

#endif
