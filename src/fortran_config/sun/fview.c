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

DtObject dovw_ ()
{
    return(dor_view_create());
}

/* ====================================================================== */

void dvu_ (view)
    DtObject *view;
{
    dor_view_update(*view);
}

/* ====================================================================== */

DtObject dvqac_ (view)
    DtObject *view;
{
    return(dor_view_inq_active_camera(*view));
}

/* ====================================================================== */

void dvqb_ (view, boundary)
    DtObject *view;
    DtVolume *boundary;
{
    dor_view_inq_boundary(*view, boundary);
}

/* ====================================================================== */

void dvqbc_ (view, colortype, color)
    DtObject *view;
    DtColorModel *colortype;
    DtReal color[];
{
    dor_view_inq_background_color (*view, colortype, color);
}

/* ====================================================================== */
 
void dvqbj_ (view, left, bottom)
    DtObject *view;
    DtReal *left;
    DtReal *bottom;
{
    dor_view_inq_background_just (*view, left, bottom);
}

/* ====================================================================== */
 
void dvqbr_ (view, raster, callback)
    DtObject *view;
    DtObject *raster;
    DtObject *callback;
{
    dor_view_inq_background_raster (*view, raster, callback);
}

/* ====================================================================== */

DtFlag dvqcf_ (view)
    DtObject *view;
{
    return dor_view_inq_clear_flag (*view);
}

/* ====================================================================== */

DtObject dvqdg_ (view)
    DtObject *view;
{
    return dor_view_inq_definition_group (*view);
}

/* ====================================================================== */

DtObject dvqig_ (view)
    DtObject *view;
{
    return dor_view_inq_display_group (*view);
}

/* ====================================================================== */

DtRenderStyle dvqrs_ (view)
    DtObject *view;
{
    return dor_view_inq_render_style (*view);
}

/* ====================================================================== */
 
DtInt dvqsi_ (view)
    DtObject *view;
{
    return dor_view_inq_shade_index (*view);
}

/* ====================================================================== */

DtUpdateType dvqut_ (view)
    DtObject *view;
{
    return dor_view_inq_update_type (*view);
}

/* ====================================================================== */

void dvqffc_ (view, type, parms)
    DtObject *view;
    DtFormFactorCompType *type;
    DtReal *parms;
{
    dor_view_inq_formfac_comptype (*view, type, parms);
}

/* ====================================================================== */

void dvqrdc_ (view, type, parms)
    DtObject *view;
    DtRadiosityConvType *type;
    DtReal *parms;
{
    dor_view_inq_rad_convergence (*view, type, parms);
}

/* ====================================================================== */

DtRadiosityUpdateType dvqrdu_ (view)
    DtObject *view;
{
    return(dor_view_inq_rad_updatetype(*view));
}

/* ====================================================================== */

void dvsac_ (view, camera)
    DtObject *view;
    DtObject *camera;
{
    dor_view_set_active_camera(*view, *camera);
}

/* ===================================================================== */

void dvsbc_ (view, colortype, color)
    DtObject *view;
    int *colortype;
    DtReal color[];
{
    dor_view_set_background_color(*view, (DtColorModel)*colortype, color);
}
 
/* ===================================================================== */

void dvsbj_ (view, left, bottom)
    DtObject *view;
    DtReal *left;
    DtReal *bottom;
{
    dor_view_set_background_just (*view,*left, *bottom);
}
 
/* ===================================================================== */

void dvsbr_ (view, raster, callback)
    DtObject *view;
    DtObject *raster;
    DtObject *callback;
{
    dor_view_set_background_raster (*view,*raster, *callback);
}

/* ===================================================================== */

void dvscf_ (view, clearflag)
    DtObject *view;
    int *clearflag;
{
    dor_view_set_clear_flag (*view, (DtFlag)*clearflag);
}

/* ===================================================================== */

void dvsrs_ (view, renderstyle)
    DtObject *view;
    int *renderstyle;
{
    dor_view_set_render_style (*view, (DtRenderStyle)*renderstyle);
}

/* ===================================================================== */

void dvssi_ (view, shade_index)
    DtObject *view;
    DtInt *shade_index;
{
   dor_view_set_shade_index (*view, *shade_index);
}

/* ===================================================================== */

void dvsut_ (view, updatetype)
    DtObject *view;
    int *updatetype;
{
    dor_view_set_update_type (*view,(DtUpdateType)*updatetype);
}

/* ===================================================================== */

void dvsb_ (view, boundary)
    DtObject *view;
    DtVolume *boundary;
{
    dor_view_set_boundary(*view, boundary);
}

/* ===================================================================== */

void dvsffc_ (view, type, parms)
    DtObject *view;
    int *type;
    DtReal *parms;
{
    dor_view_set_formfac_comptype (*view, (DtFormFactorCompType)*type, parms);
}

/* ====================================================================== */

void dvsrdc_ (view, type, parms)
    DtObject *view;
    int *type;
    DtReal *parms;
{
    dor_view_set_rad_convergence (*view, (DtRadiosityConvType)*type, parms);
}

/* ====================================================================== */

void dvsrdu_ (view, type)
    DtObject *view;
    int *type;
{
    dor_view_set_rad_updatetype (*view, (DtRadiosityConvType)*type);
}
