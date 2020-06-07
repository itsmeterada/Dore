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
	dor_clip3d_code_vertex
	dor_clip3d_edge_boundary
	dor_clip3d_point
	dor_clip3d_print_vertex
	dor_clip3d_test_point
	dor_clip3d_test_triangle
	dor_clip3d_test_triangle_clip_depth_only
	dor_clip3d_test_vector
	dor_clip3d_triangle
	dor_clip3d_triangle_boundary
	dor_clip3d_vector
	dor_clip3d_vector_boundary

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/clip3d.h>

DtSwitch doe_clip3d_interpolateswitch;
DtPFV doe_clip3d_action;
DtPtr doe_clip3d_actionparm;
DtInt doe_clip3d_max_clipcode;


/*
 ======================================================================
 */

/*  Compute the "out code" of a vertex:  
	1  - clipped by z yon
	2  - clipped by z hither
	4  - clipped by xmin
	8  - clipped by xmax
	16 - clipped by ymin
	32 - clipped by ymax
	64 - w is negative
*/

DtInt dor_clip3d_code_vertex (dot_clip3d_vertex *vertex)
{
    DtInt clipcode = 0;

    if (vertex->loc[2] < -vertex->loc[3])
	    clipcode += 1;
    if (vertex->loc[2] > 0)
	    clipcode += 2;
    if (vertex->loc[3] < 0) {
	clipcode += 64;
    } else {
	if (vertex->loc[0] < -vertex->loc[3])
		clipcode += 4;
	if (vertex->loc[0] > vertex->loc[3])
		clipcode += 8;
	if (vertex->loc[1] < -vertex->loc[3])
		clipcode += 16;
	if (vertex->loc[1] > vertex->loc[3])
		clipcode += 32;
    }

    vertex->clipcode = clipcode;

#ifdef debug
    printf("dor_clip3d_code_vertex:");
    dor_clip3d_print_vertex(vertex);
#endif

    return(clipcode);
}
/*
 ======================================================================
 */

/*  Intersect an edge with a boundary,and return point of intersection.  */

void dor_clip3d_edge_boundary (
    dot_clip3d_vertex *vertex0,
    dot_clip3d_vertex *vertex1,
    DtInt              boundary,
    dot_clip3d_vertex *vi)
{
    double alpha;

    switch(boundary){
	/* z =  -w */

    case 1:
	alpha = -(vertex0->loc[3]+vertex0->loc[2]) /
		((vertex1->loc[2]-vertex0->loc[2])+
		 (vertex1->loc[3]-vertex0->loc[3]));
	break;

	/* z =  0 */

    case 2:
	alpha = -vertex0->loc[2] / (vertex1->loc[2]-vertex0->loc[2]);
	break;

	/* x = -w */

    case 4:
	alpha = (-vertex0->loc[3]-vertex0->loc[0]) /
		((vertex1->loc[0]-vertex0->loc[0])+
		 (vertex1->loc[3]-vertex0->loc[3]));
	break;

	/* x =  w */

    case 8:
	alpha = (vertex0->loc[3]-vertex0->loc[0]) /
		((vertex1->loc[0]-vertex0->loc[0])-
		 (vertex1->loc[3]-vertex0->loc[3]));
	break;

	/* y = -w */

    case 16:
	alpha = (-vertex0->loc[3]-vertex0->loc[1]) /
		((vertex1->loc[1]-vertex0->loc[1])+
		 (vertex1->loc[3]-vertex0->loc[3]));
	break;

	/* y =  w */

    case 32:
	alpha = (vertex0->loc[3]-vertex0->loc[1]) /
		((vertex1->loc[1]-vertex0->loc[1])-
		 (vertex1->loc[3]-vertex0->loc[3]));
	break;
    }

    vi->loc[0] = vertex0->loc[0] + 
			alpha*(vertex1->loc[0] - vertex0->loc[0]);
    vi->loc[1] = vertex0->loc[1] + 
			alpha*(vertex1->loc[1] - vertex0->loc[1]);
    vi->loc[2] = vertex0->loc[2] + 
			alpha*(vertex1->loc[2] - vertex0->loc[2]);
    vi->loc[3] = vertex0->loc[3] + 
			alpha*(vertex1->loc[3] - vertex0->loc[3]);

    if (doe_clip3d_interpolateswitch){
	vi->interpolants[0] = vertex0->interpolants[0] +
		alpha*(vertex1->interpolants[0] - vertex0->interpolants[0]);
	vi->interpolants[1] = vertex0->interpolants[1] +
		alpha*(vertex1->interpolants[1] - vertex0->interpolants[1]);
	vi->interpolants[2] = vertex0->interpolants[2] +
		alpha*(vertex1->interpolants[2] - vertex0->interpolants[2]);
    }

    /* Now make sure that the clipped edge is actually at the edge.
       This makes up for roundoff error in the alpha, 1-alpha
       calculations.  */

    switch(boundary){
	/* z =  -w */

    case 1:
	vi->loc[2] = -vi->loc[3];
	break;

	/* z =  0 */

    case 2:
	vi->loc[2] =  0.0;
	break;

	/* x = -w */

    case 4:
	vi->loc[0] = -vi->loc[3];
	break;

	/* x =  w */

    case 8:
	vi->loc[0] =  vi->loc[3];
	break;

	/* y = -w */

    case 16:
	vi->loc[1] = -vi->loc[3];
	break;

	/* y =  w */

    case 32:
	vi->loc[1] =  vi->loc[3];
	break;
    }

    (void)dor_clip3d_code_vertex(vi);

    /*
      printf("dor_clip3d_edge_boundary:\n");
      dor_clip3d_print_vertex(vi);
      */
}
/*
 ======================================================================
 */

/*  Clip a point against the frustum of vision.  */

void dor_clip3d_point (
    dot_clip3d_vertex *vertex,
    DtPFV              action,
    DtPtr              actionparm)
{
    DtInt clipcode;

    clipcode = dor_clip3d_code_vertex(vertex);

#ifdef debug
    printf("clip point:");
    dor_clip3d_print_vertex(vertex);
    printf("\n");
#endif

    /*  Trivial rejection test.  */

    if (clipcode){
	return;
    }

    (action)(vertex,actionparm);
}
/*
 ======================================================================
 */

void dor_clip3d_print_vertex (dot_clip3d_vertex *vertex)
{
    printf("	(%10.4g,%10.4g,%10.4g,%10.4g) (%5.1f,%5.1f,%5.1f) %x\n",
	vertex->loc[0],vertex->loc[1],vertex->loc[2],vertex->loc[3],
	vertex->interpolants[0], vertex->interpolants[1],
	vertex->interpolants[2], vertex->clipcode);
}
/*
 ======================================================================
 */

/*  Clip a triangle against the normalized frustum of vision.  */

void dor_clip3d_triangle (
    dot_clip3d_vertex *vertex0,
    dot_clip3d_vertex *vertex1,
    dot_clip3d_vertex *vertex2,
    DtSwitch           interpolateswitch,
    DtPFV              action,
    DtPtr              actionparm)
{
    DtInt clipcode0,clipcode1,clipcode2;

    clipcode0 = dor_clip3d_code_vertex(vertex0);
    clipcode1 = dor_clip3d_code_vertex(vertex1);
    clipcode2 = dor_clip3d_code_vertex(vertex2);

    /* Trivial acceptance test */

    if (!(clipcode0 | clipcode1 | clipcode2)){
	(action)(vertex0,vertex1,vertex2,0,actionparm);

	return;
    }

    /* Trivial rejection test */

    if (clipcode0 & clipcode1 & clipcode2){
	return;
    }

    /*  Otherwise, must clip against some boundary.  */

    doe_clip3d_interpolateswitch = interpolateswitch;
    doe_clip3d_action = action;
    doe_clip3d_actionparm = actionparm;

    dor_clip3d_triangle_boundary(vertex0,vertex1,vertex2,1);
}
/*
 ======================================================================
 */

/*  Clip a triangle against some boundary.  */

void dor_clip3d_triangle_boundary (
    dot_clip3d_vertex *vertex0,
    dot_clip3d_vertex *vertex1,
    dot_clip3d_vertex *vertex2,
    DtInt              boundary)
{
    dot_clip3d_vertex vertex02, vertex13, vertex21;
    DtInt clipcode0, clipcode1, clipcode2;
    DtInt nextboundary;
    DtInt caseid;

    clipcode0 = vertex0->clipcode;
    clipcode1 = vertex1->clipcode;
    clipcode2 = vertex2->clipcode;

#ifdef DEBUG
      printf("dor_clip3d_triangle_boundary:\n");
      dor_clip3d_print_vertex(vertex0);
      dor_clip3d_print_vertex(vertex1);
      dor_clip3d_print_vertex(vertex2);
#endif

    if (clipcode0 & clipcode1 & clipcode2)
	    return;

    /* Look for a boundary that has at least one vertex outside it.  */

    for (;;){
	if (boundary == doe_clip3d_max_clipcode){
	    /* No more boundaries; display triangle.  */

	    (doe_clip3d_action)(vertex0,vertex1,vertex2,1,doe_clip3d_actionparm);

	    return;
	}

	if ((clipcode0 | clipcode1 | clipcode2) & boundary){
	    break;
	}

	boundary <<= 1;
    }

    /* Now compute the points of intersection along edges 12,23,31,
       if they exist at all.  This requires computing new values for
       x,y,z,y and w.  */

    if ((clipcode0 & boundary) != (clipcode1 & boundary))
	    dor_clip3d_edge_boundary(vertex0,vertex1,boundary,&vertex02);
    if ((clipcode1 & boundary) != (clipcode2 & boundary))
	    dor_clip3d_edge_boundary(vertex1,vertex2,boundary,&vertex13);
    if ((clipcode2 & boundary) != (clipcode0 & boundary))
	    dor_clip3d_edge_boundary(vertex2,vertex0,boundary,&vertex21);
	
    /* Now we need to examine cases and split out the right set of
       triangles to fill the clipped shape.  The "cases" can be
       described by the set of original vertices that are inside or
       outside the boundary.  So we start by computing a "caseid"
       based on the bits in the outcode.  */

    nextboundary = boundary << 1;

    caseid = 0;

    if (clipcode0 & boundary)
	    caseid += 1;
    if (clipcode1 & boundary)
	    caseid += 2;
    if (clipcode2 & boundary)
	    caseid += 4;

#ifdef debug
    printf("dor_clip3d_triangle_boundary: case = %d\n",caseid);
#endif

    switch(caseid){
	/* All vertices in and all vertices out have already
	   been handled.  */

    case 0:
    case 7:
#ifdef debug
	printf("Cannot happen\n");
#endif
	break;
	
	/* Vertex 1 outside */

    case 1:
	dor_clip3d_triangle_boundary(&vertex02,vertex1,vertex2,nextboundary);
	dor_clip3d_triangle_boundary(&vertex02,vertex2,&vertex21,nextboundary);
	break;
	
	/* Vertex 2 outside */

    case 2:
	dor_clip3d_triangle_boundary(&vertex13,vertex2,vertex0,nextboundary);
	dor_clip3d_triangle_boundary(&vertex13,vertex0,&vertex02,nextboundary);
	break;
	
	/* Vertex 3 outside */

    case 4:
	dor_clip3d_triangle_boundary(&vertex21,vertex0,vertex1,nextboundary);
	dor_clip3d_triangle_boundary(&vertex21,vertex1,&vertex13,nextboundary);
	break;
	
	/* vertices 1 & 2 outside */

    case 3:
	dor_clip3d_triangle_boundary(vertex2,&vertex21,&vertex13,nextboundary);
	break;
	
	/* vertices 2 & 3 outside */

    case 6:
	dor_clip3d_triangle_boundary(vertex0,&vertex02,&vertex21,nextboundary);
	break;
	
	/* vertices 3 & 1 outside */

    case 5:
	dor_clip3d_triangle_boundary(vertex1,&vertex13,&vertex02,nextboundary);
	break;
    }
}
/*
 ======================================================================
 */

void dor_clip3d_test_point (
    DtPoint3    loc,
    DtMatrix4x4 matrix,
    DtPFV       action,
    DtPtr       actionparm)
{
    DtReal origpoint[4];
    dot_clip3d_vertex vertex1;

    origpoint[0] = loc[0];
    origpoint[1] = loc[1];
    origpoint[2] = loc[2];
    origpoint[3] = 1.;

    dor_math_homo_trns_fast(origpoint,matrix,vertex1.loc);

    dor_clip3d_point(&vertex1,action,actionparm);
}
/*
 ======================================================================
 */

/* Transform triangle coordinates using "matrix"; apply "action"
   on triangle if it is entirely visible; otherwise clip triangle in
   all directions, then apply "action" on resulting visible triangle(s) */

void dor_clip3d_test_triangle (
    DtPoint3    loc1,
    DtPoint3    loc2,
    DtPoint3    loc3,
    DtMatrix4x4 matrix,
    DtPFV       action,
    DtPtr       actionparm)
{
    DtReal origpoint[4];
    dot_clip3d_vertex vertex1, vertex2, vertex3;

    origpoint[0] = loc1[0];
    origpoint[1] = loc1[1];
    origpoint[2] = loc1[2];
    origpoint[3] = 1.;

    dor_math_homo_trns_fast(origpoint,matrix,vertex1.loc);

    origpoint[0] = loc2[0];
    origpoint[1] = loc2[1];
    origpoint[2] = loc2[2];
    origpoint[3] = 1.;

    dor_math_homo_trns_fast(origpoint,matrix,vertex2.loc);

    origpoint[0] = loc3[0];
    origpoint[1] = loc3[1];
    origpoint[2] = loc3[2];
    origpoint[3] = 1.;

    dor_math_homo_trns_fast(origpoint,matrix,vertex3.loc);

    /* clip triangle in every direction */
    doe_clip3d_max_clipcode = 64;

    dor_clip3d_triangle(&vertex1,&vertex2,&vertex3,(DtSwitch)DcFalse,action,
			actionparm);
}
/*
 ======================================================================
 */

/* Transform triangle coordinates using "matrix"; apply "action"
   on triangle if it is entirely visible; otherwise clip triangle only
   in hither and yon, then apply "action" on triangle

   For instance, this routine is used by the bounding volume code:
   to determine if a volume should be replaced with some alternate
   object, the extent of the volume as if it were projected on an
   infinite size device must be compared against the DoMinBoundinVolExt
   value */

void dor_clip3d_test_triangle_clip_depth_only (
    DtPoint3    loc1,
    DtPoint3    loc2,
    DtPoint3    loc3,
    DtMatrix4x4 matrix,
    DtPFV       action,
    DtPtr       actionparm)
{
    DtReal origpoint[4];
    dot_clip3d_vertex vertex1, vertex2, vertex3;

    origpoint[0] = loc1[0];
    origpoint[1] = loc1[1];
    origpoint[2] = loc1[2];
    origpoint[3] = 1.;

    dor_math_homo_trns_fast(origpoint,matrix,vertex1.loc);

    origpoint[0] = loc2[0];
    origpoint[1] = loc2[1];
    origpoint[2] = loc2[2];
    origpoint[3] = 1.;

    dor_math_homo_trns_fast(origpoint,matrix,vertex2.loc);

    origpoint[0] = loc3[0];
    origpoint[1] = loc3[1];
    origpoint[2] = loc3[2];
    origpoint[3] = 1.;

    dor_math_homo_trns_fast(origpoint,matrix,vertex3.loc);

    /* we only want the triangle to be clipped with hither and yon */
    doe_clip3d_max_clipcode = 2;

    dor_clip3d_triangle(&vertex1,&vertex2,&vertex3,(DtSwitch)DcFalse,action,
			actionparm);
}
/*
 ======================================================================
 */

void dor_clip3d_test_vector (
    DtPoint3    loc1,
    DtPoint3    loc2,
    DtMatrix4x4 matrix,
    DtPFV       action,
    DtPtr       actionparm)
{
    DtReal origpoint[4];
    dot_clip3d_vertex vertex1, vertex2;

    origpoint[0] = loc1[0];
    origpoint[1] = loc1[1];
    origpoint[2] = loc1[2];
    origpoint[3] = 1.;

    dor_math_homo_trns_fast(origpoint,matrix,vertex1.loc);

    origpoint[0] = loc2[0];
    origpoint[1] = loc2[1];
    origpoint[2] = loc2[2];
    origpoint[3] = 1.;

    dor_math_homo_trns_fast(origpoint,matrix,vertex2.loc);

    dor_clip3d_vector(&vertex1,&vertex2,(DtSwitch)DcFalse,action,actionparm);
}
/*
 ======================================================================
 */

/*  Clip a vector against the normalized frustum of vision.  */

void dor_clip3d_vector (
    dot_clip3d_vertex *vertex0,
    dot_clip3d_vertex *vertex1,
    DtSwitch           interpolateswitch,
    DtPFV              action,
    DtPtr              actionparm)
{
    DtInt clipcode0, clipcode1;

    clipcode0 = dor_clip3d_code_vertex(vertex0);
    clipcode1 = dor_clip3d_code_vertex(vertex1);

#ifdef debug
    printf("dor_clip3d_vectro:\n");
    dor_clip3d_print_vertex(vertex0);
    dor_clip3d_print_vertex(vertex1);
    fprintf(stderr,"\n");
#endif

    /* Trivial acceptance test */

    if (!(clipcode0 | clipcode1)){
	(action)(vertex0,vertex1,0,actionparm);

	return;
    }

    /* Trivial rejection test */

    if (clipcode0 & clipcode1){
	return;
    }

    /*  Otherwise, must clip against some boundary.  */

    doe_clip3d_interpolateswitch = interpolateswitch;
    doe_clip3d_action = action;
    doe_clip3d_actionparm = actionparm;

    dor_clip3d_vector_boundary(vertex0,vertex1,1);
}
/*
 ======================================================================
 */

/*  Clip a vector against some boundary.  */

void dor_clip3d_vector_boundary (
    dot_clip3d_vertex *vertex0,
    dot_clip3d_vertex *vertex1,
    DtInt              boundary)
{
    dot_clip3d_vertex vertex02;
    DtInt clipcode0 = vertex0->clipcode;
    DtInt clipcode1 = vertex1->clipcode;
    DtInt nextboundary, caseid;

#ifdef debug
    fprintf(stderr,
	    "dor_clip3d_vector_boundary,boundary = %d\n",boundary);
#endif

    if (clipcode0 & clipcode1)
	    return;

    /* Look for a boundary that has at least one vertex outside it.  */

    for (;;){
	if (boundary == 64){
	    /* no more boundaries; display vector */

	    (doe_clip3d_action)(vertex0,vertex1,1,doe_clip3d_actionparm);

	    return;
	}

	if ((clipcode0|clipcode1)&boundary)
		break;

	boundary <<= 1;
    }

    /* Now compute the intersection of the line and boundary */

    if ((clipcode0 & boundary) != (clipcode1 & boundary))
	    dor_clip3d_edge_boundary(vertex0,vertex1,boundary,&vertex02);
	
    /* Now we need to examine cases and split out the portion of
       the line which is not clipped.  The "cases" can be described by
       the set of original vertices that are inside or outside the
       boundary.  So we start by computing a "caseid" based on the
       bits in the outcode.  */

    nextboundary = boundary << 1;

    caseid = 0;

    if (clipcode0 & boundary)
	    caseid += 1;
    if (clipcode1 & boundary)
	    caseid += 2;

#ifdef debug
    fprintf(stderr,"dor_clip3d_vector_boundary: case = %d\n",caseid);
#endif

    switch(caseid){
	/* Both vertices in and all vertices out have already
	   been handled.  */

    case 0:
    case 3:
#ifdef debug
	fprintf(stderr,"Cannot happen\n");
#endif
	break;
	
	/* Vertex 1 outside */

    case 1:
	dor_clip3d_vector_boundary(&vertex02,vertex1,nextboundary);
	break;
	
	/* Vertex 2 outside */

    case 2:
	dor_clip3d_vector_boundary(vertex0,&vertex02,nextboundary);
	break;
	
    }
}
