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
	void drr_samplernd_linlst_render(object)
	void drr_samplernd_linlst_generate(bbox, count, vtxlocs, vtxnrms, vtxclrs)
 ==========================================================================
*/

#include "../samplernd.h"
#include "dore/dore_develop/private/linlst.h"

/* 
 * Method routine for DoLineList object
 */

void
drr_samplernd_linlst_render(object)
     DtObjectStructure *object;
{
   struct linlst *linlst;
   void drr_samplernd_linlst_generate();

   if (!DDcondex_QueryRender(DsInqClassId("DoLineList")))
	 return;

   linlst = (struct linlst *)(object->data);

   if (linlst->line_count == 0)
	 return;

   drr_samplernd_linlst_generate(linlst->bounding_box_pts,
			linlst->line_count,
			linlst->vertex_location,
			linlst->vertex_normal,
			linlst->vertex_color);
}

/*
 ==========================================================================
*/

/* 
 * Render geometry of line list 
 */

void
drr_samplernd_linlst_generate(bbox, count, vtxlocs, vtxnrms, vtxclrs)
     DtRealTriple bbox[8];
     DtInt count;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;
{
     DtInt i;

   /*
    * render line list
    */
   
   printf("samplernd: Line list being rendered.\n");
   printf("\t Line count : %d\n", count);
   for (i=0; i<count; i++)
   	printf("\t Vertex location : (%g %g %g)\n",
			 vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);
}
