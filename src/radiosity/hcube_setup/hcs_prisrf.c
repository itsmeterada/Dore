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
#include "dore/dore_develop/private/prisrf.h"

/*
 ======================================================================
 Functions:

   void dpr_radiosity_prisrf_hcube_setup()

 ======================================================================
 */

void dpr_radiosity_prisrf_hcube_setup (
    dot_object *object)
{
    DtMethodPtr exec_rtn;
    struct prisrf *prisrf;

    if (!DDcondex_QueryRender(DsInqClassId("DoPrimSurf")))
         return;
       
    exec_rtn = DDclass_InqMethod(DsInqObjClassId(object),
                    DsInqMethodId("StdRenderDisplay"));

    (*exec_rtn) (object);

    /* increment reference count of alternate object such that
       it is not deleted when the hcube display group is deleted */
    prisrf = (struct prisrf *) object->data;
    if (prisrf->alternate_object != DcNullObject)
       (prisrf->alternate_object)->ref_count += 1;

}

/*
 ======================================================================
 */
