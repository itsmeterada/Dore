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
      void drr_samplernd_plylin_render(object)
      void drr_samplernd_plylin_generate(bbox, count, vtxlocs, vtxnrms, vtxclrs)
 ==========================================================================
*/

#include "../samplernd.h"
#include "dore/dore_develop/private/plylin.h"

/* 
 * Method routine for DoPolyline object
 */

void
drr_samplernd_plylin_render(object)
     DtObjectStructure *object;
{
   struct plylin *plylin;
   void drr_samplernd_plylin_generate();

   if (!DDcondex_QueryRender(DsInqClassId("DoPolyline")))
	 return;

   plylin = (struct plylin *)(object->data);

   if (plylin->vertex_count < 2)
	 return;

   drr_samplernd_plylin_generate(plylin->bounding_box_pts,
			plylin->vertex_count,
			plylin->vertex_location,
			plylin->vertex_normal,
			plylin->vertex_color);
}

/*
 ==========================================================================
*/

/*
 * Render geometry of connected line segments (polyline)
 */

void
drr_samplernd_plylin_generate(bbox, count, vtxlocs, vtxnrms, vtxclrs)
     DtRealTriple bbox[8];
     DtInt count;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;
{
   DtInt i;

   /*
    * render polyline 
    */

   printf("samplernd: polyline object being renderer\n");
   printf("\t Polyline vertex count : %d\n", count);
   for (i=0; i<count; i++)
   	printf("Vertex location : (%g %g %g) \n", 
   			vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);

}
