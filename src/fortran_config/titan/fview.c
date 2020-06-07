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

DtObject DOVW ()
{
	return(dor_view_create());
}

/* ====================================================================== */

void DVU (view)
    DtObject *view;
{
    dor_view_update(*view);
}

/* ====================================================================== */

DtObject DVQAC (view)
    DtObject *view;
{
    return(dor_view_inq_active_camera(*view));
}

/* ====================================================================== */

void DVQB (view, boundary)
    DtObject *view;
    DtVolume *boundary;
{
    dor_view_inq_boundary(*view, boundary);
}

/* ====================================================================== */

void DVQBC (view, colortype, color)
    DtObject *view;
    int *colortype;
    DtReal color[];
{
    dor_view_inq_background_color (*view, (DtColorModel*) colortype, color);
}

/* ====================================================================== */
 
void DVQBJ (view, left, bottom)
    DtObject *view;
    DtReal *left;
    DtReal *bottom;
{
    dor_view_inq_background_just(*view,left, bottom);
}

/* ====================================================================== */
 
void DVQBR (view, raster, callback)
    DtObject *view;
    DtObject *raster;
    DtObject *callback;
{
    dor_view_inq_background_raster(*view,raster, callback);
}

/* ====================================================================== */

DtFlag DVQCF (view)
    DtObject *view;
{
    return(dor_view_inq_clear_flag(*view));
}

/* ====================================================================== */

DtObject DVQDG (view)
    DtObject *view;
{
    return dor_view_inq_definition_group(*view);
}

/* ====================================================================== */

DtObject DVQIG (view)
    DtObject *view;
{
    return dor_view_inq_display_group(*view);
}

/* ====================================================================== */

DtRenderStyle DVQRS (view)
    DtObject *view;
{
    return dor_view_inq_render_style(*view);
}

/* ====================================================================== */

DtInt DVQSI (view)
    DtObject *view;
{
    return (dor_view_inq_shade_index(*view));
}

/* ====================================================================== */

DtUpdateType DVQUT (view)
    DtObject *view;
{
    return(dor_view_inq_update_type(*view));
}

/* ====================================================================== */

void DVQFFC (view, type, parms)
    DtObject *view;
    int *type;
    DtReal *parms;
{
    dor_view_inq_formfac_comptype (*view, (DtFormFactorCompType*) type, parms);
}

/* ====================================================================== */

void DVQRDC (view, type, parms)
    DtObject *view;
    int *type;
    DtReal *parms;
{
    dor_view_inq_rad_convergence (*view, (DtRadiosityConvType*) type, parms);
}

/* ====================================================================== */

DtRadiosityUpdateType DVQRDU (view)
    DtObject *view;
{
    return(dor_view_inq_rad_updatetype(*view));
}

/* ====================================================================== */

void DVSAC (view, camera)
    DtObject *view;
    DtObject *camera;
{
    dor_view_set_active_camera(*view, *camera);
}

/* ====================================================================== */

void DVSBC (view, colortype, color)
    DtObject *view;
    int *colortype;
    DtReal color[];
{
    dor_view_set_background_color (*view, (DtColorModel)*colortype, color);
}
 
/* ====================================================================== */

void DVSBJ (view, left, bottom)
    DtObject *view;
    DtReal *left;
    DtReal *bottom;
{
    dor_view_set_background_just(*view,*left, *bottom);
}
 
/* ====================================================================== */

void DVSBR (view, raster, callback)
    DtObject *view;
    DtObject *raster;
    DtObject *callback;
{
    dor_view_set_background_raster(*view,*raster, *callback);
}

/* ====================================================================== */

void DVSCF (view, clearflag)
    DtObject *view;
    int *clearflag;
{
    dor_view_set_clear_flag(*view, (DtFlag)*clearflag);
}

/* ====================================================================== */

void DVSRS (view, renderstyle)
    DtObject *view;
    int *renderstyle;
{
    dor_view_set_render_style(*view, (DtRenderStyle)*renderstyle);
}

/* ====================================================================== */

void DVSSI (view, shade_index)
    DtObject *view;
    DtInt *shade_index;
{
    dor_view_set_shade_index (*view, *shade_index);
}

/* ====================================================================== */

void DVSUT (view, updatetype)
    DtObject *view;
    int *updatetype;
{
    dor_view_set_update_type(*view,(DtUpdateType)*updatetype);
}

/* ====================================================================== */

void DVSB (view, boundary)
    DtObject *view;
    DtVolume *boundary;
{
    dor_view_set_boundary(*view, boundary);
}

/* ====================================================================== */

void DVSFFC (view, type, parms)
    DtObject *view;
    int *type;
    DtReal *parms;
{
    dor_view_set_formfac_comptype (*view, (DtFormFactorCompType)*type, parms);
}

/* ====================================================================== */

void DVSRDC (view, type, parms)
    DtObject *view;
    int *type;
    DtReal *parms;
{
    dor_view_set_rad_convergence (*view, (DtRadiosityConvType)*type, parms);
}

/* ====================================================================== */

void DVSRDU (view, type)
    DtObject *view;
    int *type;
{
    dor_view_set_rad_updatetype (*view, (DtRadiosityConvType)*type);
}
