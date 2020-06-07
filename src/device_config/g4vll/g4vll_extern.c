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
 
/*=======================================================================
  Functions:
	ddr_g4vll_return_functions
	ddr_g4vll_install_driver
=========================================================================*/

#include <dore/internal/dogen.h>
#include <dore/internal/driver.h>
#include <dore/dore_develop/develop.h>

#include "prom/prom.h"
#include "dcm/dcm.h"
#include "drom/drom.h"

/* ====================================================================== */

static void ddr_g4vll_return_functions (DtInt type, DtPtr *fcns)
{
    *fcns = (type == DDc_DCM)         ? ddr_g4vll_return_DCM_fcns ()
	  : (type == DDc_PROM)        ? ddr_g4vll_return_PROM_fcns ()
	  : (type == DDc_DROM)        ? ddr_g4vll_return_DROM_fcns ()
	  : (type == DDc_DROMMethods) ? ddr_g4vll_return_DROM_methods ()
	  : DcNullPtr;
}

/* ====================================================================== */

void ddr_g4vll_install_driver (char *name)
{
    DDdevice_InstallDriver
    (   name ? name:"g4vll", "G4 VLL Device Driver", ddr_g4vll_return_functions
    );

    DDclass_AddNotify (ddr_g4vll_drom_new_class);
}
