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
#include <stdio.h>
#include "dore_util.h"



/*
				PARSE ARG

	This is an argument parsing utility.  It can be used to take
	values from command line argument vectors.  All the parse arg
	routines take as their first two arguments an integer argument
	count and an array of character pointers in the same form as
	the argc and argv parameters to main().  The functions are:

		prsarg_get_keyword_int
		prsarg_get_keyword_real
		prsarg_get_keyword_short
		prsarg_get_keyword_string
		prsarg_get_keyword_switch
		prsarg_check_bad_arguments

	The get_keyword functions examine the argument vector looking for
	a given string (the third argument).  If that string is found the
	following string in the vector is interpreted as the requested type
	(int, real, string, etc.) and written into the space provided (the
	fifth argument).  If the given string is not found, then the given
	default value (the fourth argument) is written instead.

	The return value of each get_keyword function is a DtFlag value
	which is either DcTrue meaning that the keyword was found, or
	DcFalse meaning the keyword was not found.

	The get_keyword functions do distructive reads of the argument
	vector.  That is, if the given keyword is found, the keyword and
	the following value string are erased from the argument vector.

	The function prsarg_check_bad_arguments can be called to check that
	all arguments in the vector were used (erased).  If all the
	arguments were not used, it prints a message to stderr and returns
	the value DcFalse.  Otherwise it returns DcTrue.
*/




DtFlag prsarg_get_keyword_int (
    int argc,
    char *argv[],
    char *keyword,
    DtInt defaultvalue,
    DtInt *argument)
{
	DtInt argno;
	int strcmp();

	for (argno=2;argno<=argc;argno++)
		{
		if (strcmp(argv[argno-1],keyword) == 0)
			{
			/*  Null out the keyword argument.  */
			strcpy(argv[argno-1],"");

			/*  Check if the keyword argument was the last.  */
			if ((argno+1)>argc)
				{
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




DtFlag prsarg_get_keyword_real (
    int argc,
    char *argv[],
    char *keyword,
    DtReal defaultvalue,
    DtReal *argument)

{
	DtInt argno;
	int strcmp();

	for (argno=2;argno<=argc;argno++)
		{
		if (strcmp(argv[argno-1],keyword) == 0)
			{
			/*  Null out the keyword argument.  */
			strcpy(argv[argno-1],"");

			/*  Check if the keyword argument was the last.  */
			if ((argno+1)>argc)
				{
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




DtFlag prsarg_get_keyword_short (
    int argc,
    char *argv[],
    char *keyword,
    DtShort defaultvalue,
    DtShort *argument)

{
	DtInt argno;
	int strcmp();
	DtInt vali32;

	for (argno=2;argno<=argc;argno++)
		{
		if (strcmp(argv[argno-1],keyword) == 0)
			{
			/*  Null out the keyword argument.  */
			strcpy(argv[argno-1],"");

			/*  Check if the keyword argument was the last.  */
			if ((argno+1)>argc)
				{
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




DtFlag prsarg_get_keyword_string (
    int argc,
    char *argv[],
    char *keyword,
    char *defaultvalue,
    char *argument)

	{
	DtInt argno;
	int strcmp();

	for (argno=2;argno<=argc;argno++)
		{
		if (strcmp(argv[argno-1],keyword) == 0)
			{
			/*  Null out the keyword argument.  */
			strcpy(argv[argno-1],"");

			/*  Check if the keyword argument was the last.  */
			if ((argno+1)>argc)
				{
				strcpy(argument,defaultvalue);
				return(DcFalse);
				}

			/*  Otherwise, we have a value argument for this key. */
			strcpy(argument,argv[argno]);
			strcpy(argv[argno],"");

			return(DcTrue);
			}
		}

	/*  No keyword argument.  */
	strcpy(argument,defaultvalue);

	return(DcFalse);
}


DtFlag prsarg_get_keyword_switch (
    int argc,
    char *argv[],
    char *keyword,
    DtSwitch defaultvalue,
    DtSwitch *argument)

{
	DtInt argno;
	int strcmp();
	DtInt vali32;

	for (argno=2;argno<=argc;argno++)
		{
		if (strcmp(argv[argno-1],keyword) == 0)
			{
			/*  Check if the keyword argument was the last.  */
			if ((argno+1)>argc)
				{
				*argument = defaultvalue;

				return(DcFalse);
				}

			/*  Null out the keyword argument.  */
			strcpy(argv[argno-1],"");

			/*  Otherwise, we have a value argument for this key. */
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




DtFlag prsarg_check_bad_arguments (
    int argc,
    char *argv[])
{
	DtInt argno;
	int strcmp();

	for (argno=2;argno<=argc;argno++)
		{
		if (strcmp(argv[argno-1],"") != 0)
			{
			fprintf(stderr, "unknown argument %s\n", argv[argno-1]);
			return(DcTrue);
			}
		}

	/*	No bad arguments.		*/

	return(DcFalse);
} /* end of prsarg.c */
