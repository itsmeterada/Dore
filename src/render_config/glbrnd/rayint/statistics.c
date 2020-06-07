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
	int drr_rayint_initialize_statistics ()
	int drr_rayint_add_statistics_box (box)
	int drr_rayint_add_statistics_rayint_object ()
	int drr_rayint_print_run_statistics ()
	int drr_rayint_print_tree_statistics ()

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/list.h>
#include "rayint.h"
#include "../glbrnd.h"

struct rayint_statistics dre_rayint_statistics;

/*
 ======================================================================
 */

void drr_rayint_initialize_statistics (void)
{
    dre_rayint_statistics.rayint_objects = 0;
    dre_rayint_statistics.bsptree_nodes = 0;
    dre_rayint_statistics.face_nodes = 0;
    dre_rayint_statistics.boxes = 0;
    dre_rayint_statistics.active_boxes = 0;
    dre_rayint_statistics.box_objects = 0;
    dre_rayint_statistics.treedepth = 0;
    dre_rayint_statistics.treepoints = 0;
}
/*
 ======================================================================
 */

void drr_rayint_add_statistics_box (
    struct rayint_box *box)
{
    dre_rayint_statistics.boxes++;

    if (box->object_list == NULL)
	return;

    dre_rayint_statistics.active_boxes++;

    if (box->object_list->list_nodemax <= 0){
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "drr_rayint_add_statistics_box",
		    "active rayint box contains no objects");
	return;
    }

    dre_rayint_statistics.box_objects += box->object_list->list_nodemax;
}
/*
 ======================================================================
 */

void drr_rayint_add_statistics_rayint_object (void)
{
    dre_rayint_statistics.rayint_objects++;
}
/*
 ======================================================================
 */

void drr_rayint_print_run_statistics (void)
{
    DtReal averagetreedepth;

    DDprint("");
    DDprint("rayint run statistics");
    DDprint_IndentUp();

    sprintf(DDprint_GetLine(),
	    "total trees traversed:  %d",
	    dre_rayint_statistics.treedepth);
    DDprint(DDprint_GetLine());

    sprintf(DDprint_GetLine(),
	    "total point-box searches:  %d",
	    dre_rayint_statistics.treepoints);
    DDprint(DDprint_GetLine());

    averagetreedepth = (DtReal)dre_rayint_statistics.treedepth /
	    (DtReal)dre_rayint_statistics.treepoints;

    sprintf(DDprint_GetLine(),
	    "average tree search depth per point:  %lf",
	    averagetreedepth);
    DDprint(DDprint_GetLine());

    DDprint_IndentDown();
    DDprint("rayint run statistics end");

    fflush(stdout);
    fflush(stderr);
}
/*
 ======================================================================
 */

void drr_rayint_print_tree_statistics (void)
{
    auto DtInt  activeboxes;
    auto DtInt  boxes, boxsize, boxmemory;
    auto DtInt  bsptree_nodes, bsptree_nodesize, bsptree_nodememory;
    auto DtInt  objectlists, objectlistsize, objectlistmemory;
    auto DtInt  boxobjects, boxobjectsize, boxobjectmemory;
    auto int    rayintmemory;
    auto char   outputline[200];
    auto DtReal objectsperactbox;

    sprintf(DDprint_GetLine(),
	    "ray intersectable objects:  %d",
	    dre_rayint_statistics.rayint_objects);
    DDprint(DDprint_GetLine());

    sprintf(DDprint_GetLine(),
	    "space bound:  (%f,%f,%f) - (%f,%f,%f)",
	    (double)dre_rayint_data.space_x_minimum,
	    (double)dre_rayint_data.space_y_minimum,
	    (double)dre_rayint_data.space_z_minimum,
	    (double)dre_rayint_data.space_x_maximum,
	    (double)dre_rayint_data.space_y_maximum,
	    (double)dre_rayint_data.space_z_maximum);
    DDprint(DDprint_GetLine());

    sprintf(DDprint_GetLine(),
	    "minimum objects per box:  %d",
	    dre_rayint_data.minimum_objects_per_box);
    DDprint(DDprint_GetLine());

    sprintf(DDprint_GetLine(),
	    "maximum subdivision level:  %d",
	    dre_rayint_data.maximum_subdivision_level);
    DDprint(DDprint_GetLine());

    boxes = dre_rayint_statistics.boxes;

    boxsize = (sizeof (struct rayint_box));

    boxmemory = (boxes*boxsize) / 1024;

    bsptree_nodes = dre_rayint_statistics.bsptree_nodes;

    bsptree_nodesize = (sizeof (struct rayint_bsptree_node));

    bsptree_nodememory = (bsptree_nodes*bsptree_nodesize) / 1024;

    objectlists = dre_rayint_statistics.active_boxes;

    objectlistsize = (sizeof (struct list));

    objectlistmemory = (objectlists*objectlistsize) / 1024;

    boxobjects = dre_rayint_statistics.box_objects;

    boxobjectsize = (sizeof (struct list_node));

    boxobjectmemory = (boxobjects*boxobjectsize) / 1024;

    activeboxes = dre_rayint_statistics.active_boxes;

    /* If there are any ray-intersectable objects in the scene, then compute
    ** the average number of objects per active box, otherwise, set
    ** objects per active box to zero.  */

    if (activeboxes)
	objectsperactbox = (DtReal)boxobjects / (DtReal)activeboxes;
    else
	objectsperactbox = 0.0;

    rayintmemory = boxmemory + bsptree_nodememory + objectlistmemory
		 + boxobjectmemory;

    DDprint("rayint statistics:");
    DDprint("");
    DDprint("     structure      number              bytes/structure     bytes used");
    DDprint("");

    sprintf (outputline, "     %-15s%5d               %5d               %5dk",
	     "box", boxes, boxsize, boxmemory);
    DDprint (outputline);

    sprintf (outputline, "     %-15s%5d               %5d               %5dk",
	     "bsptree node", bsptree_nodes, bsptree_nodesize, bsptree_nodememory);
    DDprint (outputline);

    sprintf (outputline, "     %-15s%5d               %5d               %5dk",
	     "object list", objectlists, objectlistsize, objectlistmemory);
    DDprint(outputline);

    sprintf (outputline, "     %-15s%5d               %5d               %5dk",
	     "box object", boxobjects, boxobjectsize, boxobjectmemory);
    DDprint(outputline);
    DDprint("");

    sprintf (outputline, "     %-15s                                        %5dk",
	     "rayint memory", rayintmemory);
    DDprint(outputline);
    DDprint("");

    sprintf (outputline, "     active boxes:  %d", activeboxes);
    DDprint(outputline);

    sprintf(outputline,"     objects/active box:  %f",(double)objectsperactbox);
    DDprint(outputline);
    DDprint("");

    fflush(stdout);
    fflush(stderr);
}
