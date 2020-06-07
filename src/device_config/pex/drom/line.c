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
**  This file contains routines for both line lists and connected line lists.
*****************************************************************************/

#include "../pex_driver.h"
#include <dore/internal/system.h>
#include <dore/dore_develop/private/linlst.h>
#include <dore/dore_develop/private/varlinlst.h>
#include <dore/dore_develop/private/plylin.h>

#define DEBUG 0


    /***  Data Structures  ***/

typedef struct
{   PEXStructure  line_struct;	/* PEX PolylineSet Structure */
    DtFlag        line_flag;	/* True:  PolylineSet Ready for Execution */
    PEXStructure  point_struct;	/* PEX Points Structure */
    DtFlag        point_flag;	/* True:  Points Ready for Execution */
    DtInt         seqnum;	/* Sequence Number for Variable Primitives */
} LineData;


    /***  Function Declarations  ***/

static void  BuildLineList  ARGS((LineData*,DtInt,DtRealTriple*,DtRealTriple*));
static void  BuildPolyline  ARGS((struct plylin *, LineData *));



/*****************************************************************************
**  The lines_create routine allocates memory for the line-list and polyline
**  data structure on object creation.  This is also where preliminary
**  initialization takes place.
*****************************************************************************/

DtPtr ddr_pex_drom_lines_create (DtObjectStructure *object)
{
    register LineData  *linedata;	/* Line-List/Polyline Data */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_lines_create (%lx)\n", object);
	fflush (stdout);
#   endif

    linedata = DDspace_Allocate (sizeof(*linedata));
    linedata->line_flag  = DcFalse;
    linedata->point_flag = DcFalse;

    return linedata;
}



/*****************************************************************************
**  This routine deletes the lines data allocated with the _lines_create.
**  It destroys the associated PEX structures, if there are any, and then
**  deallocates the allocated data structure.
*****************************************************************************/

void ddr_pex_drom_lines_delete (
    DtObjectStructure *object,
    DtPtr              linedata_parm)
{
    auto LineData *linedata = linedata_parm;

#   if DEBUG_CALL
	printf ("ddr_pex_drom_lines_delete (%lx, %lx)\n", object,linedata_parm);
	fflush (stdout);
#   endif

    if (!linedata) return;

    if (linedata->line_flag)
        PEXDestroyStructures (DCM->display, 1, &(linedata->line_struct));

    if (linedata->point_flag)
        PEXDestroyStructures (DCM->display, 1, &(linedata->point_struct));

    DDspace_Deallocate (linedata);
}



/*****************************************************************************
**  The lines_print routine prints the contents of the line data structure to
**  the standard output stream.
*****************************************************************************/

void ddr_pex_drom_lines_print (
    DtObjectStructure *object,		/* Line Object */
    DtPtr              linedata)
{
    return;	/* No-op */
}



/*****************************************************************************
**  This routine is called to render a particular line list.  If this
**  routine is called for the first time, then we also need to bundle up
**  and initialize everything for PEX.
*****************************************************************************/

void ddr_pex_drom_linelist_render (dot_object *object)
{
    static DtInt          classid = -1;		/* Line List Class ID */
    static DtInt          dataindex = -1;	/* Linelist Add'l Data Field */
    auto   LineData      *linedata;		/* Line List Data */
    auto   struct linlst *linelist;		/* List List Dore' Struct */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_linelist_render (%lx)\n", object);
	fflush (stdout);
#   endif

    /* Get the class ID and additional-data index. */

    if (!GetClassInfo (&classid, "DoLineList", &dataindex))
	return;

    /* Get the additional_data field of the line list. */

    linedata = (LineData*) object->additional_data [dataindex];

    /* If the line list is not yet ready for PEX rendering, then we need
    ** to build the PEX structure for execution. */

    linelist = (struct linlst *) (object->data);

    if (  ((DROM.rep_type == DcPoints) && (!linedata->point_flag))
       || ((DROM.rep_type != DcPoints) && (!linedata->line_flag))
       )
    {
	BuildLineList
	(   linedata, linelist->line_count, linelist->vertex_location,
	    (linelist->vertextype & DcLocClr) ? linelist->vertex_color : 0
	);
    }

    /* Now execute the PEX structure that contains the line list. */

    if (DROM.rep_type != DcPoints)
    {   PEXSetLineColor
	(   DCM->display, PEX.renderer, PEXOCRender, 
	    PEXColorTypeRGB, &PEX.pc_attrs.surface_color.value
	);
	PEXExecuteStructure
	    (DCM->display, PEX.renderer, PEXOCRender, linedata->line_struct);
	doe_system_timing_lines_drawn += linelist->line_count;
    }
    else
    {   PEXSetMarkerColor
	(   DCM->display, PEX.renderer, PEXOCRender, 
	    PEXColorTypeRGB, &PEX.pc_attrs.surface_color.value
	);
	PEXExecuteStructure
	    (DCM->display, PEX.renderer, PEXOCRender, linedata->point_struct);
	doe_system_timing_points_drawn += 2 * linelist->line_count;
    }
}



/*****************************************************************************
**  This routine is called to render a variable line list.  If the linelist
**  has not yet been created or if the sequence number has changed, then the
**  old line list (if one exists) is destroyed and a new one is created.
*****************************************************************************/

void ddr_pex_drom_varlinelist_render (dot_object *object)
{
    static   DtInt             classid = -1;	/* line List Class ID */
    static   DtInt             dataindex = -1;	/* linelist Add'l Data Field */
    auto     LineData         *linedata;	/* line List Data */
    auto     struct varlinlst *varlinelist;	/* Var Line List Structure */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_linelist_render (%lx)\n", object);
	fflush (stdout);
#   endif

    varlinelist = (struct varlinlst *) object->data;

    /* Get the class ID and additional-data index. */

    if (!GetClassInfo (&classid, "DoVarLineList", &dataindex))
	return;

    /* Get the additional_data field of the line list. */

    linedata = (LineData*) object->additional_data [dataindex];

    /* If the sequence number of the variable line list has changed, then
    ** first invalidate and destroy the current line list, if one exists.  */

    if (varlinelist->sequence_number != linedata->seqnum)
    {
	if (linedata->line_flag)
	{   PEXDestroyStructures (DCM->display, 1, &(linedata->line_struct));
	    linedata->line_flag = DcFalse;
	}

	if (linedata->point_flag)
	{   PEXDestroyStructures (DCM->display, 1, &(linedata->point_struct));
	    linedata->point_flag = DcFalse;
	}
	linedata->seqnum = varlinelist->sequence_number;
    }

    /* If the line list is not yet ready for PEX rendering, then we need
    ** to build the PEX structure for execution. */

    if (  ((DROM.rep_type == DcPoints) && (!linedata->point_flag))
       || ((DROM.rep_type != DcPoints) && (!linedata->line_flag))
       )
    {
	BuildLineList
	(   linedata, varlinelist->line_count,
	    varlinelist->vertex_location, varlinelist->vertex_color
	);
    }

    /* Now execute the PEX structure that contains the line list. */

    if (DROM.rep_type != DcPoints)
    {   PEXSetLineColor
	(   DCM->display, PEX.renderer, PEXOCRender, 
	    PEXColorTypeRGB, &PEX.pc_attrs.surface_color.value
	);
	PEXExecuteStructure
	    (DCM->display, PEX.renderer, PEXOCRender, linedata->line_struct);
	doe_system_timing_lines_drawn += varlinelist->line_count;
    }
    else
    {   PEXSetMarkerColor
	(   DCM->display, PEX.renderer, PEXOCRender, 
	    PEXColorTypeRGB, &PEX.pc_attrs.surface_color.value
	);
	PEXExecuteStructure
	    (DCM->display, PEX.renderer, PEXOCRender, linedata->point_struct);
	doe_system_timing_points_drawn += 2 * varlinelist->line_count;
    }
}



/*****************************************************************************
**  This routine is called to render a particular polyline.  If this routine
**  is called for the first time, then we also need to bundle up and
**  initialize everything for PEX.
*****************************************************************************/

void ddr_pex_drom_polyline_render (dot_object *object)
{
    static   DtInt          classid = -1;	/* Line List Class ID */
    static   DtInt          dataindex = -1;	/* Polyline Add'l Data Field */
    auto     LineData      *linedata;		/* line List Data */
    auto     struct plylin *polyline;		/* Polyline Dore' Struct */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_polyline_render (%lx)\n", object);
	fflush (stdout);
#   endif

    /* Get the class ID and additional-data index. */

    if (!GetClassInfo (&classid, "DoPolyline", &dataindex))
	return;

    /* Get the additional_data field of the line list. */

    linedata = (LineData*) object->additional_data [dataindex];

    /* If the polyline is not yet ready for PEX rendering, then we need
    ** to build the PEX structure for execution. */

    polyline = (struct plylin *) object->data;

    if (  ((DROM.rep_type == DcPoints) && (!linedata->point_flag))
       || ((DROM.rep_type != DcPoints) && (!linedata->line_flag))
       )
    {
	BuildPolyline (polyline, linedata);
    }

    /* Now execute the PEX structure that contains the polyline. */

    if (DROM.rep_type != DcPoints)
    {   PEXSetLineColor
	(   DCM->display, PEX.renderer, PEXOCRender, 
	    PEXColorTypeRGB, &PEX.pc_attrs.surface_color.value
	);
	PEXExecuteStructure
	    (DCM->display, PEX.renderer, PEXOCRender, linedata->line_struct);
	doe_system_timing_connected_lines_drawn += polyline->vertex_count - 1;
    }
    else
    {   PEXSetMarkerColor
	(   DCM->display, PEX.renderer, PEXOCRender, 
	    PEXColorTypeRGB, &PEX.pc_attrs.surface_color.value
	);
	PEXExecuteStructure
	    (DCM->display, PEX.renderer, PEXOCRender, linedata->point_struct);
	doe_system_timing_points_drawn += polyline->vertex_count;
    }
}



/*****************************************************************************
**  This routine builds a PEX PolyLineSet structure.  If the current rep type
**  is DcPoints, then the point representation will be generated, otherwise
**  the line list (PEX polyline set) will be created.  If the vcolor parameter
**  is not null, then color information will also be included.  This routine
**  sets the appropriate PEX structure handle and ready flag of the linedata
**  structure.
*****************************************************************************/

static void  BuildLineList (
    LineData     *linedata,		/* Additional-Data for Line Lists */
    DtInt         line_count,		/* Number of Lines in List */
    DtRealTriple *vloc,			/* Line Vertex Locations */
    DtRealTriple *vcolor)		/* Vertex Colors (if any) */
{
    register int              ii;		/* Loop Index */
    auto     unsigned long    vertex_attrs;	/* Vertex Attribute Flags */
    auto     PEXCoord        *vertdata = 0;	/* Vertex Data */
    auto     PEXVertexRGB    *vertdata_clr = 0;	/* Vertex Data with Color */
    auto     PEXListOfVertex *vertlists = 0;	/* Lists of Polyline Vertices */

#   if DEBUG_CALL
	printf ("BuildLineList (%lx, %d, %lx, %lx)\n",
	    linedata, line_count, vloc, vcolor);
	fflush (stdout);
#   endif

    /* Create the PEX structure.  If the representation type is DcPoints,
    ** then create a PEX marker list structure with each vertex location and
    ** return.  */

    if (DROM.rep_type == DcPoints)
    {   linedata->point_struct = BuildPointList (line_count * 2, vloc);
	linedata->point_flag   = DcTrue;
	return;
    }

    /* Since the rep type is not DcPoints, go ahead and render the line
    ** list normally.  First build the PEX structure for the line list. */

    linedata->line_struct = PEXCreateStructure (DCM->display);

    if (vcolor)
    {   vertex_attrs = PEXGAColor;
	vertdata_clr = DDspace_Allocate (2*line_count * sizeof(*vertdata_clr));
    }
    else
    {   vertex_attrs = PEXGANone;
	vertdata = DDspace_Allocate (2*line_count * sizeof(*vertdata));
    }

    /* Load the vertex information into the vertex data array. */

    for (ii=0;  ii < (2 * line_count);  ++ii)
    {
	if (!vcolor)
	{   vertdata[ii].x = vloc[ii][0];
	    vertdata[ii].y = vloc[ii][1];
	    vertdata[ii].z = vloc[ii][2];
	}
	else
	{   vertdata_clr[ii].point.x = vloc[ii][0];
	    vertdata_clr[ii].point.y = vloc[ii][1];
	    vertdata_clr[ii].point.z = vloc[ii][2];

	    vertdata_clr[ii].rgb.red   = vcolor[ii][0];
	    vertdata_clr[ii].rgb.green = vcolor[ii][1];
	    vertdata_clr[ii].rgb.blue  = vcolor[ii][2];
	}
    }

    /* Allocate the vertex data array. */

    vertlists = DDspace_Allocate (line_count * sizeof(*vertlists));

    /* Fill the polyline array with the data for each line. */

    for (ii=0;  ii < line_count;  ++ii)
    {   vertlists[ii].count = 2;
	if (vcolor)
	    vertlists[ii].vertices.rgb = vertdata_clr + (2 * ii);
	else
	    vertlists[ii].vertices.no_data = vertdata + (2 * ii);
    }

    /* Store the line list into the PEX structure. */

    PEXPolylineSetWithData
    (   DCM->display, linedata->line_struct, PEXOCStore,
	vertex_attrs, PEXColorTypeRGB, line_count, vertlists
    );

    /* Now that everything's stored in the PEX structure, we can deallocate
    ** the facet, vertex and polygon data arrays. */

    if (vertdata)	DDspace_Deallocate (vertdata);
    if (vertdata_clr)	DDspace_Deallocate (vertdata_clr);
    if (vertlists)	DDspace_Deallocate (vertlists);

    /* Set the line structure flag to indicate that we now have a PEX
    ** structure for the line list. */

    linedata->line_flag = DcTrue;
}



/*****************************************************************************
**  This routine builds a PEX PolylineSetWithData structure from the Dore'
**  line-list data.  On return, it sets the line_struct field of the linedata
**  structure for future PEX execution and sets the ready flag to true.
*****************************************************************************/

static void  BuildPolyline  (
    struct plylin *polyline,	/* Dore' line List Data */
    LineData      *linedata)	/* line Data Structure */
{
    register int              ii;		/* Loop Index */
    auto     DtFlag           vcolor;		/* True: Include Vtx Colors */
    auto     unsigned long    vertex_attrs;	/* Vertex Attribute Flags */
    auto     PEXListOfVertex  vertlist;		/* Lists of Polyline Vertices */
    auto     PEXCoord        *vertdata=0;	/* Vertex Data (no color) */
    auto     PEXVertexRGB    *vertdata_clr=0;	/* Vertex Data (with color) */

#   if DEBUG_CALL
	printf ("BuildPolyline (%lx, %lx)\n", polyline, linedata);
	fflush (stdout);
#   endif

#   if DEBUG
	print ("+ Polyline Info:\n");
	printf("+     vertex type %d\n", polyline->vertextype);
	printf("+     %d vertices\n", polyline->vertex_count);
	printf("+     vloc %lx, vnorm %lx, vcolor %lx\n",
	    polyline->vertex_location, polyline->vertex_normal,
	    polyline->vertex_color);
	fflush (stdout);
#   endif

    /* Create the PEX structure.  If the representation type is DcPoints,
    ** then create a PEX marker list structure with each vertex location and
    ** return.  */

    if (DROM.rep_type == DcPoints)
    {   linedata->point_struct = BuildPointList (polyline->vertex_count,
						 polyline->vertex_location);
	linedata->point_flag = DcTrue;
	return;
    }

    /* Since the rep type is not DcPoints, go ahead and render the line
    ** list normally.  First build the PEX structure for the polyline. */

    linedata->line_struct = PEXCreateStructure (DCM->display);

    /* Determine the number of data triples that will be given for each
    ** vertex.  */

    vcolor = (polyline->vertextype & DcLocClr);

    if (vcolor)
    {   vertex_attrs = PEXGAColor;
	vertdata_clr = DDspace_Allocate
			   (polyline->vertex_count * sizeof(*vertdata_clr));
    }
    else
    {   vertex_attrs = PEXGANone;
	vertdata = DDspace_Allocate
		       (polyline->vertex_count * sizeof(*vertdata));
    }

    /* Load the vertex information into the vertex data array.  Note that all
    ** internal vertices are doubled; once for the end of a line, and once for
    ** the beginning of the next line.  */

    for (ii=0;  ii < polyline->vertex_count;  ++ii)
    {
	register DtReal *Vp, *Vc;	/* Vertex Position & Color */

	Vp = polyline->vertex_location[ii];

	if (!vcolor)
	{   vertdata[ii].x = Vp[0];
	    vertdata[ii].y = Vp[1];
	    vertdata[ii].z = Vp[2];
	}
	else
	{   Vc = polyline->vertex_color[ii];
	    vertdata_clr[ii].point.x   = Vp[0];
	    vertdata_clr[ii].point.y   = Vp[1];
	    vertdata_clr[ii].point.z   = Vp[2];
	    vertdata_clr[ii].rgb.red   = Vc[0];
	    vertdata_clr[ii].rgb.green = Vc[1];
	    vertdata_clr[ii].rgb.blue  = Vc[2];
	}
    }

    /* Create the PEX vertex list. */

    vertlist.count = polyline->vertex_count;
    if (vcolor)
	vertlist.vertices.rgb = vertdata_clr;
    else
	vertlist.vertices.no_data = vertdata;

    PEXPolylineSetWithData
    (   DCM->display, linedata->line_struct, PEXOCStore,
	vertex_attrs, PEXColorTypeRGB, 1, &vertlist
    );

    /* Now that everything's stored in the PEX structure, we can deallocate
    ** the facet, vertex and polygon data arrays. */

    if (vertdata)	DDspace_Deallocate (vertdata);
    if (vertdata_clr)	DDspace_Deallocate (vertdata_clr);

    /* Set the line structure flag to indicate that we now have a PEX
    ** structure for the line list. */

    linedata->line_flag = DcTrue;
}
