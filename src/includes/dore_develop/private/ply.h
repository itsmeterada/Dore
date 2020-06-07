#ifndef DD_PLY_H
#define DD_PLY_H

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

#include <dore/internal/gpolyd.h>

struct ply {
    DtColorModel       colormodel;
    DtVertexType       vertextype;
    DtInt              contour_count;
    DtInt             *contours;
    DtInt              vertex_count;
    DtRealTriple      *vertex_location;
    DtRealTriple      *vertex_normal;
    DtColorRGB        *vertex_color;
    DtReal            *vertex_alpha;
    DtShapeType        shape;
    DtObjectStructure *alt_object_lines;
    DtObjectStructure *alt_object_tris;
    DtVertexType       fullvtxtype;
    DtInt              uv_count;
    DtRealCouple     **uv_list;
    DtInt              uvw_count;
    DtRealTriple     **uvw_list;
};

    /***  Function Prototypes  ***/

void        dor_ply_initialize  (void);
dot_object *dor_ply_create  (DtColorModel, DtVertexType, DtInt, DtInt*,
			     DtReal*, DtShapeType);
void        dor_ply_destroy  (dot_object *);
void        dor_ply_print  (struct object *);
dot_object *dor_ply_update_alternate  (dot_object *);
void        dor_ply_create_alternate_geometry_complex_triangle
		    (struct ply *, dot_gpolyd_interpolated_vertex *,
		     dot_gpolyd_interpolated_vertex *,
		     dot_gpolyd_interpolated_vertex *);
dot_object *dor_ply_create_alternate_geom_lines  (struct ply *);
void        dor_ply_create_alternate_geometry_simple_triangle
		    (struct ply *, DtInt, DtInt, DtInt);
dot_object *dor_ply_create_alternate_geom_tris  (struct ply *);
void        dor_ply_compute_bounding_volume  (dot_object *);
DtInt       dor_ply_enable_edges_complex
		    (struct ply *, dot_gpolyd_interpolated_vertex *,
		     dot_gpolyd_interpolated_vertex *,
		     dot_gpolyd_interpolated_vertex *);
DtFlag      dor_ply_check_complex_edge
		    (struct ply *, dot_gpolyd_interpolated_vertex *,
		     dot_gpolyd_interpolated_vertex *);
DtInt       dor_ply_enable_edges_simple  (struct ply *, DtInt, DtInt, DtInt);
void        dor_ply_pick  (dot_object *);
void        dor_ply_pick_lines  (dot_object *);
void        dor_ply_pick_tris  (dot_object *);
void        dor_ply_render_standard  (dot_object *);
dot_object *dor_ply_update_lines  (dot_object *);
dot_object *dor_ply_update_tris  (dot_object *);

#endif
