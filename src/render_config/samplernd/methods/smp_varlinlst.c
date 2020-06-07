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
      void drr_samplernd_varlinlst_render(object)
 ==========================================================================
*/

#include "../samplernd.h"
#include "dore/dore_develop/private/varlinlst.h"

/*
 * Method routine for DoVarLineList object
 */

void
drr_samplernd_varlinlst_render(object)
     DtObjectStructure *object;
{
   struct varlinlst *varlinlst;

   if (!DDcondex_QueryRender(DsInqClassId("DoVarLineList")))
	 return;

   varlinlst = (struct varlinlst *)(object->data);

   if (varlinlst->line_count == 0)
	 return;

  /* render geometry using line list rendering function */

   drr_samplernd_linlst_generate(varlinlst->bounding_box_pts,
			varlinlst->line_count,
			varlinlst->vertex_location,
			varlinlst->vertex_normal,
			varlinlst->vertex_color);

   printf("samplernd: Var line list being rendered as line list.\n");
}

/*
 ==========================================================================
*/
