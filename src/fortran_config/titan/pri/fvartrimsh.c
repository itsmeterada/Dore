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
#include <dore/dore_develop/private/vartrimsh.h>

int _DCNULL[2];

DtObject DOVTRM (colormodel, vertexcount, vertices, vertnormals, vertcolors,
		 trianglecount, triangles, smoothflag)
    int *colormodel;
    DtInt *vertexcount;
    DtReal vertices[];
    DtReal vertnormals[];
    DtReal vertcolors[];
    DtInt *trianglecount;
    DtInt triangles[];
    int *smoothflag;
{
    return(dor_vartrimsh_create(
	    (DtColorModel)*colormodel, 
	    *vertexcount, 
	    (vertices == (DtReal *)_DCNULL) ? DcNullPtr : vertices, 
	    (vertnormals == (DtReal *)_DCNULL) ? DcNullPtr : vertnormals, 
	    (vertcolors == (DtReal *)_DCNULL) ? DcNullPtr : vertcolors,
	    *trianglecount, triangles, 
	    (DtFlag)*smoothflag));
}

/* ====================================================================== */

void DPUVTM (object, vertlocs, vertnormals, vertcolors, recompute_nrms)
    DtObject *object;
    DtReal vertlocs[];
    DtReal vertnormals[];
    DtReal vertcolors[];
    int *recompute_nrms;
{
    dor_vartrimsh_update(*object, 
	    (vertlocs == (DtReal *)_DCNULL) ? DcNullPtr : vertlocs,  
	    (vertnormals == (DtReal *)_DCNULL) ? DcNullPtr : vertnormals,  
	    (vertcolors == (DtReal *)_DCNULL) ? DcNullPtr : vertcolors,
	    (DtFlag) *recompute_nrms);
}
 
/* ====================================================================== */

void DPUTMV (object, uvindex, uvlist)
    DtObject *object;
    DtInt *uvindex;
    DtReal uvlist[];
{
    dor_vartrimsh_update_uv(*object, *uvindex, uvlist);
}
 
/* ====================================================================== */

void DPUTMW (object, uvwindex, uvwlist)
    DtObject *object;
    DtInt *uvwindex;
    DtReal uvwlist[];
{
    dor_vartrimsh_update_uvw(*object, *uvwindex, uvwlist);
}
