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
 
/**************************************************************************

  Functions:
    ddr_kopengl_return_DROM_fcns 

**************************************************************************/


#include "drom.h"



DDt_DROM                *dde_kopengl_DROM_fcns        = (DDt_DROM *)0;
DtFlag			 dde_kopengl_drom_initialized = DcFalse;
DtFlag			 dde_kopengl_drom_pre_init    = DcTrue;
ddt_kopengl_drom_data 	 dde_kopengl_drom_data;
ddt_kopengl_drom_device *dde_kopengl_drom_curdevdat   = 0;
ddt_kopengl_drom_view   *dde_kopengl_drom_curviwdat   = 0;
ddt_kopengl_drom_window *dde_kopengl_drom_curwindat   = 0;
ddt_kopengl_drom_camera *dde_kopengl_drom_curcamdat   = 0;




DtPtr ddr_kopengl_return_DROM_fcns (void)
 {
  DDt_DROM *fcns;

  if (dde_kopengl_DROM_fcns == (DDt_DROM *)0) 
    {
     fcns = DDdevice_CreateDROMStruct();

     if (fcns == (DDt_DROM *)0) 
       {
	DDerror (ERR_CANT_ALLOC_MEM, "dde_kopengl_return_DROM_fcns",
			DcNullPtr);
       }

     fcns->version_number = 2;

     /* version 1 fields */

     fcns->pre_initialization	= ddr_kopengl_drom_pre_initialization;
     fcns->post_initialization	= ddr_kopengl_drom_post_initialization;
     fcns->create_local_device_data = ddr_kopengl_drom_create_local_device_data;
     fcns->create_local_view_data = ddr_kopengl_drom_create_local_view_data;
     fcns->create_local_window_data = ddr_kopengl_drom_create_local_window_data;
     fcns->start_update         = ddr_kopengl_drom_start_update;
     fcns->update_local_data    = ddr_kopengl_drom_update_local_data;
     fcns->update_studio        = ddr_kopengl_drom_update_studio;
     fcns->update_display       = ddr_kopengl_drom_update_display;
     fcns->camera	        = ddr_kopengl_drom_camera;
     fcns->light	        = ddr_kopengl_drom_light;
     fcns->set_camera_matrix    = ddr_kopengl_drom_set_camera_matrix;
     fcns->set_parallel_matrix  = ddr_kopengl_drom_set_parallel_matrix;
     fcns->set_perspective_matrix = ddr_kopengl_drom_set_perspective_matrix;
     fcns->set_projection_matrix  = ddr_kopengl_drom_set_projection_matrix;
     fcns->push_att_clpvol	  = ddr_kopengl_drom_push_att_clpvol;
     fcns->pop_att_clpvol	  = ddr_kopengl_drom_pop_att_clpvol;
     fcns->set_att_clpvol	  = ddr_kopengl_drom_set_att_clpvol;
     fcns->apply_att_clpvol	  = ddr_kopengl_drom_apply_att_clpvol;
     fcns->get_wcstofcsmat	  = ddr_kopengl_drom_get_wcstofcsmat;
     fcns->pop_lcstofcsmat	  = ddr_kopengl_drom_pop_lcstofcsmat;
     fcns->push_lcstofcsmat	  = ddr_kopengl_drom_push_lcstofcsmat;
     fcns->transform_clip_z_point = ddr_kopengl_drom_transform_clip_z_point;
     fcns->render_point_list	  = ddr_kopengl_drom_render_point_list;
     fcns->render_line_list	  = ddr_kopengl_drom_render_line_list;
     fcns->render_connected_line_list = 
             ddr_kopengl_drom_render_connected_line_list;
     fcns->render_triangle_list	= ddr_kopengl_drom_render_triangle_list;
     fcns->render_triangle_mesh	= ddr_kopengl_drom_render_triangle_mesh;
     fcns->set_att_ambint       = ddr_kopengl_drom_set_att_ambint;
     fcns->set_att_ambswi	= ddr_kopengl_drom_set_att_ambswi;
     fcns->set_att_bacfacculble	= ddr_kopengl_drom_set_att_bacfacculble;
     fcns->set_att_bacfacculswi	= ddr_kopengl_drom_set_att_bacfacculswi;
     fcns->set_att_clpswi	= ddr_kopengl_drom_set_att_clpswi;
     fcns->set_att_depcue	= ddr_kopengl_drom_set_att_depcue;
     fcns->set_att_depcueswi	= ddr_kopengl_drom_set_att_depcueswi;
     fcns->set_att_difclr	= ddr_kopengl_drom_set_att_difclr;
     fcns->set_att_difint	= ddr_kopengl_drom_set_att_difint;
     fcns->set_att_difswi	= ddr_kopengl_drom_set_att_difswi;
     fcns->set_att_hidsrfswi	= ddr_kopengl_drom_set_att_hidsrfswi;
     fcns->set_att_inttyp	= ddr_kopengl_drom_set_att_inttyp;
     fcns->set_att_lgtclr	= ddr_kopengl_drom_set_att_lgtclr;
     fcns->set_att_lgtint	= ddr_kopengl_drom_set_att_lgtint;
     fcns->set_att_lgttyp	= ddr_kopengl_drom_set_att_lgttyp;
     fcns->set_att_lintyp	= ddr_kopengl_drom_set_att_lintyp;
     fcns->set_att_linwid	= ddr_kopengl_drom_set_att_linwid;
     fcns->set_att_refswi	= ddr_kopengl_drom_set_att_refswi;
     fcns->set_att_reptyp	= ddr_kopengl_drom_set_att_reptyp;
     fcns->set_att_shaswi	= ddr_kopengl_drom_set_att_shaswi;
     fcns->set_att_shdidx	= ddr_kopengl_drom_set_att_shdidx;
     fcns->set_att_spcclr	= ddr_kopengl_drom_set_att_spcclr;
     fcns->set_att_spcfct	= ddr_kopengl_drom_set_att_spcfct;
     fcns->set_att_spcint       = ddr_kopengl_drom_set_att_spcint;
     fcns->set_att_spcswi	= ddr_kopengl_drom_set_att_spcswi;
     fcns->set_att_srfshd	= ddr_kopengl_drom_set_att_srfshd;
     fcns->set_att_stereo	= ddr_kopengl_drom_set_att_stereo;
     fcns->set_att_stereoswi	= ddr_kopengl_drom_set_att_stereoswi;
     fcns->set_att_transpclr	= ddr_kopengl_drom_set_att_transpclr;
     fcns->set_att_transpint	= ddr_kopengl_drom_set_att_transpint;
     fcns->set_att_transpswi	= ddr_kopengl_drom_set_att_transpswi;
     fcns->update_lcstowcsmat_lokatfrm = 
             ddr_kopengl_drom_update_lcstowcsmat_lokatfrm;
     fcns->update_lcstowcsmat_pop    = 
             ddr_kopengl_drom_update_lcstowcsmat_pop;
     fcns->update_lcstowcsmat_push   = 
             ddr_kopengl_drom_update_lcstowcsmat_push;
     fcns->update_lcstowcsmat_rotate = 
             ddr_kopengl_drom_update_lcstowcsmat_rotate;
     fcns->update_lcstowcsmat_scale  = 
             ddr_kopengl_drom_update_lcstowcsmat_scale;
     fcns->update_lcstowcsmat_shear  = 
             ddr_kopengl_drom_update_lcstowcsmat_shear;
     fcns->update_lcstowcsmat_tfmmat = 
             ddr_kopengl_drom_update_lcstowcsmat_tfmmat;
     fcns->update_lcstowcsmat_transl = 
             ddr_kopengl_drom_update_lcstowcsmat_transl;
     fcns->get_lcstowcsmat   = ddr_kopengl_drom_get_lcstowcsmat;
     fcns->set_att_lgtswi    = ddr_kopengl_drom_set_att_lgtswi;
     fcns->set_att_lgtspdexp = ddr_kopengl_drom_set_att_lgtspdexp;
     fcns->set_att_lgtspdang = ddr_kopengl_drom_set_att_lgtspdang;
     fcns->set_att_lgtatn    = ddr_kopengl_drom_set_att_lgtatn;
     fcns->set_att_mapbmpswi = ddr_kopengl_drom_set_att_mapbmpswi;
     fcns->set_att_mapbmp    = ddr_kopengl_drom_set_att_mapbmp;
     fcns->set_att_mapdifclrswi = ddr_kopengl_drom_set_att_mapdifclrswi;
     fcns->set_att_mapdifclr = ddr_kopengl_drom_set_att_mapdifclr;
     fcns->set_att_mapenvswi = ddr_kopengl_drom_set_att_mapenvswi;
     fcns->set_att_mapenv    = ddr_kopengl_drom_set_att_mapenv;
     fcns->set_att_maptrnintswi = ddr_kopengl_drom_set_att_maptrnintswi;
     fcns->set_att_maptrnint = ddr_kopengl_drom_set_att_maptrnint;
     fcns->get_lcstowcsmat   = ddr_kopengl_drom_get_lcstowcsmat;
     fcns->delete_local_device_data = ddr_kopengl_drom_delete_local_device_data;
     fcns->delete_local_view_data   = ddr_kopengl_drom_delete_local_view_data;
     fcns->delete_local_window_data = ddr_kopengl_drom_delete_local_window_data;
     fcns->use_separate_wireframe   = DcFalse;

     /* version 2 fields */
     fcns->set_att_srfedgclr  = ddr_kopengl_drom_set_att_srfedgclr;
     fcns->set_att_localaaswi = ddr_kopengl_drom_set_att_localaaswi;
     fcns->set_att_localaasty = ddr_kopengl_drom_set_att_localaasty;

     dde_kopengl_DROM_fcns = fcns;
    }

  return (DtPtr) dde_kopengl_DROM_fcns;
 }

