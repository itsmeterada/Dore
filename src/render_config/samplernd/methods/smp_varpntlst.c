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
      void drr_samplernd_varpntlst_render(object)
 ==========================================================================
*/

/*
 * Method routine for DoVarPointList object
 */

#include "../samplernd.h"
#include "dore/dore_develop/private/varpntlst.h"

void
drr_samplernd_varpntlst_render(object)
     DtObjectStructure *object;
{
   struct varpntlst *varpntlst;

   if (!DDcondex_QueryRender(DsInqClassId("DoVarPointList")))
	 return;

   varpntlst = (struct varpntlst *)(object->data);

   if (varpntlst->point_count == 0)
	 return;

  /* render geometry using point list rendering function */

   drr_samplernd_pntlst_generate(varpntlst->bounding_box_pts,
			varpntlst->point_count,
			varpntlst->vertex_location,
			varpntlst->vertex_normal,
			varpntlst->vertex_color);

   printf("samplernd: Var point list rendered as point list.\n");
}
/*
 ==========================================================================
*/
