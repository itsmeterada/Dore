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
      void drr_samplernd_trilst_render(object)
      void drr_samplernd_trilst_generate(bbox, count, vtxlocs, vtxnrms, vtxclrs)
 ==========================================================================
*/

#include "../samplernd.h"
#include "dore/dore_develop/private/trilst.h"

/* 
 * Method routine for DoTriangleList object 
 */

void
drr_samplernd_trilst_render(object)
     DtObjectStructure *object;
{
   struct trilst *trilst;
   void drr_samplernd_trilst_generate();

   if (!DDcondex_QueryRender(DsInqClassId("DoTriangleList")))
	 return;

   trilst = (struct trilst *)(object->data);

   drr_samplernd_trilst_generate(trilst->bounding_box_pts,
			trilst->triangle_count,
			trilst->element_normal,
			trilst->vertex_location,
			trilst->vertex_normal,
			trilst->vertex_color);

}

/*
 ==========================================================================
*/

/*
 *  Render geometry of triangle list 
 */

void
drr_samplernd_trilst_generate(bbox, count, elenrms, vtxlocs, vtxnrms, vtxclrs)
     DtRealTriple bbox[8];
     DtInt count;
     DtRealTriple *elenrms;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;
{
   DtInt i;
   /*
    * render triangle list
    */

   printf("samplernd : Triangle list being rendered.\n");
   printf("\t Triangle count : %d\n", count);

   for (i=0; i<count; i++) {
      printf ("\t Vertex location : (%g,%g,%g) \n",
	      vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);
   }
}
