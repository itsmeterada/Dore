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
 
/* ======================================================================
  Functions:
	ddr_gl_drom_new_class
	ddr_gl_return_DROM_fcns
	ddr_gl_return_DROM_methods

** ====================================================================== */

#include "../gl_driver.h"
#include <dore/dore_develop/error.h>


    /**************************/
    /***  Global Variables  ***/
    /**************************/

DDt_DROM *dde_gl_DROM_fcns = (DDt_DROM *)0;


    /*************************/
    /***  Data Structures  ***/
    /*************************/

struct device_status DevStatus;

ddt_gl_drom_class_data dde_gl_drom_class_data[] =
{
    {   "DoFileRaster", -1, 
	(DtMethodPtr) ddr_gl_drom_filerstr_create, 
	(DtMethodPtr) ddr_gl_drom_filerstr_print, 
	(DtMethodPtr) ddr_gl_drom_filerstr_delete, -1 },
    {   "DoRaster", -1, 
	(DtMethodPtr) ddr_gl_drom_rstr_create, 
	(DtMethodPtr) ddr_gl_drom_rstr_print, 
	(DtMethodPtr) ddr_gl_drom_rstr_delete, -1 },
    {   "DoPrimSurf", -1,
	(DtMethodPtr) ddr_gl_drom_primsurf_create, 
	(DtMethodPtr) ddr_gl_drom_primsurf_print, 
	(DtMethodPtr) ddr_gl_drom_primsurf_delete, -1 },
    {   "DoSphereList", -1,
	(DtMethodPtr) ddr_gl_drom_spherelist_create, 
	(DtMethodPtr) ddr_gl_drom_spherelist_print, 
	(DtMethodPtr) ddr_gl_drom_spherelist_delete, -1 },
    { 0, -1, 0,0,0, -1}
};

	/* The renderer's class methods for some base primitives are
	** overridden here (e.g. to use the GL sphere library).  */

static DDt_DROMMethod dde_gl_DROM_methods[] =
{
    { "DoPrimSurf",   (DtMethodPtr) ddr_gl_drom_primsurf_render   },
    { "DoSphereList", (DtMethodPtr) ddr_gl_drom_spherelist_render },
    { 0,              (DtMethodPtr) 0                             }
};



/* ====================================================================== */

void ddr_gl_drom_new_class (
    DtPtr class_name,
    DtInt class_id)
{
    register DtInt ii;		/* Class Data Index */

    for (ii=0;  dde_gl_drom_class_data[ii].name;  ++ii)
    {
	if (0 != strcmp (dde_gl_drom_class_data[ii].name, class_name))
	    continue;

	dde_gl_drom_class_data[ii].id = class_id;
	dde_gl_drom_class_data[ii].index =
	    DDclass_AddObjectData
	    (   class_id, 
		dde_gl_drom_class_data[ii].crt_rtn,
		dde_gl_drom_class_data[ii].prt_rtn,
		dde_gl_drom_class_data[ii].del_rtn
	    );
    }
}



/* ====================================================================== */

DtPtr ddr_gl_return_DROM_fcns (void)
{
    DDt_DROM *fcns;

    if (!dde_gl_DROM_fcns) {
	fcns = DDdevice_CreateDROMStruct ();

	if (!fcns) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_gl_return_DROM_fcns", DcNullPtr);
	}

	fcns->version_number = 2;

#       ifdef __STDC__
#           define SetFuncPtr(name)  fcns->name = ddr_gl_drom_##name
#       else
#           define SetFuncPtr(name)  fcns->name = ddr_gl_drom_/**/name
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

	SetFuncPtr (set_att_srfedgclr);
	SetFuncPtr (set_att_localaaswi);
	SetFuncPtr (set_att_localaasty);

	fcns->use_separate_wireframe = DcTrue;

	dde_gl_DROM_fcns = fcns;
    }

    return (DtPtr) dde_gl_DROM_fcns;
}



/*****************************************************************************
**  This routine returns a pointer to the named DROM method routines
**  structure.
*****************************************************************************/

DtPtr ddr_gl_return_DROM_methods (void)
{
    return (DtPtr) dde_gl_DROM_methods;
}
