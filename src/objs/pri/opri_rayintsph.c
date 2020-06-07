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
	int dor_rayintsph_initialize ()
	dot_object * dor_rayintsph_create (loc,radius,color)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/rayintsph.h>

DtInt doe_rayintsph_class_id = -1;
DtFlag doe_rayintsph_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry rayintsph_methods[] = {
    { 0, (DtMethodPtr)(0) }
};

void dor_rayintsph_initialize (void)
{
    doe_rayintsph_class_id = dor_class_add ("Rayintsph", 
				0, rayintsph_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_rayintsph_create (
    DtRealTriple loc,
    DtReal radius,
    DtRealTriple color)
{
    struct rayintsph *sphere;
    dot_object *rayintsphobj;

    if (!doe_rayintsph_initialized) {
	dor_rayintsph_initialize();
	doe_rayintsph_initialized = DcTrue;
    }

    sphere = (struct rayintsph *)dor_space_allocate(sizeof *sphere);

    sphere->location[0] = loc[0];
    sphere->location[1] = loc[1];
    sphere->location[2] = loc[2];

    sphere->radius = radius;

    sphere->color[0] = color[0];
    sphere->color[1] = color[1];
    sphere->color[2] = color[2];

    rayintsphobj= dor_object_create(doe_rayintsph_class_id,sphere);

    return(rayintsphobj);
}
