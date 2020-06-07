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
#include <dore/dore_develop/develop.h>

/* ====================================================================== */

DtObject DDview_ResizeRaster (
    DtObject view,
    DtInt    width,
    DtInt    height)
{
    return dor_view_resize_raster(view, width, height);
}

/* ====================================================================== */

DtFlag DDview_TraverseDisplay (
    DtObject view,
    DtInt    method_id)
{
    return dor_view_traverse_display (view, method_id);
}

/* ====================================================================== */

DtFlag DDview_TraverseStudio (
    DtObject view,
    DtInt    method_id)
{
    return dor_view_traverse_studio (view, method_id);
}
