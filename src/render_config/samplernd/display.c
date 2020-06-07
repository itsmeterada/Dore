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
	void drr_samplernd_before_display_traversal()
	void drr_samplernd_after_display_traversal()
 ==========================================================================
*/

#include "samplernd.h"

/*
 * inititalization required before display traversal
 */

void
drr_samplernd_before_display_traversal()
{
   DtVolume *actviewport;
   DtColorModel colormodel;
   DtPFI routine;
   static DtFlag default_set = DcFalse;
   
   
   /* if clear flag is set, clear the depth buffer */
   if (dre_samplernd_render_private->viwdat->clear_flag == DcTrue) {
   	actviewport = DDdevice_InqActualViewport
		 (dre_samplernd_render_private->device_handle);

   	DDdevice_ClearRectangleDepthColor
		(dre_samplernd_render_private->device_handle,
   				(DtShort) actviewport->bll[0],
   				(DtShort) actviewport->bll[1],
   				(DtShort) actviewport->fur[0],
   				(DtShort) actviewport->fur[1]);
   }

  /*
   * Query the Dore default values for those attributes
   * that are stored in the renderer's private space.
   */

   if (default_set)
     return;

   routine = DDclass_InqMethod(DsInqClassId("AttDiffuseColor"),
                              DsInqMethodId("InqGlobalValue"));
   (*routine) (&colormodel, dre_samplernd_render_private->difclr);

   routine = DDclass_InqMethod(DsInqClassId("AttDiffuseIntens"),
                              DsInqMethodId("InqGlobalValue"));
   (*routine) (&dre_samplernd_render_private->difint);

   routine = DDclass_InqMethod(DsInqClassId("AttDiffuseSwitch"),
                              DsInqMethodId("InqGlobalValue"));
   (*routine) (&dre_samplernd_render_private->difswi);

   routine = DDclass_InqMethod(DsInqClassId("AttRepType"),
                              DsInqMethodId("InqGlobalValue"));
  (*routine) (&dre_samplernd_render_private->reptyp);

   routine = DDclass_InqMethod(DsInqClassId("AttLcstowcsmat"),
                         DsInqMethodId("InqGlobalValue"));
   (*routine) (dre_samplernd_render_private->lcstowcsmat); 

   /* Indicate that default values were set */
   default_set = DcTrue;
}

/*
===============================================================================
*/

/* Computation required after display traversal */

void
drr_samplernd_after_display_traversal()
{
   /*
    * flush device after display traversal is over
    */

   DDdevice_Flush(dre_samplernd_render_private->device_handle);
}

/*
===============================================================================
*/

