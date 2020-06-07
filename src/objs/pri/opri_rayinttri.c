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
 
/*
  ======================================================================
  Functions:
	dor_rayinttri_initialize
	dor_rayinttri_create

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/rayinttri.h>

DtInt doe_rayinttri_class_id = -1;
DtFlag doe_rayinttri_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry rayinttri_methods[] = {
    { 0, (DtMethodPtr)(0) }
};

void dor_rayinttri_initialize (void)
{
    doe_rayinttri_class_id = dor_class_install (DcTypeRayinttri, "Rayinttri", 
				0, rayinttri_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_rayinttri_create (DtRealTriple loc[3])
{
    struct rayinttri *tri;
    dot_object *rayinttriobject;

    if (!doe_rayinttri_initialized) {
	dor_rayinttri_initialize();
	doe_rayinttri_initialized = DcTrue;
    }

    tri = (struct rayinttri *)dor_space_allocate(sizeof *tri);

    tri->x1 = loc[0][0];
    tri->y1 = loc[0][1];
    tri->z1 = loc[0][2];

    tri->x2 = loc[1][0];
    tri->y2 = loc[1][1];
    tri->z2 = loc[1][2];

    tri->x3 = loc[2][0];
    tri->y3 = loc[2][1];
    tri->z3 = loc[2][2];

    dor_math_plane_equation(loc[0][0],loc[0][1],loc[0][2],
			    loc[1][0],loc[1][1],loc[1][2],
			    loc[2][0],loc[2][1],loc[2][2],
			    &(tri->cosine_x),&(tri->cosine_y),&(tri->cosine_z),
			    &(tri->D));

    if ((dor_math_fabs(tri->cosine_z) >= dor_math_fabs(tri->cosine_x)) &&
   	(dor_math_fabs(tri->cosine_z) >= dor_math_fabs(tri->cosine_y))) {
   	tri->projection_type = dod_rayinttri_projection_type_z;

    } else if ((dor_math_fabs(tri->cosine_y) >= dor_math_fabs(tri->cosine_x)) &&
	       (dor_math_fabs(tri->cosine_y) >= dor_math_fabs(tri->cosine_z))) {
   	tri->projection_type = dod_rayinttri_projection_type_y;

    } else {
   	tri->projection_type = dod_rayinttri_projection_type_x;
    }

    rayinttriobject = dor_object_create(doe_rayinttri_class_id,tri);

    return(rayinttriobject);
}
