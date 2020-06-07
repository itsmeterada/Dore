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
 
/*  ======================================================================
    This file contains the routines used to parse the command line options 
    passed to dynamic.c and production.c.
    ====================================================================== */

#include <dore/dore.h>
#include <stdio.h>

/*************************************************************************/

DtFlag 
prsarg_check_bad_arguments(argc,argv)
int argc;
char *argv[];
{
    DtInt argno;
    int strcmp();

    for (argno=2;argno<=argc;argno++) {
	if (strcmp(argv[argno-1],"") != 0) {
	    fprintf(stderr, "unknown argument %s\n", argv[argno-1]);
	    return(DcTrue);
	}
    }

    /*	No bad arguments. */

    return(DcFalse);
}

/*************************************************************************/

DtFlag 
prsarg_get_keyword_int(argc,argv,keyword,defaultvalue,argument)
int argc;
char *argv[];
char *keyword;
DtInt defaultvalue;
DtInt *argument;
{
    DtInt argno;
    int strcmp();

    for (argno=2;argno<=argc;argno++) {
	if (strcmp(argv[argno-1],keyword) == 0) {

	    /*  Null out the keyword argument.  */
	    strcpy(argv[argno-1],"");

	    /*  Check if the keyword argument was the last.  */
	    if ((argno+1)>argc) {
		*argument = defaultvalue;
		return(DcFalse);
	    }

	    sscanf(argv[argno],"%d",argument);

	    /*  Null out the value arguments.  */
	    strcpy(argv[argno],"");

	    return(DcTrue);
	}
    }

    /*  No keyword argument.  */
    *argument = defaultvalue;

    return(DcFalse);
}

/*************************************************************************/

DtFlag 
prsarg_get_keyword_real(argc,argv,keyword,defaultvalue,argument)
int argc;
char *argv[];
char *keyword;
DtReal defaultvalue;
DtReal *argument;
{
    DtInt argno;
    int strcmp();

    for (argno=2;argno<=argc;argno++) {
	if (strcmp(argv[argno-1],keyword) == 0) {

	    /*  Null out the keyword argument.  */
	    strcpy(argv[argno-1],"");

	    /*  Check if the keyword argument was the last.  */
	    if ((argno+1)>argc) {
		*argument = defaultvalue;
		return(DcFalse);
	    }

	    sscanf(argv[argno],"%lf",argument);

	    /*  Null out the value argument.  */
	    strcpy(argv[argno],"");
	    return(DcTrue);
	}
    }

    /*  No keyword argument.  */
    *argument = defaultvalue;

    return(DcFalse);
}

/*************************************************************************/

DtFlag 
prsarg_get_keyword_realnatural_4(argc,argv,keyword,
		default0,default1,default2,default3,
		argument0,argument1,argument2,argument3)
int argc;
char *argv[];
char *keyword;
DtReal default0, default1, default2, default3;
DtReal *argument0, *argument1, *argument2, *argument3;
{
    DtInt argno;
    int strcmp();

    for (argno=2;argno<=argc;argno++) {
	if (strcmp(argv[argno-1],keyword) == 0) {

	    /*  Null out the keyword argument.  */
	    strcpy(argv[argno-1],"");

	    /*  Check if the keyword argument was the last.  */
	    if ((argno+1)>argc) {
		*argument0 = default0;
		*argument1 = default1;
		*argument2 = default2;
		*argument3 = default3;
		return(DcFalse);
	    }

	    sscanf(argv[argno],"%lf",argument0);
	    sscanf(argv[argno+1],"%lf",argument1);
	    sscanf(argv[argno+2],"%lf",argument2);
	    sscanf(argv[argno+3],"%lf",argument3);

	    /*  Null out the value arguments.  */
	    strcpy(argv[argno],"");
	    strcpy(argv[argno+1],"");
	    strcpy(argv[argno+2],"");
	    strcpy(argv[argno+3],"");

	    return(DcTrue);
	}
    }

    /*  No keyword argument.  */
    *argument0 = default0;
    *argument1 = default1;
    *argument2 = default2;
    *argument3 = default3;

    return(DcFalse);
}

/*************************************************************************/

DtFlag 
prsarg_get_keyword_short(argc,argv,keyword,defaultvalue,argument)
int argc;
char *argv[];
char *keyword;
DtShort defaultvalue;
DtShort *argument;
{
    DtInt argno;
    int strcmp();
    DtInt vali32;

    for (argno=2;argno<=argc;argno++) {
	if (strcmp(argv[argno-1],keyword) == 0) {

	    /*  Null out the keyword argument.  */
	    strcpy(argv[argno-1],"");

	    /*  Check if the keyword argument was the last.  */
	    if ((argno+1)>argc) {
		*argument = defaultvalue;
		return(DcFalse);
	    }

	    sscanf(argv[argno],"%d",&vali32);
	    *argument = vali32;

	    /*  Null out the value arguments.  */
	    strcpy(argv[argno],"");

	    return(DcTrue);
	}
    }

    /*  No keyword argument.  */
    *argument = defaultvalue;

    return(DcFalse);
}

/************************************************************************/

DtFlag 
prsarg_get_keyword_string(argc,argv,keyword,defaultvalue,argument)
int argc;
char *argv[];
char *keyword;
char *defaultvalue;
char *argument;
{
    DtInt argno;
    int strcmp();

    for (argno=2;argno<=argc;argno++) {
	if (strcmp(argv[argno-1],keyword) == 0) {

	    /*  Null out the keyword argument.  */
	    strcpy(argv[argno-1],"");

	    /*  Check if the keyword argument was the last.  */
	    if ((argno+1)>argc) {
		strcpy(argument,defaultvalue);
		return(DcFalse);
	    }

	    /*  Otherwise, we have a value argument for this key.  */
	    strcpy(argument,argv[argno]);
	    strcpy(argv[argno],"");

	    return(DcTrue);
	}
    }

    /*  No keyword argument.  */
    strcpy(argument,defaultvalue);

    return(DcFalse);
}

/*************************************************************************/

DtFlag 
prsarg_get_keyword_switch(argc,argv,keyword,defaultvalue,argument)
int argc;
char *argv[];
char *keyword;
DtSwitch defaultvalue;
DtSwitch *argument;
{
    DtInt argno;
    int strcmp();
    DtInt vali32;

    for (argno=2;argno<=argc;argno++) {
	if (strcmp(argv[argno-1],keyword) == 0) {

	    /*  Check if the keyword argument was the last.  */
	    if ((argno+1)>argc) {
		*argument = defaultvalue;

		return(DcFalse);
	    }

	    /*  Null out the keyword argument.  */
	    strcpy(argv[argno-1],"");

	    /*  Otherwise, we have a value argument for this key.  */
	    sscanf(argv[argno],"%d",&vali32);
	    *argument = vali32 & 255;
	    strcpy(argv[argno],"");

	    return(DcTrue);
	}
    }

    /*  No keyword argument.  */
    *argument = defaultvalue;

    return(DcFalse);
}
