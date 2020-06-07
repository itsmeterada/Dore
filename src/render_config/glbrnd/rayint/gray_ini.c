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
	int drr_rayint_initialize_subsystem ()
	int drr_rayint_initialize_render ()
	int drr_rayint_initialize_camera (cameraobject)
	int drr_rayint_initialize_object (object)
	int drr_rayint_initialize_object_list ()
	int drr_rayint_initialize_object_modifier (lcstowcsflag)
	int drr_rayint_initialize_tree ()

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/matrix.h>
#include <dore/internal/list.h>
#include "rayint.h"
#include "../glbrnd.h"

struct rayint_data dre_rayint_data;

/* ====================================================================== */

void drr_rayint_initialize_subsystem (void)
{
    dre_rayint_data.debug_switch = 0;

    dre_rayint_data.rayint_object_list = NULL;
}



/* ====================================================================== */

void drr_rayint_initialize_render (void)
{
    drr_rayint_initialize_tree();

    drr_rayint_print_tree_statistics();
}



/* ====================================================================== */

void drr_rayint_initialize_camera (dot_object *cameraobject)
{
    dor_global_glbrndmaxobjs_get_value(
		       &(dre_rayint_data.minimum_objects_per_box));

    dor_global_glbrndmaxsub_get_value(
		      &(dre_rayint_data.maximum_subdivision_level));
}



/* ====================================================================== */

void drr_rayint_initialize_object (dot_object *object)
{
    struct rayint_object *rayintobject;

    rayintobject = (struct rayint_object *)
			DDspace_Allocate(sizeof *rayintobject);

    rayintobject->modifier = 
			dre_rayint_data.current_rayint_object_modifier;

    rayintobject->object = object;

    dor_list_add_data_last(dre_rayint_data.rayint_object_list,rayintobject);

    drr_rayint_add_statistics_rayint_object();
}



/* ====================================================================== */

void drr_rayint_initialize_object_list (void)
{
    if (dre_rayint_data.rayint_object_list != NULL)
	    dor_list_destroy(dre_rayint_data.rayint_object_list); 

    dre_rayint_data.rayint_object_list = dor_list_create();
}



/* ====================================================================== */

void drr_rayint_initialize_object_modifier (DtFlag lcstowcsflag)
{
    struct rayint_object_modifier *objectmod;
    DtMatrix4x4 lcstowcsmat;
    DtMatrix4x4 *newmatrix;

    objectmod = (struct rayint_object_modifier *)
			DDspace_Allocate(sizeof *objectmod);

    /*  Tell the global shading subsystem to get the surface
	characteristics at this time, and to return a pointer to an
	appropriate structure containing this information.  Place this
	pointer in the rayint object's object modifier.  */

    objectmod->shade_data = drr_glbshd_create_object_data();

    /*  If the object tells us that we do not have to apply a
	matrix transformation to the ray (ie. all of the object's
	subobjects are represented in wcs), then indicate this
	in the rayint object.  Otherwise, get the current lcstowcsmat
	transformation from the global lcstowcsmat attribute and place
	this matrix and its inverse in the rayint object.  */

    if (!lcstowcsflag){
	objectmod->lcstowcsmat = NULL;
	objectmod->wcstolcsmat = NULL;

    } else{
	/*  Obtain the value of the lcstowcsmat transformation
	    from the lcstowcsmat global attribute.  */

	dor_global_lcstowcsmat_get_value(lcstowcsmat);

	newmatrix = dor_matrix_create();
	dor_matrix_load (*newmatrix,lcstowcsmat);
	objectmod->lcstowcsmat = newmatrix;

	newmatrix = dor_matrix_create();
	dor_matrix_load (*newmatrix,lcstowcsmat);
	dor_matrix_invert (*newmatrix);
	objectmod->wcstolcsmat = newmatrix;
    }

    dre_rayint_data.current_rayint_object_modifier = objectmod;
}



/* ====================================================================== */

void drr_rayint_initialize_tree (void)
{
    DtReal boxmin[3], boxmax[3];
    DtReal boxsize;
    DtReal boxcenter[3];
    struct rayint_bsptree_node *worldminnode, *worldmaxnode;
    static struct rayint_bsptree_node *maxnodeptrs[8] =
	    { DcNullPtr, DcNullPtr, DcNullPtr, DcNullPtr,
		      DcNullPtr, DcNullPtr, DcNullPtr, DcNullPtr };
    static struct rayint_bsptree_node *minnodeptrs[8] =
	    { DcNullPtr, DcNullPtr, DcNullPtr, DcNullPtr,
		      DcNullPtr, DcNullPtr, DcNullPtr, DcNullPtr };
    struct rayint_box *box;

    drr_rayint_calculate_box_boundary(boxmin,boxmax);

    dre_rayint_data.space_x_minimum = boxmin[0];
    dre_rayint_data.space_y_minimum = boxmin[1];
    dre_rayint_data.space_z_minimum = boxmin[2];
    dre_rayint_data.space_x_maximum = boxmax[0];
    dre_rayint_data.space_y_maximum = boxmax[1];
    dre_rayint_data.space_z_maximum = boxmax[2];

    boxsize = boxmax[0] - boxmin[0];

    boxcenter[0] = boxmin[0] + boxsize*.5;
    boxcenter[1] = boxmin[1] + boxsize*.5;
    boxcenter[2] = boxmin[2] + boxsize*.5;

    box = drr_rayint_create_box(
		boxcenter[0],boxcenter[1],boxcenter[2],boxsize);

    /*  The world box initially has NULL boxes adjacent to all 6 faces.  */

    drr_rayint_set_box_face_adjacency(box,DcNullPtr,DcNullPtr,
		DcNullPtr,DcNullPtr,DcNullPtr,DcNullPtr);

    maxnodeptrs[0] = (struct rayint_bsptree_node *)box;

    worldmaxnode = drr_rayint_create_bsptree_node
		   (boxmax, (DtPtr*) maxnodeptrs, 0x01);

    minnodeptrs[7] = worldmaxnode;

    worldminnode = drr_rayint_create_bsptree_node
		   (boxmin, (DtPtr*) minnodeptrs, 0x00);

    dre_rayint_data.rayint_bsptree_root_node = worldminnode;

    dre_rayint_data.minimum_box_size = boxsize /
	    (DtReal)dre_rayint_data.maximum_subdivision_level;

    drr_rayint_subdivide_box(worldmaxnode,0,
			dre_rayint_data.rayint_object_list);
}
