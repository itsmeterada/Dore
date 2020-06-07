#ifndef DD_STDTEXATT_H
#define DD_STDTEXATT_H

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

typedef struct mapdifclr_stdattributes {
    DtTextureAntiAliasMode aamode;
    DtTextureOperator      operator;
    DtObject               surface_object;
    Dt2PartProjType        projection_type;
    struct {
        DtInt        idx;
        DtExtendMode extend[2];
        DtMatrix3x3  matrix;
    } uv;
    struct {
        DtInt        idx;
        DtExtendMode extend[3];
        DtMatrix4x4  matrix;
    }uvw;
} dot_stdtexatt;

#endif