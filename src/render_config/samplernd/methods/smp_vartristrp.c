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
      void drr_samplernd_vartristrip_render(object)
 ==========================================================================
*/

#include "../samplernd.h"
#include "dore/dore_develop/private/vartristrp.h"

/* 
 * Method routine for DoVarTriangleStrip object
 */

void
drr_samplernd_vartristrip_render(object)
     DtObjectStructure *object;
{
   struct vartristrip *vartristrip;

   if (!DDcondex_QueryRender(DsInqClassId("DoVarTriangleStrip")))
	 return;

   vartristrip = (struct vartristrip *)(object->data);

   /* render using trianlge strip rendering function */

   drr_samplernd_trimsh_generate(vartristrip->bounding_box_pts,
			vartristrip->vertex_count - 2,
			vartristrip->element_normal,
			vartristrip->vertex_count,
			vartristrip->vertex_location,
			vartristrip->vertex_normal,
			vartristrip->vertex_color,
			vartristrip->triangle_vertexlist);

   printf("samplernd: Var triangle styrip rendered as triangle strip.\n");
}

/*
 ==========================================================================
*/
