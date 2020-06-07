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
**  This file contains routines for point lists.
*****************************************************************************/

#include "../pex_driver.h"
#include <dore/internal/system.h>
#include <dore/dore_develop/private/pntlst.h>
#include <dore/dore_develop/private/varpntlst.h>

#define DEBUG 0



    /***  Data Structures  ***/

typedef struct
{   PEXStructure  pex_struct;	/* PEX Point List Structure */
    DtFlag        ready;	/* True:  Point List Ready for Execution */
    DtInt         seqnum;	/* Dore' Variable Point List Sequence Number */
} PointData;



/*****************************************************************************
**  The pointlist_create routine allocates memory for the point-list data
**  structure on object creation.  This is also where preliminary
**  initialization takes place.
*****************************************************************************/


DtPtr ddr_pex_drom_pointlist_create (DtObjectStructure *object)
{
    register PointData  *pointdata;	/* Point List Data */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_pointlist_create (%lx)\n", object);
	fflush (stdout);
#   endif

    pointdata = DDspace_Allocate (sizeof(*pointdata));
    pointdata->ready = DcFalse;

    return pointdata;
}



/*****************************************************************************
**  The pointlist_print routine prints the contents of the point-list data
**  structure to the standard output stream.
*****************************************************************************/

void ddr_pex_drom_pointlist_print (
    DtObjectStructure *object,
    DtPtr              pointlist_data)
{
    /* No-op */
}



/*****************************************************************************
**  The pointlist_delete routine deallocates PEX structures and memory for
**  the given point data.
*****************************************************************************/

void ddr_pex_drom_pointlist_delete (
    DtObjectStructure *object,
    DtPtr              pointdata_parm)
{
    auto PointData *pointdata = pointdata_parm;

#   if DEBUG_CALL
	printf ("ddr_pex_drom_pointlist_delete (%lx, %lx)\n",
	    object, pointdata_parm);
	fflush (stdout);
#   endif

    if (!pointdata) return;

    if (pointdata->ready)
        PEXDestroyStructures (DCM->display, 1, &(pointdata->pex_struct));

    DDspace_Deallocate (pointdata);
}



/*****************************************************************************
**  This routine is called to render a particular point list.  If this
**  routine is called for the first time, then we also need to bundle up
**  and initialize everything for PEX.
*****************************************************************************/

void ddr_pex_drom_pointlist_render (dot_object *object)
{
    static DtInt          classid = -1;		/* Point List Class ID */
    static DtInt          dataindex = -1;	/* Point List Add'l Data Field*/
    auto   PointData     *pointdata;		/* Point List Additional Data */
    auto   struct pntlst *pointlist;		/* Point List Dore' Struct */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_pointlist_render (%lx)\n", object);
	fflush (stdout);
#   endif

    /* Get the class ID and additional-data index. */

    if (!GetClassInfo (&classid, "DoPointList", &dataindex))
	return;

    /* Get the additional_data field of the point list. */

    pointdata = (PointData*) object->additional_data [dataindex];

    /* If the point list is not yet ready for PEX rendering, then we need
    ** to build the PEX marker list structure for execution. */

    pointlist = (struct pntlst *) object->data;

    if (!pointdata->ready)
    {   pointdata->pex_struct =
	    BuildPointList (pointlist->point_count, pointlist->vertex_location);
	pointdata->ready = DcTrue;
    }

    /* Now execute the PEX structure that contains the point list. */

    PEXSetMarkerColor
    (   DCM->display, PEX.renderer, PEXOCRender, 
	PEXColorTypeRGB, &PEX.pc_attrs.surface_color.value
    );
    PEXExecuteStructure
	(DCM->display, PEX.renderer, PEXOCRender, pointdata->pex_struct);

    doe_system_timing_points_drawn += pointlist->point_count;
}



/*****************************************************************************
**  This routine is called to render a variable point list.  If the PEX
**  point structure is not yet ready, or if the sequence number has changed,
**  then the old PEX structure (if one exists) is deleted, and a new one is
**  created.
*****************************************************************************/

void ddr_pex_drom_varpointlist_render (dot_object *object)
{
    static DtInt             classid = -1;	/* Point List Class ID */
    static DtInt             dataindex = -1;	/* Point List Addl Data Field */
    auto   struct varpntlst *pointlist		/* Var Point List Data */
				= (struct varpntlst *) object->data;
    auto   PointData        *pointdata;		/* Point List Additional Data */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_pointlist_render (%lx)\n", object);
	fflush (stdout);
#   endif

    /* Get the class ID and additional-data index. */

    if (!GetClassInfo (&classid, "DoVarPointList", &dataindex))
	return;

    /* Get the additional_data field of the var point list. */

    pointdata = (PointData*) object->additional_data [dataindex];

    /* If the sequence number has changed, then we need to destroy the old
    ** point list before we create the new one.  */

#   if DEBUG
	printf ("+ varpointlist sequence_number %d, last %d\n",
	    pointlist->sequence_number, pointdata->seqnum);
#   endif

    if (pointlist->sequence_number != pointdata->seqnum)
    {   if (pointdata->ready)
	{   PEXDestroyStructures (DCM->display, 1, &(pointdata->pex_struct));
	    pointdata->ready = DcFalse;
	}
	pointdata->seqnum = pointlist->sequence_number;
    }

    /* If the point list is not yet ready for PEX rendering, then we need
    ** to build the PEX marker list structure for execution. */

    if (!pointdata->ready)
    {   pointdata->pex_struct =
	    BuildPointList (pointlist->point_count, pointlist->vertex_location);
	pointdata->ready = DcTrue;
    }

    /* Now execute the PEX structure that contains the point list. */

    PEXSetMarkerColor
    (   DCM->display, PEX.renderer, PEXOCRender, 
	PEXColorTypeRGB, &PEX.pc_attrs.surface_color.value
    );
    PEXExecuteStructure
	(DCM->display, PEX.renderer, PEXOCRender, pointdata->pex_struct);

    doe_system_timing_points_drawn += pointlist->point_count;
}



/*****************************************************************************
**  This routine builds a PEX Markers structure from the `num_points'
**  locations passed by `locations'.  It returns the PEX structure ID of the
**  resulting PEX markers structure.
*****************************************************************************/

PEXStructure  BuildPointList  (
    DtInt         num_points,	/* Number of Points in List */
    DtRealTriple *locations)	/* Point Locations */
{
    register int          ii;		/* Loop Index */
    auto     PEXCoord    *pex_coords;	/* PEX Coordinate Array */
    auto     PEXStructure pex_struct;	/* PEX Structure */

#   if DEBUG_CALL
	printf ("BuildPointList (%d, %lx)\n", num_points, locations);
	fflush (stdout);
#   endif

    /* Create the PEX structure. */

    pex_struct = PEXCreateStructure (DCM->display);

    /* Allocate the PEX coordinate array. */

    pex_coords = DDspace_Allocate (num_points * sizeof(*pex_coords));

    /* Load the vertex information into the PEX coordinate data array. */

    for (ii=0;  ii < num_points;  ++ii)
    {   pex_coords[ii].x = locations[ii][0];
        pex_coords[ii].y = locations[ii][1];
        pex_coords[ii].z = locations[ii][2];
    }

    /* Set the marker type and load the PEX-markers command into the PEX
    ** structure. */

    PEXSetMarkerType (DCM->display, pex_struct, PEXOCStore, PEXMarkerDot);
    PEXMarkers (DCM->display, pex_struct, PEXOCStore, num_points, pex_coords);

    /* Now that everything's stored in the PEX structure, we can deallocate
    ** the PEX coordinate data. */

    DDspace_Deallocate (pex_coords);

    /* Return the PEX structure ID of the point list. */

    return pex_struct;
}
