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
	int drr_glbrnd_camera_initialize (object)

  ======================================================================
 */
#include "../glbrnd.h"
#include <dore/internal/matrix.h>

void drr_glbrnd_camera_initialize (dot_object *object)
{
   DtObject camera;

   camera = DvInqActiveCamera (dre_glbrnd_data.current_view);

   if (camera == DcNullObject || camera == (DtObject) object) {

      dor_matrix_load(dre_glbrnd_curcamdat->projmat, 
		      dre_glbrnd_data.projmat);
      dre_glbrnd_curcamdat->viewtype = dre_glbrnd_curcamdat->viewtype;

      drr_glbrnd_initialize_ccstowcsmat();

      drr_rayint_initialize_camera(object);

      drr_glbshd_initialize_camera();
   }
}
