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
 
#include "../pex_driver.h"

#define DEBUG 0		/* General Debug Switch */



/*****************************************************************************
**  The renderer calls this routine to allocate a chunk of memory to hold the
**  state information associated with the device `device', and returns a
**  pointer to the allocated structure.  This data pointer will later be
**  passed back to the DROM by a call to drom.start_update.  It is never
**  accessed by the renderer, just handed back to the DROM.
*****************************************************************************/

DtPtr  ddr_pex_drom_create_local_device_data  (DtObject device)
{
    auto ddt_pex_drom_device  *devicedata;	/* DROM Device Data */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_create_local_device_data (%lx)\n", device);
	fflush (stdout);
#   endif

    devicedata = DDspace_Allocate (sizeof(ddt_pex_drom_device));

    DROM.device = device;
    DEVICE = devicedata;

    return (DtPtr) devicedata;
}


/*****************************************************************************
**  The renderer calls this routine to allocate memory for the view object
**  `view', and returns a pointer to this structure.  The pointer will later
**  be passed back to the DROM by a call to drom.start_update.  It is never
**  accessed by the renderer, just handed back to the DROM.
*****************************************************************************/

DtPtr  ddr_pex_drom_create_local_view_data  (DtObject view)
{
    auto ddt_pex_drom_view *view_data;	/* DROM View Data */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_create_local_view_data (%lx)\n", view);
	fflush (stdout);
#   endif

    view_data = DDspace_Allocate (sizeof (ddt_pex_drom_view));

    DROM.view = view;
    VIEW = view_data;

    VIEW->background_color_model = DcRGB;
    VIEW->background_color[0] = DCM->bg_color[0];
    VIEW->background_color[1] = DCM->bg_color[1];
    VIEW->background_color[2] = DCM->bg_color[2];

    return (DtPtr) view_data;
}



/*****************************************************************************
**  The renderer calls this routine to allocate memory for the device/view
**  combination, and returns a pointer to this memory.  Note that other
**  functions maintain data for the view and the device separately; this data
**  is just for state associated with the combination of this view and this
**  device.  The data pointer will later be passed back to the DROM by a call
**  to drom.start_update.  It is never accessed by the renderer, just handed
**  back to the DROM.
*****************************************************************************/

DtPtr  ddr_pex_drom_create_local_window_data  (
    DtObject device,
    DtObject view)
{
    auto ddt_pex_drom_window *windowdata;	/* DROM Window Data */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_create_local_window_data (%lx, %lx)\n",
	    device, view);
	fflush (stdout);
#   endif

    windowdata = DDspace_Allocate (sizeof (ddt_pex_drom_window));

    WINDOW = windowdata;

    return (DtPtr) windowdata;
}



/*****************************************************************************
**  The renderer calls this function to delete the DROM's local device data,
**  previously created with a call to drom.create_local_device_data.
*****************************************************************************/

void  ddr_pex_drom_delete_local_device_data  (DtPtr dev_data)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_delete_local_device_data (%lx)\n", dev_data);
	fflush (stdout);
#   endif

    PEXFreeLookupTable (DCM->display, DROM.pexview_LUT);
    PEXFreeLookupTable (DCM->display, DROM.light_LUT);
    PEXFreeLookupTable (DCM->display, DROM.depthcue_LUT);

    DDspace_Deallocate (DROM.matrix_stack);
    DDspace_Deallocate (DROM.light_heap);

    /* Free the PEX renderer. */

    PEXFreeRenderer (DCM->display, PEX.renderer);

    if (dev_data) DDspace_Deallocate (dev_data);
}



/*****************************************************************************
**  The renderer calls this function to delete the DROM's local view data,
**  previously created with a call to drom.create_local_view_data.
*****************************************************************************/

void  ddr_pex_drom_delete_local_view_data  (DtPtr view_data)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_delete_local_view_data (%lx)\n", view_data);
	fflush (stdout);
#   endif

    if (view_data) DDspace_Deallocate (view_data);
}



/*****************************************************************************
**  The renderer calls this function to delete the DROM's local window data,
**  previously created with a call to drom.create_local_window_data.
*****************************************************************************/

void  ddr_pex_drom_delete_local_window_data  (DtPtr win_data)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_delete_local_window_data (%lx)\n", win_data);
	fflush (stdout);
#   endif

    if (win_data) DDspace_Deallocate (win_data);
}
