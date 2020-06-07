#ifndef DD_PLYMKR_H
#define DD_PLYMKR_H
 
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

struct plymkr {
    DtInt count;
    DtPoint3 *position;
    DtFont font;
    DtInt glyph;
    DtReal scale;
    DtReal textexp;
    DtReal xup, yup;
    DtObjectStructure *alternate_object_lines;
    DtObjectStructure *alternate_object_triangles;
};

struct plymkr_decomp_data {
    DtRealTriple *wcsvertices;
    DtObjectStructure *alternate_object_triangles;
};

    /***  Function Prototypes  ***/

void        dor_plymkr_initialize  (void);
dot_object *dor_plymkr_create  (DtInt, DtPoint3 *);
void        dor_plymkr_destroy  (dot_object *);
void        dor_plymkr_print  (dot_object *);
dot_object *dor_plymkr_update_alternate  (dot_object *);
void        dor_plymkr_create_alternate_geometry_complex_triangle
		    (struct plymkr_decomp_data *,
		     dot_gpolyd_interpolated_vertex *,
		     dot_gpolyd_interpolated_vertex *,
		     dot_gpolyd_interpolated_vertex *);
void        dor_plymkr_create_alternate_geometry_line_polygon
		    (struct plymkr *, DtInt, DtRealTriple *);
void        dor_plymkr_create_alternate_geometry_line_polyline
		    (struct plymkr *, DtInt, DtRealTriple *);
void        dor_plymkr_create_alternate_geometry_lines  (struct plymkr *);
void        dor_plymkr_create_alternate_geometry_simple_triangle
		    (struct plymkr_decomp_data *, DtInt, DtInt, DtInt);
void        dor_plymkr_create_alternate_geometry_triangle_error
		    (struct plymkr *, DtInt, DtRealTriple *);
void        dor_plymkr_create_alternate_geometry_triangle_polygon
		    (struct plymkr *, DtInt, DtRealTriple *);
void        dor_plymkr_create_alternate_geometry_triangles
		    (struct plymkr *);
DtFlag      dor_plymkr_check_attributes  (struct plymkr *);
void        dor_plymkr_compute_bounding_volume  (dot_object *);
void        dor_plymkr_pick  (dot_object *);
void        dor_plymkr_pick_lines  (dot_object *);
void        dor_plymkr_pick_triangles  (dot_object *);
dot_object *dor_plymkr_update_lines  (dot_object *);
dot_object *dor_plymkr_update_triangles  (dot_object *);

#endif
