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
 
#include "method.h"

/*
 ======================================================================
 Functions:

   void dpr_radiosity_callback_show_render(object)

 ======================================================================
 */

void dpr_radiosity_callback_show_render (
    dot_object *object)
{
    static DtMethodPtr obj_exec_rtn;

    /* temporarily fudge current method such that all objects executed
        from the user function execute the show_render method */

    dpr_radiosity_set_current_method_show_render();

    obj_exec_rtn = DDobject_InqMethod(object, dpe_rad_base_method_id);
    (*obj_exec_rtn)(object);

    /* reset current method  to base method such that global attributes
      of base method be called */

}

/*
 ======================================================================
 */
