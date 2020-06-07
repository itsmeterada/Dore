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
	dor_doresys_initialize (processors)
	dor_doresys_initialize_drivers ()
	dor_doresys_initialize_renderers ()
	dor_doresys_terminate ()

  ======================================================================
 */
#include "dore/internal/dogen.h"
/*
 ======================================================================
 */

dor_doresys_initialize (processors)
DtInt processors;
{
}
/*
 ======================================================================
 */

dor_doresys_initialize_drivers ()
{
    ddr_rasterfile_install_driver(DcNullPtr);
    ddr_sunview_cxp_install_driver(DcNullPtr);
    ddr_sunview_cxp_install_driver("sunview_cxp");
    ddr_sunview_gen_install_driver(DcNullPtr);
    ddr_sunview_gen_install_driver("sunview_gen");
    ddr_ps_install_driver(DcNullPtr);
}
/*
 ======================================================================
 */

dor_doresys_initialize_renderers ()
{
    drr_dynrnd_install_renderer(DcRealTime);

    drr_glbrnd_install_renderer(DcProductionTime);
}
/*
 ======================================================================
 */

dor_doresys_terminate ()
{
}
