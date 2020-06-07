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
	DtPtr dor_space_allocate   (size)
	DtPtr dor_space_reallocate (in_ptr, size)
	void  dor_space_deallocate (spaceptr)
======================================================================
*/

#include <errno.h>
#include <dore/internal/dogen.h>


    /***  Statistics & Debug Macros  ***/

#define DEBUG       0		/* General Debugging Information */
#define DEBUG_CALL  0		/* Print Function Calls */

#define STATS 0

#if STATS
#   define INCR_CALL_COUNT(s)   ++(s)
#else
#   define INCR_CALL_COUNT(s)	/* No Op */
#endif


    /***  Global Variables  ***/

#if STATS
    unsigned long stat_allocate   = 0,
		  stat_reallocate = 0,
		  stat_deallocate = 0;
#endif



/*****************************************************************************
**  This function prints the tallies of the number of times each memory
**  function has been called so far.  All counters are zeroed after this
**  function is called.
*****************************************************************************/
#if STATS
void dor_space_stats (void)
{
    fputs  ("=== MEMORY STATISTICS ===\n", stdout);
    printf ("%10d  allocates\n", stat_allocate);
    printf ("%10d  reallocates\n", stat_reallocate);
    printf ("%10d  deallocates\n", stat_deallocate);
    fputs  ("=========================\n\n", stdout);

    stat_allocate = stat_reallocate = stat_deallocate = 0;
}
#endif



/*****************************************************************************
**  This routine allocates a memory block of the size specified in the 'size'
**  parameter.  The memory block can be de-allocated (freed) by calling
**  dor_space_deallocate.
*****************************************************************************/

DtPtr dor_space_allocate (unsigned size)
{
    register DtPtr ptr;		/* Address of New Memory Block */

    INCR_CALL_COUNT (stat_allocate);

#   if DEBUG_CALL
	printf ("dor_space_allocate:  size %u\n", size);
#   endif

    if (size < 1) return NULL;

    ptr = malloc (size);
    if (ptr)
    {
#       if DEBUG_CALL
	    printf ("dor_space_allocate:  returned %lx\n", ptr);
#       endif
	return ptr;
    }

    sprintf (DDerror_GetLine(), "%d",size);
    DDerror (ERR_WARN_ALLOC_FAILED, "dor_space_allocate", DDerror_GetLine());

#   if DEBUG
	sprintf (dor_print_get_line(), "ERRNO %d, sbrk(0) = %x",
		 errno, sbrk(0));
	dor_print_output (dor_print_get_line());
#   endif

    return NULL;
}



/*****************************************************************************
**  This routine returns a memory block of the specified size.  If the
**  'in_ptr' is nil, then a new memory block is allocated.  If 'in_ptr' points
**  to an existing memory block, then the old memory block will be resized.
**  In both cases, the address of the new memory block is returned if the
**  allocation was successful.  On failure, this function returns nil.  The
**  memory block can later be deallocated with dor_space_deallocate().
*****************************************************************************/

DtPtr dor_space_reallocate (
    DtPtr    in_ptr,	/* Old Memory Block, or nil */
    unsigned size)	/* Size of New Memory Block */
{
    register DtPtr out_ptr;	/* Address of New Memory Block */

    INCR_CALL_COUNT (stat_reallocate);

#   if DEBUG_CALL
	printf ("dor_space_reallocate:  0x%lx, new size %u\n", in_ptr, size);
#   endif

    if (size < 1) return NULL;

    if (in_ptr) out_ptr = realloc (in_ptr, size);
    else 	out_ptr = malloc (size);

    if (out_ptr)
    {
#       if DEBUG_CALL
	    printf ("dor_space_reallocate:  returned %lx\n", out_ptr);
#       endif

	return out_ptr;
    }

    sprintf (DDerror_GetLine(), "%d", size);
    DDerror (ERR_WARN_ALLOC_FAILED, "dor_space_reallocate", DDerror_GetLine());

#   if DEBUG
	sprintf (dor_print_get_line(), "ERRNO %d, sbrk(0) = %x", errno,sbrk(0));
	dor_print_output (dor_print_get_line());
#   endif

    return NULL;
}



/*****************************************************************************
**  This routine deallocates a block of memory storage that was previously
**  allocated with either dor_space_allocate or dor_space_reallocate.
*****************************************************************************/

void dor_space_deallocate (DtPtr spaceptr)
{
    INCR_CALL_COUNT (stat_deallocate);

#   if DEBUG_CALL
	printf ("dor_space_deallocate:  free(0x%lx)\n", spaceptr);
#   endif

    if (spaceptr) free (spaceptr);
}
