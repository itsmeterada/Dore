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
	DtPtr ddr_sunview_gen_return_DCM_fcns ()
	DtPtr ddr_sunview_gen_dcm_create_local_data (device)
	DtFlag ddr_sunview_gen_dcm_initialize_device (device, device_data, argstring)
	void ddr_sunview_gen_dcm_close_device ()
	void ddr_sunview_gen_dcm_flush ()
	void ddr_sunview_gen_dcm_swap_buffers ()
	void ddr_sunview_gen_dcm_update_geometry ()
	void ddr_sunview_gen_dcm_become_current_driver (device_data)
	void ddr_sunview_gen_dcm_set_options (argstring)
	void ddr_sunview_gen_dcm_setup_shading ()
	void ddr_sunview_gen_InitSimulator ()
	void ddr_sunview_gen_FreeBuffer ()
	void ddr_sunview_gen_UpdateSimulator ()

  ======================================================================
 */
#include "dcm.h"
#include "dore/dore_develop/develop.h"

ddt_sunview_gen_data *dde_sunview_gen_current_device_data;
static DDt_DCM  *dde_sunview_gen_DCM_fcns = 0;

/*
 ======================================================================
 */
DtPtr
ddr_sunview_gen_return_DCM_fcns()
{
    DDt_DCM *fcns;

    if (dde_sunview_gen_DCM_fcns == (DDt_DCM *)0) {
	fcns = DDdevice_CreateDCMStruct();

	if (fcns == (DDt_DCM *)0) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_sunview_gen_return_DCM_fcns",
			DcNullPtr);
	}

	fcns->version_number 			= 2;
	fcns->create_local_data 		= (DtPFI) ddr_sunview_gen_dcm_create_local_data;
	fcns->initialize_device 		= (DtPFI) ddr_sunview_gen_dcm_initialize_device;
	fcns->become_current_driver 		= (DtPFI) ddr_sunview_gen_dcm_become_current_driver;
	fcns->close_device			= (DtPFI) ddr_sunview_gen_dcm_close_device;
	fcns->flush 				= (DtPFI) ddr_sunview_gen_dcm_flush;
	fcns->swap_buffers 			= (DtPFI) ddr_sunview_gen_dcm_swap_buffers;
	fcns->update_geometry 			= (DtPFI) ddr_sunview_gen_dcm_update_geometry;
	fcns->inquire_device_extent 		= (DtPFI) ddr_sunview_gen_dcm_inquire_device_extent;
	fcns->inquire_stereo 			= (DtPFI) ddr_sunview_gen_dcm_inquire_stereo;
	fcns->inquire_ncolors 			= (DtPFI) ddr_sunview_gen_dcm_inquire_ncolors;
	fcns->inquire_resolution 		= (DtPFI) ddr_sunview_gen_dcm_inquire_resolution;
	fcns->inquire_visual_type 		= (DtPFI) ddr_sunview_gen_dcm_inquire_visual_type;
	fcns->inquire_auto_size 		= (DtPFI) ddr_sunview_gen_dcm_inquire_auto_size;
	fcns->inquire_color_entries 		= (DtPFI) ddr_sunview_gen_dcm_inquire_color_entries;
	fcns->set_color_entries 		= (DtPFI) ddr_sunview_gen_dcm_set_color_entries;
	fcns->set_foreground_color 		= (DtPFI) ddr_sunview_gen_dcm_set_foreground_color;
	fcns->set_background_color 		= (DtPFI) ddr_sunview_gen_dcm_set_background_color;
	fcns->set_shade_index 			= (DtPFI) ddr_sunview_gen_dcm_set_shade_index;
	fcns->inquire_clip_list 		= (DtPFI) ddr_sunview_gen_dcm_inquire_clip_list;
	fcns->clear_rectangle_depth 		= (DtPFI) ddr_sunview_gen_dcm_clear_rectangle_depth;
	fcns->clear_rectangle_depth_and_color 	= (DtPFI) ddr_sunview_gen_dcm_clear_rectangle_depth_and_color;
	fcns->set_depth_buffer_enable 		= (DtPFI) ddr_sunview_gen_dcm_set_depth_buffer_enable;
	fcns->set_depth_buffer_write 		= (DtPFI) ddr_sunview_gen_dcm_set_depth_buffer_write;
	fcns->write_scanline_byte 		= (DtPFI) ddr_sunview_gen_dcm_write_scanline_byte;
	fcns->set_current_view 			= (DtPFI) ddr_sunview_gen_dcm_set_current_view;
	fcns->inquire_pixel_data 			= (DtPFI) ddr_sunview_gen_dcm_inquire_pixel_data;
	fcns->set_options			= (DtPFI) ddr_sunview_gen_dcm_set_options;

	dde_sunview_gen_DCM_fcns 		= fcns;
    }
    return (DtPtr)dde_sunview_gen_DCM_fcns;
}
/*
 ======================================================================
 */
DtPtr 
ddr_sunview_gen_dcm_create_local_data (device)
     DtPtr device;
{
   ddt_sunview_gen_data *device_data;

   if ((device_data = (DtPtr)DDspace_Allocate(sizeof (ddt_sunview_gen_data))) 
       == NULL) {
      DDerror (ERR_CANT_ALLOC_MEM_SIZE, 
		  "ddr_sunview_gen_dcm_create_local_data",
		  "sizeof (ddt_sunview_gen_data)");
      return ((DtPtr) DcNullPtr);
   }

   device_data->device = device;

   return ((DtPtr) device_data);
}
/*
 ======================================================================
 */
DtFlag
ddr_sunview_gen_dcm_initialize_device (device, device_data, argstring)
     DtObject device;
     ddt_sunview_gen_data *device_data;
     DtPtr argstring;
{
   char *argstringcopy, *tmp, *p, *nextarg, *DDstrtok();
   static char *funcname  = "ddr_sunview_gen_dcm_initialize_device";
   static unsigned char r[256], g[256], b[256];
   int i;
   int error=0;
   int width = 512;
   int height = 512;
   char map_name[80];

   dde_sunview_gen_current_device_data = device_data;

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
   device_data->doubleBuffered = 1;
   device_data->simulator = 0;
   device_data->dither = 0;
   device_data->update_needed = 1;
   device_data->back_buf = (Pixrect *)0;

   DdSetShadeMode (device, DcComponent);

   device_data->device = device;
   device_data->child_pid = 0;
   device_data->user_canvas = 0;
   device_data->canvas = 0;

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
   
   device_data->nred = 8;
   device_data->ngreen = 8;
   device_data->nblue = 4;
   device_data->rscale = (float) (device_data->ngreen * device_data->nblue);
   device_data->gscale = (float) (device_data->nblue);
   device_data->bscale = 1.0;

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
	    } else if ( ! strcmp ("DcStaticGrey", nextarg)) {
	       device_data->visualtype = DcStaticGrey;

	    } else {
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
	 device_data->doubleBuffered = 0;

      } else if(!strcmp(p,"-zbuffer")) {
	 device_data->simulator = 1;

      } else if(!strcmp(p,"-dither")) {
	 device_data->dither = 1;

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

   device_data->pw = canvas_pixwin(device_data->canvas);

   if (device_data->visualtype == DcPseudoColor) {
      /*
       * setup the color map
       */
      sprintf (map_name, "dore%d", getpid());
      pw_setcmsname (device_data->pw, map_name);
   
      for (i=0; i<256; i++) {
	 r[i] = (DtReal)(i>>5) / 7. * 255.;
	 g[i] = (DtReal)((i>>2) & 0x07) / 7. * 255.;
	 b[i] = (DtReal)(i & 0x03) / 3. * 255.;
      }
      pw_putcolormap (device_data->pw, 0, 256, r, g, b);
   } else {
      device_data->fg_color_index = 1;
      device_data->bg_color_index = 0;
      device_data->draw_index = 1;
   }

   device_data->width = (int) window_get (device_data->canvas,
					  CANVAS_WIDTH, 0);
   device_data->height = (int) window_get (device_data->canvas,
					   CANVAS_HEIGHT, 0);

   if(device_data->simulator)
	 ddr_sunview_gen_InitSimulator();


   if (device_data->doubleBuffered) {
      if (device_data->visualtype == DcPseudoColor) {
	 device_data->back_buf = mem_create(device_data->width,
					    device_data->height,
					    8);
      } else {
	 device_data->back_buf = mem_create(device_data->width,
					    device_data->height,
					    1);
      }
      if (device_data->back_buf == (Pixrect *)0) {
	 device_data->doubleBuffered = 0;
	 DDerror (ERR_DEVICE_SYSTEM_ACCESS,
		     funcname, "Cannot create back buffer pixrect");
      }
   }

   return DcTrue;
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_close_device ()
{
   if (dde_sunview_gen_current_device_data->child_pid) {
      /*
       * a process was forked to handle the window management so
       * it needs to be killed
       */

      kill (dde_sunview_gen_current_device_data->child_pid, SIGTERM);
   }

   if(dde_sunview_gen_current_device_data->simulator)
	 ddr_sunview_gen_FreeBuffer();

   if (dde_sunview_gen_current_device_data->back_buf) {
      pr_destroy(dde_sunview_gen_current_device_data->back_buf);
   }
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_flush ()
{
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_swap_buffers ()
{
   ddt_sunview_gen_data *device_data;

   device_data = dde_sunview_gen_current_device_data;

   if (device_data->doubleBuffered && device_data->back_buf) {
      pw_rop (device_data->pw, 0, 0, device_data->width, device_data->height,
	      PIX_SRC, device_data->back_buf, 0, 0);
   }

}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_update_geometry ()
{
   ddt_sunview_gen_data *device_data;

   device_data = dde_sunview_gen_current_device_data;

   if (device_data->update_needed == DcFalse) 
	 return;

   device_data->width  = (int) window_get (device_data->canvas,
					   CANVAS_WIDTH, 0);
   device_data->height = (int) window_get (device_data->canvas,
					   CANVAS_HEIGHT, 0);

   if (device_data->simulator)
	 ddr_sunview_gen_UpdateSimulator();

   if (device_data->back_buf) {
      pr_destroy (device_data->back_buf);
      if (device_data->visualtype == DcPseudoColor) {
	 device_data->back_buf = mem_create(device_data->width,
					    device_data->height,
					    8);
      } else {
	 device_data->back_buf = mem_create(device_data->width,
					    device_data->height,
					    1);
      }
      if (device_data->back_buf == (Pixrect *)0) {
	 device_data->doubleBuffered = 0;
	 DDerror (ERR_DEVICE_SYSTEM_ACCESS,
		     "ddr_sunview_gen_dcm_update_geometry", 
		     "Cannot create back buffer pixrect");
      }
   }

   device_data->update_needed = DcFalse;
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_become_current_driver (device_data)
     ddt_sunview_gen_data *device_data;
{
   dde_sunview_gen_current_device_data = device_data;

   if (device_data->doubleBuffered) {
      ddr_softpipe_drom_set_output_functions((DtPFI)ddr_sunview_gen_DrawPoint_back,
					     (DtPFI)ddr_sunview_gen_DrawLine_back,
					     (DtPFI)ddr_sunview_gen_DrawTriangle_back);
      if (device_data->simulator) {
	 ddr_zbuffer_SetBuffer(device_data->width, device_data->height,
			      device_data->depth_buf,
			      ddr_sunview_gen_PutPixel_back);
      }
   } else {
      ddr_softpipe_drom_set_output_functions((DtPFI)ddr_sunview_gen_DrawPoint_front,
					     (DtPFI)ddr_sunview_gen_DrawLine_front,
					     (DtPFI)ddr_sunview_gen_DrawTriangle_front);
      if (device_data->simulator) {
	 ddr_zbuffer_SetBuffer(device_data->width, device_data->height,
			      device_data->depth_buf,
			      ddr_sunview_gen_PutPixel_front);
      }
   }
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_set_options(argstring)
     DtPtr argstring;
{
   char		*argstringcopy;
   char		*tmp;
   char		*p;
   extern char	*DDstrtok();
   ddt_sunview_gen_data *devicedata;
   int 		doubleBuffered = -1;

   devicedata = dde_sunview_gen_current_device_data;

   argstringcopy = (char *)DDspace_Allocate(strlen(argstring)+1);
   strcpy(argstringcopy, argstring); /* so as not to alter user's string */
   tmp = argstringcopy;

   /* p points to first sub-string */

   while ((p = DDstrtok(tmp, " \t,"))) {
      tmp = 0;

      if (!strcmp(p,"-dither")) {
	 devicedata->dither = 1;

      } else if (!strcmp(p,"-nodither")) {
	 devicedata->dither = 0;

      } else if (!strcmp(p, "-doublebuffered")) {
	 doubleBuffered = 1;

      } else if (!strcmp(p, "-singlebuffered")) {
	 doubleBuffered = 0;

      } else if (!strcmp(p, "-zbuffer")) {
	 if (!(devicedata->simulator)) {
	    devicedata->simulator = 1;
	    ddr_sunview_gen_InitSimulator();
	 }

      } else if (!strcmp(p, "-nozbuffer")) {
	 if (devicedata->simulator) {
	    devicedata->simulator = 0;
	    ddr_sunview_gen_FreeBuffer();
	 }
      }
   }

   DDspace_Deallocate(argstringcopy);

   if (doubleBuffered != -1 && doubleBuffered != devicedata->doubleBuffered) {
      if (doubleBuffered && devicedata->back_buf != (Pixrect *)0) {
	 if (devicedata->visualtype == DcPseudoColor) {
	    devicedata->back_buf = mem_create(devicedata->width,
					      devicedata->height,
					      8);
	 } else {
	    devicedata->back_buf = mem_create(devicedata->width,
					      devicedata->height,
					      1);
	 }
	 if (devicedata->back_buf == (Pixrect *)0) {
	    doubleBuffered = 0;
	    DDerror (ERR_DEVICE_SYSTEM_ACCESS,
			"ddr_sunview_gen_dcm_set_options", 
			"Cannot create back buffer pixrect");
	 }
      }
      devicedata->doubleBuffered = doubleBuffered;
      if (devicedata->doubleBuffered) {
	 ddr_softpipe_drom_set_output_functions((DtPFI)ddr_sunview_gen_DrawPoint_back,
						(DtPFI)ddr_sunview_gen_DrawLine_back,
						(DtPFI)ddr_sunview_gen_DrawTriangle_back);
	 if (devicedata->simulator) {
	    ddr_zbuffer_SetBuffer(devicedata->width, devicedata->height,
				 devicedata->depth_buf,
				 ddr_sunview_gen_PutPixel_back);
	 }
      } else {
	 ddr_softpipe_drom_set_output_functions((DtPFI)ddr_sunview_gen_DrawPoint_front,
						(DtPFI)ddr_sunview_gen_DrawLine_front,
						(DtPFI)ddr_sunview_gen_DrawTriangle_front);
	 if (devicedata->simulator) {
	    ddr_zbuffer_SetBuffer(devicedata->width, devicedata->height,
				 devicedata->depth_buf,
				 ddr_sunview_gen_PutPixel_front);
	 }
      }
   }
}
/*
 ======================================================================
 */
void
ddr_sunview_gen_dcm_setup_shading()
{
   DtInt range_min, range_max;

   dde_sunview_gen_current_device_data->shade_mode = 
	 DdInqShadeMode (dde_sunview_gen_current_device_data->device);
   if (dde_sunview_gen_current_device_data->shade_mode == DcRange) {
      DDdevice_InqShadeRange (dde_sunview_gen_current_device_data->device, 
			   dde_sunview_gen_current_device_data->shade_index,
			   &range_min, &range_max);
      dde_sunview_gen_current_device_data->cwtavgpslim[0] = (DtReal)range_min;
      dde_sunview_gen_current_device_data->cwtavgpslim[1] = (DtReal)range_max;
      dde_sunview_gen_current_device_data->cwtavgpslim[2] = 
	    (dde_sunview_gen_current_device_data->cwtavgpslim[1] -
	     dde_sunview_gen_current_device_data->cwtavgpslim[0])/255.;
   }
}
/*
 ======================================================================
 */
void ddr_sunview_gen_InitSimulator ()
{
   ddt_sunview_gen_data *device_data;
   DtInt size;

   device_data = dde_sunview_gen_current_device_data;

   size = device_data->width * device_data->height * sizeof(DtUShort);

   device_data->depth_buf = (DtUShort *)malloc(size);
   if(!(device_data->depth_buf))
	 DDerror(ERR_DEVICE_SYSTEM_ACCESS,"ddr_sunview_gen_InitSimulator",
		    "Not enough memory");

   if (device_data->doubleBuffered) {
      ddr_zbuffer_SetBuffer(device_data->width, device_data->height,
			   device_data->depth_buf,
			   ddr_sunview_gen_PutPixel_back);
   } else {
      ddr_zbuffer_SetBuffer(device_data->width, device_data->height,
			   device_data->depth_buf,
			   ddr_sunview_gen_PutPixel_front);
   }
   ddr_zbuffer_Clear();
}
/*
 ======================================================================
 */
void ddr_sunview_gen_FreeBuffer ()
{
   free(dde_sunview_gen_current_device_data->depth_buf);

   dde_sunview_gen_current_device_data->depth_buf = 0;
}
/*
 ======================================================================
 */
void ddr_sunview_gen_UpdateSimulator ()
{
   ddt_sunview_gen_data *device_data;
   DtInt size;

   device_data = dde_sunview_gen_current_device_data;

   size = device_data->width * device_data->height * sizeof(DtUShort);

#ifdef DEBUG
   printf("size = %d\n",size);
#endif

   device_data->depth_buf = (DtUShort *)realloc(device_data->depth_buf,size);
   if(!(device_data->depth_buf))
	 DDerror(ERR_DEVICE_SYSTEM_ACCESS,"ddr_sunview_gen_UpdateSimulator",
		    "Not enough memory");

   if (device_data->doubleBuffered) {
      ddr_zbuffer_SetBuffer(device_data->width, device_data->height,
			   device_data->depth_buf,
			   ddr_sunview_gen_PutPixel_back);
   } else {
      ddr_zbuffer_SetBuffer(device_data->width, device_data->height,
			   device_data->depth_buf,
			   ddr_sunview_gen_PutPixel_front);
   }

   ddr_zbuffer_Clear();
}
