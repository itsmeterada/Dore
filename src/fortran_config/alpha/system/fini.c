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
#include <dore/internal/system.h>


void dsinit_ (processors)
    DtInt *processors;
{
    int i;

    /* Initialize all fortran object types to -1 */
    for (i=0; i<169; i++) {
       dctype_[i] = -1;
    }

    dor_system_initialize (*processors, 0);

    /* Now, make ``global'' Dore objects available to Fortran
    ** Order in the following arrays is CRITICAL: it maps to common
    **  blocks in Fortran.  See Fortran include file: DORE for details.  */

    /* Matrices */

    dcmxs_[0] = (long) DcBezier4;
    dcmxs_[1] = (long) DcHermite4;
    dcmxs_[2] = (long) DcBSpline4;

    /* Light Types */

    dcltps_[0] = (long) DcLightAmbient;
    dcltps_[1] = (long) DcLightInfinite;
    dcltps_[2] = (long) DcLightPoint;
    dcltps_[3] = (long) DcLightPointAttn;
    dcltps_[4] = (long) DcLightSpot;
    dcltps_[5] = (long) DcLightSpotAttn;

    /* Shaders */

    dcshs_[0] = (long) DcShaderConstant;
    dcshs_[1] = (long) DcShaderLightSource;

    /* Pick Callbacks */

    dcpks_[0] = (long) DcPickFirst;	
    dcpks_[1] = (long) DcPickAll;
    dcpks_[2] = (long) DcPickClosest;

    /* Input Slots */

    dciss_[ 0] = (long) DcTransXSlot;
    dciss_[ 1] = (long) DcTransYSlot;
    dciss_[ 2] = (long) DcTransZSlot;
    dciss_[ 3] = (long) DcScaleXSlot;
    dciss_[ 4] = (long) DcScaleYSlot;
    dciss_[ 5] = (long) DcScaleZSlot;
    dciss_[ 6] = (long) DcRotXSlot;
    dciss_[ 7] = (long) DcRotYSlot;
    dciss_[ 8] = (long) DcRotZSlot;
    dciss_[ 9] = (long) DcUndoSlot;
    dciss_[10] = (long) DcUpdateSlot;

    /* Super Sample Filters */
    dcsflt_[0] = (long) DcFilterBox;

    /* Texture Mapping Functions */

    dcsmap_[0] = (long) DcStdBumpMap;
    dcsmap_[1] = (long) DcStdTableLookup;
    dcsmap_[2] = (long) DcStd3dTableLookup;
    dcsmap_[3] = (long) DcStdSphereEnvironMap;
    dcsmap_[4] = (long) DcStdCubeEnvironMap;
    dcsmap_[5] = (long) Dc2PartMap;

    /* Texture UV/UVW Computation Callbacks */

    dcsuvc_[0]  = (long) DcStdUVCallback;
    dcswc_[0] = (long) DcStdUVWCallback;

    /* Raster Delete Data Callbacks */

    dcdelc_[0] = (long) DcDeleteData;
}
