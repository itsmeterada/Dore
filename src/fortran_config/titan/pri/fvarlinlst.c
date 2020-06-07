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
 
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/varlinlst.h>

int _DCNULL[2];

DtObject DOVLNL (colormodel, linecount, vertices, vertnormals, vertcolors)
    int *colormodel;
    DtInt *linecount;
    DtReal vertices[];
    DtReal vertnormals[];
    DtReal vertcolors[];
{
    return dor_varlinlst_create(
		(DtColorModel)*colormodel, *linecount, 
		(vertices == (DtReal *)_DCNULL) ? DcNullPtr : vertices, 
		(vertnormals == (DtReal *)_DCNULL) ? DcNullPtr : vertnormals, 
		(vertcolors == (DtReal *)_DCNULL) ? DcNullPtr : vertcolors);
}

/* ====================================================================== */

void DPUVLL (object, linecount, vertlocs, vertnormals, vertcolors)
    DtObject *object;
    DtInt *linecount;
    DtReal vertlocs[];
    DtReal vertnormals[];
    DtReal vertcolors[];
{
    dor_varlinlst_update (*object, *linecount, 
		(vertlocs == (DtReal *)_DCNULL) ? DcNullPtr : vertlocs, 
                (vertnormals == (DtReal *)_DCNULL) ? DcNullPtr : vertnormals, 
                (vertcolors == (DtReal *)_DCNULL) ? DcNullPtr : vertcolors);
}
