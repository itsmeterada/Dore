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
 
#include <math.h>
#include <dore/dore.h>

extern DtInt debug;
extern int corner1,corner2;
extern double windx,windz;

static double dir=90./57.2958;
static double vel=1.;

user_parse(str)
char *str;
{
	if (debug)
		printf("User_parse(%s);\n",str);

	switch (*str) {

	case 'f': /* release flag */
		corner1 = DcFalse;
		corner2 = DcFalse;
		return(1);

	case '1': /* release corner1 */
		corner1 = DcFalse;
		return(1);

	case '2': /* release corner2 */
		corner2 = DcFalse;
		return(1);

	case 'W': /* Wind velocity. */
		sscanf(str+1,"%lf",&vel);
		calc_wind();
		return(1);

	case '*': /* Wind velocity. */
		sscanf(str+1,"%lf",&dir);
		dir /= 57.2958;
		calc_wind();
		return(1);
	}

	return(0);
}

calc_wind()
{
	windx = sin(dir)*vel;
	windz = -cos(dir)*vel;
}

