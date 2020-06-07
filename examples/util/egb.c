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
 
/*
 * Aug 11, 1988  MEC  Created
 * Jan 12, 1989  MEC  Exit status 1 == EGB
 * Apr 26, 1989  DRG  Added the use of the new X call
 *
 * Discover whether the system has an expansion graphics board.
 *
 * See also: "man 7 tigr"
 */


#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/XTitan.h>

static int stat1;

void fcn()
{
stat1 = 0;
}


int
inq_expansion_board()
{
	Display *displayPtr;
	char *displayName;
	extern char *getenv();
	int screen, is_board;

	if((displayName = getenv("DISPLAY")) == NULL)
                {
                printf("inq_expansion_board: couldn't get display name\n");
                exit(1);
                }
        if((displayPtr = XOpenDisplay(displayName)) == NULL )
                {
                printf("couldn't open display\n");
                exit(1);
                }
	screen = DefaultScreen(displayPtr);

	if (VendorRelease(displayPtr) >= 211)
		{
		stat1 = 1;
		XSetErrorHandler(fcn);
		is_board = XTitanGetExpansionStatus(displayPtr, screen);
		if (!stat1) {
			is_board = -1;
			}
		XSetErrorHandler(NULL);
		}
	else
		is_board = pre_211_inq_expansion_board();

	XCloseDisplay(displayPtr);

	return(is_board);
}

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <machine/tigr.h>



/* this is how to inquire whether there is an egb on pre 2.1.1 systems */

int pre_211_inq_expansion_board( )
{
	int		fdTg;
	int		fEgb;
	char		strTigr[20];
	int		status;



	/*
	 * Open a /dev/tigr[0-9] file.
	 */
	strcpy( strTigr, "/dev/tigr#" );
	for ( strTigr[9] = '0'; strTigr[9] <= '9'; strTigr[9]++ )
	{
		fdTg = open( strTigr, O_WRONLY );
		if ( fdTg >= 0 )
			goto LOpen;
		if ( errno != EBUSY )
		{
			fprintf( stderr, "Open error %d on file %s\n",
				errno, strTigr );
			return(-1);
		}
	}

	fprintf( stderr, "All devices /dev/tigr0 to /dev/tigr9 busy\n" );
	return(-1);

 LOpen:
	;



	/*
	 * Grab status.
	 */
	if ( ioctl( fdTg, TGIOSBUS, &status ) < 0 )
	{
		fprintf( stderr, "Can't get status word on %s\n",
			strTigr );
		return(-1);
	}

	fEgb = ( status & 0x1000 ) ? 1 : 0;



	/*
	 * That's all, folks.
	 */
	close( fdTg );
	return( fEgb );
}
