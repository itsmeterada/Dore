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
**  This file contains the procedures are used to convert a triangle-mesh
**  grid surface into a single triangle strip.  Triangle strips are (supposed
**  to be) much faster than triangle meshes on the G4 architecture.  The
**  main routine in this file generates the vertex list for a triangle strip
**  if the trimesh is indeed a gridded surface.
*****************************************************************************/

#include <dore/internal/dogen.h>

#if DEBUG
#   define dprintf(arglist)	printf arglist
#else
#   define dprintf(arglist)	/* Null */
#endif

    /***  Type Definitions  ***/

    /* The following type characterizes the way that a patch is triangulated.
    ** It specifies whether the diagonal seam goes up (from lower left to
    ** upper right) or down (from upper left to lower right), and whether the
    ** normal vector - by vertex order - goes in or out of the patch.  Note
    ** that this is more of a topological meaning than any real geometric
    ** meaning.  */

typedef enum { BAD_GRID=0, UP_OUT, UP_IN, DOWN_OUT, DOWN_IN } PatchType;
typedef enum { NORM_IN, NORM_OUT  } NormDirection;
typedef enum { DIAG_UP, DIAG_DOWN } DiagDirection;


    /***  Function Declarations  ***/

DtFlag ddr_g4vll_drom_gridmesh_to_strip
	    ARGS((DtInt, DtInt, DtInt[][3], DtInt*, DtInt**, DtInt*));

static PatchType CheckFirstPatch ARGS((DtInt[][3], DtInt));
static DtFlag    CheckAllPatches ARGS((DtInt[][3], DtInt, DtInt));
static void      StripGrid       ARGS((DtInt,DtInt,PatchType,DtInt*,DtInt**));



/*****************************************************************************
**  This routine analyzes trimesh connectivity and determines if the
**  underlying connectivity is a grid structure (NxM).  If the trimesh does
**  not define a gridded surface, then this routine returns DcFalse.  If the
**  trimesh does define a gridded surface, then it returns DcTrue and returns
**  the equivalent tristrip connectivity through the parameters 'Nstripverts'
**  and 'stripverts'.  'Nstripverts' is the number of vertices in the
**  corresponding tristrip, and 'stripverts' is the list of vertex _indices_
**  (for the original trimesh vertex data) for the triangle strip.  If either
**  'Nstripverts' or 'stripverts' is passed as null, then these values will
**  not be stored (this can be used to just check to see if the trimesh is
**  a grid).  In addition, if the trimesh is a grid, and if the 'griddim'
**  parameter is not null, the dimensions of the grid will be stored in that
**  array (most cases won't need this data).
*****************************************************************************/

DtFlag 
ddr_g4vll_drom_gridmesh_to_strip (
    DtInt Nverts,		/* Number of Vertices in Triangle Mesh */
    DtInt Ntriangles,		/* Number of triangles in Triangle Mesh */
    DtInt triangles[][3],	/* Triangle Vertex Indices */
    DtInt *Nstripverts,		/* Number of Vertices in Resulting TriStrip */
    DtInt **stripverts,		/* List of Vertex Indices in Result TriStrip */
    DtInt *griddim)		/* Returned Dimensions of Mesh Grid */
{
    register DtInt      ii;		/* Loop Index */
    auto     DtInt      temp1, temp2;	/* Temporary Indices */
    auto     DtInt     *tptr;		/* Pointer to List of Triangle Verts */
    auto     PatchType  patch_type;	/* Grid Patch Type */
    auto     DtInt      N, M;		/* Grid Dimensions */

    /* The 'tptr' alias provides cleaner access in some cases. */

    tptr = (DtInt*) triangles;

    /* This algorithm makes certain assumptions about the construction of
    ** a grid.  The first assumption is that the first patch of the grid
    ** is constructed of vertices 0, 1, N and N+1.  The grid is assumed to
    ** look like this:
    **
    **           N-1 -- 2N-1 -- 3N-1 -- 4N-1 - ... --- MN-1
    **            |       |       |       |              |
    **            |       |       |       |              |
    **           N-2 -- 2N-2 -- 3N-2 -- 4N-2 - ... --- MN-2
    **            |       |       |       |              |
    **            :       :       :       :              :
    **            |       |       |       |              |
    **            3 ---- N+3 -- 2N+3 -- 3N+3 - ... - (M-1)N+3
    **            |       |       |       |              |
    **            |       |       |       |              |
    **            2 ---- N+2 -- 2N+2 -- 3N+2 - ... - (M-1)N+2
    **            |       |       |       |              |
    **            |       |       |       |              |
    **            1 ---- N+1 -- 2N+1 -- 3N+1 - ... - (M-1)N+1
    **            |       |       |       |              |
    **            |       |       |       |              |
    **            0 ----- N ---- 2N ---- 3N -- ... -- (M-1)N
    **
    ** Furthermore, this procedure assumes that patches are specified in
    ** the following order:  {0, 1, N, N+1}, {1, 2, N+1, N+2}, ...
    ** {N-2, N-1, 2N-2, 2N-1}, {N, N+1, 2N, 2N+1}, {N+1, N+2, 2N+1, 2N+2},
    ** ... {2N, 2N+1, 3N, 3N+1} ... {(M-1)N - 2, (M-1)N - 1, MN-2, MN-1}.
    ** Using the layout of the above diagram, this is equivalent to starting
    ** at the lower left patch, and then running from bottom to top, and
    ** from left to right.
    **
    ** Note also that each patch in the grid         X+1 ----- X+N+1
    ** takes on the form of the patch to the          |          |
    ** left.  So if we're looking at the patch        |          |
    ** whose lower left corner is 713, and if         |          |
    ** N=212, then the vertices are 713, 713+1,       |          |
    ** 713+212, and 713+212+1, or 713, 714, 925       X ------- X+N
    ** and 926.
    */

    /* The first step of trying to match this triangle mesh to a grid is
    ** to ensure that the first patch has vertices 0,1,N,N+1.  This is done
    ** by examining the vertices of the first two triangles.  When done,
    ** temp1 should contain the value N, and temp2 should contain the value
    ** N+1.  */

    dprintf (("\ngridmesh:  Nverts = %d, Ntris = %d\n", Nverts, Ntriangles));

    dprintf (("gridmesh:  First six verts are %d,%d,%d,%d,%d,%d\n",
	tptr[0],tptr[1],tptr[2],tptr[3],tptr[4],tptr[5]));

    temp1 = temp2 = 0;

    for (ii=0;  ii < 6;  ++ii)		/* temp1 <-- N,  temp2 <-- N+1 */
    {   if (tptr[ii] <= 1)              /*              or             */
	    continue;			/* temp2 <-- N,  temp1 <-- N+1 */
	else if (!temp1)
	    temp1 = tptr[ii];
	else if (tptr[ii] == temp1)
	    continue;
	else if (!temp2)
	    temp2 = tptr[ii];
	else if (tptr[ii] != temp2)
	{   dprintf (("gridmesh:  Invalid first 6 vertices.\n"));
	    return DcFalse;
	}
    }

    /* Assign the variable N using temp1 or temp2. */

    if (temp2 == (temp1+1))
	N = temp1;
    else if (temp1 == (temp2+1))
	N = temp2;
    else
	return DcFalse;

    dprintf (("gridmesh:  First guess at N is %d.\n", N));

    /* Ensure that the number of vertices divides N cleanly. */

    if ((Nverts % N) != 0)
    {   dprintf (("gridmesh:  Nverts (%d) isn't divisible by N (%d).\n",
	    Nverts,N));
	return DcFalse;
    }

    M = Nverts / N;

    /* Ensure that the triangle_count is correct, given the dimension of
    ** the grid.  */

    if (Ntriangles != ((N-1) * (M-1) * 2))
    {   dprintf (("gridmesh:  Ntriangles (%d) != %d.\n",
	    Ntriangles, ((N-1)*(M-1)*2)));
	return DcFalse;
    }

    /* Characterize the connectivity of the grid by checking the first
    ** patch.  Every patch in the grid should be constructed the same as
    ** the first patch in the grid.  For example, if the first patch is
    ** constructed of triangles 0,N,1 and 1,N,N+1, then every patch in
    ** the grid should be constructed likewise, i.e.:
    **
    **          2 .       0 _____ 2       All diagonals should go in the
    **            |\        \    |        same direction.  In this case,
    **            | \        \ 2 |        they should all go from upper
    **            |  \        \  |        left to lower right.
    **            | 1 \        \ |
    **          0 |____\ 1      \| 1
    **                                                 C +-------+ D
    ** In addition, the connectivity should be           |       |  
    ** the same.  Each patch ABCD should (in             |       |
    ** the example given) consist of triangle            |       |
    ** ABC and then of triangle CBD.                   A +-------+ B
    **
    ** To get a "signature" of the first patch, need to determine which
    ** way the diagonal goes (which way the patch is split into two
    ** triangles), and which direction the normals go in (in or out).  */

    patch_type = CheckFirstPatch (triangles, N);

    if (patch_type == BAD_GRID)
    {   dprintf (("gridmesh:  First patch failed test.\n"));
	return DcFalse;
    }

    /* Ensure that all patches in the grid conform to the pattern of the
    ** first patch (normal vector orientation and diagonal direction).  */

    if (!CheckAllPatches (triangles, N, M))
    {   dprintf (("gridmesh:  Grid test failed for some patch.\n"));
	return DcFalse;
    }

    if (griddim)
    {   griddim[0] = N;
	griddim[1] = M;
    }

    if (Nstripverts && stripverts)
	StripGrid (N, M, patch_type, Nstripverts, stripverts);

    dprintf (("gridmesh:  Trimesh successfully gridded to %dx%d.\n", N, M));
    return DcTrue;
}



/*****************************************************************************
**  This routine takes the vertex indices of the two triangles of the first
**  patch and determines if they represent a valid triangulation of the
**  patch.  If the patch is valid, it will return the type fo triangulation
**  used, otherwise it will return the value BAD_PATCH.  This routine assumes
**  that the first patch has vertices 0, 1, N and N+1.
*****************************************************************************/

    /* The following table is used to characterize
    ** the two triangles that make up the first patch.      1 +------+ 3
    ** It is used to ensure that the orientations of          |      |
    ** the triangles are consistent, and that they            |      |
    ** properly cover the first patch.  The vertices        0 +------+ 2
    ** are based on the patch at the right.  */

static struct
{   DtInt v[3];		/* Vertices */
    DtInt value;	/* Triangle Signature Value */
} patch_signature_table[] =
{
    { {0,2,3}, -UP_OUT   },	/* up   diagonal, out normal, /|  */
    { {0,3,1},  UP_OUT   },	/* up   diagonal, out normal, |/  */
    { {0,3,2}, -UP_IN    },	/* up   diagonal, in  normal, /|  */
    { {0,1,3},  UP_IN    },	/* up   diagonal, in  normal, |/  */
    { {0,2,1}, -DOWN_OUT },	/* down diagonal, out normal, |\  */
    { {1,2,3},  DOWN_OUT },	/* down diagonal, out normal, \|  */
    { {0,1,2}, -DOWN_IN  },	/* down diagonal, in  normal, |\  */
    { {1,3,2},  DOWN_IN  },	/* down diagonal, in  normal, \|  */
};

static PatchType CheckFirstPatch (
    DtInt triangles[][3],	/* Triangle Vertex Indices */
    DtInt N)			/* Grid Height */
{
    register int   ii;		/* Scratch Integer Value */
    auto     int   v1, v2;	/* Triangle Signature Values */
    auto     DtInt A0, A1, A2;	/* First Triangle Indices */
    auto     DtInt B0, B1, B2;	/* Second Triangle Indices */

    /* Map the vertices from {0,1,N,N+1} to {0,1,2,3}.  This makes them
    ** easier to work with for characterization.  */

#   define VERT_CANONICAL(X) (((X) <= 1) ? (X) : (((X) == N) ? 2 : 3))

    A0 = VERT_CANONICAL (triangles[0][0]);
    A1 = VERT_CANONICAL (triangles[0][1]);
    A2 = VERT_CANONICAL (triangles[0][2]);
    B0 = VERT_CANONICAL (triangles[1][0]);
    B1 = VERT_CANONICAL (triangles[1][1]);
    B2 = VERT_CANONICAL (triangles[1][2]);

    /* To reduce the vertex indices to a common form, order them so that
    ** the clockwise or counter-clockwise orientation is preserved.  The
    ** common form is such that the first vertex index is the lowest, i.e.,
    ** V0 < V1 and V0 < V2.  */

    while ((A0 > A1) || (A0 > A2))
    {   auto DtInt temp;
	temp = A0;
	A0 = A1;
	A1 = A2;
	A2 = temp;
    }

    while ((B0 > B1) || (B0 > B2))
    {   auto DtInt temp;
	temp = B0;
	B0 = B1;
	B1 = B2;
	B2 = temp;
    }

    /* Get the signature of the first triangle. */

    for (ii=0;  ii < 8;  ++ii)
    {   if (  (A0 == patch_signature_table[ii].v[0])
           && (A1 == patch_signature_table[ii].v[1])
           && (A2 == patch_signature_table[ii].v[2])
	   )
	    break;
    }

    if (ii < 8)
    {   v1 = patch_signature_table[ii].value;
	dprintf (("gridmesh:  First triangle signature is %d.\n", v1));
    }
    else
    {   dprintf (("gridmesh:  Bad first triangle in first patch.\n"));
	return BAD_GRID;
    }

    /* Get the signature of the second triangle. */

    for (ii=0;  ii < 8;  ++ii)
    {   if (  (B0 == patch_signature_table[ii].v[0])
           && (B1 == patch_signature_table[ii].v[1])
           && (B2 == patch_signature_table[ii].v[2])
	   )
	    break;
    }

    if (ii < 8)
    {   v2 = patch_signature_table[ii].value;
	dprintf (("gridmesh:  Second triangle signature is %d.\n", v2));
    }
    else
    {   dprintf (("gridmesh:  Bad second triangle in first patch.\n"));
	return BAD_GRID;
    }

    /* Now ensure that the two triangles are proper pairs. */

    if (v1 + v2)
    {   dprintf (("gridmesh:  First triangles don't match.\n"));
	return BAD_GRID;
    }

    /* Return the patch type. */

    return (v1 < 0) ? -v1 : v1;
}



/*****************************************************************************
**  The following routine checks all patches in the grid mesh to ensure that
**  they are of the same type as the first patch (lower-left corner,
**  topologically speaking).
*****************************************************************************/

static DtFlag CheckAllPatches (
    DtInt triangles[][3],	/* Array of Triangles' Vertex Indices */
    DtInt N,
    DtInt M)			/* Grid Dimensions */
{
    register unsigned int row;		/* Grid Row Index */
    register unsigned int column;	/* Grid Column Index */
    register unsigned int kk;		/* Triangle Index */
    register unsigned int corner;	/* Vertex Index for Patch Corner */

    /* Check every patch in the grid to ensure that all other patches look
    ** the same as the first patch.  For the first pass, patches are assumed
    ** to run in the same way that the vertices were laid out.  */

    kk = 0;
    corner = 0;

    for (column=0;  column < (M-1);  ++column)
    {   for (row=0;  row < (N-1);  ++row)
	{   if (  (triangles[kk+0][0] != (corner + triangles[0][0]))
	       || (triangles[kk+0][1] != (corner + triangles[0][1]))
	       || (triangles[kk+0][2] != (corner + triangles[0][2]))
	       || (triangles[kk+1][0] != (corner + triangles[1][0]))
	       || (triangles[kk+1][1] != (corner + triangles[1][1]))
	       || (triangles[kk+1][2] != (corner + triangles[1][2]))
	       )
	    {
#		if DEBUG
		    printf ("gridmesh:  Bad patch found in pass 1.\n");
		    printf ("           Got (%d,%d,%d), expected (%d,%d,%d).\n",
			triangles[kk+0][0], triangles[kk+0][1],
			triangles[kk+0][2], corner+triangles[0][0],
			corner+triangles[0][1], corner+triangles[0][2]);
		    printf ("           Got (%d,%d,%d), expected (%d,%d,%d).\n",
			triangles[kk+1][0], triangles[kk+1][1],
			triangles[kk+1][2], corner+triangles[1][0],
			corner+triangles[1][1], corner+triangles[1][2]);
#               endif
		goto FAIL_PASS_1;	/* I don't care; it's efficient! */
	    }
	    kk += 2;
	    ++corner;
	}
	++corner;
    }

    return DcTrue;		/* Success in pass 1; valid grid. */

    FAIL_PASS_1:

    /* At this point, we know that the candidate grid doesn't have the patches
    ** running in the same direction as the vertices.  This second pass tries
    ** to determine if, for example, the vertices are ordered in columns and
    ** the patches are ordered in row by row.  If this check fails, then we
    ** don't have a grid.  */

    kk=0;	/* Triangle Index */

    for (row=0;  row < (N-1);  ++row)
    {   corner = row;
	for (column=0;  column < (M-1);  ++column)
	{   if (  (triangles[kk+0][0] != (corner + triangles[0][0]))
	       || (triangles[kk+0][1] != (corner + triangles[0][1]))
	       || (triangles[kk+0][2] != (corner + triangles[0][2]))
	       || (triangles[kk+1][0] != (corner + triangles[1][0]))
	       || (triangles[kk+1][1] != (corner + triangles[1][1]))
	       || (triangles[kk+1][2] != (corner + triangles[1][2]))
	       )
	    {
#		if DEBUG
		    printf ("gridmesh:  Bad patch found in pass 2.\n");
		    printf ("           Got (%d,%d,%d), expected (%d,%d,%d).\n",
			triangles[kk+0][0], triangles[kk+0][1],
			triangles[kk+0][2], corner+triangles[0][0],
			corner+triangles[0][1], corner+triangles[0][2]);
		    printf ("           Got (%d,%d,%d), expected (%d,%d,%d).\n",
			triangles[kk+1][0], triangles[kk+1][1],
			triangles[kk+1][2], corner+triangles[1][0],
			corner+triangles[1][1], corner+triangles[1][2]);
#               endif
		return DcFalse;
	    }
	    kk += 2;
	    corner += N;
	}
    }

    return DcTrue;
}



/*****************************************************************************
**  This procedure generates the vertex sequence for a triangle strip that
**  covers the given quadrilaterally-gridded surface.  The parameters 'N' and
**  'M' define the number of rows and columns of the grid, respectively.  The
**  vertices are assumed to be in the positions given in the comment for the
**  CheckGrid routine.  The 'patchtype' parameter determines which of four
**  patche types to use - the normal vector can go into our out of the
**  surface, and the diagonal can slope up or down going from left to right.
**  On successful calculation, the routine will load 'Nverts' with the number
**  of vertices in the triangle strip, and 'vertices' will contain the vertex
**  indices (NOT geometry) of the triangle strip.  If this routine does not
**  successfully return, 'Nverts' will be 0 and 'vertices' will be null. 
*****************************************************************************/

static void StripGrid (
    DtInt N,		/* Number of Rows in Grid */
    DtInt M,		/* Number of Columns in Grid */
    PatchType patchtype,/* Type Triangle Patches to Generate */
    DtInt *Nverts,	/* Number of Vertices in Triangle Strip */
    DtInt **vertices)	/* Triangle Strip Vertex List */
{
    register DtInt         column;	/* Current Grid Column */
    register unsigned int  ii;		/* Vertex List Index */
    auto     DiagDirection diagdir;	/* Slope Direction of Patch Diagonal */
    auto     DtInt         V1, V2;	/* Left & Right Vertex Indices */

    switch (patchtype)
    {   case   UP_IN :  diagdir = DIAG_UP;    break;
        case   UP_OUT:  diagdir = DIAG_UP;    break;
        case DOWN_IN :  diagdir = DIAG_DOWN;  break;
        case DOWN_OUT:  diagdir = DIAG_DOWN;  break;
	default:
	    *Nverts = 0;
	    *vertices = 0;
	    return;
    }

    /* Calculate the total number of vertices in the resultant triangle
    ** strip.  This is equal to the number of vertices for each column
    ** strip (2N(M-1)), plus the "filler" vertices to set up for the next
    ** column strip (2(M-2)).  This is equal to 2[(N+1)(M-1) - 1].  */

    *Nverts = 2 * ((N+1) * (M-1) - 1);

    /* If the patch diagonal orientation and normal vector direction are
    ** in conflict, then we specify the first vertex twice to reverse the
    ** "out" direction of the triangle strip.  */

    if ((patchtype == UP_OUT) || (patchtype == DOWN_IN))
	++(*Nverts);

    *vertices = dor_space_allocate (*Nverts * sizeof(DtInt));

    /* If the grid normal direction is opposite what is "natural" for the
    ** triangle strip, then reverse the normal sense by specifying the first
    ** vertex twice.  */

    ii = 0;

    if (patchtype == UP_OUT)
	(*vertices)[ii++] = N;
    else if (patchtype == DOWN_IN)
	(*vertices)[ii++] = 0;

    /* Generate the triangle strip by looping over each column. */

    if (diagdir == DIAG_UP)
	{ V1 = N;  V2 = 0; }
    else
	{ V1 = 0;  V2 = N; }

    for (column=0;  column < (M-1);  ++column)
    {
	register DtInt row;	/* Grid Vertex Row */

	if ((column % 2) == 0)
	{   for (row = 0;  row < N;  ++row)
	    {   (*vertices)[ii++] = V1++;
		(*vertices)[ii++] = V2++;
	    }
	    --V1; --V2;
	}
	else
	{   for (row = 0;  row < N;  ++row)
	    {   (*vertices)[ii++] = V1--;
		(*vertices)[ii++] = V2--;
	    }
	    ++V1; ++V2;
	}

	if (column == M-2) continue;

	if (  (((column % 2) == 0) && (diagdir == DIAG_UP))
	   || (((column % 2) == 1) && (diagdir == DIAG_DOWN))
	   )
	{   (*vertices)[ii++] = V1;
	    (*vertices)[ii++] = V1;
	    V2 = V1 + N;
	}
	else
	{   (*vertices)[ii++] = V2;
	    (*vertices)[ii++] = V2 + N;
	    V1 = V2 + N;
	}
    }
}
