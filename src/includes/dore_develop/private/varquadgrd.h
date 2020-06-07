#ifndef DD_VARQUADGRD_H
#define DD_VARQUADGRD_H
 
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
 
typedef DtInt varquadgrid_vertexlist[3];

struct varquadgrid {
    DtColorModel colormodel;
    DtInt width;
    DtInt height;
    DtRealTriple *vertex_location;
    DtRealTriple *vertex_normal;
    DtColorRGB *vertex_color;
    DtReal *vertex_alpha;
    DtRealTriple *element_normal;
    DtFlag smoothflag;
    DtFlag needelemnrms;
    DtFlag needvertnrms;
    DtRealTriple bounding_box_pts[8];

    DtObjectStructure *alt_object_tris;
    DtIntTriple  *alt_trivtxlist;

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

void        dor_varquadgrid_bounding_box  (struct varquadgrid *);
void        dor_varquadgrid_calculate_quad_normals  (struct varquadgrid *);
void        dor_varquadgrid_calculate_vertex_normals  (struct varquadgrid *);
void        dor_varquadgrid_compute_bounding_volume  (dot_object *);
dot_object* dor_varquadgrid_create
		(DtColorModel, DtInt, DtInt, DtReal*, DtReal*, DtReal*, DtFlag);
dot_object* dor_varquadgrid_create_alternate_geom_lines  (struct varquadgrid *);
dot_object* dor_varquadgrid_create_alternate_geom_tris  (struct varquadgrid *);
void        dor_varquadgrid_destroy  (dot_object *);
void        dor_varquadgrid_initialize  (void);
void        dor_varquadgrid_pick  (dot_object *);
void        dor_varquadgrid_pick_lines  (dot_object *);
void        dor_varquadgrid_pick_tris  (dot_object *);
void        dor_varquadgrid_print  (dot_object *);
void        dor_varquadgrid_render_standard  (dot_object *);
void        dor_varquadgrid_transfer_vtx_info (DtInt,DtInt,struct varquadgrid*);
void        dor_varquadgrid_update
		(dot_object *, DtReal *, DtReal *, DtReal *, DtFlag);
dot_object* dor_varquadgrid_update_alternate  (dot_object *);
dot_object* dor_varquadgrid_update_lines  (dot_object *);
dot_object* dor_varquadgrid_update_tris  (dot_object *);
void        dor_varquadgrid_update_uv  (dot_object *, DtInt, DtRealCouple *);
void        dor_varquadgrid_update_uvw  (dot_object *, DtInt, DtRealTriple *);

#endif
