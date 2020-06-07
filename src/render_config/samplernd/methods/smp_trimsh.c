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
      void drr_samplernd_trimsh_render(object)
      void drr_samplernd_trimsh_generate(bbox, count, vtxlocs, vtxnrms, vtxclrs)
 ==========================================================================
*/

#include "../samplernd.h"
#include "dore/dore_develop/private/trimsh.h"

/* 
 * Method routine for DoTriangleMesh object
 */

void
drr_samplernd_trimsh_render(object)
     DtObjectStructure *object;
{
   struct trimsh *trimsh;
   void drr_samplernd_trimsh_generate();

   if (!DDcondex_QueryRender(DsInqClassId("DoTriangleMesh")))
	 return;

   trimsh = (struct trimsh *)(object->data);

   drr_samplernd_trimsh_generate(trimsh->bounding_box_pts,
			trimsh->triangle_count,
			trimsh->element_normal,
			trimsh->vertex_count,
			trimsh->vertex_location,
			trimsh->vertex_normal,
			trimsh->vertex_color,
			trimsh->triangle_vertexlist);

}

/*
 ==========================================================================
*/

/* 
 * Render geometry of triangle mesh
 */

void
drr_samplernd_trimsh_generate(bbox, tricount, elenrms, 
		     vtxcount, vtxlocs, vtxnrms, vtxclrs, vtxlist)
     DtRealTriple bbox[8];
     DtInt tricount;
     DtRealTriple *elenrms;
     DtInt vtxcount;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;
     DtInt vtxlist[][3];
{
   DtInt i;
   /*
    * render triangle mesh
    */

   printf("samplernd: Triangle mesh being rendered.\n");
   printf("\t Triangle count : %d\n", tricount);

   for (i=0; i<tricount; i++) {
   	printf ("\t Vertex location : (%g,%g,%g) \n",
  	vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);
   }

}
