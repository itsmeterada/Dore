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
	dor_bitfield_create
	dor_bitfield_destroy
	dor_bitfield_print_with_string
	dor_bitfield_add
	dor_bitfield_copy
	dor_bitfield_delete
	dor_bitfield_invert
	dor_bitfield_load
	dor_bitfield_test_intersect
	dor_bitfield_test_membership
	dor_bitfield_unload

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/bitfield.h>

/*
 ======================================================================
 */

DtPtr dor_bitfield_create (void)
{
    dot_bitfield *newspace;

    newspace = (dot_bitfield *) dor_space_allocate(sizeof(*newspace));

    return(newspace);
}
/*
 ======================================================================
 */

void dor_bitfield_destroy (dot_bitfield bitfield)
{
    dor_space_deallocate(bitfield);
}
/*
 ======================================================================
 */

void dor_bitfield_print_with_string (char *str, dot_bitfield bf, DtInt ints)
{
    int i;

    fprintf(stderr, "%s", str);
    for(i=ints-1; i>=0; --i)
	    fprintf(stderr, " %08x", bf[i]);
    fprintf(stderr, "\n");
}
/*
 ======================================================================
 */

void dor_bitfield_add (dot_bitfield addee, dot_bitfield addor, DtInt ints)
{
    register i;

    for(i=0; i<ints; i++){
	addee[i] |= addor[i];
    }
}
/*
 ======================================================================
 */

void dor_bitfield_copy (dot_bitfield to, dot_bitfield from, DtInt ints)
{
    register i;

    for(i=0; i<ints; i++){
	to[i] = from[i];
    }
}
/*
 ======================================================================
 */

void dor_bitfield_delete (
    dot_bitfield  deletee, dot_bitfield  deleteor, DtInt ints)
{
    register i;

    for(i=0; i<ints; i++){
	deletee[i] &= ( ~ deleteor[i]);
    }
}
/*
 ======================================================================
 */

void dor_bitfield_invert (
    dot_bitfield  invertee, dot_bitfield  invertor, DtInt ints)
{
    register i;

    for(i=0; i<ints; i++){
	invertee[i] ^= invertor[i];
    }
}
/*
 ======================================================================
 */

void dor_bitfield_load (
    dot_bitfield  bitfield,
    DtInt         listsize,
    DtInt         list[],
    DtInt         ints,
    DtInt         bits)
{
    DtUInt check;
    register i, word, bit_position_in_word;

    for(i=0; i<ints; i++){
	bitfield[i] = 0;	/* empty the bitfield */
    }

    for(i=0; i<listsize; i++){
	check = list[i];
	if(check >= bits){
	    DDerror (ERR_VALUE_OUT_OF_RANGE,
			"dor_bitfield_load", 
			"list entry out of range");
	    continue;
	}
	word = list[i] / 32;	/* get word where bit belongs */
	bit_position_in_word = list[i] % 32;
	bitfield[word] |= (1 <<bit_position_in_word); /* store bit */
    }
}
/*
 ======================================================================
 */

DtFlag dor_bitfield_test_intersect (
    dot_bitfield  bf1,
    dot_bitfield  bf2,
    DtInt         ints)
{
    register i;

    for(i=0; i<ints; i++)
	    if(bf1[i] & bf2[i])
		    return(DcTrue);

    return(DcFalse);
}
/*
 ======================================================================
 */

DtFlag dor_bitfield_test_membership (dot_bitfield  bitfield, DtInt member)
{
    DtInt word, offset;
    DtFlag membership;

    word = member / 32;

    offset = member % 32;

    membership = (bitfield[word] & (1 << offset))  > 0;

    return(membership);
}
/*
 ======================================================================
 */

void dor_bitfield_unload (dot_bitfield  bitfield, DtInt *n, DtInt list[])
{
    register int i, count=0;

    for(i=0; i<dod_bitfield_bits; i++){
	if(dor_bitfield_test_membership(bitfield, i)){
	    list[count++] = i;
	}
    }
    *n = count;
}
