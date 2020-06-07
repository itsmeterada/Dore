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
#include <dore/dore_develop/private/stdtexatt.h>

    /************************************/
    /***  Data Structure Definitions  ***/
    /************************************/

typedef struct dynrnd_data {
   dot_object	       *current_device;
   dot_object	       *current_view;
   DtInt		device_index;
   DtInt		view_index;
   DtInt		studio_method;
   DtInt		display_method;
   DtInt		current_display_method;

   DtInt		light_switch_count;
   char               **light_switch;

   DtInt		bmp_max_textures;
   DtObject	       *bmp_mapping;
   DtObject	       *bmp_raster;
   dot_stdtexatt       *bmp_attributes;
   DtInt	       *bmp_size;

   DtInt		difclr_max_textures;
   DtObject	       *difclr_mapping;
   DtObject	       *difclr_raster;
   dot_stdtexatt       *difclr_attributes;
   DtInt	       *difclr_size;

   DtInt		env_max_textures;
   DtObject	       *env_mapping;
   DtObject	       *env_raster;
   dot_stdtexatt       *env_attributes;
   DtInt	       *env_size;

   DtInt		trnint_max_textures;
   DtObject	       *trnint_mapping;
   DtObject	       *trnint_raster;
   dot_stdtexatt       *trnint_attributes;
   DtInt	       *trnint_size;
} drt_dynrnd_data;

typedef struct dynrnd_device_data {
   DtInt modification_sequence_number;
   DtPtr user_data;
} drt_dynrnd_device_data;

typedef struct dynrnd_window_data {
   DtPFI del_fcn;
   DtPtr user_data;
} drt_dynrnd_window_data;

typedef struct dynrnd_view_data {
   DtInt		    modification_sequence_number;
   DtPFI		    crt_fcn;
   DtPFI		    del_fcn;
   DtPtr		    user_data;
   DtInt		    window_devicecount;
   dot_object 		  **window_device;
   drt_dynrnd_window_data **window_data;
} drt_dynrnd_view_data;

typedef struct dynrnd_driver_data {
   DtInt		driver_count;
   drt_dynrnd_view_data **view_data;
} drt_dynrnd_driver_data;

typedef struct {
    char        *name;
    DtMethodPtr  routine;
} drt_dynrnd_methods;


    /**************************/
    /***  Global Variables  ***/
    /**************************/

extern drt_dynrnd_data		 dre_dynrnd_data;
extern drt_dynrnd_device_data	*dre_dynrnd_curdevdat;
extern drt_dynrnd_view_data	*dre_dynrnd_curviwdat;
extern drt_dynrnd_window_data	*dre_dynrnd_curwindat;
extern DDt_DROM			*dre_dynrnd_DROM_fcns;
extern DDt_DROMMethod		*dre_dynrnd_DROM_method;
extern DDt_DROMMethod		*dre_dynrnd_DROM_default_method;
extern DtInt			 dre_dynrnd_DROM_default_method_max_count;
extern DtInt			 dre_dynrnd_DROM_default_method_count;
extern drt_dynrnd_methods	 dre_dynrnd_attribute[];
extern drt_dynrnd_methods	 dre_dynrnd_studio_method[];
extern drt_dynrnd_methods	 dre_dynrnd_display_method[];


    /*****************************/
    /***  Function Prototypes  ***/
    /*****************************/

void  drr_dynrnd_initialize_attributes (void);
void  drr_dynrnd_ambint (void);
void  drr_dynrnd_ambswi (void);
void  drr_dynrnd_bacfacculble (void);
void  drr_dynrnd_bacfacculswi (void);
void  drr_dynrnd_depcue (void);
void  drr_dynrnd_depcueswi (void);
void  drr_dynrnd_difswi (void);
void  drr_dynrnd_difclr (void);
void  drr_dynrnd_difint (void);
void  drr_dynrnd_hidsrfswi (void);
void  drr_dynrnd_inttyp (void);
void  drr_dynrnd_lintyp (void);
void  drr_dynrnd_linwid (void);
void  drr_dynrnd_localaasty (void);
void  drr_dynrnd_localaaswi (void);
void  drr_dynrnd_refswi (void);
void  drr_dynrnd_reptyp (void);
void  drr_dynrnd_shaswi (void);
void  drr_dynrnd_shdidx (void);
void  drr_dynrnd_spcclr (void);
void  drr_dynrnd_spcfct (void);
void  drr_dynrnd_spcint (void);
void  drr_dynrnd_spcswi (void);
void  drr_dynrnd_srfedgclr (void);
void  drr_dynrnd_srfshd (void);
void  drr_dynrnd_transpclr (void);
void  drr_dynrnd_transpint (void);
void  drr_dynrnd_transpswi (void);
void  drr_dynrnd_camera (dot_object *);
void  drr_dynrnd_stereo (void);
void  drr_dynrnd_stereoswi (void);
void  drr_dynrnd_camera_matrix (dot_object *);
void  drr_dynrnd_parallel_matrix (dot_object *);
void  drr_dynrnd_perspective_matrix (dot_object *);
void  drr_dynrnd_projection_matrix (dot_object *);
drt_dynrnd_device_data
     *drr_dynrnd_create_device_data (void);
drt_dynrnd_driver_data
     *drr_dynrnd_create_driver_data (void);
drt_dynrnd_view_data
      *drr_dynrnd_create_view_data (void);
void  drr_dynrnd_delete_device_data (DtObject, drt_dynrnd_device_data *);
void  drr_dynrnd_delete_driver_data (DtObject, drt_dynrnd_driver_data *);
void  drr_dynrnd_delete_view_data (drt_dynrnd_view_data *);
void  drr_dynrnd_delete_window_data (drt_dynrnd_window_data *);
DtInt drr_dynrnd_install_renderer (DtInt);
void  drr_dynrnd_new_class (DtPtr, DtInt);
void  drr_dynrnd_set_object_methods (void);
void  drr_dynrnd_update_default_methods (void);
void  drr_dynrnd_lcstowcs (DtMatrix4x4, DtGlobalCall, DtInt);
void  drr_dynrnd_lokatfrm (dot_object *);
void  drr_dynrnd_rotate (dot_object *);
void  drr_dynrnd_scale (dot_object *);
void  drr_dynrnd_shear (dot_object *);
void  drr_dynrnd_tfmmat (dot_object *);
void  drr_dynrnd_transl (dot_object *);
void  drr_dynrnd_get_wcstofcsmat (DtMatrix4x4);
void  drr_dynrnd_light (dot_object *);
void  drr_dynrnd_lgtatn (void);
void  drr_dynrnd_lgtclr (void);
void  drr_dynrnd_lgtint (void);
void  drr_dynrnd_lgtspdang (void);
void  drr_dynrnd_lgtspdexp (void);
void  drr_dynrnd_lgtswi (void);
void  drr_dynrnd_lgttyp (void);
void  drr_dynrnd_mapbmp (void);
void  drr_dynrnd_mapbmpswi (void);
void  drr_dynrnd_mapdifclr (void);
void  drr_dynrnd_mapdifclrswi (void);
void  drr_dynrnd_mapenv (void);
void  drr_dynrnd_mapenvswi (void);
void  drr_dynrnd_maptrnint (void);
void  drr_dynrnd_maptrnintswi (void);
void  drr_dynrnd_clpswi (void);
void  drr_dynrnd_clpvol (dot_object *);
void  drr_dynrnd_global_clpvol (DtGlobalCall, DtInt);
void  drr_dynrnd_pntlst (dot_object *);
void  drr_dynrnd_linlst (dot_object *);
void  drr_dynrnd_plylin (dot_object *);
void  drr_dynrnd_trilst (dot_object *);
void  drr_dynrnd_trimsh (dot_object *);
void  drr_dynrnd_tristrip (dot_object *);
void  drr_dynrnd_anntxt (dot_object *);
void  drr_dynrnd_plymkr (dot_object *);
void  drr_dynrnd_varpntlst (dot_object *);
void  drr_dynrnd_varlinlst (dot_object *);
void  drr_dynrnd_vartrimsh (dot_object *);
void  drr_dynrnd_vartristrip (dot_object *);
void  drr_dynrnd_update_device_and_view (dot_object *, dot_object *);
void  drr_dynrnd_render (dot_object *, dot_object *, DtFlag, DtFlag);
void  drr_dynrnd_transporientclr (void);
void  drr_dynrnd_transporientexp (void);
void  drr_dynrnd_transporientint (void);
void  drr_dynrnd_transporientswi (void);
void  drr_dynrnd_smpadapt (void);
void  drr_dynrnd_smpadaptswi (void);
void  drr_dynrnd_smpfilter (void);
void  drr_dynrnd_smpjitter (void);
void  drr_dynrnd_smpjitterswi (void);
void  drr_dynrnd_smpsuper (void);
void  drr_dynrnd_smpsuperswi (void);
void  drr_dynrnd_traverse_studio_group (void);
void  drr_dynrnd_traverse_display_group (void);
void  dre_dynrnd_reset_current_display_method (DtInt);
