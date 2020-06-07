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
 
/* ======================================================================
  Functions:
	dor_pick_pop_lcstofcsmat
	dor_pick_push_lcstofcsmat
	dor_pick_transform_clip_z_point

  ====================================================================== */

#include <dore/internal/dogen.h>
#include <dore/internal/matrix.h>
#include <dore/dore_develop/private/device.h>
#include "pick.h"

/* ====================================================================== */

void dor_pick_pop_lcstofcsmat (void)
{
    dor_matrix_load (doe_pick_data.lcstofcsmat, doe_pick_data.lcstofcsmat_temp);
}

/* ====================================================================== */

void dor_pick_push_lcstofcsmat (
    DtMatrix4x4 newmatrix)
{
    dor_matrix_load (doe_pick_data.lcstofcsmat_temp, doe_pick_data.lcstofcsmat);
    dor_matrix_load (doe_pick_data.lcstofcsmat, newmatrix);
}

/* ====================================================================== */

DtFlag dor_pick_transform_clip_z_point (
    DtRealTriple point,
    DtMatrix4x4 tfmmatrix)
{
    DtInt row, column;
    DtReal fcspoint[4];
    DtReal oneoverw;
    DtRealTriple tfmpoint;
    DtReal ctodscale[3], ctodtranslate[3];
    DtReal ftodscale[3], ftodtranslate[3];
    DtReal scalefactor[3];

    /*  Transform the point.  */

    for (row=0;row<4;row++){
	fcspoint[row] = (doe_pick_data.lcstofcsmat)[row][3];

	for (column=0;column<3;column++){
	    fcspoint[row] += (doe_pick_data.lcstofcsmat)[row][column]
			   * point[column];
	}
    }

    if ((fcspoint[2] < -fcspoint[3]) || (fcspoint[2] > 0))
	return DcTrue;

    oneoverw = 1. / fcspoint[3];

    tfmpoint[0] = fcspoint[0] * oneoverw;
    tfmpoint[1] = fcspoint[1] * oneoverw;
    tfmpoint[2] = fcspoint[2] * oneoverw;

    dor_matrix_load_identity(tfmmatrix);

    dor_matrix_translate
	(tfmmatrix, tfmpoint[0],tfmpoint[1],tfmpoint[2], DcPreConcatenate);

    dor_device_get_scltrn_parms(doe_pick_data.device, ctodscale,ctodtranslate);

    dor_device_get_ftod_parms (doe_pick_data.device, ftodscale, ftodtranslate);

    scalefactor[0] = ftodscale[0] / ctodscale[0];
    scalefactor[1] = ftodscale[1] / ctodscale[1];
    scalefactor[2] = ftodscale[2] / ctodscale[2];

    dor_matrix_scale
    (   tfmmatrix,
	scalefactor[0],scalefactor[1],scalefactor[2],
	DcPreConcatenate
    );

    return DcFalse;
}
