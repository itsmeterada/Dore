#ifndef DD_SIMPLY_H
#define DD_SIMPLY_H
 
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

struct simply {
    DtColorModel colormodel;
    DtVertexType vertextype;
    DtInt vertex_count;
    DtRealTriple *vertex_location;
    DtRealTriple *vertex_normal;
    DtColorRGB   *vertex_color;
    DtReal       *vertex_alpha;
    DtShapeType shape;
    DtObjectStructure *alt_object_lines;
    DtObjectStructure *alt_object_tris;
    DtVertexType fullvtxtype;
    DtInt uv_count;
    DtRealCouple **uv_list;
    DtInt uvw_count;
    DtRealTriple **uvw_list;
};


    /***  Function Prototypes  ***/

DtFlag      dor_simply_check_complex_edge
		(struct simply *, dot_gpolyd_interpolated_vertex *,
		 dot_gpolyd_interpolated_vertex *);
void        dor_simply_compute_bounding_volume  (dot_object *);
dot_object* dor_simply_create
		(DtColorModel, DtVertexType, DtInt, DtReal *, DtShapeType);
void        dor_simply_create_alternate_geometry_complex_triangle
		(struct simply *, dot_gpolyd_interpolated_vertex *,
		 dot_gpolyd_interpolated_vertex *,
		 dot_gpolyd_interpolated_vertex *);
void        dor_simply_create_alternate_geometry_simple_triangle
		(struct simply *, DtInt, DtInt, DtInt);
dot_object* dor_simply_create_alternate_lines (struct simply *);
dot_object* dor_simply_create_alternate_tris (struct simply *);
void        dor_simply_destroy (dot_object *);
DtInt       dor_simply_enable_edges_complex
		(struct simply *, dot_gpolyd_interpolated_vertex *,
		 dot_gpolyd_interpolated_vertex *,
		 dot_gpolyd_interpolated_vertex *);
DtInt       dor_simply_enable_edges_simple  (struct simply *,DtInt,DtInt,DtInt);
void        dor_simply_initialize  (void);
void        dor_simply_pick  (dot_object *);
void        dor_simply_pick_lines  (dot_object *);
void        dor_simply_pick_tris  (dot_object *);
void        dor_simply_print  (struct object *);
void        dor_simply_render_standard  (dot_object *);
dot_object* dor_simply_update_alternate  (dot_object *);
dot_object* dor_simply_update_lines  (dot_object *);
dot_object* dor_simply_update_tris  (dot_object *);

#endif
