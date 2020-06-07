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
	DtPtr ddr_sunview_cxp_return_DCM_fcns ()
	DtPtr ddr_sunview_cxp_dcm_create_local_data (device)
	DtFlag ddr_sunview_cxp_dcm_initialize_device (device, device_data, argstring)
	void ddr_sunview_cxp_dcm_close_device ()
	void ddr_sunview_cxp_dcm_flush ()
	void ddr_sunview_cxp_dcm_swap_buffers ()
	void ddr_sunview_cxp_dcm_update_geometry ()
	void ddr_sunview_cxp_dcm_become_current_driver (device_data)
	void ddr_sunview_cxp_dcm_setup_shading ()
	void ddr_sunview_cxp_dcm_notify_clip_change ()

  ======================================================================
 */
#include "dcm.h"

GP_data_ptr dde_sunview_cxp_current_gp;
ddt_sunview_cxp_data *dde_sunview_cxp_current_dcm_data;

extern DtPtr ddr_sunview_cxp_dcm_create_local_data();
extern DtFlag ddr_sunview_cxp_dcm_initialize_device();
extern void ddr_sunview_cxp_dcm_become_current_driver();
extern void ddr_sunview_cxp_dcm_close_device();
extern void ddr_sunview_cxp_dcm_flush();
extern void ddr_sunview_cxp_dcm_swap_buffers();
extern void ddr_sunview_cxp_dcm_update_geometry();
extern void ddr_sunview_cxp_dcm_inquire_device_extent();
extern void ddr_sunview_cxp_dcm_inquire_stereo();
extern void ddr_sunview_cxp_dcm_inquire_ncolors();
extern void ddr_sunview_cxp_dcm_inquire_resolution();
extern void ddr_sunview_cxp_dcm_inquire_visual_type();
extern void ddr_sunview_cxp_dcm_inquire_auto_size();
extern void ddr_sunview_cxp_dcm_inquire_color_entries();
extern void ddr_sunview_cxp_dcm_set_color_entries();
extern void ddr_sunview_cxp_dcm_set_foreground_color();
extern void ddr_sunview_cxp_dcm_set_background_color();
extern void ddr_sunview_cxp_dcm_set_shade_index();
extern void ddr_sunview_cxp_dcm_inquire_clip_list();
extern void ddr_sunview_cxp_dcm_clear_rectangle_depth();
extern void ddr_sunview_cxp_dcm_clear_rectangle_depth_and_color();
extern void ddr_sunview_cxp_dcm_set_depth_buffer_enable();
extern void ddr_sunview_cxp_dcm_set_depth_buffer_write();
extern void ddr_sunview_cxp_dcm_write_scanline_byte();
extern void ddr_sunview_cxp_dcm_set_current_view();
extern DtFlag ddr_sunview_cxp_dcm_inquire_pixel_data();

DDt_DCM *dde_sunview_cxp_DCM_fcns = (DDt_DCM *)0;

DtPtr
ddr_sunview_cxp_return_DCM_fcns()
{
    DDt_DCM *fcns;

    if (dde_sunview_cxp_DCM_fcns == (DDt_DCM *)0) {
	fcns = DDdevice_CreateDCMStruct();

	if (fcns == (DDt_DCM *)0) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_sunview_cxp_return_DCM_fcns",
			DcNullPtr);
	}

	fcns->version_number 			= 1;
	fcns->create_local_data 		= (DtPFI) ddr_sunview_cxp_dcm_create_local_data;
	fcns->initialize_device 		= (DtPFI) ddr_sunview_cxp_dcm_initialize_device;
	fcns->become_current_driver 		= (DtPFI) ddr_sunview_cxp_dcm_become_current_driver;
	fcns->close_device			= (DtPFI) ddr_sunview_cxp_dcm_close_device;
	fcns->flush 				= (DtPFI) ddr_sunview_cxp_dcm_flush;
	fcns->swap_buffers 			= (DtPFI) ddr_sunview_cxp_dcm_swap_buffers;
	fcns->update_geometry 			= (DtPFI) ddr_sunview_cxp_dcm_update_geometry;
	fcns->inquire_device_extent 		= (DtPFI) ddr_sunview_cxp_dcm_inquire_device_extent;
	fcns->inquire_stereo 			= (DtPFI) ddr_sunview_cxp_dcm_inquire_stereo;
	fcns->inquire_ncolors 			= (DtPFI) ddr_sunview_cxp_dcm_inquire_ncolors;
	fcns->inquire_resolution 		= (DtPFI) ddr_sunview_cxp_dcm_inquire_resolution;
	fcns->inquire_visual_type 		= (DtPFI) ddr_sunview_cxp_dcm_inquire_visual_type;
	fcns->inquire_auto_size 		= (DtPFI) ddr_sunview_cxp_dcm_inquire_auto_size;
	fcns->inquire_color_entries 		= (DtPFI) ddr_sunview_cxp_dcm_inquire_color_entries;
	fcns->set_color_entries 		= (DtPFI) ddr_sunview_cxp_dcm_set_color_entries;
	fcns->set_foreground_color 		= (DtPFI) ddr_sunview_cxp_dcm_set_foreground_color;
	fcns->set_background_color 		= (DtPFI) ddr_sunview_cxp_dcm_set_background_color;
	fcns->set_shade_index 			= (DtPFI) ddr_sunview_cxp_dcm_set_shade_index;
	fcns->inquire_clip_list 		= (DtPFI) ddr_sunview_cxp_dcm_inquire_clip_list;
	fcns->clear_rectangle_depth 		= (DtPFI) ddr_sunview_cxp_dcm_clear_rectangle_depth;
	fcns->clear_rectangle_depth_and_color 	= (DtPFI) ddr_sunview_cxp_dcm_clear_rectangle_depth_and_color;
	fcns->set_depth_buffer_enable 		= (DtPFI) ddr_sunview_cxp_dcm_set_depth_buffer_enable;
	fcns->set_depth_buffer_write 		= (DtPFI) ddr_sunview_cxp_dcm_set_depth_buffer_write;
	fcns->write_scanline_byte 		= (DtPFI) ddr_sunview_cxp_dcm_write_scanline_byte;
	fcns->set_current_view 			= (DtPFI) ddr_sunview_cxp_dcm_set_current_view;
	fcns->inquire_pixel_data 			= (DtPFI) ddr_sunview_cxp_dcm_inquire_pixel_data;

	dde_sunview_cxp_DCM_fcns 			= fcns;
    }
    return (DtPtr)dde_sunview_cxp_DCM_fcns;
}
/*
 ======================================================================
 */

DtPtr 
ddr_sunview_cxp_dcm_create_local_data (device)
     DtPtr device;
{
    ddt_sunview_cxp_data *device_data;

    if ((device_data = (DtPtr)DDspace_Allocate(sizeof (ddt_sunview_cxp_data))) 
	== NULL) {
	DDerror (ERR_CANT_ALLOC_MEM_SIZE, 
		    "ddr_sunview_cxp_dcm_create_local_data",
		    "sizeof (ddt_sunview_cxp_data)");
	return ((DtPtr) DcNullPtr);
    }

    device_data->device = device;

    return ((DtPtr) device_data);
}
/*
 ======================================================================
 */

DtFlag
ddr_sunview_cxp_dcm_initialize_device (device, device_data, argstring)
     DtObject device;
     ddt_sunview_cxp_data *device_data;
     DtPtr argstring;
{
    extern void ddr_sunview_cxp_dcm_notify_clip_change();
    char *argstringcopy, *tmp, *p, *nextarg, *DDstrtok();
    static char *funcname  = "ddr_sunview_cxp_dcm_initialize_device";
    static unsigned char r[256], g[256], b[256];
    int i;
    int error=0;
    int width = 512;
    int height = 512;
    int DoubleBuffer = 1;
    char map_name[80];

    /*
     * This function allocates the device driver instance specific
     * storage and setups up the device for display.
     * A Canvas Id can be passed into this routine via the argument
     * "-canvas 'id'".  If it is not specified then a new frame and
     * canvas are created and then a process is forked to handle the
     * window_main_loop()
     */

    device_data->auto_size = DcTrue;
    device_data->visualtype = DcPseudoColor;

    DdSetShadeMode (device, DcRange);

    device_data->device = device;
    device_data->child_pid = 0;
    device_data->user_canvas = 0;
    device_data->canvas = 0;
    device_data->gp = NULL;
    device_data->rects = NULL;
    device_data->nrects = 0;

    device_data->shade_index = 0;

    device_data->cbtselpsweight[0] = 256. / 256.;
    device_data->cbtselpsweight[1] = 32.  / 256.;
    device_data->cbtselpsweight[2] = 4.   / 256.;
   
    device_data->cbtselpsmask[0] = 0xe0;
    device_data->cbtselpsmask[1] = 0x1c;
    device_data->cbtselpsmask[2] = 0x03;
   
    device_data->cwtavgpsweight[0] = .30;
    device_data->cwtavgpsweight[1] = .59;
    device_data->cwtavgpsweight[2] = .11;
   
    device_data->previous_view = DcNullObject;
    device_data->current_view = DcNullObject;

    /*
     * Process the argstring
     */

    argstringcopy = (char *)DDspace_Allocate(strlen(argstring)+1);
    strcpy(argstringcopy, argstring); /* so as not to alter user's string */
    tmp = argstringcopy;

    /* p points to first sub-string */

    while ((p = DDstrtok(tmp, " \t,"))) {
	tmp = 0;

	if (!strcmp(p,"-noautosize")) {
	    device_data->auto_size = DcFalse;
	} else if (!strcmp(p, "-visualtype")) {
	    if (!(nextarg = DDstrtok(0, " \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "visualtype: (no value)");
		error = 1;
	    } else {
		if ( ! strcmp("DcPseudoColor", nextarg)) {
		    device_data->visualtype = DcPseudoColor;
		} else if ( ! strcmp ("DcDirectColor", nextarg)) {
		    DDerror (ERR_BAD_DEVICE_OPT_VAL,
				funcname, "visualtype: DcDirectColor");
		}		/* else if ...
				   
				   (others possible color types)
				   
				   } */ else {
				       sprintf (DDerror_GetLine(), "visualtype: '%s'", nextarg);
				       DDerror (ERR_BAD_DEVICE_OPT_VAL,
						   funcname, DDerror_GetLine());
				       error = 1;
				   }
	    } 
	} else if (!strcmp(p,"-width")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "width: (no value)");
		error = 1;
	    } else {
		sscanf(nextarg,"%d",&width);
	    }
	} else if (!strcmp(p,"-height")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "height: (no value)");
		error = 1;
	    } else {
		sscanf(nextarg,"%d",&height);
	    }
	} else if (!strcmp(p,"-canvas")) {
	    if (!(nextarg = DDstrtok(0," \t,"))) {
		DDerror (ERR_BAD_DEVICE_OPT_VAL,
			    funcname, "canvas: (no value)");
		error = 1;
	    } else {
		int w;
	    
		device_data->user_canvas = 1;
		sscanf(nextarg,"%d",&w);
		device_data->canvas = (Canvas)w;
		if (device_data->canvas == NULL) {
		    DDerror (ERR_BAD_DEVICE_OPT_VAL,
				funcname, "canvas: NULL");
		    error = 1;
		}
	    }
	} else if (!strcmp(p,"-singlebuffered")) {
	    DoubleBuffer = 0;
	}	else {
	    sprintf(DDerror_GetLine(), "flag '%s'", p);
	    DDerror (ERR_BAD_DEVICE_OPT,
			funcname, DDerror_GetLine());
	    error = 1;
	}
    }

    DDspace_Deallocate(argstringcopy);

    if (error) {
	return(DcFalse);
    }

    if (!device_data->user_canvas) {
	/*
	 * The user has not supplied a canvas to use so create a new frame
	 * and canvas and fork a process to handle the window processing.
	 */

	Frame frame;
	int pid;

	if (getenv ("WINDOW_ME") == NULL) {
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, "Not Running Sunview");
	    return DcFalse;
	}

	if ((frame = window_create (NULL, FRAME, 0)) == NULL) {
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, "Cannot create Sunview Frame");
	    return DcFalse;
	}

	if ((device_data->canvas = window_create (frame, CANVAS,
						  CANVAS_RETAINED, FALSE,
						  CANVAS_WIDTH, width,
						  CANVAS_HEIGHT, height,
						  WIN_WIDTH, width,
						  WIN_HEIGHT, height,
						  0)) == NULL) {
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, "Cannot create Sunview Canvas");
	    return DcFalse;
	}

	window_fit (frame);

	window_set (frame, WIN_SHOW, TRUE, 0);

	if (pid = fork()) {
	    /*
	     * parent context
	     */
	    device_data->child_pid = pid;

	} else if (pid == 0) {
	    /*
	     * child context
	     */

	    window_main_loop(frame);
	 
	    exit(0);
	} else if (pid < 0) {
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			funcname, "Cannot fork process");
	    return DcFalse;
	}
    }

    /*
     * setup the color map
     */
    device_data->pw = canvas_pixwin(device_data->canvas);
    sprintf (map_name, "dore%d", getpid());
    pw_setcmsname (device_data->pw, map_name);
   
    for (i=0; i<256; i++) {
	r[i] = g[i] = b[i] = i;
    }

    pw_putcolormap (device_data->pw, 0, 256, r, g, b);

    device_data->width = (int) window_get (device_data->canvas,
					   CANVAS_WIDTH, 0);
    device_data->height = (int) window_get (device_data->canvas,
					    CANVAS_HEIGHT, 0);

    device_data->gp = GP_init (device_data->pw, 2, DcTrue, DoubleBuffer, 
			       255, ddr_sunview_cxp_dcm_notify_clip_change);
    if (device_data->gp == NULL) {
	DDerror (ERR_DEVICE_SYSTEM_ACCESS,
		    funcname, 
		    "Could not access GP");
	return DcFalse;
    }

    dde_sunview_cxp_current_gp = device_data->gp;

    return DcTrue;
		    
}				/* ddr_sunview_cxp_dcm_initialize_device */
/*
 ======================================================================
 */

void
ddr_sunview_cxp_dcm_close_device ()
{
    if (dde_sunview_cxp_current_dcm_data->child_pid) {
	/*
	 * a process was forked to handle the window management so
	 * it needs to be killed
	 */

	kill (dde_sunview_cxp_current_dcm_data->child_pid, SIGTERM);
    }

}				/* ddr_sunview_cxp_dcm_close_device */
/*
 ======================================================================
 */

void
ddr_sunview_cxp_dcm_flush ()
{
    GP_flush (dde_sunview_cxp_current_dcm_data->gp);
}
/*
 ======================================================================
 */

void
ddr_sunview_cxp_dcm_swap_buffers ()
{
    if (!DDdevice_InqAbortedDraw(dde_sunview_cxp_current_dcm_data->device)) {
	GP_switch_buffer(dde_sunview_cxp_current_dcm_data->gp);
    }
}
/*
 ======================================================================
 */

void
ddr_sunview_cxp_dcm_update_geometry ()
{
    Rect *pBox, *pBoxBase;
    DtInt nrects;
    DtReal *p;

    pBoxBase = GP_get_cliplist (dde_sunview_cxp_current_dcm_data->gp, 
				&nrects);

    if (nrects == 0) {
	return;
    }

    dde_sunview_cxp_current_dcm_data->nrects = nrects;

    pBox = pBoxBase;

    p = dde_sunview_cxp_current_dcm_data->rects;

    if (p) {
	p = (DtReal *)DDspace_Reallocate(p, 4*nrects*sizeof(DtReal));
    } else {
	p = (DtReal *)DDspace_Allocate(4*nrects*sizeof(DtReal));
    }
    dde_sunview_cxp_current_dcm_data->rects = p;

    while (nrects--) {
	p[0] = pBox->r_left;
	p[1] = pBox->r_top;
	p[2] = pBox->r_left + pBox->r_width;
	p[3] = pBox->r_top + pBox->r_height;

	p += 4;
	pBox++;
    }

    if (pBoxBase) {
	DDspace_Deallocate(pBoxBase);
    }
}
/*
 ======================================================================
 */

void
ddr_sunview_cxp_dcm_become_current_driver (device_data)
     ddt_sunview_cxp_data *device_data;
{
    dde_sunview_cxp_current_gp = device_data->gp;
    dde_sunview_cxp_current_dcm_data = device_data;
}
/*
 ======================================================================
 */

void
ddr_sunview_cxp_dcm_setup_shading()
{
    DtInt range_min, range_max;

    dde_sunview_cxp_current_dcm_data->shade_mode = 
	    DdInqShadeMode (dde_sunview_cxp_current_dcm_data->device);
    if (dde_sunview_cxp_current_dcm_data->shade_mode == DcRange) {
	DDdevice_InqShadeRange (dde_sunview_cxp_current_dcm_data->device, 
			     dde_sunview_cxp_current_dcm_data->shade_index,
			     &range_min, &range_max);
	dde_sunview_cxp_current_dcm_data->cwtavgpslim[0] = (DtReal)range_min;
	dde_sunview_cxp_current_dcm_data->cwtavgpslim[1] = (DtReal)range_max;
	dde_sunview_cxp_current_dcm_data->cwtavgpslim[2] = 
		(dde_sunview_cxp_current_dcm_data->cwtavgpslim[1]-dde_sunview_cxp_current_dcm_data->cwtavgpslim[0])/255.;
    }
}
/*
 ======================================================================
 */

void
ddr_sunview_cxp_dcm_notify_clip_change()
{
    DDdevice_SetAbortedDraw(dde_sunview_cxp_current_dcm_data->device, DcTrue);
    dde_sunview_cxp_current_dcm_data->current_view = DcNullObject;
}
