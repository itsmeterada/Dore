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
	int drr_recrnd_initialize_subsystem ()
	int drr_recrnd_initialize_device_and_view ()

  ======================================================================
 */
#include "recrnd.h"
#include <dore/internal/matrix.h>

struct recrnd_data dre_recrnd_data;

/*
 ======================================================================
 */

void drr_recrnd_initialize_subsystem (void)
{
    /*
      dre_recrnd_data.view_ray.origin_rayint_object =
      (struct rayint_object *)NULL;
      */

    dre_recrnd_data.view_ray.current_box = NULL;
}
/*
 ======================================================================
 */

void drr_recrnd_initialize_device_and_view (void)
{
    DtVolume *viewport;
    DtColorModel backgroundcolormodel;
    DtColorRGB backgroundcolor;
    DtVolume devext;

    if (dre_glbrnd_initialize_studio) {	      
	viewport = dor_device_inq_actual_viewport_ptr(
				      dre_glbrnd_data.current_device);

	dor_device_inq_extent(dre_glbrnd_data.current_device,&devext);

	dre_recrnd_data.resolution_x = 
			viewport->fur[0] - viewport->bll[0];
	dre_recrnd_data.resolution_y = 
			viewport->fur[1] - viewport->bll[1];
		
	dre_recrnd_data.pixel_window_x_minimum = 
			viewport->bll[0] - devext.bll[0];
	dre_recrnd_data.pixel_window_y_minimum = 
			viewport->bll[1] - devext.bll[1];

	dre_recrnd_data.pixel_width = 
			2. / (DtReal)dre_recrnd_data.resolution_x;
	dre_recrnd_data.pixel_height = 
			2. / (DtReal)dre_recrnd_data.resolution_y;

	dor_view_inq_background_color(dre_glbrnd_data.current_view,
			     	&backgroundcolormodel,backgroundcolor);

	dre_recrnd_data.background_color[0] = backgroundcolor[0];
	dre_recrnd_data.background_color[1] = backgroundcolor[1];
	dre_recrnd_data.background_color[2] = backgroundcolor[2];

	dor_matrix_load(dre_recrnd_data.fcstowcsmat,
			dre_glbrnd_curcamdat->wcstofcsmat);
	dor_matrix_invert(dre_recrnd_data.fcstowcsmat);
    }
}
