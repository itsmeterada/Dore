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
	void ddr_softpipe_return_functions (rtn_fcns)

  ======================================================================
 */
#include "drom.h"

struct dynout_data		dde_softpipe_drom_data;
struct dynout_device_data	*dde_softpipe_drom_curdevdat;
struct dynout_view_data		*dde_softpipe_drom_curviwdat;
struct dynout_window_data	*dde_softpipe_drom_curwindat;
struct dynout_light_data	*dde_softpipe_drom_curlgtdat;
struct dynout_camera_data	*dde_softpipe_drom_curcamdat;

int dde_softpipe_drom_pre_init;

static DDt_DROM  *dde_softpipe_DROM_fcns = 0;

/*
 ======================================================================
 */
void ddr_softpipe_return_functions (
    DtPtr *rtn_fcns)
{
    DDt_DROM *fcns;

#ifdef DEBUG
    printf("softpipe:... processing return functions ...\n");
    fflush(stdout);
#endif

    if (!dde_softpipe_DROM_fcns) {
	fcns = DDdevice_CreateDROMStruct();

	if (!fcns) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_softpipe_return_DROM_fcns",
			DcNullPtr);
	}

	fcns->version_number = 1;

	/* version 1 fields */

# ifndef WIN32
#       ifdef __STDC__
#           define SetFuncPtr(name)  fcns->name = ddr_softpipe_drom_##name
#       else
#           define SetFuncPtr(name)  fcns->name = ddr_softpipe_drom_/**/name
#       endif

	SetFuncPtr (pre_initialization);
	SetFuncPtr (post_initialization);
	SetFuncPtr (create_local_device_data);
	SetFuncPtr (create_local_view_data);
	SetFuncPtr (create_local_window_data);
	SetFuncPtr (start_update);
	SetFuncPtr (update_local_data);
	SetFuncPtr (update_studio);
	SetFuncPtr (update_display);
	SetFuncPtr (camera);
	SetFuncPtr (light);
	SetFuncPtr (set_camera_matrix);
	SetFuncPtr (set_parallel_matrix);
	SetFuncPtr (set_perspective_matrix);
	SetFuncPtr (set_projection_matrix);
	SetFuncPtr (push_att_clpvol);
	SetFuncPtr (pop_att_clpvol);
	SetFuncPtr (set_att_clpvol);
	SetFuncPtr (apply_att_clpvol);
	SetFuncPtr (get_wcstofcsmat);
	SetFuncPtr (pop_lcstofcsmat);
	SetFuncPtr (push_lcstofcsmat);
	SetFuncPtr (transform_clip_z_point);
	SetFuncPtr (render_point_list);
	SetFuncPtr (render_line_list);
	SetFuncPtr (render_connected_line_list);
	SetFuncPtr (render_triangle_list);
	SetFuncPtr (render_triangle_mesh);
	SetFuncPtr (set_att_ambint);
	SetFuncPtr (set_att_ambswi);
	SetFuncPtr (set_att_bacfacculble);
	SetFuncPtr (set_att_bacfacculswi);
	SetFuncPtr (set_att_clpswi);
	SetFuncPtr (set_att_depcue);
	SetFuncPtr (set_att_depcueswi);
	SetFuncPtr (set_att_difclr);
	SetFuncPtr (set_att_difint);
	SetFuncPtr (set_att_difswi);
	SetFuncPtr (set_att_hidsrfswi);
	SetFuncPtr (set_att_inttyp);
	SetFuncPtr (set_att_lgtclr);
	SetFuncPtr (set_att_lgtint);
	SetFuncPtr (set_att_lgttyp);
	SetFuncPtr (set_att_lintyp);
	SetFuncPtr (set_att_linwid);
	SetFuncPtr (set_att_refswi);
	SetFuncPtr (set_att_reptyp);
	SetFuncPtr (set_att_shaswi);
	SetFuncPtr (set_att_shdidx);
	SetFuncPtr (set_att_spcclr);
	SetFuncPtr (set_att_spcfct);
	SetFuncPtr (set_att_spcint);
	SetFuncPtr (set_att_spcswi);
	SetFuncPtr (set_att_srfshd);
	SetFuncPtr (set_att_stereo);
	SetFuncPtr (set_att_stereoswi);
	SetFuncPtr (set_att_transpclr);
	SetFuncPtr (set_att_transpint);
	SetFuncPtr (set_att_transpswi);
	SetFuncPtr (update_lcstowcsmat_lokatfrm);
	SetFuncPtr (update_lcstowcsmat_pop);
	SetFuncPtr (update_lcstowcsmat_push);
	SetFuncPtr (update_lcstowcsmat_rotate);
	SetFuncPtr (update_lcstowcsmat_scale);
	SetFuncPtr (update_lcstowcsmat_shear);
	SetFuncPtr (update_lcstowcsmat_tfmmat);
	SetFuncPtr (update_lcstowcsmat_transl);
	SetFuncPtr (set_att_lgtswi);
	SetFuncPtr (set_att_lgtspdexp);
	SetFuncPtr (set_att_lgtspdang);
	SetFuncPtr (set_att_lgtatn);
	SetFuncPtr (set_att_mapbmpswi);
	SetFuncPtr (set_att_mapbmp);
	SetFuncPtr (set_att_mapdifclrswi);
	SetFuncPtr (set_att_mapdifclr);
	SetFuncPtr (set_att_mapenvswi);
	SetFuncPtr (set_att_mapenv);
	SetFuncPtr (set_att_maptrnintswi);
	SetFuncPtr (set_att_maptrnint);
	SetFuncPtr (get_lcstowcsmat);
	SetFuncPtr (delete_local_device_data);
	SetFuncPtr (delete_local_view_data);
	SetFuncPtr (delete_local_window_data);

# else

        fcns->pre_initialization = ddr_softpipe_drom_pre_initialization;
        fcns->post_initialization = ddr_softpipe_drom_post_initialization;
        fcns->create_local_device_data = 
                      ddr_softpipe_drom_create_local_device_data;
        fcns->create_local_view_data = ddr_softpipe_drom_create_local_view_data;
        fcns->create_local_window_data = 
                      ddr_softpipe_drom_create_local_window_data;
        fcns->start_update = ddr_softpipe_drom_start_update;
        fcns->update_local_data = ddr_softpipe_drom_update_local_data;
        fcns->update_studio = ddr_softpipe_drom_update_studio;
        fcns->update_display = ddr_softpipe_drom_update_display;
        fcns->camera = ddr_softpipe_drom_camera;
        fcns->light = ddr_softpipe_drom_light;
        fcns->set_camera_matrix = ddr_softpipe_drom_set_camera_matrix;
        fcns->set_parallel_matrix = ddr_softpipe_drom_set_parallel_matrix;
        fcns->set_perspective_matrix = ddr_softpipe_drom_set_perspective_matrix;
        fcns->set_projection_matrix = ddr_softpipe_drom_set_projection_matrix;
        fcns->push_att_clpvol = ddr_softpipe_drom_push_att_clpvol;
        fcns->pop_att_clpvol = ddr_softpipe_drom_pop_att_clpvol;
        fcns->set_att_clpvol = ddr_softpipe_drom_set_att_clpvol;
        fcns->apply_att_clpvol = ddr_softpipe_drom_apply_att_clpvol;
        fcns->get_wcstofcsmat = ddr_softpipe_drom_get_wcstofcsmat;
        fcns->pop_lcstofcsmat = ddr_softpipe_drom_pop_lcstofcsmat;
        fcns->push_lcstofcsmat = ddr_softpipe_drom_push_lcstofcsmat;
        fcns->transform_clip_z_point = ddr_softpipe_drom_transform_clip_z_point;
        fcns->render_point_list = ddr_softpipe_drom_render_point_list;
        fcns->render_line_list = ddr_softpipe_drom_render_line_list;
        fcns->render_connected_line_list = 
                         ddr_softpipe_drom_render_connected_line_list;
        fcns->render_triangle_list = ddr_softpipe_drom_render_triangle_list;
        fcns->render_triangle_mesh = ddr_softpipe_drom_render_triangle_mesh;
        fcns->set_att_ambint = ddr_softpipe_drom_set_att_ambint;
        fcns->set_att_ambswi = ddr_softpipe_drom_set_att_ambswi;
        fcns->set_att_bacfacculble = ddr_softpipe_drom_set_att_bacfacculble;
        fcns->set_att_bacfacculswi = ddr_softpipe_drom_set_att_bacfacculswi;
        fcns->set_att_clpswi = ddr_softpipe_drom_set_att_clpswi;
        fcns->set_att_depcue = ddr_softpipe_drom_set_att_depcue;
        fcns->set_att_depcueswi = ddr_softpipe_drom_set_att_depcueswi;
        fcns->set_att_difclr = ddr_softpipe_drom_set_att_difclr;
        fcns->set_att_difint = ddr_softpipe_drom_set_att_difint;
        fcns->set_att_difswi = ddr_softpipe_drom_set_att_difswi;
        fcns->set_att_hidsrfswi = ddr_softpipe_drom_set_att_hidsrfswi;
        fcns->set_att_inttyp = ddr_softpipe_drom_set_att_inttyp;
        fcns->set_att_lgtclr = ddr_softpipe_drom_set_att_lgtclr;
        fcns->set_att_lgtint = ddr_softpipe_drom_set_att_lgtint;
        fcns->set_att_lgttyp = ddr_softpipe_drom_set_att_lgttyp;
        fcns->set_att_lintyp = ddr_softpipe_drom_set_att_lintyp;
        fcns->set_att_linwid = ddr_softpipe_drom_set_att_linwid;
        fcns->set_att_refswi = ddr_softpipe_drom_set_att_refswi;
        fcns->set_att_reptyp = ddr_softpipe_drom_set_att_reptyp;
        fcns->set_att_shaswi = ddr_softpipe_drom_set_att_shaswi;
        fcns->set_att_shdidx = ddr_softpipe_drom_set_att_shdidx;
        fcns->set_att_spcclr = ddr_softpipe_drom_set_att_spcclr;
        fcns->set_att_spcfct = ddr_softpipe_drom_set_att_spcfct;
        fcns->set_att_spcint = ddr_softpipe_drom_set_att_spcint;
        fcns->set_att_spcswi = ddr_softpipe_drom_set_att_spcswi;
        fcns->set_att_srfshd = ddr_softpipe_drom_set_att_srfshd;
        fcns->set_att_stereo = ddr_softpipe_drom_set_att_stereo;
        fcns->set_att_stereoswi = ddr_softpipe_drom_set_att_stereoswi;
        fcns->set_att_transpclr = ddr_softpipe_drom_set_att_transpclr;
        fcns->set_att_transpint = ddr_softpipe_drom_set_att_transpint;
        fcns->set_att_transpswi = ddr_softpipe_drom_set_att_transpswi;
        fcns->update_lcstowcsmat_lokatfrm = 
                          ddr_softpipe_drom_update_lcstowcsmat_lokatfrm;
        fcns->update_lcstowcsmat_pop = ddr_softpipe_drom_update_lcstowcsmat_pop;
        fcns->update_lcstowcsmat_push = 
                          ddr_softpipe_drom_update_lcstowcsmat_push;
        fcns->update_lcstowcsmat_rotate = 
                          ddr_softpipe_drom_update_lcstowcsmat_rotate;
        fcns->update_lcstowcsmat_scale = 
                          ddr_softpipe_drom_update_lcstowcsmat_scale;
        fcns->update_lcstowcsmat_shear = 
                          ddr_softpipe_drom_update_lcstowcsmat_shear;
        fcns->update_lcstowcsmat_tfmmat = 
                          ddr_softpipe_drom_update_lcstowcsmat_tfmmat;
        fcns->update_lcstowcsmat_transl = 
                          ddr_softpipe_drom_update_lcstowcsmat_transl;
        fcns->set_att_lgtswi = ddr_softpipe_drom_set_att_lgtswi;
        fcns->set_att_lgtspdexp = ddr_softpipe_drom_set_att_lgtspdexp;
        fcns->set_att_lgtspdang = ddr_softpipe_drom_set_att_lgtspdang;
        fcns->set_att_lgtatn = ddr_softpipe_drom_set_att_lgtatn;
        fcns->set_att_mapbmpswi = ddr_softpipe_drom_set_att_mapbmpswi;
        fcns->set_att_mapbmp = ddr_softpipe_drom_set_att_mapbmp;
        fcns->set_att_mapdifclrswi = ddr_softpipe_drom_set_att_mapdifclrswi;
        fcns->set_att_mapdifclr = ddr_softpipe_drom_set_att_mapdifclr;
        fcns->set_att_mapenvswi = ddr_softpipe_drom_set_att_mapenvswi;
        fcns->set_att_mapenv = ddr_softpipe_drom_set_att_mapenv;
        fcns->set_att_maptrnintswi = ddr_softpipe_drom_set_att_maptrnintswi;
        fcns->set_att_maptrnint = ddr_softpipe_drom_set_att_maptrnint;
        fcns->get_lcstowcsmat = ddr_softpipe_drom_get_lcstowcsmat;
        fcns->delete_local_device_data = 
                          ddr_softpipe_drom_delete_local_device_data;
        fcns->delete_local_view_data = ddr_softpipe_drom_delete_local_view_data;
        fcns->delete_local_window_data = 
                          ddr_softpipe_drom_delete_local_window_data;

# endif

	fcns->use_separate_wireframe = DcFalse;

	dde_softpipe_DROM_fcns = fcns;
    }

    *rtn_fcns = (DtPtr) dde_softpipe_DROM_fcns;
}

/* ====================================================================== */

void ddr_softpipe_drom_set_output_functions (
    void (*point_fcn)(),
    void (*line_fcn)(),
    void (*triangle_fcn)())
{
   dde_softpipe_drom_data.draw_point_dc    = point_fcn;
   dde_softpipe_drom_data.draw_line_dc     = line_fcn;
   dde_softpipe_drom_data.draw_triangle_dc = triangle_fcn;
}
