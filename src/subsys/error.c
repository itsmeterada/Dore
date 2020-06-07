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
  ======================================================================
  Functions:
	dor_error_initialize
	dor_error
	dor_error_get_line
	dor_error_default_handler
	dor_error_inquire_message
	dor_error_inquire_variables
	dor_error_set_variables

  ======================================================================
 */
#include <string.h>
#include <dore/internal/dogen.h>


    /***  Defined Constants  ***/

#define BUFSIZE 1024	/* Output Line Buffer Size */


    /***  Global Variables  ***/

DtInt   doe_error_file_desc;
DtPFV   doe_error_handler;
#ifndef WIN32
DtFlag  doe_error_fortran_handler;
#endif
FILE   *errmsgfile = 0;
DtFlag  doe_error_initialized = DcFalse;


    /***  Function Prototypes  ***/

static void dor_error_default_handler (DtInt, DtInt, char*, char*);



/* ====================================================================== */

void dor_error_initialize (void)
{
    auto   char *errmsgfilename;
    auto   char *errpath;
    static int   in_initialize = 0;

    if (doe_error_initialized) return;

    /*
     * It is possible for dor_space_allocate to generate an error.
     * To avoid an infinite loop we detect the situation and exit.
     */
    if (in_initialize) {
	printf ("recursive call to dor_error_initialize\n");
	printf ("memory allocation failed -- exiting\n");
	exit (1);
    }
    in_initialize = 1;

    if( ! (errmsgfilename = DDgetenv("DORE_ERRMSGFILE"))) {
	errmsgfilename = DORE_ERRMSGFILE;
    }


    if(NULL == (errmsgfile = fopen(errmsgfilename, "r"))){
	printf("couldn't open error message file %s\n",errmsgfilename);
	exit(1);
    }

    doe_error_file_desc = 2; /* standard error */
    doe_error_handler = dor_error_default_handler;
#ifndef WIN32
    doe_error_fortran_handler = DcFalse;
#endif

    doe_error_initialized = DcTrue;
    in_initialize = 0;
}



/* ====================================================================== */

void dor_error (
    DtInt number,
    char* funcname,
    char* string)
{
    if (!doe_error_initialized) dor_error_initialize();

    if (!string) string = "";

#ifndef WIN32
    if (doe_error_fortran_handler) {
	fortran_call_error_handler
	    (doe_error_handler, doe_error_file_desc, number, funcname, string);
    } else {
#endif
	(*doe_error_handler) (doe_error_file_desc, number, funcname, string);
#ifndef WIN32
    }
#endif
}



/* ====================================================================== */

char *dor_error_get_line (void)
{
    static char error_message_line[256];
    return error_message_line;
}



/* ====================================================================== */

static void dor_error_default_handler (
    DtInt fd,
    DtInt number,
    char* funcname,
    char* string)
{
    char msg[BUFSIZE], buf[BUFSIZE];
    DtErrorStatus severity;
    DtReadStatus status;
    static DtFlag inerror=DcFalse;
    static char badfilmsg[] = "error handler: got bad file descriptor.\n";

    if(inerror) {	
	/* Error handler has recursed. 
	   Output only simple msg */
	sprintf(buf, "ERROR #%d", number);
	if(strlen(buf) != write(fd, buf, strlen(buf)))
		fprintf(stderr, badfilmsg);
	else if(funcname != DcNullPtr){
	    sprintf(buf, " in routine %s.\n", funcname);
	    write(fd, buf, strlen(buf));
	}
	inerror = DcFalse;
	return;
    }

    inerror = DcTrue;

    status = DsInqErrorMessage(number, BUFSIZE, msg, &severity);

    if(6 != write(fd, "***** ", 6)) {
	/* test that file descriptor is ok */
	fprintf(stderr, badfilmsg);
	inerror = DcFalse;
	return;
    }

    if(DcReadTrunc == status){
	write(fd, "error handler: message was truncated\n", 37);
    }

    if(DcReadUnsuc == status){
	sprintf(buf, "error handler: msg #%d does not exist\n", number);
	write(fd, buf, strlen(buf));
	inerror = DcFalse;
	return;
    }


    switch(severity){
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

    write(fd, "ERROR ***** ", 12);
    sprintf(buf, "(#%d) ", number);
    write(fd, buf, strlen(buf));
    if(DcNullPtr != funcname){
	write(fd, "in routine ", 11);
	write(fd, funcname, strlen(funcname));
	write(fd, " - ", 3);
    }
    write(fd, "\n\t", 2);
    write(fd, msg, strlen(msg)); /* the error message */
    if(DcNullPtr != string){
	write(fd, " - ", 3);
	write(fd, string, strlen(string));
    }
    write(fd, "\n", 1);
	
    inerror = DcFalse;

    if(severity == DcErrorFatal) /* That's all folks! */{
	exit(1);
    }
}



/* ====================================================================== */

DtReadStatus dor_error_inquire_message (
    DtInt          errornumber,
    DtInt          bufsiz,
    char           buf[],
    DtErrorStatus *severity)
{
    int i= -999, got, c, n;

    if (!doe_error_initialized) dor_error_initialize();

    rewind(errmsgfile);

    /* look for errornumber at start of a line in error message file */
    while((got = fscanf(errmsgfile, "%d", &i)) &&
	  (got != EOF) &&
	  (i != errornumber)){
	while((EOF != (c = fgetc(errmsgfile))) && (c != '\n'))
		;
    }

    if(i != errornumber) {
	/* searched entire file. errornumber not found */
	return(DcReadUnsuc);
    }

    /* read the error severity value */
    fscanf(errmsgfile, "%d", &n);
    *severity = (DtErrorStatus)n;

    /* read the error message */
    for(i=0; i<bufsiz-1; i++){
	buf[i] = fgetc(errmsgfile);
	if(('\n' == buf[i]) || (EOF == buf[i]))
		break;
    }
	
    buf[i] = '\0';	/* null terminate the message string */

    if(i == bufsiz-1)	/* buffer too small ? */
	    return(DcReadTrunc);
    else
	    return(DcReadOk);
}



/* ====================================================================== */

void dor_error_inquire_variables (
    DtInt  *errorfile,
    DtPFV  *errorhandler)
{
    *errorfile = doe_error_file_desc;
    *errorhandler = doe_error_handler;
}



/* ====================================================================== */

void dor_error_set_variables (
    DtInt  errorfile,
    DtPFV  errorhandler,
    DtFlag call_fortran)
{
    doe_error_file_desc = errorfile;

#ifndef WIN32
    if (errorhandler == DcNullPtr) {
#endif
	doe_error_handler = dor_error_default_handler;
#ifndef WIN32
	doe_error_fortran_handler = DcFalse;
    } else {
	doe_error_handler = errorhandler;
	doe_error_fortran_handler = call_fortran;
    }
#endif
}
