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
	drr_rayint_create_box
	drr_rayint_destroy_box
	drr_rayint_get_box
	drr_rayint_get_first_box
	drr_rayint_get_next_box
	drr_rayint_subdivide_box
	drr_rayint_calculate_box_boundary
	drr_rayint_create_bsptree_node
	drr_rayint_create_face_node
	drr_rayint_delete_face_node
	drr_rayint_get_face_node
	drr_rayint_subdivide_face
	drr_rayint_link_faces
	drr_rayint_set_box_face_adjacency
	drr_rayint_change_adjacent_boxes
	drr_rayint_set_bit

  ======================================================================
 */
#include "rayint.h"
#include <dore/internal/list.h>

/*
 ======================================================================
 */

struct rayint_box *drr_rayint_create_box (
    DtReal centerx,
    DtReal centery,
    DtReal centerz,
    DtReal size)
{
    struct rayint_box *box;
    DtReal halfsize;

    box = (struct rayint_box *)DDspace_Allocate(sizeof *box);

    halfsize = size * .5;

    box->xmin = centerx - halfsize;
    box->ymin = centery - halfsize;
    box->zmin = centerz - halfsize;
    box->xmax = centerx + halfsize;
    box->ymax = centery + halfsize;
    box->zmax = centerz + halfsize;

    box->object_list = NULL;

    return(box);
}



/* ====================================================================== */

void drr_rayint_destroy_box (struct rayint_box *box)
{
    if (box->object_list != NULL){
	DDerror (ERR_BOX_NOT_EMPTY, "drr_rayint_destroy_box", DcNullPtr);
	return;
    }

    DDspace_Deallocate(box);
}



/* ====================================================================== */

struct rayint_box *drr_rayint_get_box (DtReal point[3])
{
    struct rayint_bsptree_node *curtreenode, *nexttreenode;
    DtInt nodeno;
    DtInt treedepth;

    curtreenode = dre_rayint_data.rayint_bsptree_root_node;

    treedepth = 1;

    while (1){
	/* If the current bsp tree node pointer is NULL, 
	   then we have left known space, so return a NULL box 
	   pointer, indicating this.  */

	if (curtreenode == NULL){
	    return ((struct rayint_box *)NULL);
	}

	if (point[0] <= curtreenode->x){
	    if (point[1] <= curtreenode->y){
		if (point[2] <= curtreenode->z){
		    nexttreenode = (struct rayint_bsptree_node *)
					curtreenode->nodes[0];
		    nodeno = 0;
		} else {
		    nexttreenode = (struct rayint_bsptree_node *)
					curtreenode->nodes[1];
		    nodeno = 1;
		}
	    } else {
		if (point[2] <= curtreenode->z){
		    nexttreenode = (struct rayint_bsptree_node *)
					curtreenode->nodes[2];
		    nodeno = 2;
		} else {
		    nexttreenode = (struct rayint_bsptree_node *)
					curtreenode->nodes[3];
		    nodeno = 3;
		}
	    }
	} else {
	    if (point[1] <= curtreenode->y){
		if (point[2] <= curtreenode->z){
		    nexttreenode = (struct rayint_bsptree_node *)
					curtreenode->nodes[4];
		    nodeno = 4;
		} else {
		    nexttreenode = (struct rayint_bsptree_node *)
					curtreenode->nodes[5];
		    nodeno = 5;
		}
	    } else {
		if (point[2] <= curtreenode->z){
		    nexttreenode = (struct rayint_bsptree_node *)
					curtreenode->nodes[6];
		    nodeno = 6;
		} else {
		    nexttreenode = (struct rayint_bsptree_node *)
					curtreenode->nodes[7];
		    nodeno = 7;
		}
	    }
	}

	/*  If the selected node points to a box (type is 1), 
	    then return the pointer to the box.  */

	if (curtreenode->nodetypes & (1<<nodeno)){
	    dre_rayint_statistics.treedepth += treedepth;
	    dre_rayint_statistics.treepoints++;

	    return ((struct rayint_box *)nexttreenode);
	}

	/*  The selected node points to another bsp tree node, 
	    set the current bsp tree node pointer to it, and 
	    continue searching the bsp tree.  */
	curtreenode = nexttreenode;

	curtreenode = nexttreenode;

	treedepth++;
    }

}



/* ====================================================================== */

struct rayint_box *drr_rayint_get_first_box (
    DtReal rayorigin[3],
    DtReal raycosine[3],
    DtReal rayslope[3])
{
    struct rayint_box *box;
    DtFlag intersectflag;
    DtReal intersect[3];

    /*  If rayint_get_box returns a NULL box node, then the ray
	originates outside of known space.  If so, find where 
	the ray intersects known space.  If it does not intersect, 
	then return a NULL first box node.  Otherwise, find the 
	box node of the space box where it first enters known space.  
	If rayint_get_box returns a non-NULL box node, then the ray 
	originates within known space.  Return this box node.  */

    box = drr_rayint_get_box(rayorigin);

    if (box != NULL){
	return(box);
    }

    intersectflag = drr_rayint_intersect_ray_space(
				rayorigin,raycosine,rayslope,intersect);

    if (!intersectflag){
	return((struct rayint_box *)NULL);
    }

    intersect[0] += raycosine[0] * .0001;
    intersect[1] += raycosine[1] * .0001;
    intersect[2] += raycosine[2] * .0001;

    box = drr_rayint_get_box(intersect);

    /*  If the box from get_space_box is still NULL, then 
	the point has been pushed through the box.  This can 
	happen in certain special cases where the initial 
	ray-space intersection is right on an edge.  In this 
	case, it is not an error to return a NULL pointer, 
	indicating that the ray has missed the box.  */

    return(box);
}



/* ====================================================================== */

struct rayint_box *drr_rayint_get_next_box (
    struct rayint_box *box,
    DtInt faceno,
    DtReal point[3])
{
    DtReal center1, center2;
    struct rayint_face_node *facenode;
    DtInt facenodeno;

    /*  'Faceno' facenode of the current box points to 
	a single box, return it.  */

    if (box->facetypes & (1<<faceno)){
	return((struct rayint_box *)box->facenodes[faceno]);
    }

    /*  'Faceno' facenode of the current box contains a 
	quad splitting tree, traverse this tree until we find 
	the correct leafnode.  */

    switch (faceno){
    case 0:
	center1 = point[1];
	center2 = point[2];
	break;

    case 1:
	center1 = point[1];
	center2 = point[2];
	break;

    case 2:
	center1 = point[0];
	center2 = point[1];
	break;

    case 3:
	center1 = point[0];
	center2 = point[1];
	break;

    case 4:
	center1 = point[0];
	center2 = point[2];
	break;

    case 5:
	center1 = point[0];
	center2 = point[2];
	break;
    }

    facenode = (struct rayint_face_node *)(box->facenodes[faceno]);

    while (1){
	if (center1 <= facenode->center1){
	    if (center2 <= facenode->center2){
		facenodeno = 0;
	    }
	    else {
		facenodeno = 1;
	    }
	}
	else {
	    if (center2 <= facenode->center2){
		facenodeno = 2;
	    }
	    else {
		facenodeno = 3;
	    }
	}

	if (facenode->nodetypes & (1<<facenodeno)){
	    return ((struct rayint_box *)(facenode->nodes[facenodeno]));
	}

	facenode = (struct rayint_face_node *)(facenode->nodes[facenodeno]);
    }

}
/*
 ======================================================================
 */

void drr_rayint_subdivide_box (
    struct rayint_bsptree_node *bsptreenode,
    DtInt nodeno,
    struct list *rayintobjectlist)
{
    DtReal boxsize;
    DtReal halfboxsize;
    DtReal boxcenter[3];
    struct list *localrayintobjectlist;
    struct rayint_object *rayintobject;
    dot_object *object;
    struct rayint_box *box;
    DtReal boxminmax[6];
    DtFlag overlapflag;
    DtMethodPtr spcboxovrmethod;
    DtReal quarterboxsize;
    struct rayint_bsptree_node *newbsptreenode;
    static DtInt initialnodetypes = 0xff;
    struct rayint_box *subboxes[8];

    box = (struct rayint_box *)(bsptreenode->nodes[nodeno]);

    boxsize = box->xmax - box->xmin;
    halfboxsize = boxsize * .5;

    boxcenter[0] = box->xmin + halfboxsize;
    boxcenter[1] = box->ymin + halfboxsize;
    boxcenter[2] = box->zmin + halfboxsize;

    localrayintobjectlist = dor_list_create();

    boxminmax[0] = box->xmin;
    boxminmax[1] = box->ymin;
    boxminmax[2] = box->zmin;
    boxminmax[3] = box->xmax;
    boxminmax[4] = box->ymax;
    boxminmax[5] = box->zmax;

    for (rayintobject=(struct rayint_object *)
	 dor_list_get_first_data(rayintobjectlist);
	 rayintobject!=NULL;
	 rayintobject=(struct rayint_object *)
	 dor_list_get_next_data(rayintobjectlist)){
	object = rayintobject->object;

	spcboxovrmethod = DDobject_InqMethod(object,
			    	dre_glbrnd_data.SpcboxOvr_method);
	overlapflag = (DtFlag)(*spcboxovrmethod)(object,
				rayintobject->modifier->lcstowcsmat,
				boxminmax);

	if (!overlapflag)
		continue;

	dor_list_add_data_last(localrayintobjectlist,rayintobject);
    }

    /*  Box is empty.  */

    if (dor_list_get_first_data(localrayintobjectlist) == NULL){
	box->object_list = NULL;

	drr_rayint_add_statistics_box(box);

	dor_list_destroy(localrayintobjectlist);

	return;
    }

    quarterboxsize = boxsize * .25;

    /*  Box contains the minimum number of objects.  */

    if (localrayintobjectlist->list_nodemax <=
	dre_rayint_data.minimum_objects_per_box){
	box->object_list = localrayintobjectlist;

	drr_rayint_add_statistics_box(box);

	return;
    }

    /*  Box is of minimum size.  */

    if (halfboxsize <= dre_rayint_data.minimum_box_size){
	box->object_list = localrayintobjectlist;

	drr_rayint_add_statistics_box(box);

	return;
    }

    /*  Subdivide box.  */

    subboxes[0] = drr_rayint_create_box(
				boxcenter[0]-quarterboxsize,
				boxcenter[1]-quarterboxsize,
				boxcenter[2]-quarterboxsize,
				halfboxsize);

    subboxes[1] = drr_rayint_create_box(
				boxcenter[0]-quarterboxsize,
				boxcenter[1]-quarterboxsize,
				boxcenter[2]+quarterboxsize,
				halfboxsize);

    subboxes[2] = drr_rayint_create_box(
				boxcenter[0]-quarterboxsize,
				boxcenter[1]+quarterboxsize,
				boxcenter[2]-quarterboxsize,
				halfboxsize);

    subboxes[3] = drr_rayint_create_box(
				boxcenter[0]-quarterboxsize,
				boxcenter[1]+quarterboxsize,
				boxcenter[2]+quarterboxsize,
				halfboxsize);

    subboxes[4] = drr_rayint_create_box(
				boxcenter[0]+quarterboxsize,
				boxcenter[1]-quarterboxsize,
				boxcenter[2]-quarterboxsize,
				halfboxsize);

    subboxes[5] = drr_rayint_create_box(
				boxcenter[0]+quarterboxsize,
				boxcenter[1]-quarterboxsize,
				boxcenter[2]+quarterboxsize,
				halfboxsize);

    subboxes[6] = drr_rayint_create_box(
				boxcenter[0]+quarterboxsize,
				boxcenter[1]+quarterboxsize,
				boxcenter[2]-quarterboxsize,
				halfboxsize);

    subboxes[7] = drr_rayint_create_box(
				boxcenter[0]+quarterboxsize,
				boxcenter[1]+quarterboxsize,
				boxcenter[2]+quarterboxsize,
				halfboxsize);

    newbsptreenode = drr_rayint_create_bsptree_node
		         (boxcenter, (DtPtr*)subboxes, initialnodetypes);

    bsptreenode->nodes[nodeno] = (DtPtr)newbsptreenode;
    drr_rayint_set_bit(&(bsptreenode->nodetypes),nodeno,0);

    drr_rayint_link_faces(box,subboxes,boxcenter);

    drr_rayint_destroy_box(box);

    drr_rayint_subdivide_box(newbsptreenode,0,localrayintobjectlist);
    drr_rayint_subdivide_box(newbsptreenode,1,localrayintobjectlist);
    drr_rayint_subdivide_box(newbsptreenode,2,localrayintobjectlist);
    drr_rayint_subdivide_box(newbsptreenode,3,localrayintobjectlist);
    drr_rayint_subdivide_box(newbsptreenode,4,localrayintobjectlist);
    drr_rayint_subdivide_box(newbsptreenode,5,localrayintobjectlist);
    drr_rayint_subdivide_box(newbsptreenode,6,localrayintobjectlist);
    drr_rayint_subdivide_box(newbsptreenode,7,localrayintobjectlist);

    dor_list_destroy(localrayintobjectlist);
}



/* ====================================================================== */

void drr_rayint_calculate_box_boundary (
    DtReal boxmin[3],
    DtReal boxmax[3])
{
    DtFlag firstobjectflag = DcTrue;
    dot_object *object;
    DtMethodPtr wldbndmethod;
    struct list *rayintobjectlist;
    struct rayint_object *rayintobject;
    DtReal newboxmin[3], newboxmax[3];
    DtReal range[3];
    DtReal maxrange;
    DtReal halfrange;
    DtReal midpoint[3];

    rayintobjectlist = dre_rayint_data.rayint_object_list;

    /* Get the first ray-intersectable object from the list. */

    rayintobject = dor_list_get_first_data (rayintobjectlist);

    /* If there are no ray-intersectable objects in the list, then set the box
    ** boundary to [0,0,0] x [1,1,1] (it really doesn't matter) and return.  */

    if (!rayintobject)
    {   boxmin[0] = boxmin[1] = boxmin[2] = 0.0;
	boxmax[0] = boxmax[1] = boxmax[2] = 1.0;
	return;
    }

    /* If there are objects in the ray-intersectable object list, then expand
    ** the bounding box to contain the bounding box for each object.   */

    while (rayintobject) {
	object = rayintobject->object;

	wldbndmethod = DDobject_InqMethod(object,dre_glbrnd_data.WldBnd_method);
	(*wldbndmethod)
	(   object, rayintobject->modifier->lcstowcsmat,
	    &newboxmin[0], &newboxmin[1], &newboxmin[2],
	    &newboxmax[0], &newboxmax[1], &newboxmax[2]
	);

	if (firstobjectflag){
	    boxmin[0] = newboxmin[0];
	    boxmin[1] = newboxmin[1];
	    boxmin[2] = newboxmin[2];
	    boxmax[0] = newboxmax[0];
	    boxmax[1] = newboxmax[1];
	    boxmax[2] = newboxmax[2];

	    firstobjectflag = DcFalse;
	} else {
	    boxmin[0] = min(boxmin[0],newboxmin[0]);
	    boxmax[0] = max(boxmax[0],newboxmax[0]);
	    boxmin[1] = min(boxmin[1],newboxmin[1]);
	    boxmax[1] = max(boxmax[1],newboxmax[1]);
	    boxmin[2] = min(boxmin[2],newboxmin[2]);
	    boxmax[2] = max(boxmax[2],newboxmax[2]);
	}

	rayintobject = dor_list_get_next_data (rayintobjectlist);
    }

    range[0] = boxmax[0] - boxmin[0];
    range[1] = boxmax[1] - boxmin[1];
    range[2] = boxmax[2] - boxmin[2];

    maxrange = max (range[0], range[1]);
    maxrange = max (maxrange, range[2]);

    maxrange = 1.1 * maxrange;

    midpoint[0] = (boxmax[0]+boxmin[0]) * 0.5;
    midpoint[1] = (boxmax[1]+boxmin[1]) * 0.5;
    midpoint[2] = (boxmax[2]+boxmin[2]) * 0.5;

    halfrange = maxrange * 0.5;

    boxmin[0] = midpoint[0] - halfrange;
    boxmin[1] = midpoint[1] - halfrange;
    boxmin[2] = midpoint[2] - halfrange;
    boxmax[0] = midpoint[0] + halfrange;
    boxmax[1] = midpoint[1] + halfrange;
    boxmax[2] = midpoint[2] + halfrange;
}
/*
 ======================================================================
 */

struct rayint_bsptree_node *drr_rayint_create_bsptree_node (
    DtReal location[3],
    DtPtr  nodeptrs[8],
    DtInt  nodetypes)
{
    struct rayint_bsptree_node *node;
    DtInt nodeno;

    node = (struct rayint_bsptree_node *)
		DDspace_Allocate(sizeof *node);

    node->x = location[0];
    node->y = location[1];
    node->z = location[2];

    for (nodeno=0;nodeno<8;nodeno++){
	node->nodes[nodeno] = nodeptrs[nodeno];
    }

    node->nodetypes = nodetypes;

    dre_rayint_statistics.bsptree_nodes++;

    return(node);
}
/*
 ======================================================================
 */

struct rayint_face_node *drr_rayint_create_face_node (
    DtReal center1,
    DtReal center2,
    struct rayint_box *llbox,
    struct rayint_box *ulbox,
    struct rayint_box *lrbox,
    struct rayint_box *urbox)
{
    struct rayint_face_node *node;

    node = (struct rayint_face_node *)
		DDspace_Allocate(sizeof *node);

    node->center1 = center1;
    node->center2 = center2;

    node->nodes[0] = (DtPtr)llbox;
    node->nodes[1] = (DtPtr)ulbox;
    node->nodes[2] = (DtPtr)lrbox;
    node->nodes[3] = (DtPtr)urbox;

    drr_rayint_set_bit(&(node->nodetypes),0,1);
    drr_rayint_set_bit(&(node->nodetypes),1,1);
    drr_rayint_set_bit(&(node->nodetypes),2,1);
    drr_rayint_set_bit(&(node->nodetypes),3,1);

    dre_rayint_statistics.face_nodes++;

    return(node);
}
/*
 ======================================================================
 */

void drr_rayint_delete_face_node (struct rayint_face_node *facenode)
{
    /*
      if (facenode->nodes[0] != NULL ||
      facenode->nodes[1] != NULL ||
      facenode->nodes[2] != NULL ||
      facenode->nodes[3] != NULL){
      DDerror (ERR_VALUE_OUT_OF_RANGE,
      "drr_rayint_delete_face_node", 
      "face node still points to something");
      
      return;
      }
      */

    DDspace_Deallocate(facenode);
}
/*
 ======================================================================
 */

void drr_rayint_get_face_node (
    struct rayint_face_node *facenode,
    struct rayint_box *origbox,
    struct rayint_face_node **adjfacenode,
    DtInt *adjnodeno)
{
    if (facenode == NULL){
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "drr_rayint_get_face_node", "facenode is NULL");
	return;
    }

    *adjfacenode = NULL;

    if (facenode->nodetypes & (1<<0)){
	if ((struct rayint_box *)(facenode->nodes[0]) == origbox){
	    *adjfacenode = facenode;
	    *adjnodeno = 0;

	    return;
	}
    } else {
	drr_rayint_get_face_node(facenode->nodes[0],
				origbox,adjfacenode,adjnodeno);

	if (*adjfacenode != NULL){
	    return;
	}
    }

    if (facenode->nodetypes & (1<<1)){
	if ((struct rayint_box *)(facenode->nodes[1]) == origbox){
	    *adjfacenode = facenode;
	    *adjnodeno = 1;

	    return;
	}
    } else {
	drr_rayint_get_face_node(facenode->nodes[1],
				origbox,adjfacenode,adjnodeno);

	if (*adjfacenode != NULL){
	    return;
	}
    }

    if (facenode->nodetypes & (1<<2)){
	if ((struct rayint_box *)(facenode->nodes[2]) == origbox){
	    *adjfacenode = facenode;
	    *adjnodeno = 2;

	    return;
	}
    } else {
	drr_rayint_get_face_node(facenode->nodes[2],
				origbox,adjfacenode,adjnodeno);

	if (*adjfacenode != NULL){
	    return;
	}
    }

    if (facenode->nodetypes & (1<<3)){
	if ((struct rayint_box *)(facenode->nodes[3]) == origbox){
	    *adjfacenode = facenode;
	    *adjnodeno = 3;

	    return;
	}
    } else {
	drr_rayint_get_face_node(facenode->nodes[3],
				origbox,adjfacenode,adjnodeno);

	if (*adjfacenode != NULL){
	    return;
	}
    }
}
/*
 ======================================================================
 */

	/*  We are subdividing 'origbox' into the new boxes 'llbox',
	'ulbox', 'urbox', and 'lrbox'.  This routine updates the
	adjacency links between the 'faceno' faces of the new boxes,
	and the faces of the boxes adjacent to them.  */

void drr_rayint_subdivide_face (
    struct rayint_box *origbox,
    struct rayint_box *llbox,
    struct rayint_box *ulbox,
    struct rayint_box *lrbox,
    struct rayint_box *urbox,
    DtInt faceno,
    DtReal center1,
    DtReal center2)
{
    DtFlag oneadjboxflag;
    DtInt adjfaceno;
    struct rayint_box *adjbox;
    struct rayint_face_node *facenode;
    struct rayint_face_node *adjfacenode;
    DtInt adjnodeno;
    struct rayint_face_node *firstfacenode;
    DtInt boxbit;

    /*  There are two possible cases; either the current 'origbox'
	box points to a single adjacent box, or it points to 4 or more
	adjacent boxes.  The first case happens when the adjacent box
	has never been subdivided, or the adjacent box is outside of
	known space (in which case the pointer to it is NULL).  The
	second case happens when the adjacent box has been subdivided.
	In this case, 'origbox' will have at least one level of
	splitting face nodes in its face adjacency graph.  */

    oneadjboxflag = (origbox->facetypes & (1<<faceno));

    /*  Compute the general face identification number which would
	be adjacent to 'faceno'.  */

    adjfaceno = (faceno ^ 1);

    if (oneadjboxflag){
	/*  There is a single box adjacent to this face, 
	    it can either be NULL (outside of known space), 
	    or an actual box.  */

	adjbox = (struct rayint_box *)(origbox->facenodes[faceno]);

	if (adjbox != NULL){
	    /*  There is one box adjacent to 'origbox'.  */

	    /*  Check if the appropriate face adjacency
		graph of the adjacent box has been subdivided.  */

	    if (adjbox->facetypes & (1<<adjfaceno)){
		/*  No subdivision, adjacent box should point 
		    only to origbox.  */

		if ((struct rayint_box *)
			(adjbox->facenodes[adjfaceno]) != origbox){
		    DDerror (ERR_VALUE_OUT_OF_RANGE,
				"drr_rayint_subdivide_face",
				"single adjacent box does not point to origbox");

		    return;
		}

		/*  Create a face splitting node
		    pointing to the new boxes being
		    created, and attach the node to the
		    adjacent box's face.  */

		facenode = drr_rayint_create_face_node(
						center1,center2,
						llbox,ulbox,lrbox,urbox);

		adjbox->facenodes[adjfaceno] = (DtPtr)facenode;

		/*  Set the adjacent box's facetype
		    field to indicate that it now points to
		    a splitting node, rather than a box
		    node.  */

		drr_rayint_set_bit(&(adjbox->facetypes),adjfaceno,0);

		/*  Make each of the faces of the new
		    boxes point to the single adjacent
		    box.  */

		llbox->facenodes[faceno] = (DtPtr)adjbox;
		drr_rayint_set_bit(&(llbox->facetypes),faceno,1);

		ulbox->facenodes[faceno] = (DtPtr)adjbox;
		drr_rayint_set_bit(&(ulbox->facetypes),faceno,1);

		lrbox->facenodes[faceno] = (DtPtr)adjbox;
		drr_rayint_set_bit(&(lrbox->facetypes),faceno,1);

		urbox->facenodes[faceno] = (DtPtr)adjbox;
		drr_rayint_set_bit(&(urbox->facetypes),faceno,1);

	    } else {
		/*  Subdivision, one of the leaf nodes of the 
		    adjacent box should point to origbox.  */

		/*  Find the face graph face node attached to 
		    the adjacent box which points to origbox.  
		    Return a pointer to the face node and the 
		    nodeno of the portion that points to origbox.  */

		drr_rayint_get_face_node(adjbox->facenodes[adjfaceno],
					origbox, &adjfacenode,&adjnodeno);

		if (adjfacenode == NULL){
		    DDerror (ERR_VALUE_OUT_OF_RANGE,
				"drr_rayint_subdivide_face", 
				"subdivided adjacent box does not point to origbox");
		    return;
		}

		/*  Create a face splitting node pointing to the 
		    new boxes being created, and attach the node 
		    to the adjacent box's face node.  */

		facenode = drr_rayint_create_face_node(
						center1,center2,
						llbox,ulbox,lrbox,urbox);

		adjfacenode->nodes[adjnodeno] = (DtPtr)facenode;

		/*  Set the adjacent box's node's nodetype 
		    field to indicate that it now points to 
		    a splitting node, rather than a box node.  */

		drr_rayint_set_bit(&(adjfacenode->nodetypes),adjnodeno,0);

		/*  Make each of the faces of the new
		    boxes point to the single adjacent box.  */

		llbox->facenodes[faceno] = (DtPtr)adjbox;
		drr_rayint_set_bit(&(llbox->facetypes),faceno,1);

		ulbox->facenodes[faceno] = (DtPtr)adjbox;
		drr_rayint_set_bit(&(ulbox->facetypes),faceno,1);

		lrbox->facenodes[faceno] = (DtPtr)adjbox;
		drr_rayint_set_bit(&(lrbox->facetypes),faceno,1);

		urbox->facenodes[faceno] = (DtPtr)adjbox;
		drr_rayint_set_bit(&(urbox->facetypes),faceno,1);
	    }

	} else {
	    /*  Adjacent box is NULL, indicating that it is
		outside of known space.  */

	    /*  Make each of the faces of the new boxes
		point to outside of known space.  */

	    llbox->facenodes[faceno] = NULL;
	    drr_rayint_set_bit(&(llbox->facetypes),faceno,1);

	    ulbox->facenodes[faceno] = NULL;
	    drr_rayint_set_bit(&(ulbox->facetypes),faceno,1);

	    lrbox->facenodes[faceno] = NULL;
	    drr_rayint_set_bit(&(lrbox->facetypes),faceno,1);

	    urbox->facenodes[faceno] = NULL;
	    drr_rayint_set_bit(&(urbox->facetypes),faceno,1);
	}
    } else {
	/*  There are multiple boxes adjacent to this face.
	    All of these boxes should currently be pointing to
	    'origbox', and all of them should be changed to point
	    to the appropriate new box.  */

	firstfacenode = (struct rayint_face_node *)
				(origbox->facenodes[faceno]);

	drr_rayint_change_adjacent_boxes(firstfacenode,0,adjfaceno,
					origbox,llbox);
	drr_rayint_change_adjacent_boxes(firstfacenode,1,adjfaceno,
					origbox,ulbox);
	drr_rayint_change_adjacent_boxes(firstfacenode,2,adjfaceno,
					origbox,lrbox);
	drr_rayint_change_adjacent_boxes(firstfacenode,3,adjfaceno,
					origbox,urbox);

	/*  Split the first face node of 'origbox' up into
	    its 4 components, and attach each component to the
	    appropriate new box.  */

	llbox->facenodes[faceno] = firstfacenode->nodes[0];
	boxbit = ((firstfacenode->nodetypes & (1<<0)) != 0);
	drr_rayint_set_bit(&(llbox->facetypes),faceno,boxbit);

	ulbox->facenodes[faceno] = firstfacenode->nodes[1];
	boxbit = ((firstfacenode->nodetypes & (1<<1)) != 0);
	drr_rayint_set_bit(&(ulbox->facetypes),faceno,boxbit);

	lrbox->facenodes[faceno] = firstfacenode->nodes[2];
	boxbit = ((firstfacenode->nodetypes & (1<<2)) != 0);
	drr_rayint_set_bit(&(lrbox->facetypes),faceno,boxbit);

	urbox->facenodes[faceno] = firstfacenode->nodes[3];
	boxbit = ((firstfacenode->nodetypes & (1<<3)) != 0);
	drr_rayint_set_bit(&(urbox->facetypes),faceno,boxbit);

	/*  Delete the first face node.  */

	drr_rayint_delete_face_node(firstfacenode);
    }

    /*  Delete the face adjacency reference from the
	'faceno' face of 'origbox'.  */

    origbox->facenodes[faceno] = NULL;
    drr_rayint_set_bit(&(origbox->facetypes),faceno,1);
}



/* ====================================================================== */

void drr_rayint_link_faces (
    struct rayint_box *box,
    struct rayint_box *sbx[8],
    DtReal center[3])
{
    /*  Front Lower Left box.  */
    drr_rayint_set_box_face_adjacency(sbx[0], DcNullPtr, sbx[4],
				DcNullPtr, sbx[1],DcNullPtr,sbx[2]);

    /*  Back Lower Left box.  */
    drr_rayint_set_box_face_adjacency(sbx[1], DcNullPtr,sbx[5],
				sbx[0],DcNullPtr,DcNullPtr,sbx[3]);

    /*  Front Upper Left box.  */
    drr_rayint_set_box_face_adjacency(sbx[2], DcNullPtr,sbx[6],
				DcNullPtr,sbx[3],sbx[0],DcNullPtr);

    /*  Back Upper Left box.  */
    drr_rayint_set_box_face_adjacency(sbx[3], DcNullPtr,sbx[7],
				sbx[2],DcNullPtr,sbx[1],DcNullPtr);

    /*  Front Lower Right box.  */
    drr_rayint_set_box_face_adjacency(sbx[4], sbx[0],DcNullPtr,
				DcNullPtr,sbx[5],DcNullPtr,sbx[6]);

    /*  Back Lower Right box.  */
    drr_rayint_set_box_face_adjacency(sbx[5], sbx[1],DcNullPtr,
				sbx[4],DcNullPtr,DcNullPtr,sbx[7]);

    /*  Front Upper Right box.  */
    drr_rayint_set_box_face_adjacency(sbx[6], sbx[2],DcNullPtr,
				DcNullPtr,sbx[7],sbx[4],DcNullPtr);

    /*  Back Upper Right box.  */
    drr_rayint_set_box_face_adjacency(sbx[7], sbx[3],DcNullPtr,
				sbx[6],DcNullPtr,sbx[5],DcNullPtr);

    /*  Left face.  */
    drr_rayint_subdivide_face(box,sbx[0],sbx[1],sbx[2],sbx[3],0,
			      center[1],center[2]);

    /*  Right face.  */
    drr_rayint_subdivide_face(box,sbx[4],sbx[5],sbx[6],sbx[7],1,
			      center[1],center[2]);

    /*  Front face.  */
    drr_rayint_subdivide_face(box,sbx[0],sbx[2],sbx[4],sbx[6],2,
			      center[0],center[1]);

    /*  Back face.  */
    drr_rayint_subdivide_face(box,sbx[1],sbx[3],sbx[5],sbx[7],3,
			      center[0],center[1]);

    /*  Bottom face.  */
    drr_rayint_subdivide_face(box,sbx[0],sbx[1],sbx[4],sbx[5],4,
			      center[0],center[2]);

    /*  Top face.  */
    drr_rayint_subdivide_face(box,sbx[2],sbx[3],sbx[6],sbx[7],5,
			      center[0],center[2]);
}
/*
 ======================================================================
 */

void drr_rayint_set_box_face_adjacency (
    struct rayint_box *box,
    struct rayint_box *box0,
    struct rayint_box *box1,
    struct rayint_box *box2,
    struct rayint_box *box3,
    struct rayint_box *box4,
    struct rayint_box *box5)
{
    box->facenodes[0] = (DtPtr)box0;
    box->facenodes[1] = (DtPtr)box1;
    box->facenodes[2] = (DtPtr)box2;
    box->facenodes[3] = (DtPtr)box3;
    box->facenodes[4] = (DtPtr)box4;
    box->facenodes[5] = (DtPtr)box5;

    drr_rayint_set_bit(&(box->facetypes),0,1);
    drr_rayint_set_bit(&(box->facetypes),1,1);
    drr_rayint_set_bit(&(box->facetypes),2,1);
    drr_rayint_set_bit(&(box->facetypes),3,1);
    drr_rayint_set_bit(&(box->facetypes),4,1);
    drr_rayint_set_bit(&(box->facetypes),5,1);
}
/*
 ======================================================================
 */

	/*  The 'nodeno' branch of the 'facenode' passed in points to
	to one or more boxes adjacent to the 'origbox' box being
	subdivided.  Check to make sure that the 'adjfaceno' face node
	of each of these boxes is a single box pointer back to
	'origbox'.  Change this pointer to point to 'newbox'.  */

void drr_rayint_change_adjacent_boxes (
    struct rayint_face_node *facenode,
    DtInt nodeno,
    DtInt adjfaceno,
    struct rayint_box *origbox,
    struct rayint_box *newbox)
{
    struct rayint_box *adjbox;

    if (facenode->nodetypes & (1<<nodeno)){
	/*  Face node branch is a pointer to a box.  */

	adjbox = (struct rayint_box *)facenode->nodes[nodeno];

	if (adjbox->facetypes & (1<<adjfaceno)){
	    if ((struct rayint_box *)(adjbox->facenodes[adjfaceno]) !=
		origbox){
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "drr_rayint_change_adjacent_boxes", 
			    "adjacent box does not point to origbox");

		return;
	    }

	    adjbox->facenodes[adjfaceno] = (DtPtr)newbox;
	    drr_rayint_set_bit(&(adjbox->facetypes),adjfaceno,1);

	} else {
	    DDerror (ERR_VALUE_OUT_OF_RANGE,
			"drr_rayint_change_adjacent_boxes", 
			"adjacent box does not have single box pointer");

	    return;
	}

    } else {
	/*  Face node branch is a pointer to another face node.  */

	drr_rayint_change_adjacent_boxes(facenode->nodes[nodeno],0,
					 adjfaceno,origbox,newbox);
	drr_rayint_change_adjacent_boxes(facenode->nodes[nodeno],1,
					 adjfaceno,origbox,newbox);
	drr_rayint_change_adjacent_boxes(facenode->nodes[nodeno],2,
					 adjfaceno,origbox,newbox);
	drr_rayint_change_adjacent_boxes(facenode->nodes[nodeno],3,
					 adjfaceno,origbox,newbox);
    }
}
/*
 ======================================================================
 */

void drr_rayint_set_bit (
    DtInt *variable,
    DtInt bitno,
    DtInt value)
{
    *variable = (*variable & ~(1<<bitno)) | (value<<bitno);
}
