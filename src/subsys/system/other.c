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
	dor_system_initialize
	dor_system_terminate
	dor_system_compute_bounding_volume
	dor_system_execute_object
	dor_system_report_bounding_points
	dor_std3dtablelookup_handler
	dor_stdtablelookup_handler
	dor_stdbumpmap_handler
	dor_stdsphereenvironmap_handler
	dor_stdcubeenvironmap_handler
	dor_2partmap_handler
	dor_system_initialize_texture_mapping_callbacks
	dor_system_texture_uv_count
	dor_system_texture_uvw_count
	dor_system_inq_angle_units
	dor_system_set_angle_units
        dor_system_radiosity_initialize

  ======================================================================
 */

#include <string.h>
#include <dore/internal/dogen.h>
#include "system.h"

Dt32Bits doe_system_state;
DtInt doe_system_execution_depth;

DtSwitch doe_system_valuator_switch;
DtFlag doe_system_alternate_object_execution_flag;
DtFlag doe_system_subdivision_modification_flag;
DtInt doe_system_main_processors;
DtInt doe_system_current_method;
DtInt doe_system_active_traversals;

DtInt doe_system_timing_points_drawn;
DtInt doe_system_timing_lines_drawn;
DtInt doe_system_timing_connected_lines_drawn;
DtInt doe_system_timing_triangles_drawn;
DtInt doe_system_timing_mesh_triangles_drawn;
DtInt doe_system_timing_strip_triangles_drawn;
DtInt doe_system_timing_quads_drawn;
DtInt doe_system_timing_mesh_quads_drawn;
DtInt doe_system_timing_firmware_spheres_drawn;
DtInt doe_system_timing_firmware_cylinders_drawn;
DtInt doe_system_timing_display_traverse_count;

DtAngleUnits doe_system_angle_units;

DtInt doe_system_edit_source;

DtFlag doe_system_bndvol_first_report;
DtReal
	doe_system_bndvol_min_x,
	doe_system_bndvol_max_x,
	doe_system_bndvol_min_y,
	doe_system_bndvol_max_y,
	doe_system_bndvol_min_z,
	doe_system_bndvol_max_z;

DtPtr doe_system_version_string;

/*
 ======================================================================
 */

void dor_system_initialize (
    DtInt processors,	/* Number of Processors */
    int   precision)	/* Precision of a DtReal (1 or 2) */
{
    static char *version = "@(#)Dore 6.6";
    static DtFlag initialization_done = DcFalse;
#   ifdef DORE_VERSION
	extern char *dore_product_info[];
	char *sid_version = dore_product_info[0];
#   endif

    if (initialization_done) {
        /* Currently, Dore cannot be terminated and re-initialized */
	DDerror (ERR_DORE_ALREADY_INIT, "DsInitialize", DcNullPtr);
        return;
    }

    /* Error initialization must occur first in case an error occurs during
    ** the call to dor_doresys_initialize */

    dor_error_initialize ();

    doe_system_version_string = DDspace_Allocate (strlen(version) + 12);
    if (!doe_system_version_string) {
	doe_system_version_string = version;
    } else {
	strcpy (doe_system_version_string, &(version[4]));

#       ifdef DORE_REAL_SINGLE
	    strcat (doe_system_version_string, " (SINGLE)");
#       else
	    strcat (doe_system_version_string, " (DOUBLE)");
#       endif
    }

    /* Ensure that the application agrees with this version of the Dore'
    ** library on the precision of a DtReal.  NOTE:  The following errors
    ** are flagged only if the precision parameter is opposite what the
    ** current type is (single, precision == 2, or double, precision == 1).
    ** One reason for this is that if FORTRAN is used, we specify 0 for
    ** the precision value, since we can't use cpp hackery to determine
    ** the app program's precision.  If precision isn't 1 or 2, then the
    ** following checking is not performed.  */

#   ifdef DORE_REAL_SINGLE
	if (precision == 2)
	{   DDerror (ERR_DTREAL_CONFLICT, "DsInitialize",
		"Single-precision Dore'; application uses -DDORE_REAL_DOUBLE");
	}
#   else
	if (precision == 1)
	{   DDerror (ERR_DTREAL_CONFLICT, "DsInitialize",
		"Double-precision Dore'; application uses -DDORE_REAL_SINGLE");
	}
#   endif

    doe_system_main_processors	 = processors;
    doe_system_current_method	 = DcMethodNull;
    doe_system_active_traversals = 0;
    doe_system_angle_units	 = DcAngleOldStyle;

    dor_doresys_initialize(processors);

    dor_object_initialize();

    /*  Initialize the utilities.  */

    dor_condex_initialize();
    dor_font_initialize();
    dor_gas_initialize();
    dor_gpolyd_initialize();
    dor_print_initialize();

    dor_device_initialize();
    dor_frame_initialize();
    dor_group_initialize();
    dor_view_initialize();
    dor_slot_initialize();

    dor_camera_initialize();
    dor_light_initialize();

    dor_global_exeset_initialize();

    /* Initialize the standard types provided with Dore. */

    dor_cmptexuvcbk_initialize_types();
    dor_cmptexuvwcbk_initialize_types();
    dor_lgttyp_initialize_types();
    dor_srfshd_initialize_types();
    dor_patchmatrix_initialize_types();
    dor_smpfilter_initialize_filters();
    dor_system_initialize_texture_mapping_callbacks();
    dor_rstr_init_delete_callback();

    /*  Initialize the attributes stacks.  */

    dor_global_ambint_initialize();
    dor_global_ambswi_initialize();
    dor_global_bacfacculble_initialize();
    dor_global_bacfacculswi_initialize();
    dor_global_bndvolswi_initialize();
    dor_global_cammat_initialize();
    dor_global_cmptexuvswi_initialize();
    dor_global_cmptexuvwswi_initialize();
    dor_global_cmptexuvcbk_initialize();
    dor_global_cmptexuvwcbk_initialize();
    dor_global_depcue_initialize();
    dor_global_depcueswi_initialize();
    dor_global_difclr_initialize();
    dor_global_difint_initialize();
    dor_global_difswi_initialize();
    dor_global_emisclr_initialize();
    dor_global_emisfct_initialize();
    dor_global_emisswi_initialize();
    dor_global_fil_initialize();
    dor_global_glbrndmaxobjs_initialize();
    dor_global_glbrndmaxsub_initialize();
    dor_global_glbrndraylvl_initialize();
    dor_global_hidsrfswi_initialize();
    dor_global_intermdisptype_initialize();
    dor_global_inttyp_initialize();
    dor_global_invswi_initialize();
    dor_global_lcstowcsmat_initialize();

    /*
     * modelling clip attributes must be initialized
     * after lcstowcsmat 
     */
    dor_global_clpswi_initialize();
    dor_global_clpvol_initialize();

    dor_global_lgtclr_initialize();
    dor_global_lgtint_initialize();
    dor_global_lgttyp_initialize();
    dor_global_lintyp_initialize();
    dor_global_linwid_initialize();

    dor_global_lgtswi_initialize();
    dor_global_lgtatn_initialize();
    dor_global_lgtspdang_initialize();
    dor_global_lgtspdexp_initialize();

    dor_global_mapbmp_initialize();
    dor_global_mapbmpswi_initialize();
    dor_global_mapdifclr_initialize();
    dor_global_mapdifclrswi_initialize();
    dor_global_mapenv_initialize();
    dor_global_mapenvswi_initialize();
    dor_global_maptrnint_initialize();
    dor_global_maptrnintswi_initialize();

    dor_global_minbndext_initialize();
    dor_global_mkrfnt_initialize();
    dor_global_mkrgly_initialize();
    dor_global_mkrscl_initialize();
    dor_global_namset_initialize();
    dor_global_pckid_initialize();
    dor_global_pckswi_initialize();
    dor_global_radswi_initialize();
    dor_global_radambswi_initialize();
    dor_global_reflecswi_initialize();
    dor_global_refractswi_initialize();
    dor_global_refractidx_initialize();
    dor_global_reptyp_initialize();
    dor_global_shadowswi_initialize();
    dor_global_shdidx_initialize();

    dor_global_smpadapt_initialize();
    dor_global_smpadaptswi_initialize();
    dor_global_smpjitter_initialize();
    dor_global_smpjitterswi_initialize();
    dor_global_smpsuper_initialize();
    dor_global_smpsuperswi_initialize();
    dor_global_smpfilter_initialize();

    dor_global_spcclr_initialize();
    dor_global_spcint_initialize();
    dor_global_spcswi_initialize();
    dor_global_spcfct_initialize();
    dor_global_stereo_initialize();
    dor_global_stereoswi_initialize();
    dor_global_srfshd_initialize();
    dor_global_subdivspc_initialize();
    dor_global_transpclr_initialize();
    dor_global_transpint_initialize();
    dor_global_transpswi_initialize();
    dor_global_transporientclr_initialize();
    dor_global_transporientint_initialize();
    dor_global_transporientexp_initialize();
    dor_global_transporientswi_initialize();

    dor_global_texaa_initialize();
    dor_global_texextuv_initialize();
    dor_global_texextuvw_initialize();
    dor_global_texintsrf_initialize();
    dor_global_texmatuv_initialize();
    dor_global_texmatuvw_initialize();
    dor_global_texop_initialize();
    dor_global_texuvidx_initialize();
    dor_global_texuvwidx_initialize();

    dor_global_txtaln_initialize();
    dor_global_txtexp_initialize();
    dor_global_txtfnt_initialize();
    dor_global_txthgt_initialize();
    dor_global_txtpat_initialize();
    dor_global_txtprc_initialize();
    dor_global_txtspc_initialize();
    dor_global_txtupvec_initialize();

    dor_global_srfedgclr_initialize();
    dor_global_localaaswi_initialize();
    dor_global_localaasty_initialize();

    /* Set the initial state of the global variables */

    doe_system_state = dod_system_state_initialized;

    doe_system_execution_depth = 20;

    doe_system_valuator_switch = DcOn;

    doe_system_alternate_object_execution_flag = DcFalse;

    doe_system_edit_source = dod_system_edit_source_others;

    doe_system_subdivision_modification_flag = DcFalse;

    /*  Initialize the processing subsystems.  
	These subsystems create callback and datavalue 
	objects, and use the attribute objects, so they 
	must be initialized after the other objects.  */

    dor_pick_initialize();

    dor_render_initialize();

    initialization_done = DcTrue;
}
/*
 ======================================================================
 */

void dor_system_terminate (void)
{
    /* these should be called last before terminating */

    dor_device_terminate() ;
    dor_object_terminate() ;

    dor_doresys_terminate();

    doe_system_state = dod_system_state_uninitialized ;
}
/*
 ======================================================================
 */

DtVolume *dor_system_compute_bounding_volume (
    DtVolume *volume,
    DtObject object)
{
#ifdef ERRCHK
    if(DcObjectValid != dor_object_validate(object)){
	DDerror(ERR_BAD_OBJECT,
		   "DsCompBoundingVol",DcNullPtr);
	return(volume);
    }
#endif

    /*  The variable doe_system_active_traversals is used 
	to indicate that a function is active that requires 
	traversals of the data base.  
	These functions include:
        	DdUpdate		
        	DdPick		
        	DfUpdate
        	DgCheck	
        	DsCompBoundingVol	
        	DvUpdate	

      	Multiple active traversals are not possible because
      	the state of a traversal is maintained in the global 
      	attributes.  This state would need to be saved and 
      	restored if traversals were allowed to be re-entrant.
      	Further each of the above sub-systems also maintains 
	state during traversal and that state would likewise 
	need to be saved and restored.  */

    if (doe_system_active_traversals) {
	DDerror (ERR_MULTIPLE_TRAVERSAL,
		    "DsCompBoundingVol", DcNullPtr);
	return(volume);
    }

    doe_system_active_traversals++;

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

    doe_system_active_traversals--;

    return(volume);
}
/*
 ======================================================================
 */

    /*  Execute the current method on the object, and then 
	call an object management routine to delete the object 
	if is not referenced anywhere else in the Dore database 
	and is not held by the user.  */

void dor_system_execute_object (
    dot_object *object)
{
#ifdef ERRCHK
    if(DcObjectValid != dor_object_validate(object)){
	DDerror(ERR_BAD_OBJECT,
		     "DsExecuteObj", DcNullPtr);
	return;
    }

    if(DcMethodNull == doe_system_current_method){
	DDerror(ERR_NO_TRAVERSAL,
		     "DsExecuteObj", DcNullPtr);
	return;
    }
#endif

    switch (doe_system_current_method) {
    case DcMethodPrint:
	dor_object_print (object);
	break;

    case DcMethodPick:
	dor_pick_initialize_element(DcTrue);
	dor_object_get_current_method(object)(object);
	dor_pick_terminate_element();
	break;

    default:
	dor_object_get_current_method(object)(object);
	break;
    }

    dor_object_check_deletion(object);
}
/*
 ======================================================================
 */

    /*  Called by each primitive to report a set of bounding points
	when executing their "compute bounding volume" methods.
	The convex hull of the given points is assumed to completely
	(and hopefully tightly) enclose that primitive.  */

void dor_system_report_bounding_points (
    DtInt v_count,
    DtReal vtxs[])
{
    DtMatrix4x4 curmatrix;
    DtReal min_x, max_x, min_y, max_y, min_z, max_z;
    DtReal trans_x, trans_y, trans_z;
    int i;

    if (v_count <= 0)
	    return;

    /* get the current transformation matrix */

    dor_global_lcstowcsmat_get_value (curmatrix);

    /*  take transformed first point for initial 
	min and max values */

    dor_math_pnttrns(vtxs[0],vtxs[1],vtxs[2],
			curmatrix,&min_x,&min_y,&min_z);

    max_x = min_x;
    max_y = min_y;
    max_z = min_z;

    /*  transform and compare remaining points in list 
	with min and max */

    for(i=3; i<v_count*3; i+=3){
	dor_math_pnttrns(vtxs[i],vtxs[i+1],vtxs[i+2],curmatrix,
			 &trans_x,&trans_y,&trans_z);

	min_x = min(min_x,trans_x);
	max_x = max(max_x,trans_x);
	min_y = min(min_y,trans_y);
	max_y = max(max_y,trans_y);
	min_z = min(min_z,trans_z);
	max_z = max(max_z,trans_z);
    }

    if (doe_system_bndvol_first_report){
	doe_system_bndvol_first_report = DcFalse;
	doe_system_bndvol_min_x = min_x;
	doe_system_bndvol_max_x = max_x;
	doe_system_bndvol_min_y = min_y;
	doe_system_bndvol_max_y = max_y;
	doe_system_bndvol_min_z = min_z;
	doe_system_bndvol_max_z = max_z;

    } else{
	doe_system_bndvol_min_x = min(doe_system_bndvol_min_x,min_x);
	doe_system_bndvol_max_x = max(doe_system_bndvol_max_x,max_x);
	doe_system_bndvol_min_y = min(doe_system_bndvol_min_y,min_y);
	doe_system_bndvol_max_y = max(doe_system_bndvol_max_y,max_y);
	doe_system_bndvol_min_z = min(doe_system_bndvol_min_z,min_z);
	doe_system_bndvol_max_z = max(doe_system_bndvol_max_z,max_z);
    }
}
/*
 ======================================================================
 */

DtObject DcStd3dTableLookup;
DtObject DcStdTableLookup;
DtObject DcStdBumpMap;
DtObject DcStdSphereEnvironMap;
DtObject DcStdCubeEnvironMap;
DtObject Dc2PartMap;

/*  Initialize the sample filter callback objects provided 
    with standard Dore.  */

static void dor_std3dtablelookup_handler    (void) { return; }
static void dor_stdtablelookup_handler      (void) { return; }
static void dor_stdbumpmap_handler          (void) { return; }
static void dor_stdsphereenvironmap_handler (void) { return; }
static void dor_stdcubeenvironmap_handler   (void) { return; }
static void dor_2partmap_handler            (void) { return; }


void dor_system_initialize_texture_mapping_callbacks (void)
{
    dot_object *datavalobj;

    datavalobj = dor_datval_create((Dt32Bits)0);

    DcStd3dTableLookup = dor_calbak_create
		((DtPFI) dor_std3dtablelookup_handler, datavalobj, DcFalse);
    DcStdTableLookup = dor_calbak_create
		((DtPFI) dor_stdtablelookup_handler, datavalobj, DcFalse);
    DcStdBumpMap = dor_calbak_create
		((DtPFI) dor_stdbumpmap_handler, datavalobj, DcFalse);
    DcStdCubeEnvironMap = dor_calbak_create
		((DtPFI) dor_stdcubeenvironmap_handler, datavalobj, DcFalse);
    DcStdSphereEnvironMap = dor_calbak_create
		((DtPFI) dor_stdsphereenvironmap_handler, datavalobj, DcFalse);
    Dc2PartMap = dor_calbak_create
		((DtPFI) dor_2partmap_handler, datavalobj, DcFalse);

    /* Create dummy references to these objects in the event that
     * they are dereferenced such that their reference count goes to
     * zero: this keeps them from being deleted entirely.
     */

    dor_object_add_reference(DcStd3dTableLookup, DcNullObject);
    dor_object_add_reference(DcStdTableLookup, DcNullObject);
    dor_object_add_reference(DcStdBumpMap, DcNullObject);
    dor_object_add_reference(DcStdCubeEnvironMap, DcNullObject);
    dor_object_add_reference(DcStdSphereEnvironMap, DcNullObject);
    dor_object_add_reference(Dc2PartMap, DcNullObject);
}
/*
 ======================================================================
 */

DtVertexType dor_system_texture_uv_count (
    Dt32Bits count)
{
    if (count > dod_system_max_vertex_uv_cnt) {
	sprintf (DDerror_GetLine(), "Maximum UV Count is %d", 
		 dod_system_max_vertex_uv_cnt);
	DDerror (ERR_VALUE_OUT_OF_RANGE, 
		    "DsTextureUVCount",
		    DDerror_GetLine());
    }

    return ((DtVertexType)((count&0xF)<<16));
}
/*
 ======================================================================
 */

DtVertexType dor_system_texture_uvw_count (
    Dt32Bits count)
{
    if (count > dod_system_max_vertex_uvw_cnt) {
	sprintf (DDerror_GetLine(), "Maximum UVW Count is %d", 
		 dod_system_max_vertex_uvw_cnt);
	DDerror (ERR_VALUE_OUT_OF_RANGE, 
		    "DsTextureUVWCount",
		    DDerror_GetLine());
    }

    return ((DtVertexType)((count&0xF)<<24));
}
/*
 ======================================================================
 */
DtAngleUnits dor_system_inq_angle_units (void)
{
    return doe_system_angle_units;
}
/*
 ======================================================================
 */
void dor_system_set_angle_units (
    DtAngleUnits units)
{
#ifdef ERRCHK
    if (units != DcAngleOldStyle && units != DcAngleRadians &&
	units != DcAngleDegrees) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		 "DsSetAngleUnits", DcNullPtr);
	return;
    }
#endif

    doe_system_angle_units = units;
}
/*
 ======================================================================
 */

void dor_system_radiosity_initialize (void)
{
    dpr_radiosity_initialize ();
}
