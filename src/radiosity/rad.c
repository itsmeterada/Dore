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
 
#include <string.h>
#include <dore/dore_develop/private/device.h>
#include "rad.h"

/*
 =====================================================================
 Functions:

   dpr_radiosity_clean_up
   dpr_radiosity_compute_bounding_volume
   dpr_radiosity_compute_iteration
   dpr_radiosity_create_hcube_display_grp
   dpr_radiosity_create_hcube_studio_grp
   dpr_radiosity_display
   dpr_radiosity_gather_scene_energy_info
   dpr_radiosity_need_to_display
   dpr_radiosity_open_hcube_device
   dpr_radiosity_render
   dpr_radiosity_support

 ======================================================================
 */

/* this should be isolated to ease addition of other hcube renderers */
extern DtInt dre_dynrnd_display_method_id;

/* color of background for hcube rendering. No object should have this color */
DtReal dpe_rad_backg_col[]   = {0.0, 0.0, 0.0};
DtInt  dpe_rad_int_backg_col = 0x00000000;

/* data related to the rendering performed to compute the form factors
   using the hemi-cube method */ 
dpt_rad_hcube_data dpe_rad_hcube_data;
                     
/* data related to the rendering performed to display the final
   image with vertex radiosities */
dpt_rad_show_data dpe_rad_show_data;

DtFlag dpe_rad_radiosity_installed = DcFalse;

DtInt   dpe_rad_base_method_id;
DtInt   dpe_rad_iteration_number;
DtInt   dpe_rad_fix_elem;

DtFlag dpe_radiosity_ok_to_use_tristrip_alternate_rep = DcTrue;
DtRadiosityUpdateType dpe_rnd_updtype;

dpt_rad_brightest_patch    dpe_rad_brightest_patch;

dpt_rad_ambient    dpe_rad_ambient;
dpt_rad_energy     dpe_rad_energy;

DtFlag dpe_rad_converged;
static dpe_rad_very_first_time = DcTrue;
DtInt dpe_rad_pixel_offset;

/* Function Prototypes */

DtFlag dpr_radiosity_support (dot_object *device);

/*
 ======================================================================
 */

/*
 * Main radiosity routine. It should be called directly from the
 * renderer's rendering routine. dpr_radiosity_render performs
 * iterations of the progressive radiosity algorithm until the
 * convergence criteria is met. It uses the Dore vll driver to
 * draw the hemi-cube projections.
 *
 * Returns DcTrue if radiosity is supported for the type of the
 * device passed in, otherwise returns DcFalse.
 */

DtFlag 
dpr_radiosity_render (
    dot_object *device,
    dot_object *view,
    DtInt dispgrp_method_id,    /* current display method of the renderer */
    DtPFI reset_dispgrp_method_id_fcn /* function to change the display method */
)
{
   /* check if radiosity is supported for current device type;
       if it isn't, silently ignore all radiosity-related functions */
   if (!dpr_radiosity_support(device)) 
      return(DcFalse);

    /* initialize data for renderer used for display */
    dpe_rad_show_data.view = view;
    dpe_rad_show_data.device = device;
    dpe_rad_show_data.renderer_method_id = dispgrp_method_id;
    dpe_rad_show_data.renderer_reset_method_id_fcn =
                                reset_dispgrp_method_id_fcn;

    /* initialize data for renderer used for hcube drawing */
    /* for now we support only the dynamic renderer for hcube */

    dpe_rad_hcube_data.renderer_method_id = dre_dynrnd_display_method_id;
    dpe_rad_hcube_data.renderer_reset_method_id_fcn = 
             (DtPFI) dre_dynrnd_reset_current_display_method;

    dpe_rnd_updtype = DvInqRadiosityUpdateType(view);

    /* temporarily change to avoid endless rendering loop */
    DvSetRadiosityUpdateType(view, DcRadiosityNone);

    /* find what value of the DoIntermediateDispType attribute we have */
    dpr_radiosity_find_intermediate_disp_type();
   
    DvInqRadiosityConvergence(dpe_rad_show_data.view,
                              &dpe_rad_hcube_data.conv_type,
                              dpe_rad_hcube_data.conv_parms);
   
    /* tell driver not to use tristrip alt.rep. for trimeshes.
       if we'd use tristrips we'd loose some connectivity info for
       interpolating the radiosities */
    dpe_radiosity_ok_to_use_tristrip_alternate_rep = DcFalse;

    if (dpe_rnd_updtype == DcRadiosityComputeDisp) {

        /* clean up linked lists of additional data from previous
           radiosity pass, in any */
        if (!dpe_rad_very_first_time) 
           dpr_radiosity_del_datalsts_execute();
         
        dpe_rad_very_first_time = DcFalse;

        /* create subset of display group with radiosity objects in it*/
        dpr_radiosity_create_hcube_display_grp();

        dpe_rad_iteration_number = 0;

        /* see if convergence criteria is already met in system */
        dpe_rad_converged = dpr_radiosity_gather_scene_energy_info();

        dpe_rad_iteration_number = 1;

        /* display initial scene (without shooting any light)*/

        if (dpr_radiosity_need_to_display()) {
            dpr_radiosity_display();
        }

        if (!dpe_rad_converged) {
            if (!dpr_radiosity_open_hcube_device()) {
                 DDerror(ERR_INVALID_OPERATION, "dpr_radiosity_render",
                    "Can't create device, frame or view for radiosity");
                     dpr_radiosity_clean_up();
                 return(DcFalse);
            }
 
            while (!dpe_rad_converged) {
                dpr_radiosity_compute_iteration();

                /* display if necessary; always display last frame */
                if (dpe_rad_converged || dpr_radiosity_need_to_display()) 
                     dpr_radiosity_display();

                dpe_rad_iteration_number++;
            }

            dpr_radiosity_clean_up();
        }

    } else {

        /* update type is DcRadiosityDisp */
        dpr_radiosity_display();
    }

    /* restore initial radiosity update type */
    DvSetRadiosityUpdateType(view, dpe_rnd_updtype);

    /* restore display method of renderer */
    (*dpe_rad_show_data.renderer_reset_method_id_fcn)
                       (dpe_rad_show_data.renderer_method_id);

    /* restore display method of hemi-cube medium */
    (*dpe_rad_hcube_data.renderer_reset_method_id_fcn)
                       (dpe_rad_hcube_data.renderer_method_id);

   return(DcTrue);
}

/*
 ======================================================================
 */

/* return DcTrue if radiosity is supported for selected rendering style
   and specififed device type; otherwise return DcFalse */

DtFlag dpr_radiosity_support (
    dot_object *device)
{
    char **driv_names, **descriptions;
    DtInt i, size;
    char device_descr[200];
    static int num_drivers = -1;
    static int g2_display;
    static char display_descrp[200], stdx11_descrp[200];

    if (!dpe_rad_radiosity_installed) {
        DDerror(ERR_RAD_NOT_INSTALLED, "dpr_radiosity_support",
               "Use DsRadiosityInitialize immediately after Dsinitialize");
        return(DcFalse);
    }

   /* check if current device is the titan driver or the stdx11 driver;
       since the same device can be installed with different names,
       we key off of the device description strings instead */

   if (num_drivers == -1) {
      g2_display = DcFalse;
      num_drivers = DsInqNumDrivers();
      size = num_drivers*sizeof(DtPtr);
       driv_names = DDspace_Allocate(size);
       descriptions = DDspace_Allocate(size);
       DsInqDriverInfo (num_drivers, driv_names, descriptions);
      strcpy(stdx11_descrp, "junk");
      strcpy(display_descrp, "junk");
      for (i=0; i<num_drivers; i++) {
         if (0==(strcmp(driv_names[i], "display"))) 
            strcpy(display_descrp, descriptions[i]);
         else if (0==(strcmp(driv_names[i], "stdx11")))
            strcpy(stdx11_descrp, descriptions[i]);
            else if (0==(strcmp(driv_names[i], "g2")))
            /* Dore 1500 installs a device called "display"
                   but it does not support radiosity */
            g2_display = DcTrue;
      }
   }

   /* get description string of current device */
   strcpy(device_descr, (((dot_device *)(device->data))->driver)->description);

   if (g2_display || ((0 != strcmp(device_descr, display_descrp)) &&
      0 != strcmp(device_descr, stdx11_descrp))) {
      /* radiosity not supported with this device */
      return(DcFalse);
   }

    /* radiosity is supported */
   return(DcTrue);
}

/*
 ======================================================================
 */

DtFlag dpr_radiosity_open_hcube_device (void)
{
    char geomstring[128];
    DtFormFactorCompType fftype;
    DtReal res[1];
    DtInt resol;

    DvInqFormFactorCompType(dpe_rad_show_data.view, &fftype, res);

    if (fftype != DcFormFactorHemicube) {
       DDerror(ERR_RAD_BAD_FF_CMPTYP, "dpr_radiosity_open_hcube_device",
               "Using default values");
       res[0] = 128;
    }

    /* make sure hemicube resolution is an even integer number */
    resol = ((int) ((int) res[0] / 2.0 + 0.5)) * 2;

    dpe_rad_hcube_data.resolution = resol;

    sprintf (geomstring, "-offscreen -geometry %dx%d+0+0",
             resol, resol);

    dpe_rad_hcube_data.device = DoDevice("display", geomstring);
    dpe_rad_hcube_data.view = DoView();
    dpe_rad_hcube_data.frame = DoFrame();

    if (dpe_rad_hcube_data.device == DcNullObject ||
        dpe_rad_hcube_data.view   == DcNullObject ||
        dpe_rad_hcube_data.frame  == DcNullObject)
        return(DcFalse);

    DdSetFrame(dpe_rad_hcube_data.device, dpe_rad_hcube_data.frame);

    DgAddObjToGroup(DfInqViewGroup(dpe_rad_hcube_data.frame),
                    dpe_rad_hcube_data.view);

    DgAddObjToGroup(DvInqDisplayGroup(dpe_rad_hcube_data.view),
                    dpe_rad_hcube_data.display_group);

    dpe_rad_hcube_data.studio_group = DcNullPtr;

    return(DcTrue);
}

/*
 ======================================================================
 */

void dpr_radiosity_compute_iteration (void)
{
    DtInt face;
    DtInt width, height;
    DtRasterType returntype;
    DtFlag userdelete, can_return_data;
    DtPtr imgbuffer;


    /* create a new studio group according to the brightest patch */
    dpr_radiosity_create_hcube_studio_grp();

    dpr_radiosity_hcube_distrib_init_execution();

    /* set display method for the hcube medium  */
    (*dpe_rad_hcube_data.renderer_reset_method_id_fcn)
                 (dpe_rad_hcube_data.hcube_render_method_id);

    /* set shadow method to original display method of hcube medium */
    dpe_rad_base_method_id = dpe_rad_hcube_data.renderer_method_id;

    for (face=0; face<5; face++) {

        DvSetActiveCamera(dpe_rad_hcube_data.view,
                          dpe_rad_hcube_data.cameras[face]);

        DvSetBackgroundColor(dpe_rad_hcube_data.view, DcRGB, dpe_rad_backg_col);

        /* draw from point of view of brightest patch */
        dpr_radiosity_device_update_view(
             dpe_rad_hcube_data.device, dpe_rad_hcube_data.view);

        /* read data back from device*/
        /* more efficient to ask for RGBA and drop A later */
        can_return_data =
               DdInqPixelData(dpe_rad_hcube_data.device, DcRasterRGBA,
                     &width, &height, &returntype, &imgbuffer, &userdelete);

        if (!can_return_data) {
           DDerror(ERR_RAD_CANT_GET_PIX,
                  "dpr_radiosity_compute_iteration", DcNullPtr);
        }

        /* we store the number of bytes returned per pixel*/
        if (returntype == DcRasterRGBA) {
            dpe_rad_pixel_offset = 4;
        } else if (returntype == DcRasterRGB) {
            dpe_rad_pixel_offset = 3;
        } else {
            DDerror(ERR_RAD_CANT_GET_PIX,
              "dpr_radiosity_open_hcube_device",
              "Sorry, DdInqPixelData must support DcRasterRGBA or DcRasterRGB");
        }

        /* increment the form factors using this buffer */
        dpr_radiosity_hcube_distrib_update_form_factors(face,
                                             (unsigned char *)imgbuffer);

        /* check if it is our responsability or not to free the space */
        if (userdelete) 
            DDspace_Deallocate(imgbuffer);

    }

    /* distribute radiosity according to cumulated form factors */ 
    dpr_radiosity_hcube_distrib_execute();

    /* reset radiosity of shooting patch to zero */
    /* we are acutality changing the additional data of the shooting object */

    dpe_rad_brightest_patch.unshotRadPtr[0] = 0.0;
    dpe_rad_brightest_patch.unshotRadPtr[1] = 0.0;
    dpe_rad_brightest_patch.unshotRadPtr[2] = 0.0;


    /* select next shooting patch, and gather info about energy in current
       system; find out if convergence criterion was met */

    dpe_rad_converged = dpr_radiosity_gather_scene_energy_info();

}

/*
 ======================================================================
 */

DtFlag dpr_radiosity_gather_scene_energy_info (void)
{
    DtFlag convergence;

    dpr_radiosity_hcube_select_execute();

    /* set total energy in the system if this is the first pass */

    if (dpe_rad_iteration_number == 0)
        dpe_rad_energy.total = dpe_rad_energy.total_unshot;

    /* compute global ambient component for scene */

    dpr_radiosity_hcube_select_compute_ambient();

    convergence = dpr_radiosity_hcube_select_test_convergence();

    return(convergence);
}

/*
 ======================================================================
 */

void dpr_radiosity_clean_up (void)
{
    /* delete hemicube device. At the same time, the hcube frame, studio
       and display groups are deleted (since they are only referenced
       by the device ) */

    if (dpe_rad_hcube_data.device != DcNullObject)
        DeDeleteObject(dpe_rad_hcube_data.device);

    /* deleting a device has this silly side effect of making it the
       current device before deletion occurs. So reset current device. */

    dor_device_set_current_view(dpe_rad_show_data.device,
                                dpe_rad_show_data.view);
}

/*
 ======================================================================
 */


DtFlag dpr_radiosity_need_to_display (void)
{
    DtInt i;
    /* this function returns true if the we wish to have the results
       of this iteration displayed, according to the value of the
       DoIntermediateDispType attribute. Always display last frame. */

    /* always display the final frame */
    if (dpe_rad_converged)
        return(DcTrue);

    switch(dpe_rad_show_data.interm_dsptyp) {
      case DcDispIntermediateAll:
           return(DcTrue);
           break;

      case DcDispFinal:
           /* this has already been tested for on first line */
           break;

      case DcDispRegularIntervals:
           if ((dpe_rad_iteration_number %
                dpe_rad_show_data.dsptyp_parms[0]) == 0)
                  return(DcTrue);
           break;

      case DcDispIrregularIntervals:
           for (i=0; i<dpe_rad_show_data.dsptyp_parms[0]; i++) 
               if (dpe_rad_iteration_number ==
                   dpe_rad_show_data.dsptyp_parms[i]) 
                     return(DcTrue);
           break;
    }

    return(DcFalse);
}

/*
 ======================================================================
 */

void dpr_radiosity_display (void)
{
    
    /* set display method for display medium to be the one that knows 
       about vertex colors */

    (*dpe_rad_show_data.renderer_reset_method_id_fcn)
                       (dpe_rad_show_data.show_render_method_id);

    /* set base method to original display method of rendering medium */
    dpe_rad_base_method_id = dpe_rad_show_data.renderer_method_id;

    /* traverse data base, using show_render method */
    dpr_radiosity_device_update_view
          (dpe_rad_show_data.device, dpe_rad_show_data.view);
}

/*
 ======================================================================
 */
 
void dpr_radiosity_create_hcube_display_grp (void)
{
    /* The following will execute the hcube_setup method and create a
       new display group containing a subset of the initial display
       group ith only the primitives for which the radiosity switch
       is on, plus geometric attributes. The additional data of each
       of these primitives is initialized, and data used for later 
       computing the ambient component is collected */

    dpr_radiosity_hcube_setup_execute();

}

/*
 ======================================================================
 */

void dpr_radiosity_create_hcube_studio_grp (void)
{
    DtRealTriple at[5], up[5];
    DtRealTriple tangU, tangV, randvec;
    DtRealTriple normal, from;
    DtRealTriple *transf_vert;
    DtReal hither, yon;
    DtReal length, diag_length;
    DtReal xdim, ydim, zdim;
    DtVolume bounding_vol;
    DtObject new_studio;
    DtInt try, size;

    /* create a studio group that will be used for hemicube drawing */

    /* use the size of the bounding volume for the set of radiosity
       objects to come up with decent values of hither and yon */
    dpr_radiosity_compute_bounding_volume(&bounding_vol,
                                  dpe_rad_hcube_data.display_group);
   
    xdim = bounding_vol.fur[0] - bounding_vol.bll[0];
    ydim = bounding_vol.fur[1] - bounding_vol.bll[1];
    zdim = bounding_vol.fur[2] - bounding_vol.bll[2];
    diag_length = sqrt(xdim*xdim + ydim*ydim + zdim*zdim);

    /* use the size of the diagonal of the cube (and 5% more) to
       set the yon plane value */
    yon = 0.0 - 1.05*diag_length;

    hither = HITHER_OVER_YON * yon; 

    size = sizeof(DtRealTriple)*dpe_rad_brightest_patch.nVert;
    transf_vert = (DtRealTriple *) DDspace_Allocate(size);
    
    /* transform vertices of brightest patch using its local to world matrix */
    dpr_radiosity_transform_vertices(dpe_rad_brightest_patch.vertices,
                                     dpe_rad_brightest_patch.nVert,
                                     dpe_rad_brightest_patch.lcswcs,
                                     transf_vert);

    /* calculate center and unit normal of brightest patch */
    dpr_radiosity_compute_center_and_normal(transf_vert,
                                     dpe_rad_brightest_patch.nVert,
                                     from, normal);

    DDspace_Deallocate(transf_vert);

    /* rotate cube by random amount along normal axis. This reduces aliasing. */

    try = 0;
    do {
        randvec[0] = dpr_radiosity_random();
        randvec[1] = dpr_radiosity_random();
        randvec[2] = dpr_radiosity_random();

        CROSS_VEC(normal, randvec, tangU);
        NORMALIZE_VEC(tangU, length);
        try++;

    } while (length == 0 && try<MAX_LOOP);

    if (try >= MAX_LOOP) {

        /* couldn't get a random vector. So try to pick an arbitrary 
           vector perpendicular to the normal */

        if (normal[2] != 0.) {
            tangU[0] = 1.0;
            tangU[1] = 1.0;
            tangU[2] = (-normal[0] - normal[1]) / normal[2];
            NORMALIZE_VEC(tangU, length);

        } else 
        if (normal[1] != 0.) {
            tangU[0] = 1.0;
            tangU[2] = 1.0;
            tangU[1] = (-normal[0] - normal[2]) / normal[1];
            NORMALIZE_VEC(tangU, length);

        } else 
        if (normal[0] != 0.) {
            tangU[1] = 1.0;
            tangU[2] = 1.0;
            tangU[0] = (-normal[1] - normal[2]) / normal[0];
            NORMALIZE_VEC(tangU, length);
        } else {
            DDerror(ERR_ZERO_LEN_VEC,
                    "dpr_radiosity_create_hcube_studio_grp",
                    "Normal to emissive surface is null");
        }
    }

    CROSS_VEC (normal, tangU, tangV);

    ADD_VEC(from, normal, at[0]);

    up[0][0] = tangU[0];
    up[0][1] = tangU[1];
    up[0][2] = tangU[2];

    ADD_VEC(from, tangU, at[1]);
    up[1][0] = normal[0];
    up[1][1] = normal[1];
    up[1][2] = normal[2];

    ADD_VEC(from, tangV, at[2]);
    up[2][0] = normal[0];
    up[2][1] = normal[1];
    up[2][2] = normal[2];

    SUB_VEC(from, tangU, at[3]);
    up[3][0] = normal[0];
    up[3][1] = normal[1];
    up[3][2] = normal[2];

    SUB_VEC(from, tangV, at[4]);
    up[4][0] = normal[0];
    up[4][1] = normal[1];
    up[4][2] = normal[2];

    new_studio = DoGroup(DcTrue);

        DgAddObj(DoPerspective(90.0, hither, yon));

        DgAddObj(DoPushMatrix());
           DgAddObj(DoLookAtFrom(at[0], from, up[0]));
           DgAddObj(dpe_rad_hcube_data.cameras[0] = DoCamera());
        DgAddObj(DoPopMatrix());

        DgAddObj(DoPushMatrix());
           DgAddObj(DoLookAtFrom(at[1], from, up[1]));
           DgAddObj(dpe_rad_hcube_data.cameras[1] = DoCamera());
        DgAddObj(DoPopMatrix());

        DgAddObj(DoPushMatrix());
           DgAddObj(DoLookAtFrom(at[2], from, up[2]));
           DgAddObj(dpe_rad_hcube_data.cameras[2] = DoCamera());
        DgAddObj(DoPopMatrix());

        DgAddObj(DoPushMatrix());
           DgAddObj(DoLookAtFrom(at[3], from, up[3]));
           DgAddObj(dpe_rad_hcube_data.cameras[3] = DoCamera());
        DgAddObj(DoPopMatrix());

        DgAddObj(DoPushMatrix());
           DgAddObj(DoLookAtFrom(at[4], from, up[4]));
           DgAddObj(dpe_rad_hcube_data.cameras[4] = DoCamera());
        DgAddObj(DoPopMatrix());

    DgClose();

    /* associate new studio and display groups to new view */
    if (dpe_rad_hcube_data.studio_group == DcNullObject) {
        dpe_rad_hcube_data.studio_group = new_studio;

        DgAddObjToGroup(DvInqDefinitionGroup(dpe_rad_hcube_data.view),
                    dpe_rad_hcube_data.studio_group);
    } else {
        /* replace studio group by new one */

        DgOpen(DvInqDefinitionGroup(dpe_rad_hcube_data.view), DcFalse);
        DgSetElePtr(0, DcBeginning);
        DgReplaceObj(new_studio);
        DgClose();

        dpe_rad_hcube_data.studio_group = new_studio;
    }

}

/*
 ======================================================================
 */

/* this function is similar to dor_system_compute_bounding_volume
   except that it does not issue an error when an active traversal is
   in progress (we know it is, but we still want to use this fcn).
   Our function returns void instead of DtVolume  */

extern DtFlag doe_system_bndvol_first_report;
extern DtReal
        doe_system_bndvol_min_x,
        doe_system_bndvol_max_x,
        doe_system_bndvol_min_y,
        doe_system_bndvol_max_y,
        doe_system_bndvol_min_z,
        doe_system_bndvol_max_z;

void dpr_radiosity_compute_bounding_volume (
    DtVolume *volume,
    DtObject object)
{
    doe_system_bndvol_first_report = DcTrue;

    doe_system_bndvol_min_x =
    doe_system_bndvol_max_x = 0.0;
    doe_system_bndvol_min_y =
    doe_system_bndvol_max_y = 0.0;
    doe_system_bndvol_min_z =
    doe_system_bndvol_max_z = 0.0;

    doe_system_current_method = DcMethodCmpBndVolume;

    dor_object_get_current_method(object)(object);

    doe_system_current_method = DcMethodNull;

    volume->bll[0] = doe_system_bndvol_min_x;
    volume->bll[1] = doe_system_bndvol_min_y;
    volume->bll[2] = doe_system_bndvol_min_z;
    volume->fur[0] = doe_system_bndvol_max_x;
    volume->fur[1] = doe_system_bndvol_max_y;
    volume->fur[2] = doe_system_bndvol_max_z;

}
