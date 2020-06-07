#ifndef DD_PLYMSH_H
#define DD_PLYMSH_H
 
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

struct plymsh {
    DtColorModel colormodel;
    DtVertexType vertextype;
    DtInt vertex_count;
    DtRealTriple *vertex_location;
    DtRealTriple *vertex_normal;
    DtColorRGB *vertex_color;
    DtReal *vertex_alpha;
    DtInt polygon_count;
    DtInt *polygons;
    DtInt *contours;
    DtInt *vertexlist;
    DtFlag smoothflag;
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

void        dor_plymsh_calculate_vertex_normals (struct plymsh *);
DtFlag      dor_plymsh_check_complex_edge
		    (struct plymsh *, dot_gpolyd_interpolated_vertex*,
		     dot_gpolyd_interpolated_vertex*);
void        dor_plymsh_compute_bounding_volume(dot_object *);
dot_object* dor_plymsh_create
		    (DtColorModel,DtVertexType,DtInt, DtReal[], DtInt, DtInt[],
		     DtInt[], DtInt[], DtShapeType, DtFlag);
dot_object* dor_plymsh_create_alternate_geom_lines (struct plymsh *);
dot_object* dor_plymsh_create_alternate_geom_tris (struct plymsh *);
void        dor_plymsh_create_alternate_geometry_complex_triangle
		    (struct plymsh*, dot_gpolyd_interpolated_vertex*,
		     dot_gpolyd_interpolated_vertex*,
		     dot_gpolyd_interpolated_vertex*);
void        dor_plymsh_create_alternate_geometry_simple_triangle
		    (struct plymsh *, DtInt, DtInt, DtInt);
void        dor_plymsh_destroy                (dot_object *);
DtInt       dor_plymsh_enable_edges_complex
		    (struct plymsh *, dot_gpolyd_interpolated_vertex *,
		     dot_gpolyd_interpolated_vertex*,
		     dot_gpolyd_interpolated_vertex*);
DtInt       dor_plymsh_enable_edges_simple (struct plymsh*,DtInt,DtInt,DtInt);
void        dor_plymsh_initialize       (void);
void        dor_plymsh_pick             (dot_object *);
void        dor_plymsh_pick_lines       (dot_object *);
void        dor_plymsh_pick_tris        (dot_object *);
void        dor_plymsh_print            (struct object *);
void        dor_plymsh_render_standard  (dot_object *);
dot_object* dor_plymsh_update_alternate (dot_object *);
dot_object* dor_plymsh_update_lines     (dot_object *);
dot_object* dor_plymsh_update_tris      (dot_object *);

#endif
