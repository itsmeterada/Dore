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
	dor_exeset_create_exeset
	dor_exeset_destroy_exeset
	dor_exeset_copy_exeset

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/exeset.h>


/*
 ======================================================================
 */

dot_exeset *dor_exeset_create_exeset (
    DtInt ntypes)
{
    dot_exeset *newspace;
    int ints_needed;


    newspace = (dot_exeset *)
	    dor_space_allocate(sizeof(*newspace));

    newspace->ints   = ints_needed = ntypes/32 + (ntypes%32 != 0);
    newspace->objs   = ntypes;
    newspace->bitset = dor_space_allocate(sizeof(Dt32Bits)*ints_needed);

    return(newspace);
}
/*
 ======================================================================
 */

void dor_exeset_destroy_exeset (
    dot_exeset *exeset)
{
    dor_space_deallocate (exeset->bitset);
    dor_space_deallocate (exeset);
}
/*
 ======================================================================
 */

/*  Makes one exeset into a copy of another. If the number of ints in the 
    bitsets differ, then the recieving exeset is given a new bitset of the 
    proper size. The bit count and bitset fields are then copied. */


void dor_exeset_copy_exeset (
    dot_exeset *to,
    dot_exeset *from)
{
    int i;

    if((to->ints != from->ints) || (to->bitset == (Dt32Bits *)DcNullPtr)){
	if(to->bitset != (Dt32Bits *)DcNullPtr)
		dor_space_deallocate(to->bitset);
	to->bitset = (Dt32Bits *)
		dor_space_allocate(sizeof(Dt32Bits)*from->ints);
	to->ints = from->ints;
    }

    to->objs = from->objs;
    for(i=0; i<from->ints; i++)
	    to->bitset[i] = from->bitset[i];
}
