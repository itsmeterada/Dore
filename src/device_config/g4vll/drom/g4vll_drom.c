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
	void ddr_g4vll_drom_new_class (class_name, class_id)
	DtPtr ddr_g4vll_return_DROM_fcns ()
	DtPtr ddr_g4vll_return_DROM_methods ()

  ======================================================================
 */
#include "drom.h"

/*
 ======================================================================
 */

ddt_g4vll_drom_quadlst     *dde_g4vll_drom_quadlst;
ddt_g4vll_drom_quadgrid    *dde_g4vll_drom_quadgrid;
ddt_g4vll_drom_trilst      *dde_g4vll_drom_trilst;
ddt_g4vll_drom_trimsh      *dde_g4vll_drom_trimsh;
ddt_g4vll_drom_tristrip    *dde_g4vll_drom_tristrip;
ddt_g4vll_drom_vartrilist  *dde_g4vll_drom_vartrilist;
ddt_g4vll_drom_vartrimsh   *dde_g4vll_drom_vartrimsh;
ddt_g4vll_drom_vartristrip *dde_g4vll_drom_vartristrip;

ddt_g4vll_drom_data dde_g4vll_drom_data;

ddt_g4vll_drom_device *dde_g4vll_drom_curdevdat = 0;
ddt_g4vll_drom_view   *dde_g4vll_drom_curviwdat = 0;
ddt_g4vll_drom_window *dde_g4vll_drom_curwindat = 0;
ddt_g4vll_drom_camera *dde_g4vll_drom_curcamdat = 0;

static DDt_DROM *dde_g4vll_DROM_fcns = (DDt_DROM *)0;

/*
 * The classes in this array will have their standard method routines
 * replaced at execution time by the method routines specified here.
 */
static DDt_DROMMethod dde_g4vll_DROM_methods[] = {
   {"DoPrimSurf",	       (DtMethodPtr) ddr_g4vll_drom_prisrf_render },
   {"DoQuadList",	       (DtMethodPtr) ddr_g4vll_drom_quadlst_render },
   {"DoQuadGrid",	       (DtMethodPtr) ddr_g4vll_drom_quadgrid_render },
   {"DoQuadMesh",              (DtMethodPtr) ddr_g4vll_drom_quadmsh_render },
   {"DoSphereList",	       (DtMethodPtr) ddr_g4vll_drom_sphlst_render },
   {"DoTriangleList",	       (DtMethodPtr) ddr_g4vll_drom_trilst_render },
   {"DoTriangleMesh",	       (DtMethodPtr) ddr_g4vll_drom_trimsh_render },
   {"DoTriangleStrip",	       (DtMethodPtr) ddr_g4vll_drom_tristrip_render },
   {"DoVarTriangleMesh",       (DtMethodPtr) ddr_g4vll_drom_vartrimsh_render },
   {"DoVarTriangleList",       (DtMethodPtr) ddr_g4vll_drom_vartrilist_render },
   {"DoVarTriangleStrip",      (DtMethodPtr) ddr_g4vll_drom_vartristrip_render},
   {"DoBoundingVol",	       (DtMethodPtr) ddr_g4vll_drom_bndvol_execute },
   {0,0}
};

/* 
 * The classes in this array require additional data space in their 
 * private data for g4VLL-specific requirements.
 */
ddt_g4vll_drom_class_data dde_g4vll_drom_class_data[] = {
    { "DoFileRaster", -1, 
	      (DtMethodPtr) ddr_g4vll_drom_filerstr_create, 
	      (DtMethodPtr) ddr_g4vll_drom_filerstr_print, 
	      (DtMethodPtr) ddr_g4vll_drom_filerstr_delete, -1},
    { "DoRaster", -1, 
	      (DtMethodPtr) ddr_g4vll_drom_rstr_create, 
	      (DtMethodPtr) ddr_g4vll_drom_rstr_print, 
	      (DtMethodPtr) ddr_g4vll_drom_rstr_delete, -1},
    { "DoTriangleList", -1,
	      (DtMethodPtr) ddr_g4vll_drom_trilst_create,
	      (DtMethodPtr) ddr_g4vll_drom_trilst_print,
	      (DtMethodPtr) ddr_g4vll_drom_trilst_delete, -1},
    { "DoTriangleMesh", -1,
	      (DtMethodPtr) ddr_g4vll_drom_trimsh_create,
	      (DtMethodPtr) ddr_g4vll_drom_trimsh_print,
	      (DtMethodPtr) ddr_g4vll_drom_trimsh_delete, -1},
    { "DoTriangleStrip", -1,
	      (DtMethodPtr) ddr_g4vll_drom_tristrip_create,
	      (DtMethodPtr) ddr_g4vll_drom_tristrip_print,
	      (DtMethodPtr) ddr_g4vll_drom_tristrip_delete, -1},
    { "DoVarTriangleList", -1,
	      (DtMethodPtr) ddr_g4vll_drom_vartrilist_create,
	      (DtMethodPtr) ddr_g4vll_drom_vartrilist_print,
	      (DtMethodPtr) ddr_g4vll_drom_vartrilist_delete, -1},
    { "DoVarTriangleMesh", -1,
	      (DtMethodPtr) ddr_g4vll_drom_vartrimsh_create,
	      (DtMethodPtr) ddr_g4vll_drom_vartrimsh_print,
	      (DtMethodPtr) ddr_g4vll_drom_vartrimsh_delete, -1},
    { "DoVarTriangleStrip", -1,
	      (DtMethodPtr) ddr_g4vll_drom_vartristrip_create,
	      (DtMethodPtr) ddr_g4vll_drom_vartristrip_print,
	      (DtMethodPtr) ddr_g4vll_drom_vartristrip_delete, -1},
    { "DoQuadList", -1,
	      (DtMethodPtr) ddr_g4vll_drom_quadlst_create,
	      (DtMethodPtr) ddr_g4vll_drom_quadlst_print,
	      (DtMethodPtr) ddr_g4vll_drom_quadlst_delete, -1},
    { "DoQuadGrid", -1,
	      (DtMethodPtr) ddr_g4vll_drom_quadgrid_create,
	      (DtMethodPtr) ddr_g4vll_drom_quadgrid_print,
	      (DtMethodPtr) ddr_g4vll_drom_quadgrid_delete, -1},
    { "DoQuadMesh", -1,
	      (DtMethodPtr) ddr_g4vll_drom_quadmsh_create,
	      (DtMethodPtr) ddr_g4vll_drom_quadmsh_print,
	      (DtMethodPtr) ddr_g4vll_drom_quadmsh_delete, -1},
    { "DoSphereList", -1,
	      (DtMethodPtr) ddr_g4vll_drom_sphlst_create,
	      (DtMethodPtr) ddr_g4vll_drom_sphlst_print,
	      (DtMethodPtr) ddr_g4vll_drom_sphlst_delete, -1},
    { "DoPrimSurf", -1,
	      (DtMethodPtr) ddr_g4vll_drom_prisrf_create,
	      (DtMethodPtr) ddr_g4vll_drom_prisrf_print,
	      (DtMethodPtr) ddr_g4vll_drom_prisrf_delete, -1},

    { 0, -1,  0,0,0, -1 }
};
/*
 ======================================================================
 */

/*
 * This routine is installed by DDclass_AddNotify at driver installation time
 * so that it is called whenever a class is initialised.  It adds a data
 * slot for additional object data to those classes contained in the 
 * dde_g4vll_drom_class_data structure.
 */
void ddr_g4vll_drom_new_class (
    DtPtr class_name,
    DtInt class_id)
{
    DtInt i;

    for (i=0; dde_g4vll_drom_class_data[i].name != 0; i++) {
	if (!strcmp(dde_g4vll_drom_class_data[i].name, class_name)) {
	    dde_g4vll_drom_class_data[i].id = class_id;
	    dde_g4vll_drom_class_data[i].index =
		DDclass_AddObjectData
		(   class_id, 
		    dde_g4vll_drom_class_data[i].crt_rtn,
		    dde_g4vll_drom_class_data[i].prt_rtn,
		    dde_g4vll_drom_class_data[i].del_rtn
		);

	}
    }
}



/* ====================================================================== */

DtPtr ddr_g4vll_return_DROM_fcns (void)
{
    auto DDt_DROM *fcns;

    if (!dde_g4vll_DROM_fcns) {
	fcns = DDdevice_CreateDROMStruct();

	if (!fcns) {
	    DDerror (ERR_CANT_ALLOC_MEM,"dde_g4vll_return_DROM_fcns",DcNullPtr);
	}

	fcns->version_number = 2;

	/* Version 1 Fields */

#       ifdef __STDC__
#           define SetFuncPtr(name)  fcns->name = ddr_g4vll_drom_##name
#       else
#           define SetFuncPtr(name)  fcns->name = ddr_g4vll_drom_/**/name
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
	SetFuncPtr (get_lcstowcsmat);
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

	/* version 2 fields */

	SetFuncPtr (set_att_srfedgclr);
	SetFuncPtr (set_att_localaaswi);
	SetFuncPtr (set_att_localaasty);

	SetFuncPtr (set_att_transporientclr);
	SetFuncPtr (set_att_transporientexp);
	SetFuncPtr (set_att_transporientint);
	SetFuncPtr (set_att_transporientswi);

	SetFuncPtr (set_att_smpadapt);
	SetFuncPtr (set_att_smpadaptswi);
	SetFuncPtr (set_att_smpfilter);
	SetFuncPtr (set_att_smpjitter);
	SetFuncPtr (set_att_smpjitterswi);
	SetFuncPtr (set_att_smpsuper);
	SetFuncPtr (set_att_smpsuperswi);

	fcns->use_separate_wireframe = DcFalse;

	dde_g4vll_DROM_fcns = fcns;
    }

    return (DtPtr) dde_g4vll_DROM_fcns;
}



/* ====================================================================== */

DtPtr ddr_g4vll_return_DROM_methods (void)
{
    return (DtPtr) dde_g4vll_DROM_methods;
}
