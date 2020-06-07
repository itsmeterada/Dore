#ifndef DD_QUADGRID_H
#define DD_QUADGRID_H
 
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
 
struct quadgrid {
    DtColorModel colormodel;
    DtVertexType vertextype;
    DtInt width;
    DtInt height;
    DtRealTriple *vertex_location;
    DtRealTriple *vertex_normal;
    DtColorRGB   *vertex_color;
    DtReal       *vertex_alpha;
    DtRealTriple *element_normal;
    DtFlag smoothflag;
    DtRealTriple bounding_box_pts[8];
    DtObjectStructure *alt_object_lines;
    DtObjectStructure *alt_object_tris;
    DtInt uv_count;
    DtRealCouple **uv_list;
    DtInt uvw_count;
    DtRealTriple **uvw_list;
};

    /***  Function Prototypes  ***/

void        dor_quadgrid_initialize (void);
dot_object *dor_quadgrid_create (DtColorModel, DtVertexType, DtInt, DtInt, DtReal [3], DtFlag);
void        dor_quadgrid_destroy (dot_object *);
void        dor_quadgrid_print (dot_object *);
dot_object *dor_quadgrid_update_alternate (dot_object *);
dot_object *dor_quadgrid_update_lines (dot_object *);
dot_object *dor_quadgrid_update_tris (dot_object *);
dot_object *dor_quadgrid_create_alternate_geom_lines (struct quadgrid *);
void        dor_quadgrid_transfer_vtx_info (DtReal [], DtInt, struct quadgrid *, DtInt);
dot_object *dor_quadgrid_create_alternate_geom_tris (struct quadgrid *);
void        dor_quadgrid_calculate_quad_normals (struct quadgrid *);
void        dor_quadgrid_calculate_vertex_normals (struct quadgrid *);
void        dor_quadgrid_bounding_box (struct quadgrid *);
void        dor_quadgrid_compute_bounding_volume (dot_object *);
void        dor_quadgrid_pick (dot_object *);
void        dor_quadgrid_pick_lines (dot_object *);
void        dor_quadgrid_pick_tris (dot_object *);
void        dor_quadgrid_render_standard (dot_object *);

#endif
