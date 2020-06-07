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
#include <dore/dore_develop/private/varsplymsh.h>

int _DCNULL[2];

DtObject DOVSPM (colormodel, vertexcount, vertlocs, vertnormals, vertcolors,
		 polycount, contours, vertexlist, shape, smoothflag)
    int *colormodel;
    DtInt *vertexcount;
    DtReal vertlocs[];
    DtReal vertnormals[];
    DtReal vertcolors[];
    DtInt *polycount;
    DtInt contours[];
    DtInt vertexlist[];
    int *shape;
    int *smoothflag;
{
    return dor_varsimplymsh_create(
	    (DtColorModel)*colormodel, *vertexcount,
	    (vertlocs == (DtReal *)_DCNULL) ? DcNullPtr : vertlocs, 
	    (vertnormals == (DtReal *)_DCNULL) ? DcNullPtr : vertnormals, 
	    (vertcolors == (DtReal *)_DCNULL) ? DcNullPtr : vertcolors,
	    *polycount,contours,vertexlist,*shape,*smoothflag);
}

/* ====================================================================== */

void DPUVSM (object, vertlocs, vertnormals, vertcolors, shape, decompose,
	     recompute_nrms)
    DtObject *object;
    DtReal vertlocs[];
    DtReal vertnormals[];
    DtReal vertcolors[];
    int *shape;
    int *decompose;
    int *recompute_nrms;
{
    dor_varsimplymsh_update(*object, 
	    (vertlocs == (DtReal *)_DCNULL) ? DcNullPtr : vertlocs,  
	    (vertnormals == (DtReal *)_DCNULL) ? DcNullPtr : vertnormals,  
	    (vertcolors == (DtReal *)_DCNULL) ? DcNullPtr : vertcolors,
	    (DtShapeType)*shape, 
	    (DtFlag)*decompose, (DtFlag)*recompute_nrms);
}

/* ====================================================================== */

void DPUSMV (object, uvindex, uvlist)
    DtObject *object;
    DtInt *uvindex;
    DtReal uvlist[];
{
    dor_varsimplymsh_update_uv(*object, *uvindex, uvlist);
}

/* ====================================================================== */
 
void DPUSMW (object, uvwindex, uvwlist)
    DtObject *object;
    DtInt *uvwindex;
    DtReal uvwlist[];
{
    dor_varsimplymsh_update_uvw(*object, *uvwindex, uvwlist);
}
