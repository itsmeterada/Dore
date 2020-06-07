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
	static null_routine ()
	DtInt drr_dynrnd_install_renderer (renderer_id)
	void drr_dynrnd_new_class (class_name, class_id)
	void drr_dynrnd_set_object_methods ()
	void drr_dynrnd_update_default_methods ()

  ======================================================================
 */
#include "dynrnd.h"

drt_dynrnd_data		 dre_dynrnd_data;
drt_dynrnd_device_data	*dre_dynrnd_curdevdat;
drt_dynrnd_view_data	*dre_dynrnd_curviwdat;
drt_dynrnd_window_data	*dre_dynrnd_curwindat;
DDt_DROM		*dre_dynrnd_DROM_fcns;
DDt_DROMMethod	*dre_dynrnd_DROM_method;
DDt_DROMMethod	*dre_dynrnd_DROM_default_method;
DtInt			 dre_dynrnd_DROM_default_method_max_count;
DtInt			 dre_dynrnd_DROM_default_method_count;
DtInt			 dre_dynrnd_display_method_id;

drt_dynrnd_methods dre_dynrnd_attribute[] = {
   { "AttAmbientIntens",                (DtMethodPtr) drr_dynrnd_ambint },
   { "AttAmbientSwitch",                (DtMethodPtr) drr_dynrnd_ambswi },
   { "AttBackfaceCullable",             (DtMethodPtr) drr_dynrnd_bacfacculble },
   { "AttBackfaceCullSwitch",           (DtMethodPtr) drr_dynrnd_bacfacculswi },
   { "AttClipSwitch",                   (DtMethodPtr) drr_dynrnd_clpswi },
   { "AttDepthCue",                     (DtMethodPtr) drr_dynrnd_depcue },
   { "AttDepthCueSwitch",               (DtMethodPtr) drr_dynrnd_depcueswi },
   { "AttDiffuseColor",                 (DtMethodPtr) drr_dynrnd_difclr },
   { "AttDiffuseIntens",                (DtMethodPtr) drr_dynrnd_difint },
   { "AttDiffuseSwitch",                (DtMethodPtr) drr_dynrnd_difswi },
   { "AttHiddenSurfaceSwitch",          (DtMethodPtr) drr_dynrnd_hidsrfswi },
   { "AttInterpType",                   (DtMethodPtr) drr_dynrnd_inttyp },
   { "AttLightAttenuation",             (DtMethodPtr) drr_dynrnd_lgtatn },
   { "AttLightColor",                   (DtMethodPtr) drr_dynrnd_lgtclr },
   { "AttLightIntens",                  (DtMethodPtr) drr_dynrnd_lgtint },
   { "AttLightSpreadAngles",            (DtMethodPtr) drr_dynrnd_lgtspdang },
   { "AttLightSpreadExp",               (DtMethodPtr) drr_dynrnd_lgtspdexp },
   { "AttLightSwitch",                  (DtMethodPtr) drr_dynrnd_lgtswi },
   { "AttLightType",                    (DtMethodPtr) drr_dynrnd_lgttyp },
   { "AttLineType",                     (DtMethodPtr) drr_dynrnd_lintyp },
   { "AttLineWidth",                    (DtMethodPtr) drr_dynrnd_linwid },
   { "AttTextureMapBump",               (DtMethodPtr) drr_dynrnd_mapbmp },
   { "AttTextureMapBumpSwitch",         (DtMethodPtr) drr_dynrnd_mapbmpswi },
   { "AttTextureMapDiffuseColorSwitch", (DtMethodPtr) drr_dynrnd_mapdifclrswi },
   { "AttTextureMapDiffuseColor",       (DtMethodPtr) drr_dynrnd_mapdifclr },
   { "AttTextureMapEnviron",            (DtMethodPtr) drr_dynrnd_mapenv },
   { "AttTextureMapEnvironSwitch",      (DtMethodPtr) drr_dynrnd_mapenvswi },
   { "AttTextureMapTranpIntensSwitch",  (DtMethodPtr) drr_dynrnd_maptrnintswi },
   { "AttTextureMapTranspIntens",       (DtMethodPtr) drr_dynrnd_maptrnint },
   { "AttReflectionSwitch",             (DtMethodPtr) drr_dynrnd_refswi },
   { "AttRepType",                      (DtMethodPtr) drr_dynrnd_reptyp },
   { "AttShadowSwitch",                 (DtMethodPtr) drr_dynrnd_shaswi },
   { "AttShadeIndex",                   (DtMethodPtr) drr_dynrnd_shdidx },
   { "AttSpecularColor",                (DtMethodPtr) drr_dynrnd_spcclr },
   { "AttSpecularFactor",               (DtMethodPtr) drr_dynrnd_spcfct },
   { "AttSpecularIntens",               (DtMethodPtr) drr_dynrnd_spcint },
   { "AttSpecularSwitch",               (DtMethodPtr) drr_dynrnd_spcswi },
   { "AttSurfaceShade",                 (DtMethodPtr) drr_dynrnd_srfshd },
   { "AttStereo",                       (DtMethodPtr) drr_dynrnd_stereo },
   { "AttStereoSwitch",                 (DtMethodPtr) drr_dynrnd_stereoswi },
   { "AttTranspColor",                  (DtMethodPtr) drr_dynrnd_transpclr },
   { "AttTranspIntens",                 (DtMethodPtr) drr_dynrnd_transpint },
   { "AttTranspSwitch",                 (DtMethodPtr) drr_dynrnd_transpswi },
   { "AttSurfaceEdgeColor",             (DtMethodPtr) drr_dynrnd_srfedgclr },
   { "AttLocalAntiAliasSwitch",         (DtMethodPtr) drr_dynrnd_localaaswi },
   { "AttLocalAntiAliasStyle",          (DtMethodPtr) drr_dynrnd_localaasty },
   { "AttTranspOrientColor",     (DtMethodPtr) drr_dynrnd_transporientclr },
   { "AttTranspOrientExp",       (DtMethodPtr) drr_dynrnd_transporientexp },
   { "AttTranspOrientIntens",    (DtMethodPtr) drr_dynrnd_transporientint },
   { "AttTranspOrientSwitch",    (DtMethodPtr) drr_dynrnd_transporientswi },
   { "AttSampleAdaptive",               (DtMethodPtr) drr_dynrnd_smpadapt },
   { "AttSampleAdaptiveSwitch",         (DtMethodPtr) drr_dynrnd_smpadaptswi },
   { "AttSampleFilter",         	(DtMethodPtr) drr_dynrnd_smpfilter },
   { "AttSampleJitter",         	(DtMethodPtr) drr_dynrnd_smpjitter },
   { "AttSampleJitterSwitch",         	(DtMethodPtr) drr_dynrnd_smpjitterswi },
   { "AttSampleSuper",         		(DtMethodPtr) drr_dynrnd_smpsuper },
   { "AttSampleSuperSwitch",         	(DtMethodPtr) drr_dynrnd_smpsuperswi },
   { 0, 0 }
};

drt_dynrnd_methods dre_dynrnd_studio_method[] = {
   { "AttLightAttenuation",  (DtMethodPtr) drr_dynrnd_lgtatn },
   { "AttLightColor",        (DtMethodPtr) drr_dynrnd_lgtclr },
   { "AttLightIntens",       (DtMethodPtr) drr_dynrnd_lgtint },
   { "AttLightSpreadAngles", (DtMethodPtr) drr_dynrnd_lgtspdang },
   { "AttLightSpreadExp",    (DtMethodPtr) drr_dynrnd_lgtspdexp },
   { "AttLightType",         (DtMethodPtr) drr_dynrnd_lgttyp },
   { "AttStereo",            (DtMethodPtr) drr_dynrnd_stereo },
   { "AttStereoSwitch",      (DtMethodPtr) drr_dynrnd_stereoswi },
   { "DoLight",              (DtMethodPtr) drr_dynrnd_light },
   { "DoCamera",             (DtMethodPtr) drr_dynrnd_camera },
   { "DoLookAtFrom",         (DtMethodPtr) drr_dynrnd_lokatfrm },
   { "DoRotate",             (DtMethodPtr) drr_dynrnd_rotate },
   { "DoScale",              (DtMethodPtr) drr_dynrnd_scale },
   { "DoShear",              (DtMethodPtr) drr_dynrnd_shear },
   { "DoTranslate",          (DtMethodPtr) drr_dynrnd_transl },
   { "DoTransformMatrix",    (DtMethodPtr) drr_dynrnd_tfmmat },
   { "AttLcstowcsmat",       (DtMethodPtr) drr_dynrnd_lcstowcs },
   { "DoCameraMatrix",       (DtMethodPtr) drr_dynrnd_camera_matrix },
   { "DoParallel",           (DtMethodPtr) drr_dynrnd_parallel_matrix },
   { "DoPerspective",        (DtMethodPtr) drr_dynrnd_perspective_matrix },
   { "DoProjection",         (DtMethodPtr) drr_dynrnd_projection_matrix },
   { "AttSampleAdaptive",               (DtMethodPtr) drr_dynrnd_smpadapt },
   { "AttSampleAdaptiveSwitch",         (DtMethodPtr) drr_dynrnd_smpadaptswi },
   { "AttSampleFilter",         	(DtMethodPtr) drr_dynrnd_smpfilter },
   { "AttSampleJitter",         	(DtMethodPtr) drr_dynrnd_smpjitter },
   { "AttSampleJitterSwitch",         	(DtMethodPtr) drr_dynrnd_smpjitterswi },
   { "AttSampleSuper",         		(DtMethodPtr) drr_dynrnd_smpsuper },
   { "AttSampleSuperSwitch",         	(DtMethodPtr) drr_dynrnd_smpsuperswi },
   { 0, 0 }
};

drt_dynrnd_methods dre_dynrnd_display_method[] = {
   { "AttAmbientIntens",                (DtMethodPtr) drr_dynrnd_ambint },
   { "AttAmbientSwitch",                (DtMethodPtr) drr_dynrnd_ambswi },
   { "AttBackfaceCullable",             (DtMethodPtr) drr_dynrnd_bacfacculble },
   { "AttBackfaceCullSwitch",           (DtMethodPtr) drr_dynrnd_bacfacculswi },
   { "AttClipSwitch",                   (DtMethodPtr) drr_dynrnd_clpswi },
   { "AttDepthCue",                     (DtMethodPtr) drr_dynrnd_depcue },
   { "AttDepthCueSwitch",               (DtMethodPtr) drr_dynrnd_depcueswi },
   { "AttDiffuseColor",                 (DtMethodPtr) drr_dynrnd_difclr },
   { "AttDiffuseIntens",                (DtMethodPtr) drr_dynrnd_difint },
   { "AttDiffuseSwitch",                (DtMethodPtr) drr_dynrnd_difswi },
   { "AttHiddenSurfaceSwitch",          (DtMethodPtr) drr_dynrnd_hidsrfswi },
   { "AttInterpType",                   (DtMethodPtr) drr_dynrnd_inttyp },
   { "AttLightSwitch",                  (DtMethodPtr) drr_dynrnd_lgtswi },
   { "AttLineType",                     (DtMethodPtr) drr_dynrnd_lintyp },
   { "AttLineWidth",                    (DtMethodPtr) drr_dynrnd_linwid },
   { "AttTextureMapBump",               (DtMethodPtr) drr_dynrnd_mapbmp },
   { "AttTextureMapBumpSwitch",         (DtMethodPtr) drr_dynrnd_mapbmpswi },
   { "AttTextureMapDiffuseColorSwitch", (DtMethodPtr) drr_dynrnd_mapdifclrswi },
   { "AttTextureMapDiffuseColor",       (DtMethodPtr) drr_dynrnd_mapdifclr },
   { "AttTextureMapEnviron",            (DtMethodPtr) drr_dynrnd_mapenv },
   { "AttTextureMapEnvironSwitch",      (DtMethodPtr) drr_dynrnd_mapenvswi },
   { "AttTextureMapTranpIntensSwitch",  (DtMethodPtr) drr_dynrnd_maptrnintswi },
   { "AttTextureMapTranspIntens",       (DtMethodPtr) drr_dynrnd_maptrnint },
   { "AttReflectionSwitch",             (DtMethodPtr) drr_dynrnd_refswi },
   { "AttRepType",                      (DtMethodPtr) drr_dynrnd_reptyp },
   { "AttShadowSwitch",                 (DtMethodPtr) drr_dynrnd_shaswi },
   { "AttShadeIndex",                   (DtMethodPtr) drr_dynrnd_shdidx },
   { "AttSpecularColor",                (DtMethodPtr) drr_dynrnd_spcclr },
   { "AttSpecularFactor",               (DtMethodPtr) drr_dynrnd_spcfct },
   { "AttSpecularIntens",               (DtMethodPtr) drr_dynrnd_spcint },
   { "AttSpecularSwitch",               (DtMethodPtr) drr_dynrnd_spcswi },
   { "AttSurfaceShade",                 (DtMethodPtr) drr_dynrnd_srfshd },
   { "AttTranspColor",                  (DtMethodPtr) drr_dynrnd_transpclr },
   { "AttTranspIntens",                 (DtMethodPtr) drr_dynrnd_transpint },
   { "AttTranspSwitch",                 (DtMethodPtr) drr_dynrnd_transpswi },
   { "AttClipVol",                      (DtMethodPtr) drr_dynrnd_global_clpvol},
   { "AttSurfaceEdgeColor",             (DtMethodPtr) drr_dynrnd_srfedgclr },
   { "AttLocalAntiAliasSwitch",         (DtMethodPtr) drr_dynrnd_localaaswi },
   { "AttLocalAntiAliasStyle",          (DtMethodPtr) drr_dynrnd_localaasty },
   { "DoClipVol",                       (DtMethodPtr) drr_dynrnd_clpvol },
   { "DoLookAtFrom",                    (DtMethodPtr) drr_dynrnd_lokatfrm },
   { "DoRotate",                        (DtMethodPtr) drr_dynrnd_rotate },
   { "DoScale",                         (DtMethodPtr) drr_dynrnd_scale },
   { "DoShear",                         (DtMethodPtr) drr_dynrnd_shear },
   { "DoTranslate",                     (DtMethodPtr) drr_dynrnd_transl },
   { "DoTransformMatrix",               (DtMethodPtr) drr_dynrnd_tfmmat },
   { "AttLcstowcsmat",                  (DtMethodPtr) drr_dynrnd_lcstowcs },
   { "DoAnnoText",                      (DtMethodPtr) drr_dynrnd_anntxt },
   { "DoLineList",                      (DtMethodPtr) drr_dynrnd_linlst },
   { "DoPolyline",                      (DtMethodPtr) drr_dynrnd_plylin },
   { "DoPolymarker",                    (DtMethodPtr) drr_dynrnd_plymkr },
   { "DoPointList",                     (DtMethodPtr) drr_dynrnd_pntlst },
   { "DoTriangleList",                  (DtMethodPtr) drr_dynrnd_trilst },
   { "DoTriangleMesh",                  (DtMethodPtr) drr_dynrnd_trimsh },
   { "DoTriangleStrip",                 (DtMethodPtr) drr_dynrnd_tristrip },
   { "DoVarLineList",                   (DtMethodPtr) drr_dynrnd_varlinlst },
   { "DoVarPointList",                  (DtMethodPtr) drr_dynrnd_varpntlst },
   { "DoVarTriangleMesh",               (DtMethodPtr) drr_dynrnd_vartrimsh },
   { "DoVarTriangleStrip",              (DtMethodPtr) drr_dynrnd_vartristrip },
   { "AttTranspOrientColor",         (DtMethodPtr) drr_dynrnd_transporientclr },
   { "AttTranspOrientExp",           (DtMethodPtr) drr_dynrnd_transporientexp },
   { "AttTranspOrientIntens",        (DtMethodPtr) drr_dynrnd_transporientint },
   { "AttTranspOrientSwitch",        (DtMethodPtr) drr_dynrnd_transporientswi },
   { 0, 0 }
};
/*
 ======================================================================
 */
static void null_routine (void) { return; }
/*
 ======================================================================
 *
 * NOTE: The dynrnd renderer uses a special rendering interface call
 * to create its methods.  This is for backwards compatibility with 
 * old user code that may have used the constants DcMethodDyn... for  
 * access to dynrnd methods.
 * The code contains comments that indicate how the methods would be 
 * created using the standard rendering interface calls.
 ======================================================================
 */
DtInt drr_dynrnd_install_renderer (
    DtInt renderer_id)
{
    DtInt i;
    DtInt type;

    dre_dynrnd_data.current_device = NULL;
    dre_dynrnd_data.current_view = NULL;

    dre_dynrnd_curdevdat = NULL;
    dre_dynrnd_curviwdat = NULL;
    dre_dynrnd_curwindat = NULL;

    dre_dynrnd_DROM_default_method = (DDt_DROMMethod *)0;
    dre_dynrnd_DROM_default_method_count = 0;
    dre_dynrnd_DROM_default_method_max_count = 0;

    dre_dynrnd_data.studio_method =
	    DDclass_ReplaceMethod("DynrndStudio", DcMethodDynIniRender,
				  DcMethodStdRenderStudio);
    dre_dynrnd_data.display_method =
	    DDclass_ReplaceMethod("DynrndDisplay", DcMethodDynRender,
				  DcMethodStdRenderDisplay);

    /*
     *  current_display_method is the method that will be used
     *  to perform the traversal. It will be different from
     *  display_method only if dre_dynrnd_reset_current_display_method
     *  was called to modify the method used for traversal.
     */
    dre_dynrnd_data.current_display_method =
                                   dre_dynrnd_data.display_method;

    /* save a copy for extern references */
    dre_dynrnd_display_method_id = dre_dynrnd_data.display_method;


    /*
     * For Standard Rendering Interface method creation use:
     *   dre_dynrnd_data.studio_method =
     *	 DDclass_CopyMethod(DcMethodStdRenderStudio, "DynrndStudio");
     *   dre_dynrnd_data.display_method =
     *	 DDclass_CopyMethod(DcMethodStdRenderDisplay, "DynrndDisplay");
     */

    dre_dynrnd_data.device_index =
	    DDclass_AddObjectData(DsInqClassId("DoDevice"),
				  (DtMethodPtr) drr_dynrnd_create_device_data,
				  (DtMethodPtr) null_routine,
				  (DtMethodPtr) drr_dynrnd_delete_device_data);
				 
    dre_dynrnd_data.view_index =
	    DDclass_AddObjectData(DsInqClassId("DoView"),
				  (DtMethodPtr) drr_dynrnd_create_driver_data,
				  (DtMethodPtr) null_routine,
				  (DtMethodPtr) drr_dynrnd_delete_driver_data);

#   ifdef DEBUG
	printf ("studio_method  = %lx\n", dre_dynrnd_data.studio_method);
	printf ("display_method = %lx\n", dre_dynrnd_data.display_method);
	printf ("device_index   = %d\n",  dre_dynrnd_data.device_index);
	printf ("view_index     = %d\n",  dre_dynrnd_data.view_index);
#   endif

    dre_dynrnd_data.light_switch_count = 0;
    dre_dynrnd_data.light_switch = DcNullPtr;

    dre_dynrnd_data.bmp_max_textures = 0;
    dre_dynrnd_data.bmp_mapping    = (DtObject *)0;
    dre_dynrnd_data.bmp_raster     = (DtObject *)0;
    dre_dynrnd_data.bmp_attributes = (dot_stdtexatt *)0;
    dre_dynrnd_data.bmp_size       = (DtInt *)0;

    dre_dynrnd_data.difclr_max_textures = 0;
    dre_dynrnd_data.difclr_mapping    = (DtObject *)0;
    dre_dynrnd_data.difclr_raster     = (DtObject *)0;
    dre_dynrnd_data.difclr_attributes = (dot_stdtexatt *)0;
    dre_dynrnd_data.difclr_size       = (DtInt *)0;

    dre_dynrnd_data.env_max_textures = 0;
    dre_dynrnd_data.env_mapping    = (DtObject *)0;
    dre_dynrnd_data.env_raster     = (DtObject *)0;
    dre_dynrnd_data.env_attributes = (dot_stdtexatt *)0;
    dre_dynrnd_data.env_size       = (DtInt *)0;

    dre_dynrnd_data.trnint_max_textures = 0;
    dre_dynrnd_data.trnint_mapping    = (DtObject *)0;
    dre_dynrnd_data.trnint_raster     = (DtObject *)0;
    dre_dynrnd_data.trnint_attributes = (dot_stdtexatt *)0;
    dre_dynrnd_data.trnint_size       = (DtInt *)0;


    for (i=0; dre_dynrnd_studio_method[i].name != 0; i++) {
#       ifdef DEBUG
	    printf ("studio method for %s - ",dre_dynrnd_studio_method[i].name);
#       endif

	if ((type = DsInqClassId(dre_dynrnd_studio_method[i].name)) != -1) {
#           ifdef DEBUG
	        printf ("added\n");
#           endif
	    DDclass_SetMethod(type, dre_dynrnd_data.studio_method,
			       dre_dynrnd_studio_method[i].routine);
	}
#       ifdef DEBUG
	    else {
	        printf ("not added\n");
	    }
#       endif
    }

    for (i=0; dre_dynrnd_display_method[i].name != 0; i++) {
#       ifdef DEBUG
	    printf ("display method for %s - ", 
		dre_dynrnd_display_method[i].name);
#       endif
	if ((type = DsInqClassId(dre_dynrnd_display_method[i].name)) != -1) {
#           ifdef DEBUG
	        printf ("added\n");
#           endif
	    DDclass_SetMethod(type, dre_dynrnd_data.display_method,
			       dre_dynrnd_display_method[i].routine);
	}
#       ifdef DEBUG
	    else printf ("not added\n");
#       endif
    }

    return DDrender_AddRenderer
	   (   renderer_id, "Dynamic Renderer",
	       (DtMethodPtr) drr_dynrnd_render,
	       (DtMethodPtr) null_routine,
	       drr_dynrnd_new_class,
	       (DtMethodPtr) drr_dynrnd_get_wcstofcsmat
	   );
}
/*
 ======================================================================
 */
void drr_dynrnd_new_class (
    DtPtr class_name,
    DtInt class_id)
{
    DtInt i;

#ifdef DEBUG
    printf ("new class %s - %d\n", class_name, class_id);
#endif

    for (i=0; dre_dynrnd_studio_method[i].name != 0; i++) {
	if (strcmp(dre_dynrnd_studio_method[i].name, class_name) == 0) {
#ifdef DEBUG
	    printf ("\tadded studio method\n");
#endif
	    DDclass_SetMethod(class_id,
			       dre_dynrnd_data.studio_method,
			       dre_dynrnd_studio_method[i].routine);
	    break;
	}
    }

    for (i=0; dre_dynrnd_display_method[i].name != 0; i++) {
	if (strcmp(dre_dynrnd_display_method[i].name, class_name) == 0) {
#ifdef DEBUG
	    printf ("\tadded display method\n");
#endif
	    DDclass_SetMethod(class_id,
			       dre_dynrnd_data.display_method,
			       dre_dynrnd_display_method[i].routine);
	}
    }

    for (i=0; i < dre_dynrnd_DROM_default_method_count; i++) {
	if (strcmp(dre_dynrnd_DROM_default_method[i].name, class_name) == 0) {
	    dre_dynrnd_DROM_default_method[i].routine =
		    DDclass_InqMethod(class_id, 
					dre_dynrnd_data.display_method);
	}
    }

    if (dre_dynrnd_DROM_method != (DDt_DROMMethod *)0) {
	for (i=0; dre_dynrnd_DROM_method[i].name != 0; i++) {
	    if (strcmp(dre_dynrnd_DROM_method[i].name, class_name) == 0) {
		DDclass_SetMethod(class_id,
				   dre_dynrnd_data.display_method,
				   dre_dynrnd_DROM_method[i].routine);
	    }
	}
    }
}
/*
 ======================================================================
 */
void drr_dynrnd_set_object_methods (void)
{
    DtInt i;
    DtInt class_id;

    for (i=0; i<dre_dynrnd_DROM_default_method_count; i++) {
	if ((class_id=DsInqClassId(dre_dynrnd_DROM_default_method[i].name)) != -1) {
	    DDclass_SetMethod(class_id,
			     dre_dynrnd_data.display_method,
			     dre_dynrnd_DROM_default_method[i].routine);
	}
    }

    if (dre_dynrnd_DROM_method != (DDt_DROMMethod *)0) {
	for (i=0; dre_dynrnd_DROM_method[i].name != 0; i++) {
	    if ((class_id=DsInqClassId(dre_dynrnd_DROM_method[i].name)) != -1) {
		DDclass_SetMethod(class_id,
				   dre_dynrnd_data.display_method,
				   dre_dynrnd_DROM_method[i].routine);
	    }
	}
    }
}
/*
 ======================================================================
 */
void drr_dynrnd_update_default_methods (void)
{
    DtInt i, j;
    DtFlag add_method;

    if (dre_dynrnd_DROM_method != (DDt_DROMMethod *)0) {
	for (i=0; dre_dynrnd_DROM_method[i].name != 0; i++) {
	    add_method = DcTrue;
	    for (j=0; j < dre_dynrnd_DROM_default_method_count; j++) {
		if (strcmp(dre_dynrnd_DROM_method[i].name, 
			   dre_dynrnd_DROM_default_method[j].name) == 0) {
		    /* the device method for this object is already
		       in the default list */
		    add_method = DcFalse;
		    break;
		}
	    }
	    if (add_method) {
		DtInt index;
		DtInt class_id;
		DtInt new_size;
		DDt_DROMMethod *new_list;

		index = dre_dynrnd_DROM_default_method_count;

		if ((dre_dynrnd_DROM_default_method_count + 1) >
		    dre_dynrnd_DROM_default_method_max_count) {
		    new_size = dre_dynrnd_DROM_default_method_max_count 
				+ 10;
		    new_list = (DDt_DROMMethod *)
			    DDspace_Reallocate(
				dre_dynrnd_DROM_default_method,
				new_size*sizeof(DDt_DROMMethod));
		    if (new_list == (DDt_DROMMethod *)0) {
			DDerror(ERR_CANT_ALLOC_MEM, 
				   "drr_dynrnd_set_device_methods",
				   DcNullPtr);
			return;
		    }
		    dre_dynrnd_DROM_default_method_max_count = new_size;
		    dre_dynrnd_DROM_default_method = new_list;
		}
		dre_dynrnd_DROM_default_method[index].name = 
			dre_dynrnd_DROM_method[i].name;
		if ((class_id = DsInqClassId(dre_dynrnd_DROM_method[i].name)) != -1) {
		    dre_dynrnd_DROM_default_method[i].routine =
			    DDclass_InqMethod(class_id, 
					dre_dynrnd_data.display_method);
		}
		dre_dynrnd_DROM_default_method_count++;
	    }
	}
    }
}
