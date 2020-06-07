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

/* These are Fortran common blocks */

int _DCMXS[3];
int _DCLTPS[6];
int _DCSHS[2];
int _DCPKS[3];
int _DCISS[11];
int _DCSFLT[1];
int _DCSMAP[6];
int _DCSTUV[1];
int _DCDELC[1];
int _DCTYPE[169];

void DSINIT (processors)
    DtInt *processors;
{
    int i;

    /* 
     * Initialize all fortran object types to -1
     */

    for (i=0; i<169; i++) {
       _DCTYPE[i] = -1;
    }

    dor_system_initialize(*processors);

    /*
     * Now, make ``global'' Dore objects available to Fortran
     *
     * Order in the following arrays is CRITICAL: it maps to common
     *  blocks in Fortran.  See Fortran include file: DORE for details.
     */

    /* Matrices */

    _DCMXS[0] = (int)DcBezier4;
    _DCMXS[1] = (int)DcHermite4;
    _DCMXS[2] = (int)DcBSpline4;

    /* Light Types */

    _DCLTPS[0] = (int)DcLightAmbient;
    _DCLTPS[1] = (int)DcLightInfinite;
    _DCLTPS[2] = (int)DcLightPoint;
    _DCLTPS[3] = (int)DcLightPointAttn;
    _DCLTPS[4] = (int)DcLightSpot;
    _DCLTPS[5] = (int)DcLightSpotAttn;

    /* Shaders */

    _DCSHS[0] = (int)DcShaderConstant;
    _DCSHS[1] = (int)DcShaderLightSource;

    /* Pick Callbacks */

    _DCPKS[0] = (int)DcPickFirst;	
    _DCPKS[1] = (int)DcPickAll;
    _DCPKS[2] = (int)DcPickClosest;

    /* Input Slots */

    _DCISS[0] = (int)DcTransXSlot;
    _DCISS[1] = (int)DcTransYSlot;
    _DCISS[2] = (int)DcTransZSlot;
    _DCISS[3] = (int)DcScaleXSlot;
    _DCISS[4] = (int)DcScaleYSlot;
    _DCISS[5] = (int)DcScaleZSlot;
    _DCISS[6] = (int)DcRotXSlot;
    _DCISS[7] = (int)DcRotYSlot;
    _DCISS[8] = (int)DcRotZSlot;
    _DCISS[9] = (int)DcUndoSlot;
    _DCISS[10] = (int)DcUpdateSlot;

    /* Super Sample Filters */

    _DCSFLT[0] = (int)DcFilterBox;

    /* Texture Mapping Functions */

    _DCSMAP[0] = (int)DcStdBumpMap;
    _DCSMAP[1] = (int)DcStdTableLookup;
    _DCSMAP[2] = (int)DcStd3dTableLookup;
    _DCSMAP[3] = (int)DcStdSphereEnvironMap;
    _DCSMAP[4] = (int)DcStdCubeEnvironMap;
    _DCSMAP[5] = (int)Dc2PartMap;

    /* Texture UV Computation Callbacks */

    _DCSTUV[0] = (int)DcStdUVCallback;

    /* Raster Delete Data Callbacks */

    _DCDELC[0] = (int)DcDeleteData;
}

