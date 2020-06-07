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
 
#ifndef DD_VARTRILIST_H
#define DD_VARTRILIST_H
 
struct vartrilist {
    DtColorModel colormodel;
    DtInt	 triangle_count;
    DtRealTriple *vertex_location;
    DtRealTriple *vertex_normal;
    DtColorRGB   *vertex_color;
    DtReal       *vertex_alpha;
    DtRealTriple bounding_box_pts[8];
    DtFlag needelemnrms;
    DtFlag decompose_lines;
    
    DtObjectStructure *alt_object_lines;
    DtRealTriple *alt_linevtx;
    DtRealTriple *alt_linenrm;
    DtRealTriple *alt_lineclr;
    DtInt             *edge_enable;	     /* Edge Flags Array */
    DtInt alt_triangle_count;

    DtRealTriple *element_normal;
    DtInt sequence_number; /* incremented when the object
			    is updated with DpUpd.. */

    DtInt uv_count;
    DtRealCouple **uv_list;
    DtInt uvw_count;
    DtRealTriple **uvw_list;
};

    /***  Function Prototypes  ***/

void        dor_vartrilist_initialize (void);
dot_object *dor_vartrilist_create (DtColorModel,DtInt,DtReal*,DtReal*,DtReal*,
				   DtReal*);
void        dor_vartrilist_destroy (dot_object *);
void        dor_vartrilist_print (dot_object *);
dot_object *dor_vartrilist_update_lines (dot_object *);
void        dor_vartrilist_bounding_box (struct vartrilist *);
dot_object *dor_vartrilist_create_alternate_geom_lines (struct vartrilist *);
void        dor_vartrilist_calculate_triangle_normals (struct vartrilist *);
void        dor_vartrilist_compute_bounding_volume (dot_object *);
void        dor_vartrilist_pick (dot_object *);
void        dor_vartrilist_update (dot_object*,DtInt,DtReal*,DtReal*,DtReal*);
void        dor_vartrilist_update_uv (dot_object *, DtInt, DtRealCouple *);
void        dor_vartrilist_update_uvw (dot_object *, DtInt, DtRealTriple *);

#endif
