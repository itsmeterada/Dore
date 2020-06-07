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
 
#include <dore/dore.h>
#include "dore_util.h"


/* converts a color in the HSV color model to RGB.               */
/* See Alvy Ray Smith, Color Gamut Transform Pairs, SIGGRAPH '78 */

void 
hsv_to_rgb (     /*...hexcone model...*/
    DtReal hsv[3],
    DtReal rgb[3]   /* all variables in range [0,1] */
)
{
	register int i;
	register double f, h, s, v;

	h = hsv[0];
	s = hsv[1];
	v = hsv[2];
	h *= 6;
	i = (int) h;
	f = h-i;
	if (!(i&1)) f = 1-f;
	rgb[(i+1)%6/2] = v;		/* greatest color */
	rgb[(i+4)%6/2] = v*(1-s);	/* least color */
	rgb[(7-i)%3]   = v*(1-s*f);	/* middle color */
}
