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
 

#ifndef DORE_GEOATT_H
#define DORE_GEOATT_H
 
/*****************************************************************************
**  This file contains function prototypes for Dore' geometric attributes.
*****************************************************************************/

dot_object*    dor_lokatfrm_create  (DtPoint3, DtPoint3, DtVector3);
void           dor_lokatfrm_destroy  (dot_object *);
void           dor_lokatfrm_execute  (dot_object *);
void           dor_lokatfrm_initialize  (void);
void           dor_lokatfrm_print  (dot_object *);
struct object* dor_popmat_create  (void);
void           dor_popmat_execute  (dot_object *);
void           dor_popmat_initialize  (void);
void           dor_popmat_print  (dot_object *);
dot_object*    dor_pshmat_create  (void);
void           dor_pshmat_dynexe  (dot_object *);
void           dor_pshmat_execute  (dot_object *);
void           dor_pshmat_initialize  (void);
void           dor_pshmat_print  (dot_object *);
struct object* dor_rotate_create  (DtAxis, DtReal);
void           dor_rotate_destroy  (dot_object *);
void           dor_rotate_execute  (struct object *);
void           dor_rotate_initialize  (void);
void           dor_rotate_print  (dot_object *);
dot_object*    dor_scale_create  (DtReal, DtReal, DtReal);
void           dor_scale_destroy  (dot_object *);
void           dor_scale_execute  (struct object *);
void           dor_scale_initialize  (void);
void           dor_scale_print  (dot_object *);
dot_object*    dor_shear_create  (DtMajorPlane, DtReal, DtReal);
void           dor_shear_destroy  (dot_object *);
void           dor_shear_execute  (struct object *);
void           dor_shear_initialize  (void);
void           dor_shear_print  (dot_object *);
dot_object*    dor_tfmmat_create  (DtMatrix4x4, DtCompType);
void           dor_tfmmat_destroy  (dot_object *);
void           dor_tfmmat_execute  (struct object *);
void           dor_tfmmat_initialize  (void);
void           dor_tfmmat_print  (dot_object *);
dot_object*    dor_transl_create  (DtReal, DtReal, DtReal);
void           dor_transl_destroy  (dot_object *);
void           dor_transl_execute  (dot_object *);
void           dor_transl_initialize  (void);
void           dor_transl_print  (dot_object *);

#endif
