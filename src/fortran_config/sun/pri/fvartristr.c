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
#include <dore/dore_develop/private/vartristrp.h>

extern int dcnull_[2];

DtObject dovtrs_ (colormodel, vertexcount, vertices, vertnormals, vertcolors)
    int *colormodel;
    DtInt *vertexcount;
    DtReal vertices[];
    DtReal vertnormals[];
    DtReal vertcolors[];
{
    return(dor_vartristrip_create(
   	   (DtColorModel)*colormodel, 
	   *vertexcount, 
   	   (vertices == (DtReal *)dcnull_) ? (DtReal *)0 : vertices, 
           (vertnormals == (DtReal *)dcnull_) ? (DtReal *)0 : vertnormals, 
           (vertcolors == (DtReal *)dcnull_) ? (DtReal *)0 : vertcolors));
}

/* ====================================================================== */

void dpuvts_ (object, vtxcount, vertlocs, vertnormals, vertcolors)
    DtObject *object;
    DtInt *vtxcount;
    DtReal vertlocs[];
    DtReal vertnormals[];
    DtReal vertcolors[];
{
    dor_vartristrip_update(*object, 
	*vtxcount,
   	(vertlocs == (DtReal *)dcnull_) ? (DtReal *)0 : vertlocs,  
        (vertnormals == (DtReal *)dcnull_) ? (DtReal *)0 : vertnormals,  
        (vertcolors == (DtReal *)dcnull_) ? (DtReal *)0 : vertcolors);
}

/* ====================================================================== */
 
void dputsv_ (object, uvindex, uvlist)
    DtObject *object;
    DtInt *uvindex;
    DtRealCouple uvlist[];
{
    dor_vartristrip_update_uv (*object, *uvindex, uvlist);
}

/* ====================================================================== */
 
void dputsw_ (object, uvwindex, uvwlist)
    DtObject *object;
    DtInt *uvwindex;
    DtRealTriple uvwlist[];
{
    dor_vartristrip_update_uvw (*object, *uvwindex, uvwlist);
}
