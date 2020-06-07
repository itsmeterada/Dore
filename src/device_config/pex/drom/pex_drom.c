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
 
/*****************************************************************************
**  This file contains the startup function for the PEX dynamic renderer
**  output module.
*****************************************************************************/

#include "../pex_driver.h"


    /************************/
    /*** Global Variables ***/
    /************************/

DtMatrix4x4 Identity4x4 = IDENTITY_4x4;

DDt_DROM            *dde_pex_DROM_fcns        = 0;
DtFlag               dde_pex_drom_pre_init    = DcTrue;
ddt_pex_drom_data    dde_pex_drom_data;

ddt_pex_drom_device *dde_pex_drom_device_curr = 0;
ddt_pex_drom_view   *dde_pex_drom_view_curr   = 0;
ddt_pex_drom_window *dde_pex_drom_window_curr = 0;



/*****************************************************************************
**  This routine sets up the DROM function pointers in the Dore' device-
**  driver's DROM structure.
*****************************************************************************/

DtPtr ddr_pex_return_DROM_fcns (void)
{
    auto DDt_DROM *fcns;	/* DROM Structure Pointer */

#   if DEBUG_CALL
	print ("ddr_pex_return_DROM_fcns ()\n");
	fflush (stdout);
#   endif

    if (!dde_pex_DROM_fcns)
    {
	fcns = DDdevice_CreateDROMStruct ();

	if (!fcns)
	    DDerror (ERR_CANT_ALLOC_MEM, "dde_pex_return_DROM_fcns", DcNullPtr);

	fcns->version_number = 2;

	/* version 1 fields */

#       ifdef __STDC__
#           define SetFuncPtr(name)  fcns->name = ddr_pex_drom_##name
#       else
#           define SetFuncPtr(name)  fcns->name = ddr_pex_drom_/**/name
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
	SetFuncPtr (set_att_reptyp);
	SetFuncPtr (set_att_shdidx);
	SetFuncPtr (set_att_spcclr);
	SetFuncPtr (set_att_spcfct);
	SetFuncPtr (set_att_spcint);
	SetFuncPtr (set_att_spcswi);
	SetFuncPtr (set_att_srfshd);
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
	SetFuncPtr (get_lcstowcsmat);
	SetFuncPtr (delete_local_device_data);
	SetFuncPtr (delete_local_view_data);
	SetFuncPtr (delete_local_window_data);

	fcns->use_separate_wireframe = DcTrue;

	/* version 2 fields */

	SetFuncPtr (set_att_srfedgclr);

	dde_pex_DROM_fcns = fcns;
    }

    return (DtPtr) dde_pex_DROM_fcns;
}
