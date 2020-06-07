#ifndef DD_PATCH_H
#define DD_PATCH_H
 
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

#include <dore/dore_develop/private/patchmat.h>

struct patch_control {
    short     nrow;
    short     ncol;
    DtNPoint3 corner00;
    DtNPoint3 corner03;
    DtNPoint3 corner30;
    DtNPoint3 corner33;
    DtNPoint3 point[4][4];
};

struct patch {
    DtShort patch_type;
    DtInt   subdivtype;
    DtReal  subdivlvl[5];
    short
      edge0flat:1,
      edge1flat:1,
      edge2flat:1,
      edge3flat:1,
      corner00flat:1,
      corner03flat:1,
      corner30flat:1,
      corner33flat:1,
      has_colors:1;
    DtColorModel          colormodel;
    DtVertexType          vertextype;
    DtObject              lmatrix;
    DtObject              rmatrix;
    struct patch_control *points;
    struct patch_control *colors;
    DtObjectStructure    *alternate_object;
};

    /***  Function Prototypes  ***/

void        dor_patch_initialize  (void);
void        dor_patch_bezinv  (DtReal *, int);
void        dor_patch_bezconv  (struct matrix *, DtNPoint3 *, struct matrix *);
dot_object *dor_patch_create  (DtColorModel, DtVertexType, dot_object *,
			       DtNPoint3[], dot_object *);
void        dor_patch_destroy  (dot_object *);
void        dor_patch_print  (dot_object *);
dot_object *dor_patch_update_alternate  (dot_object *);
dot_object *dor_patch_create_alternate_geometry  (struct patch *);
void        dor_patch_create_alternate_geometry_clr_triangle
		    (dot_object*, DtNPoint3*, DtNPoint3*, DtNPoint3*,
		     DtNPoint3*, DtNPoint3*, DtNPoint3*, DtNPoint3*, DtNPoint3*,
		     DtNPoint3*);
void        dor_patch_create_alternate_geometry_triangle
		    (dot_object*, DtNPoint3*, DtNPoint3*, DtNPoint3*,
		     DtNPoint3 *, DtNPoint3 *, DtNPoint3 *);
DtFlag      dor_patch_check_subdiv  (struct patch *);
DtFlag      dor_patch_check_triangle
		    (DtNPoint3 *, DtNPoint3 *, DtNPoint3 *);
void        dor_patch_compute_bounding_volume (dot_object *);
void        dor_patch_get_center (dot_object *, DtNPoint3 *);
void        dor_patch_matmul (DtReal *, DtReal *, DtReal *, int, int, int);
void        dor_patch_mattrn (DtReal *, DtReal *, int, int);
void        dor_patch_pick (dot_object *);
void        dor_patch_render_standard (dot_object*);
void        dor_patch_subdivide_adaptive
		    (struct patch*, DtShort, DtPFV, DtPtr);
void        dor_patch_subdivide_fixed
		    (struct patch *, DtInt, DtShort, DtPFV, DtPtr);
void        dor_patch_subdivide_control_along_u
		    (struct patch *, struct patch_control *,
		     struct patch_control *, struct patch_control *);
void        dor_patch_subdivide_control_along_v
		    (struct patch *, struct patch_control *,
		     struct patch_control *, struct patch_control *);
void        dor_patch_subdivide_along_u
		    (struct patch *, struct patch *, struct patch *);
void        dor_patch_subdivide_along_u_and_v
		    (struct patch *, struct patch *, struct patch *,
		     struct patch *, struct patch *);
void        dor_patch_subdivide_along_v
		    (struct patch *, struct patch *, struct patch *);
void        dor_patch_tessellate
		    (struct patch *, DtShort, DtPFV, DtPtr);
void        dor_patch_tessellate_patch
		    (struct patch *, DtShort, DtPFV, DtPtr);
DtFlag      dor_patch_test_linear  (DtNPoint3[4], DtInt, DtReal *);
DtFlag      dor_patch_test_u_boundaries  (struct patch *);
DtFlag      dor_patch_test_v_boundaries  (struct patch *);

#endif
