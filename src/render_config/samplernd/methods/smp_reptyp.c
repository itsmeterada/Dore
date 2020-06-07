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
	void drr_samplernd_reptyp_render ()
 ==========================================================================
*/

#include "../samplernd.h"

/* 
 * Method routine for representation type global attribute
 */

void 
drr_samplernd_reptyp_render ()
{
   static DtPFI routine = DcNullPtr;

   /* query current value of representation type attribute */
   if (routine == DcNullPtr) {
      routine = DDclass_InqMethod(DsInqClassId("AttRepType"), 
				 DsInqMethodId("InqGlobalValue"));
   }

   (*routine) (&dre_samplernd_render_private->reptyp);

   printf ("samplernd: representation type is : %d\n",
		 dre_samplernd_render_private->reptyp);
}

