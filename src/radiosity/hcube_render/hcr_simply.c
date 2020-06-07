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
#include "dore/dore_develop/private/simply.h"

/*
 ======================================================================
 Functions:
    void dpr_radiosity_simply_hcube_render(object)

 ======================================================================
 */

void dpr_radiosity_simply_hcube_render (
    dot_object *object)
{
    static DtMethodPtr exec_rtn = DcNullPtr;
    struct simply *simply;
    DtObject alt_obj;

    /* force use of alternate object */

    simply = (struct simply *) object->data;

    alt_obj = simply->alt_object_tris;

    if (exec_rtn == DcNullPtr)
       exec_rtn =
            DDobject_InqMethod(alt_obj,
                     dpe_rad_hcube_data.hcube_render_method_id);

    (*exec_rtn)(alt_obj);
}
