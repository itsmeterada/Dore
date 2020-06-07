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
#include <dore/dore_develop/private/varquadgrd.h>

DtObject dovqdg_ (colormodel, width, height, vertices, vertnormals,
		  vertcolors, smoothflag)
    DtColorModel *colormodel;
    DtInt *width;
    DtInt *height;
    DtReal vertices[];
    DtReal vertnormals[];
    DtReal vertcolors[];
    DtFlag *smoothflag;
{
    return dor_varquadgrid_create
	   (   *colormodel, *width, *height, FORTRAN_PTR (vertices), 
               FORTRAN_PTR (vertnormals), FORTRAN_PTR (vertcolors),
	       *smoothflag
	   );
}

/* ====================================================================== */

void dpuvqg_ (object, vertlocs, vertnormals, vertcolors, recompute_nrms)
    DtObject *object;
    DtReal vertlocs[];
    DtReal vertnormals[];
    DtReal vertcolors[];
    DtFlag *recompute_nrms;
{
    dor_varquadgrid_update
    (   *object, FORTRAN_PTR (vertlocs),  FORTRAN_PTR (vertnormals),
        FORTRAN_PTR (vertcolors), *recompute_nrms
    );
}

/* ====================================================================== */
 
void dpuqgv_ (object, uvindex, uvlist)
    DtObject *object;
    DtInt *uvindex;
    DtRealCouple uvlist[];
{
    dor_varquadgrid_update_uv (*object, *uvindex, uvlist);
}

/* ====================================================================== */
 
void dpuqgw_ (object, uvwindex, uvwlist)
    DtObject *object;
    DtInt *uvwindex;
    DtRealTriple uvwlist[];
{
    dor_varquadgrid_update_uvw (*object, *uvwindex, uvwlist);
}
