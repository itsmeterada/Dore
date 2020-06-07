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
#include <dore/internal/system.h>

/* These are Fortran common blocks */

int dcmxs_[3];
int dcltps_[6];
int dcshs_[2];
int dcpks_[3];
int dciss_[11];
int dcsflt_[1];
int dcsmap_[6];
int dcstuv_[1];
int dcstuvw_[1];
int dcdelc_[1];
int dctype_[169];
int dcnull_[1];

void dsinit_ (processors)
    DtInt *processors;
{
    int i;

    /* Initialize all fortran object types to -1 */
    for (i=0; i<169; i++) {
       dctype_[i] = -1;
    }

    dor_system_initialize(*processors);

    /* Now, make ``global'' Dore objects available to Fortran
    ** Order in the following arrays is CRITICAL: it maps to common
    **  blocks in Fortran.  See Fortran include file: DORE for details.  */

    /* Matrices */

    dcmxs_[0] = (int)DcBezier4;
    dcmxs_[1] = (int)DcHermite4;
    dcmxs_[2] = (int)DcBSpline4;

    /* Light Types */

    dcltps_[0] = (int)DcLightAmbient;
    dcltps_[1] = (int)DcLightInfinite;
    dcltps_[2] = (int)DcLightPoint;
    dcltps_[3] = (int)DcLightPointAttn;
    dcltps_[4] = (int)DcLightSpot;
    dcltps_[5] = (int)DcLightSpotAttn;

    /* Shaders */

    dcshs_[0] = (int)DcShaderConstant;
    dcshs_[1] = (int)DcShaderLightSource;

    /* Pick Callbacks */

    dcpks_[0] = (int)DcPickFirst;	
    dcpks_[1] = (int)DcPickAll;
    dcpks_[2] = (int)DcPickClosest;

    /* Input Slots */

    dciss_[0] = (int)DcTransXSlot;
    dciss_[1] = (int)DcTransYSlot;
    dciss_[2] = (int)DcTransZSlot;
    dciss_[3] = (int)DcScaleXSlot;
    dciss_[4] = (int)DcScaleYSlot;
    dciss_[5] = (int)DcScaleZSlot;
    dciss_[6] = (int)DcRotXSlot;
    dciss_[7] = (int)DcRotYSlot;
    dciss_[8] = (int)DcRotZSlot;
    dciss_[9] = (int)DcUndoSlot;
    dciss_[10] = (int)DcUpdateSlot;

    /* Super Sample Filters */
    dcsflt_[0] = (int)DcFilterBox;

    /* Texture Mapping Functions */

    dcsmap_[0] = (int)DcStdBumpMap;
    dcsmap_[1] = (int)DcStdTableLookup;
    dcsmap_[2] = (int)DcStd3dTableLookup;
    dcsmap_[3] = (int)DcStdSphereEnvironMap;
    dcsmap_[4] = (int)DcStdCubeEnvironMap;
    dcsmap_[5] = (int)Dc2PartMap;

    /* Texture UV/UVW Computation Callbacks */

    dcstuv_[0]  = (int)DcStdUVCallback;
    dcstuvw_[0] = (int)DcStdUVWCallback;

    /* Raster Delete Data Callbacks */

    dcdelc_[0] = (int)DcDeleteData;
}
