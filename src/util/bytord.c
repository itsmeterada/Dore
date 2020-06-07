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
	DtFlag dor_util_bigendian_word_byte_order ()

  ======================================================================
 */
#include <dore/internal/dogen.h>

/* ====================================================================== */

/*
 * Returns DcTrue if machine is big-endian 
 *         DcFalse if machine is little-endian
 *         -1 if unknown
 */

DtFlag dor_util_bigendian_word_byte_order (void)
{
#define BYTES_TO_CHK 4
    typedef unsigned int integer;
    integer i;
    char c[16];
    integer _BIG_ENDIAN    = 0x3c5aa5c3;
    integer _LITTLE_ENDIAN = 0xc3a55a3c;
    int mask;
    int j;
    static DtFlag know_endian = DcFalse;
    static DtFlag return_flag;

    if (know_endian) 
        return(return_flag);

    c[0] = 0x3c;
    c[1] = 0x5a;
    c[2] = 0xa5;
    c[3] = 0xc3;
    for (j=BYTES_TO_CHK; j<16; j++) c[j] = 0x0;

    bcopy (c, &i, sizeof(integer));

#ifdef DEBUG
    printf ("sizeof (integer) = %d\n", sizeof(integer));
    printf ("i = 0x%x\n", i);
#endif

    if (sizeof(integer) < BYTES_TO_CHK) {
	_BIG_ENDIAN = _BIG_ENDIAN >> (BYTES_TO_CHK - sizeof(integer)) * 8; 
	for (j = 0, mask = 0x0; j < (BYTES_TO_CHK - sizeof(integer)); j++) {
	    mask = (mask << 8) | 0xff;
	}
	_LITTLE_ENDIAN = _LITTLE_ENDIAN & mask;
    } else if (sizeof(integer) > BYTES_TO_CHK) {
	_BIG_ENDIAN = _BIG_ENDIAN << (sizeof(integer) - BYTES_TO_CHK) * 8;
    }
#ifdef DEBUG
    printf ("_BIG_ENDIAN    = 0x%x\n", _BIG_ENDIAN);
    printf ("_LITTLE_ENDIAN = 0x%x\n", _LITTLE_ENDIAN);
#endif

    know_endian = DcTrue;
    if (i == _BIG_ENDIAN) {
        return_flag = DcTrue;
    } else if (i == _LITTLE_ENDIAN) {
        return_flag = DcFalse;
    } else {
        return_flag = -1;
    }
    return(return_flag);
}
