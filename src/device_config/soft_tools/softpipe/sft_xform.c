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
  ======================================================================
  Functions:
	void ddr_softpipe_drom_transform_vertices_ctod (vertexcount,vtxptr)
	void ddr_softpipe_drom_transform_vertices (vertexcount,invtxptr,outvtxptr)

  ======================================================================
 */
#include "drom.h"
#include <dore/internal/matrix.h>

/*
 ======================================================================
 */
void ddr_softpipe_drom_transform_vertices_ctod (
    DtInt vertexcount,
    DtRealTriple *vtxptr)
{
   int i;

#ifdef DEBUG
   printf("...processing dynout transform vertices ctod\n");
   fflush(stdout);
#endif

   for(i=0; i<vertexcount; i++) {
      vtxptr[i][0] *= dde_softpipe_drom_curdevdat->ctodscale[0];
      vtxptr[i][1] *= dde_softpipe_drom_curdevdat->ctodscale[1];
      vtxptr[i][2] *= dde_softpipe_drom_curdevdat->ctodscale[2];

      vtxptr[i][0] += dde_softpipe_drom_curdevdat->ctodtranslate[0];
      vtxptr[i][1] += dde_softpipe_drom_curdevdat->ctodtranslate[1];
      vtxptr[i][2] += dde_softpipe_drom_curdevdat->ctodtranslate[2];
   }
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_transform_vertices (
    DtInt vertexcount,
    DtRealTriple *invtxptr,
    DtRealTriple **outvtxptr)
{
   int i,j;
   DtPoint4 newvertex;
   DtPoint4 srcvertex;
   DtReal tmpover;

#ifdef DEBUG
   printf("...processing dynout transform vertices \n");
   fflush(stdout);
#endif

   if (vertexcount > dde_softpipe_drom_data.xformed_vtxlocs_count)  {
      if (dde_softpipe_drom_data.xformed_vtxlocs == NULL)  {	   
	 dde_softpipe_drom_data.xformed_vtxlocs =
	       DDspace_Allocate (sizeof(DtRealTriple) * vertexcount);
      }
      else {
	 dde_softpipe_drom_data.xformed_vtxlocs =
	       DDspace_Reallocate(dde_softpipe_drom_data.xformed_vtxlocs,
				  sizeof(DtRealTriple) * vertexcount);
      }
      if(!dde_softpipe_drom_data.xformed_vtxlocs) {
	 DDerror(ERR_CANT_ALLOC_MEM, "ddr_softpipe_drom_transform_vertices",
		    "vertex memory space");
      }
      dde_softpipe_drom_data.xformed_vtxlocs_count = vertexcount;
   }

   for(i=0; i<vertexcount; i++) {
      srcvertex[0] = invtxptr[i][0];
      srcvertex[1] = invtxptr[i][1];
      srcvertex[2] = invtxptr[i][2];
      srcvertex[3] = 1.;

      dor_matrix_new_vectormultiply(srcvertex,
				    dde_softpipe_drom_curcamdat->lcstofcsmat,newvertex);

      tmpover = 1. / newvertex[3];

      for(j=0; j<3; j++)
	    newvertex[j] *= tmpover;

      dde_softpipe_drom_data.xformed_vtxlocs[i][0] = newvertex[0];
      dde_softpipe_drom_data.xformed_vtxlocs[i][1] = newvertex[1];
      dde_softpipe_drom_data.xformed_vtxlocs[i][2] = newvertex[2];
   }
#ifdef DEBUG
   dor_matrix_print(dde_softpipe_drom_curcamdat->lcstofcsmat);
#endif

   *outvtxptr = dde_softpipe_drom_data.xformed_vtxlocs;
}
