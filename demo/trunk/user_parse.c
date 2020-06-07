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

extern DtInt debug;

extern float XBend, YBend, ZBend, Speed;

static float bcr, bcg, bcb;

user_parse(str)
char *str;
{
	float val;
	int ival;
	char cmd[80];

	if (debug)
		printf("User_parse(%s);\n",str);

	switch(str[0]) {
		case('b'):
			if (str[1] != 'c')
				return(0);
			sscanf(str+3, "%f", &val);
			switch(str[2]) {
				case('r'): bcr = val; goto bc;
				case('g'): bcg = val; goto bc;
				case('b'): bcb = val; goto bc;
			}
			return(0);
bc:			
			sprintf(cmd, "b %f,%f,%f", bcr, bcg, bcb);
			default_parse(cmd);
			return(1);

		case('U'):
			sscanf(str+2, "%f", &val);
			switch(str[1]) {
				case('x'):
					XBend = val;
					return(1);
				case('y'):
					YBend = val;
					return(1);
				case('z'):
					ZBend = val;
					return(1);
				case('s'):
					Speed = val;
					return(1);
			}
			return(1);
	}

	return(0);
}

