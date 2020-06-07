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

DtObject DoVarSimplePolygonMesh (
    DtColorModel colormodel,
    DtInt        vertexcount,
    DtReal       vertlocs[],
    DtReal       vertnormals[],
    DtReal       vertcolors[],
    DtInt        polycount,
    DtInt        contours[],
    DtInt        vertexlist[],
    DtShapeType  shape,
    DtFlag       smoothflag)
{
    return dor_varsimplymsh_create
	   (   colormodel, vertexcount, vertlocs, vertnormals, vertcolors,
	       polycount, contours, vertexlist, shape, smoothflag
	   );
}

/* ===================================================================== */

void DpUpdVarSimplePolygonMesh (
    DtObject    object,
    DtReal      vertlocs[],
    DtReal      vertnormals[],
    DtReal      vertcolors[],
    DtShapeType shape,
    DtFlag      decompose,
    DtFlag      recompute_nrms)
{
    dor_varsimplymsh_update
    (   object, vertlocs, vertnormals, vertcolors, shape, decompose,
	recompute_nrms
    );
}
 
/* ===================================================================== */

void DpUpdVarSimplePolygonMeshUV (
    DtObject object,
    DtInt    uvindex,
    DtReal   uvlist[])
{
    dor_varsimplymsh_update_uv (object, uvindex, (void*)uvlist);
}
 
/* ===================================================================== */

void DpUpdVarSimplePolygonMeshUVW (
    DtObject object,
    DtInt    uvwindex,
    DtReal   uvwlist[])
{
    dor_varsimplymsh_update_uvw (object, uvwindex, (void*)uvwlist);
}
