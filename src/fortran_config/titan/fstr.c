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
 
int fstr (s, sn, fp, fn)
    char **s;
    int sn, fn;
    struct {
	char *fcp;
	int   fcn;
    } *fp;
{
    int nn;
    char *p;

    /*
    printf("fstr(%x,%d,%x,%d)\n", s, sn, fp, fn);
    printf("%x,%d\n", fp->fcp, fp->fcn);
    printf("'%s'\n", fp->fcp);
    */

    p = *s;
    if(fn >= (nn = sn)) {
	nn = fn + 10;
	if(!sn)
	    p = (char *)malloc(nn);
	else
	    p = (char *)realloc(p, nn);
    }
    strncpy(p, fp->fcp, fn);
    p[fn] = 0;
    *s = p;
    return nn;
}
