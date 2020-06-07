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
#include "dore/dore_develop/private/plymsh.h"

/*
 ======================================================================
 Functions:

   void dpr_radiosity_plymsh_hcube_setup()

 ======================================================================
 */

void dpr_radiosity_plymsh_hcube_setup (
    dot_object *object)
{
    DtMethodPtr exec_rtn;
    struct plymsh *plymsh;

    /* no radiosity allowed on alt. line representation */
    if (dpe_rad_hcube_setup_att.reptyp == DcOutlines) 
       return;

    /* return if object is invisible (DoInvisSwitch) or if the class
       is not executable (DoExecSet) */
    if (!DDcondex_QueryRender(DsInqClassId("DoPolygonMesh")))
         return;
       
    exec_rtn = DDclass_InqMethod(DsInqObjClassId(object),
                    DsInqMethodId("StdRenderDisplay"));

    (*exec_rtn) (object);

    /* increment reference count of alternate object such that
       it is not deleted when the hcube display group is deleted */
    plymsh = (struct plymsh *) object->data;
    if (plymsh->alt_object_tris != DcNullObject)
       (plymsh->alt_object_tris)->ref_count += 1;

}

/*
 ======================================================================
 */
