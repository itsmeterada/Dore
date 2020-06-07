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

   void dpr_radiosity_lcswcs_hcube_setup()

 ======================================================================
 */

void dpr_radiosity_lcswcs_hcube_setup (void)
{
    DtMethodPtr query_rtn = DcNullPtr;

    if (query_rtn == DcNullPtr)
        query_rtn = DDclass_InqMethod(DsInqClassId("AttLcstowcsmat"),
                    DsInqMethodId("InqGlobalValue"));

    (*query_rtn) (dpe_rad_hcube_setup_att.lcswcs);

}
/*
 ======================================================================
 */

