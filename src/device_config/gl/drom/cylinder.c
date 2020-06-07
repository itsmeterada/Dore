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
 
/*****************************************************************************
**  This file contains method routines for cylinder primitives.
*****************************************************************************/

#include <math.h>
#include "../gl_driver.h"

    /*** Macro Definitions ***/

#define DEBUG_CALL  0
#define DEBUG_CYL   0

#define CYL_POINT   1		/* GL Tesselation Type */
#define CYL_LINE    2
#define CYL_FILL    3


typedef float glvert[3];	/* GL Vertex */


    /*******************************/
    /***  Function Declarations  ***/
    /*******************************/

static void drawGLCylinder    (DtInt, DtInt);
static void cylinderDrawPoint (glvert*, DtInt);
static void cylinderDrawLine  (glvert*, DtInt);
static void cylinderDrawFill  (glvert*, DtInt);



/*****************************************************************************
**  This procedure generates a GL cylinder object.  If an error
**  occurs such that the cylinder cannot be generated, then this routine returns
**  a -1 for the object identifier.  If the `object_id' parameter is -1, then
**  a new object identifier is generated, otherwise the given value is used.
**  On successful completion, this routine returns the object identifier.
*****************************************************************************/

Object MakeGLCylinder (
    Object  object_id,		/* Existing GL Object Identifier, Else -1 */
    DtInt  *prevprimtype,	/* Previous Tesselation Primitive Type */
    DtInt  *prevdepth)		/* Previous Tesselation Depth */
{
    auto     int         depth;			/* Sphere Tesselation Depth */
    auto     int         primtype;		/* Sphere Tess. Prim Type */
    static   DtMethodPtr routine_subdiv=0;	/* Subdiv Query Routine Ptr */
    auto     DtInt       subdivtype;		/* Current Subdivision Type */
    auto     DtReal      subdivlevel[1];	/* Current Subdivision Level */

#   if DEBUG_CALL
	printf ("* MakeGLCylinder (\n");
	printf (":     object_id %08lX,\n", object_id);
	printf (":     prevprimtype %d, prevdepth %d,\n",
	    *prevprimtype, *prevdepth);
#   endif

    /* If we haven't yet grabbed the subdivision inquiry routine address,
    ** then do it now (only done once). */

    if (!routine_subdiv)
    {   routine_subdiv = DDclass_InqMethod
			 (   DsInqClassId  ("AttSubDivSpec"),
			     DsInqMethodId ("InqGlobalValue")
			 );
    }

    /* Get the current subdivision parameters. */

    (*routine_subdiv) (&subdivtype, subdivlevel);

    /* Determine the GL cylinder tesselation depth, depending on the Dore'
    ** subdivision type and parameter.  Note that for the DcSubDivAbsolute
    ** and DcSubDivRelative subdivision types, we're just sort of faking it. */

#   if DEBUG_CYL
	printf ("-- subdivtype %d, subdivlevel %f\n",
	    subdivtype, subdivlevel[0]);
#   endif

    switch (subdivtype)
    {
	case DcSubDivSegments:
	    depth = subdivlevel[0];
	    break;
	
	case DcSubDivFixed:
	    if (subdivlevel[0] <= 5)
		depth = 3;
	    else
		depth = 1 << ((int)(subdivlevel[0]) - 3);
	    break;
	
	case DcSubDivAbsolute:
	case DcSubDivRelative:
	    depth = (10 * subdivlevel[0]) + 2;
	    break;

	default:
	    depth = 6;
	    break;
    }

    /* Clamp the depth to a reasonable range.  The upper limit of 256 is a
    ** result of the fact that GL polygons can have no more than 256 vertices;
    ** the cylinders are generated with a single polygon for each surface
    ** end-cap.  */

    depth = CLAMP (depth, 3, 256);

    /* Select the GL sphere's primitive type (points, lines or polygonal mesh)
    ** depending on the Dore' reptype.  The default value is polygonal mesh. */

    switch (DevStatus.reptype)
    {
	case DcPoints:
            primtype = CYL_POINT;
            break;

        case DcWireframe:
        case DcOutlines:
            primtype = CYL_LINE;
            break;

        default:
        case DcSurface:
        case DcSurfaceWireframeEdges:
        case DcSurfaceOutlineEdges:
            primtype = CYL_FILL;
            break;
    }

    /* If the given object identifier is -1, then we need to create a new
    ** object, instead of re-using a previous object id.  If the object ID is
    ** valid (not -1), then we don't
    ** need to re-create the cylinder if the tesselation primitive type and
    ** tesselation depth are unchanged.  */

    if (object_id == -1)
	object_id = genobj();
    else if ((depth == *prevdepth) && (primtype == *prevprimtype))
	return object_id;

    *prevdepth    = depth;
    *prevprimtype = primtype;

    makeobj (object_id);
    drawGLCylinder (primtype,depth);
    closeobj ();

    return object_id;
}



/*****************************************************************************
**  This routine calls the appropriate cylinder-generation function, depending
**  on the current surface representation type.
*****************************************************************************/

static void drawGLCylinder (
    DtInt primtype,       /* Tesselation Primitive Type */
    DtInt depth)          /* Tesselation Depth */
{
    auto      DtReal  theta;	/* Circle Angle */
    auto      DtReal  delta;	/* Delta Angle for Theta Increment */
    register  int     ii;	/* Loop Counter */
    register  glvert *varray;	/* Vertex Array and Traversal Pointer */

    /* Allocate the array to hold the vertex positions.  Make it one vertex
    ** larger to hold the center position of the endcaps.  The end result
    ** is an array of XY coordinates for a unit circle.  The Z coordinate
    ** is left undefined.  */

    varray = DDspace_Allocate (sizeof(glvert) * (depth+1));

#   if DEBUG_CYL
	printf ("* drawGLCylinder (primtype %d, depth %d)\n", primtype, depth);
#   endif

    /* The first element in the array is the center of the endcap */

    varray[0][0] = 0.0;
    varray[0][1] = 0.0;

    /* fill the rest of the array with the cylinder vertex positions */

    theta = 0.0;
    delta = (2*M_PI) / (DtReal)(depth);

    for (ii=1;  ii <= depth;  ++ii)
    {   varray[ii][0] = cos (theta);
	varray[ii][1] = sin (theta);
	theta += delta;
    }

#   ifdef SGI_POLYMODE
	switch (primtype)
	{   case CYL_POINT:
                polymode (PYM_POINT);
                break;
	    case CYL_LINE:
                polymode (PYM_LINE);
                break;
	    case CYL_FILL:
	    default:
                polymode (PYM_FILL);
                break;
        }
#   else
        switch (primtype)
	{   case CYL_POINT:
                cylinderDrawPoint (varray, depth);
		break;
	    case CYL_LINE:
                cylinderDrawLine (varray, depth);
                break;
	    case CYL_FILL:
	    default:
                cylinderDrawFill (varray, depth);
        }
#   endif

    /* free the allocated array */

    DDspace_Deallocate (varray);
}



/*****************************************************************************
**  This routine generates the point representation of a cylinder using GL
**  calls.
*****************************************************************************/

static void cylinderDrawPoint (
    glvert *varray,
    DtInt depth)
{
    register int ii;	/* Loop Index */

    /* Traverse the vertex circle for both endcaps at the same time. */

    bgnpoint();

    for (ii=0;  ii <= depth;  ++ii)
    {   n3f(varray[ii]);
	varray[ii][2] = 0.0;  v3f(varray[ii]);
	varray[ii][2] = 1.0;  v3f(varray[ii]);
    }

    endpoint();
}



/*****************************************************************************
**  This routine generates the wireframe representation of the cylinder, using
**  GL functions.
*****************************************************************************/

static void cylinderDrawLine (
    glvert *varray,	/* Circle Vertex Array */
    DtInt depth)	/* Number of Vertices Around the Circle */
{
    register int    ii;				/* Loop Counter */
    static   glvert end_normal = {0,0,0};	/* Endcap Normal Vector */

    /* Draw the cap at Z=0. */

    for (ii=0;  ii <= depth;  ++ii)	/* Set Z=0 */
	varray[ii][2] = 0;

    end_normal[2] = -1;
    bgnline ();
    n3f (end_normal);

    /* The first vertex is specified twice because the first line segment
    ** exhibits some strange lighting problems.  */

    v3f (varray[0]);

    for (ii=1;  ii < depth;  ++ii)
    {   v3f (varray[0]);
	v3f (varray[ii]);
	v3f (varray[ii+1]);
    }
    v3f (varray[0]);
    v3f (varray[depth]);
    v3f (varray[1]);

    endline();

    /* Draw the cap at Z=1. */

    for (ii=0; ii <= depth;  ++ii)	/* Set Z=1  */
	varray[ii][2] = 1.0;

    end_normal[2] = 1;
    bgnline ();
    n3f (end_normal);
    v3f (varray[0]);

    for (ii=1;  ii < depth;  ++ii)
    {   v3f (varray[0]);
	v3f (varray[ii]);
	v3f (varray[ii+1]);
    }
    v3f (varray[0]);
    v3f (varray[depth]);
    v3f (varray[1]);
    endline ();

    /* Draw the lines around the wall of the cylinder. */

    bgnline ();
    n3f (varray[1]);
    v3f (varray[1]);
    for (ii=1;  ii <= depth;  ++ii)
    {   n3f (varray[ii]);
	v3f (varray[ii]);
	varray[ii][2] = 0;
	v3f (varray[ii]);
    }

    varray[1][2] = 1.0;
    n3f (varray[1]);
    v3f (varray[1]);
    endline ();
}



/*****************************************************************************
**  This routine generates the surface representation of the cylinder, using
**  GL functions.
*****************************************************************************/

static void cylinderDrawFill (
    glvert *varray,
    DtInt depth)
{
    auto     glvert  end_normal;	/* Endcap Normal Vector */
    register int     ii;		/* Loop Counter */

    /* End Cap:  Z=0 */

    end_normal[0] = 0;
    end_normal[1] = 0;
    end_normal[2] = -1;

    bgnpolygon ();
    n3f (end_normal);
    for (ii=depth;  ii;  --ii)
    {   varray[ii][2] = 0;
	v3f (varray[ii]);
    }
    endpolygon ();

    /* End Cap:  Z=1 */

    end_normal[2] = 1;
    bgnpolygon ();
    n3f (end_normal);
    for (ii=1;  ii <= depth;  ++ii)
    {   varray[ii][2] = 1;
	v3f (varray[ii]);
    }
    endpolygon();

    /* Cylinder Wall */

    for (ii=1;  ii < depth;  ++ii)
    {   bgnpolygon ();
	varray[ii  ][2] = 0;  n3f (varray[ii]);
	varray[ii  ][2] = 1;  v3f (varray[ii  ]);
	varray[ii  ][2] = 0;  v3f (varray[ii  ]);
	varray[ii+1][2] = 0;  n3f (varray[ii+1]);
			      v3f (varray[ii+1]);
	varray[ii+1][2] = 1;  v3f (varray[ii+1]);
	endpolygon ();
    }
    bgnpolygon ();
    varray[depth][2] = 0;  n3f (varray[depth]);
    varray[depth][2] = 1;  v3f (varray[depth]);
    varray[depth][2] = 0;  v3f (varray[depth]);
    varray[1][2] = 0;  n3f (varray[1]);
		       v3f (varray[1]);
    varray[1][2] = 1;  v3f (varray[1]);
    endpolygon ();
}
