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
	DtRealTriple * ddr_softpipe_drom_unfold_triangle_mesh (trianglecount,vertexcount,vtxptr, vertexlist)
	DtRealTriple * ddr_softpipe_drom_unfold_vertex_colors (trianglecount,vertexcount, vtxclrs,vertexlist)
	DtRealTriple * ddr_softpipe_drom_unfold_vertex_normals (trianglecount,vertexcount, vtxnrms,vertexlist)

  ======================================================================
 */
#include "drom.h"

/*
 ======================================================================
 */
DtRealTriple *ddr_softpipe_drom_unfold_triangle_mesh (
    DtInt trianglecount,
    DtInt vertexcount,
    DtRealTriple *vtxptr,
    DtInt vertexlist[][3])
{
   DtRealTriple *newvtxptr;
   int i;

#ifdef DEBUG
   printf("...processing dynout unfold triangle mesh\n");
   fflush(stdout);
#endif

   if (trianglecount*3 > dde_softpipe_drom_data.trimsh_unfold_vtx_cnt) {
      if (dde_softpipe_drom_data.trimsh_unfold_vtx == NULL) {
	 newvtxptr = (DtRealTriple *)DDspace_Allocate(sizeof(DtRealTriple)*trianglecount*3);
      } else {
	 newvtxptr = (DtRealTriple *)DDspace_Reallocate(dde_softpipe_drom_data.trimsh_unfold_vtx,
					     sizeof(DtRealTriple)*trianglecount*3);
      }
      if(!newvtxptr) {
	 DDerror(ERR_CANT_ALLOC_MEM,
		    "ddr_softpipe_drom_unfold_triangle_mesh",
		    "vertex memory space");
      }

      dde_softpipe_drom_data.trimsh_unfold_vtx = newvtxptr;
      dde_softpipe_drom_data.trimsh_unfold_vtx_cnt = trianglecount*3;
   }

   newvtxptr = dde_softpipe_drom_data.trimsh_unfold_vtx;

   for(i=0; i<trianglecount; i++) {
      newvtxptr[i*3][0] = vtxptr[vertexlist[i][0]][0];
      newvtxptr[i*3][1] = vtxptr[vertexlist[i][0]][1];
      newvtxptr[i*3][2] = vtxptr[vertexlist[i][0]][2];

      newvtxptr[i*3+1][0] = vtxptr[vertexlist[i][1]][0];
      newvtxptr[i*3+1][1] = vtxptr[vertexlist[i][1]][1];
      newvtxptr[i*3+1][2] = vtxptr[vertexlist[i][1]][2];

      newvtxptr[i*3+2][0] = vtxptr[vertexlist[i][2]][0];
      newvtxptr[i*3+2][1] = vtxptr[vertexlist[i][2]][1];
      newvtxptr[i*3+2][2] = vtxptr[vertexlist[i][2]][2];
   }

   return(newvtxptr);
}
/*
 ======================================================================
 */
DtRealTriple *ddr_softpipe_drom_unfold_vertex_colors (
    DtInt trianglecount,
    DtInt vertexcount,
    DtRealTriple *vtxclrs,
    DtInt vertexlist[][3])
{
   DtRealTriple *newvtxclrs;
   int i;

#ifdef DEBUG
   printf("...processing dynout unfold vertex colors\n");
   fflush(stdout);
#endif

   if(vtxclrs == NULL) 
	 return (DtRealTriple *)NULL;

   if (trianglecount*3 > dde_softpipe_drom_data.trimsh_unfold_clr_cnt) {
      if (dde_softpipe_drom_data.trimsh_unfold_clr == NULL) {
	 newvtxclrs = (DtRealTriple *)DDspace_Allocate(sizeof(DtRealTriple)*trianglecount*3);
      } else {
	 newvtxclrs = (DtRealTriple *)DDspace_Reallocate(dde_softpipe_drom_data.trimsh_unfold_clr,
					      sizeof(DtRealTriple)*trianglecount*3);
      }
      if(!newvtxclrs) {
	 DDerror(ERR_CANT_ALLOC_MEM,
		    "ddr_softpipe_drom_unfold_vertex_colors",
		    "vertex memory space");
      }

      dde_softpipe_drom_data.trimsh_unfold_clr = newvtxclrs;
      dde_softpipe_drom_data.trimsh_unfold_clr_cnt = trianglecount*3;
   }

   newvtxclrs = dde_softpipe_drom_data.trimsh_unfold_clr;

   for(i=0; i<trianglecount; i++) {
      newvtxclrs[i*3][0] = vtxclrs[vertexlist[i][0]][0];
      newvtxclrs[i*3][1] = vtxclrs[vertexlist[i][0]][1];
      newvtxclrs[i*3][2] = vtxclrs[vertexlist[i][0]][2];

      newvtxclrs[i*3+1][0] = vtxclrs[vertexlist[i][1]][0];
      newvtxclrs[i*3+1][1] = vtxclrs[vertexlist[i][1]][1];
      newvtxclrs[i*3+1][2] = vtxclrs[vertexlist[i][1]][2];

      newvtxclrs[i*3+2][0] = vtxclrs[vertexlist[i][2]][0];
      newvtxclrs[i*3+2][1] = vtxclrs[vertexlist[i][2]][1];
      newvtxclrs[i*3+2][2] = vtxclrs[vertexlist[i][2]][2];
   }
#ifdef DEBUG
   printf("oldvtxclrs(%d)\n",vertexcount);
   for(i=0; i<vertexcount; i++) {
      printf("## %lf,%lf,%lf (%d) ##\n",vtxclrs[i][0],
	     vtxclrs[i][1],vtxclrs[i][2],i);
   }
#endif
   return(newvtxclrs);
}
/*
 ======================================================================
 */
DtRealTriple *ddr_softpipe_drom_unfold_vertex_normals (
    DtInt trianglecount,
    DtInt vertexcount,
    DtRealTriple *vtxnrms,
    DtInt vertexlist[][3])
{
   DtRealTriple *newvtxnrms;
   int i;

#ifdef DEBUG
   printf("...processing dynout unfold vertex normals\n");
   fflush(stdout);
#endif

   if(vtxnrms == NULL)
	 return (DtRealTriple *)NULL;

   if (trianglecount*3 > dde_softpipe_drom_data.trimsh_unfold_nrm_cnt) {
      if (dde_softpipe_drom_data.trimsh_unfold_nrm == NULL) {
	 newvtxnrms = (DtRealTriple *)DDspace_Allocate(sizeof(DtRealTriple)*trianglecount*3);
      } else {
	 newvtxnrms = (DtRealTriple *)DDspace_Reallocate(dde_softpipe_drom_data.trimsh_unfold_nrm,
					      sizeof(DtRealTriple)*trianglecount*3);
      }
      if(!newvtxnrms) {
	 DDerror(ERR_CANT_ALLOC_MEM,
		    "ddr_softpipe_drom_unfold_vertex_colors",
		    "vertex memory space");
      }

      dde_softpipe_drom_data.trimsh_unfold_nrm = newvtxnrms;
      dde_softpipe_drom_data.trimsh_unfold_nrm_cnt = trianglecount*3;
   }

   newvtxnrms = dde_softpipe_drom_data.trimsh_unfold_nrm;

   for(i=0; i<trianglecount; i++) {
      newvtxnrms[i*3][0] = vtxnrms[vertexlist[i][0]][0];
      newvtxnrms[i*3][1] = vtxnrms[vertexlist[i][0]][1];
      newvtxnrms[i*3][2] = vtxnrms[vertexlist[i][0]][2];

      newvtxnrms[i*3+1][0] = vtxnrms[vertexlist[i][1]][0];
      newvtxnrms[i*3+1][1] = vtxnrms[vertexlist[i][1]][1];
      newvtxnrms[i*3+1][2] = vtxnrms[vertexlist[i][1]][2];

      newvtxnrms[i*3+2][0] = vtxnrms[vertexlist[i][2]][0];
      newvtxnrms[i*3+2][1] = vtxnrms[vertexlist[i][2]][1];
      newvtxnrms[i*3+2][2] = vtxnrms[vertexlist[i][2]][2];
   }

   return(newvtxnrms);
}
