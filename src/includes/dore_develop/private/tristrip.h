#ifndef DD_TRISTRIP_H
#define DD_TRISTRIP_H
 
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

struct tristrip {
    DtColorModel colormodel;
    DtVertexType vertextype;
    DtRealTriple *vertex_location;
    DtRealTriple *vertex_normal;
    DtColorRGB   *vertex_color;
    DtReal       *vertex_alpha;
    DtInt vertex_count;
    DtObjectStructure *alt_object_lines;
    DtRealTriple bounding_box_pts[8];
    DtInt uv_count;
    DtRealCouple **uv_list;
    DtInt uvw_count;
    DtRealTriple **uvw_list;

    DtRealTriple *element_normal;
    DtIntTriple *triangle_vertexlist;
    DtIntTriple *compiled_triangle_vertexlist;
};

    /***  Function Prototypes  ***/

void        dor_tristrip_initialize (void);
dot_object *dor_tristrip_create (DtColorModel, DtVertexType, DtInt, DtReal []);
dot_object *dor_tristrip_create_from_list
		    (DtColorModel, DtVertexType, DtInt, DtRealTriple *,
		     DtRealTriple *, DtRealTriple *, DtRealTriple *,
		     DtRealCouple **, DtRealTriple **, DtFlag);
dot_object *dor_tristrip_create_from_mesh
		    (DtColorModel, DtVertexType, DtInt, DtIntTriple *,
		     DtRealTriple *, DtRealTriple *, DtRealTriple *,
		     DtRealTriple *, DtRealCouple **, DtRealTriple **, DtFlag);
void        dor_tristrip_destroy (dot_object *);
void        dor_tristrip_print (dot_object *);
dot_object *dor_tristrip_update_alternate_lines (dot_object *);
void        dor_tristrip_bounding_box (struct tristrip *);
dot_object *dor_tristrip_create_alternate_lines (struct tristrip *);
void        dor_tristrip_calculate_triangle_normals (struct tristrip *);
void        dor_tristrip_compute_bounding_volume (dot_object *);
void        dor_tristrip_pick (dot_object *);

#endif
