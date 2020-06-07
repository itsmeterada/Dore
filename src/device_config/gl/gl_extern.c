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

#include "gl_driver.h"
#include <dore/internal/driver.h>



/*=======================================================================
==  This routine returns a pointer to the structure of function pointers
==  for the given type.
=======================================================================*/

void ddr_gl_return_functions (
    DtInt type,		/* Type of Function List for Query */
    DtPtr *fcns)	/* Pointer to Function List Structure */
{
    switch (type)
    {   case DDc_DCM:         *fcns = ddr_gl_return_DCM_fcns();      break;
	case DDc_PROM:        *fcns = ddr_gl_return_PROM_fcns();     break;
	case DDc_DROM:        *fcns = ddr_gl_return_DROM_fcns();     break;
	case DDc_DROMMethods: *fcns = ddr_gl_return_DROM_methods();  break;
	default:              *fcns = 0;
    }
}



/*=======================================================================
==  This procedure installs the GL device driver.
=======================================================================*/

void ddr_gl_install_driver (char *name)
{
    DDdevice_InstallDriver
    (   name ? name : "gl",
	"SGI GL device driver",
	ddr_gl_return_functions
    );

    DDclass_AddNotify (ddr_gl_drom_new_class);
}
