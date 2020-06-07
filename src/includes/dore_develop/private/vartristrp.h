#ifndef DD_VARTRISTRIP_H
#define DD_VARTRISTRIP_H
 
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

struct vartristrip {
    DtColorModel colormodel;
    DtRealTriple *vertex_location;
    DtRealTriple *vertex_normal;
    DtColorRGB   *vertex_color;
    DtReal       *vertex_alpha;
    DtInt vertex_count;
    DtRealTriple bounding_box_pts[8];
    DtFlag needelemnrms;
    DtFlag decompose_lines;
    
    DtObjectStructure *alt_object_lines;
    DtRealTriple *alt_linevtx;
    DtRealTriple *alt_linenrm;
    DtRealTriple *alt_lineclr;
    DtInt alt_vertexcount;

    DtRealTriple *element_normal;
    DtIntTriple *triangle_vertexlist;
    DtIntTriple *compiled_triangle_vertexlist;
    DtInt sequence_number; /* incremented when the object
			    is updated with DpUpd.. */

    DtInt uv_count;
    DtRealCouple **uv_list;
    DtInt uvw_count;
    DtRealTriple **uvw_list;
};

    /***  Function Prototypes  ***/

void        dor_vartristrip_initialize (void);
dot_object *dor_vartristrip_create (DtColorModel,DtInt,DtReal*,DtReal*,DtReal*);
void        dor_vartristrip_destroy (dot_object *);
void        dor_vartristrip_print (dot_object *);
dot_object *dor_vartristrip_update_lines (dot_object *);
void        dor_vartristrip_bounding_box (struct vartristrip *);
dot_object *dor_vartristrip_create_alternate_geom_lines (struct vartristrip *);
void        dor_vartristrip_calculate_triangle_normals (struct vartristrip *);
void        dor_vartristrip_compute_bounding_volume (dot_object *);
void        dor_vartristrip_pick (dot_object *);
void        dor_vartristrip_update (dot_object*,DtInt,DtReal*,DtReal*,DtReal*);
void        dor_vartristrip_update_uv (dot_object *, DtInt, DtRealCouple *);
void        dor_vartristrip_update_uvw (dot_object *, DtInt, DtRealTriple *);

#endif
