#ifndef DD_VARSIMPLYMSH_H
#define DD_VARSIMPLYMSH_H
 
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

struct varsimplymsh {
    DtColorModel colormodel;
    DtInt vertex_count;
    DtRealTriple *vertex_location;
    DtRealTriple *vertex_normal;
    DtColorRGB *vertex_color;
    DtReal *vertex_alpha;
    DtInt polygon_count;
    DtInt *contours;
    DtInt *vertexlist;
    DtShapeType shape;
    DtFlag smoothflag;
    DtFlag needvertnrms;
    DtObjectStructure *alt_object_lines;
    DtInt alt_tricount;
    DtIntTriple  *alt_trilist;
    DtFlag decompose_lines;
    DtObjectStructure *alt_object_tris;
    DtRealTriple *alt_linevtx;
    DtRealTriple *alt_linenrm;
    DtRealTriple *alt_lineclr;
    DtFlag decompose_tris;
    DtInt sequence_number;	/* incremented when the object
			    is updated with DpUpd.. */
    DtInt uv_count;
    DtRealCouple **uv_list;
    DtInt uvw_count;
    DtRealTriple **uvw_list;
    DtInt *edge_enable;
};

    /***  Function Prototypes  ***/

void        dor_varsimplymsh_initialize (void);
dot_object *dor_varsimplymsh_create (DtColorModel, DtInt, DtReal *, DtReal *,
		    DtReal *, DtInt, DtInt [], DtInt [], DtShapeType, DtFlag);
void        dor_varsimplymsh_destroy (dot_object *);
void        dor_varsimplymsh_pick (dot_object *);
void        dor_varsimplymsh_pick_lines (dot_object *);
void        dor_varsimplymsh_pick_tris (dot_object *);
void        dor_varsimplymsh_print (struct object *);
void        dor_varsimplymsh_render_standard (dot_object *);
dot_object *dor_varsimplymsh_update_alternate (dot_object *);
dot_object *dor_varsimplymsh_update_lines (dot_object *);
dot_object *dor_varsimplymsh_update_tris (dot_object *);
void        dor_varsimplymsh_create_alternate_geometry_complex_triangle
		    (struct varsimplymsh *, dot_gpolyd_interpolated_vertex *,
		     dot_gpolyd_interpolated_vertex *,
		     dot_gpolyd_interpolated_vertex *);
dot_object *dor_varsimplymsh_create_alternate_geom_lines
		    (struct varsimplymsh *);
void        dor_varsimplymsh_create_alternate_geometry_simple_triangle
		    (struct varsimplymsh *, DtInt, DtInt, DtInt);
dot_object *dor_varsimplymsh_create_alternate_geom_tris (struct varsimplymsh *);
void        dor_varsimplymsh_calculate_vertex_normals (struct varsimplymsh *);
void        dor_varsimplymsh_compute_bounding_volume (dot_object *);
DtInt       dor_varsimplymsh_enable_edges_simple
		    (struct varsimplymsh *, DtInt, DtInt, DtInt);
void        dor_varsimplymsh_polygon_normal
		    (struct varsimplymsh *, DtInt, DtInt, DtRealTriple);
void        dor_varsimplymsh_update
		    (dot_object*, DtReal*, DtReal*, DtReal*, DtShapeType,
		     DtFlag, DtFlag);
void        dor_varsimplymsh_update_uv (dot_object *, DtInt, DtRealCouple *);
void        dor_varsimplymsh_update_uvw (dot_object *, DtInt, DtRealTriple *);

#endif
