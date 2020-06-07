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
 
#if 0
#include <dore/internal/dogen.h>
#include <dore/internal/driver.h>
#include <dore/dore_develop/develop.h>
#include "dcm/dcm.h"
#include "drom/drom.h"
#endif

#define  DEFINE_PEX_GLOBALS
#include "pex_driver.h"



/*****************************************************************************
**  This procedure sets a pointer to the DCM, PROM or DROM function mappings 
**  routines.  If an unknown device type is requested, the pointer is set to
**  nil.
*****************************************************************************/

static void  ddr_pex_return_functions (
    DtInt  type,	/* Type of Device */
    DtPtr *mapfunc)	/* Pointer to Mapping Function */
{
    switch (type)
    {   case DDc_DCM:         *mapfunc = ddr_pex_return_DCM_fcns();     break;
	case DDc_PROM:        *mapfunc = ddr_pex_return_PROM_fcns();    break;
	case DDc_DROM:        *mapfunc = ddr_pex_return_DROM_fcns();    break;
	case DDc_DROMMethods: *mapfunc = ddr_pex_return_DROM_methods(); break;
	default:              *mapfunc = 0;
    }
}



/***************************************************************************
**  This routine installs the PEX driver.
***************************************************************************/

void ddr_pex_install_driver (char *name)
{
    DDdevice_InstallDriver
	(name ? name : "pex", "PEX X11 Driver", ddr_pex_return_functions);

    DDclass_AddNotify (ddr_pex_drom_new_class);
}
