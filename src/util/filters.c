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
	dor_filters_copy
	dor_filters_create
	dor_filters_destroy
	dor_filters_test_intersections

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/filters.h>

/*
 ======================================================================
 */

dot_filters *dor_filters_create (void)
{
    return dor_space_allocate (sizeof(dot_filters));
}
/*
 ======================================================================
 */

void dor_filters_destroy (
    dot_filters filters)
{
    dor_space_deallocate (filters);
}
/*
 ======================================================================
 */

void dor_filters_copy (
    dot_filters to,
    dot_filters from)
{
    register int ii;

    for (ii=0;  ii < dod_filter_bitfields;  ++ii)
	dor_bitfield_copy (to[ii], from[ii], dod_bitfield_ints);
}
/*
 ======================================================================
 */

DtFlag dor_filters_test_intersections (
    dot_bitfield nameset,
    dot_bitfield inclusion,
    dot_bitfield exclusion)
{
    return
    (     dor_bitfield_test_intersect (nameset, inclusion, dod_bitfield_ints)
      && !dor_bitfield_test_intersect (nameset, exclusion, dod_bitfield_ints)
    );
}
