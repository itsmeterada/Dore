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

main()
{
    extern error_handler();
    DtObject dev;
    DtPtr version;
    DtInt default_fd;
    DtPFI default_errhandler;

    DsInitialize(0);

    DsInqVersion (&version);
    printf ("Version %s\n", version);

    DsInqErrorVars(&default_fd, &default_errhandler);

    printf ("Error from DoDevice - using standard error handler\n");
    dev = DoDevice("junk", " ");

    printf ("Installing C error handler\n");
    DsSetErrorVars(2, error_handler);

    printf ("Error from DoDevice - using C Error handler\n");
    dev = DoDevice("junk", " ");

    printf ("Resetting Error handler to default\n");
    DsSetErrorVars(default_fd, DcNullPtr);

    printf ("Error from DdUpdate\n");
    DdUpdate(dev);

    DsTerminate();
    exit(0);
}

/*
  ======================================================================
  ======================================================================
 */
#define BUFSIZE 1024

error_handler(fd, number, funcname, string)
     DtInt fd, number;
     DtPtr funcname, string;
{
    char		msg[BUFSIZE];
    char		buf[BUFSIZE];
    DtErrorStatus	severity;
    DtReadStatus	status;
    static DtFlag	inerror=DcFalse;
    static char		badfilmsg[] =
	    "error_handler: got bad file descriptor.\n";

    if(inerror) {
	/* 
	 * Error handler has recursed. 
	 *  Output only simple message.
	 */

	if (funcname != DcNullPtr) {
	    sprintf (buf, 
		     "(recursive) ERROR #%d in routine %s.\n", 
		     number, funcname);
	} else {
	    sprintf (buf, "(recursive) ERROR #%d\n", number);
	}
	if(strlen(buf) != write(fd, buf, strlen(buf))) {
	    fprintf(stderr, badfilmsg);
	}
	inerror = DcFalse;
	return;
    }

    inerror = DcTrue;

    status = DsInqErrorMessage(number, BUFSIZE, msg,
			       &severity);

    /* test that file descriptor is ok */

    if(6 != write(fd, "***** ", 6)) {
	fprintf(stderr, badfilmsg);
	inerror = DcFalse;
	return;
    }

    if(DcReadTrunc == status) {
	write(fd, 
	      "Error message truncated\n",
	      37);
    }

    if(DcReadUnsuc == status) {
	sprintf(buf, 
		"Error message #%d does not exist\n",
		number);
	write(fd, buf, strlen(buf));
	inerror = DcFalse;
	return;
    }


    switch(severity) {
    case DcErrorMinor: 
	write(fd, "MINOR ", 6);
	break;
    case DcErrorSevere:
	write(fd, "SEVERE ", 7);
	break;
    case DcErrorFatal:
	write(fd, "FATAL ", 6);
	break;
    }

    write(fd, "Error ***** ", 12);
    sprintf(buf, "(#%d) ", number);
    write(fd, buf, strlen(buf));
    if(DcNullPtr != funcname) {
	write(fd, "in routine ", 11);
	write(fd, funcname, strlen(funcname));
	write(fd, " - ", 3);
    }
    write(fd, "\n\t", 2);
    write(fd, msg, strlen(msg)); 
    if ((DcNullPtr != string) && (strlen(string) > 0)) {
	write(fd, " - ", 3);
	write(fd, string, strlen(string));
    }
    write(fd, "\n", 1);
	
    inerror = DcFalse;

    if(severity == DcErrorFatal) {
	/* That's all folks! */
	exit(1);
    }
}
