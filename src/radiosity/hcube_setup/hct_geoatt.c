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

   void dpr_radiosity_geoatt_hcube_setup(object)

 ======================================================================
 */

void dpr_radiosity_geoatt_hcube_setup (
    dot_object *object)
{
    DtMethodPtr query_rtn = DcNullPtr;

    /* this function is executed on any object of one of the following
       classes:

       DoRotate
       DoTranslate
       DoShear
       DoScale
       DoTransformMatrix
       DoLookAtFrom
       DoPushMatrix
       DoPopMatrix
       DoPushAtts
       DoPopAtts
  
       We add all geometric attributes to the new group, whether
       radswi is on or off, to get correct transformations if
       the radiosity switch is turned on in the middle of a
       group.
    */

    DgAddObjToGroup(dpe_rad_hcube_data.display_group, object);

    /* execute such that our global attribute variables get
       updated */

    if (query_rtn == DcNullPtr)
        query_rtn = DDclass_InqMethod(DsInqObjClassId(object),
                    DsInqMethodId("StdRenderDisplay"));
    (*query_rtn) (object);

}
/*
 ======================================================================
 */

