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
 
/* ======================================================================
  Functions:
	dor_pick_all
	dor_pick_closest
	dor_pick_first
  ====================================================================== */

#include <dore/internal/dogen.h>
#include "pick.h"



/* ====================================================================== */

DtHitStatus dor_pick_all (
    DtPtr    data,
    DtInt    PathElements,
    long     Path[],
    DtReal   ZValue,
    DtObject View,
    DtInt    HitsAccepted)
{
    return DcHitAccept;
}



/* ====================================================================== */

DtHitStatus dor_pick_closest (
    DtPtr    data,
    DtInt    PathElements,
    long     Path[],
    DtReal   ZValue,
    DtObject View,
    DtInt    HitsAccepted)
{
    DtHitStatus return_status;
    static DtReal BestSoFar = 0;
	
    if ((HitsAccepted == 0) || (ZValue > BestSoFar)) {
	BestSoFar = ZValue;
	return_status = DcHitOverwrite;
    } else {
	return_status = DcHitReject;
    }

    return return_status;
}



/* ====================================================================== */

DtHitStatus dor_pick_first (
    DtPtr    data,
    DtInt    PathElements,
    long     Path[],
    DtReal   ZValue,
    DtObject View,
    DtInt    HitsAccepted)
{
    DsExecutionAbort ();
    return DcHitAccept;
}
