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
	int drr_glbrnd_text_initialize_objects (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/dore_develop/develop.h>
#include <dore/dore_develop/private/text.h>
#include "../glbrnd.h"

void drr_glbrnd_text_initialize_objects (dot_object *object)
{
   struct text *text;
   DtInt fonttype;
   dot_font_state ts;
   DtFlag changed;
   static DtInt class_id = -1;
   DtMethodPtr routine;

   if (class_id == -1) {
      class_id = DsInqClassId("DoText");
   }

   if (!DDcondex_QueryRender(class_id))
         return;

   text = (struct text *)(object->data);

   changed = dor_text_check_attributes(text);

   ts.font_index = text->font;
   fonttype = dor_font_get_type(&ts);

   if (fonttype == dod_font_type_polygonal) {
	if ((text->alternate_object_triangles != DcNullPtr) && !changed) {
	    routine = DDobject_InqMethod(text->alternate_object_triangles,
					 DcMethodGlbrndIniObjs);
	    (*routine)(text->alternate_object_triangles);
		return;
	} else {
	    if (text->alternate_object_triangles != NULL) {
		routine = DDobject_InqMethod(text->alternate_object_triangles,
					     DcMethodDestroy);
		(*routine)(text->alternate_object_triangles);

		text->alternate_object_triangles = NULL;
            }

	    dor_text_create_alternate_geometry_triangles(text);

	    if (text->alternate_object_triangles == NULL) {
		DDerror(ERR_CANT_CREATE_ALTOBJ,
			"dor_text_glbrnd_initialize_objects", "");
		return;
	    }

	    routine = DDobject_InqMethod(text->alternate_object_triangles,
					 DcMethodGlbrndIniObjs);
	    (*routine)(text->alternate_object_triangles);
	}
   }
}
