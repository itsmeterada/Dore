#ifndef DORE_EXESET_H
#define DORE_EXESET_H
 
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


    /**************************/
    /***  Type Definitions  ***/
    /**************************/

typedef struct {
    DtShort   ints;
    DtShort   objs;
    Dt32Bits *bitset;
} dot_exeset;


    /*****************************/
    /***  Function Prototypes  ***/
    /*****************************/

void        dor_exeset_copy_exeset    (dot_exeset*, dot_exeset*);
dot_object* dor_exeset_create         (DtInt, DtInt[], DtSetOperation);
dot_exeset* dor_exeset_create_exeset  (DtInt);
void        dor_exeset_destroy        (dot_object*);
void        dor_exeset_destroy_exeset (dot_exeset*);
void        dor_exeset_execute        (dot_object*);
void        dor_exeset_initialize     (void);
void        dor_exeset_print          (dot_object*);

#endif
