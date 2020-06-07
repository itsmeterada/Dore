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
 
/*
  ======================================================================
  Functions:
	DtPtr ddr_sunview_cxp_return_DROM_fcns ()

  ======================================================================
 */
#include "drom.h"

struct drom_data	dde_sunview_cxp_drom_data;
struct drom_device_data	*dde_sunview_cxp_drom_curdevdat;
struct drom_view_data	*dde_sunview_cxp_drom_curviwdat;
struct drom_window_data *dde_sunview_cxp_drom_curwindat;
struct drom_camera_data *dde_sunview_cxp_drom_curcamdat;
struct drom_light_data	*dde_sunview_cxp_drom_curlgtdat;
int			dde_sunview_cxp_drom_pre_init;


extern ddr_sunview_cxp_drom_pre_initialization();
extern ddr_sunview_cxp_drom_post_initialization();
extern ddr_sunview_cxp_drom_create_local_device_data();
extern ddr_sunview_cxp_drom_create_local_view_data();
extern ddr_sunview_cxp_drom_create_local_window_data();
extern ddr_sunview_cxp_drom_start_update();
extern ddr_sunview_cxp_drom_update_local_data();
extern ddr_sunview_cxp_drom_update_studio();
extern ddr_sunview_cxp_drom_update_display();
extern ddr_sunview_cxp_drom_camera();
extern ddr_sunview_cxp_drom_light();
extern ddr_sunview_cxp_drom_set_camera_matrix();
extern ddr_sunview_cxp_drom_set_parallel_matrix();
extern ddr_sunview_cxp_drom_set_perspective_matrix();
extern ddr_sunview_cxp_drom_set_projection_matrix();
extern ddr_sunview_cxp_drom_push_att_clpvol();
extern ddr_sunview_cxp_drom_pop_att_clpvol();
extern ddr_sunview_cxp_drom_set_att_clpvol();
extern ddr_sunview_cxp_drom_apply_att_clpvol();
extern ddr_sunview_cxp_drom_get_wcstofcsmat();
extern ddr_sunview_cxp_drom_pop_lcstofcsmat();
extern ddr_sunview_cxp_drom_push_lcstofcsmat();
extern ddr_sunview_cxp_drom_transform_clip_z_point();
extern ddr_sunview_cxp_drom_render_point_list();
extern ddr_sunview_cxp_drom_render_line_list();
extern ddr_sunview_cxp_drom_render_connected_line_list();
extern ddr_sunview_cxp_drom_render_triangle_list();
extern ddr_sunview_cxp_drom_render_triangle_mesh();
extern ddr_sunview_cxp_drom_set_att_ambint();
extern ddr_sunview_cxp_drom_set_att_ambswi();
extern ddr_sunview_cxp_drom_set_att_bacfacculble();
extern ddr_sunview_cxp_drom_set_att_bacfacculswi();
extern ddr_sunview_cxp_drom_set_att_clpswi();
extern ddr_sunview_cxp_drom_set_att_depcue();
extern ddr_sunview_cxp_drom_set_att_depcueswi();
extern ddr_sunview_cxp_drom_set_att_difclr();
extern ddr_sunview_cxp_drom_set_att_difint();
extern ddr_sunview_cxp_drom_set_att_difswi();
extern ddr_sunview_cxp_drom_set_att_hidsrfswi();
extern ddr_sunview_cxp_drom_set_att_inttyp();
extern ddr_sunview_cxp_drom_set_att_lgtclr();
extern ddr_sunview_cxp_drom_set_att_lgtint();
extern ddr_sunview_cxp_drom_set_att_lgttyp();
extern ddr_sunview_cxp_drom_set_att_lintyp();
extern ddr_sunview_cxp_drom_set_att_linwid();
extern ddr_sunview_cxp_drom_set_att_refswi();
extern ddr_sunview_cxp_drom_set_att_reptyp();
extern ddr_sunview_cxp_drom_set_att_shaswi();
extern ddr_sunview_cxp_drom_set_att_shdidx();
extern ddr_sunview_cxp_drom_set_att_spcclr();
extern ddr_sunview_cxp_drom_set_att_spcfct();
extern ddr_sunview_cxp_drom_set_att_spcint();
extern ddr_sunview_cxp_drom_set_att_spcswi();
extern ddr_sunview_cxp_drom_set_att_srfshd();
extern ddr_sunview_cxp_drom_set_att_stereo();
extern ddr_sunview_cxp_drom_set_att_stereoswi();
extern ddr_sunview_cxp_drom_set_att_transpclr();
extern ddr_sunview_cxp_drom_set_att_transpint();
extern ddr_sunview_cxp_drom_set_att_transpswi();
extern ddr_sunview_cxp_drom_update_lcstowcsmat_lokatfrm();
extern ddr_sunview_cxp_drom_update_lcstowcsmat_pop();
extern ddr_sunview_cxp_drom_update_lcstowcsmat_push();
extern ddr_sunview_cxp_drom_update_lcstowcsmat_rotate();
extern ddr_sunview_cxp_drom_update_lcstowcsmat_scale();
extern ddr_sunview_cxp_drom_update_lcstowcsmat_shear();
extern ddr_sunview_cxp_drom_update_lcstowcsmat_tfmmat();
extern ddr_sunview_cxp_drom_update_lcstowcsmat_transl();
extern ddr_sunview_cxp_drom_set_att_lgtswi();
extern ddr_sunview_cxp_drom_set_att_lgtspdexp();
extern ddr_sunview_cxp_drom_set_att_lgtspdang();
extern ddr_sunview_cxp_drom_set_att_lgtatn();
extern ddr_sunview_cxp_drom_set_att_mapbmpswi();
extern ddr_sunview_cxp_drom_set_att_mapbmp();
extern ddr_sunview_cxp_drom_set_att_mapdifclrswi();
extern ddr_sunview_cxp_drom_set_att_mapdifclr();
extern ddr_sunview_cxp_drom_set_att_mapenvswi();
extern ddr_sunview_cxp_drom_set_att_mapenv();
extern ddr_sunview_cxp_drom_set_att_maptrnintswi();
extern ddr_sunview_cxp_drom_set_att_maptrnint();
extern ddr_sunview_cxp_drom_get_lcstowcsmat();
extern ddr_sunview_cxp_drom_delete_local_device_data();
extern ddr_sunview_cxp_drom_delete_local_view_data();
extern ddr_sunview_cxp_drom_delete_local_window_data();

DDt_DROM *dde_sunview_cxp_DROM_fcns = (DDt_DROM *)0;

DtPtr
ddr_sunview_cxp_return_DROM_fcns()
{
    DDt_DROM *fcns;

    if (dde_sunview_cxp_DROM_fcns == (DDt_DROM *)0) {
	fcns = DDdevice_CreateDROMStruct();

	if (fcns == (DDt_DROM *)0) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_sunview_cxp_return_DROM_fcns",
			DcNullPtr);
	}

	fcns->version_number 			= 1;

	/* version 1 fields */

	fcns->pre_initialization		= (DtPFI) ddr_sunview_cxp_drom_pre_initialization;               
	fcns->post_initialization		= (DtPFI) ddr_sunview_cxp_drom_post_initialization;              
	fcns->create_local_device_data		= (DtPFI) ddr_sunview_cxp_drom_create_local_device_data;         
	fcns->create_local_view_data		= (DtPFI) ddr_sunview_cxp_drom_create_local_view_data;           
	fcns->create_local_window_data		= (DtPFI) ddr_sunview_cxp_drom_create_local_window_data;         
	fcns->start_update			= (DtPFI) ddr_sunview_cxp_drom_start_update;                     
	fcns->update_local_data			= (DtPFI) ddr_sunview_cxp_drom_update_local_data;                
	fcns->update_studio	= (DtPFI) ddr_sunview_cxp_drom_update_studio;    
	fcns->update_display		= (DtPFI) ddr_sunview_cxp_drom_update_display;             
	fcns->camera			= (DtPFI) ddr_sunview_cxp_drom_camera;                      
	fcns->light			= (DtPFI) ddr_sunview_cxp_drom_light;                       
	fcns->set_camera_matrix			= (DtPFI) ddr_sunview_cxp_drom_set_camera_matrix;                
	fcns->set_parallel_matrix		= (DtPFI) ddr_sunview_cxp_drom_set_parallel_matrix;              
	fcns->set_perspective_matrix		= (DtPFI) ddr_sunview_cxp_drom_set_perspective_matrix;           
	fcns->set_projection_matrix		= (DtPFI) ddr_sunview_cxp_drom_set_projection_matrix;            
	fcns->push_att_clpvol			= (DtPFI) ddr_sunview_cxp_drom_push_att_clpvol;                  
	fcns->pop_att_clpvol			= (DtPFI) ddr_sunview_cxp_drom_pop_att_clpvol;                   
	fcns->set_att_clpvol			= (DtPFI) ddr_sunview_cxp_drom_set_att_clpvol;                   
	fcns->apply_att_clpvol			= (DtPFI) ddr_sunview_cxp_drom_apply_att_clpvol;                 
	fcns->get_wcstofcsmat			= (DtPFI) ddr_sunview_cxp_drom_get_wcstofcsmat;                  
	fcns->pop_lcstofcsmat			= (DtPFI) ddr_sunview_cxp_drom_pop_lcstofcsmat;                  
	fcns->push_lcstofcsmat			= (DtPFI) ddr_sunview_cxp_drom_push_lcstofcsmat;                 
	fcns->transform_clip_z_point		= (DtPFI) ddr_sunview_cxp_drom_transform_clip_z_point;           
	fcns->render_point_list			= (DtPFI) ddr_sunview_cxp_drom_render_point_list;                
	fcns->render_line_list			= (DtPFI) ddr_sunview_cxp_drom_render_line_list;                 
	fcns->render_connected_line_list	= (DtPFI) ddr_sunview_cxp_drom_render_connected_line_list;       
	fcns->render_triangle_list		= (DtPFI) ddr_sunview_cxp_drom_render_triangle_list;             
	fcns->render_triangle_mesh		= (DtPFI) ddr_sunview_cxp_drom_render_triangle_mesh;             
	fcns->set_att_ambint			= (DtPFI) ddr_sunview_cxp_drom_set_att_ambint;                   
	fcns->set_att_ambswi			= (DtPFI) ddr_sunview_cxp_drom_set_att_ambswi;                   
	fcns->set_att_bacfacculble		= (DtPFI) ddr_sunview_cxp_drom_set_att_bacfacculble;             
	fcns->set_att_bacfacculswi		= (DtPFI) ddr_sunview_cxp_drom_set_att_bacfacculswi;             
	fcns->set_att_clpswi			= (DtPFI) ddr_sunview_cxp_drom_set_att_clpswi;                   
	fcns->set_att_depcue			= (DtPFI) ddr_sunview_cxp_drom_set_att_depcue;                   
	fcns->set_att_depcueswi			= (DtPFI) ddr_sunview_cxp_drom_set_att_depcueswi;                
	fcns->set_att_difclr			= (DtPFI) ddr_sunview_cxp_drom_set_att_difclr;                   
	fcns->set_att_difint			= (DtPFI) ddr_sunview_cxp_drom_set_att_difint;                   
	fcns->set_att_difswi			= (DtPFI) ddr_sunview_cxp_drom_set_att_difswi;                   
	fcns->set_att_hidsrfswi			= (DtPFI) ddr_sunview_cxp_drom_set_att_hidsrfswi;                
	fcns->set_att_inttyp			= (DtPFI) ddr_sunview_cxp_drom_set_att_inttyp;                   
	fcns->set_att_lgtclr			= (DtPFI) ddr_sunview_cxp_drom_set_att_lgtclr;                   
	fcns->set_att_lgtint			= (DtPFI) ddr_sunview_cxp_drom_set_att_lgtint;                   
	fcns->set_att_lgttyp			= (DtPFI) ddr_sunview_cxp_drom_set_att_lgttyp;                   
	fcns->set_att_lintyp			= (DtPFI) ddr_sunview_cxp_drom_set_att_lintyp;                   
	fcns->set_att_linwid			= (DtPFI) ddr_sunview_cxp_drom_set_att_linwid;                   
	fcns->set_att_refswi			= (DtPFI) ddr_sunview_cxp_drom_set_att_refswi;                   
	fcns->set_att_reptyp			= (DtPFI) ddr_sunview_cxp_drom_set_att_reptyp;                   
	fcns->set_att_shaswi			= (DtPFI) ddr_sunview_cxp_drom_set_att_shaswi;                   
	fcns->set_att_shdidx			= (DtPFI) ddr_sunview_cxp_drom_set_att_shdidx;                   
	fcns->set_att_spcclr			= (DtPFI) ddr_sunview_cxp_drom_set_att_spcclr;                   
	fcns->set_att_spcfct			= (DtPFI) ddr_sunview_cxp_drom_set_att_spcfct;                   
	fcns->set_att_spcint			= (DtPFI) ddr_sunview_cxp_drom_set_att_spcint;                   
	fcns->set_att_spcswi			= (DtPFI) ddr_sunview_cxp_drom_set_att_spcswi;                   
	fcns->set_att_srfshd			= (DtPFI) ddr_sunview_cxp_drom_set_att_srfshd;                   
	fcns->set_att_stereo			= (DtPFI) ddr_sunview_cxp_drom_set_att_stereo;                   
	fcns->set_att_stereoswi			= (DtPFI) ddr_sunview_cxp_drom_set_att_stereoswi;                
	fcns->set_att_transpclr			= (DtPFI) ddr_sunview_cxp_drom_set_att_transpclr;                
	fcns->set_att_transpint			= (DtPFI) ddr_sunview_cxp_drom_set_att_transpint;                
	fcns->set_att_transpswi			= (DtPFI) ddr_sunview_cxp_drom_set_att_transpswi;                
	fcns->update_lcstowcsmat_lokatfrm	= (DtPFI) ddr_sunview_cxp_drom_update_lcstowcsmat_lokatfrm;      
	fcns->update_lcstowcsmat_pop		= (DtPFI) ddr_sunview_cxp_drom_update_lcstowcsmat_pop;           
	fcns->update_lcstowcsmat_push		= (DtPFI) ddr_sunview_cxp_drom_update_lcstowcsmat_push;          
	fcns->update_lcstowcsmat_rotate		= (DtPFI) ddr_sunview_cxp_drom_update_lcstowcsmat_rotate;        
	fcns->update_lcstowcsmat_scale		= (DtPFI) ddr_sunview_cxp_drom_update_lcstowcsmat_scale;         
	fcns->update_lcstowcsmat_shear		= (DtPFI) ddr_sunview_cxp_drom_update_lcstowcsmat_shear;         
	fcns->update_lcstowcsmat_tfmmat		= (DtPFI) ddr_sunview_cxp_drom_update_lcstowcsmat_tfmmat;        
	fcns->update_lcstowcsmat_transl		= (DtPFI) ddr_sunview_cxp_drom_update_lcstowcsmat_transl;        
	fcns->get_lcstowcsmat			= (DtPFI) ddr_sunview_cxp_drom_get_lcstowcsmat;
	fcns->set_att_lgtswi 			= (DtPFI) ddr_sunview_cxp_drom_set_att_lgtswi;
	fcns->set_att_lgtspdexp 		= (DtPFI) ddr_sunview_cxp_drom_set_att_lgtspdexp;
	fcns->set_att_lgtspdang 		= (DtPFI) ddr_sunview_cxp_drom_set_att_lgtspdang;
	fcns->set_att_lgtatn 			= (DtPFI) ddr_sunview_cxp_drom_set_att_lgtatn;
	fcns->set_att_mapbmpswi 		= (DtPFI) ddr_sunview_cxp_drom_set_att_mapbmpswi;
	fcns->set_att_mapbmp 			= (DtPFI) ddr_sunview_cxp_drom_set_att_mapbmp;
	fcns->set_att_mapdifclrswi 		= (DtPFI) ddr_sunview_cxp_drom_set_att_mapdifclrswi;
	fcns->set_att_mapdifclr 		= (DtPFI) ddr_sunview_cxp_drom_set_att_mapdifclr;
	fcns->set_att_mapenvswi 		= (DtPFI) ddr_sunview_cxp_drom_set_att_mapenvswi;
	fcns->set_att_mapenv 			= (DtPFI) ddr_sunview_cxp_drom_set_att_mapenv;
	fcns->set_att_maptrnintswi 		= (DtPFI) ddr_sunview_cxp_drom_set_att_maptrnintswi;
	fcns->set_att_maptrnint 		= (DtPFI) ddr_sunview_cxp_drom_set_att_maptrnint;
	fcns->get_lcstowcsmat 			= (DtPFI) ddr_sunview_cxp_drom_get_lcstowcsmat;
	fcns->delete_local_device_data 		= (DtPFI) ddr_sunview_cxp_drom_delete_local_device_data;
	fcns->delete_local_view_data 		= (DtPFI) ddr_sunview_cxp_drom_delete_local_view_data;
	fcns->delete_local_window_data 		= (DtPFI) ddr_sunview_cxp_drom_delete_local_window_data;
	fcns->use_separate_wireframe		= DcFalse;

	dde_sunview_cxp_DROM_fcns = fcns;
    }

    return (DtPtr) dde_sunview_cxp_DROM_fcns;
}
