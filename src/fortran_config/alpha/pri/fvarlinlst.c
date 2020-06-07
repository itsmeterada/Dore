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
 
#include "../fortran.h"
#include <dore/dore_develop/private/varlinlst.h>


DtObject dovlnl_ (colormodel, linecount, vertlocs, vertnormals, vertcolors)
    DtColorModel *colormodel;
    DtInt *linecount;
    DtReal vertlocs[];
    DtReal vertnormals[];
    DtReal vertcolors[];
{
    return dor_varlinlst_create
	   (   *colormodel, *linecount, FORTRAN_PTR (vertlocs),
	       FORTRAN_PTR (vertnormals), FORTRAN_PTR (vertcolors)
	   );
}

/* ====================================================================== */

void dpuvll_ (object, linecount, vertlocs, vertnormals, vertcolors)
    DtObject *object;
    DtInt *linecount;
    DtReal vertlocs[];
    DtReal vertnormals[];
    DtReal vertcolors[];
{
    dor_varlinlst_update
    (   *object, *linecount, FORTRAN_PTR (vertlocs),
	FORTRAN_PTR (vertnormals), FORTRAN_PTR (vertcolors)
    );
}
