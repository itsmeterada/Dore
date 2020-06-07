#ifndef DD_TEXT_H
#define DD_TEXT_H
 
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

struct text {
    DtPoint3 position;
    DtVector3 u;
    DtVector3 v;
    char *string;
    DtTextAlignHorizontal halign;
    DtTextAlignVertical valign;
    DtReal textexp;
    DtFont font;
    DtReal textheight;
    DtTextPath textpath;
    DtTextPrecision precision;
    DtReal textspace;
    DtReal xup, yup;
    DtMatrix4x4 planematrix;
    DtObjectStructure *alternate_object_lines;
    DtObjectStructure *alternate_object_triangles;
};

struct text_decomp_data {
    DtRealTriple *wcsvertices;
    DtObjectStructure *alternate_object_triangles;
};

    /***  Function Prototypes  ***/

void        dor_text_initialize (void);
dot_object *dor_text_create (DtPoint3, DtVector3, DtVector3, char *);
void        dor_text_destroy (dot_object *);
void        dor_text_print (dot_object *);
dot_object *dor_text_update_alternate (dot_object *);
dot_object *dor_text_update_lines (dot_object *);
dot_object *dor_text_update_triangles (dot_object *);
void        dor_text_create_alternate_geometry_complex_triangle
		    (struct text_decomp_data *,
		     dot_gpolyd_interpolated_vertex *,
		     dot_gpolyd_interpolated_vertex *,
		     dot_gpolyd_interpolated_vertex *);
void        dor_text_create_alternate_geometry_line_polygon
		    (struct text *, DtInt, DtRealTriple *);
void        dor_text_create_alternate_geometry_line_polyline
		    (struct text *, DtInt, DtRealTriple *);
void        dor_text_create_alternate_geometry_lines (struct text *);
void        dor_text_create_alternate_geometry_simple_triangle
		    (struct text_decomp_data *, DtInt, DtInt, DtInt);
void        dor_text_create_alternate_geometry_triangle_error
		    (struct text *, DtInt, DtRealTriple *);
void        dor_text_create_alternate_geometry_triangle_polygon
		    (struct text *, DtInt, DtRealTriple *);
void        dor_text_create_alternate_geometry_triangles (struct text *);
DtFlag      dor_text_check_attributes (struct text *);
void        dor_text_calculate_text_plane
		    (DtPoint3, DtVector3, DtVector3, DtMatrix4x4);
void        dor_text_compute_bounding_volume (dot_object *);
void        dor_text_cmpbndvol_point_callback
		    (struct text *, DtInt, DtRealTriple *);
DtInt       dor_text_enable_edges_complex
		    (dot_gpolyd_interpolated_vertex *,
		     dot_gpolyd_interpolated_vertex *,
		     dot_gpolyd_interpolated_vertex *);
DtFlag      dor_text_check_complex_edge
		    (dot_gpolyd_interpolated_vertex *,
		    dot_gpolyd_interpolated_vertex *);
DtInt       dor_text_enable_edges_simple (DtInt, DtInt, DtInt);
void        dor_text_pick (dot_object *);
void        dor_text_pick_lines (dot_object *);
void        dor_text_pick_triangles (dot_object *);
void        dor_text_render_standard (dot_object *);

#endif
