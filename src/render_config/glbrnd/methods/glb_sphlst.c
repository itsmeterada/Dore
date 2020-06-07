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
	int drr_glbrnd_sphlst_initialize_objects (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/dore_develop/private/sphlst.h>
#include "../glbrnd.h"

/*
 ======================================================================
 */

void drr_glbrnd_sphlst_initialize_objects (dot_object *object)
{
    struct sphlst *sphlst;
    DtInt sphereno;
    static DtInt class_id = -1;
    DtRealTriple wcsloc;
    DtRealTriple location;
    DtInt locoffset;
    DtReal radius;
    DtRealTriple color;
    DtColorModel colormodel;
    DtMatrix4x4 lcstowcsmat;
    dot_object *rayintobj;

    if (class_id == -1) {
	class_id = DsInqClassId("DoSphereList");
    }

    if (!DDcondex_QueryRender(class_id))
	return;

    sphlst = (struct sphlst *)(object->data);

    /* 
     * Set object modifier false as sphere lists have locations that
     * should be stored in wcs.
     */ 
    drr_glbrnd_initialize_object_modifier(DcFalse);

    dor_global_lcstowcsmat_get_value (lcstowcsmat);

    for (sphereno=0; sphereno<sphlst->sphere_count; sphereno++) {

	locoffset = 3 * sphereno;

	location[0] = sphlst->locations[locoffset];
	location[1] = sphlst->locations[locoffset+1];
	location[2] = sphlst->locations[locoffset+2];

        dor_math_pnttrns_fast(location, lcstowcsmat, wcsloc);

	if (sphlst->radii != NULL) {
	    radius = sphlst->radii[sphereno];
	} else {
	    radius = 1.0;
	}

	if (sphlst->colors != NULL) {
	    color[0] = sphlst->colors[locoffset];
	    color[1] = sphlst->colors[locoffset+1];
	    color[2] = sphlst->colors[locoffset+2];
	} else {
	    dor_global_difclr_get_value(&colormodel, color);
	}

	rayintobj = dor_rayintsph_create(wcsloc, radius, color);
	
	drr_glbrnd_initialize_object(rayintobj);
    }
}
