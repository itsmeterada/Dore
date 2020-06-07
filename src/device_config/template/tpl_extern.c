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
  ======================================================================
  Functions:
	void ddr_template_return_functions (type, fcns)
	void ddr_template_install_driver (name)

  ======================================================================
 */
#include <dore/dore.h>
#include <dore/dore_develop/develop.h>

/* ====================================================================== */

static void ddr_template_return_functions (
    DtInt type,
    DtPtr *fcns)
{
    extern DtPtr ddr_template_return_DCM_fcns();
    extern DtPtr ddr_template_return_PROM_fcns();
    extern DtPtr ddr_template_return_DROM_fcns();

    switch (type) {
    case DDc_DCM:
	*fcns = ddr_template_return_DCM_fcns();
	break;

    case DDc_PROM:
	*fcns = ddr_template_return_PROM_fcns();
	break;

    case DDc_DROM:
	*fcns = ddr_template_return_DROM_fcns();
	break;

    default:
	*fcns = (DtPtr) 0;
	break;
    }
}

/* ====================================================================== */

void ddr_template_install_driver (char *name)
{
    DDdevice_InstallDriver
	    ((name == DcNullPtr) ? (DtPtr)"template" : name,
	     (DtPtr)"Template Driver",
	     (DtPFI)ddr_template_return_functions);
}
