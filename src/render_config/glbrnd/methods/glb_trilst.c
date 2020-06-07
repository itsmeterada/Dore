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
	int drr_glbrnd_trilst_initialize_objects (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/dore_develop/develop.h>
#include <dore/dore_develop/private/trilst.h>
#include <dore/dore_develop/private/rayintmtr.h>
#include <dore/dore_develop/private/rayinttri.h>
#include "../glbrnd.h"

void drr_glbrnd_trilst_initialize_objects (dot_object *object)
{
   struct trilst *trilst;
   DtFlag normal;
   DtMatrix4x4 lcstowcsmat;
   DtInt triangleno;
   DtInt vertex1, vertex2, vertex3;
   DtRealTriple wcsloc[3];
   DtRealTriple wcsnrm[3];
   dot_object *rayintobj;
   static DtInt class_id = -1;

   if (class_id == -1) {
      class_id = DsInqClassId("DoTriangleList");
   }

   if (!DDcondex_QueryRender(class_id))
         return;

   trilst = (struct trilst *)(object->data);

   drr_glbrnd_initialize_object_modifier(DcFalse);

   normal = (trilst->vertextype == DcLocNrm) ||
   	(trilst->vertextype == DcLocNrmClr);

   dor_global_lcstowcsmat_get_value(lcstowcsmat);

   for (triangleno=0;triangleno<trilst->triangle_count;triangleno++) {
   	vertex1 = triangleno*3;
   	vertex2 = vertex1+1;
   	vertex3 = vertex1+2;

        /* check if triangle is degenerate */
   	if (!dor_check_degen_triangle_vtx_values(
   	    trilst->vertex_location[vertex1],
   	    trilst->vertex_location[vertex2],
   	    trilst->vertex_location[vertex3]))
   	    continue;

   	dor_math_pnttrns_fast(trilst->vertex_location[vertex1],
				lcstowcsmat, wcsloc[0]);

   	dor_math_pnttrns_fast(trilst->vertex_location[vertex2],
				lcstowcsmat, wcsloc[1]);

   	dor_math_pnttrns_fast(trilst->vertex_location[vertex3],
				lcstowcsmat, wcsloc[2]);

   	if (normal) {
   	    dor_math_vectrns_fast(trilst->vertex_normal[vertex1],
				lcstowcsmat, wcsnrm[0]);

   	    dor_math_renormalize_fast(wcsnrm[0]);

   	    dor_math_vectrns_fast(trilst->vertex_normal[vertex2],
				lcstowcsmat, wcsnrm[1]);

   	    dor_math_renormalize_fast(wcsnrm[1]);

   	    dor_math_vectrns_fast(trilst->vertex_normal[vertex3],
				lcstowcsmat, wcsnrm[2]);

   	    dor_math_renormalize_fast(wcsnrm[2]);

   	    rayintobj = dor_rayintmshtri_create(wcsloc,wcsnrm);
   	} else {
   	    rayintobj = dor_rayinttri_create(wcsloc);
   	}

   	drr_glbrnd_initialize_object(rayintobj);
   }
}
