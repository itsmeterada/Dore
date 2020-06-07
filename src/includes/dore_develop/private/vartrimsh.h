#ifndef DD_VARTRIMSH_H
#define DD_VARTRIMSH_H
 
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

typedef DtInt vartrimsh_vertexlist[3];

struct vartrimsh {
    DtColorModel colormodel;
    DtInt vertex_count;
    DtRealTriple *vertex_location;
    DtRealTriple *vertex_normal;
    DtColorRGB   *vertex_color;
    DtReal       *vertex_alpha;
    DtInt triangle_count;
    vartrimsh_vertexlist *triangle_vertexlist;
    vartrimsh_vertexlist *compiled_triangle_vertexlist;
    DtRealTriple *element_normal;
    DtFlag smoothflag;
    DtFlag needelemnrms;
    DtFlag needvertnrms;
    DtRealTriple bounding_box_pts[8];

    DtFlag decompose_lines;
    DtObjectStructure *alt_object_lines;
    DtRealTriple *alt_linevtx;
    DtRealTriple *alt_linenrm;
    DtRealTriple *alt_lineclr;
    DtInt sequence_number; /* incremented when the object
			    is updated with DpUpd.. */

    DtInt uv_count;
    DtRealCouple **uv_list;
    DtInt uvw_count;
    DtRealTriple **uvw_list;
    DtInt *edge_enable;
};

    /***  Function Prototypes  ***/

void        dor_vartrimsh_initialize (void);
dot_object *dor_vartrimsh_create (DtColorModel, DtInt, DtReal*, DtReal*,
				  DtReal*, DtInt, DtInt[][3], DtFlag);
dot_object *dor_vartrimsh_create_with_edges
		    (DtColorModel, DtInt, DtReal*, DtReal*, DtReal*, DtInt,
		     DtIntTriple*, DtFlag, DtInt*);
void        dor_vartrimsh_destroy (dot_object *);
void        dor_vartrimsh_print (dot_object *);
dot_object *dor_vartrimsh_update_lines (dot_object *);
void        dor_vartrimsh_bounding_box (struct vartrimsh *);
dot_object *dor_vartrimsh_create_alternate_geom_lines (struct vartrimsh *);
void        dor_vartrimsh_calculate_triangle_normals (struct vartrimsh *);
void        dor_vartrimsh_calculate_vertex_normals (struct vartrimsh *);
void        dor_vartrimsh_compute_bounding_volume (dot_object *);
void        dor_vartrimsh_pick (dot_object *);
void        dor_vartrimsh_update (dot_object*,DtReal*,DtReal*,DtReal*,DtFlag);
void        dor_vartrimsh_update_uv (dot_object *, DtInt, DtRealCouple *);
void        dor_vartrimsh_update_uvw (dot_object *, DtInt, DtRealTriple *);

#endif
