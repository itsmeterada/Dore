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
 
#include "rad.h"
#include <dore/dore_develop/private/device.h>

/*
 ======================================================================
 Functions:

   dpr_radiosity_device_update_view
   dpr_radiosity_swap_check

 ======================================================================
 */

void dpr_radiosity_device_update_view (
    dot_object *device,
    dot_object *view)
{
    dor_render_render(device,view);

    /* check if we need to swap buffers: for DcRadiosityComputeDisp loop
       we need to swap, unless this is the last frame generated in
       which case Dore will do the swapping (as called by Df/Dv/DdUpdate) */

    if (dpe_rnd_updtype == DcRadiosityComputeDisp && !dpe_rad_converged) 
        dpr_radiosity_swap_check();
}

/*
  ======================================================================
 */

void dpr_radiosity_swap_check(void)
{
   OUTFUN(swap_buffers)();
}

/*
  ======================================================================
 */
