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
#include <stdio.h>
#include "dore.h"

#define false  0
#define true   1

char prsarg_print_help = false;


/*
========================================================================
==  This function scans the argument list and looks for a "-help"
==  switch.  If the switch is present, the function prints a usage
==  message and returns true, otherwise it returns false.
========================================================================
*/
char prsarg_check_help_argument (argc, argv)
    int   argc;
    char *argv[];
{
    long argno;

    for (argno=2;  argno<=argc;  argno++) {
	if (strcmp(argv[argno-1], "-help") == 0) {

	    /*  Null out the keyword argument.  */
	    strcpy(argv[argno-1],"");

	    prsarg_print_help = true;
	    fprintf (stderr, "usage: %s\n", argv[0]);

	    return true;
	}
    }
    return false;
}



/*
========================================================================
==  This function looks to see if any of the command-line arguments
==  have not been nulled out.  If so, it prints an error message and
==  returns true, otherwise it returns false.
========================================================================
*/
char prsarg_check_bad_arguments (argc, argv)
    int   argc;
    char *argv[];
{
    long argno;

    if (prsarg_print_help) exit(0);

    for (argno=2;  argno<=argc;  argno++)
    {   if (strcmp(argv[argno-1],"") != 0)
	{   fprintf (stderr,
		"prsarg_check_bad_arguments: WARNING unknown argument %s\n",
		argv[argno-1]
            );
	    return true;
	}
    }

    /*	No bad arguments. */

    return false;
}



/*
========================================================================
==  This routine scans the command-line arguments and looks for the
==  keyword that matches the `keyword' parameter.  If the keyword is
==  found, the argv[] entry is nulled out, the array `color' is set
==  to the color given on the command line and the function returns 1.
==  If the keyword is not found on the command line, the color is set to
==  the default color and the function returns 0.
========================================================================
*/
char prsarg_get_keyword_colorRGB (argc, argv, keyword, default_color, color)
    int     argc;
    char   *argv[];
    char   *keyword;
    double  default_color[3];
    double  color[3];
{
    long   argno;
    double val;

    if (prsarg_print_help) {
	fprintf (stderr, "\t%s %f %f %f\n", keyword, default_color[0], 
		default_color[1], default_color[2]);
    }

    for (argno=2;  argno<=argc;  argno++) {
	if (strcmp(argv[argno-1],keyword) == 0) {
	    /*  Null out the keyword argument.  */

	    strcpy(argv[argno-1],"");

	    /*  Check if the keyword argument was the last.  */

	    if ((argno+1)>argc)  {
		color[0] = default_color[0];
		color[1] = default_color[1];
		color[2] = default_color[2];
		return false;
	    }

	    sscanf(argv[argno+0],"%lf",&val);  color[0] = val;
	    sscanf(argv[argno+1],"%lf",&val);  color[1] = val;
	    sscanf(argv[argno+2],"%lf",&val);  color[2] = val;

	    /*  Null out the value arguments.  */

	    strcpy(argv[argno+0],"");
	    strcpy(argv[argno+1],"");
	    strcpy(argv[argno+2],"");

	    return true;
	}
    }

    /*  No keyword argument.  */

    color[0] = default_color[0];
    color[1] = default_color[1];
    color[2] = default_color[2];

    return false;
}



/*
========================================================================
==  This function looks for an argument on the command line that takes
==  an integer argument.  The `keyword' parameter defines the keyword
==  string to look for.  If the keyword is found, the argv[] entry is
==  nulled out, the `argument' parameter is set to the command-line
==  value, and the function returns 1.  If the keyword is not found on
==  the command line, then the function sets `argument' to the default
==  value and returns 0.
========================================================================
*/
char prsarg_get_keyword_int (argc, argv, keyword, defaultvalue, argument)
    int   argc;
    char *argv[];
    char *keyword;
    int   defaultvalue;
    int  *argument;
{
    long argno;

    if (prsarg_print_help) {
	fprintf (stderr, "\t%s %d\n", keyword, defaultvalue);
    }

    for (argno=2;argno<=argc;argno++) {
	if (strcmp(argv[argno-1],keyword) == 0) {
	    /*  Null out the keyword argument.  */

	    strcpy(argv[argno-1],"");

	    /*  Check if the keyword argument was the last.  */

	    if ((argno+1)>argc) {
		*argument = defaultvalue;

		return false;
	    }

	    sscanf(argv[argno],"%d",argument);

	    /*  Null out the value arguments.  */

	    strcpy(argv[argno],"");

	    return true;
	}
    }

    /*  No keyword argument.  */

    *argument = defaultvalue;

    return false;
}



/*
========================================================================
==  This function looks for an argument on the command line that takes
==  a long integer argument.  The `keyword' parameter defines the keyword
==  string to look for.  If the keyword is found, the argv[] entry is
==  nulled out, the `argument' parameter is set to the command-line
==  value, and the function returns 1.  If the keyword is not found on
==  the command line, then the function sets `argument' to the default
==  value and returns 0.
========================================================================
*/
char prsarg_get_keyword_long (argc, argv, keyword, defaultvalue, argument)
    int   argc;
    char *argv[];
    char *keyword;
    long  defaultvalue;
    long *argument;
{
    register int argno;

    if (prsarg_print_help) {
	fprintf (stderr, "\t%s %ld\n", keyword, defaultvalue);
    }

    for (argno=2;  argno <= argc;  ++argno) {
	if (0 == strcmp(argv[argno-1],keyword)) {
	    /*  Null out the keyword argument.  */

	    argv[argno-1][0] = 0;

	    /*  Check if the keyword argument was the last.  */

	    if ((argno+1) > argc) {
		*argument = defaultvalue;
		return false;
	    }

	    sscanf (argv[argno], "%ld", argument);

	    /*  Null out the value arguments.  */

	    argv[argno][0] = 0;

	    return true;
	}
    }

    /*  No keyword argument.  */

    *argument = defaultvalue;

    return false;
}



/*
=========================================================================
==  This function scans the command-line arguments for the switch that
==  matches `keyword'.  If the keyword is found, the function nulls out
==  the argv[] entry, sets `argument' to the value on the command line,
==  and returns 1.  If the keyword is not found on the command line, then
==  the `defaultvalue' is assigned to `argument' and the function returns
==  zero.
=========================================================================
*/
char prsarg_get_keyword_real (argc, argv, keyword, defaultvalue, argument)
    int     argc;
    char   *argv[];
    char   *keyword;
    double  defaultvalue;
    double *argument;
{
    long   argno;
    double val;

    if (prsarg_print_help) {
	fprintf (stderr, "\t%s %f\n", keyword, defaultvalue);
    }

    for (argno=2;argno<=argc;argno++) {
	if (strcmp(argv[argno-1],keyword) == 0) {
	    /*  Null out the keyword argument.  */

	    strcpy(argv[argno-1],"");

	    /*  Check if the keyword argument was the last.  */

	    if ((argno+1)>argc) {
		*argument = defaultvalue;

		return false;
	    }

	    sscanf(argv[argno],"%lf",&val);
	    *argument = val;

	    /*  Null out the value argument.  */

	    strcpy(argv[argno],"");

	    return true;
	}
    }

    /*  No keyword argument.  */

    *argument = defaultvalue;

    return false;
}



/*
========================================================================
==  This subroutine scans the command-line argument list for the first
==  word that matches `keyword'.  If the keyword is found, the function
==  assigns the next four real numbers on the command line to argument0
==  through argument4, nulls out the command-line arguments, and returns
==  1.  If the keyword is not found on the command line, then the
==  function assigns the default values to the arguments and returns 0.
========================================================================
*/
char prsarg_get_keyword_realnatural_4 (argc, argv, keyword,
				       default0, default1, default2, default3,
				       argument0,argument1,argument2,argument3)
    int     argc;
    char   *argv[];
    char   *keyword;
    double  default0, default1, default2, default3;
    double *argument0, *argument1, *argument2, *argument3;
{
    long   argno;
    double val;

    if (prsarg_print_help) {
	fprintf (stderr, "\t%s %f %f %f %f\n",
		keyword, default0, default1, default2, default3);
    }

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

		return false;
	    }

	    sscanf(argv[argno+0],"%lf",&val);  *argument0 = val;
	    sscanf(argv[argno+1],"%lf",&val);  *argument1 = val;
	    sscanf(argv[argno+2],"%lf",&val);  *argument2 = val;
	    sscanf(argv[argno+3],"%lf",&val);  *argument3 = val;

	    /*  Null out the value arguments.  */

	    strcpy(argv[argno+0],"");
	    strcpy(argv[argno+1],"");
	    strcpy(argv[argno+2],"");
	    strcpy(argv[argno+3],"");

	    return true;
	}
    }

    /*  No keyword argument.  */

    *argument0 = default0;
    *argument1 = default1;
    *argument2 = default2;
    *argument3 = default3;

    return false;
}



/*
========================================================================
==  This function looks for an argument on the command line that takes
==  a short integer argument.  The `keyword' parameter defines the
==  keyword string to look for.  If the keyword is found, the argv[]
==  entry is nulled out, the `argument' parameter is set to the
==  command-line value, and the function returns 1.  If the keyword is
==  not found on the command line, then the function sets `argument' to
==  the default value and returns 0.
========================================================================
*/
char prsarg_get_keyword_short (argc, argv, keyword, defaultvalue, argument)
    int    argc;
    char  *argv[];
    char  *keyword;
    short  defaultvalue;
    short *argument;
{
    long argno;
    int  value;

    if (prsarg_print_help) {
	fprintf (stderr, "\t%s %d\n", keyword, defaultvalue);
    }

    for (argno=2;  argno <= argc;  argno++) {
	if (strcmp(argv[argno-1],keyword) == 0) {
	    /*  Null out the keyword argument.  */

	    strcpy (argv[argno-1],"");

	    /*  Check if the keyword argument was the last.  */

	    if ((argno+1)>argc) {
		*argument = defaultvalue;

		return false;
	    }

	    sscanf (argv[argno], "%d", &value);
	    *argument = value;

	    /*  Null out the value arguments.  */

	    argv[argno][0] = 0;

	    return true;
	}
    }

    /*  No keyword argument.  */

    *argument = defaultvalue;

    return false;
}



/*
========================================================================
==  This routine scans the command-line arguments and looks for the
==  keyword that matches the `keyword' parameter.  If the keyword is
==  found, the argv[] entry is nulled out, the string `argument' is set
==  to the string given on the command line and the function returns 1.
==  If the keyword is not found on the command line, the string is set
==  to the default value and the function returns 0.
========================================================================
*/
char prsarg_get_keyword_string (argc, argv, keyword, defaultvalue, argument)
    int   argc;
    char *argv[];
    char *keyword;
    char *defaultvalue;
    char *argument;
{
    long argno;

    if (prsarg_print_help) {
	fprintf (stderr, "\t%s \"%s\"\n", keyword, defaultvalue);
    }

    for (argno=2;argno<=argc;argno++) {
	if (strcmp(argv[argno-1],keyword) == 0) {
	    /*  Null out the keyword argument.  */

	    strcpy(argv[argno-1],"");

	    /*  Check if the keyword argument was the last.  */

	    if ((argno+1)>argc) {
		strcpy(argument,defaultvalue);
		return false;
	    }

	    /*  Otherwise, we have a value argument for this key.  */

	    strcpy(argument,argv[argno]);

	    strcpy(argv[argno],"");

	    return true;
	}
    }

    /*  No keyword argument.  */

    strcpy(argument,defaultvalue);

    return false;
}



/*
========================================================================
==  This routine scans the command-line arguments and looks for the
==  keyword that matches the `keyword' parameter.  If the keyword is
==  found, the argv[] entry is nulled out, the character `argument' is
==  set to the character given on the command line and the function
==  returns 1.  If the keyword is not found on the command line, the
==  character is set to the default value and the function returns 0.
========================================================================
*/
char prsarg_get_keyword_switch (argc, argv, keyword, defaultvalue, argument)
    int   argc;
    char *argv[];
    char *keyword;
    char  defaultvalue;
    char *argument;
{
    long argno;
    int  value;

    if (prsarg_print_help) {
	fprintf (stderr, "\t%s %d\n", keyword, defaultvalue);
    }

    for (argno=2;argno<=argc;argno++) {
	if (strcmp(argv[argno-1],keyword) == 0) {
	    /*  Check if the keyword argument was the last.  */

	    if ((argno+1)>argc) {
		*argument = defaultvalue;

		return false;
	    }

	    /*  Null out the keyword argument.  */

	    strcpy(argv[argno-1],"");

	    /*  Otherwise, we have a value argument for this key.  */

	    sscanf (argv[argno], "%d", &value);
	    *argument = value;

	    argv[argno][0] = 0;

	    return true;
	}
    }

    /*  No keyword argument.  */

    *argument = defaultvalue;

    return false;
}



/*
========================================================================
==  This routine scans the command-line arguments and looks for the
==  keyword that matches the `keyword' parameter.  If the keyword is
==  found, the argv[] entry is nulled out, the flag `argument' is set
==  to the value of !`defaultvalue', and the function returns 1.  If the
==  keyword is not found on the command line, the character is set to
the default value and the function returns 0.
========================================================================
*/
char prsarg_get_keyword_flag (argc, argv, keyword, defaultvalue, argument)
    int   argc;
    char *argv[];
    char *keyword;
    char  defaultvalue;
    char *argument;
{
    long argno;

    if (prsarg_print_help) {
	fprintf (stderr, "\t[ %s ]\n",keyword);
    }

    for (argno=2;argno<=argc;argno++) {
	if (strcmp(argv[argno-1],keyword) == 0) {
	    /*  Null out the keyword argument.  */

	    strcpy(argv[argno-1],"");

	    /*  Otherwise, we have a value argument for this key.  */

	    *argument = !defaultvalue;

	    return true;
	}
    }

    /*  No keyword argument.  */

    *argument = defaultvalue;

    return false;
}
