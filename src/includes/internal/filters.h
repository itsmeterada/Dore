#ifndef DORE_FILTERS_H
#define DORE_FILTERS_H
 
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

#include <dore/internal/bitfield.h>


    /***************************/
    /***  Defined Constants  ***/
    /***************************/
 
#define dod_filter_bitfields	6


    /**************************/
    /***  Type Definitions  ***/
    /**************************/

typedef dot_bitfield dot_filters[dod_filter_bitfields];


    /*****************************/
    /***  Function Prototypes  ***/
    /*****************************/

dot_filters *dor_filters_create             (void);
void         dor_filters_destroy            (dot_filters);
void         dor_filters_copy               (dot_filters, dot_filters);
DtFlag       dor_filters_test_intersections (dot_bitfield, dot_bitfield,
					     dot_bitfield);

#endif
