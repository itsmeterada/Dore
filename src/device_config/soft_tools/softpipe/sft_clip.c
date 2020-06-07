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
	void ddr_softpipe_drom_render_clip_line (vertex1,vertex2,color1,color2)
	void ddr_softpipe_drom_render_point_clipped (vertex,color)
	void ddr_softpipe_drom_render_clip_triangle (vertex1,vertex2,vertex3, color1,color2,color3)
	void ddr_softpipe_drom_clip_box (vtxs,zclipflag,xyclipflag)

  ======================================================================
 */
#include "drom.h"

/*
 ======================================================================
 */
void ddr_softpipe_drom_render_clip_line (
    DtRealTriple vertex1,
    DtRealTriple vertex2,
    DtColorRGB color1,
    DtColorRGB color2)
{
   DtInt clipcode0, clipcode1;
   DtRealTriple newvertex1;
   DtRealTriple newvertex2;
   DtInt i;

#ifdef DEBUG
   printf("...processing dynout render clip line\n");
   fflush(stdout);
#endif

   for(i=0; i<3; i++) {
      newvertex1[i] = vertex1[i];
      newvertex2[i] = vertex2[i];
   }

   clipcode0 = ddr_softpipe_drom_clip3d_code_vertex(newvertex1);
   clipcode1 = ddr_softpipe_drom_clip3d_code_vertex(newvertex2);

   if(!(clipcode0 | clipcode1))	/* Trivial acceptance test */ {
      ddr_softpipe_drom_render_clip_line2d(newvertex1,newvertex2,
					    color1,color2);
   }
   else if(clipcode0 & clipcode1) /* Trivial rejection test */ {
      return;
   }
   else				/* Otherwise, must clip against some boundary. */ {
      ddr_softpipe_drom_clip3d_vector_boundary(newvertex1,
						newvertex2,color1,color2,1);
   }
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_render_point_clipped (
    DtRealTriple vertex,
    DtColorRGB color)
{
   DtInt clip3dcode, clip2dcode;

#ifdef DEBUG
   printf("...processing dynout render point clipped\n");
   fflush(stdout);
#endif

   clip3dcode = ddr_softpipe_drom_clip3d_code_vertex(vertex);
   clip2dcode = ddr_softpipe_drom_clip2d_code_vertex(vertex);

   if(clip3dcode || clip2dcode)
	 return;

   ddr_softpipe_drom_output_point(vertex,color);
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_render_clip_triangle (
    DtRealTriple vertex1,
    DtRealTriple vertex2,
    DtRealTriple vertex3,
    DtColorRGB color1,
    DtColorRGB color2,
    DtColorRGB color3)
{
   DtInt clipcode0, clipcode1, clipcode2;
   DtRealTriple newvertex1;
   DtRealTriple newvertex2;
   DtRealTriple newvertex3;
   DtInt i;

#ifdef DEBUG
   printf("...processing dynout render clip triangle\n");
   fflush(stdout);
#endif

   for(i=0; i<3; i++) {
      newvertex1[i] = vertex1[i];
      newvertex2[i] = vertex2[i];
      newvertex3[i] = vertex3[i];
   }

   clipcode0 = ddr_softpipe_drom_clip3d_code_vertex(newvertex1);
   clipcode1 = ddr_softpipe_drom_clip3d_code_vertex(newvertex2);
   clipcode2 = ddr_softpipe_drom_clip3d_code_vertex(newvertex3);

   if(!(clipcode0 | clipcode1 | clipcode2)) /* Trivial acceptance test */ {
      ddr_softpipe_drom_output_triangle(newvertex1,newvertex2,
					 newvertex3,color1,color2,color3);
   }
   else if(clipcode0 & clipcode1 & clipcode2) /* Trivial rejection test */ {
      return;
   }
   else				/* Otherwise, must clip against some boundary. */ {
      ddr_softpipe_drom_clip3d_triangle_boundary(newvertex1,
						  newvertex2,newvertex3,color1,color2,color3,1);
   }
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_clip_box (
    DtRealTriple vtxs[8],
    DtFlag *zclipflag,
    DtFlag *xyclipflag)
{
   register DtInt        vertexno;
   register DtInt        row, column;
   auto     DtReal       fcsvtxs[8][4];
   register DtMatrix4x4 *lcstofcsmat;

#  ifdef DEBUG
      printf("...processing dynout clip box \n");
      fflush(stdout);
#  endif

   lcstofcsmat = (DtMatrix4x4*)(dde_softpipe_drom_curcamdat->lcstofcsmat);

   for(vertexno=0; vertexno<8; vertexno++) {
      for(row=0; row<4; row++) {
	 fcsvtxs[vertexno][row] = (*lcstofcsmat)[row][3];
			
	 for(column=0; column<3; column++) {
	    fcsvtxs[vertexno][row] += 
		  (*lcstofcsmat)[row][column] 
			* vtxs[vertexno][column];
	 }
      }
   }
   for (vertexno=0;  vertexno < 8;  ++vertexno)
   {  for (row=0;  row < 4;  ++row)
      {
	 fcsvtxs[vertexno][row] = (*lcstofcsmat)[row][3];
	 for (column=0;  column < 3;  ++column)
	    fcsvtxs[vertexno][row] +=
	       (*lcstofcsmat)[row][column] * vtxs[vertexno][column];
      }
   }

   *zclipflag = DcFalse;
   *xyclipflag = DcFalse;

   /** fcs(npc): z/w < -1, z/w > 0 -- zclip
     x/w < -1, x/w > 1, y < -1, y > 1 -- xyclip **/

   for(vertexno=0; vertexno<8; vertexno++) {
      if(fcsvtxs[vertexno][2] < -fcsvtxs[vertexno][3])
	    *zclipflag = DcTrue;
      else if(fcsvtxs[vertexno][2] > 0)
	    *zclipflag = DcTrue;

      if(fcsvtxs[vertexno][0] < -fcsvtxs[vertexno][3])
	    *xyclipflag = DcTrue;
      else if(fcsvtxs[vertexno][0] > fcsvtxs[vertexno][3])
	    *xyclipflag = DcTrue;
      else if(fcsvtxs[vertexno][1] < -fcsvtxs[vertexno][3])
	    *xyclipflag = DcTrue;
      else if(fcsvtxs[vertexno][1] > fcsvtxs[vertexno][3])
	    *xyclipflag = DcTrue;
   }
}
