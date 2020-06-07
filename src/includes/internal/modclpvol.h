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
 
#ifndef DORE_MODCLPVOL_H
#define DORE_MODCLPVOL_H
 
/*****************************************************************************
**  This file contains type definitions and function prototypes for the model-
**  clipping volume routines.
*****************************************************************************/

    /**************************/
    /***  Type Definitions  ***/
    /**************************/

	/* Model-Clipping Volume.  This structure contains a list of
	// halfspaces that define the acceptance region of the model-
	// clipping space.  */

typedef struct {
    DtInt        halfspacecount;	/* Number of Halfspaces */
    DtHalfSpace *halfspaces;		/* Array of Halfspaces */
    DtPoint4    *plneqns;		/* Array of Halfspace Plane Equations */
} dot_modclpvol;


    /*****************************/
    /***  Function Prototypes  ***/
    /*****************************/

void           dor_modclpvol_apply_volume   (dot_modclpvol*, DtMatrix4x4,
					     DtClipOperator,DtInt,DtHalfSpace*);
dot_modclpvol *dor_modclpvol_copy           (dot_modclpvol*);
dot_modclpvol *dor_modclpvol_create         (DtMatrix4x4, DtClipOperator, DtInt,
					     DtHalfSpace*);
void           dor_modclpvol_destroy        (dot_modclpvol*);
void           dor_modclpvol_get_value      (dot_modclpvol*, DtInt*, DtPoint3*,
					     DtVector3*, DtPoint4*);
void           dor_modclpvol_plane_equation (dot_modclpvol*,DtInt,DtMatrix4x4);
void           dor_modclpvol_print          (dot_modclpvol*);
void           dor_modclpvol_set_value      (dot_modclpvol*, DtMatrix4x4,
					     DtClipOperator,DtInt,DtHalfSpace*);

#endif
