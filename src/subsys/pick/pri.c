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
	dor_pick_point_list
	dor_pick_line_list
	dor_pick_connected_line_list
	dor_pick_triangle_list
	dor_pick_triangle_mesh
	dor_pick_triangle_strip
	dor_pick_report_point
	dor_pick_report_triangle
	dor_pick_report_vector
	dor_pick_bounding_box_out

  ======================================================================
 */
#include "pick.h"

/* ====================================================================== */

void dor_pick_point_list (
    DtRealTriple bndboxpts[8],
    DtInt vertexcount,
    DtRealTriple *vertices)
{
    DtRepType reptype;
    DtInt vertexno;

    if (dor_pick_bounding_box_out(bndboxpts)) {
   	return;
    }

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    case DcPoints:
    case DcWireframe:
    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
    case DcOutlines:
	for (vertexno=0;vertexno<vertexcount;vertexno++) {
	    dor_clip3d_test_point(vertices[vertexno],
				  doe_pick_data.lcstofcsmat,
				  dor_pick_report_point,DcNullPtr);
	}
	break;
    }
}

/* ====================================================================== */

void dor_pick_line_list (
    DtRealTriple bndboxpts[8],
    DtInt linecount,
    DtReal vertices[][3])
{
    DtRepType reptype;
    DtInt vertexno;
    DtInt lineno;
    DtInt basevertexno;

    if (dor_pick_bounding_box_out(bndboxpts)) {
   	return;
    }

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    case DcPoints:
	for (vertexno=0;vertexno<linecount*2;vertexno++) {
	    dor_clip3d_test_point(vertices[vertexno],
				  doe_pick_data.lcstofcsmat,
				  dor_pick_report_point,DcNullPtr);
	}
	break;
   		
    case DcWireframe:
    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
    case DcOutlines:
	for (lineno=0;lineno<linecount;lineno++) {
	    basevertexno = 2 * lineno;

	    dor_clip3d_test_vector
	    (   vertices[basevertexno], vertices[basevertexno+1],
		doe_pick_data.lcstofcsmat, (DtPFV) dor_pick_report_vector,
		DcNullPtr
	    );
	}
	break;
    }
}

/* ====================================================================== */

void dor_pick_connected_line_list (
    DtRealTriple bndboxpts[8],
    DtInt vertexcount,
    DtRealTriple *vertices)
{
    DtRepType reptype;
    DtInt vertexno;

    if (dor_pick_bounding_box_out(bndboxpts)) {
   	return;
    }

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    case DcPoints:
	for (vertexno=0;vertexno<vertexcount;vertexno++) {
	    dor_clip3d_test_point
	    (   vertices[vertexno], doe_pick_data.lcstofcsmat,
	        dor_pick_report_point, DcNullPtr
	    );
	}
	break;
   		
    case DcWireframe:
    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
    case DcOutlines:
	for (vertexno=0;vertexno<vertexcount-1;vertexno++) {
	    dor_clip3d_test_vector
	    (   vertices[vertexno], vertices[vertexno+1],
		doe_pick_data.lcstofcsmat, (DtPFV) dor_pick_report_vector,
		DcNullPtr
	    );
	}
	break;
    }
}

/* ====================================================================== */

void dor_pick_triangle_list (
    DtRealTriple bndboxpts[8],
    DtInt trianglecount,
    DtRealTriple *vertices)
{
    DtRepType reptype;
    DtInt vertexno;
    DtInt triangleno;
    DtInt basevertexno;

    if (dor_pick_bounding_box_out(bndboxpts)) {
   	return;
    }

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    case DcPoints:
	for (vertexno=0;vertexno<trianglecount*3;vertexno++) {
	    dor_clip3d_test_point(vertices[vertexno],
				  doe_pick_data.lcstofcsmat,
				  dor_pick_report_point,
				  DcNullPtr);
	}
	break;
   		
    case DcWireframe:
    case DcOutlines:
	for (triangleno=0;triangleno<trianglecount;triangleno++) {
	    basevertexno = 3 * triangleno;

	    dor_clip3d_test_vector
	    (   vertices[basevertexno], vertices[basevertexno+1],
	        doe_pick_data.lcstofcsmat, (DtPFV) dor_pick_report_vector,
	        DcNullPtr
	    );

	    dor_clip3d_test_vector
	    (   vertices[basevertexno+1], vertices[basevertexno+2],
	        doe_pick_data.lcstofcsmat, (DtPFV) dor_pick_report_vector,
		DcNullPtr
	    );

	    dor_clip3d_test_vector
	    (   vertices[basevertexno+2], vertices[basevertexno],
	        doe_pick_data.lcstofcsmat, (DtPFV) dor_pick_report_vector,
		DcNullPtr
	    );
	}
	break;

    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
	for (triangleno=0;triangleno<trianglecount;triangleno++) {
	    dor_clip3d_test_triangle(vertices[triangleno*3],
				     vertices[triangleno*3+1],
				     vertices[triangleno*3+2],
				     doe_pick_data.lcstofcsmat,
				     (DtPFV) dor_pick_report_triangle,
				     DcNullPtr);
	}
	break;

    default:
	printf("unknown reptype\n");
	break;
    }
}

/* ====================================================================== */

void dor_pick_triangle_mesh (
    DtRealTriple bndboxpts[8],
    DtInt trianglecount,
    DtInt vertexcount,
    DtRealTriple *vertices,
    DtInt vertexlist[][3])
{
    DtRepType reptype;
    DtInt vertexno;
    DtInt triangleno;
    DtInt vertex1, vertex2, vertex3;

    if (dor_pick_bounding_box_out(bndboxpts)) {
   	return;
    }

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    case DcPoints:
	for (vertexno=0;vertexno<vertexcount;vertexno++) {
	    dor_clip3d_test_point(vertices[vertexno],
				  doe_pick_data.lcstofcsmat,
				  dor_pick_report_point,
				  DcNullPtr);
	}
	break;
   		
    case DcWireframe:
    case DcOutlines:
	for (triangleno=0;triangleno<trianglecount;triangleno++) {
	    vertex1 = vertexlist[triangleno][0];
	    vertex2 = vertexlist[triangleno][1];
	    vertex3 = vertexlist[triangleno][2];

	    dor_clip3d_test_vector
	    (   vertices[vertex1], vertices[vertex2],
	        doe_pick_data.lcstofcsmat, (DtPFV) dor_pick_report_vector,
		DcNullPtr
	    );

	    dor_clip3d_test_vector
	    (   vertices[vertex2], vertices[vertex3],
	        doe_pick_data.lcstofcsmat, (DtPFV) dor_pick_report_vector,
		DcNullPtr
	    );

	    dor_clip3d_test_vector
	    (   vertices[vertex3], vertices[vertex1],
	        doe_pick_data.lcstofcsmat, (DtPFV) dor_pick_report_vector,
		DcNullPtr
	    );
	}
	break;

    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
	for (triangleno=0;triangleno<trianglecount;triangleno++) {
	    vertex1 = vertexlist[triangleno][0];
	    vertex2 = vertexlist[triangleno][1];
	    vertex3 = vertexlist[triangleno][2];

	    dor_clip3d_test_triangle(vertices[vertex1],
				     vertices[vertex2],
				     vertices[vertex3],
				     doe_pick_data.lcstofcsmat,
				     (DtPFV) dor_pick_report_triangle,
				     DcNullPtr);
	}
	break;

    default:
	printf("unknown reptype\n");
	break;
    }
}

/* ====================================================================== */

void dor_pick_triangle_strip (
    DtRealTriple bndboxpts[8],
    DtInt vertexcount,
    DtRealTriple *vertices)
{
    DtRepType reptype;
    DtInt vertexno;
    DtInt basevtxno;

    if (dor_pick_bounding_box_out(bndboxpts)) {
   	return;
    }

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    case DcPoints:
	for (vertexno=0;vertexno<vertexcount;vertexno++) {
	    dor_clip3d_test_point(vertices[vertexno],
				  doe_pick_data.lcstofcsmat,
				  dor_pick_report_point,
				  DcNullPtr);
	}
	break;
   		
    case DcWireframe:
    case DcOutlines:
	for (basevtxno=0;basevtxno<vertexcount-2;basevtxno++) {
	    dor_clip3d_test_vector(vertices[basevtxno],
				   vertices[basevtxno+1],
				   doe_pick_data.lcstofcsmat,
				   (DtPFV) dor_pick_report_vector,
				   (char *)NULL);

	    dor_clip3d_test_vector(vertices[basevtxno+1],
				   vertices[basevtxno+2],
				   doe_pick_data.lcstofcsmat,
				   (DtPFV) dor_pick_report_vector,
				   (char *)NULL);

	    dor_clip3d_test_vector(vertices[basevtxno+2],
				   vertices[basevtxno],
				   doe_pick_data.lcstofcsmat,
				   (DtPFV) dor_pick_report_vector,
				   (char *)NULL);
	}
	break;

    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
	for (basevtxno=0;basevtxno<vertexcount-2;basevtxno++) {
	    dor_clip3d_test_triangle(vertices[basevtxno],
				     vertices[basevtxno+1],
				     vertices[basevtxno+2],
				     doe_pick_data.lcstofcsmat,
				     (DtPFV) dor_pick_report_triangle,
				     DcNullPtr);
	}
	break;

    default:
	printf("unknown reptype\n");
	break;
    }
}

/* ====================================================================== */

void dor_pick_report_point (
    dot_clip3d_vertex *point,
    DtPtr *data)
{
    DtReal wldpnt[4];

    doe_pick_data.hits_on_element++;

    if (point->loc[2] > doe_pick_data.front_z_of_hit) {
	doe_pick_data.front_z_of_hit = point->loc[2];

	dor_math_homo_trns_fast (point->loc, 
				 doe_pick_data.fcstowcsmat, wldpnt);
	doe_pick_data.wcspoint.x = wldpnt[0] / wldpnt[3];
	doe_pick_data.wcspoint.y = wldpnt[1] / wldpnt[3];
	doe_pick_data.wcspoint.z = wldpnt[2] / wldpnt[3];

	if (doe_pick_data.lcs_points != NULL) {
	    dor_math_homo_trns_fast (point->loc, 
				doe_pick_data.fcstolcsmat, wldpnt);
	    doe_pick_data.lcspoint.x = wldpnt[0] / wldpnt[3];
	    doe_pick_data.lcspoint.y = wldpnt[1] / wldpnt[3];
	    doe_pick_data.lcspoint.z = wldpnt[2] / wldpnt[3];
	}
    }
}

/* ====================================================================== */

void dor_pick_report_triangle (
    dot_clip3d_vertex *point1,
    dot_clip3d_vertex *point2,
    dot_clip3d_vertex *point3,
    DtShort clipped,
    DtPtr data)
{
    DtReal             wldpnt[4];
    dot_clip3d_vertex *near_point;

    doe_pick_data.hits_on_element++;

    if (point1->loc[2] > point2->loc[2])
	near_point = point1;
    else 
	near_point = point2;

    if (point3->loc[2] > near_point->loc[2]) 
	    near_point = point3;

    if (near_point->loc[2] > doe_pick_data.front_z_of_hit) {
	doe_pick_data.front_z_of_hit = near_point->loc[2];

	dor_math_homo_trns_fast (near_point->loc, 
				doe_pick_data.fcstowcsmat, wldpnt);
	doe_pick_data.wcspoint.x = wldpnt[0] / wldpnt[3];
	doe_pick_data.wcspoint.y = wldpnt[1] / wldpnt[3];
	doe_pick_data.wcspoint.z = wldpnt[2] / wldpnt[3];

	if (doe_pick_data.lcs_points != NULL) {
	    dor_math_homo_trns_fast (near_point->loc, 
				doe_pick_data.fcstolcsmat, wldpnt);
	    doe_pick_data.lcspoint.x = wldpnt[0] / wldpnt[3];
	    doe_pick_data.lcspoint.y = wldpnt[1] / wldpnt[3];
	    doe_pick_data.lcspoint.z = wldpnt[2] / wldpnt[3];
	}
    }
}

/* ====================================================================== */

void dor_pick_report_vector (
    dot_clip3d_vertex *point1,
    dot_clip3d_vertex *point2,
    DtShort clipped,
    DtPtr data)
{
    DtReal wldpnt[4];
    dot_clip3d_vertex *near_point;

    doe_pick_data.hits_on_element++;

    if (point1->loc[2] > point2->loc[2])
	near_point = point1;
    else
	near_point = point2;
	
    if (near_point->loc[2] > doe_pick_data.front_z_of_hit) {
	doe_pick_data.front_z_of_hit = near_point->loc[2];

	dor_math_homo_trns_fast (near_point->loc, 
				doe_pick_data.fcstowcsmat, wldpnt);
	doe_pick_data.wcspoint.x = wldpnt[0] / wldpnt[3];
	doe_pick_data.wcspoint.y = wldpnt[1] / wldpnt[3];
	doe_pick_data.wcspoint.z = wldpnt[2] / wldpnt[3];

	if (doe_pick_data.lcs_points != NULL) {
	    dor_math_homo_trns_fast (near_point->loc, 
				   doe_pick_data.fcstolcsmat, wldpnt);
	    doe_pick_data.lcspoint.x = wldpnt[0] / wldpnt[3];
	    doe_pick_data.lcspoint.y = wldpnt[1] / wldpnt[3];
	    doe_pick_data.lcspoint.z = wldpnt[2] / wldpnt[3];
	}
    }
}

/* ====================================================================== */

DtFlag dor_pick_bounding_box_out (
    DtRealTriple vtxs[8])
{
    DtFlag return_flag;
    DtInt vertexno;
    DtInt row, column;
    DtReal fcsvtxs[8][4];
    DtInt clipcode[8];

    /*  Transform the bounding boxes vertices into the frustum 
	coordinate system. 

	compute the clipcode for each of the vertices:
	1  - clipped by z yon
	2  - clipped by z hither
	4  - clipped by xmin
	8  - clipped by xmax
	16 - clipped by ymin
	32 - clipped by ymax
	64 - w is negative
	
	Trivial reject if all vertices are clipped by the same plane.
	*/

    for (vertexno=0;vertexno<8;vertexno++){
	for (row=0;row<4;row++){
	    fcsvtxs[vertexno][row] = 
			doe_pick_data.lcstofcsmat[row][3];

	    for (column=0;column<3;column++){
		fcsvtxs[vertexno][row] += 
			doe_pick_data.lcstofcsmat[row][column]*
					vtxs[vertexno][column];
	    }
	}
	clipcode[vertexno] = 0;
	if (fcsvtxs[vertexno][2] < -fcsvtxs[vertexno][3]) 
		clipcode[vertexno] += 1;
	if (fcsvtxs[vertexno][2] > 0) 
		clipcode[vertexno] += 2;
	if (fcsvtxs[vertexno][3] < 0) {
	    clipcode[vertexno] += 64;
	} else {
	    if (fcsvtxs[vertexno][0] < -fcsvtxs[vertexno][3]) 
		    clipcode[vertexno] += 4;
	    if (fcsvtxs[vertexno][0] > fcsvtxs[vertexno][3]) 
		    clipcode[vertexno] += 8;
	    if (fcsvtxs[vertexno][1] < -fcsvtxs[vertexno][3]) 
		    clipcode[vertexno] += 16;
	    if (fcsvtxs[vertexno][1] > fcsvtxs[vertexno][3]) 
		    clipcode[vertexno] += 32;
	}
    }

    if (clipcode[0] & clipcode[1] & clipcode[2] & clipcode[3] &
	clipcode[4] & clipcode[5] & clipcode[6] & clipcode[7])
	    return_flag = DcTrue;
    else
	    return_flag = DcFalse;

    return(return_flag);
}
