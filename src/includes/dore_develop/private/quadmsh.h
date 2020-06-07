#ifndef DD_QUADMESH_H
#define DD_QUADMESH_H
 
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
 
typedef DtInt quadmsh_vertexlist[4];

struct quadmsh {
    DtColorModel colormodel;
    DtVertexType vertextype;
    DtInt vertex_count;
    DtRealTriple *vertex_location;
    DtRealTriple *vertex_normal;
    DtColorRGB   *vertex_color;
    DtReal       *vertex_alpha;
    DtInt quad_count;
    quadmsh_vertexlist *quad_vertexlist;
    quadmsh_vertexlist *compiled_quad_vertexlist;
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

void        dor_quadmsh_initialize (void);
dot_object *dor_quadmsh_create
		    (DtColorModel, DtVertexType, DtInt, DtReal [3], DtInt,
		     DtInt [][4], DtFlag);
void        dor_quadmsh_destroy (dot_object *);
void        dor_quadmsh_print (dot_object *);
dot_object *dor_quadmsh_update_alternate (dot_object *);
dot_object *dor_quadmsh_update_lines (dot_object *);
dot_object *dor_quadmsh_update_tris (dot_object *);
dot_object *dor_quadmsh_create_alternate_geom_lines (struct quadmsh *);
dot_object *dor_quadmsh_create_alternate_geom_tris (struct quadmsh *);
DtFlag      dor_quadmsh_check_quad (DtInt, DtInt, DtInt, DtInt);
void        dor_quadmsh_calculate_quad_normals (struct quadmsh *);
void        dor_quadmsh_calculate_vertex_normals (struct quadmsh *);
void        dor_quadmsh_bounding_box (struct quadmsh *);
void        dor_quadmsh_compute_bounding_volume (dot_object *);
void        dor_quadmsh_pick (dot_object *);
void        dor_quadmsh_pick_lines (dot_object *);
void        dor_quadmsh_pick_tris (dot_object *);
void        dor_quadmsh_render_standard (dot_object *);

#endif
