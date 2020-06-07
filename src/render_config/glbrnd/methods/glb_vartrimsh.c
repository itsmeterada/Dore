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
	drr_glbrnd_vartrimsh_initialize_objects

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/dore_develop/develop.h>
#include <dore/dore_develop/private/vartrimsh.h>
#include <dore/dore_develop/private/rayintmtr.h>
#include <dore/dore_develop/private/rayinttri.h>
#include "../glbrnd.h"

void drr_glbrnd_vartrimsh_initialize_objects (dot_object *object)
{
    struct vartrimsh *vartrimsh;
    DtFlag normal;
    DtMatrix4x4 lcstowcsmat;
    DtInt triangleno;
    DtInt vertex1, vertex2, vertex3;
    DtRealTriple wcsloc[3];
    DtRealTriple wcsnrm[3];
    dot_object *rayintobj;
    static DtInt class_id = -1;
 
    if (class_id == -1) {
      	class_id = DsInqClassId("DoVarTriangleMesh");
    }

    if (!DDcondex_QueryRender(class_id))
         return;

    vartrimsh = (struct vartrimsh *)(object->data);

    drr_glbrnd_initialize_object_modifier(DcFalse);


    if (vartrimsh->needvertnrms) {
     	if (vartrimsh->needelemnrms) {
	    if (vartrimsh->element_normal == NULL) {
	    	   vartrimsh->element_normal = 
			(DtRealTriple *) DDspace_Allocate( 
			vartrimsh->triangle_count*(sizeof (DtRealTriple)));
	    }
	    dor_vartrimsh_calculate_triangle_normals(vartrimsh);
	    vartrimsh->needelemnrms = DcFalse;
	}

	if (vartrimsh->vertex_normal == NULL) {
	    vartrimsh->vertex_normal = (DtRealTriple *)DDspace_Allocate(
                	vartrimsh->vertex_count*(sizeof (DtRealTriple)));
	}
	dor_vartrimsh_calculate_vertex_normals(vartrimsh);
	vartrimsh->needvertnrms = DcFalse;
    }
    normal = (vartrimsh->vertex_normal != NULL);
 
    dor_global_lcstowcsmat_get_value(lcstowcsmat);
  
    for (triangleno=0;triangleno<vartrimsh->triangle_count;triangleno++) {
 	vertex1 = vartrimsh->triangle_vertexlist[triangleno][0];
 	vertex2 = vartrimsh->triangle_vertexlist[triangleno][1];
 	vertex3 = vartrimsh->triangle_vertexlist[triangleno][2];

	dor_math_pnttrns_fast(vartrimsh->vertex_location[vertex1],
			lcstowcsmat, wcsloc[0]);

	dor_math_pnttrns_fast(vartrimsh->vertex_location[vertex2],
			lcstowcsmat, wcsloc[1]);

	dor_math_pnttrns_fast(vartrimsh->vertex_location[vertex3],
			lcstowcsmat, wcsloc[2]);

   	if (normal) {
	    dor_math_vectrns_fast(vartrimsh->vertex_normal[vertex1],
			lcstowcsmat, wcsnrm[0]);

	    dor_math_renormalize_fast(wcsnrm[0]);

	    dor_math_vectrns_fast(vartrimsh->vertex_normal[vertex2],
			lcstowcsmat, wcsnrm[1]);

	    dor_math_renormalize_fast(wcsnrm[1]);

	    dor_math_vectrns_fast(vartrimsh->vertex_normal[vertex3],
			lcstowcsmat, wcsnrm[2]);

	    dor_math_renormalize_fast(wcsnrm[2]);

	    rayintobj = dor_rayintmshtri_create(wcsloc,wcsnrm);
	} else {
	    rayintobj = dor_rayinttri_create(wcsloc);
	}

	drr_glbrnd_initialize_object(rayintobj);
    }
}
