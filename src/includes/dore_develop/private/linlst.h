#ifndef DD_LINLST_H
#define DD_LINLST_H
 
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

struct linlst {
    DtColorModel   colormodel;
    DtInt          line_count;
    DtVertexType   vertextype;
    DtRealTriple  *vertex_location;
    DtRealTriple  *vertex_normal;
    DtColorRGB    *vertex_color;
    DtReal        *vertex_alpha;
    DtInt          space_estimate;
    DtInt          line_space;
    DtRealTriple   bounding_box_pts[8];
    DtInt          uv_count;
    DtRealCouple **uv_list;
    DtInt          uvw_count;
    DtRealTriple **uvw_list;
};

    /***  Function Prototypes  ***/

void        dor_linlst_initialize  (void);
dot_object *dor_linlst_create  (DtColorModel, DtVertexType, DtInt, DtReal *);
void        dor_linlst_destroy  (dot_object *);
void        dor_linlst_print  (dot_object *);
void        dor_linlst_add_line  (dot_object *, DtReal *);
void        dor_linlst_add_line_space  (struct linlst *);
void        dor_linlst_bounding_box  (struct linlst *);
void        dor_linlst_close  (dot_object *);
void        dor_linlst_compute_bounding_volume  (dot_object *);
dot_object *dor_linlst_open  (DtColorModel, DtVertexType, DtInt);
void        dor_linlst_pick  (dot_object *);

#endif
