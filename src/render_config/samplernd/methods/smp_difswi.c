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
	void drr_samplernd_difswi_render ()
 ==========================================================================
*/

#include "../samplernd.h"

/* 
 * Method routine for diffuse switch global attribute
 */

void
drr_samplernd_difswi_render ()
{
   static DtPFI routine = DcNullPtr;

   if (routine == DcNullPtr) {
      routine = DDclass_InqMethod(DsInqClassId("AttDiffuseSwitch"), 
				 DsInqMethodId("InqGlobalValue"));
   }

   (*routine) (&dre_samplernd_render_private->difswi);

   printf ("samplernd: diffuse switch (%s)\n",
		 dre_samplernd_render_private->difswi ? "DcOn" : "DcOff");
}

