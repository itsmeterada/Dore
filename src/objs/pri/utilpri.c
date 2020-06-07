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
    dor_check_degen_triangle_vtx_values
    dor_check_degen_quad_vtx_values
    dor_utilpri_print_vertextype
	
  ======================================================================
 */

#include <dore/dore.h>
#include <dore/internal/dogen.h>

/*
 * This file contains utility functions that are used or
 * might be used by different primitives
 */

/*
 ======================================================================
 */

/* Return false if at least two of the vertex lcoations are the same 
   Use floating point comparisons (can be used for a mesh or list) */

DtFlag dor_check_degen_triangle_vtx_values (
    DtReal vertex1[3],
    DtReal vertex2[3],
    DtReal vertex3[3])
{
    if (vertex1[0] == vertex2[0] && vertex1[1] == vertex2[1] &&
        vertex1[2] == vertex2[2])
            return(DcFalse);

    if (vertex1[0] == vertex3[0] && vertex1[1] == vertex3[1] &&
        vertex1[2] == vertex3[2])
            return(DcFalse);

    if (vertex2[0] == vertex3[0] && vertex2[1] == vertex3[1] &&
        vertex2[2] == vertex3[2])
            return(DcFalse);

    return(DcTrue);
}

/*
 ======================================================================
 */

/* Return false if the quad is degenerate to a point or a line.
   If the four vertices are different, or if only two vertices are
   identical (so equivalent to a triangle), we return true */

DtFlag dor_quadmsh_check_degen_quad_vtx_values (
    DtReal vertex1[3],
    DtReal vertex2[3],
    DtReal vertex3[3],
    DtReal vertex4[3])
{

    if (vertex1[0] == vertex2[0] && vertex2[0] == vertex3[0] &&
        vertex1[1] == vertex2[1] && vertex2[1] == vertex3[1] &&
        vertex1[2] == vertex2[2] && vertex2[2] == vertex3[2])
        return(DcFalse);

    if (vertex1[0] == vertex3[0] && vertex3[0] == vertex4[0] &&
        vertex1[1] == vertex3[1] && vertex3[1] == vertex4[1] &&
        vertex1[2] == vertex3[2] && vertex3[2] == vertex4[2])
        return(DcFalse);

    if (vertex2[0] == vertex3[0] && vertex3[0] == vertex4[0] &&
        vertex2[1] == vertex3[1] && vertex3[1] == vertex4[1] &&
        vertex2[2] == vertex3[2] && vertex3[2] == vertex4[2])
        return(DcFalse);

    return(DcTrue);
}


/*
 ======================================================================
 */

void dor_utilpri_print_vertextype(
    DtVertexType vtype)
{
    switch (vtype){
    case DcLoc:
        dor_print_output("vertextype = DcLoc");
        break;

    case DcLocNrm:
        dor_print_output("vertextype = DcLocNrm");
        break;

    case DcLocClr:
        dor_print_output("vertextype = DcLocClr");
        break;

    case DcLocNrmClr:
        dor_print_output("vertextype = DcLocNrmClr");
        break;

    case DcLocAlpha:
        dor_print_output("vertextype = DcLocAlpha");
        break;

    case DcLocNrmAlpha:
        dor_print_output("vertextype = DcLocNrmAlpha");
        break;

    case DcLocClrAlpha:
        dor_print_output("vertextype = DcLocClrAlpha");
        break;

    case DcLocNrmClrAlpha:
        dor_print_output("vertextype = DcLocNrmClrAlpha");
        break;

    default:
        dor_print_output("vertextype = UNKNOWN");
        break;
    }
}

/*
 ======================================================================
 */
