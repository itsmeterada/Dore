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

  Functions:
    void ddr_kopengl_return_functions (type, fcns)
    void ddr_kopengl_install_driver   (name)

**************************************************************************/

#include <stdio.h>
#include <dore/dore.h>
#include <dore/dore_develop/develop.h>


/*****************************************************************************
 *  This procedure sets a pointer to the DCM, PROM or DROM function mappings
 *  routines.  If an unknown device type is requested, the pointer is set to
 *  nil.
 ****************************************************************************/

static void ddr_kopengl_return_functions (DtInt type, DtPtr *fcns)
 {
  extern DtPtr ddr_kopengl_return_DCM_fcns ();
  extern DtPtr ddr_kopengl_return_PROM_fcns ();
  extern DtPtr ddr_kopengl_return_DROM_fcns ();

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ddr_kopengl_return_functions\n");
#endif

  switch (type) 
    {
     case DDc_DCM:
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ..returning DCM functions\n");
#endif
       *fcns = ddr_kopengl_return_DCM_fcns();
       break;

    case DDc_PROM:
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ..returning PROM functions\n");
#endif
       *fcns = ddr_kopengl_return_PROM_fcns();
       break;

    case DDc_DROM:
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ..returning DROM functions\n");
#endif
       *fcns = ddr_kopengl_return_DROM_fcns();
       break;

    default:
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ..invalid option in ddr_kopengl_return_functions\n");
#endif
       *fcns = (DtPtr)0;
       break;
   }
 }




/***************************************************************************
 *  This routine installs the Kubota OpenGL driver.
 **************************************************************************/

void ddr_kopengl_install_driver (DtPtr name)
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ddr_kopengl_install_driver\n");
#endif

  DDdevice_InstallDriver ( name ? name : "kopengl", 
                           (DtPtr)"Kubota OpenGL Driver",
	                   ddr_kopengl_return_functions );

  return;
 }
