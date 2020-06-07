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
	void drr_samplernd_before_studio_traversal()
	void drr_samplernd_after_studio_traversal()
	
 ==========================================================================
*/

#include "samplernd.h"

/*
 * Perform initialization before studio traversal takes place
 */

void
drr_samplernd_before_studio_traversal()
{
   /*
    * initialize light list. All ambient lights will be cumulated
    * and stored in the first element of the linked list.
    */
   dre_samplernd_render_private->viwdat->light_cnt = 1;
   dre_samplernd_render_private->viwdat->lights[0].type = DcLightAmbient;
   dre_samplernd_render_private->viwdat->lights[0].color[0] = 0.0;
   dre_samplernd_render_private->viwdat->lights[0].color[1] = 0.0;
   dre_samplernd_render_private->viwdat->lights[0].color[2] = 0.0;

}


/*
===============================================================================
*/

/* 
 * Save information collected during during studio traversal in
 * renderer's private space
 */

void
drr_samplernd_after_studio_traversal()
{
   DtMatrix4x4 tempmat;
   DtReal xscale, yscale, zscale;
   DtReal xtrans, ytrans, ztrans;
   DtVolume viewboundary;
   DtVolume *clipvolume;
   DtReal xres, yres;
   drt_samplernd_view_data_ptr viwdat;

   /*
    * Compute the world to camera trnasformation matrix
    */

   viwdat = dre_samplernd_render_private->viwdat;

   drr_samplernd_matrix_load(viwdat->wcstoccsmat,
   		viwdat->ccstowcsmat);
   drr_samplernd_matrix_invert(
   		viwdat->wcstoccsmat);

   drr_samplernd_matrix_load(viwdat->wcstofcsmat,
		 viwdat->wcstoccsmat);

   /*
    * map from the camera coordinates to the projection volume of
    * -w < x < w, -w < y < w, -w < z < 0 (frustum space)
    */

   drr_samplernd_matrix_post_concatenate(
   	viwdat->wcstofcsmat,
	viwdat->projmat);
   /*
    * map from normalized space to the volume of the view
    */

   if (viwdat->camera_type !=
   	 DcCameraArbitrary) {
      DvInqBoundary(dre_samplernd_render_private->view_handle, &viewboundary);

      xscale = (viewboundary.fur[0]-viewboundary.bll[0])/2.;
      yscale = (viewboundary.fur[1]-viewboundary.bll[1])/2.;
      zscale = (viewboundary.fur[2]-viewboundary.bll[2]);

      xtrans = (viewboundary.fur[0]+viewboundary.bll[0])/2.;
      ytrans = (viewboundary.fur[1]+viewboundary.bll[1])/2.;
      ztrans = viewboundary.fur[2];

   	/*
       * adjust the x scaling if the pixels of the screen are non-square
       */

      DdInqResolution(dre_samplernd_render_private->device_handle,
				 &xres ,&yres);
      if (xres != 0.0) 
	 xscale *= (yres / xres);

      drr_samplernd_matrix_scale(tempmat,xscale,yscale,zscale);
      drr_samplernd_matrix_post_concatenate(
		viwdat->wcstofcsmat,tempmat);
      drr_samplernd_matrix_translate(tempmat, xtrans,ytrans,ztrans);
      drr_samplernd_matrix_post_concatenate(
		viwdat->wcstofcsmat,tempmat);
   }

   /*
    * map from the view volume back to a normalized space
    * ( -w<x<w, -w<y<w, -w<z<0) but this normalized space is just
    * the visible portion of the volume
    * The clipping volume is in frame coordinates, the same coordinates
    * as the view volume
    */

   clipvolume = DDdevice_InqClippingVolume(
		dre_samplernd_render_private->device_handle);

   xscale = 2./(clipvolume->fur[0]-clipvolume->bll[0]);
   yscale = 2./(clipvolume->fur[1]-clipvolume->bll[1]);
   zscale = 1./(clipvolume->fur[2]-clipvolume->bll[2]);

   xtrans = -xscale*clipvolume->bll[0]-1;
   ytrans = -yscale*clipvolume->bll[1]-1;
   ztrans = -zscale*clipvolume->fur[2];

   drr_samplernd_matrix_scale(tempmat, xscale,yscale,zscale);
   drr_samplernd_matrix_post_concatenate(
	viwdat->wcstofcsmat,tempmat);
   drr_samplernd_matrix_translate(tempmat,xtrans,ytrans,ztrans);

   /*
    * Post concatenate the translation to map to frustrum space
    * of visible region.
    */

   drr_samplernd_matrix_post_concatenate(
	viwdat->wcstofcsmat,tempmat);
}

/*
===============================================================================
*/
