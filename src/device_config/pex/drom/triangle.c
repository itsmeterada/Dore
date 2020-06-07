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
**  This file contains routines for both triangle lists and triangle meshes.
*****************************************************************************/

#include "../pex_driver.h"
#include <dore/internal/system.h>
#include <dore/dore_develop/private/trilst.h>
#include <dore/dore_develop/private/trimsh.h>
#include <dore/dore_develop/private/tristrip.h>
#include <dore/dore_develop/private/vartrimsh.h>
#include <dore/dore_develop/private/vartristrp.h>

#define DEBUG 0


    /***  Data Structures  ***/

typedef struct
{   PEXStructure  tri_struct;	/* PEX Triangle List Structure */
    DtFlag        tri_flag;	/* True:  Tri List Ready for Execution */
    PEXStructure  point_struct;	/* PEX Point List Structure */
    DtFlag        point_flag;	/* True:  Point List Ready for Execution */
    DtInt         seqnum;	/* Variable Primitive Sequence Number */
} TriData;


    /***  Function Declarations  ***/

static void  BuildTriList  ARGS((struct trilst *,   TriData *));
static void  BuildTriMesh  ARGS((TriData*, DtInt, DtRealTriple*, DtRealTriple*,
			         DtRealTriple*, DtInt, DtRealTriple*,
				 DtIntTriple*));
static void  BuildTriStrip ARGS((TriData*, DtInt, DtRealTriple*, DtRealTriple*,
				 DtRealTriple*, DtRealTriple*));



/*****************************************************************************
**  This routine allocates memory for triangle-list, triangle-mesh and
**  triangle-strip data structures on object creation.  This is also where
**  preliminary initialization takes place.
*****************************************************************************/

DtPtr ddr_pex_drom_triangles_create (DtObjectStructure *object)
{
    register TriData  *tridata;	/* Triangle List/Mesh/Strip Data */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_triangles_create (%lx)\n", object);
	fflush (stdout);
#   endif

    tridata = DDspace_Allocate (sizeof(*tridata));
    tridata->tri_flag   = DcFalse;
    tridata->point_flag = DcFalse;
    return tridata;
}



/*****************************************************************************
**  The triangles_create routine allocates memory for the triangle data
**  structure on object creation.  This is also where preliminary
**  initialization takes place.
*****************************************************************************/

void ddr_pex_drom_triangles_delete (
    DtObjectStructure *object,
    DtPtr              tridata_parm)
{
    auto TriData *tridata = tridata_parm;

#   if DEBUG_CALL
	printf ("ddr_pex_drom_triangles_delete (%lx, %lx)\n", object, tridata);
	fflush (stdout);
#   endif

    if (!tridata) return;

    if (tridata->tri_flag)
        PEXDestroyStructures (DCM->display, 1, &(tridata->tri_struct));

    if (tridata->point_flag)
        PEXDestroyStructures (DCM->display, 1, &(tridata->point_struct));

    DDspace_Deallocate (tridata);
}



/*****************************************************************************
**  The triangles_print routine prints the contents of the triangle
**  list/mesh/strip data structure to the standard output stream.
*****************************************************************************/

void ddr_pex_drom_triangles_print (
    DtObjectStructure *object,		/* Triangle Object */
    DtPtr              tridata)		/* Additional Triangle Data */
{
    /* No-op */
}



/*****************************************************************************
**  This routine is called to render a particular triangle list.  If this
**  routine is called for the first time, then we also need to bundle up
**  and initialize everything for PEX.
*****************************************************************************/

void ddr_pex_drom_trilist_render (dot_object *object)
{
    static   DtInt    classid = -1;	/* Triangle List Class ID */
    static   DtInt    dataindex = -1;	/* Trilist Add'l Data Field */
    auto     TriData *tridata;		/* Additional Triangle Data */
    auto     DtInt    tri_count;	/* Number of Triangles in List */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_trilist_render (%lx)\n", object);
	fflush (stdout);
#   endif

    /* Get the class ID and additional-data index. */

    if (!GetClassInfo (&classid, "DoTriangleList", &dataindex))
	return;

    /* Get the additional_data field of the triangle list. */

    tridata = (TriData*) object->additional_data [dataindex];

    /* If the triangle list is not yet ready for PEX rendering, then we need
    ** to build the PEX FillArea structure for execution. */

    if (  ((DROM.rep_type == DcPoints) && (!tridata->point_flag))
       || ((DROM.rep_type != DcPoints) && (!tridata->tri_flag))
       )
    {
	BuildTriList ((struct trilst *)(object->data), tridata);
    }

    /* Now execute the PEX structure that contains the triangle list. */

    tri_count = ((struct trilst *)(object->data)) -> triangle_count;

    if (DROM.rep_type != DcPoints)
    {   PEXExecuteStructure
	    (DCM->display, PEX.renderer, PEXOCRender, tridata->tri_struct);
	doe_system_timing_triangles_drawn += tri_count;
    }
    else
    {   PEXSetMarkerColor
	(   DCM->display, PEX.renderer, PEXOCRender, 
	    PEXColorTypeRGB, &PEX.pc_attrs.surface_color.value
	);
	PEXExecuteStructure
	    (DCM->display, PEX.renderer, PEXOCRender, tridata->point_struct);
	doe_system_timing_points_drawn += 3 * tri_count;
    }
}



/*****************************************************************************
**  This routine is called to render a particular triangle mesh.  If this
**  routine is called for the first time, then we also need to bundle up
**  and initialize everything for PEX.
*****************************************************************************/

void ddr_pex_drom_trimesh_render (dot_object *object)
{
    static   DtInt          classid = -1;	/* Triangle Mesh Class ID */
    static   DtInt          dataindex = -1;	/* Tri Mesh Add'l Data Index */
    auto     TriData       *tridata;		/* Additional Triangle Data */
    auto     struct trimsh *tmesh =		/* Triangle Mesh Parms */
			    (struct trimsh *)(object->data);

#   if DEBUG_CALL
	printf ("ddr_pex_drom_trimesh_render (%lx)\n", object);
	fflush (stdout);
#   endif

    /* Get the class ID and additional-data index. */

    if (!GetClassInfo (&classid, "DoTriangleMesh", &dataindex))
	return;

    /* Get the additional_data field of the triangle mesh. */

    tridata = (TriData*) object->additional_data [dataindex];

    /* If the triangle mesh is not yet ready for PEX rendering, then we need
    ** to build the PEX FillArea structure for execution. */

    if (  ((DROM.rep_type == DcPoints) && (!tridata->point_flag))
       || ((DROM.rep_type != DcPoints) && (!tridata->tri_flag))
       )
    {
	BuildTriMesh
	(   tridata, tmesh->vertex_count, tmesh->vertex_location,
	    (tmesh->vertextype & DcLocNrm) ? tmesh->vertex_normal : 0,
	    (tmesh->vertextype & DcLocClr) ? tmesh->vertex_color  : 0,
	    tmesh->triangle_count, tmesh->element_normal,
	    tmesh->triangle_vertexlist
	);
    }

    /* Execute the PEX structure that contains the triangle mesh. */

    if (DROM.rep_type != DcPoints)
    {   PEXExecuteStructure
	    (DCM->display, PEX.renderer, PEXOCRender, tridata->tri_struct);
	doe_system_timing_mesh_triangles_drawn += tmesh->triangle_count;
    }
    else
    {   PEXSetMarkerColor
	(   DCM->display, PEX.renderer, PEXOCRender, 
	    PEXColorTypeRGB, &PEX.pc_attrs.surface_color.value
	);
	PEXExecuteStructure
	    (DCM->display, PEX.renderer, PEXOCRender, tridata->point_struct);
	doe_system_timing_points_drawn += 3 * tmesh->triangle_count;
    }
}



/*****************************************************************************
**  This routine is called to render a variable triangle mesh.  If the object
**  has been updated, then the current object (if one exists) is first
**  destroyed.  If the object is not currently ready for rendering (if it's
**  new or updated), then a new triangle mesh PEX structure is created.
**  Finally, the PEX structure for the triangle mesh is executed.
*****************************************************************************/

void ddr_pex_drom_vartrimesh_render (dot_object *object)
{
    static   DtInt             classid = -1;	/* Triangle Mesh Class ID */
    static   DtInt             dataindex = -1;	/* Tri Mesh Add'l Data Index */
    auto     TriData          *tridata;		/* Additional Triangle Data */
    auto     struct vartrimsh *vtmesh =		/* Var Triangle Mesh Parms */
			    (struct vartrimsh *)(object->data);

#   if DEBUG_CALL
	printf ("ddr_pex_drom_vartrimesh_render (%lx)\n", object);
	fflush (stdout);
#   endif

    /* Get the class ID and additional-data index. */

    if (!GetClassInfo (&classid, "DoVarTriangleMesh", &dataindex))
	return;

    /* Get the additional_data field of the triangle mesh. */

    tridata = (TriData*) object->additional_data [dataindex];

    /* If the triangle mesh has been updated, then destroy the current
    ** and set the ready flags to false.  */

    if (vtmesh->sequence_number != tridata->seqnum)
    {
	if (tridata->tri_flag)
	{   PEXDestroyStructures (DCM->display, 1, &(tridata->tri_struct));
	    tridata->tri_flag = DcFalse;
	}

	if (tridata->point_flag)
	{   PEXDestroyStructures (DCM->display, 1, &(tridata->point_struct));
	    tridata->point_flag = DcFalse;
	}
	tridata->seqnum = vtmesh->sequence_number;
    }

    /* If the triangle mesh is not yet ready for PEX rendering, then we need
    ** to build the PEX FillArea structure for execution. */

    if (  ((DROM.rep_type == DcPoints) && (!tridata->point_flag))
       || ((DROM.rep_type != DcPoints) && (!tridata->tri_flag))
       )
    {
	BuildTriMesh
	(   tridata, vtmesh->vertex_count, vtmesh->vertex_location,
	    vtmesh->vertex_normal, vtmesh->vertex_color,
	    vtmesh->triangle_count, vtmesh->element_normal,
	    vtmesh->triangle_vertexlist
	);
    }

    /* Execute the PEX structure that contains the triangle mesh. */

    if (DROM.rep_type != DcPoints)
    {   PEXExecuteStructure
	    (DCM->display, PEX.renderer, PEXOCRender, tridata->tri_struct);
	doe_system_timing_mesh_triangles_drawn += vtmesh->triangle_count;
    }
    else
    {   PEXSetMarkerColor
	(   DCM->display, PEX.renderer, PEXOCRender, 
	    PEXColorTypeRGB, &PEX.pc_attrs.surface_color.value
	);
	PEXExecuteStructure
	    (DCM->display, PEX.renderer, PEXOCRender, tridata->point_struct);
	doe_system_timing_points_drawn += 3 * vtmesh->triangle_count;
    }
}



/*****************************************************************************
**  This routine renders a triangle strip.  If the object is being rendered
**  for the first time, then it also creates the associated PEX structure.
*****************************************************************************/

void ddr_pex_drom_tristrip_render (dot_object *object)
{
    static   DtInt            classid = -1;	/* Triangle Strip Class ID */
    static   DtInt            dataindex = -1;	/* Tristrip Add'l Data Field */
    auto     TriData         *tridata;		/* Additional Triangle Data */
    auto     struct tristrip *tstrip =		/* Triangle Strip Parms */
				    (struct tristrip *)(object->data);

#   if DEBUG_CALL
	printf ("ddr_pex_drom_tristrip_render (%lx)\n", object);
	fflush (stdout);
#   endif

    /* Get the class ID and additional-data index. */

    if (!GetClassInfo (&classid, "DoTriangleStrip", &dataindex))
	return;

    /* Get the additional_data field of the triangle strip. */

    tridata = (TriData*) object->additional_data [dataindex];

    /* If the triangle strip is not yet ready for PEX rendering, then we need
    ** to build the PEX structure for execution. */

    if (  ((DROM.rep_type == DcPoints) && (!tridata->point_flag))
       || ((DROM.rep_type != DcPoints) && (!tridata->tri_flag))
       )
    {
	BuildTriStrip
	(   tridata, tstrip->vertex_count, tstrip->vertex_location,
	    tstrip->vertex_normal,
	    (tstrip->vertextype & DcLocClr) ? tstrip->vertex_color : 0,
	    (tstrip->vertextype & DcLocNrm) ? tstrip->element_normal : 0
	);
    }

    /* Now execute the PEX structure that contains the triangle strip. */

    if (DROM.rep_type != DcPoints)
    {   PEXExecuteStructure
	    (DCM->display, PEX.renderer, PEXOCRender, tridata->tri_struct);
	doe_system_timing_strip_triangles_drawn += tstrip->vertex_count - 2;
    }
    else
    {   PEXSetMarkerColor
	(   DCM->display, PEX.renderer, PEXOCRender, 
	    PEXColorTypeRGB, &PEX.pc_attrs.surface_color.value
	);
	PEXExecuteStructure
	    (DCM->display, PEX.renderer, PEXOCRender, tridata->point_struct);
	doe_system_timing_points_drawn += tstrip->vertex_count;
    }
}



/*****************************************************************************
**  This routine is called to render a variable triangle strip.  If the object
**  has been updated, then the existing PEX structure is destroyed.  A new
**  object is created if it hasn't yet been created.  Finally, the object
**  is sent down the PEX pipeline and rendered.
*****************************************************************************/

void ddr_pex_drom_vartristrip_render (dot_object *object)
{
    static   DtInt               classid = -1;	/* Triangle Strip Class ID */
    static   DtInt               dataindex = -1;/* Tristrip Add'l Data Field */
    auto     TriData            *tridata;	/* Additional Triangle Data */
    auto     struct vartristrip *vtstrip =	/* Triangle Strip Parms */
				    (struct vartristrip *)(object->data);

#   if DEBUG_CALL
	printf ("ddr_pex_drom_vartristrip_render (%lx)\n", object);
	fflush (stdout);
#   endif

    /* Get the class ID and additional-data index. */

    if (!GetClassInfo (&classid, "DoVarTriangleStrip", &dataindex))
	return;

    /* Get the additional_data field of the triangle strip. */

    tridata = (TriData*) object->additional_data [dataindex];

    /* If the variable triangle strip has been updated, then destroy the
    ** existing triangle strip PEX structure (if one exists) and set the
    ** ready flags to false.  */

    if (vtstrip->sequence_number != tridata->seqnum)
    {
	if (tridata->tri_flag)
	{   PEXDestroyStructures (DCM->display, 1, &(tridata->tri_struct));
	    tridata->tri_flag = DcFalse;
	}

	if (tridata->point_flag)
	{   PEXDestroyStructures (DCM->display, 1, &(tridata->point_struct));
	    tridata->point_flag = DcFalse;
	}
	tridata->seqnum = vtstrip->sequence_number;
    }

    /* If the triangle strip is not yet ready for PEX rendering, then we need
    ** to build the PEX structure for execution. */

    if (  ((DROM.rep_type == DcPoints) && (!tridata->point_flag))
       || ((DROM.rep_type != DcPoints) && (!tridata->tri_flag))
       )
    {
	BuildTriStrip
	(   tridata, vtstrip->vertex_count, vtstrip->vertex_location,
	    vtstrip->vertex_normal, vtstrip->vertex_color,
	    vtstrip->element_normal
	);
    }

    /* Now execute the PEX structure that contains the triangle strip. */

    if (DROM.rep_type != DcPoints)
    {   PEXExecuteStructure
	    (DCM->display, PEX.renderer, PEXOCRender, tridata->tri_struct);
	doe_system_timing_strip_triangles_drawn += vtstrip->vertex_count - 2;
    }
    else
    {   PEXSetMarkerColor
	(   DCM->display, PEX.renderer, PEXOCRender, 
	    PEXColorTypeRGB, &PEX.pc_attrs.surface_color.value
	);
	PEXExecuteStructure
	    (DCM->display, PEX.renderer, PEXOCRender, tridata->point_struct);
	doe_system_timing_points_drawn += vtstrip->vertex_count;
    }
}



/*****************************************************************************
**  This routine builds a PEX Fill Area structure from the Dore' triangle-list
**  data.  On return, it sets the tri_struct field of the tridata structure
**  for future PEX execution and sets the ready flag to true.
*****************************************************************************/

static void  BuildTriList  (
    struct trilst  *trilist,	/* Dore' Triangle List Data */
    TriData        *tridata)	/* Additional Triangle Data */
{
    auto     unsigned int         facet_attrs;	/* Triangle Attribute Flags */
    auto     PEXArrayOfFacetData  facet_data;	/* Triangle Normal Pointer */
    auto     PEXVector            facet_normal;	/* Triangle Normal */
    register int                  ii;		/* Loop Index */
    auto     unsigned int         vertex_attrs;	/* Vertex Attribute Flags */
    auto     PEXArrayOfVertex     vertex_data;	/* Triangle Vertex Data */

#   if DEBUG_CALL
	printf ("BuildTriList (%lx, %lx)\n", trilist, tridata);
	fflush (stdout);
#   endif

#   if DEBUG
	print ("! Trilist Info:\n");
	printf("!     %d triangles\n", trilist->triangle_count);
	printf("!     vertex type %d\n", trilist->vertex_type);
	printf("!     vloc %lx, vnorm %lx, vcolor %lx, elnorm %lx\n",
	    trilist->vertex_location, trilist->vertex_normal,
	    trilist->vertex_color, trilist->element_normal);
	printf("!     edge_enable %lx\n", trilist->edge_enable);
#   endif

    /* Create the PEX structure.  If the representation type is DcPoints,
    ** then create a PEX marker list structure with each vertex location and
    ** return.  */

    if (DROM.rep_type == DcPoints)
    {   tridata->point_struct = BuildPointList (trilist->triangle_count * 3,
						trilist->vertex_location);
	tridata->point_flag = DcTrue;
	return;
    }

    tridata->tri_struct = PEXCreateStructure (DCM->display);

    facet_data.normal = &facet_normal;
    facet_attrs = (trilist->element_normal) ? PEXGANormal : PEXGANone;

#   define COPY_LOC(a,b)    (a).x  =(b)[0]; (a).y    =(b)[1]; (a).z   =(b)[2]
#   define COPY_NORM(a,b)   (a).x  =(b)[0]; (a).y    =(b)[1]; (a).z   =(b)[2]
#   define COPY_COLOR(a,b)  (a).red=(b)[0]; (a).green=(b)[1]; (a).blue=(b)[2]

    switch (trilist->vertextype)
    {
	case DcLoc:
	{   auto     PEXCoord      vdata[3];
	    register DtRealTriple *vloc;

	    vertex_attrs = PEXGANone;
	    vertex_data.no_data = vdata;

	    for (ii=0;  ii < trilist->triangle_count;  ++ii)
	    {
		vloc = trilist->vertex_location + (3*ii);

		COPY_LOC (vdata[0], vloc[0]);
		COPY_LOC (vdata[1], vloc[1]);
		COPY_LOC (vdata[2], vloc[2]);

		if (facet_attrs == PEXGANormal)
		{   facet_normal.x = trilist->element_normal[ii][0];
		    facet_normal.y = trilist->element_normal[ii][1];
		    facet_normal.z = trilist->element_normal[ii][2];
		}

		PEXTriangleStrip
		(   DCM->display, tridata->tri_struct, PEXOCStore,
		    facet_attrs, vertex_attrs, PEXColorTypeRGB,
		    facet_data, (unsigned int)(3), vertex_data
		);
	    }
	    break;
	}

	case DcLocNrm:
	{   auto     PEXVertexNormal  vdata[3];
	    register DtRealTriple    *vloc;
	    register DtRealTriple    *vnorm;

	    vertex_attrs = PEXGANormal;
	    vertex_data.normal = vdata;

	    for (ii=0;  ii < trilist->triangle_count;  ++ii)
	    {
		vloc  = trilist->vertex_location + (3*ii);
		vnorm = trilist->vertex_normal   + (3*ii);

		COPY_LOC  (vdata[0].point,  vloc[0]);
		COPY_NORM (vdata[0].normal, vnorm[0]);

		COPY_LOC  (vdata[1].point,  vloc[1]);
		COPY_NORM (vdata[1].normal, vnorm[1]);

		COPY_LOC  (vdata[2].point,  vloc[2]);
		COPY_NORM (vdata[2].normal, vnorm[2]);

		if (facet_attrs == PEXGANormal)
		{   facet_normal.x = trilist->element_normal[ii][0];
		    facet_normal.y = trilist->element_normal[ii][1];
		    facet_normal.z = trilist->element_normal[ii][2];
		}

		PEXTriangleStrip
		(   DCM->display, tridata->tri_struct, PEXOCStore,
		    facet_attrs, vertex_attrs, PEXColorTypeRGB,
		    facet_data, (unsigned int)(3), vertex_data
		);
	    }
	    break;
	}

	case DcLocClr:
	{   auto     PEXVertexRGB  vdata[3];
	    register DtRealTriple *vloc;
	    register DtColorRGB   *vcolor;

	    vertex_attrs = PEXGAColor;
	    vertex_data.rgb = vdata;

	    for (ii=0;  ii < trilist->triangle_count;  ++ii)
	    {
		vloc   = trilist->vertex_location + (3*ii);
		vcolor = trilist->vertex_color    + (3*ii);

		COPY_LOC   (vdata[0].point, vloc[0]);
		COPY_COLOR (vdata[0].rgb,   vcolor[0]);

		COPY_LOC   (vdata[1].point, vloc[1]);
		COPY_COLOR (vdata[1].rgb,   vcolor[1]);

		COPY_LOC   (vdata[2].point, vloc[2]);
		COPY_COLOR (vdata[2].rgb,   vcolor[2]);

		if (facet_attrs == PEXGANormal)
		{   facet_normal.x = trilist->element_normal[ii][0];
		    facet_normal.y = trilist->element_normal[ii][1];
		    facet_normal.z = trilist->element_normal[ii][2];
		}

		PEXTriangleStrip
		(   DCM->display, tridata->tri_struct, PEXOCStore,
		    facet_attrs, vertex_attrs, PEXColorTypeRGB,
		    facet_data, (unsigned int)(3), vertex_data
		);
	    }
	    break;
	}

	case DcLocNrmClr:
	{   auto     PEXVertexRGBNormal  vdata[3];
	    register DtRealTriple       *vloc;
	    register DtRealTriple       *vnorm;
	    register DtColorRGB         *vcolor;

	    vertex_attrs = PEXGANormal | PEXGAColor;
	    vertex_data.rgb_normal = vdata;

	    for (ii=0;  ii < trilist->triangle_count;  ++ii)
	    {
		vloc   = trilist->vertex_location + (3*ii);
		vnorm  = trilist->vertex_normal   + (3*ii);
		vcolor = trilist->vertex_color    + (3*ii);

		COPY_LOC   (vdata[0].point,  vloc[0]);
		COPY_NORM  (vdata[0].normal, vnorm[0]);
		COPY_COLOR (vdata[0].rgb,    vcolor[0]);

		COPY_LOC   (vdata[1].point,  vloc[1]);
		COPY_NORM  (vdata[1].normal, vnorm[1]);
		COPY_COLOR (vdata[1].rgb,    vcolor[1]);

		COPY_LOC   (vdata[2].point,  vloc[2]);
		COPY_NORM  (vdata[2].normal, vnorm[2]);
		COPY_COLOR (vdata[2].rgb,    vcolor[2]);

		if (facet_attrs == PEXGANormal)
		{   facet_normal.x = trilist->element_normal[ii][0];
		    facet_normal.y = trilist->element_normal[ii][1];
		    facet_normal.z = trilist->element_normal[ii][2];
		}

		PEXTriangleStrip
		(   DCM->display, tridata->tri_struct, PEXOCStore,
		    facet_attrs, vertex_attrs, PEXColorTypeRGB,
		    facet_data, (unsigned int)(3), vertex_data
		);
	    }
	    break;
	}
    }

    /* Set the triangle structure flag to indicate that we now have a PEX
    ** structure for the triangle list. */

    tridata->tri_flag = DcTrue;
}



/*****************************************************************************
**  This routine builds a PEX Fill Area structure from the Dore' triangle-mesh
**  data.  On return, it sets the tri_struct or point_struct field (whichever
**  is appropriate) of the tridata structure for future PEX execution and sets
**  the ready flag to true.  If any of vtx_normal, vtx_color, or tri_normal
**  are null, then that data will be omitted from the final triangle mesh.
*****************************************************************************/

static void BuildTriMesh (
    TriData      *tridata,	/* Additional Triangle Data */
    DtInt         vtx_count,	/* Number of Vertices in Mesh */
    DtRealTriple *vtx_location,	/* Vertex XYZ Locations */
    DtRealTriple *vtx_normal,	/* Vertex Normal Vectors */
    DtRealTriple *vtx_color,	/* Vertex RGB Colors */
    DtInt         tri_count,	/* Number of Triangles in Mesh */
    DtRealTriple *tri_normal,	/* Triangle Normal Vectors */
    DtIntTriple  *tri_vtxlist)	/* Triangle Vertex Index List */
{
    auto     PEXConnectivityData *connect_data;	/* PEX Connectivity Array */
    auto     PEXListOfUShort     *contour_data;	/* PEX Contour Array */
    auto     unsigned int         facet_attrs;	/* Triangle Attribute Flags */
    auto     PEXArrayOfFacetData  facet_data;	/* Triangle Facet Data */
    register int                  ii;		/* Loop Index */
    auto     unsigned int         ntriverts;	/* Number Triangle Vertices */
    auto     unsigned short      *index_data;	/* PEX Fill Area Index Array */
    auto     void                *vertex_data;	/* Vertex Data */
    auto     PEXArrayOfVertex     vertices;	/* PEX Vertex Data Union */
    auto     unsigned int         vertex_attrs;	/* Vertex Attribute Flags */

#   if DEBUG_CALL
	printf ("BuildTriMesh (%lx, %d, %lx, %lx, %lx, %d, %lx, %lx)\n",
	    tridata, vtx_count, vtx_location, vtx_normal, vtx_color,
	    tri_count, tri_normal, tri_vtxlist);
	fflush (stdout);
#   endif

    /* Sanity Checks */

    if ((vtx_count < 3) || (tri_count < 1) || !vtx_location || !tri_vtxlist)
	return;

    ntriverts = 3 * tri_count;	/* Number of Triangle Vertices */

    /* Create the PEX structure.  If the representation type is DcPoints,
    ** then create a PEX marker list structure with each vertex location and
    ** return.  */

    if (DROM.rep_type == DcPoints)
    {
	auto DtRealTriple *point_locations;

	/* The optimal way to do this is to specify every used vertex exactly
	** once.  However, the mesh may contain vertices that aren't actually
	** used by any of the triangles in the mesh.  The brute force solution
	** is to add to the list all vertices used for each triangle.  However,
	** this has the disadvantage that a vertex shared by N triangles will
	** be rendered N times.  As a compromise, all vertices are initialized
	** to the coordinates of the first vertex of the first triangle.  Then
	** the list of triangles is scanned, and the data for each vertex
	** actually used is loaded into the vertex list.  Every spurious vertex
	** in the mesh vertex list will then contain the data for the first
	** vertex used, so no extraneous vertices will be drawn.  Also, in the
	** case where there are no spurious vertices, each vertex will be drawn
	** exactly once.  If there are N spurious vertices, then the first used
	** vertex will be drawn N+1 times.  */

	/* Allocate the vertex array. */

	point_locations = DDspace_Allocate (vtx_count * sizeof(DtRealTriple));

	/* Initialize the vertices to the first vertex used. */

	point_locations[0][0] = vtx_location [tri_vtxlist[0][0]] [0];
	point_locations[0][1] = vtx_location [tri_vtxlist[0][0]] [1];
	point_locations[0][2] = vtx_location [tri_vtxlist[0][0]] [2];

	for (ii=1;  ii < vtx_count;  ++ii)
	{   point_locations[ii][0] = point_locations[0][0];
	    point_locations[ii][1] = point_locations[0][1];
	    point_locations[ii][2] = point_locations[0][2];
	}

	/* Scan through each of the triangles in the mesh and load the
	** coordinates for each vertex used.  */

	for (ii=0;  ii < tri_count;  ++ii)
	{   auto DtInt vindex;	/* Vertex Index */

	    vindex = tri_vtxlist[ii][0];
	    point_locations [vindex][0] = vtx_location [vindex][0];
	    point_locations [vindex][1] = vtx_location [vindex][1];
	    point_locations [vindex][2] = vtx_location [vindex][2];

	    vindex = tri_vtxlist[ii][1];
	    point_locations [vindex][0] = vtx_location [vindex][0];
	    point_locations [vindex][1] = vtx_location [vindex][1];
	    point_locations [vindex][2] = vtx_location [vindex][2];

	    vindex = tri_vtxlist[ii][2];
	    point_locations [vindex][0] = vtx_location [vindex][0];
	    point_locations [vindex][1] = vtx_location [vindex][1];
	    point_locations [vindex][2] = vtx_location [vindex][2];
	}

	tridata->point_struct = BuildPointList (vtx_count, point_locations);
	tridata->point_flag = DcTrue;

	DDspace_Deallocate (point_locations);
	return;
    }

    tridata->tri_struct = PEXCreateStructure (DCM->display);

    /* Set up the facet normals, if any were given. */

    if (!tri_normal)
    {   facet_attrs = PEXGANone;
	facet_data.normal = 0;
    }
    else
    {   register PEXVector *normal_data;

	/* Facet normals are specified as an array of PEXVector's. */

	facet_attrs = PEXGANormal;
	normal_data = DDspace_Allocate (tri_count * sizeof(*normal_data));
	facet_data.normal = normal_data;

	for (ii=0;  ii < tri_count;  ++ii)
	{   normal_data->x = tri_normal[ii][0];
	    normal_data->y = tri_normal[ii][1];
	    normal_data->z = tri_normal[ii][2];
	    ++normal_data;
	}
    }

    /* For now, the two optional vertex data fields are normals and colors. */

    vertex_attrs = (vtx_normal ? PEXGANormal : 0)
		 | (vtx_color  ? PEXGAColor  : 0);

    switch (vertex_attrs)
    {
        case PEXGANone:
        {   register PEXCoord     *pex_vdata;
	    register DtRealTriple *dore_vloc;

            vertex_data = DDspace_Allocate (ntriverts * sizeof(PEXCoord));

	    pex_vdata = vertices.no_data = (PEXCoord*) vertex_data;
	    dore_vloc = vtx_location;

	    for (ii=0;  ii < vtx_count;  ++ii)
	    {   pex_vdata->x = (*dore_vloc)[0];
		pex_vdata->y = (*dore_vloc)[1];
		pex_vdata->z = (*dore_vloc)[2];

		++pex_vdata;
		++dore_vloc;
	    }
            break;
	}

        case PEXGANormal:
        {   register PEXVertexNormal *pex_vdata;
	    register DtRealTriple    *dore_vloc;
	    register DtRealTriple    *dore_vnorm;

            vertex_data = DDspace_Allocate (ntriverts*sizeof(PEXVertexNormal));

	    pex_vdata  = vertices.normal = (PEXVertexNormal*) vertex_data;
	    dore_vloc  = vtx_location;
	    dore_vnorm = vtx_normal;

	    for (ii=0;  ii < vtx_count;  ++ii)
	    {   pex_vdata->point.x  = (*dore_vloc)[0];
		pex_vdata->point.y  = (*dore_vloc)[1];
		pex_vdata->point.z  = (*dore_vloc)[2];
		pex_vdata->normal.x = (*dore_vnorm)[0];
		pex_vdata->normal.y = (*dore_vnorm)[1];
		pex_vdata->normal.z = (*dore_vnorm)[2];

		++pex_vdata;
		++dore_vloc;
		++dore_vnorm;
	    }
            break;
	}

        case PEXGAColor:
        {   register PEXVertexRGB *pex_vdata;
	    register DtRealTriple *dore_vloc;
	    register DtRealTriple *dore_vcolor;

            vertex_data = DDspace_Allocate (ntriverts * sizeof(PEXVertexRGB));

	    pex_vdata   = vertices.rgb = (PEXVertexRGB*) vertex_data;
	    dore_vloc   = vtx_location;
	    dore_vcolor = vtx_color;

	    for (ii=0;  ii < vtx_count;  ++ii)
	    {   pex_vdata->point.x   = (*dore_vloc)[0];
		pex_vdata->point.y   = (*dore_vloc)[1];
		pex_vdata->point.z   = (*dore_vloc)[2];
		pex_vdata->rgb.red   = (*dore_vcolor)[0];
		pex_vdata->rgb.green = (*dore_vcolor)[1];
		pex_vdata->rgb.blue  = (*dore_vcolor)[2];

		++pex_vdata;
		++dore_vloc;
		++dore_vcolor;
	    }
            break;
	}

        case (PEXGANormal | PEXGAColor):
        {   register PEXVertexRGBNormal *pex_vdata;
	    register DtRealTriple       *dore_vloc;
	    register DtRealTriple       *dore_vnorm;
	    register DtRealTriple       *dore_vcolor;

            vertex_data = DDspace_Allocate
			  (ntriverts * sizeof(PEXVertexRGBNormal));

	    pex_vdata = vertices.rgb_normal = (PEXVertexRGBNormal*) vertex_data;
	    dore_vloc   = vtx_location;
	    dore_vnorm  = vtx_normal;
	    dore_vcolor = vtx_color;

	    for (ii=0;  ii < vtx_count;  ++ii)
	    {   pex_vdata->point.x   = (*dore_vloc)[0];
		pex_vdata->point.y   = (*dore_vloc)[1];
		pex_vdata->point.z   = (*dore_vloc)[2];
		pex_vdata->rgb.red   = (*dore_vcolor)[0];
		pex_vdata->rgb.green = (*dore_vcolor)[1];
		pex_vdata->rgb.blue  = (*dore_vcolor)[2];
		pex_vdata->normal.x  = (*dore_vnorm)[0];
		pex_vdata->normal.y  = (*dore_vnorm)[1];
		pex_vdata->normal.z  = (*dore_vnorm)[2];

		++pex_vdata;
		++dore_vloc;
		++dore_vnorm;
		++dore_vcolor;
	    }
            break;
	}
    }

    /* Create the trimesh connectivity for the PEXSetOfFillAreaSets primitive.
    ** This is how the data looks for the triangle mesh:
    **
    **                (PEXCoord*  or  PEXVertex???*)
    ** Vertex         +--+--+--+--+--+--+--+--+--+--+--+--
    ** Data           |V |V |V |V |V |V |V |V |V |V |V | . . .
    **                | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|
    **                +--+--+--+--+--+--+--+--+--+--+--+--
    **
    **                (unsigned short)
    ** Triangle       +---+---+---|---+---+---|---+---+---|---+---+---|--
    ** Indices        | 0 : 1 : 2 | 1 : 2 : 4 | 6 : 7 : 9 | 12: 9 : 10| . . .
    **                +---+---+---|---+---+---|---+---+---|---+---+---|--
    **                   \_____      \_____      \_____      \_____
    **                         \           \           \           \
    ** Contour        +---------|-+---------|-+---------|-+---------|-+--
    ** Data           | shorts: ^ | shorts: ^ | shorts: ^ | shorts: ^ | . . .
    ** (PEXList       | count : 3 | count : 3 | count : 3 | count : 3 |
    **  OfUShort)     +-----------+-----------+-----------+-----------+--
    **                         \           \           \           \   
    ** Connectivity   +---------|-+---------|-+---------|-+---------|-+--
    ** Data           | lists : ^ | lists : ^ | lists : ^ | lists : ^ | . . .
    **                | count : 1 | count : 1 | count : 1 | count : 1 |
    **                +-----------+-----------+-----------+-----------+--
    **                (PEXConnectivityData)
    */

    /* Set up the index array. */

    {   register DtInt          *dore_index;	/* Dore' Index Pointer */
	register unsigned short *pex_index;	/* PEX Index Pointer */

	index_data = DDspace_Allocate (ntriverts * sizeof(*index_data));
	pex_index  = index_data;
	dore_index = (DtInt*) tri_vtxlist;

	for (ii=0;  ii < ntriverts;  ++ii)
	{   *pex_index++ = (unsigned short) *dore_index++;
	}
    }

    /* Set up the contour & connectivity arrays. */

    {   register PEXListOfUShort     *contour_ptr;
	register PEXConnectivityData *connect_ptr;

	contour_data = DDspace_Allocate (tri_count * sizeof (*contour_data));
	connect_data = DDspace_Allocate (tri_count * sizeof (*connect_data));

	contour_ptr = contour_data;
	connect_ptr = connect_data;

	for (ii=0;  ii < tri_count;  ++ii)
	{   contour_ptr->count = 3;
	    contour_ptr->shorts = index_data + 3*ii;

	    connect_ptr->count = 1;
	    connect_ptr->lists = contour_ptr;

	    ++contour_ptr;
	    ++connect_ptr;
	}
    }

    /* Store the PEXSetOfFillAreaSets in the triangle structure. */

    PEXSetOfFillAreaSets
    (   DCM->display, tridata->tri_struct, PEXOCStore,
	PEXShapeConvex,			/* Shape Hint */
	facet_attrs,			/* Facet Data Flag */
	vertex_attrs,			/* Vertex Data Flag */
	PEXGANone,			/* Edge Data Flag */
	PEXContourDisjoint,		/* Contour Hint */
	True,				/* Single Contours */
	PEXColorTypeRGB,		/* Color Type */
	(unsigned int) tri_count,	/* Number of Sets */
	facet_data,			/* Facet Data */
	(unsigned int) vtx_count,	/* Number of Vertices */
	vertices,			/* Vertices */
	ntriverts,			/* Number of Indices */
	NULL,				/* Edge Flags */
        connect_data			/* Connectivity */
    );

    /* Now that everything's stored in the PEX structure, we can deallocate
    ** all new memory structures. */

    if (facet_data.normal)	DDspace_Deallocate (facet_data.normal);
    if (vertex_data)		DDspace_Deallocate (vertex_data);
    if (index_data)		DDspace_Deallocate (index_data);
    if (contour_data)		DDspace_Deallocate (contour_data);
    if (connect_data)		DDspace_Deallocate (connect_data);

    /* Set the triangle structure flag to indicate that we now have a PEX
    ** structure for the triangle list. */

    tridata->tri_flag = DcTrue;
}



/*****************************************************************************
**  This routine builds a PEX triangle strip from the Dore' triangle strip
**  data.  On return, it sets the tri_struct or point_struct field of the
**  tridata structure for future PEX execution and sets the appropriate ready
**  flag to true.  If vtx_normal, vtx_color or tri_normal are null, then that
**  data will be omitted from the triangle strip.  If either vtx_normal,
**  vtx_color, or tri_normal is null, then that data will be omitted from the
**  final triangle strip.
*****************************************************************************/

static void BuildTriStrip (
    TriData      *tridata,	/* Additional Triangle Data */
    DtInt         vtx_count,	/* Number of Vertices in Strip */
    DtRealTriple *vtx_location,	/* Vertex XYZ Locations */
    DtRealTriple *vtx_normal,	/* Vertex Normal Vectors */
    DtRealTriple *vtx_color,	/* Vertex RGB Colors */
    DtRealTriple *tri_normal)	/* Triangle Normal Vectors */
{
    auto     unsigned int         facet_attrs;	 /* Triangle Attribute Flags */
    auto     PEXArrayOfFacetData  facet_array;   /* Triangle Facet Data Array */
    register int                  ii;		 /* Loop Indices */
    auto     unsigned int         vertex_attrs;	 /* Vertex Attribute Flags */
    auto     PEXArrayOfVertex     vertex_data;   /* Vertex Data */

#   if DEBUG_CALL
	printf ("BuildTriStrip (%lx, %d, %lx, %lx, %lx, %lx)\n",
	    tridata, vtx_count, vtx_location,vtx_normal,vtx_color, tri_normal);
	fflush (stdout);
#   endif

    /* Create the PEX structure.  If the representation type is DcPoints,
    ** then create a PEX marker list structure with each vertex location and
    ** return.  */

    if (DROM.rep_type == DcPoints)
    {   tridata->point_struct = BuildPointList (vtx_count, vtx_location);
	tridata->point_flag = DcTrue;
	return;
    }

    tridata->tri_struct = PEXCreateStructure (DCM->display);

    /* Set up the facet normals, if any were given. */

    if (!tri_normal)
    {   facet_attrs = PEXGANone;
	facet_array.normal = 0;
    }
    else
    {   register PEXVector *normal_ptr;

	facet_attrs = PEXGANormal;
	facet_array.normal
	    = DDspace_Allocate ((vtx_count-2) * sizeof(*(facet_array.normal)));

	normal_ptr = facet_array.normal;
	for (ii=0;  ii < (vtx_count-2);  ++ii)
	{   normal_ptr->x = tri_normal[ii][0];
	    normal_ptr->y = tri_normal[ii][1];
	    normal_ptr->z = tri_normal[ii][2];
	    ++normal_ptr;
	}
    }

    /* Set up the vertex data according to type. */

    vertex_attrs = (vtx_normal ? PEXGANormal : 0)
		 | (vtx_color  ? PEXGAColor  : 0);

    switch (vertex_attrs)
    {
	case (PEXGANone):
	{   register PEXCoord     *pex_data;
	    register DtRealTriple *vloc;

	    vertex_data.no_data = DDspace_Allocate
				  (vtx_count * sizeof(*(vertex_data.no_data)));

	    pex_data = vertex_data.no_data;
	    vloc = vtx_location;
	    for (ii=0;  ii < vtx_count;  ++ii)
	    {   pex_data->x = (*vloc)[0];
		pex_data->y = (*vloc)[1];
		pex_data->z = (*vloc)[2];
		++pex_data; ++vloc;
	    }
	    break;
	}

	case (PEXGANormal):
	{   register PEXVertexNormal *pex_data;
	    register DtRealTriple    *vloc, *vnrm;

	    vertex_data.normal = DDspace_Allocate
				 (vtx_count * sizeof(*(vertex_data.normal)));

	    pex_data = vertex_data.normal;
	    vloc = vtx_location;
	    vnrm = vtx_normal;
	    for (ii=0;  ii < vtx_count;  ++ii)
	    {   pex_data->point.x  = (*vloc)[0];
		pex_data->point.y  = (*vloc)[1];
		pex_data->point.z  = (*vloc)[2];
		pex_data->normal.x = (*vnrm)[0];
		pex_data->normal.y = (*vnrm)[1];
		pex_data->normal.z = (*vnrm)[2];
		++pex_data; ++vloc; ++vnrm;
	    }
	    break;
	}

	case (PEXGAColor):
	{   register PEXVertexRGB *pex_data;
	    register DtRealTriple *vloc, *vclr;

	    vertex_data.rgb = DDspace_Allocate
			      (vtx_count * sizeof(*(vertex_data.rgb)));

	    pex_data = vertex_data.rgb;
	    vloc = vtx_location;
	    vclr = vtx_color;
	    for (ii=0;  ii < vtx_count;  ++ii)
	    {   pex_data->point.x   = (*vloc)[0];
		pex_data->point.y   = (*vloc)[1];
		pex_data->point.z   = (*vloc)[2];
		pex_data->rgb.red   = (*vclr)[0];
		pex_data->rgb.green = (*vclr)[1];
		pex_data->rgb.blue  = (*vclr)[2];
		++pex_data; ++vloc; ++vclr;
	    }
	    break;
	}

	case (PEXGANormal | PEXGAColor):
	{   register PEXVertexRGBNormal *pex_data;
	    register DtRealTriple *vloc, *vnrm, *vclr;

	    vertex_data.rgb_normal =
		DDspace_Allocate (vtx_count *sizeof(*(vertex_data.rgb_normal)));

	    pex_data = vertex_data.rgb_normal;
	    vloc = vtx_location;
	    vnrm = vtx_normal;
	    vclr = vtx_color;
	    for (ii=0;  ii < vtx_count;  ++ii)
	    {   pex_data->point.x   = (*vloc)[0];
		pex_data->point.y   = (*vloc)[1];
		pex_data->point.z   = (*vloc)[2];
		pex_data->normal.x  = (*vnrm)[0];
		pex_data->normal.y  = (*vnrm)[1];
		pex_data->normal.z  = (*vnrm)[2];
		pex_data->rgb.red   = (*vclr)[0];
		pex_data->rgb.green = (*vclr)[1];
		pex_data->rgb.blue  = (*vclr)[2];
		++pex_data; ++vloc; ++vnrm; ++vclr;
	    }
	    break;
	}
    }

    /* Create the PEX triangle strip. */
    
    PEXTriangleStrip
    (   DCM->display, tridata->tri_struct, PEXOCStore,
	facet_attrs,
	vertex_attrs,
	PEXColorTypeRGB,
	facet_array,
	(int)(vtx_count),
	vertex_data
    );

    /* Now that everything's stored in the PEX structure, we can deallocate
    ** the facet, vertex and polygon data arrays. */

    if (facet_array.normal)
	DDspace_Deallocate (facet_array.normal);

    switch (vertex_attrs)
    {   case (PEXGANone):
	    DDspace_Deallocate (vertex_data.no_data);
	    break;
        case (PEXGANormal):
	    DDspace_Deallocate (vertex_data.normal);
	    break;
        case (PEXGAColor):
	    DDspace_Deallocate (vertex_data.rgb);
	    break;
        case (PEXGANormal | PEXGAColor):
	    DDspace_Deallocate (vertex_data.rgb_normal);
	    break;
    }

    /* Set the triangle structure flag to indicate that we now have a PEX
    ** structure for the triangle strip. */

    tridata->tri_flag = DcTrue;
}
