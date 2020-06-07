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
	DtPtr ddr_softpipe_drom_create_local_device_data (device)
	DtPtr ddr_softpipe_drom_create_local_view_data (view)
	DtPtr ddr_softpipe_drom_create_local_window_data (device,view)
	void ddr_softpipe_drom_delete_local_device_data (data)
	void ddr_softpipe_drom_delete_local_view_data (data)
	void ddr_softpipe_drom_delete_local_window_data (data)

  ======================================================================
 */
#include "drom.h"
#include <dore/internal/matrix.h>

/* ====================================================================== */

DtPtr ddr_softpipe_drom_create_local_device_data (DtObject device)
{
   struct dynout_device_data *device_data;

#ifdef DEBUG
   printf("...processing dynout create local device data \n");
   fflush(stdout);
#endif

   device_data = (struct dynout_device_data *)DDspace_Allocate
	 ((sizeof(struct dynout_device_data)));

   dde_softpipe_drom_data.current_device = device;

   return((DtPtr)device_data);
}

/* ====================================================================== */

DtPtr ddr_softpipe_drom_create_local_view_data (DtObject view)
{
   struct dynout_view_data *viewdata;

#  ifdef DEBUG
      printf("...processing dynout local view data\n");
      fflush(stdout);
#  endif

   viewdata = DDspace_Allocate ((sizeof(struct dynout_view_data)));

   viewdata->camera_data =DDspace_Allocate((sizeof(struct dynout_camera_data)));

   dor_matrix_load_identity (viewdata->camera_data->projmat);
   dor_matrix_load_identity (viewdata->camera_data->ccstowcsmat);
   dor_matrix_load_identity (viewdata->camera_data->wcstoccsmat);
   dor_matrix_load_identity (viewdata->camera_data->npctowcsmat);
   dor_matrix_load_identity (viewdata->camera_data->wcstofcsmat);

   viewdata->light_data  =DDspace_Allocate((sizeof(struct dynout_light_data)));

   viewdata->light_data->count = 0;
   viewdata->light_data->max_count = 0;
   viewdata->light_data->cosine = NULL;
   viewdata->light_data->intens = NULL;
   viewdata->light_data->color = NULL;
   viewdata->light_data->h = NULL;
   viewdata->light_data->lcscosine = NULL;
   viewdata->light_data->scaledcolor = NULL;

   return((DtPtr)viewdata);
}

/* ====================================================================== */

DtPtr ddr_softpipe_drom_create_local_window_data (
   DtObject device,
   DtObject view)
{
   struct dynout_window_data *windata;

#ifdef DEBUG
   printf("...processing dynout create local window data\n");
   fflush(stdout);
#endif

   windata = (struct dynout_window_data *)DDspace_Allocate
	 ((sizeof(struct dynout_window_data)));

   return ((DtPtr)windata);
}

/* ====================================================================== */

void ddr_softpipe_drom_delete_local_device_data (DtPtr data)
{
   if (data) DDspace_Deallocate (data);
}

/* ====================================================================== */

void ddr_softpipe_drom_delete_local_view_data (DtPtr data)
{
   if (data) {
      if (((struct dynout_view_data*)(data))->camera_data) {
	 DDspace_Deallocate (((struct dynout_view_data*)(data))->camera_data);
      }
      if (((struct dynout_view_data*)(data))->light_data) {
	 DDspace_Deallocate (((struct dynout_view_data*)(data))->light_data);
      }
      DDspace_Deallocate (data);
   }
}

/* ====================================================================== */

void ddr_softpipe_drom_delete_local_window_data (DtPtr data)
{
   if (data) DDspace_Deallocate (data);
}
