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
 ==========================================================================
 Functions :
      void drr_samplernd_tristrip_render(object)
 ==========================================================================
*/

#include "../samplernd.h"
#include "dore/dore_develop/private/tristrip.h"

/* 
 * Method routine for DoTriangleStrip object
 */

void
drr_samplernd_tristrip_render(object)
     DtObjectStructure *object;
{
   struct tristrip *tristrip;

   if (!DDcondex_QueryRender(DsInqClassId("DoTriangleStrip")))
	 return;

   tristrip = (struct tristrip *)(object->data);

   /* render geometry using triangle mesh rendering function */

   drr_samplernd_trimsh_generate(tristrip->bounding_box_pts,
			tristrip->vertex_count - 2,
			tristrip->element_normal,
			tristrip->vertex_count,
			tristrip->vertex_location,
			tristrip->vertex_normal,
			tristrip->vertex_color,
			tristrip->triangle_vertexlist);

   printf("samplernd: Triangle strip rendered as triangle mesh.\n");
}

/*
 ==========================================================================
*/
