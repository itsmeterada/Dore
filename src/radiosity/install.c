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
 
#include "rad.h"

/*
 ======================================================================
 Functions:

    dpr_radiosity_initialize
    dpr_radiosity_new_class

 ======================================================================
 */

/* ====================================================================== */

void dpr_radiosity_initialize (void)
{
    if (dpe_rad_radiosity_installed)
        return;

    /* none of the geometry classes that are used in radiosity can
       be installed at this point or else we can't request additional
       data for them */
    if (dpr_radiosity_geom_check_already_installed()) {
       DDerror(ERR_RAD_CANT_INSTALL,
                 "dpr_radiosity_initialize",
                 "Is DsRadiosityInitialize immediately after DsInitialize?");
    }

    /* create method to clean up linked lists of additional data after
       previous radiosity pass */
    dpe_rad_hcube_data.del_datalsts_method_id =
          dpr_radiosity_del_datalsts_method_initialize();

    /* create method to traverse original display group and build and
       initialize a group with only the radiosity objects and attributes
       for hemicube drawing */
    dpe_rad_hcube_data.hcube_setup_method_id =
          dpr_radiosity_hcube_setup_method_initialize();

    /* create method to traverse hemicube display group */
    dpe_rad_hcube_data.hcube_render_method_id =
          dpr_radiosity_hcube_render_method_initialize();

    /* create method to distribute the radiosities  */
    dpe_rad_hcube_data.hcube_distrib_method_id =
          dpr_radiosity_hcube_distrib_method_initialize();

    /* create method to select new shooting patch and comp. energy in
       system */
    dpe_rad_hcube_data.hcube_select_method_id =
          dpr_radiosity_hcube_select_method_initialize();

    /* create method to find out what the intermediate disp type is */
    dpe_rad_show_data.find_dsptyp_method_id =
          dpr_radiosity_find_dsptyp_method_initialize();

    /* create method to traverse original display group at display time*/
    dpe_rad_show_data.show_render_method_id =
          dpr_radiosity_show_render_method_initialize();

    /* have Dore call this routine for every new class installed */
    DDclass_AddNotify (dpr_radiosity_new_class);

    dpe_rad_radiosity_installed = DcTrue;
}

/* ====================================================================== */

void dpr_radiosity_new_class (
    DtPtr class_name,
    DtInt class_id)
{
    dpr_radiosity_del_datalsts_new_class(class_name, class_id);
    dpr_radiosity_hcube_setup_new_class(class_name, class_id);
    dpr_radiosity_hcube_render_new_class(class_name, class_id);
    dpr_radiosity_hcube_distrib_new_class(class_name, class_id);
    dpr_radiosity_hcube_select_new_class(class_name, class_id);
    dpr_radiosity_find_dsptyp_new_class(class_name, class_id);
    dpr_radiosity_show_render_new_class(class_name, class_id);

    /* request additional data for that class */

    dpr_radiosity_request_additional_data(class_name, class_id);
}
