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
	int drr_glbrnd_simplymsh_initialize_objects (object)

  ======================================================================
 */
#include "dore/internal/dogen.h"
#include "dore/dore_develop/develop.h"
#include "dore/dore_develop/private/simplymsh.h"
#include "../glbrnd.h"

void drr_glbrnd_simplymsh_initialize_objects (dot_object *object)
{
   dot_object *alt;
   static DtInt class_id = -1;

   if (class_id == -1) {
      class_id = DsInqClassId("DoSimplePolygonMesh");
   }

   if (!DDcondex_QueryRender(class_id))
         return;

   alt = dor_simplymsh_update_tris (object);
   if (alt) {
       DDobject_InqCurrentMethod(alt) (alt);
   }
}
