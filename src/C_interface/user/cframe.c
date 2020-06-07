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
 
#include "dore/internal/dogen.h"

DtObject DoFrame (void)
{
    return dor_frame_create();
}

/* ====================================================================== */

void DfUpdate (DtObject frame)
{
    dor_frame_update (frame);
}

/* ====================================================================== */

void DfInqBoundary (DtObject frame, DtVolume *boundary)
{
    dor_frame_inq_boundary (frame, boundary);
}

/* ====================================================================== */

void DfInqJust (
    DtObject  frame,
    DtReal   *left,
    DtReal   *bottom)
{
    dor_frame_inq_justification (frame, left, bottom);
}

/* ====================================================================== */

DtObject DfInqViewGroup (DtObject frame)
{
    return dor_frame_inq_view_group (frame);
}

/* ====================================================================== */

void DfSetBoundary (DtObject frame, DtVolume *boundary)
{
    dor_frame_set_boundary (frame, boundary);
}

/* ====================================================================== */

void DfSetJust (
    DtObject frame,
    DtReal   left,
    DtReal   bottom)
{
    dor_frame_set_justification (frame, left, bottom);
}

