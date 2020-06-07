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
**  This file contains the routines and functions necessary for the model-
**  clipping operations.  Not all of the Dore' clipping volume operations are
**  supported.  For more information about the modelling clip information,
**  see the appendix at the end of this file.
*****************************************************************************/

#include "../pex_driver.h"

    /*** Defined Constants ***/

#define DEBUG 0

#define CHUNK_SIZE  4		/* Model Clip Halfspace List Chunk Size */


    /*** Internal Functions ***/

static void ClipList_Add   ARGS((ModClipList*, int, DtHalfSpace*));
static void ClipList_Set   ARGS((ModClipList*));
#if DEBUG
static void ClipList_Print ARGS((ModClipList*));
#endif



/*****************************************************************************
**  This routine set the modeling clip switch.  The modeling clip volume is
**  defined by a set of half spaces and operators that specify how they
**  interact.  If `switchvalue' is DcOn, then modeling clip is enabled,
*   otherwise it is disabled.
*****************************************************************************/

void  ddr_pex_drom_set_att_clpswi  (DtSwitch switchvalue)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_clpswi (%d)\n", switchvalue);
	fflush (stdout);
#   endif

    /* The default modeling-clip settings are OFF, so if we're in the
    |  pre-initialization phase, just return.  */

    if (dde_pex_drom_pre_init)
	return;

    DROM.modclip_switch = switchvalue;
    PEXSetModelClipFlag
	(DCM->display, PEX.renderer, PEXOCRender, switchvalue ? PEXOn : PEXOff);
}



/*****************************************************************************
**  This routine adds modeling clip planes to the current clip list.
*****************************************************************************/

void  ddr_pex_drom_apply_att_clpvol (
    DtClipOperator operator,		/* Type of Addition to Perform */
    DtInt          halfspacecount,	/* Number of New Halfspaces */
    DtHalfSpace   *halfspaces)
{
#   if DEBUG_CALL
    {   register int ii;	/* Loop Index */
	printf ("ddr_pex_drom_apply_att_clpvol (%d, %d, %lx)\n",
	    operator, halfspacecount, halfspaces);
	for (ii=0;  ii < halfspacecount;  ++ii)
	    printf ("+     halfspace %d pnt <%g %g %g> vec <%g %g %g>\n",
		ii, halfspaces[ii].point[0], halfspaces[ii].point[1],
		halfspaces[ii].point[2], halfspaces[ii].vector[0],
		halfspaces[ii].vector[1], halfspaces[ii].vector[2]);
	fflush (stdout);
    }
#   endif

#   if DEBUG
	printf ("+ Current clipvol:\n");
	ClipList_Print (DROM.modclip_list);
	fflush (stdout);
#   endif

    if (!DROM.modclip_list) return;

    switch (operator)
    {
	case DcClipXOr:		/* Unsupported Operations */
	case DcClipOr:
	case DcClipEqv:
	case DcClipOrReverse:
	case DcClipOrInverted:
	case DcClipNAnd:
	    return;

	case DcClipNoOp:	/* Do-Nothing Operation */
	    return;

	default:
	    break;
    }

    /* Invert the current volume if it consists of a single halfspace. */

    if (  (operator == DcClipAndInverted) || (operator == DcClipNor)
       || (operator == DcClipInvert)
       )
    {
	/* Can't support the inverse of multiple halfspaces, so just return
	   if this is requested.  */

	if (DROM.modclip_list->num_planes > 1)
	    return;

	/* If the current clipping volume consists of a single halfspace,
	   then invert it. */

	if (DROM.modclip_list->num_planes == 1)
	{   auto PEXHalfSpace *plane = DROM.modclip_list->planes;
	    plane->vector.x = -plane->vector.x;
	    plane->vector.y = -plane->vector.y;
	    plane->vector.z = -plane->vector.z;
	}
    }

    /* Invert the new volume if it consists of a single halfspace. */

    if (  (operator == DcClipAndReverse) || (operator == DcClipNor)
       || (operator == DcClipInvertVolume)
       )
    {
	/* Can't support the inverse of multiple halfspaces, so just return
	   if this is requested.  */

	if (halfspacecount > 1)
	    return;
	
	if (halfspacecount == 1)
	{   halfspaces->vector[0] = -halfspaces->vector[0];
	    halfspaces->vector[1] = -halfspaces->vector[1];
	    halfspaces->vector[2] = -halfspaces->vector[2];
	}
    }

    switch (operator)
    {
	case DcClipNone:
	    DROM.modclip_list->num_planes = 0;
	    break;

	/* If the new region needed to be inverted, it was done above. */

	case DcClipReplace:
	case DcClipInvertVolume:
	    DROM.modclip_list->num_planes = 0;	/* Clear current region. */
	    /* vvv Fall through to new-region addition. vvv */

	/* The following operations are all equivalent, since the inversions
	   were performed above.  */

	case DcClipAnd:
	case DcClipAndReverse:
	case DcClipAndInverted:
	case DcClipNor:
	    ClipList_Add (DROM.modclip_list, halfspacecount, halfspaces);
	    break;

	/* The current region was already inverted above. */

	case DcClipInvert:
	    break;

	/* Fake DcClipAll by defining two mutually-exclusive halfspaces. */

	case DcClipAll:
	{   if (DROM.modclip_list->array_size < 2)
	    {   DROM.modclip_list->planes =
		    DDspace_Reallocate (DROM.modclip_list->planes,
					2 * sizeof(PEXHalfSpace));
		DROM.modclip_list->array_size = 2;
	    }
	    DROM.modclip_list->num_planes = 2;

	    /* Define the two halfspaces. */

	    DROM.modclip_list->planes[0].point.x  = 0;  /* Point  0,0,0 */
	    DROM.modclip_list->planes[0].point.y  = 0;
	    DROM.modclip_list->planes[0].point.z  = 0;
	    DROM.modclip_list->planes[0].vector.x = 1;  /* Vector 1,0,0 */
	    DROM.modclip_list->planes[0].vector.y = 0;
	    DROM.modclip_list->planes[0].vector.z = 0;

	    DROM.modclip_list->planes[1].point.x  = 0;  /* Point  0,0,0 */
	    DROM.modclip_list->planes[1].point.y  = 0;
	    DROM.modclip_list->planes[1].point.z  = 0;
	    DROM.modclip_list->planes[1].vector.x = -1; /* Vector -1,0,0 */
	    DROM.modclip_list->planes[1].vector.y = 0;
	    DROM.modclip_list->planes[1].vector.z = 0;
	    break;
	}

	default:
	    fprintf (stderr,
		"ERROR in %s[%d]:  Unexpected clipping operator (%d).\n",
		__FILE__, __LINE__, operator);
	    return;
    }

#   if DEBUG
	printf ("+ Resulting clipvol:\n");
	ClipList_Print (DROM.modclip_list);
	fflush (stdout);
#   endif

    ClipList_Set (DROM.modclip_list);
}



/*****************************************************************************
**  This routine pushes the current modeling-clip set on the modeling clip
**  stack.  It creates a new duplicate entry on the top of the stack, which
**  may then be modified through the ddr_pex_drom_set_att_clpvol() or
**  ddr_pex_drom_add_att_clpvol
*****************************************************************************/

void ddr_pex_drom_push_att_clpvol (void)
{
    register int           ii;				/* Loop Index */
    register ModClipList  *new_modclip;			/* New Clip List */
    register PEXHalfSpace *newplane, *prevplane;	/* Clip Plane Ptrs */

#   if DEBUG_CALL
	print ("ddr_pex_drom_push_att_clpvol ()\n");
	fflush (stdout);
#   endif

#   if DEBUG
	printf ("+ Before push:\n");
	ClipList_Print (DROM.modclip_list);
	fflush (stdout);
#   endif

    if (!DROM.modclip_list)
	return;

    /* Allocate a new modeling clip list. */

    new_modclip = DDspace_Allocate (sizeof(ModClipList));

    /* Set the new fields to the previous fields. */

    new_modclip->next = DROM.modclip_list;
    new_modclip->num_planes = DROM.modclip_list->num_planes;
    new_modclip->array_size = DROM.modclip_list->array_size;
    new_modclip->planes     = DDspace_Allocate (new_modclip->array_size
						* sizeof(PEXHalfSpace));

    newplane  = new_modclip->planes;
    prevplane = DROM.modclip_list->planes;

    for (ii=0;  ii < new_modclip->num_planes;  ++ii)
    {   newplane->point.x = prevplane->point.x;
	newplane->point.y = prevplane->point.y;
	newplane->point.z = prevplane->point.z;

	newplane->vector.x = prevplane->vector.x;
	newplane->vector.y = prevplane->vector.y;
	newplane->vector.z = prevplane->vector.z;

	++newplane;  ++prevplane;
    }

    DROM.modclip_list = new_modclip;

#   if DEBUG
	printf ("+ After push:\n");
	ClipList_Print (DROM.modclip_list);
	fflush (stdout);
#   endif
}



/*****************************************************************************
**  This routine pops the current modeling clip set from the model clipping
**  stack and uses the previous set.
*****************************************************************************/

void ddr_pex_drom_pop_att_clpvol (void)
{
    register ModClipList *next;		/* Next Model Clipping Set */

#   if DEBUG_CALL
	print ("ddr_pex_drom_pop_att_clpvol ()\n");
	fflush (stdout);
#   endif

#   if DEBUG
	printf ("+ Before pop:\n");
	ClipList_Print (DROM.modclip_list);
	fflush (stdout);
#   endif

    /* Just return if there's nothing to pop. */

    if (!DROM.modclip_list) return;

    DDspace_Deallocate (DROM.modclip_list->planes);
    next = DROM.modclip_list->next;
    DDspace_Deallocate (DROM.modclip_list);
    DROM.modclip_list = next;

    if (next)
	ClipList_Set (DROM.modclip_list);
    else
	PEXSetModelClipFlag (DCM->display, PEX.renderer, PEXOCRender, PEXOff);

#   if DEBUG
	printf ("+ After pop:\n");
	ClipList_Print (DROM.modclip_list);
	fflush (stdout);
#   endif
}



/*****************************************************************************
**  This routine sets the current clip volume.
*****************************************************************************/

void  ddr_pex_drom_set_att_clpvol  (
    DtClipOperator operator,
    DtInt          halfspacecount,
    DtHalfSpace   *halfspaces)
{
    register ModClipList  *new_clipvol;	/* New Model Clip Volume */

#   if DEBUG_CALL
    {   register int ii;	/* Loop Index */
	printf ("ddr_pex_drom_set_att_clpvol (%d, %d, %lx)\n",
	    operator, halfspacecount, halfspaces);
	for (ii=0;  ii < halfspacecount;  ++ii)
	    printf ("+     halfspace %d pnt <%g %g %g> vec <%g %g %g>\n",
		ii, halfspaces[ii].point[0], halfspaces[ii].point[1],
		halfspaces[ii].point[2], halfspaces[ii].vector[0],
		halfspaces[ii].vector[1], halfspaces[ii].vector[2]);
	fflush (stdout);
    }
#   endif

    /* If there is no current clip volume, then create one. */

    if (!DROM.modclip_list)
    {   new_clipvol = DDspace_Allocate (sizeof(ModClipList));
	new_clipvol->next = 0;
	new_clipvol->array_size = halfspacecount + CHUNK_SIZE;
	new_clipvol->planes =
	    DDspace_Allocate (new_clipvol->array_size * sizeof(PEXHalfSpace));
	DROM.modclip_list = new_clipvol;
    }

    /* Zap out the previous model-clipping set. */

    DROM.modclip_list->num_planes = 0;

    /* Now add the new halfspaces to the clipping volume. */

    ddr_pex_drom_apply_att_clpvol (operator, halfspacecount, halfspaces);

#   if DEBUG
	print ("+ After set_att_clpvol:\n");
	ClipList_Print (DROM.modclip_list);
	fflush (stdout);
#   endif
}



/*****************************************************************************
**  This routine adds a list of halfspaces to the current clipping region.
*****************************************************************************/

static void ClipList_Add (
    ModClipList *clipset,		/* Clipping Region */
    int          halfspace_count,	/* Number of New Halfspaces */
    DtHalfSpace *halfspace_list)	/* List of New Halfspaces */
{
    register int           target_size;	/* Target Array Size With New Planes */
    register PEXHalfSpace *newplane;	/* New Halfspace */

    target_size = clipset->num_planes + halfspace_count;

    if (target_size >= clipset->array_size)
    {   target_size += CHUNK_SIZE;
	clipset->planes = DDspace_Reallocate
			  (clipset->planes, target_size * sizeof(PEXHalfSpace));
	clipset->array_size = target_size;
    }

    newplane = clipset->planes + clipset->num_planes;

    while (halfspace_count)
    {   newplane->point.x  = halfspace_list->point[0];
	newplane->point.y  = halfspace_list->point[1];
	newplane->point.z  = halfspace_list->point[2];

	newplane->vector.x = halfspace_list->vector[0];
	newplane->vector.y = halfspace_list->vector[1];
	newplane->vector.z = halfspace_list->vector[2];

	++halfspace_list;
	++newplane;
	++clipset->num_planes;
	--halfspace_count;
    }
}



/*****************************************************************************
**  This routine send the current clipping region to the PEX server.
*****************************************************************************/

static void ClipList_Set (ModClipList *clipset)
{
#   if DEBUG_CALL
	printf ("ClipList_Set (%lx)\n", clipset);
	fflush (stdout);
#   endif

#   if DEBUG
	ClipList_Print (clipset);
#   endif

    if (dde_pex_drom_pre_init)
	return;

    if (DROM.modclip_switch)
	PEXSetModelClipFlag (DCM->display, PEX.renderer, PEXOCRender, PEXOn);

    PEXSetModelClipVolume
    (   DCM->display, PEX.renderer, PEXOCRender,
	PEXModelClipReplace, clipset->num_planes, clipset->planes
    );
}



#if DEBUG
/*****************************************************************************
**  This routine is used for debugging and prints the contents of a given
**  clipping volume.
*****************************************************************************/

static void ClipList_Print (ModClipList *clipset)
{
    register int ii;	/* Loop Index */

    printf (":   ClipList Address %lx\n", clipset);
    if (!clipset) return;
    printf (":   Next cliplist %lx\n", clipset->next);
    printf (":   Array size %d\n", clipset->array_size);
    printf (":   num planes %d\n", clipset->num_planes);
    printf (":   planes %lx\n", clipset->planes);
    if (!clipset->planes) return;
    for (ii=0;  ii < clipset->num_planes;  ++ii)
        printf (":     plane %d pnt <%g %g %g> vec <%g %g %g>\n",
	    ii,
	    clipset->planes[ii].point.x,
	    clipset->planes[ii].point.y,
	    clipset->planes[ii].point.z,
	    clipset->planes[ii].vector.x,
	    clipset->planes[ii].vector.y,
	    clipset->planes[ii].vector.z
	);
}
#endif



/****************************  A P P E N D I X  ******************************

    The following list defines the mapping from the Dore' clipping model to
the PEX model clipping modes.  In the following list, T denotes the current
model clipping volume, and S denotes the new volume.  A prime denotes the
inverse of the clipping space (e.g. S').

                           Acceptance   PEX
      Dore' Type           Region       Support   Note
      ------------------   ----------   -------   ----
      DcClipAll            Nowhere      Faked      a
      DcClipAnd            T and S      Full
      DcClipAndReverse     T and S'     Limited    b
      DcClipNoOp           T            Full
      DcClipAndInverted    T' and S     Limited    b
      DcClipReplace        S            Full
      DcClipXOr            T xor S      None       c
      DcClipOr             T or S       None       c
      DcClipNor            T' and S'    Limited    b
      DcClipEqv            T eqv S      None       c
      DcClipInvertVolume   S'           Limited    b
      DcClipOrReverse      T or S'      None
      DcClipInvert         T'           Limited    b
      DcClipOrInverted     T' or S      None       c
      DcClipNAnd           T' or S'     None       c
      DcClipNone           Everywhere   Full

      [a]  Faked with two mutually-exclusive halfspaces.
      [b]  The inverse of a volume is supported only if the volume consists
           of a single halfspace.
      [c]  Operations that yield multiple disconnected regions (e.g. OR, XOR)
	   are not supported.

    The limited PEX support for the Dore' model-clipping model is due to the
fact that PEX supports only the intersection of halfspaces.  Because of this,
only single convex clipping volumes are supported.

    Regarding the inversion of halfspaces, inversion is only supported with
single halfspaces because the inverse of multiple halfspaces may yield
disconnected regions.  For the region

                      |                    |
                      |                    |
          clipped     |--->  accepted  <---|     clipped
          region      |       region       |     region
                      |                    |

  when inverted, yields the following two regions:

                      |                    |
                      |                    |
         accepted <---|      clipped       |---> accepted
          region      |      region        |      region
                      |                    |

  PEX supports the former volume, but not the latter.  For this reason,
  the current driver doesn't do anything if the inverse of multiple
  halfspaces is needed.

*****************************************************************************/
