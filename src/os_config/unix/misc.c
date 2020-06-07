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
 
/* ======================================================================
  Functions:
	dor_util_getenv
	dor_util_strtok

  ====================================================================== */

#include <string.h>
#include <dore/internal/dogen.h>


/* ====================================================================== */

char *dor_util_getenv (char *name)
{
    return getenv (name);
}

/* ====================================================================== */

static int isin (char c, char *s)
{
    for (; *s; s++)
	if (*s == c) return 1;
    return 0;
}


char *dor_util_strtok (char *toks, char *seps)
{
    static char *cp, *cpend;
    register char *cpin;

    if (toks) {
	cp = toks;
	cpend = cp + strlen(cp);
    } else {
	if (cp >= cpend) {
	    return(0);
	}
    }

    while (isin(*cp,seps))
	cp++;

    if (*cp == '\0') {
	return(0);
    }

    cpin = cp;

    while (*cp && (!isin(*cp,seps)))
	cp++;

    *(cp++) = '\0';

    return(cpin);
}
