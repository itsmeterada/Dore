#ifndef DD_VARPNTLST_H
#define DD_VARPNTLST_H
 
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

struct varpntlst {
    DtColorModel colormodel;
    DtInt point_count;
    DtRealTriple *vertex_location;
    DtRealTriple *vertex_normal;
    DtColorRGB *vertex_color;
    DtReal *vertex_alpha;
    DtInt point_space;
    DtRealTriple bounding_box_pts[8];
    DtInt sequence_number;	/* Incremented when the object
                                ** is updated with DpUpd. */
};
    
    /***  Function Prototypes  ***/

void        dor_varpntlst_initialize  (void);
dot_object *dor_varpntlst_create  (DtColorModel,DtInt,DtReal*,DtReal*,DtReal*);
void        dor_varpntlst_destroy  (dot_object *);
void        dor_varpntlst_print  (dot_object *);
void        dor_varpntlst_bounding_box  (struct varpntlst *);
void        dor_varpntlst_compute_bounding_volume  (dot_object *);
void        dor_varpntlst_pick  (dot_object *);
void        dor_varpntlst_update  (dot_object*, DtInt, DtReal*,DtReal*,DtReal*);

#endif
