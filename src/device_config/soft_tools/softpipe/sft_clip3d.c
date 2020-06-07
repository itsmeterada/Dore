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
	void ddr_softpipe_drom_clip3d_triangle_boundary (vertex1,vertex2,vertex3, color1,color2,color3,boundary)
	void ddr_softpipe_drom_clip3d_vector_boundary (vertex1,vertex2, color1,color2,boundary)
	DtInt ddr_softpipe_drom_clip3d_code_vertex (vertex)
	void ddr_softpipe_drom_clip3d_edge_boundary (vertex1,vertex2,color1,color2, boundary,vertex0,color0)

  ======================================================================
 */
#include "drom.h"

/*
 ======================================================================
 */
void ddr_softpipe_drom_clip3d_triangle_boundary (
    DtRealTriple vertex1,
    DtRealTriple vertex2,
    DtRealTriple vertex3,
    DtColorRGB color1,
    DtColorRGB color2,
    DtColorRGB color3,
    DtInt boundary)
{
   DtInt clipcode1, clipcode2, clipcode3;
   DtRealTriple vertex02, vertex13, vertex21;
   DtColorRGB color02, color13, color21;
   DtInt caseid, nextboundary;

#ifdef DEBUG
   printf("...processing dynout clip3d triangle boundary\n");
   fflush(stdout);
#endif

   clipcode1 = ddr_softpipe_drom_clip3d_code_vertex(vertex1);
   clipcode2 = ddr_softpipe_drom_clip3d_code_vertex(vertex2);
   clipcode3 = ddr_softpipe_drom_clip3d_code_vertex(vertex3);

   if(clipcode1 & clipcode2 & clipcode3)
	 return;

   /* Look for a boundary that has at least one vertex outside it. */

   for(;;) {
      if(boundary == 4) {
	 ddr_softpipe_drom_render_clip_triangle2d(vertex1,
						   vertex2,vertex3,color1,color2,color3);
	 return;
      }

      if((clipcode1|clipcode2|clipcode3)&boundary)
	    break;
	
      boundary <<= 1;
   }

   /* Now compute the intersection of the line and boundary */

   if((clipcode1&boundary) != (clipcode2&boundary))
	 ddr_softpipe_drom_clip3d_edge_boundary(vertex1,vertex2,
						 color1,color2,boundary,vertex02,color02);

   if((clipcode2&boundary) != (clipcode3&boundary))
	 ddr_softpipe_drom_clip3d_edge_boundary(vertex2,vertex3,
						 color2,color3,boundary,vertex13,color13);

   if((clipcode3&boundary) != (clipcode1&boundary))
	 ddr_softpipe_drom_clip3d_edge_boundary(vertex3,vertex1,
						 color3,color1,boundary,vertex21,color21);

   nextboundary = boundary << 1;

   caseid = 0;

   if(clipcode1 & boundary)
	 caseid += 1;
   if(clipcode2 & boundary)
	 caseid += 2;
   if(clipcode3 & boundary)
	 caseid += 4;

   switch(caseid) {
   case 0:
   case 7:
#ifdef DEBUG
      printf("Cannot happen\n");
#endif
      break;

   case 1:			/* vertex1 outside */
      ddr_softpipe_drom_clip3d_triangle_boundary(vertex02,
						  vertex2,vertex3,color02,color2,color3,
						  nextboundary);
      ddr_softpipe_drom_clip3d_triangle_boundary(vertex02,
						  vertex3,vertex21,color02,color3,color21,
						  nextboundary);
      break;

   case 2:			/* vertex2 outside */
      ddr_softpipe_drom_clip3d_triangle_boundary(vertex13,
						  vertex3,vertex1,color13,color3,color1,
						  nextboundary);
      ddr_softpipe_drom_clip3d_triangle_boundary(vertex13,
						  vertex1,vertex02,color13,color1,color02,
						  nextboundary);
      break;

   case 4:			/* vertex3 outside */
      ddr_softpipe_drom_clip3d_triangle_boundary(vertex21,
						  vertex1,vertex2,color21,color1,color2,
						  nextboundary);
      ddr_softpipe_drom_clip3d_triangle_boundary(vertex21,
						  vertex2,vertex13,color21,color2,color13,
						  nextboundary);
      break;

   case 3:			/* vertex 1 & 2 outside */
      ddr_softpipe_drom_clip3d_triangle_boundary(vertex3,
						  vertex21,vertex13,color3,color21,color13,
						  nextboundary);
      break;

   case 6:			/* vertex 2 & 3 outside */
      ddr_softpipe_drom_clip3d_triangle_boundary(vertex1,
						  vertex02,vertex21,color1,color02,color21,
						  nextboundary);
      break;

   case 5:			/* vertex 3 & 1 outside */
      ddr_softpipe_drom_clip3d_triangle_boundary(vertex2,
						  vertex13,vertex02,color2,color13,color02,
						  nextboundary);
      break;
   }
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_clip3d_vector_boundary (
    DtRealTriple vertex1,
    DtRealTriple vertex2,
    DtColorRGB color1,
    DtColorRGB color2,
    DtInt boundary)
{
   DtInt clipcode1, clipcode2;
   DtRealTriple vertex0;
   DtColorRGB color0;
   DtInt caseid, nextboundary;

#ifdef DEBUG
   printf("...processing dynout clip3d vector boundary\n");
   fflush(stdout);
#endif

   clipcode1 = ddr_softpipe_drom_clip3d_code_vertex(vertex1);
   clipcode2 = ddr_softpipe_drom_clip3d_code_vertex(vertex2);

   /* Look for a boundary that has at least one vertex outside it. */

   for(;;) {
      if(boundary == 4) {
	 ddr_softpipe_drom_render_clip_line2d(vertex1,vertex2,
					       color1,color2);
	 return;
      }

      if((clipcode1|clipcode2)&boundary)
	    break;
	
      boundary <<= 1;
   }

   /* Now compute the intersection of the line and boundary */

   if((clipcode1&boundary) != (clipcode2&boundary))
	 ddr_softpipe_drom_clip3d_edge_boundary(vertex1,vertex2,
						 color1,color2,boundary,vertex0,color0);

   nextboundary = boundary << 1;

   caseid = 0;

   if(clipcode1 & boundary)
	 caseid += 1;
   if(clipcode2 & boundary)
	 caseid += 2;

#ifdef DEBUG
   printf("clip3d_vector_bound: case = %d\n",caseid);
#endif

   switch(caseid) {
   case 0:
   case 3:
      break;

   case 1:			/* vertex1 outside */
      ddr_softpipe_drom_clip3d_vector_boundary(vertex0,
						vertex2,color0,color2,nextboundary);
      break;

   case 2:			/* vertex2 outside */
      ddr_softpipe_drom_clip3d_vector_boundary(vertex1,
						vertex0,color1,color0,nextboundary);
      break;
   }
}
/*
 ======================================================================
 */
DtInt ddr_softpipe_drom_clip3d_code_vertex (
    DtRealTriple vertex)
{
   DtInt clipcode = 0;

#ifdef DEBUG
   printf("...processing dynout clip3d code vertex\n");
   fflush(stdout);
#endif

   if(vertex[2] < -1.)
	 clipcode += 1;
   if(vertex[2] > 0.)
	 clipcode += 2;

   return(clipcode);
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_clip3d_edge_boundary (
    DtRealTriple vertex1,
    DtRealTriple vertex2,
    DtColorRGB color1,
    DtColorRGB color2,
    DtInt boundary,
    DtRealTriple vertex0,
    DtColorRGB color0)
{
   double alpha;

#ifdef DEBUG
   printf("...processing dynout clip3d edge boundary\n");
   fflush(stdout);
#endif

   switch(boundary) {
      /* z = -1 */
   case 1:
      alpha = (-1.-vertex1[2])/(vertex2[2]-vertex1[2]);
      break;

      /* z = 0 */
   case 2:
      alpha = (0.-vertex1[2])/(vertex2[2]-vertex1[2]);
      break;

   }

   vertex0[0] = vertex1[0] + alpha*(vertex2[0]-vertex1[0]);
   vertex0[1] = vertex1[1] + alpha*(vertex2[1]-vertex1[1]);
   vertex0[2] = vertex1[2] + alpha*(vertex2[2]-vertex1[2]);
   color0[0] = color1[0] + alpha*(color2[0]-color1[0]);
   color0[1] = color1[1] + alpha*(color2[1]-color1[1]);
   color0[2] = color1[2] + alpha*(color2[2]-color1[2]);

   switch(boundary) {
      /* z = -1 */
   case 1:
      vertex0[2] = -1.;
      break;

      /* z = 1 */
   case 2:
      vertex0[2] = 0.;
      break;
   }

#ifdef DEBUG
   printf("clip3d_edge_boundary: newvertex = (%lf,%lf,%lf)\n",
	  vertex0[0],vertex0[1],vertex0[2]);
#endif

}
