#ifndef DD_PNTLST_H
#define DD_PNTLST_H
 
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

struct pntlst {
    DtColorModel colormodel;
    DtInt point_count;
    DtVertexType vertextype;
    DtRealTriple *vertex_location;
    DtRealTriple *vertex_normal;
    DtColorRGB   *vertex_color;
    DtReal       *vertex_alpha;
    DtInt point_space;
    DtRealTriple bounding_box_pts[8];
    DtInt uv_count;
    DtRealCouple **uv_list;
    DtInt uvw_count;
    DtRealTriple **uvw_list;
};

    /***  Function Prototypes  ***/

void        dor_pntlst_initialize  (void);
dot_object *dor_pntlst_create  (DtColorModel, DtVertexType, DtInt, DtReal *);
void        dor_pntlst_destroy  (dot_object *);
void        dor_pntlst_print  (dot_object *);
void        dor_pntlst_add_point  (dot_object *, DtReal *);
void        dor_pntlst_add_point_space  (struct pntlst *);
void        dor_pntlst_bounding_box  (struct pntlst *);
void        dor_pntlst_close  (dot_object *);
void        dor_pntlst_compute_bounding_volume  (dot_object *);
dot_object *dor_pntlst_open  (DtColorModel, DtVertexType);
void        dor_pntlst_pick  (dot_object *);

#endif
