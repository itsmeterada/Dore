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
 
#include <stdlib.h>
#include <string.h>
#include "dore/fortran.h"

int fstr (
    char **s,
    int    sn,
    char  *fcp,
    int    fn)
{
    int nn;
    char *p;

    /* printf("fstr(%x,%d,%x,%d)\n", s, sn, fp, fn); */

    p = *s;
    if(fn >= (nn = sn)) {
	nn = fn + 10;
	if(!sn)
	    p = (char *)malloc(nn);
	else
	    p = (char *)realloc(p, nn);
    }
    strncpy(p, fcp, fn);
    p[fn] = 0;
    *s = p;
    return nn;
}
