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
	void ddr_softpipe_drom_render_clip_line2d (vertex1,vertex2,color1,color2)
	void ddr_softpipe_drom_render_point_clip2d (vertex,color)
	void ddr_softpipe_drom_render_clip_triangle2d (vertex1,vertex2,vertex3, color1,color2,color3)
	void ddr_softpipe_drom_clip2d_triangle_boundary (vertex1,vertex2,vertex3, color1,color2,color3,boundary)
	void ddr_softpipe_drom_clip2d_vector_boundary (vertex1,vertex2, color1,color2,boundary)
	void ddr_softpipe_drom_clip2d_triangle (vertex1,vertex2,vertex3, color1,color2,color3)
	void ddr_softpipe_drom_clip2d_vector (vertex1,vertex2,color1,color2)
	DtInt ddr_softpipe_drom_clip2d_code_vertex (vertex)
	void ddr_softpipe_drom_clip2d_edge_boundary (vertex1,vertex2,color1,color2, boundary,vertex0,color0)

  ======================================================================
 */
#include "drom.h"

/*
 ======================================================================
 */
void ddr_softpipe_drom_render_clip_line2d (
    DtRealTriple vertex1,
    DtRealTriple vertex2,
    DtColorRGB color1,
    DtColorRGB color2)
{
   DtRealTriple newvertex1, newvertex2;
   DtInt i;

#ifdef DEBUG
   printf("...processing dynout render clip line2d\n");
   fflush(stdout);
#endif

   for(i=0; i<3; i++) {
      newvertex1[i] = vertex1[i];
      newvertex2[i] = vertex2[i];
   }

   if(dde_softpipe_drom_data.xyclipped_flag) {
      ddr_softpipe_drom_clip2d_vector(newvertex1,newvertex2,
				       color1,color2);
   }
   else {
      ddr_softpipe_drom_output_line(newvertex1,newvertex2,
				     color1,color2);
   }
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_render_point_clip2d (
    DtRealTriple vertex,
    DtColorRGB color)
{
   DtInt clipcode;

#ifdef DEBUG
   printf("...processing dynout render point clipp2d\n");
   fflush(stdout);
#endif

   clipcode = ddr_softpipe_drom_clip2d_code_vertex(vertex);

   if(!clipcode) {
      ddr_softpipe_drom_output_point(vertex,color);
   }
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_render_clip_triangle2d (
    DtRealTriple vertex1,
    DtRealTriple vertex2,
    DtRealTriple vertex3,
    DtColorRGB color1,
    DtColorRGB color2,
    DtColorRGB color3)
{
   DtRealTriple newvertex1, newvertex2, newvertex3;
   DtInt i;

#ifdef DEBUG
   printf("... processing render clip triangle2d ...\n");
   fflush(stdout);
#endif

   for(i=0; i<3; i++) {
      newvertex1[i] = vertex1[i];
      newvertex2[i] = vertex2[i];
      newvertex3[i] = vertex3[i];
   }

   if(dde_softpipe_drom_data.xyclipped_flag) {
      ddr_softpipe_drom_clip2d_triangle(newvertex1,newvertex2,
					 newvertex3,color1,color2,color3);
   }
   else {
      ddr_softpipe_drom_output_triangle(newvertex1,newvertex2,
					 newvertex3,color1,color2,color3);
   }
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_clip2d_triangle_boundary (
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
   printf("...processing dynout clip2d triangle boundary\n");
   fflush(stdout);
#endif

   clipcode1 = ddr_softpipe_drom_clip2d_code_vertex(vertex1);
   clipcode2 = ddr_softpipe_drom_clip2d_code_vertex(vertex2);
   clipcode3 = ddr_softpipe_drom_clip2d_code_vertex(vertex3);

   /* Look for a boundary that has at least one vertex outside it. */

   for(;;) {
      if(boundary == 16) {
	 ddr_softpipe_drom_output_triangle(vertex1,vertex2,
					    vertex3,color1,color2,color3);
	 return;
      }

      if((clipcode1|clipcode2|clipcode3)&boundary)
	    break;
	
      boundary <<= 1;
   }

   /* Now compute the intersection of the line and boundary */
#ifdef DEBUG
   printf("boundary = %d\n",boundary);
#endif

   if((clipcode1&boundary) != (clipcode2&boundary))
	 ddr_softpipe_drom_clip2d_edge_boundary(vertex1,vertex2,color1,
						 color2, boundary,vertex02,color02);

   if((clipcode2&boundary) != (clipcode3&boundary))
	 ddr_softpipe_drom_clip2d_edge_boundary(vertex2,vertex3,color2,
						 color3,boundary,vertex13,color13);

   if((clipcode3&boundary) != (clipcode1&boundary))
	 ddr_softpipe_drom_clip2d_edge_boundary(vertex3,vertex1,color3,
						 color1,boundary,vertex21,color21);

   nextboundary = boundary << 1;

   caseid = 0;

   if(clipcode1 & boundary)
	 caseid += 1;
   if(clipcode2 & boundary)
	 caseid += 2;
   if(clipcode3 & boundary)
	 caseid += 4;

#ifdef DEBUG
   printf("clip2d_vector_bound: case = %d\n",caseid);
#endif

   switch(caseid) {
   case 0:
   case 7:
      break;

   case 1:
      ddr_softpipe_drom_clip2d_triangle_boundary(vertex02,
						  vertex2,vertex3,color02,color2,color3,
						  nextboundary);
      ddr_softpipe_drom_clip2d_triangle_boundary(vertex02,
						  vertex3,vertex21,color02,color3,color21,
						  nextboundary);
      break;

   case 2:
      ddr_softpipe_drom_clip2d_triangle_boundary(vertex13,
						  vertex3,vertex1,color13,color3,color1,
						  nextboundary);
      ddr_softpipe_drom_clip2d_triangle_boundary(vertex13,
						  vertex1,vertex02,color13,color1,color02,
						  nextboundary);
      break;

   case 4:
      ddr_softpipe_drom_clip2d_triangle_boundary(vertex21,
						  vertex1,vertex2,color21,color1,color2,
						  nextboundary);
      ddr_softpipe_drom_clip2d_triangle_boundary(vertex21,
						  vertex2,vertex13,color21,color2,color13,
						  nextboundary);
      break;

   case 3:
      ddr_softpipe_drom_clip2d_triangle_boundary(vertex3,
						  vertex21,vertex13,color3,color21,color13,
						  nextboundary);
      break;

   case 6:
      ddr_softpipe_drom_clip2d_triangle_boundary(vertex1,
						  vertex02,vertex21,color1,color02,color21,
						  nextboundary);
      break;

   case 5:
      ddr_softpipe_drom_clip2d_triangle_boundary(vertex2,
						  vertex13,vertex02,color2,color13,color02,
						  nextboundary);
      break;

   }
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_clip2d_vector_boundary (
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
   printf("... processing dynout clip2d vector boundary ...\n");
   fflush(stdout);
#endif

   clipcode1 = ddr_softpipe_drom_clip2d_code_vertex(vertex1);
   clipcode2 = ddr_softpipe_drom_clip2d_code_vertex(vertex2);

   /* Look for a boundary that has at least one vertex outside it. */

   for(;;) {
      if(boundary == 16) {
	 ddr_softpipe_drom_output_line(vertex1,vertex2,
					color1,color2);
	 return;
      }

      if((clipcode1|clipcode2)&boundary)
	    break;
	
      boundary <<= 1;
   }

   /* Now compute the intersection of the line and boundary */
#ifdef DEBUG
   printf("boundary = %d\n",boundary);
#endif

   if((clipcode1&boundary) != (clipcode2&boundary))
	 ddr_softpipe_drom_clip2d_edge_boundary(vertex1,vertex2,
						 color1,color2,boundary,vertex0,color0);

   nextboundary = boundary << 1;

   caseid = 0;

   if(clipcode1 & boundary)
	 caseid += 1;
   if(clipcode2 & boundary)
	 caseid += 2;

   switch(caseid) {
   case 0:
   case 3:
      break;

   case 1:
      ddr_softpipe_drom_clip2d_vector_boundary(vertex0,
						vertex2,color0,color2,nextboundary);
      break;

   case 2:
      ddr_softpipe_drom_clip2d_vector_boundary(vertex1,
						vertex0,color1,color0,nextboundary);
      break;
   }
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_clip2d_triangle (
    DtRealTriple vertex1,
    DtRealTriple vertex2,
    DtRealTriple vertex3,
    DtColorRGB color1,
    DtColorRGB color2,
    DtColorRGB color3)
{
   DtInt clipcode1, clipcode2, clipcode3;

#ifdef DEBUG
   printf("...processing dynout clip2d triangle \n");
   fflush(stdout);
#endif

   clipcode1 = ddr_softpipe_drom_clip2d_code_vertex(vertex1);
   clipcode2 = ddr_softpipe_drom_clip2d_code_vertex(vertex2);
   clipcode3 = ddr_softpipe_drom_clip2d_code_vertex(vertex3);

   if(!(clipcode1 | clipcode2 | clipcode3)) /* Trivial acceptance test */ {
      ddr_softpipe_drom_output_triangle(vertex1,vertex2,vertex3,
					 color1,color2,color3);
   }
   else if(clipcode1 & clipcode2 & clipcode3) /* Trivial rejection test */ {
      return;
   }
   else				/* Otherwise, must clip against some boundary. */ {
      ddr_softpipe_drom_clip2d_triangle_boundary(vertex1,vertex2,
						  vertex3,color1,color2,color3,1);
   }
}

/*
 ======================================================================
 */
void ddr_softpipe_drom_clip2d_vector (
    DtRealTriple vertex1,
    DtRealTriple vertex2,
    DtColorRGB color1,
    DtColorRGB color2)
{
   DtInt clipcode1, clipcode2;

#ifdef DEBUG
   printf("...processing dynout clip2d vector \n");
   fflush(stdout);
#endif

   clipcode1 = ddr_softpipe_drom_clip2d_code_vertex(vertex1);
   clipcode2 = ddr_softpipe_drom_clip2d_code_vertex(vertex2);

   if(!(clipcode1 | clipcode2))	/* Trivial acceptance test */ {
      ddr_softpipe_drom_output_line(vertex1,vertex2,color1,color2);
   }
   else if(clipcode1 & clipcode2) /* Trivial rejection test */ {
      return;
   }
   else				/* Otherwise, must clip against some boundary. */ {
      ddr_softpipe_drom_clip2d_vector_boundary(vertex1,vertex2,
						color1,color2,1);
   }
}
/*
 ======================================================================
 */
DtInt ddr_softpipe_drom_clip2d_code_vertex (
    DtRealTriple vertex)
{
   DtInt clipcode = 0;

#ifdef DEBUG
   printf("...processing dynout clip2d code vertex\n");
   fflush(stdout);
#endif

   if(vertex[0] < -1.)
	 clipcode |= 8;		/* x < -1. */
   if(vertex[1] < -1.)
	 clipcode |= 2;		/* y < -1. */
   if(vertex[0] > 1.)
	 clipcode |= 4;		/* x > 1. */
   if(vertex[1] > 1.)
	 clipcode |= 1;		/* y > 1. */

   return(clipcode);
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_clip2d_edge_boundary (
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
   printf("...processing dynout clip2d edge boundary\n");
   fflush(stdout);
#endif

   switch(boundary) {
      /* y = 1 */
   case 1:
      alpha = (1-vertex1[1])/(vertex2[1]-vertex1[1]);
      break;

      /* y = -1 */
   case 2:
      alpha = (-1-vertex1[1])/(vertex2[1]-vertex1[1]);
      break;

      /* x = 1 */
   case 4:
      alpha = (1-vertex1[0])/(vertex2[0]-vertex1[0]);
      break;

      /* x = -1 */
   case 8:
      alpha = (-1-vertex1[0])/(vertex2[0]-vertex1[0]);
      break;
   }

   vertex0[0] = vertex1[0] + alpha*(vertex2[0]-vertex1[0]);
   vertex0[1] = vertex1[1] + alpha*(vertex2[1]-vertex1[1]);
   vertex0[2] = vertex1[2] + alpha*(vertex2[2]-vertex1[2]);
   color0[0] = color1[0] + alpha*(color2[0]-color1[0]);
   color0[1] = color1[1] + alpha*(color2[1]-color1[1]);
   color0[2] = color1[2] + alpha*(color2[2]-color1[2]);

   switch(boundary) {
      /* y = 1 */
   case 1:
      vertex0[1] = 1.;
      break;

      /* y = -1 */
   case 2:
      vertex0[1] = -1.;
      break;

      /* x = 1 */
   case 4:
      vertex0[0] = 1.;
      break;

      /* x = -1 */
   case 8:
      vertex0[0] = -1.;
      break;
   }

#ifdef DEBUG
   printf("clip2d_edge_boundary: newvertex = (%lf,%lf,%lf)\n",
	  vertex0[0],vertex0[1],vertex0[2]);
#endif

}
