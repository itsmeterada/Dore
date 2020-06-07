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
	ddr_rasterfile_return_DCM_fcns
	ddr_rasterfile_dcm_create_local_data
	ddr_rasterfile_dcm_initialize_device
	ddr_rasterfile_dcm_close_device
	ddr_rasterfile_dcm_flush
	ddr_rasterfile_dcm_become_current_driver

  ======================================================================
 */

#include <time.h>
#include <string.h>
#include "dcm.h"

ddt_rasterfile_data *dde_rasterfile_current_device_data;
static DDt_DCM  *dde_rasterfile_DCM_fcns = 0;

/* ====================================================================== */

DtPtr ddr_rasterfile_return_DCM_fcns (void)
{
    DDt_DCM *fcns;

    if (dde_rasterfile_DCM_fcns == (DDt_DCM *)0) {
	fcns = DDdevice_CreateDCMStruct();

	if (fcns == (DDt_DCM *)0) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_rasterfile_return_DCM_fcns",
		     DcNullPtr);
	}

	fcns->version_number = 1;
	fcns->create_local_data     = ddr_rasterfile_dcm_create_local_data;
	fcns->initialize_device     = ddr_rasterfile_dcm_initialize_device;
	fcns->become_current_driver = ddr_rasterfile_dcm_become_current_driver;
	fcns->close_device          = ddr_rasterfile_dcm_close_device;
	fcns->flush                 = ddr_rasterfile_dcm_flush;
	fcns->inquire_device_extent = ddr_rasterfile_dcm_inquire_device_extent;
	fcns->inquire_ncolors       = ddr_rasterfile_dcm_inquire_ncolors;
	fcns->inquire_resolution    = ddr_rasterfile_dcm_inquire_resolution;
	fcns->inquire_visual_type   = ddr_rasterfile_dcm_inquire_visual_type;
	fcns->inquire_auto_size     = ddr_rasterfile_dcm_inquire_auto_size;
	fcns->inquire_color_entries = ddr_rasterfile_dcm_inquire_color_entries;
	fcns->inquire_clip_list     = ddr_rasterfile_dcm_inquire_clip_list;
	fcns->write_scanline_byte   = ddr_rasterfile_dcm_write_scanline_byte;
	fcns->inquire_pixel_data    = ddr_rasterfile_dcm_inquire_pixel_data;

	dde_rasterfile_DCM_fcns 	= fcns;
    }
    return (DtPtr)dde_rasterfile_DCM_fcns;
}

/* ====================================================================== */

DtPtr ddr_rasterfile_dcm_create_local_data (DtObject device)
{
    ddt_rasterfile_data *device_data;

    device_data = DDspace_Allocate (sizeof (ddt_rasterfile_data)); 
    if (!device_data) {
	DDerror (ERR_CANT_ALLOC_MEM_SIZE, 
		 "ddr_rasterfile_dcm_create_local_data",
		 "sizeof (ddt_rasterfile_data)");
	return DcNullPtr;
    }

    return ((DtPtr) device_data);
}

/* ====================================================================== */

DtFlag ddr_rasterfile_dcm_initialize_device (
    DtObject device,
    DtPtr    device_data_parm,
    DtPtr    argstring,
    DtPtr    name)
{
#   define device_data ((ddt_rasterfile_data*)(device_data_parm))

    char *argstringcopy, *tmp, *p, *nextarg;
    static char *funcname  = "ddr_rasterfile_dcm_initialize_device";
    DtInt error=0;
    char filename[200];
    long offset, length, offset1;
    extern int errno;

    strcpy(filename,"test.img");

    device_data->width = 512;
    device_data->height = 512;
    device_data->depth = 1;
    device_data->type = DcRasterRGB;

    argstringcopy = (char *)DDspace_Allocate(strlen(argstring)+1);
    strcpy(argstringcopy, argstring); /* so as not to alter user's string */
    tmp = argstringcopy;

    while((p = DDstrtok(tmp, " \t,"))) /* p points to first sub-string */ {
	tmp = 0;

	if (!strcmp(p,"-width")) {
	    if (!(nextarg = DDstrtok(0, " \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			 funcname, "width: (no value)");
		error = 1;
	    }
	    else {
		sscanf(nextarg,"%d",&(device_data->width));
	    }
	}
	else if (!strcmp(p,"-height")) {
	    if (!(nextarg = DDstrtok(0, " \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL, funcname,"height: (no value)");
		error = 1;
	    }
	    else {
		sscanf (nextarg, "%d", &(device_data->height));
	    }
	}
	else if (!strcmp(p,"-filename")) {
	    if (!(nextarg = DDstrtok(0, " \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			 funcname, "filename: (no value)");
		error = 1;
	    }
	    else {
		strcpy(filename,nextarg);
	    }
	}
	else {
	    sprintf (DDerror_GetLine(), "flag '%s'", p);
	    DDerror (ERR_BAD_DEVICE_OPT, funcname, DDerror_GetLine());
	    error = 1;
	}
    }
   
    DDspace_Deallocate (argstringcopy);

    if (error) {
	return(DcFalse);
    }

    if ((device_data->fp = fopen(filename, "w")) == NULL) {
	return(DcFalse);
    }
   
    {   time_t clock;
	char *logname;

	time (&clock);
	if (!(logname = DDgetenv("LOGNAME"))) logname = "<UNKNOWN>";

	fprintf (device_data->fp, "# Dore' Rasterfile created by '%s' on %s", 
		 logname, ctime (&clock));
    }

    fprintf (device_data->fp, "rastertype = image\n");
    fprintf (device_data->fp, "width = %d\n", device_data->width);
    fprintf (device_data->fp, "height = %d\n", device_data->height);
    fprintf (device_data->fp, "depth = %d\n", device_data->depth);
    fprintf (device_data->fp, "pixel = r8g8b8\n");
    fprintf (device_data->fp, "\f\f");
    fflush (device_data->fp);

    device_data->bindata = ftell (device_data->fp);
#   ifdef DEBUG
	printf ("offset to binary data %d\n", device_data->bindata);
#   endif
    length = strlen(filename) + 1;
    offset = device_data->bindata +
	    device_data->height * device_data->width * 3 - length;

    if (fseek (device_data->fp, offset, 0) != 0) {
	sprintf (DDerror_GetLine(), "fseek error %d", errno);
	DDerror(ERR_IO_ERROR, "ddr_rasterfile_dcm_initialize",
		DDerror_GetLine());
    }
    fwrite (filename, sizeof (char), length, device_data->fp);
    fflush(device_data->fp);
    if ((offset1 = ftell(device_data->fp)) != offset + length) {
	sprintf (DDerror_GetLine(), "offset does not match required %ld - %ld",
	    offset1, offset + length);
	DDerror (ERR_IO_ERROR, "ddr_rasterfile_dcm_initialize",
	    DDerror_GetLine());
    }      

#   ifdef DEBUG
	printf ("file length = %d (%d + %d)\n", offset1, offset, length);
#   endif

    return DcTrue;

#   undef device_data
}

/* ====================================================================== */

void ddr_rasterfile_dcm_close_device (void)
{
    fclose(dde_rasterfile_current_device_data->fp);
}

/* ====================================================================== */

void ddr_rasterfile_dcm_flush (void)
{
    return;
}

/* ====================================================================== */

void ddr_rasterfile_dcm_become_current_driver (DtPtr device_data)
{
    dde_rasterfile_current_device_data = ((ddt_rasterfile_data*)(device_data));
}
