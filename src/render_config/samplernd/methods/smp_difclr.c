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
	void drr_samplernd_difclr_render ()
 ==========================================================================
*/

#include "../samplernd.h"

/* 
 * Method routine diffuse color global attribute
 */

void 
drr_samplernd_difclr_render ()
{
   DtColorModel colormodel;
   static DtPFI routine = (DtPFI)0;

   /* Get pointer to method routine */
   if (routine == (DtPFI)0) {
      routine = DDclass_InqMethod(DsInqClassId("AttDiffuseColor"), 
				 DsInqMethodId("InqGlobalValue"));
   }

   /* Query and store current value in renderer's space */
   (*routine) (&colormodel, dre_samplernd_render_private->difclr);

   printf ("samplernd: diffuse color is : (%g,%g,%g)\n",
   		dre_samplernd_render_private->difclr[0],
   		dre_samplernd_render_private->difclr[1],
   		dre_samplernd_render_private->difclr[2]);
}

