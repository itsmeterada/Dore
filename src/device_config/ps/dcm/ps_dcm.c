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
	DtPtr ddr_ps_return_DCM_fcns ()
	DtPtr ddr_ps_dcm_create_local_data (device)
	DtFlag ddr_ps_dcm_initialize_device (device,device_data,argstring,name)
	void ddr_ps_dcm_close_device ()
	void ddr_ps_dcm_flush ()
	void ddr_ps_dcm_swap_buffers ()
	void ddr_ps_dcm_update_geometry ()
	void ddr_ps_dcm_become_current_driver (device_data)
	void ddr_ps_dcm_set_options (argstring)
	void ddr_ps_dcm_start_page ()

  ======================================================================
 */
#include <string.h>
#include <time.h>
#include "dcm.h"

ddt_ps_data *dde_ps_current_device_data;
static DDt_DCM  *dde_ps_DCM_fcns = 0;

/* ====================================================================== */

DtPtr ddr_ps_return_DCM_fcns (void)
{
    DDt_DCM *fcns;

    if (dde_ps_DCM_fcns == (DDt_DCM *)0) {
	fcns = DDdevice_CreateDCMStruct();

	if (fcns == (DDt_DCM *)0) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_ps_return_DCM_fcns",
			DcNullPtr);
	}

	fcns->version_number = 2;

# ifndef WIN32
#       ifdef __STDC__
#           define AddFunction(name) \
		fcns->name = ddr_ps_dcm_##name
#       else
#           define AddFunction(name) \
		fcns->name = ddr_ps_dcm_/**/name
#       endif

	AddFunction (create_local_data);
	AddFunction (initialize_device);
	AddFunction (become_current_driver);
	AddFunction (close_device);
	AddFunction (flush);
	AddFunction (swap_buffers);
	AddFunction (update_geometry);
	AddFunction (inquire_device_extent);
	AddFunction (inquire_stereo);
	AddFunction (inquire_ncolors);
	AddFunction (inquire_resolution);
	AddFunction (inquire_visual_type);
	AddFunction (inquire_auto_size);
	AddFunction (inquire_color_entries);
	AddFunction (set_color_entries);
	AddFunction (set_foreground_color);
	AddFunction (set_background_color);
	AddFunction (set_shade_index);
	AddFunction (inquire_clip_list);
	AddFunction (clear_rectangle_depth);
	AddFunction (clear_rectangle_depth_and_color);
	AddFunction (set_depth_buffer_enable);
	AddFunction (set_depth_buffer_write);
	AddFunction (write_scanline_byte);
	AddFunction (set_current_view);
	AddFunction (inquire_pixel_data);
	AddFunction (set_options);

# else /* If WIN32 */

        fcns->create_local_data = ddr_ps_dcm_create_local_data;
        fcns->initialize_device = ddr_ps_dcm_initialize_device;
        fcns->become_current_driver = ddr_ps_dcm_become_current_driver;
        fcns->close_device = ddr_ps_dcm_close_device;
        fcns->flush = ddr_ps_dcm_flush;
        fcns->swap_buffers = ddr_ps_dcm_swap_buffers;
        fcns->update_geometry = ddr_ps_dcm_update_geometry;
        fcns->inquire_device_extent = ddr_ps_dcm_inquire_device_extent;
        fcns->inquire_stereo = ddr_ps_dcm_inquire_stereo;
        fcns->inquire_ncolors = ddr_ps_dcm_inquire_ncolors;
        fcns->inquire_resolution = ddr_ps_dcm_inquire_resolution;
        fcns->inquire_visual_type = ddr_ps_dcm_inquire_visual_type;
        fcns->inquire_auto_size = ddr_ps_dcm_inquire_auto_size;
        fcns->inquire_color_entries = ddr_ps_dcm_inquire_color_entries;
        fcns->set_color_entries = ddr_ps_dcm_set_color_entries;
        fcns->set_foreground_color = ddr_ps_dcm_set_foreground_color;
        fcns->set_background_color = ddr_ps_dcm_set_background_color;
        fcns->set_shade_index = ddr_ps_dcm_set_shade_index;
        fcns->inquire_clip_list = ddr_ps_dcm_inquire_clip_list;
        fcns->clear_rectangle_depth = ddr_ps_dcm_clear_rectangle_depth;
        fcns->clear_rectangle_depth_and_color = ddr_ps_dcm_clear_rectangle_depth_and_color;
        fcns->set_depth_buffer_enable = ddr_ps_dcm_set_depth_buffer_enable;
        fcns->set_depth_buffer_write = ddr_ps_dcm_set_depth_buffer_write;
        fcns->write_scanline_byte = ddr_ps_dcm_write_scanline_byte;
        fcns->set_current_view = ddr_ps_dcm_set_current_view;
        fcns->inquire_pixel_data = ddr_ps_dcm_inquire_pixel_data;
        fcns->set_options = ddr_ps_dcm_set_options;

# endif

	dde_ps_DCM_fcns = fcns;
    }
    return (DtPtr)dde_ps_DCM_fcns;
}

/* ====================================================================== */

DtPtr ddr_ps_dcm_create_local_data (DtObject device)
{
    ddt_ps_data *device_data;

#ifdef DEBUG
    printf("... processing device create local data ...\n");
    fflush(stdout);
#endif

    if((device_data = (DtPtr)DDspace_Allocate(sizeof(ddt_ps_data))) 
       == NULL) {
	DDerror(ERR_CANT_ALLOC_MEM_SIZE,
		   "ddr_ps_dcm_create_local_data",
		   "sizeof(ddt_ps_data)");
	return((DtPtr)DcNullPtr);
    }

    device_data->device = device;

    return((DtPtr)device_data);
}

/* ====================================================================== */

DtFlag ddr_ps_dcm_initialize_device (
    DtObject device,
    DtPtr    device_data_parm,
    DtPtr    argstring,
    DtPtr    name)
{
#   define device_data ((ddt_ps_data*)(device_data_parm))

    char *argstringcopy, *tmp, *p, *nextarg;
    static char *funcname  = "ddr_ps_dcm_initialize_device";
    int error=0;
    char *filename = "dore.ps";
    int malloc_filename = 0;
    time_t clock;
    char *logname;

    /*
     * This function allocates the device driver instance specific
     * storage and setups up the device for display.
     */

    device_data->device = device;
    device_data->auto_size = DcTrue;

    DdSetShadeMode(device,DcComponent);

    device_data->width = 8.5;
    device_data->height = 8.5;
    device_data->page_no = 1;
    device_data->begin_page = DcTrue;
    device_data->visualtype = DcTrueColor;
    device_data->shade_index = 0;

    device_data->current_view = DcNullObject;
    device_data->previous_view = DcNullObject;
    device_data->border = DcTrue;

    device_data->cwtavgpsweight[0] = 0.30;
    device_data->cwtavgpsweight[1] = 0.59;
    device_data->cwtavgpsweight[2] = 0.11;

    /*
     * Process the argstring
     */

    argstringcopy = (char *)DDspace_Allocate(strlen(argstring)+1);
    strcpy(argstringcopy, argstring); /* so as not to alter user's string */
    tmp = argstringcopy;

    /* p points to first sub-string */

    while((p = DDstrtok(tmp," \t,"))) {
	tmp = 0;

	if(!strcmp(p,"-noautosize")) {
	    device_data->auto_size = DcFalse;
	}
	else if(!strcmp(p,"-border")) {
	    device_data->border = DcTrue;
	}
	else if(!strcmp(p,"-noborder")) {
	    device_data->border = DcFalse;
	}
	else if (!strcmp(p,"-filename")) {
	    if(!(nextarg = DDstrtok(0," \t,"))) {
		DDerror(ERR_BAD_DEVICE_OPT_VAL,funcname,
			   "screen: (no value)");
		error = 1;
	    }
	    else {
		malloc_filename = 1;
		filename = (char *)DDspace_Allocate(strlen(nextarg)+1);
		strcpy (filename, nextarg);
	    }
	}
	else if (!strcmp(p,"-visualtype")) {
	    if(!(nextarg = DDstrtok(0," \t,"))) {
		DDerror(ERR_BAD_DEVICE_OPT_VAL,funcname,
			   "visualtype: (no value)");
		error = 1;
	    }
	    else {
		if(!strcmp("DcStaticGrey",nextarg)) {
		    device_data->visualtype = DcStaticGrey;
		} 
		else if(!strcmp("DcTrueColor",nextarg)) {
		    device_data->visualtype = DcTrueColor;
		}
		else  {
		    sprintf(DDerror_GetLine(),
			    "visualtype: '%s'",nextarg);
		    DDerror(ERR_BAD_DEVICE_OPT_VAL,
			       funcname,DDerror_GetLine());
		    error = 1;
		}
	    } 
	}
	else if(!strcmp(p,"-width")) {
	    if(!(nextarg = DDstrtok(0," \t,"))) {
		DDerror(ERR_BAD_DEVICE_OPT_VAL,
			   funcname, "width: (no value)");
		error = 1;
	    }
	    else {
		double x;
		sscanf (nextarg, "%lf", &x);
		device_data->width = x;
	    }
	}
	else if(!strcmp(p,"-height")) {
	    if(!(nextarg = DDstrtok(0," \t,"))) {
		DDerror(ERR_BAD_DEVICE_OPT_VAL,
			   funcname, "height: (no value)");
		error = 1;
	    }
	    else {
		double x;
		sscanf (nextarg, "%lf", &x);
		device_data->height = x;
	    }
	}
	else {
	    sprintf(DDerror_GetLine(),"flag '%s' ignored",p);
	    DDerror(ERR_BAD_DEVICE_OPT,
		       funcname, DDerror_GetLine());
	}
    }

    DDspace_Deallocate(argstringcopy);

    if(error) {
	return(DcFalse);
    }

    /*
     * do device specific stuff
     */

    if ((device_data->fp = fopen (filename, "w")) == NULL) {
	perror (filename);
	if (malloc_filename) DDspace_Deallocate(filename);
	return (DcFalse);
    }

    fprintf (device_data->fp, "%%!PS-Adobe-1.0\n");
    fprintf (device_data->fp, "%%%%Title: %s\n", filename);
    fprintf (device_data->fp, "%%%%Creator: Dore' Postscript Device Driver\n");
    if ((logname = DDgetenv("LOGNAME")) != NULL)
	    fprintf (device_data->fp, "%%%%For: %s\n", logname);
    time (&clock);
    fprintf (device_data->fp, "%%%%CreationDate: %s", ctime (&clock));
    fprintf (device_data->fp, "%%%%Pages: (atend)\n");
    fprintf (device_data->fp, "%%%%DocumentFonts: \n");
    fprintf (device_data->fp, "%%%%Endcomments\n");

    /*
     * added prolog
     */

    fprintf (device_data->fp, "/Dore 20 dict def\n");
    fprintf (device_data->fp, "Dore begin\n");
    fprintf (device_data->fp, "/Prgb { moveto -1 -1 rmoveto 2 0 rlineto\n");
    fprintf (device_data->fp, "        0 2 rlineto -2 0 rlineto\n");
    fprintf (device_data->fp, "        closepath setrgbcolor fill } def\n");
    fprintf (device_data->fp, "/Lrgb { moveto lineto setrgbcolor stroke } def\n");
    fprintf (device_data->fp, "/Trgb { moveto lineto lineto closepath\n");
    fprintf (device_data->fp, "        setrgbcolor fill } def\n");
    fprintf (device_data->fp, "/BGrgb { dup 3 -1 roll dup 5 -1 roll dup\n");
    fprintf (device_data->fp, "         7 -1 roll dup 3 -1 roll moveto\n");
    fprintf (device_data->fp, "         6 -1 roll lineto exch 4 -1 roll\n");
    fprintf (device_data->fp, "         lineto lineto closepath setrgbcolor\n");
    fprintf (device_data->fp, "         fill } def\n");
    fprintf (device_data->fp, "/Pgray { moveto -1 -1 rmoveto 2 0 rlineto\n");
    fprintf (device_data->fp, "         0 2 rlineto -2 0 rlineto\n");
    fprintf (device_data->fp, "         closepath setgray fill } def\n");
    fprintf (device_data->fp, "/Lgray { moveto lineto setgray stroke } def\n");
    fprintf (device_data->fp, "/Tgray { moveto lineto lineto closepath\n");
    fprintf (device_data->fp, "         setgray fill } def\n");
    fprintf (device_data->fp, "/BGgray { dup 3 -1 roll dup 5 -1 roll dup\n");
    fprintf (device_data->fp, "          7 -1 roll dup 3 -1 roll moveto\n");
    fprintf (device_data->fp, "          6 -1 roll lineto exch 4 -1 roll\n");
    fprintf (device_data->fp, "          lineto lineto closepath setgray\n");
    fprintf (device_data->fp, "          fill } def\n");
    fprintf (device_data->fp, "/BDR { dup 3 -1 roll dup 5 -1 roll dup\n");
    fprintf (device_data->fp, "       7 -1 roll dup 3 -1 roll moveto\n");
    fprintf (device_data->fp, "       6 -1 roll lineto exch 4 -1 roll\n");
    fprintf (device_data->fp, "       lineto lineto closepath setgray\n");
    fprintf (device_data->fp, "       stroke } def\n");

    fprintf (device_data->fp, "%%%%EndProlog\n");

    if (malloc_filename) DDspace_Deallocate(filename);

    return DcTrue;

#   undef device_data
}

/* ====================================================================== */

void ddr_ps_dcm_close_device (void)
{
    ddt_ps_data *device_data;

#ifdef DEBUG
    printf("... processing device close device ...\n");
    fflush(stdout);
#endif

    device_data = dde_ps_current_device_data;

    if (!device_data->begin_page) {
	fprintf (device_data->fp, "grestore\n");
	fprintf (device_data->fp, "showpage\n");
    }

    fprintf (device_data->fp, "%%%%Trailer\n");
    fprintf (device_data->fp, "%%%%Pages: %d\n", device_data->page_no - 1);
    fprintf (device_data->fp, "end\n");
   
    fclose(device_data->fp);
}

/* ====================================================================== */

void ddr_ps_dcm_flush (void)
{
#   ifdef DEBUG
	printf ("... processing device flush ...\n");
	fflush (stdout);
#   endif
}

/* ====================================================================== */

void ddr_ps_dcm_swap_buffers (void)
{
    ddt_ps_data *device_data;

#   ifdef DEBUG
	printf ("...processing device swap buffers \n");
	fflush (stdout);
#   endif

    device_data = dde_ps_current_device_data;

    if (!device_data->begin_page) {
	fprintf (device_data->fp, "grestore\n");
	fprintf (device_data->fp, "showpage\n");
    }
    device_data->begin_page = DcTrue;
}

/* ====================================================================== */

void ddr_ps_dcm_update_geometry (void)
{
#   ifdef DEBUG
	printf ("...processing device update geometry\n");
	fflush (stdout);
#   endif
}

/* ====================================================================== */

void ddr_ps_dcm_become_current_driver (DtPtr device_data)
{
#   ifdef DEBUG
	printf ("... processing device become current driver ...\n");
	fflush (stdout);
#   endif

    dde_ps_current_device_data = device_data;

    ddr_softpipe_drom_set_output_functions((DtPFV)ddr_ps_DrawPoint,
					   (DtPFV)ddr_ps_DrawLine,
					   (DtPFV)ddr_ps_DrawTriangle);
}

/* ====================================================================== */

void ddr_ps_dcm_set_options (DtPtr argstring)
{
    char	*argstringcopy;
    char	*tmp;
    char	*p;
    ddt_ps_data *device_data;

    device_data = dde_ps_current_device_data;

    argstringcopy = (char *)DDspace_Allocate(strlen(argstring)+1);
    strcpy(argstringcopy, argstring); /* so as not to alter user's string */
    tmp = argstringcopy;

    /* p points to first sub-string */

    while ((p = DDstrtok(tmp, " \t,"))) {
	tmp = 0;

	if(!strcmp(p,"-border")) {
	    device_data->border = DcTrue;
	}
	else if(!strcmp(p,"-noborder")) {
	    device_data->border = DcFalse;
	}
    }

    DDspace_Deallocate(argstringcopy);
}

/* ====================================================================== */

void ddr_ps_dcm_start_page (void)
{
    ddt_ps_data *device_data;

    device_data = dde_ps_current_device_data;

    fprintf (device_data->fp, "%%%%Page: ? %d\n", device_data->page_no);
    fprintf (device_data->fp, "gsave\n");
    if (device_data->width < 8.5 && device_data->height < 11.5) {
	fprintf (device_data->fp, "%f %f translate\n",
		 (8.5 - device_data->width) / 2.0 * 72.,
		 (11.5 - device_data->height) / 2.0 * 72.);
    }
    if (device_data->border)
	    fprintf (device_data->fp, "0 %d %d %d %d BDR\n",
		     0, 0, (int)(device_data->width *72), 
		     (int)(device_data->height*72));

    device_data->page_no ++;
    device_data->begin_page = DcFalse;
}   
