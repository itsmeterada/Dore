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
      void drr_samplernd_pntlst_render(object)
      void drr_samplernd_pntlst_generate(bbox, count, vtxlocs, vtxnrms, vtxclrs)
 ==========================================================================
*/

#include "../samplernd.h"
#include "dore/dore_develop/private/pntlst.h"

/* 
 * Method routine for DoPointList object
 */

void
drr_samplernd_pntlst_render(object)
     DtObjectStructure *object;
{
   struct pntlst *pntlst;
   DtObject altobj;
   DtPFI routine;
   void drr_samplernd_pntlst_generate();

   /* Check if class is renderable */
   if (!DDcondex_QueryRender(DsInqClassId("DoPointList")))
	 return;

   pntlst = (struct pntlst *)(object->data);

   if (pntlst->point_count == 0)
	 return;

   /*
    * If the current reptype is surface, render the alternate
    * geometry for point list, which is a sphere list;
    * otherwise render point list
    */

   if (dre_samplernd_render_private->reptyp == DcSurface) {

      /* Get pointer to the sphere list object  */

  	altobj = (struct DtObjectStructure *)
   	object->additional_data[dre_samplernd_render_private->pntlst_index];

      /* Execute the current method on sphere list */

   	routine = DDobject_InqCurrentMethod(altobj);
   	(*routine) (altobj);

   } else {

   	/* render point list */
   	drr_samplernd_pntlst_generate(pntlst->bounding_box_pts,
			pntlst->point_count,
			pntlst->vertex_location,
			pntlst->vertex_normal,
			pntlst->vertex_color);
   }
}

/*
 ==========================================================================
*/

/*
 * Render geometry of point list 
 */

void
drr_samplernd_pntlst_generate(bbox, count, vtxlocs, vtxnrms, vtxclrs)
     DtRealTriple bbox[8];
     DtInt count;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;
{
   DtInt i;
   /*
    * render point list
    */
	printf("samplernd: Point list being rendered.\n");
	printf("\t Point count : %d\n", count);
	for (i=0; i<count; i++)
		printf("\t Vertex location : (%g %g %g)\n",
			vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);

}
