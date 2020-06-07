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

/**************************************************************************

 This file contains some functions and routines for setting colors 

  Functions:
    ddr_kopengl_restore_specular_color 
    ddr_kopengl_restore_diffuse_color 
    ddr_kopengl_restore_specular_factor

***************************************************************************/


#include "drom.h"

/*****************************************************************************
 *  Restores the specular color as specified. Does nothing in pseudocolor mode 
 ****************************************************************************/

void 
ddr_kopengl_restore_specular_color (void)
{
 GLfloat param[4];

 if (DEVICE->visual_type == DcPseudoColor)
   return;

 param[0] = DROM.spcclr[0] * DROM.spcint;
 param[1] = DROM.spcclr[1] * DROM.spcint;
 param[2] = DROM.spcclr[2] * DROM.spcint;
 param[3] = 1.0;

 glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, param);
}

/*****************************************************************************
 *  Restores the specular factor. Does nothing in pseudocolor mode 
 ****************************************************************************/

void
ddr_kopengl_restore_specular_factor (void)
{
 GLfloat param;

 if (DEVICE->visual_type == DcPseudoColor)
   return;

 param = DROM.spcswi ? DROM.spcfct : 0.0;
 glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, param);

}

/*****************************************************************************
 *  Restores the diffuse color as specified. 
 ****************************************************************************/

void 
ddr_kopengl_restore_diffuse_color (void)
{

 /* Pseudo-Color */

 if (DEVICE->visual_type == DcPseudoColor)
   {
   }

 /* True Color */

 glColor3fv (DROM.difclr);
}
