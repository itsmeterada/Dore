#ifndef CLIP3D_H
#define CLIP3D_H
 
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


    /***  Type Definitions  ***/

typedef struct clip3d_vertex {
    DtPoint4 loc;
    DtReal   interpolants[3];
    char     clipcode;
} dot_clip3d_vertex;


    /***  Function Prototypes  ***/

DtInt dor_clip3d_code_vertex       (dot_clip3d_vertex*);
void  dor_clip3d_edge_boundary     (dot_clip3d_vertex*, dot_clip3d_vertex*,
				    DtInt, dot_clip3d_vertex*);
void  dor_clip3d_point             (dot_clip3d_vertex*, DtPFV, DtPtr);
void  dor_clip3d_print_vertex      (dot_clip3d_vertex*);
void  dor_clip3d_test_point        (DtPoint3, DtMatrix4x4, DtPFV, DtPtr);
void  dor_clip3d_test_triangle     (DtPoint3, DtPoint3, DtPoint3, DtMatrix4x4,
				    DtPFV, DtPtr);
void  dor_clip3d_test_triangle_clip_depth_only (DtPoint3, DtPoint3, DtPoint3,
				    DtMatrix4x4, DtPFV, DtPtr);
void  dor_clip3d_test_vector       (DtPoint3,DtPoint3,DtMatrix4x4,DtPFV,DtPtr);
void  dor_clip3d_triangle          (dot_clip3d_vertex*, dot_clip3d_vertex*,
				    dot_clip3d_vertex*, DtSwitch, DtPFV,DtPtr);
void  dor_clip3d_triangle_boundary (dot_clip3d_vertex*, dot_clip3d_vertex*,
				    dot_clip3d_vertex*, DtInt);
void  dor_clip3d_vector            (dot_clip3d_vertex*, dot_clip3d_vertex*,
				    DtSwitch, DtPFV, DtPtr);
void  dor_clip3d_vector_boundary   (dot_clip3d_vertex*, dot_clip3d_vertex*,
				    DtInt);

#endif
