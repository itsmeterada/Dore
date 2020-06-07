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

extern double dt,fmode,height,phase;

user_parse(str)
char *str;
{
    if (debug)
    	printf("User_parse(%s);\n",str);

    switch(str[0]) {
    	case('W'):	/* wave */
    	    sscanf(str+1, "%lf", &dt);
    	    if (debug)
    		printf("dt: %f\n", dt);
    	    return(1);

    	case('L'):	/* Change Initial Conditions */
    	    switch(str[1]) {
    		case('M'):
    		    sscanf(str+2, "%lf", &fmode);
    		    if (debug)
    			printf("fmode: %lf\n", fmode);
    		    return(1);
    
    		case('H'):
    		    sscanf(str+2, "%lf", &height);
    		    if (debug)
    			printf("height: %lf\n", height);
    		    return(1);
    
    		case('P'):
    		    sscanf(str+2, "%lf", &phase);
    		    if (debug)
    			printf("phase: %lf\n", phase);
    		    return(1);
    	   }
    };

    return(0);
}
