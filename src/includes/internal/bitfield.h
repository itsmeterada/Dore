#ifndef BITFIELD_H
#define BITFIELD_H
 
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

    /***************************/
    /***  Defined Constants  ***/
    /***************************/

#define dod_bitfield_ints	8
#define dod_bitfield_bits	(dod_bitfield_ints * 32)


    /**************************/
    /***  Type Definitions  ***/
    /**************************/

typedef Dt32Bits dot_bitfield[dod_bitfield_ints];


    /***************************/
    /*** Function Prototypes ***/
    /***************************/

void   dor_bitfield_add               (dot_bitfield, dot_bitfield, DtInt);
void   dor_bitfield_copy              (dot_bitfield, dot_bitfield, DtInt);
DtPtr  dor_bitfield_create            (void);
void   dor_bitfield_delete            (dot_bitfield, dot_bitfield, DtInt);
void   dor_bitfield_destroy           (dot_bitfield);
void   dor_bitfield_invert            (dot_bitfield, dot_bitfield, DtInt);
void   dor_bitfield_load              (dot_bitfield,DtInt,DtInt[],DtInt,DtInt);
void   dor_bitfield_print_with_string (char*, dot_bitfield, DtInt);
DtFlag dor_bitfield_test_intersect    (dot_bitfield, dot_bitfield, DtInt);
DtFlag dor_bitfield_test_membership   (dot_bitfield, DtInt);
void   dor_bitfield_unload            (dot_bitfield, DtInt*, DtInt[]);

#endif
