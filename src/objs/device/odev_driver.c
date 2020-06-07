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
	dor_device_install_driver
	null_ok_proc
	null_DCM_proc
	null_DROM_proc
	null_PROM_proc
	dor_device_create_DCM_struct
	dor_device_create_PROM_struct
	dor_device_create_DROM_struct

  ====================================================================== */

#include <string.h>
#include <dore/internal/dogen.h>
#include <dore/dore_develop/develop.h>
#include <dore/dore_develop/private/device.h>



/* ====================================================================== */

void dor_device_install_driver (
    char  *name,		/* Name of The Device Driver */
    char  *description,		/* Description String */
    DtPFV  get_fcns_fcn)	/* Routine to Fill in Device Func Pointers */
{
    auto dot_device_driver *driver;
    auto DDt_DCM           *fcns;
   
    if ((doe_device_driver_count + 1) > doe_device_max_drivers) {
	if (doe_device_max_drivers == 0) {
	    driver = dor_space_allocate 
		     ((doe_device_max_drivers + 5) * sizeof(dot_device_driver));
	} else {
	    driver = dor_space_reallocate
		     (   doe_device_drivers,
		         (doe_device_max_drivers+5) * sizeof(dot_device_driver)
		     );
	}
	if (!driver) {
	    DDerror
		(ERR_CANT_ALLOC_MEM, "DDdevice_InstallDriver", "driver array");
	    return;
	} else {
	    doe_device_max_drivers += 5;
	    doe_device_drivers = driver;
	}
    }

    driver = &(doe_device_drivers[doe_device_driver_count++]);

    if (!name) {
	driver->device_name = "";
    } else {
	driver->device_name = dor_space_allocate (strlen(name) + 1);
	strcpy (driver->device_name, name);
    }

    if (!description) {
	driver->description = "";
    } else {
	driver->description = dor_space_allocate (strlen(description) + 1);
	strcpy (driver->description, description);
    }

    if (!get_fcns_fcn) {
	DDerror (ERR_VALUE_OUT_OF_RANGE, 
		 "DDdevice_InstallDriver", "get_fcns_fcn == NULL");
	return;
    }

    driver->get_interface_functions = get_fcns_fcn;

    /* Get the DCM functions and install them into the device driver.  */

    (*get_fcns_fcn) (DDc_DCM, &fcns);

    if (!fcns) {
	DDerror
	    (ERR_NO_OUTPUT_MODULE, "DDdevice_InstallDriver","Device Functions");
	return;
    }

    driver->functions = fcns;
}

/* ====================================================================== */

static DtInt null_ok_proc (void)
{
    return 0;
}

/* ====================================================================== */

static DtInt null_DCM_proc (void)
{
    DDerror (ERR_UNIMPLEMENTED_SEVERE, "DCM_fcn", DcNullPtr);
    return 0;
}

/* ====================================================================== */

static DtInt null_DROM_proc (void)
{
    DDerror (ERR_UNIMPLEMENTED_SEVERE, "DROM_fcn", DcNullPtr);
    return 0;
}

/* ====================================================================== */

static DtInt null_PROM_proc (void)
{
    DDerror (ERR_UNIMPLEMENTED_SEVERE, "PROM_fcn", DcNullPtr);
    return 0;
}

/* ====================================================================== */

DDt_DCM *dor_device_create_DCM_struct (void)
{
    auto DDt_DCM *fcns;

    fcns = dor_space_allocate (sizeof(DDt_DCM));

    if (!fcns) {
	DDerror (ERR_CANT_ALLOC_MEM_SIZE, "DDdevice_CreateDCMStruct",DcNullPtr);
	return DcNullPtr;
    }

    fcns->version_number = 2;

    /* version 1 fields */

#   define NULL_OK(ret_type,args)	(ret_type (*) args) null_ok_proc
#   define NULL_DCM(ret_type,args)	(ret_type (*) args) null_DCM_proc

    fcns->create_local_data	= NULL_DCM (DtPtr, (DtObject));
    fcns->initialize_device	= NULL_DCM (DtFlag,
					    (DtObject, DtPtr, DtPtr, DtPtr));
    fcns->become_current_driver	= NULL_OK (void, (DtPtr));
    fcns->close_device		= NULL_OK (void, (void));
    fcns->flush			= NULL_OK (void, (void));
    fcns->swap_buffers		= NULL_OK (void, (void));
    fcns->update_geometry	= NULL_OK (void, (void));
    fcns->inquire_device_extent	= NULL_DCM (void, (DtVolume*));
    fcns->inquire_stereo	= NULL_DCM (void, (DtFlag*));
    fcns->inquire_ncolors	= NULL_DCM (void, (DtInt*));
    fcns->inquire_resolution	= NULL_DCM (void, (DtReal*, DtReal*));
    fcns->inquire_visual_type	= NULL_DCM (void, (DtVisualType*));
    fcns->inquire_auto_size	= NULL_DCM (void, (DtFlag*));
    fcns->inquire_color_entries = NULL_DCM (void,
					(DtColorModel, DtInt, DtInt, DtReal[]));
    fcns->set_color_entries	= NULL_OK (void,
					(DtColorModel, DtInt, DtInt, DtReal[]));
    fcns->set_foreground_color	= NULL_OK (void, (DtColorModel, DtReal[]));
    fcns->set_background_color	= NULL_OK (void, (DtColorModel, DtReal[]));
    fcns->set_shade_index	= NULL_OK (void, (DtInt));
    fcns->inquire_clip_list	= NULL_DCM (void,
					(DtUInt*,DtReal**, DtReal[4], DtFlag*));
    fcns->clear_rectangle_depth = NULL_OK (void,
					(DtShort, DtShort, DtShort, DtShort));
    fcns->clear_rectangle_depth_and_color
				  = NULL_OK (void,
					(DtShort, DtShort, DtShort, DtShort));
    fcns->set_depth_buffer_enable = NULL_OK (void, (DtFlag));
    fcns->set_depth_buffer_write  = NULL_OK (void, (DtFlag));
    fcns->write_scanline_byte	= NULL_OK (void, (DtInt,DtInt,DtInt,DtUChar*));
    fcns->set_current_view	= NULL_OK (void, (DtObject));
    fcns->inquire_pixel_data	= NULL_DCM (DtFlag,
					(DtRasterType, DtInt*, DtInt*,
					DtRasterType*, DtPtr*, DtFlag*));

    /* version 2 fields */

    fcns->set_options = NULL_OK (void, (DtPtr));

    /* Return the filled-in DCM structure. */

    return fcns;

#   undef NULL_OK
#   undef NULL_DCM
}

/* ====================================================================== */

DDt_PROM *dor_device_create_PROM_struct (void)
{
    auto DDt_PROM *fcns;

    fcns = dor_space_allocate (sizeof (DDt_PROM));
    if (!fcns) {
	DDerror (ERR_CANT_ALLOC_MEM_SIZE,"DDdevice_CreatePROMStruct",DcNullPtr);
	return DcNullPtr;
    }

    fcns->version_number = 1; 

    /* version 1 fields */

    fcns->write_rectangle_byte_rgb
	= (void (*)(DtInt,DtInt,DtInt,DtInt,DtUChar*)) null_PROM_proc;

    return fcns;
}

/* ====================================================================== */

DDt_DROM *dor_device_create_DROM_struct (void)
{
    auto DDt_DROM *fcns;

    fcns = dor_space_allocate (sizeof (DDt_DROM));
    if (!fcns) {
	DDerror(ERR_CANT_ALLOC_MEM_SIZE, "DDdevice CreateDROMStruct",DcNullPtr);
	return DcNullPtr;
    }

    fcns->version_number = 2;

    /* version 1 fields */

#   define NULL_OK(ret_type,args)	(ret_type (*) args) null_ok_proc
#   define NULL_DROM(ret_type,args)	(ret_type (*) args) null_DROM_proc

    fcns->pre_initialization  = NULL_OK (void, (void));
    fcns->post_initialization = NULL_OK (void, (void));

    fcns->create_local_device_data = NULL_DROM (DtPtr, (DtObject));
    fcns->create_local_view_data   = NULL_DROM (DtPtr, (DtObject));
    fcns->create_local_window_data = NULL_DROM (DtPtr, (DtObject, DtObject));

    fcns->start_update      = NULL_OK (void,
					(DtObject,DtObject,DtPtr,DtPtr,DtPtr));
    fcns->update_local_data = NULL_OK (void, (void));
    fcns->update_studio     = NULL_OK (void, (DtPFI));
    fcns->update_display    = NULL_OK (void, (DtPFI));

    fcns->camera = NULL_OK (void, (void));
    fcns->light  = NULL_OK (void, (DtObject));

    fcns->set_camera_matrix	= NULL_OK (void, (DtMatrix4x4));
    fcns->set_parallel_matrix	= NULL_OK (void, (DtReal, DtReal, DtReal));
    fcns->set_perspective_matrix= NULL_OK (void, (DtReal, DtReal, DtReal));
    fcns->set_projection_matrix	= NULL_OK (void, (DtArea*, DtProjectionType,
						  DtPoint3, DtReal, DtReal,
						  DtReal));

    fcns->push_att_clpvol  = NULL_OK (void,(void));
    fcns->pop_att_clpvol   = NULL_OK (void,(void));
    fcns->set_att_clpvol   = NULL_OK (void,(DtClipOperator,DtInt,DtHalfSpace*));
    fcns->apply_att_clpvol = NULL_OK (void,(DtClipOperator,DtInt,DtHalfSpace*));

    fcns->get_wcstofcsmat	= NULL_DROM (void,   (DtMatrix4x4));
    fcns->pop_lcstofcsmat	= NULL_OK   (void,   (void));
    fcns->push_lcstofcsmat	= NULL_OK   (void,   (DtMatrix4x4));
    fcns->transform_clip_z_point= NULL_DROM (DtFlag, (DtPoint3, DtMatrix4x4));

    fcns->render_point_list = NULL_OK (void, (
	DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtRealTriple*,
	DtRealTriple*, DtInt, DtRealCouple**, DtInt, DtRealTriple**, DtReal*));

    fcns->render_line_list = NULL_OK (void, (
	DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtRealTriple*,
	DtRealTriple*, DtRealTriple*, DtInt, DtRealCouple**, DtInt,
	DtRealTriple**, DtReal*));

    fcns->render_connected_line_list = NULL_OK (void, (
	DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtRealTriple*,
	DtRealTriple*, DtRealTriple*, DtInt, DtRealCouple**, DtInt,
	DtRealTriple**, DtReal*));

    fcns->render_triangle_list = NULL_OK (void, (
	DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtRealTriple*,
	DtRealTriple*, DtRealTriple*, DtInt, DtRealCouple**, DtInt,
	DtRealTriple**, DtReal*));

    fcns->render_triangle_mesh = NULL_OK (void, (
	DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtInt,
	DtRealTriple*, DtRealTriple*, DtRealTriple*, DtInt[][3], DtInt[][3],
	DtInt, DtRealCouple**, DtInt, DtRealTriple**, DtReal*));

    fcns->set_att_ambint	= NULL_OK (void, (DtReal));
    fcns->set_att_ambswi	= NULL_OK (void, (DtSwitch));
    fcns->set_att_bacfacculble	= NULL_OK (void, (DtSwitch));
    fcns->set_att_bacfacculswi	= NULL_OK (void, (DtSwitch));
    fcns->set_att_clpswi	= NULL_OK (void, (DtSwitch));
    fcns->set_att_depcue	= NULL_OK (void, (DtReal, DtReal, DtReal,
					    DtReal, DtColorModel, DtReal[]));
    fcns->set_att_depcueswi	= NULL_OK (void, (DtSwitch));
    fcns->set_att_difclr	= NULL_OK (void, (DtColorModel, DtReal[]));
    fcns->set_att_difint	= NULL_OK (void, (DtReal));
    fcns->set_att_difswi	= NULL_OK (void, (DtSwitch));
    fcns->set_att_hidsrfswi	= NULL_OK (void, (DtSwitch));
    fcns->set_att_inttyp	= NULL_OK (void, (DtInterpType));
    fcns->set_att_lgtclr	= NULL_OK (void, (DtColorModel, DtReal[]));
    fcns->set_att_lgtint	= NULL_OK (void, (DtReal));
    fcns->set_att_lgttyp	= NULL_OK (void, (DtObject));
    fcns->set_att_lintyp	= NULL_OK (void, (DtLineType));
    fcns->set_att_linwid	= NULL_OK (void, (DtReal));
    fcns->set_att_refswi	= NULL_OK (void, (DtSwitch));
    fcns->set_att_reptyp	= NULL_OK (void, (DtRepType));
    fcns->set_att_shaswi	= NULL_OK (void, (DtSwitch));
    fcns->set_att_shdidx	= NULL_OK (void, (DtInt));
    fcns->set_att_spcclr	= NULL_OK (void, (DtColorModel, DtReal[]));
    fcns->set_att_spcfct	= NULL_OK (void, (DtReal));
    fcns->set_att_spcint	= NULL_OK (void, (DtReal));
    fcns->set_att_spcswi	= NULL_OK (void, (DtSwitch));
    fcns->set_att_srfshd	= NULL_OK (void, (DtObject));
    fcns->set_att_stereo	= NULL_OK (void, (DtReal, DtReal));
    fcns->set_att_stereoswi	= NULL_OK (void, (DtSwitch));
    fcns->set_att_transpclr	= NULL_OK (void, (DtColorModel, DtReal[]));
    fcns->set_att_transpint	= NULL_OK (void, (DtReal));
    fcns->set_att_transpswi	= NULL_OK (void, (DtSwitch));

    fcns->update_lcstowcsmat_lokatfrm
	= NULL_OK (void, (DtPoint3, DtPoint3, DtVector3, DtCompType));
    fcns->update_lcstowcsmat_pop
	= NULL_OK (void, (void));
    fcns->update_lcstowcsmat_push
	= NULL_OK (void, (void));
    fcns->update_lcstowcsmat_rotate
	= NULL_OK (void, (DtAxis, DtReal, DtCompType));
    fcns->update_lcstowcsmat_scale
	= NULL_OK (void, (DtReal, DtReal, DtReal, DtCompType));
    fcns->update_lcstowcsmat_shear
	= NULL_OK (void, (DtMajorPlane, DtReal, DtReal, DtCompType));
    fcns->update_lcstowcsmat_tfmmat
	= NULL_OK (void, (DtMatrix4x4, DtCompType));
    fcns->update_lcstowcsmat_transl
	= NULL_OK (void, (DtReal, DtReal, DtReal, DtCompType));

    fcns->set_att_lgtswi    = NULL_OK (void, (DtInt, DtObject*));
    fcns->set_att_lgtspdexp = NULL_OK (void, (DtReal));
    fcns->set_att_lgtspdang = NULL_OK (void, (DtReal, DtReal));
    fcns->set_att_lgtatn    = NULL_OK (void, (DtReal, DtReal));

    fcns->set_att_mapbmpswi	= NULL_OK (void, (DtSwitch));
    fcns->set_att_mapbmp
	= NULL_OK (void, (DtInt, DtObject[], DtObject[], dot_stdtexatt[]));
    fcns->set_att_mapdifclrswi	= NULL_OK (void, (DtSwitch));
    fcns->set_att_mapdifclr
	= NULL_OK (void, (DtInt, DtObject[], DtObject[], dot_stdtexatt[]));
    fcns->set_att_mapenvswi	= NULL_OK (void, (DtSwitch));
    fcns->set_att_mapenv
	= NULL_OK (void, (DtInt, DtObject[], DtObject[], dot_stdtexatt[]));
    fcns->set_att_maptrnintswi	= NULL_OK (void, (DtSwitch));
    fcns->set_att_maptrnint
	= NULL_OK (void, (DtInt, DtObject[], DtObject[], dot_stdtexatt[]));
    fcns->get_lcstowcsmat	= NULL_DROM (void, (DtMatrix4x4));

    fcns->delete_local_device_data = NULL_OK (void, (DtPtr));
    fcns->delete_local_view_data   = NULL_OK (void, (DtPtr));
    fcns->delete_local_window_data = NULL_OK (void, (DtPtr));

    fcns->use_separate_wireframe = DcFalse;

    /* version 2 fields */

    fcns->set_att_srfedgclr  = NULL_OK (void, (DtColorModel, DtReal[]));
    fcns->set_att_localaaswi = NULL_OK (void, (DtSwitch));
    fcns->set_att_localaasty = NULL_OK (void, (DtLocalAntiAliasStyle));

    fcns->set_att_transporientclr = NULL_OK (void, (DtColorModel, DtReal[]));
    fcns->set_att_transporientexp = NULL_OK (void, (DtReal));
    fcns->set_att_transporientint = NULL_OK (void, (DtReal));
    fcns->set_att_transporientswi = NULL_OK (void, (DtSwitch));

    fcns->set_att_smpadapt     = NULL_OK (void, (DtReal));
    fcns->set_att_smpadaptswi  = NULL_OK (void, (DtSwitch));
    fcns->set_att_smpfilter    = NULL_OK (void, (DtObject, DtReal, DtReal));
    fcns->set_att_smpjitter    = NULL_OK (void, (DtReal));
    fcns->set_att_smpjitterswi = NULL_OK (void, (DtSwitch));
    fcns->set_att_smpsuper     = NULL_OK (void, (DtInt, DtInt));
    fcns->set_att_smpsuperswi  = NULL_OK (void, (DtSwitch));

    return fcns;

#   undef NULL_OK
#   undef NULL_DROM
}
