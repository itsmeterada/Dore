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

	dor_io_file_open_read
	dor_io_file_open_write
	dor_io_open_read
	dor_io_open_write
	dor_io_read
	dor_io_write
	dor_io_close
	dor_io_file_close

	-- Inactive Functions --
	dor_io_append_filename_slash
	dor_io_file_exist
	dor_io_file_get_string
	dor_io_file_scan_realnatural
	dor_io_get_string
	dor_io_lseek
	dor_io_open_readwrite
	dor_io_put_string
	dor_io_read_float
	dor_io_read_int32
	dor_io_read_test
	dor_io_sleep_seconds
	dor_io_string_scan_realnatural
	dor_io_wait_for_input
	dor_io_write_int32

  ======================================================================
 */
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <dore/internal/dogen.h>




/* ====================================================================== */

FILE *dor_io_file_open_read (char *filename)
{
    FILE *file;

    if (!(file = fopen (filename, "r"))) {
	sprintf(DDerror_GetLine(), "open failed for file named %s",filename);
	DDerror (ERR_FATAL_IO_ERROR,"dor_io_file_open_read", DDerror_GetLine());
	return((FILE *)NULL);
    }

    return(file);
}

/* ====================================================================== */

FILE *dor_io_file_open_write (char *filename)
{
    FILE *file;

    if (!(file = fopen (filename, "w"))) {
	sprintf(DDerror_GetLine(), "open failed for file named %s", filename);
	DDerror (ERR_FATAL_IO_ERROR,"dor_io_file_open_write",DDerror_GetLine());
    }

    return(file);
}

/* ====================================================================== */

int dor_io_open_read (char *filename)
{
    int filefd;

    if ((filefd=open(filename,O_RDONLY)) < 0){
	sprintf(DDerror_GetLine(),
		"open failed for file named %s, system error code %d",
		filename,errno);
	DDerror (ERR_FATAL_IO_ERROR, "dor_io_open_read", DDerror_GetLine());
	return((int)-1);
    }

    return(filefd);
}

/* ====================================================================== */

int dor_io_open_write (char *filename)
{
    int filefd;

    if ((filefd=open(filename,O_WRONLY|O_CREAT|O_TRUNC,0755)) == -1){
	sprintf(DDerror_GetLine(),
		"open failed for file named %s, system error code %d",
		filename,errno);
	DDerror (ERR_FATAL_IO_ERROR, "dor_io_open_write", DDerror_GetLine());
	return((int)-1);
    }

    return(filefd);
}

/* ====================================================================== */

int dor_io_read (
    int filefd,
    char *buf,
    unsigned int length)
{
    int readbytes;

    if ((readbytes=read(filefd,buf,length)) != length){
	sprintf (DDerror_GetLine(), "read failed, read %d bytes", readbytes);
	DDerror (ERR_IO_ERROR, "dor_io_read", DDerror_GetLine());
	return((int)-1);
    }

    return(readbytes);
}

/* ====================================================================== */

void dor_io_write (
    int filefd,
    char *buf,
    unsigned int length)
{
    unsigned int written;

    if ((written=write(filefd,buf,length)) != length) {
	sprintf (DDerror_GetLine(), "write failed, wrote %d bytes", written);
	DDerror (ERR_IO_ERROR, "dor_io_write", DDerror_GetLine());
    }
}

/* ====================================================================== */

int dor_io_file_close (FILE *file)
{
    if (fclose(file) == EOF){
	sprintf (DDerror_GetLine(),"fclose failed, system error code %d",errno);
	DDerror (ERR_IO_ERROR, "dor_io_file_close", DDerror_GetLine());
	return -1;
    }

    return 0;
}

/* ====================================================================== */

int dor_io_close (int filefd)
{
    if (close(filefd) == -1){
	sprintf (DDerror_GetLine(),"close failed, system error code %d",errno);
	DDerror (ERR_IO_ERROR, "dor_io_close", DDerror_GetLine());
	return -1;
    }

    return 0;
}





/*****************************************************************************
******************************************************************************
**
**  The following functions are unreferenced in the Dore' code, and have been
**  commented out.
**
******************************************************************************
*****************************************************************************/





#if 0

int dor_io_open_readwrite (
    char *filename)
{
    int filefd;
    extern int errno;

    if ((filefd=open(filename,O_RDWR,0755)) == -1){
	sprintf(DDerror_GetLine(),
		"open failed for file named %s, system error code %d",
		filename,errno);
	DDerror (ERR_FATAL_IO_ERROR,
		    "dor_io_open_readwrite", 
		    DDerror_GetLine());
	return((int)-1);
    }

    return(filefd);
}
/*
 ======================================================================
 */

int dor_io_read_float (
    int filefd,
    DtReal *fvalue)
{
    int readbytes;

    if ((readbytes=read(filefd,fvalue,4)) != 4){
	sprintf(DDerror_GetLine(),
		"read failed, read %d bytes",
		readbytes);
	DDerror (ERR_IO_ERROR,
		    "dor_io_read_float", 
		    DDerror_GetLine());
	return(0);
    }

    return(1);
}
/*
 ======================================================================
 */

int dor_io_read_int32 (
    int filefd,
    Dt32Bits *int32)
{
    char bytes[4];
    unsigned readbytes;

    if ((readbytes=read(filefd,bytes,4)) != 4){
	sprintf(DDerror_GetLine(),
		"read failed, read %d bytes",readbytes);
	DDerror (ERR_IO_ERROR,
		    "dor_io_read_integer_32", 
		    DDerror_GetLine());
	return(0);
    }

    *int32 = ((Dt32Bits)(bytes[0]) << 24)
	   | ((Dt32Bits)(bytes[1]) << 16)
	   | ((Dt32Bits)(bytes[2]) <<  8)
	   | ((Dt32Bits)(bytes[3]) <<  0);

    return 1;
}
/*
 ======================================================================
 */


int dor_io_read_test (
    int filefd,
    char *buf,
    unsigned int length)
{
    int readbytes;

    if ((readbytes=read(filefd,buf,length)) != length)
	    return(-1);
    else
	    return(readbytes);
}
/*
 ======================================================================
 */

int dor_io_get_string (
    int filefd,
    char string[])
{
    int charsread;
    char newchar;
    int stringlength;
    int maxchar = 200;

    /*	get characters until either the maximum string length, a newline
	character, or EOF is encountered.				*/

    for (stringlength = 0;
	 stringlength < maxchar-1 &&
	 (charsread = (read(filefd,&newchar,1) != 0) &&
	  newchar != '\n');
	 ++stringlength)
	    string[stringlength] = newchar;

    if (stringlength == maxchar-1){
	DDerror (ERR_IO_ERROR,
		    "dor_io_get_string",
		    "maximum input string length exceeded");
	return((int)EOF);
    }

    string[stringlength] = '\0';

    /*	if EOF was reached before any characters (including a newline) were
	read, then return EOF, otherwise, return 0.			*/

    if ((stringlength == 0) && (charsread == 0))
	    return((int)EOF);
    else{
	return((int)0);
    }
}
/*
 ======================================================================
 */

int dor_io_file_get_string (
    FILE *file,
    char string[])
{
    int newchar, stringlength;
    int maxchar = 200;
	
    /*	get characters until either the maximum string length, 
	a newline character, or EOF is encountered.	*/

    for (stringlength = 0;
	 stringlength < maxchar-1 &&
	 ((newchar=getc(file)) != EOF && newchar != '\n');
	 ++stringlength)
	    string[stringlength] = newchar;

    if (stringlength == maxchar-1){
	DDerror (ERR_IO_ERROR,
		    "dor_io_file_get_string", 
		    "maximum input string length exceeded");
	return((int)EOF);
    }

    string[stringlength] = '\0';

    /*	if EOF was reached before any characters 
	(including a newline) were read, then return EOF, 
	otherwise, return 0.		*/

    if ((stringlength == 0) && (newchar == EOF))
	    return((int)EOF);
    else{
	return((int)0);
    }
}
/*
 ======================================================================
 */

int 
dor_io_file_scan_realnatural(file,value)
FILE *file;
dot_realnatural *value;
{
    int returnvalue;
    double tmpval;

    returnvalue = fscanf(file,"%lf",&tmpval);
    *value = tmpval;

    return(returnvalue);
}
/*
 ======================================================================
 */

int 
dor_io_string_scan_realnatural(string,value)
char *string;
dot_realnatural *value;
{
    int returnvalue;
    double tmpval;

    returnvalue = sscanf(string,"%lf",&tmpval);
    *value = tmpval;

    return(returnvalue);
}
/*
 ======================================================================
 */

int dor_io_write_int32 (
    int filefd,
    Dt32Bits int32)
{
    char bytes[4];
    unsigned int written;

    bytes[3] = int32 & 255;

    bytes[2] = (int32>>8) & 255;

    bytes[1] = (int32>>16) & 255;

    bytes[0] = (int32>>24) & 255;

    if ((written=write(filefd,bytes,4)) != 4){
	sprintf(DDerror_GetLine(),
		"write failed, wrote %d bytes",written);
	DDerror (ERR_IO_ERROR,
		    "dor_io_write_integer_32", 
		    DDerror_GetLine());
    }
}
/*
 ======================================================================
 */


int dor_io_put_string (
    int filefd,
    char string[])
{
    int stringlength;

    stringlength = strlen(string);

    if (stringlength > 0)
	dor_io_write(filefd,string,stringlength);
}
/*
 ======================================================================
 */

int dor_io_lseek (
    int fd,
    int offset,
    int whence)
{
    DtInt pos;
    DtInt long_offset;
    extern int errno;

    long_offset = offset;
    pos = lseek(fd,long_offset,whence);

    if (pos < 0){
	sprintf(DDerror_GetLine(),
		"lseek failed for fd %d, system error code %d",
		fd,errno);
	DDerror (ERR_FATAL_IO_ERROR,
		    "dor_io_lseek", 
		    DDerror_GetLine());
    }
}
/*
 ======================================================================
 */


int dor_io_wait_for_input (void)
{
    char string[200];

    dor_io_file_get_string(stdin,string);
}
/*
 ======================================================================
 */


int dor_io_sleep_seconds (
    DtInt sleepseconds)
{
    sleep(sleepseconds);
}
/*
 ======================================================================
 */

int dor_io_file_exist (
    char *filename)
{
    int filefd;

    if ((filefd=open(filename,O_RDONLY)) < 0){
	return(DcFalse);
    } else {
	dor_io_close(filefd);
	return(DcTrue);
    }
}
/*
 ======================================================================
 */

int dor_io_append_filename_slash (
    char *filename)
{
    strcat(filename,"/");
}

#endif
