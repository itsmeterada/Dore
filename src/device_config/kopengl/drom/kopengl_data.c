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
   ddr_kopengl_drom_create_local_device_data 
   ddr_kopengl_drom_create_local_view_data 
   ddr_kopengl_drom_create_local_window_data 
   ddr_kopengl_drom_delete_local_device_data
   ddr_kopengl_drom_delete_local_view_data 
   ddr_kopengl_drom_delete_local_window_data

 **************************************************************************/



#include "drom.h"




/*****************************************************************************
 *  The renderer calls this routine to allocate a chunk of memory to hold the
 *  state information associated with the device `device', and returns a
 *  pointer to the allocated structure.  This data pointer will later be
 *  passed back to the DROM by a call to drom.start_update.  It is never
 *  accessed by the renderer, just handed back to the DROM.
 ****************************************************************************/

DtPtr 
ddr_kopengl_drom_create_local_device_data (DtObject device)
{
 ddt_kopengl_drom_device *devicedata;

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.create_local_device_data\n");
#endif

 devicedata = DDspace_Allocate (sizeof (ddt_kopengl_drom_device));

 DROM.device = device;
 DEVICE = devicedata;

 return (DtPtr) devicedata;
}



/*****************************************************************************
 *  The renderer calls this routine to allocate memory for the view object
 *  `view', and returns a pointer to this structure.  The pointer will later
 *  be passed back to the DROM by a call to drom.start_update.  It is never
 *  accessed by the renderer, just handed back to the DROM.
 ****************************************************************************/

DtPtr 
ddr_kopengl_drom_create_local_view_data (DtObject view)
{
 ddt_kopengl_drom_view *viewdata;

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.create_local_view_data\n");
#endif

 viewdata = (ddt_kopengl_drom_view *) DDspace_Allocate
  (sizeof (ddt_kopengl_drom_view));

 viewdata->total_lights = 0;
 viewdata->max_lights = GL_MAX_LIGHTS; 

 DROM.view = view;
 VIEW = viewdata;

 return (DtPtr) viewdata;
}


/*****************************************************************************
 *  The renderer calls this routine to allocate memory for the device/view
 *  combination, and returns a pointer to this memory.  Note that other
 *  functions maintain data for the view and the device separately; this data
 *  is just for state associated with the combination of this view and this
 *  device.  The data pointer will later be passed back to the DROM by a call
 *  to drom.start_update.  It is never accessed by the renderer, just handed
 *  back to the DROM.
 ****************************************************************************/

DtPtr 
ddr_kopengl_drom_create_local_window_data (DtObject device,
					   DtObject view)
{
 ddt_kopengl_drom_window *windowdata;

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.create_local_window_data\n");
#endif

 windowdata = (ddt_kopengl_drom_window *) DDspace_Allocate
  (sizeof (ddt_kopengl_drom_window));

 WINDOW = windowdata;

 return (DtPtr) windowdata;
}



/*****************************************************************************
 *  The renderer calls this function to delete the DROM's local device data,
 *  previously created with a call to drom_create_local_device_data.
 ****************************************************************************/

void 
ddr_kopengl_drom_delete_local_device_data (ddt_kopengl_drom_device * data)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.delete_local_device_data\n");
#endif

 if (data != (ddt_kopengl_drom_device *) 0)
  DDspace_Deallocate (data);
}



/*****************************************************************************
 *  The renderer calls this function to delete the DROM's local view data,
 *  previously created with a call to drom.create_local_view_data.
 ****************************************************************************/

void 
ddr_kopengl_drom_delete_local_view_data (ddt_kopengl_drom_view * data)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.delete_local_view_data\n");
#endif

 if (data)
   {
    DDspace_Deallocate (data);
   }
}


/*****************************************************************************
 *  The renderer calls this function to delete the DROM's local window data,
 *  previously created with a call to drom.create_local_window_data.
 ****************************************************************************/

void 
ddr_kopengl_drom_delete_local_window_data (ddt_kopengl_drom_window * data)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.delete_local_window_data\n");
#endif

 if (data)
  DDspace_Deallocate (data);
}
