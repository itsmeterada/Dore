#ifndef DD_PLYLIN_H
#define DD_PLYLIN_H
 
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

struct plylin {
    DtColorModel   colormodel;
    DtVertexType   vertextype;
    DtInt          vertex_count;
    DtRealTriple  *vertex_location;
    DtRealTriple  *vertex_normal;
    DtColorRGB    *vertex_color;
    DtReal        *vertex_alpha;
    DtRealTriple   bounding_box_pts[8];
    DtInt          uv_count;
    DtRealCouple **uv_list;
    DtInt          uvw_count;
    DtRealTriple **uvw_list;
};

    /***  Function Prototypes  ***/

void        dor_plylin_initialize  (void);
dot_object *dor_plylin_create  (DtColorModel, DtVertexType, DtInt, DtReal *);
void        dor_plylin_destroy  (struct object *);
void        dor_plylin_print  (struct object *);
void        dor_plylin_bounding_box  (struct plylin *);
void        dor_plylin_compute_bounding_volume  (dot_object *);
void        dor_plylin_pick  (dot_object *);

#endif
