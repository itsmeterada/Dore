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
 ==========================================================================
 Functions :
	void drr_samplernd_render_invoke_rtn(device, view,
					 view_device_changed, studio_changed)
	void drr_samplernd_update_device_view(device, view)
 ==========================================================================
*/

#include "samplernd.h"

/* temporary */
#include <stdio.h>

/*
===============================================================================
*/

/*
 * Rendering routine.
 * This is the routine that will be called by Dore every time a view
 * needs to be rendered with the sample renderer
 */

void
drr_samplernd_render_invoke_rtn(device, view,
			 view_device_changed, studio_changed)
     DtObjectStructure *device;
     DtObjectStructure *view;
     DtFlag view_device_changed;
     DtFlag studio_changed;
{
     void drr_samplernd_update_device_view();

   /*
    * Get output module interface routines
    */

   DDdevice_InqInterfaceRoutines(device, DDc_PROM,
				 (DtPtr *) &(dre_samplernd_render_private->output));

   if (dre_samplernd_render_private->output == DcNullPtr) {
      DDerror (ERR_NO_OUTPUT_MODULE,
		  "drr_samplernd_render_invoke_rtn", "DDc_PROM");
      return;
   }

  /*
   * store view and device object handles so that they can be accessed
   * directly by the renderer. This allows any method routine to query
   * information about the current view and device.
   */

   dre_samplernd_render_private->view_handle = view;
   dre_samplernd_render_private->device_handle = device;


   /*
    * get pointers to additional data in the view and device 
    * for the renderer.
    */

   dre_samplernd_render_private->viwdat = 
	 (drt_samplernd_view_data_ptr)view->additional_data[
   			dre_samplernd_render_private->view_index];
   dre_samplernd_render_private->devdat = 
	 (drt_samplernd_device_data_ptr)device->additional_data[
			dre_samplernd_render_private->device_index];
		      
   /*
    * update view and device data 
    */

   if (view_device_changed) {
      drr_samplernd_update_device_view(device, view);
   }

   /*
    * Do studio traversal to determine lights and active camera
    */

   if (studio_changed) {
      drr_samplernd_before_studio_traversal();
      DDview_TraverseStudio (view,
		 dre_samplernd_render_private->studio_traversal);
      drr_samplernd_after_studio_traversal();

   }


   /*
    * Do display traversal generate image via device output interface
    */
   drr_samplernd_before_display_traversal();
   DDview_TraverseDisplay (view,
		 dre_samplernd_render_private->display_traversal);
   drr_samplernd_after_display_traversal();
}

/*
===============================================================================
*/

/*
 * Update the renderer's private data with current information about
 * the view and device
 */

void 
drr_samplernd_update_device_view(device, view)
     DtObject view;
     DtObject device;
{
   DtVolume *actviewport;
   drt_samplernd_view_data_ptr viwdat;
   drt_samplernd_device_data_ptr devdat;


   viwdat = dre_samplernd_render_private->viwdat;
   devdat = dre_samplernd_render_private->devdat;


   actviewport = DDdevice_InqActualViewport (device);
			   
   devdat->extent[0] = actviewport->bll[0];
   devdat->extent[1] = actviewport->bll[1];
   devdat->extent[2] = actviewport->fur[0];
   devdat->extent[3] = actviewport->fur[1];

   viwdat->clear_flag = DvInqClearFlag(view);
   DvInqBackgroundColor (view,
	&viwdat->background_color_model,
	viwdat->background_color);

   DDdevice_InqFrustumScaleTrans(device,
	devdat->scale,
	devdat->trans);

   devdat->shade_mode = DdInqShadeMode (device);
   devdat->visual_type = DdInqVisualType (device);

   if (devdat->visual_type == DcPseudoColor) {
      viwdat->background_shade_index =
		DvInqShadeIndex(view);
   }
}
