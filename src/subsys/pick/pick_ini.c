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
 
/* ======================================================================
  Functions:
	dor_pick_initialize
	dor_pick_initialize_before_environment
	dor_pick_initialize_environment
	dor_pick_initialize_after_environment
	dor_pick_initialize_camera
	dor_pick_initialize_object
	dor_pick_initialize_ccstowcsmat

  ====================================================================== */

#include <dore/internal/system.h>
#include <dore/internal/matrix.h>
#include "pick.h"

struct pick_data doe_pick_data;

DtObject DcPickFirst;
DtObject DcPickClosest;
DtObject DcPickAll;

/* ====================================================================== */

void dor_pick_initialize (void)
{
    doe_pick_data.stack_base
	= dor_space_allocate
	  (     STACK_GROW_SIZE			    /* Initial # Elements */
	      * 3				    /* # Words per Element */
	      * sizeof(*doe_pick_data.stack_base)   /* Size of Each Word */
	  );

    /* The root pick id always 0. */

    doe_pick_data.stack_base[1] = 0;

    doe_pick_data.stack_words = STACK_GROW_WORDS;

    DcPickFirst
	= dor_calbak_create ((DtPFI)dor_pick_first, DcNullObject, DcFalse);
    DcPickClosest
	= dor_calbak_create ((DtPFI)dor_pick_closest, DcNullObject, DcFalse);
    DcPickAll
	= dor_calbak_create ((DtPFI)dor_pick_all, DcNullObject, DcFalse);

    /* Create a dummy reference so that these don't go away when the user
    ** dereferences them.  This is because the derefrencing code deletes
    ** the object when the reference count goes to zero.  */

    dor_object_add_reference (DcPickFirst,   DcNullObject);
    dor_object_add_reference (DcPickClosest, DcNullObject);
    dor_object_add_reference (DcPickAll,     DcNullObject);
}

/* ====================================================================== */

void dor_pick_initialize_before_environment (void)
{
    /*   The lcstowcsmat attribute is always initialized to the identity
    ** matrix.  When the initialize camera method is being executed on the
    ** definition object, the lcstowcsmat attribute represents the current
    ** local to world transformation for the camera.  When the camera is 
    ** executed, the current value of this transformation represents the
    ** final local to world transformation for the camera, and will be
    ** loaded into the pick_data data structure.  */

    doe_pick_data.object_viewtype = dod_pick_viewtype_perspective;

    dor_matrix_load_perspective
	(doe_pick_data.object_projmat, 90.0, -0.01, -1.0);

    dor_matrix_load (doe_pick_data.projmat, doe_pick_data.object_projmat);
    doe_pick_data.viewtype = doe_pick_data.object_viewtype;

    dor_pick_initialize_ccstowcsmat();
}

/* ===========================================================================
**    The purpose of environmental initialization is to collect data related
**  to the camera that is performing the picking.  This data always includes
**  the camera's local to world transform (which is inverted to obtain the
**  world to camera transform).
=========================================================================== */

void dor_pick_initialize_environment (void)
{
    dot_object *definitiongroup;
    DtMethodPtr executemethod;

    definitiongroup = dor_view_inq_definition_group(doe_pick_data.view);

    dor_pick_initialize_before_environment();

    /*  Indicate to all of the attribute objects what the current 
	method being executed is.  */

    doe_system_current_method = DcMethodIniPick;

    executemethod = dor_object_get_current_method(definitiongroup);
    (*executemethod)(definitiongroup);

    doe_system_current_method = DcMethodNull;

    dor_pick_initialize_after_environment();
}

/* ====================================================================== */

void dor_pick_initialize_after_environment (void)
{
    dor_matrix_load (doe_pick_data.wcstoccsmat, doe_pick_data.ccstowcsmat);
    dor_matrix_invert (doe_pick_data.wcstoccsmat);
}

/* ====================================================================== */

void dor_pick_initialize_camera (
    dot_object *object)
{
    DtObject camera;

    camera = dor_view_inq_active_camera (doe_pick_data.view);

    if (camera == DcNullObject || camera == (DtObject) object) {
	dor_matrix_load (doe_pick_data.projmat, doe_pick_data.object_projmat);
	doe_pick_data.viewtype = doe_pick_data.object_viewtype;
	dor_pick_initialize_ccstowcsmat();
    }
}

/* ====================================================================== */

void dor_pick_initialize_object (
    dot_object *object)
{
    auto DtMatrix4x4 lcstowcsmat;  /* Local-To-World Transformation */

    doe_pick_data.current_element = object;

    dor_global_lcstowcsmat_get_value (lcstowcsmat);

    dor_matrix_load (doe_pick_data.lcstofcsmat, doe_pick_data.wcstofcsmat);
    dor_matrix_pre_concatenate (doe_pick_data.lcstofcsmat, lcstowcsmat);

    if (doe_pick_data.lcs_points != NULL) {
	dor_matrix_load (doe_pick_data.fcstolcsmat, doe_pick_data.lcstofcsmat);
	dor_matrix_invert (doe_pick_data.fcstolcsmat);
    }
}

/* ====================================================================== */

void dor_pick_initialize_ccstowcsmat (void)
{
    auto DtMatrix4x4 lcstowcsmat;

    dor_global_lcstowcsmat_get_value (lcstowcsmat);
    dor_matrix_load (doe_pick_data.ccstowcsmat, lcstowcsmat);
}
