#ifndef DD_ANNTXT_H
#define DD_ANNTXT_H

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

struct anntxt {
    DtPoint3 position;
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
    DtObjectStructure *alternate_object_lines;
    DtObjectStructure *alternate_object_triangles;
};

struct anntxt_decomp_data {
    DtRealTriple *wcsvertices;
    DtObjectStructure *alternate_object_triangles;
};

    /***  Function Prototypes  ***/

void        dor_anntxt_initialize        (void);
dot_object *dor_anntxt_create            (DtPoint3, char *);
void        dor_anntxt_destroy           (dot_object *);
void        dor_anntxt_print             (dot_object *);
dot_object *dor_anntxt_update_alternate  (dot_object *);
dot_object *dor_anntxt_update_lines      (dot_object *);
dot_object *dor_anntxt_update_triangles  (dot_object *);
void        dor_anntxt_create_alternate_geometry_line_polygon
		    (struct anntxt *, DtInt, DtRealTriple *);
void        dor_anntxt_create_alternate_geometry_line_polyline
		    (struct anntxt *, DtInt, DtRealTriple *);
void        dor_anntxt_create_alternate_geometry_lines
		    (struct anntxt *);
void        dor_anntxt_create_alternate_geometry_simple_triangle
		    (struct anntxt_decomp_data *, DtInt, DtInt, DtInt);
void        dor_anntxt_create_alternate_geometry_complex_triangle
		    (struct anntxt_decomp_data *,
		    dot_gpolyd_interpolated_vertex *,
		    dot_gpolyd_interpolated_vertex *,
		    dot_gpolyd_interpolated_vertex *);
void        dor_anntxt_create_alternate_geometry_triangle_error
		    (struct anntxt *, DtInt, DtRealTriple *);
void        dor_anntxt_create_alternate_geometry_triangle_polygon
		    (struct anntxt *, DtInt, DtRealTriple *);
void        dor_anntxt_create_alternate_geometry_triangles
		    (struct anntxt *);
DtFlag      dor_anntxt_check_attributes
		    (struct anntxt *);
void        dor_anntxt_compute_bounding_volume (dot_object *);
DtInt       dor_anntxt_enable_edges_complex (dot_gpolyd_interpolated_vertex *,
				       dot_gpolyd_interpolated_vertex *,
				       dot_gpolyd_interpolated_vertex *);
DtFlag      dor_anntxt_check_complex_edge  (dot_gpolyd_interpolated_vertex *,
				       dot_gpolyd_interpolated_vertex *);
DtInt       dor_anntxt_enable_edges_simple  (DtInt, DtInt, DtInt);
void        dor_anntxt_pick           (dot_object *);
void        dor_anntxt_pick_lines     (dot_object *);
void        dor_anntxt_pick_triangles (dot_object *);

#endif
