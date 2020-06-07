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
#include <dore/dore_develop/private/varquadmsh.h>

DtObject DoVarQuadMesh (
    DtColorModel colormodel,
    DtInt        vertexcount,
    DtReal      *vertlocs,
    DtReal      *vertnormals,
    DtReal      *vertcolors,
    DtInt        quadcount,
    DtInt        quads[],
    DtFlag       smoothflag)

{
    return(dor_varquadmsh_create(colormodel, vertexcount, vertlocs, 
	vertnormals, vertcolors, quadcount, (void *)quads, smoothflag));
}

/* ===================================================================== */

void DpUpdVarQuadMesh (
    DtObject  object,
    DtReal   *vertlocs,
    DtReal   *vertnormals,
    DtReal   *vertcolors,
    DtFlag    recompute_nrms)

{
    dor_varquadmsh_update
	(object, vertlocs, vertnormals, vertcolors, recompute_nrms);
}
 
/* ===================================================================== */

void DpUpdVarQuadMeshUV (
    DtObject object,
    DtInt    uvindex,
    DtReal  *uvlist)

{
    dor_varquadmsh_update_uv(object, uvindex, (void *)uvlist);
}
 
/* ===================================================================== */

void DpUpdVarQuadMeshUVW (
    DtObject object,
    DtInt    uvwindex,
    DtReal  *uvwlist)

{
    dor_varquadmsh_update_uvw (object, uvwindex, (void *)uvwlist);
}
