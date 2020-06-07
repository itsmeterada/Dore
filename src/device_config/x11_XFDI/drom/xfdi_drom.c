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
	int ddr_x11_XFDI_drom_new_class (class_name, class_id)
	DtPtr ddr_x11_XFDI_return_DROM_fcns ()
	DtPtr ddr_x11_XFDI_return_DROM_methods ()

  ======================================================================
 */
#include "drom.h"
#include "dore/dore_develop/develop.h"

ddt_x11_XFDI_drom_data		 dde_x11_XFDI_drom_data;
ddt_x11_XFDI_drom_device	*dde_x11_XFDI_drom_curdevdat = 0;
ddt_x11_XFDI_drom_view  	*dde_x11_XFDI_drom_curviwdat = 0;
ddt_x11_XFDI_drom_window	*dde_x11_XFDI_drom_curwindat = 0;
ddt_x11_XFDI_drom_camera	*dde_x11_XFDI_drom_curcamdat = 0;
DtSwitch			 dde_x11_XFDI_drom_pre_init = DcTrue;

extern DtPtr ddr_x11_XFDI_drom_cvtclrs_create();
extern ddr_x11_XFDI_drom_cvtclrs_print();
extern ddr_x11_XFDI_drom_cvtclrs_delete();

extern DtPtr ddr_x11_XFDI_drom_filerstr_create();
extern ddr_x11_XFDI_drom_filerstr_delete();
extern ddr_x11_XFDI_drom_filerstr_print();

extern DtPtr ddr_x11_XFDI_drom_prisrf_create();
extern ddr_x11_XFDI_drom_prisrf_print();
extern ddr_x11_XFDI_drom_prisrf_delete();

extern DtPtr ddr_x11_XFDI_drom_rstr_create();
extern ddr_x11_XFDI_drom_rstr_delete();
extern ddr_x11_XFDI_drom_rstr_print();

extern DtPtr ddr_x11_XFDI_drom_trilst_create();
extern ddr_x11_XFDI_drom_trilst_print();
extern ddr_x11_XFDI_drom_trilst_delete();

extern DtPtr ddr_x11_XFDI_drom_trimsh_create();
extern ddr_x11_XFDI_drom_trimsh_print();
extern ddr_x11_XFDI_drom_trimsh_delete();

ddt_x11_XFDI_drom_class_data dde_x11_XFDI_drom_class_data[] = {
  { "DoFileRaster", -1, 
	  (DtPFI)ddr_x11_XFDI_drom_filerstr_create, 
	  ddr_x11_XFDI_drom_filerstr_print, 
	  ddr_x11_XFDI_drom_filerstr_delete, -1},
  { "DoLineList", -1, 
	  (DtPFI)ddr_x11_XFDI_drom_cvtclrs_create, 
	  ddr_x11_XFDI_drom_cvtclrs_print, 
	  ddr_x11_XFDI_drom_cvtclrs_delete, -1},
  { "DoPointList", -1, 
	  (DtPFI)ddr_x11_XFDI_drom_cvtclrs_create, 
	  ddr_x11_XFDI_drom_cvtclrs_print, 
	  ddr_x11_XFDI_drom_cvtclrs_delete, -1},
  { "DoPolyline", -1, 
	  (DtPFI)ddr_x11_XFDI_drom_cvtclrs_create, 
	  ddr_x11_XFDI_drom_cvtclrs_print, 
	  ddr_x11_XFDI_drom_cvtclrs_delete, -1},
  { "DoPrimSurf", -1, 
	  (DtPFI)ddr_x11_XFDI_drom_prisrf_create, 
	  ddr_x11_XFDI_drom_prisrf_print, 
	  ddr_x11_XFDI_drom_prisrf_delete, -1},
  { "DoRaster", -1, 
	  (DtPFI)ddr_x11_XFDI_drom_rstr_create, 
	  ddr_x11_XFDI_drom_rstr_print, 
	  ddr_x11_XFDI_drom_rstr_delete, -1},
  { "DoTriangleStrip", -1, 
	  (DtPFI)ddr_x11_XFDI_drom_cvtclrs_create, 
	  ddr_x11_XFDI_drom_cvtclrs_print, 
	  ddr_x11_XFDI_drom_cvtclrs_delete, -1},
  { "DoTriangleList", -1, 
	  (DtPFI)ddr_x11_XFDI_drom_trilst_create, 
	  ddr_x11_XFDI_drom_trilst_print, 
	  ddr_x11_XFDI_drom_trilst_delete, -1},
  { "DoTriangleMesh", -1, 
	  (DtPFI)ddr_x11_XFDI_drom_trimsh_create, 
	  ddr_x11_XFDI_drom_trimsh_print, 
	  ddr_x11_XFDI_drom_trimsh_delete, -1},
  { "DoVarLineList", -1,
	  (DtPFI)ddr_x11_XFDI_drom_cvtclrs_create, 
	  ddr_x11_XFDI_drom_cvtclrs_print, 
	  ddr_x11_XFDI_drom_cvtclrs_delete, -1},
  { "DoVarPointList", -1,
	  (DtPFI)ddr_x11_XFDI_drom_cvtclrs_create, 
	  ddr_x11_XFDI_drom_cvtclrs_print, 
	  ddr_x11_XFDI_drom_cvtclrs_delete, -1},
  { "DoVarTriangleMesh", -1, 
	  (DtPFI)ddr_x11_XFDI_drom_cvtclrs_create, 
	  ddr_x11_XFDI_drom_cvtclrs_print, 
	  ddr_x11_XFDI_drom_cvtclrs_delete, -1},
  { "DoVarTriangleStrip", -1, 
	  (DtPFI)ddr_x11_XFDI_drom_cvtclrs_create, 
	  ddr_x11_XFDI_drom_cvtclrs_print, 
	  ddr_x11_XFDI_drom_cvtclrs_delete, -1},
  { 0, -1, (DtPFI)0, (DtPFI)0, (DtPFI)0, -1},
};
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_new_class(class_name, class_id)
     DtPtr class_name;
     DtInt class_id;
{
    DtInt i;

    for (i=0; dde_x11_XFDI_drom_class_data[i].name != 0; i++) {
	if (strcmp(dde_x11_XFDI_drom_class_data[i].name, class_name) == 0) {
	    dde_x11_XFDI_drom_class_data[i].id = class_id;
	    dde_x11_XFDI_drom_class_data[i].index =
		    DDclass_AddObjectData(class_id, 
					    dde_x11_XFDI_drom_class_data[i].crt_rtn,
					    dde_x11_XFDI_drom_class_data[i].prt_rtn,
					    dde_x11_XFDI_drom_class_data[i].del_rtn);

	}
    }
}
/*
 ======================================================================
 */
extern ddr_x11_XFDI_drom_pre_initialization();
extern ddr_x11_XFDI_drom_post_initialization();
extern ddr_x11_XFDI_drom_create_local_device_data();
extern ddr_x11_XFDI_drom_create_local_view_data();
extern ddr_x11_XFDI_drom_create_local_window_data();
extern ddr_x11_XFDI_drom_start_update();
extern ddr_x11_XFDI_drom_update_local_data();
extern ddr_x11_XFDI_drom_update_studio();
extern ddr_x11_XFDI_drom_update_display();
extern ddr_x11_XFDI_drom_camera();
extern ddr_x11_XFDI_drom_light();
extern ddr_x11_XFDI_drom_set_camera_matrix();
extern ddr_x11_XFDI_drom_set_parallel_matrix();
extern ddr_x11_XFDI_drom_set_perspective_matrix();
extern ddr_x11_XFDI_drom_set_projection_matrix();
extern ddr_x11_XFDI_drom_push_att_clpvol();
extern ddr_x11_XFDI_drom_pop_att_clpvol();
extern ddr_x11_XFDI_drom_set_att_clpvol();
extern ddr_x11_XFDI_drom_apply_att_clpvol();
extern ddr_x11_XFDI_drom_get_wcstofcsmat();
extern ddr_x11_XFDI_drom_pop_lcstofcsmat();
extern ddr_x11_XFDI_drom_push_lcstofcsmat();
extern ddr_x11_XFDI_drom_transform_clip_z_point();
extern ddr_x11_XFDI_drom_render_point_list();
extern ddr_x11_XFDI_drom_render_line_list();
extern ddr_x11_XFDI_drom_render_connected_line_list();
extern ddr_x11_XFDI_drom_render_triangle_list();
extern ddr_x11_XFDI_drom_render_triangle_mesh();
extern ddr_x11_XFDI_drom_set_att_ambint();
extern ddr_x11_XFDI_drom_set_att_ambswi();
extern ddr_x11_XFDI_drom_set_att_bacfacculble();
extern ddr_x11_XFDI_drom_set_att_bacfacculswi();
extern ddr_x11_XFDI_drom_set_att_clpswi();
extern ddr_x11_XFDI_drom_set_att_depcue();
extern ddr_x11_XFDI_drom_set_att_depcueswi();
extern ddr_x11_XFDI_drom_set_att_difclr();
extern ddr_x11_XFDI_drom_set_att_difint();
extern ddr_x11_XFDI_drom_set_att_difswi();
extern ddr_x11_XFDI_drom_set_att_hidsrfswi();
extern ddr_x11_XFDI_drom_set_att_inttyp();
extern ddr_x11_XFDI_drom_set_att_lgtclr();
extern ddr_x11_XFDI_drom_set_att_lgtint();
extern ddr_x11_XFDI_drom_set_att_lgttyp();
extern ddr_x11_XFDI_drom_set_att_lintyp();
extern ddr_x11_XFDI_drom_set_att_linwid();
extern ddr_x11_XFDI_drom_set_att_refswi();
extern ddr_x11_XFDI_drom_set_att_reptyp();
extern ddr_x11_XFDI_drom_set_att_shaswi();
extern ddr_x11_XFDI_drom_set_att_shdidx();
extern ddr_x11_XFDI_drom_set_att_spcclr();
extern ddr_x11_XFDI_drom_set_att_spcfct();
extern ddr_x11_XFDI_drom_set_att_spcint();
extern ddr_x11_XFDI_drom_set_att_spcswi();
extern ddr_x11_XFDI_drom_set_att_srfshd();
extern ddr_x11_XFDI_drom_set_att_stereo();
extern ddr_x11_XFDI_drom_set_att_stereoswi();
extern ddr_x11_XFDI_drom_set_att_transpclr();
extern ddr_x11_XFDI_drom_set_att_transpint();
extern ddr_x11_XFDI_drom_set_att_transpswi();
extern ddr_x11_XFDI_drom_update_lcstowcsmat_lokatfrm();
extern ddr_x11_XFDI_drom_update_lcstowcsmat_pop();
extern ddr_x11_XFDI_drom_update_lcstowcsmat_push();
extern ddr_x11_XFDI_drom_update_lcstowcsmat_rotate();
extern ddr_x11_XFDI_drom_update_lcstowcsmat_scale();
extern ddr_x11_XFDI_drom_update_lcstowcsmat_shear();
extern ddr_x11_XFDI_drom_update_lcstowcsmat_tfmmat();
extern ddr_x11_XFDI_drom_update_lcstowcsmat_transl();
extern ddr_x11_XFDI_drom_set_att_lgtswi();
extern ddr_x11_XFDI_drom_set_att_lgtspdexp();
extern ddr_x11_XFDI_drom_set_att_lgtspdang();
extern ddr_x11_XFDI_drom_set_att_lgtatn();
extern ddr_x11_XFDI_drom_set_att_mapbmpswi();
extern ddr_x11_XFDI_drom_set_att_mapbmp();
extern ddr_x11_XFDI_drom_set_att_mapdifclrswi();
extern ddr_x11_XFDI_drom_set_att_mapdifclr();
extern ddr_x11_XFDI_drom_set_att_mapenvswi();
extern ddr_x11_XFDI_drom_set_att_mapenv();
extern ddr_x11_XFDI_drom_set_att_maptrnintswi();
extern ddr_x11_XFDI_drom_set_att_maptrnint();
extern ddr_x11_XFDI_drom_get_lcstowcsmat();
extern ddr_x11_XFDI_drom_delete_local_device_data();
extern ddr_x11_XFDI_drom_delete_local_view_data();
extern ddr_x11_XFDI_drom_delete_local_window_data();

DDt_DROM *dde_x11_XFDI_DROM_fcns = (DDt_DROM *)0;

DtPtr
ddr_x11_XFDI_return_DROM_fcns()
{
    DDt_DROM *fcns;

    if (dde_x11_XFDI_DROM_fcns == (DDt_DROM *)0) {
	fcns = DDdevice_CreateDROMStruct();

	if (fcns == (DDt_DROM *)0) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_x11_XFDI_return_DROM_fcns",
			DcNullPtr);
	}

	fcns->version_number 			= 1;

	/* version 1 fields */

	fcns->pre_initialization		= (DtPFI) ddr_x11_XFDI_drom_pre_initialization;               
	fcns->post_initialization		= (DtPFI) ddr_x11_XFDI_drom_post_initialization;              
	fcns->create_local_device_data		= (DtPFI) ddr_x11_XFDI_drom_create_local_device_data;         
	fcns->create_local_view_data		= (DtPFI) ddr_x11_XFDI_drom_create_local_view_data;           
	fcns->create_local_window_data		= (DtPFI) ddr_x11_XFDI_drom_create_local_window_data;         
	fcns->start_update			= (DtPFI) ddr_x11_XFDI_drom_start_update;                     
	fcns->update_local_data			= (DtPFI) ddr_x11_XFDI_drom_update_local_data;                
	fcns->update_studio	= (DtPFI) ddr_x11_XFDI_drom_update_studio;    
	fcns->update_display		= (DtPFI) ddr_x11_XFDI_drom_update_display;             
	fcns->camera			= (DtPFI) ddr_x11_XFDI_drom_camera;                      
	fcns->light			= (DtPFI) ddr_x11_XFDI_drom_light;                       
	fcns->set_camera_matrix			= (DtPFI) ddr_x11_XFDI_drom_set_camera_matrix;                
	fcns->set_parallel_matrix		= (DtPFI) ddr_x11_XFDI_drom_set_parallel_matrix;              
	fcns->set_perspective_matrix		= (DtPFI) ddr_x11_XFDI_drom_set_perspective_matrix;           
	fcns->set_projection_matrix		= (DtPFI) ddr_x11_XFDI_drom_set_projection_matrix;            
	fcns->push_att_clpvol			= (DtPFI) ddr_x11_XFDI_drom_push_att_clpvol;                  
	fcns->pop_att_clpvol			= (DtPFI) ddr_x11_XFDI_drom_pop_att_clpvol;                   
	fcns->set_att_clpvol			= (DtPFI) ddr_x11_XFDI_drom_set_att_clpvol;                   
	fcns->apply_att_clpvol			= (DtPFI) ddr_x11_XFDI_drom_apply_att_clpvol;                 
	fcns->get_wcstofcsmat			= (DtPFI) ddr_x11_XFDI_drom_get_wcstofcsmat;                  
	fcns->pop_lcstofcsmat			= (DtPFI) ddr_x11_XFDI_drom_pop_lcstofcsmat;                  
	fcns->push_lcstofcsmat			= (DtPFI) ddr_x11_XFDI_drom_push_lcstofcsmat;                 
	fcns->transform_clip_z_point		= (DtPFI) ddr_x11_XFDI_drom_transform_clip_z_point;           
	fcns->render_point_list			= (DtPFI) ddr_x11_XFDI_drom_render_point_list;                
	fcns->render_line_list			= (DtPFI) ddr_x11_XFDI_drom_render_line_list;                 
	fcns->render_connected_line_list	= (DtPFI) ddr_x11_XFDI_drom_render_connected_line_list;       
	fcns->render_triangle_list		= (DtPFI) ddr_x11_XFDI_drom_render_triangle_list;             
	fcns->render_triangle_mesh		= (DtPFI) ddr_x11_XFDI_drom_render_triangle_mesh;             
	fcns->set_att_ambint			= (DtPFI) ddr_x11_XFDI_drom_set_att_ambint;                   
	fcns->set_att_ambswi			= (DtPFI) ddr_x11_XFDI_drom_set_att_ambswi;                   
	fcns->set_att_bacfacculble		= (DtPFI) ddr_x11_XFDI_drom_set_att_bacfacculble;             
	fcns->set_att_bacfacculswi		= (DtPFI) ddr_x11_XFDI_drom_set_att_bacfacculswi;             
	fcns->set_att_clpswi			= (DtPFI) ddr_x11_XFDI_drom_set_att_clpswi;                   
	fcns->set_att_depcue			= (DtPFI) ddr_x11_XFDI_drom_set_att_depcue;                   
	fcns->set_att_depcueswi			= (DtPFI) ddr_x11_XFDI_drom_set_att_depcueswi;                
	fcns->set_att_difclr			= (DtPFI) ddr_x11_XFDI_drom_set_att_difclr;                   
	fcns->set_att_difint			= (DtPFI) ddr_x11_XFDI_drom_set_att_difint;                   
	fcns->set_att_difswi			= (DtPFI) ddr_x11_XFDI_drom_set_att_difswi;                   
	fcns->set_att_hidsrfswi			= (DtPFI) ddr_x11_XFDI_drom_set_att_hidsrfswi;                
	fcns->set_att_inttyp			= (DtPFI) ddr_x11_XFDI_drom_set_att_inttyp;                   
	fcns->set_att_lgtclr			= (DtPFI) ddr_x11_XFDI_drom_set_att_lgtclr;                   
	fcns->set_att_lgtint			= (DtPFI) ddr_x11_XFDI_drom_set_att_lgtint;                   
	fcns->set_att_lgttyp			= (DtPFI) ddr_x11_XFDI_drom_set_att_lgttyp;                   
	fcns->set_att_lintyp			= (DtPFI) ddr_x11_XFDI_drom_set_att_lintyp;                   
	fcns->set_att_linwid			= (DtPFI) ddr_x11_XFDI_drom_set_att_linwid;                   
	fcns->set_att_refswi			= (DtPFI) ddr_x11_XFDI_drom_set_att_refswi;                   
	fcns->set_att_reptyp			= (DtPFI) ddr_x11_XFDI_drom_set_att_reptyp;                   
	fcns->set_att_shaswi			= (DtPFI) ddr_x11_XFDI_drom_set_att_shaswi;                   
	fcns->set_att_shdidx			= (DtPFI) ddr_x11_XFDI_drom_set_att_shdidx;                   
	fcns->set_att_spcclr			= (DtPFI) ddr_x11_XFDI_drom_set_att_spcclr;                   
	fcns->set_att_spcfct			= (DtPFI) ddr_x11_XFDI_drom_set_att_spcfct;                   
	fcns->set_att_spcint			= (DtPFI) ddr_x11_XFDI_drom_set_att_spcint;                   
	fcns->set_att_spcswi			= (DtPFI) ddr_x11_XFDI_drom_set_att_spcswi;                   
	fcns->set_att_srfshd			= (DtPFI) ddr_x11_XFDI_drom_set_att_srfshd;                   
	fcns->set_att_stereo			= (DtPFI) ddr_x11_XFDI_drom_set_att_stereo;                   
	fcns->set_att_stereoswi			= (DtPFI) ddr_x11_XFDI_drom_set_att_stereoswi;                
	fcns->set_att_transpclr			= (DtPFI) ddr_x11_XFDI_drom_set_att_transpclr;                
	fcns->set_att_transpint			= (DtPFI) ddr_x11_XFDI_drom_set_att_transpint;                
	fcns->set_att_transpswi			= (DtPFI) ddr_x11_XFDI_drom_set_att_transpswi;                
	fcns->update_lcstowcsmat_lokatfrm	= (DtPFI) ddr_x11_XFDI_drom_update_lcstowcsmat_lokatfrm;      
	fcns->update_lcstowcsmat_pop		= (DtPFI) ddr_x11_XFDI_drom_update_lcstowcsmat_pop;           
	fcns->update_lcstowcsmat_push		= (DtPFI) ddr_x11_XFDI_drom_update_lcstowcsmat_push;          
	fcns->update_lcstowcsmat_rotate		= (DtPFI) ddr_x11_XFDI_drom_update_lcstowcsmat_rotate;        
	fcns->update_lcstowcsmat_scale		= (DtPFI) ddr_x11_XFDI_drom_update_lcstowcsmat_scale;         
	fcns->update_lcstowcsmat_shear		= (DtPFI) ddr_x11_XFDI_drom_update_lcstowcsmat_shear;         
	fcns->update_lcstowcsmat_tfmmat		= (DtPFI) ddr_x11_XFDI_drom_update_lcstowcsmat_tfmmat;        
	fcns->update_lcstowcsmat_transl		= (DtPFI) ddr_x11_XFDI_drom_update_lcstowcsmat_transl;        
	fcns->get_lcstowcsmat			= (DtPFI) ddr_x11_XFDI_drom_get_lcstowcsmat;
	fcns->set_att_lgtswi 			= (DtPFI) ddr_x11_XFDI_drom_set_att_lgtswi;
	fcns->set_att_lgtspdexp 		= (DtPFI) ddr_x11_XFDI_drom_set_att_lgtspdexp;
	fcns->set_att_lgtspdang 		= (DtPFI) ddr_x11_XFDI_drom_set_att_lgtspdang;
	fcns->set_att_lgtatn 			= (DtPFI) ddr_x11_XFDI_drom_set_att_lgtatn;
	fcns->set_att_mapbmpswi 		= (DtPFI) ddr_x11_XFDI_drom_set_att_mapbmpswi;
	fcns->set_att_mapbmp 			= (DtPFI) ddr_x11_XFDI_drom_set_att_mapbmp;
	fcns->set_att_mapdifclrswi 		= (DtPFI) ddr_x11_XFDI_drom_set_att_mapdifclrswi;
	fcns->set_att_mapdifclr 		= (DtPFI) ddr_x11_XFDI_drom_set_att_mapdifclr;
	fcns->set_att_mapenvswi 		= (DtPFI) ddr_x11_XFDI_drom_set_att_mapenvswi;
	fcns->set_att_mapenv 			= (DtPFI) ddr_x11_XFDI_drom_set_att_mapenv;
	fcns->set_att_maptrnintswi 		= (DtPFI) ddr_x11_XFDI_drom_set_att_maptrnintswi;
	fcns->set_att_maptrnint 		= (DtPFI) ddr_x11_XFDI_drom_set_att_maptrnint;
	fcns->get_lcstowcsmat 			= (DtPFI) ddr_x11_XFDI_drom_get_lcstowcsmat;
	fcns->delete_local_device_data 		= (DtPFI) ddr_x11_XFDI_drom_delete_local_device_data;
	fcns->delete_local_view_data 		= (DtPFI) ddr_x11_XFDI_drom_delete_local_view_data;
	fcns->delete_local_window_data 		= (DtPFI) ddr_x11_XFDI_drom_delete_local_window_data;
	fcns->use_separate_wireframe		= DcTrue;

	dde_x11_XFDI_DROM_fcns = fcns;
    }

    return (DtPtr) dde_x11_XFDI_DROM_fcns;
}
/*
 ======================================================================
 */

extern void ddr_x11_XFDI_drom_trimsh_render();     
extern void ddr_x11_XFDI_drom_trilst_render();     
extern void ddr_x11_XFDI_drom_pntlst_render();     
extern void ddr_x11_XFDI_drom_linlst_render();     
extern void ddr_x11_XFDI_drom_plylin_render();     
extern void ddr_x11_XFDI_drom_tristrip_render();   
extern void ddr_x11_XFDI_drom_prisrf_render();     
extern void ddr_x11_XFDI_drom_vartristrip_render();
extern void ddr_x11_XFDI_drom_vartrimsh_render();  

DDt_DROMMethod DROM_object_method[] = {
   "DoTriangleMesh",	(DtPFI)ddr_x11_XFDI_drom_trimsh_render,     
   "DoTriangleList",	(DtPFI)ddr_x11_XFDI_drom_trilst_render,     
   "DoPointList",	(DtPFI)ddr_x11_XFDI_drom_pntlst_render,     
   "DoLineList",	(DtPFI)ddr_x11_XFDI_drom_linlst_render,     
   "DoPolyline",	(DtPFI)ddr_x11_XFDI_drom_plylin_render,     
   "DoTriangleStrip",	(DtPFI)ddr_x11_XFDI_drom_tristrip_render,   
   "DoPrimSurf",	(DtPFI)ddr_x11_XFDI_drom_prisrf_render,     
   "DoVarTriangleStrip",(DtPFI)ddr_x11_XFDI_drom_vartristrip_render,
   "DoVarTriangleMesh", (DtPFI)ddr_x11_XFDI_drom_vartrimsh_render,  
   0,			0,
};

DtPtr
ddr_x11_XFDI_return_DROM_methods()
{
    return (DtPtr)DROM_object_method;
}
