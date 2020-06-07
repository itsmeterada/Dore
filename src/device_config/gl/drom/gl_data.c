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
	ddr_gl_drom_create_local_device_data
	ddr_gl_drom_create_local_view_data
	ddr_gl_drom_create_local_window_data
	ddr_gl_drom_delete_local_device_data
	ddr_gl_drom_delete_local_view_data
	ddr_gl_drom_delete_local_window_data

  ======================================================================
 */
#include "../gl_driver.h"



/* ====================================================================== */

DtPtr ddr_gl_drom_create_local_device_data (DtObject device)
{
    DevStatus.dore_device = device;

    return DcNullPtr;
}



/* ====================================================================== */

DtPtr ddr_gl_drom_create_local_view_data (DtObject view)
{
    auto struct view_data *vd;

#   ifdef DEBUGP
	printf("Creating local view data...\n");
#   endif

    DevStatus.dore_view = view;

    vd = DDspace_Allocate (sizeof (struct view_data));

    vd->clear_flag = DcTrue;

    return((DtPtr)vd);
}



/* ====================================================================== */

DtPtr ddr_gl_drom_create_local_window_data (
    DtObject device,
    DtObject view)
{
    auto struct window_data *wd;

#   ifdef DEBUGP
	printf("Creating local window data...\n");
#   endif

    wd = DDspace_Allocate(sizeof(struct window_data));

    return((DtPtr)wd);
}



/* ====================================================================== */

void ddr_gl_drom_delete_local_device_data (DtPtr data)
{
}



/* ====================================================================== */

void ddr_gl_drom_delete_local_view_data (DtPtr data)
{
    if (data) DDspace_Deallocate (data);
}



/* ====================================================================== */

void ddr_gl_drom_delete_local_window_data (DtPtr data)
{
    if (data) DDspace_Deallocate (data);
}
