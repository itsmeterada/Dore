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
	void drr_samplernd_camera_render(object)
 ==========================================================================
*/

#include "../samplernd.h"

 /*
  * Method routine for DoCamera object
  */

void
drr_samplernd_camera_render(object)
     DtObjectStructure *object;
{
   DtObject active_camera;
   DtPFI routine;
   
   active_camera = DvInqActiveCamera(dre_samplernd_render_private->view_handle);

   /*
    * If there is no active camera explicitely set or if the camera that is
    * being executed is the active camera, update the view data
    * with the camera info.
    */

   if (active_camera == DcNullObject || active_camera == object) {

      /* find current value of camera projection matrix */
      routine = DDclass_InqMethod (DsInqClassId("AttCameraMatrix"),
				    DsInqMethodId("InqGlobalValue"));
      (*routine)(&dre_samplernd_render_private->viwdat->camera_type,
			 dre_samplernd_render_private->viwdat->projmat);
      
   	/*
   	 * find current value of local-to-world transformation matrix
   	 * it will become the camera-to-world matrix
       */
      routine = DDclass_InqMethod (DsInqClassId("AttLcstowcsmat"),
				    DsInqMethodId("InqGlobalValue"));
      (*routine)(dre_samplernd_render_private->viwdat->ccstowcsmat);

   }
}
