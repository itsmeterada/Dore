#ifndef DORE_MATRIX_H
#define DORE_MATRIX_H
 
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

#include <dore/internal/dogen.h>
#include <dore/internal/list.h>
 

/*****************************************************************************
**  This file contains function declarations for the Dore' internal matrix
**  operation functions.  Note that dore.h should be included before this
**  header file.
*****************************************************************************/

void    dor_matrix_concat  (DtMatrix4x4, DtMatrix4x4, DtCompType);
DtPtr   dor_matrix_create  (void);
void    dor_matrix_destroy  (DtMatrix4x4);
void    dor_matrix_invert  (DtMatrix4x4);
void    dor_matrix_look_at_from  (DtMatrix4x4,DtReal,DtReal,DtReal,DtReal,
	                      DtReal,DtReal,DtReal,DtReal,DtReal,DtCompType);
void    dor_matrix_load  (DtMatrix4x4, DtMatrix4x4);
void    dor_matrix_load_constant  (DtMatrix4x4, DtReal);
void    dor_matrix_load_identity  (DtMatrix4x4);
void    dor_matrix_load_look_at_from  (DtMatrix4x4,DtReal,DtReal,DtReal,DtReal,
			   DtReal,DtReal,DtReal,DtReal,DtReal);
void    dor_matrix_load_parallel  (DtMatrix4x4, DtReal,DtReal,DtReal);
void    dor_matrix_load_perspective  (DtMatrix4x4, DtReal,DtReal,DtReal);
void    dor_matrix_load_projection  (DtMatrix4x4, DtArea*, DtProjectionType,
                                     DtPoint3, DtReal, DtReal, DtReal);
void    dor_matrix_load_rotate_x  (DtMatrix4x4, DtReal);
void    dor_matrix_load_rotate_y  (DtMatrix4x4, DtReal);
void    dor_matrix_load_rotate_z  (DtMatrix4x4, DtReal);
void    dor_matrix_load_scale  (DtMatrix4x4, DtReal,DtReal,DtReal);
void    dor_matrix_load_shear  (DtMatrix4x4,DtMajorPlane,DtReal,DtReal);
void    dor_matrix_load_shear_z  (DtMatrix4x4,DtReal,DtReal);
void    dor_matrix_load_translate  (DtMatrix4x4, DtReal,DtReal,DtReal);
void    dor_matrix_multiply  (DtMatrix4x4, DtMatrix4x4, DtMatrix4x4);
void    dor_matrix_new_vectormultiply (DtVector4,DtMatrix4x4,DtVector4);
void    dor_matrix_parallel  (DtMatrix4x4,DtReal,DtReal,DtReal,DtCompType);
void    dor_matrix_perspective  (DtMatrix4x4, DtReal, DtReal, DtReal, DtCompType);
void    dor_matrix_post_concatenate  (DtMatrix4x4, DtMatrix4x4);
void    dor_matrix_pre_concatenate  (DtMatrix4x4, DtMatrix4x4);
void    dor_matrix_print  (DtMatrix4x4);
void    dor_matrix_rotate_x  (DtMatrix4x4, DtReal, DtCompType);
void    dor_matrix_rotate_y  (DtMatrix4x4, DtReal, DtCompType);
void    dor_matrix_rotate_z  (DtMatrix4x4, DtReal, DtCompType);
void    dor_matrix_scale  (DtMatrix4x4, DtReal, DtReal, DtReal, DtCompType);
void    dor_matrix_shear  (DtMatrix4x4, DtMajorPlane, DtReal, DtReal, DtCompType);
void    dor_matrix_shear_z  (DtMatrix4x4, DtReal, DtReal, DtCompType);
DtShort dor_matrix_solve  (DtReal[4][4], DtReal[4][4], DtReal[4][4]);
void    dor_matrix_translate  (DtMatrix4x4, DtReal, DtReal, DtReal, DtCompType);
void    dor_matrix_transpose  (DtMatrix4x4);
void    dor_matrix_vectormultiply  (DtVector4, DtMatrix4x4, DtVector4);
void    dor_matrix_load3x3  (DtMatrix3x3, DtMatrix3x3);
void    dor_matrix_load_constant3x3  (DtMatrix3x3, DtReal);
void    dor_matrix_load_identity3x3  (DtMatrix3x3);
void    dor_matrix_print3x3  (DtMatrix3x3);

#endif
