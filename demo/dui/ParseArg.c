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

/*******************************************************************************
 ***************  p r s a r g _ g e t _ k e y w o r d _ i n t  *****************
 *******************************************************************************
 * Module Name: prsarg_get_keyword_int
 * Module Type: c
 *     Purpose: Search the command line for a keyword field followed by an
 *		integer.
 *   Arguments: argc ==> number of command line arguments
 *		argv ==> array of command line argument strings
 *		keyword ==> character string keyword to look for -- look for 
 *		   this string immediately preceded by a '-'
 *		defaultvalue ==> default value to return if the specified 
 *		   keyword is not found
 *		argument ==> variable to hold the parsed or default value
 * ReturnValue: Set to 1 if the keyword and a corresponding value were found;
 *		otherwise set to 0.
 ******************************************************************************/
DtFlag prsarg_get_keyword_int(argc,argv,keyword,defaultvalue,argument)
int argc;
char *argv[];
char *keyword;
DtInt defaultvalue;
DtInt *argument;
{
/*******************************************************************************
 * Declare variables.
 ******************************************************************************/
	DtInt argno;
	int strcmp();

	for (argno=2;argno<=argc;argno++)
		{
		if (strcmp(argv[argno-1],keyword) == 0)
			{
			/*  Null out the keyword argument.  */

/*			strcpy(argv[argno-1],""); */

			/*  Check if the keyword argument was the last.  */

			if ((argno+1)>argc)
				{
				*argument = defaultvalue;

				return(DcFalse);
				}

			sscanf(argv[argno],"%d",argument);

			/*  Null out the value arguments.  */

/*			strcpy(argv[argno],""); */

			return(DcTrue);
			}
		}

	/*  No keyword argument.  */

	*argument = defaultvalue;

	return(DcFalse);
	}



/*******************************************************************************
 ***************  p r s a r g _ g e t _ k e y w o r d _ r e a l  ***************
 *******************************************************************************
 * Module Name: prsarg_get_keyword_real
 * Module Type: c
 *     Purpose: Search the command line for a keyword field followed by a
 *		double precision floating point number.
 *   Arguments: argc ==> number of command line arguments
 *		argv ==> array of command line argument strings
 *		keyword ==> character string keyword to look for -- look for 
 *		   this string immediately preceded by a '-'
 *		defaultvalue ==> default value to return if the specified 
 *		   keyword is not found
 *		argument ==> variable to hold the parsed or default value
 * ReturnValue: Set to 1 if the keyword and a corresponding value were found;
 *		otherwise set to 0.
 ******************************************************************************/
	DtFlag prsarg_get_keyword_real(argc,argv,keyword,defaultvalue,argument)
	int argc;
	char *argv[];
	char *keyword;
	DtReal defaultvalue;
	DtReal *argument;

	{
	DtInt argno;
	int strcmp();

	for (argno=2;argno<=argc;argno++)
		{
		if (strcmp(argv[argno-1],keyword) == 0)
			{
			/*  Null out the keyword argument.  */

/*			strcpy(argv[argno-1],""); */

			/*  Check if the keyword argument was the last.  */

			if ((argno+1)>argc)
				{
				*argument = defaultvalue;

				return(DcFalse);
				}

			sscanf(argv[argno],"%lf",argument);

			/*  Null out the value argument.  */

/*			strcpy(argv[argno],""); */

			return(DcTrue);
			}
		}

	/*  No keyword argument.  */

	*argument = defaultvalue;

	return(DcFalse);
	}



/*******************************************************************************
 *************  p r s a r g _ g e t _ k e y w o r d _ s h o r t  ***************
 *******************************************************************************
 * Module Name: prsarg_get_keyword_short
 * Module Type: c
 *     Purpose: Search the command line for a keyword field followed by a
 *		short integer.
 *   Arguments: argc ==> number of command line arguments
 *		argv ==> array of command line argument strings
 *		keyword ==> character string keyword to look for -- look for 
 *		   this string immediately preceded by a '-'
 *		defaultvalue ==> default value to return if the specified 
 *		   keyword is not found
 *		argument ==> variable to hold the parsed or default value
 * ReturnValue: Set to 1 if the keyword and a corresponding value were found;
 *		otherwise set to 0.
 ******************************************************************************/
	DtFlag prsarg_get_keyword_short(argc,argv,keyword,defaultvalue,argument)
	int argc;
	char *argv[];
	char *keyword;
	DtShort defaultvalue;
	DtShort *argument;

	{
	DtInt argno;
	int strcmp();
	DtInt vali32;

	for (argno=2;argno<=argc;argno++)
		{
		if (strcmp(argv[argno-1],keyword) == 0)
			{
			/*  Null out the keyword argument.  */

/*			strcpy(argv[argno-1],""); */

			/*  Check if the keyword argument was the last.  */

			if ((argno+1)>argc)
				{
				*argument = defaultvalue;

				return(DcFalse);
				}

			sscanf(argv[argno],"%d",&vali32);
			*argument = vali32;

			/*  Null out the value arguments.  */

/*			strcpy(argv[argno],""); */

			return(DcTrue);
			}
		}

	/*  No keyword argument.  */

	*argument = defaultvalue;

	return(DcFalse);
	}



/*******************************************************************************
 ************  p r s a r g _ g e t _ k e y w o r d _ s t r i n g  **************
 *******************************************************************************
 * Module Name: prsarg_get_keyword_string
 * Module Type: c
 *     Purpose: Search the command line for a keyword field followed by a
 *		character string.
 *   Arguments: argc ==> number of command line arguments
 *		argv ==> array of command line argument strings
 *		keyword ==> character string keyword to look for -- look for 
 *		   this string immediately preceded by a '-'
 *		defaultvalue ==> default value to return if the specified 
 *		   keyword is not found
 *		argument ==> variable to hold the parsed or default value
 * ReturnValue: Set to 1 if the keyword and a corresponding value were found;
 *		otherwise set to 0.
 ******************************************************************************/
	DtFlag prsarg_get_keyword_string(argc,argv,keyword,defaultvalue,argument)
	int argc;
	char *argv[];
	char *keyword;
	char *defaultvalue;
	char *argument;

	{
	DtInt argno;
	int strcmp();

	for (argno=2;argno<=argc;argno++)
		{
		if (strcmp(argv[argno-1],keyword) == 0)
			{
			/*  Null out the keyword argument.  */

/*			strcpy(argv[argno-1],""); */

			/*  Check if the keyword argument was the last.  */

			if ((argno+1)>argc)
				{
				strcpy(argument,defaultvalue);
				return(DcFalse);
				}

			/*  Otherwise, we have a value argument for this key.  */

			strcpy(argument,argv[argno]);

/*			strcpy(argv[argno],""); */

			return(DcTrue);
			}
		}

	/*  No keyword argument.  */

	strcpy(argument,defaultvalue);

	return(DcFalse);
	}



/*******************************************************************************
 ************  p r s a r g _ g e t _ k e y w o r d _ s w i t c h  **************
 *******************************************************************************
 * Module Name: prsarg_get_keyword_switch
 * Module Type: c
 *     Purpose: Search the command line for a keyword field followed by a
 *		0/1 type switch.
 *   Arguments: argc ==> number of command line arguments
 *		argv ==> array of command line argument strings
 *		keyword ==> character string keyword to look for -- look for 
 *		   this string immediately preceded by a '-'
 *		defaultvalue ==> default value to return if the specified 
 *		   keyword is not found
 *		argument ==> variable to hold the parsed or default value
 * ReturnValue: Set to 1 if the keyword and a corresponding value were found;
 *		otherwise set to 0.
 ******************************************************************************/
	DtFlag prsarg_get_keyword_switch(argc,argv,keyword,defaultvalue,argument)
	int argc;
	char *argv[];
	char *keyword;
	DtSwitch defaultvalue;
	DtSwitch *argument;

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

/*			strcpy(argv[argno-1],""); */

			/*  Otherwise, we have a value argument for this key.  */

			sscanf(argv[argno],"%d",&vali32);
			*argument = vali32 & 255;

/*			strcpy(argv[argno],""); */

			return(DcTrue);
			}
		}

	/*  No keyword argument.  */

	*argument = defaultvalue;

	return(DcFalse);
	}
