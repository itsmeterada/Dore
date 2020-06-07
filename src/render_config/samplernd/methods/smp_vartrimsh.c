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
      void drr_samplernd_vartrimsh_render(object)
 ==========================================================================
*/

#include "../samplernd.h"
#include "dore/dore_develop/private/vartrimsh.h"

/*
 * Method routine for DoVarTriangleMesh object
 */

void
drr_samplernd_vartrimsh_render(object)
     DtObjectStructure *object;
{
   struct vartrimsh *vartrimsh;

   if (!DDcondex_QueryRender(DsInqClassId("DoVarTriangleMesh")))
	 return;

   vartrimsh = (struct vartrimsh *)(object->data);

  /* render geometry using triangle mesh rendering funciton */

   drr_samplernd_trimsh_generate(vartrimsh->bounding_box_pts,
			vartrimsh->triangle_count,
			vartrimsh->element_normal,
			vartrimsh->vertex_count,
			vartrimsh->vertex_location,
			vartrimsh->vertex_normal,
			vartrimsh->vertex_color,
			vartrimsh->triangle_vertexlist);

   printf("samplernd: Var Triangle mesh rendered as triangle mesh.\n");
}
