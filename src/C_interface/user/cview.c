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

DtObject DoView (void)
{
    return dor_view_create();
}

/* ====================================================================== */

void DvUpdate (DtObject view)
{
    dor_view_update (view);
}

/* ====================================================================== */

DtObject DvInqActiveCamera (DtObject view)
{
    return (dor_view_inq_active_camera(view));
}

/* ====================================================================== */

void DvInqBoundary (DtObject view, DtVolume *boundary)
{
    dor_view_inq_boundary (view, boundary);
}

/* ====================================================================== */

void DvInqBackgroundColor (
    DtObject      view,
    DtColorModel *colortype,
    DtReal        color[])
{
    dor_view_inq_background_color (view, colortype, color);
}

/* ====================================================================== */

void DvInqBackgroundJust (
    DtObject  view,
    DtReal   *left,
    DtReal   *bottom)
{
   dor_view_inq_background_just(view,left, bottom);
}

/* ====================================================================== */
 
void DvInqBackgroundRaster (
    DtObject  view,
    DtObject *raster,
    DtObject *callback)
{
   dor_view_inq_background_raster(view,raster, callback);
}

/* ====================================================================== */

DtFlag DvInqClearFlag (DtObject view)
{
    return(dor_view_inq_clear_flag(view));
}

/* ====================================================================== */

DtObject DvInqDefinitionGroup (DtObject view)
{
    return dor_view_inq_definition_group (view);
}

/* ====================================================================== */

DtObject DvInqDisplayGroup (DtObject view)
{
    return dor_view_inq_display_group(view);
}

/* ====================================================================== */

DtRenderStyle DvInqRendStyle (DtObject view)
{
    return dor_view_inq_render_style(view);
}

/* ====================================================================== */

DtInt DvInqShadeIndex (DtObject view)
{
   return (dor_view_inq_shade_index(view));
}

/* ====================================================================== */

DtUpdateType DvInqUpdateType (DtObject view)
{
    return(dor_view_inq_update_type(view));
}

/* ====================================================================== */

void DvInqRadiosityConvergence (
    DtObject             view,
    DtRadiosityConvType *type,
    DtReal               parms[])
{
    dor_view_inq_rad_convergence (view,type,parms);
}

/* ====================================================================== */

DtRadiosityUpdateType DvInqRadiosityUpdateType (DtObject view)
{
    return (dor_view_inq_rad_updatetype(view));
}

/* ====================================================================== */

void DvInqFormFactorCompType (
    DtObject              view,
    DtFormFactorCompType *type,
    DtReal                parms[])
{
    dor_view_inq_formfac_comptype(view,type,parms);
}

/* ====================================================================== */

void DvSetActiveCamera (DtObject view, DtObject camera)
{
    dor_view_set_active_camera (view, camera);
}

/* ====================================================================== */

void DvSetBackgroundColor (
    DtObject     view,
    DtColorModel colortype,
    DtReal       color[])
{
    dor_view_set_background_color(view,colortype,color);
}

/* ====================================================================== */
 
void DvSetBackgroundJust (
    DtObject view,
    DtReal   left,
    DtReal   bottom)
{
   dor_view_set_background_just (view, left, bottom);
}

/* ====================================================================== */
 
void DvSetBackgroundRaster (
    DtObject view,
    DtObject raster,
    DtObject callback)
{
   dor_view_set_background_raster(view,raster, callback);
}

/* ====================================================================== */

void DvSetClearFlag (DtObject view, DtFlag clearflag)
{
    dor_view_set_clear_flag(view,clearflag);
}

/* ====================================================================== */

void DvSetRendStyle (DtObject view, DtRenderStyle renderstyle)
{
    dor_view_set_render_style(view,renderstyle);
}

/* ====================================================================== */

void DvSetShadeIndex (DtObject view, DtInt shade_index)
{
   dor_view_set_shade_index (view, shade_index);
}

/* ====================================================================== */

void DvSetUpdateType (
    DtObject     view,
    DtUpdateType updatetype)
{
    dor_view_set_update_type (view, updatetype);
}

/* ====================================================================== */

void DvSetBoundary (
    DtObject  view,
    DtVolume *boundary)
{
    dor_view_set_boundary(view,boundary);
}

/* ====================================================================== */

void DvSetRadiosityConvergence (
    DtObject            view,
    DtRadiosityConvType type,
    DtReal              parms[])
{
    dor_view_set_rad_convergence (view, type, parms);
}

/* ====================================================================== */

void DvSetRadiosityUpdateType (
    DtObject              view,
    DtRadiosityUpdateType updatetype)
{
    dor_view_set_rad_updatetype (view, updatetype);
}

/* ====================================================================== */

void DvSetFormFactorCompType (
    DtObject             view,
    DtFormFactorCompType type,
    DtReal               parms[])
{
    dor_view_set_formfac_comptype (view, type, parms);
}
