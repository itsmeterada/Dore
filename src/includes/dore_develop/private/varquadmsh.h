#ifndef DD_VARQUADMSH_H
#define DD_VARQUADMSH_H
 
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
 
typedef DtInt varquadmsh_vertexlist[4];

struct varquadmsh {
	DtColorModel colormodel;
	DtInt vertex_count;
	DtRealTriple *vertex_location;
	DtRealTriple *vertex_normal;
	DtColorRGB *vertex_color;
	DtInt quad_count;
	varquadmsh_vertexlist *quad_vertexlist;
	varquadmsh_vertexlist *compiled_quad_vertexlist;
	DtRealTriple *element_normal;
	DtFlag smoothflag;
	DtFlag needelemnrms;
	DtFlag needvertnrms;
	DtRealTriple bounding_box_pts[8];

	DtObjectStructure *alt_object_tris;
        DtIntTriple  *alt_triangle_vertexlist;

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

void        dor_varquadmsh_bounding_box  (struct varquadmsh *);
void        dor_varquadmsh_calculate_quad_normals  (struct varquadmsh *);
void        dor_varquadmsh_calculate_vertex_normals  (struct varquadmsh *);
void        dor_varquadmsh_compute_bounding_volume  (dot_object *);
dot_object  *dor_varquadmsh_create  (DtColorModel, DtInt, DtReal *, DtReal *,
                DtReal *, DtInt, DtInt [][4], DtFlag);
dot_object  *dor_varquadmsh_create_alternate_geom_lines  (struct varquadmsh *);
dot_object  *dor_varquadmsh_create_alternate_geom_tris  (struct varquadmsh *);
void        dor_varquadmsh_destroy  (dot_object *);
void        dor_varquadmsh_pick  (dot_object *);
void        dor_varquadmsh_pick_lines  (dot_object *);
void        dor_varquadmsh_pick_tris  (dot_object *);
void        dor_varquadmsh_initialize  (void);
void        dor_varquadmsh_print  (dot_object *);
void        dor_varquadmsh_render_standard  (dot_object *);
void        dor_varquadmsh_update  (dot_object *, DtReal *, DtReal *,
                DtReal *, DtFlag);
dot_object  *dor_varquadmsh_update_alternate  (dot_object *);
dot_object  *dor_varquadmsh_update_lines  (dot_object *);
dot_object  *dor_varquadmsh_update_tris  (dot_object *);
void        dor_varquadmsh_update_uv  (dot_object *, DtInt, DtRealCouple *);
void        dor_varquadmsh_update_uvw  (dot_object *, DtInt, DtRealTriple *);

#endif
