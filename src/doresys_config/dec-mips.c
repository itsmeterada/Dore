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
 
/*****************************************************************************
*****************************************************************************/

#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/internal/driver.h>


/****************************************************************************/

void dor_doresys_initialize (DtInt processors)
{
    /* Empty */
}


/****************************************************************************/

void dor_doresys_initialize_drivers (void)
{
    ddr_rasterfile_install_driver ("rasterfile");
    ddr_ps_install_driver         ("ps");
    ddr_pex_install_driver        ("pex");
    ddr_stdx11_install_driver     ("stdx11");
}


/****************************************************************************/

void dor_doresys_initialize_renderers (void)
{
    drr_dynrnd_install_renderer (DcRealTime);
    drr_glbrnd_install_renderer (DcProductionTime);
}


/****************************************************************************/

void dor_doresys_terminate (void)
{
    /* Empty */
}
