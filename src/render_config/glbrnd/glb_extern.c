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
	DtInt drr_glbrnd_install_renderer (renderer_id)
	int drr_glbrnd_new_class (class_name, class_id)

  ======================================================================
 */
#include "glbrnd.h"


struct glbrnd_data dre_glbrnd_data;

struct glbrnd_device_data *dre_glbrnd_curdevdat;
struct glbrnd_view_data   *dre_glbrnd_curviwdat;
struct glbrnd_window_data *dre_glbrnd_curwindat;
struct glbrnd_light_data  *dre_glbrnd_curlgtdat;
struct glbrnd_camera_data *dre_glbrnd_curcamdat;

DDt_PROM *dre_glbrnd_PROM_fcns;

DtFlag dre_glbrnd_initialize_studio;

drt_glbrnd_methods dre_glbrnd_studio_method[] = {
   { "DoLight",		(DtMethodPtr) drr_glbrnd_light_initialize },
   { "DoCamera",	(DtMethodPtr) drr_glbrnd_camera_initialize },
   { "DoCameraMatrix",	(DtMethodPtr) drr_glbrnd_cammat_initialize },
   { "DoParallel",	(DtMethodPtr) drr_glbrnd_parallel_initialize },
   { "DoPerspective",	(DtMethodPtr) drr_glbrnd_perspective_initialize },
   { "DoProjection",	(DtMethodPtr) drr_glbrnd_projection_initialize },
   { 0, 0 }
};

drt_glbrnd_methods dre_glbrnd_display_method[] = {
   { "DoPolygon",          (DtMethodPtr)drr_glbrnd_ply_initialize_objects },
   { "DoPolygonMesh",      (DtMethodPtr)drr_glbrnd_plymsh_initialize_objects },
   { "DoPrimSurf",         (DtMethodPtr)drr_glbrnd_prisrf_initialize_objects },
   { "DoQuadList",         (DtMethodPtr)drr_glbrnd_quadlst_initialize_objects },
   { "DoQuadMesh",         (DtMethodPtr)drr_glbrnd_quadmsh_initialize_objects },
   { "DoSimplePolygon",    (DtMethodPtr)drr_glbrnd_simply_initialize_objects },
   { "DoSimplePolygonMesh",
		(DtMethodPtr) drr_glbrnd_simplymsh_initialize_objects },
   { "DoSphereList",       (DtMethodPtr)drr_glbrnd_sphlst_initialize_objects },
   { "DoText",             (DtMethodPtr)drr_glbrnd_text_initialize_objects },
   { "DoTriangleList",     (DtMethodPtr)drr_glbrnd_trilst_initialize_objects },
   { "DoTriangleMesh",     (DtMethodPtr)drr_glbrnd_trimsh_initialize_objects },
   { "DoTriangleStrip",    (DtMethodPtr)drr_glbrnd_tristrip_initialize_objects},
   { "DoVarSimplePolygonMesh" ,
		(DtMethodPtr) drr_glbrnd_varsimplymsh_initialize_objects },
   { "DoVarTriangleMesh", (DtMethodPtr)drr_glbrnd_vartrimsh_initialize_objects},
   { "DoVarTriangleStrip",
		(DtMethodPtr) drr_glbrnd_vartristrip_initialize_objects },
   { 0, 0 }
};

drt_glbrnd_methods dre_glbrnd_SpcboxOvr_method[] = {
   { "DoPrimSurf",    (DtMethodPtr) drr_glbrnd_prisrf_overlap_spacebox },
   { "Rayintmshtri",  (DtMethodPtr) drr_glbrnd_rayintmshtri_overlap_spacebox },
   { "Rayinttri",     (DtMethodPtr) drr_glbrnd_rayinttri_overlap_spacebox },
   { "Rayintsph",     (DtMethodPtr) drr_glbrnd_rayintsph_overlap_spacebox },
   { 0, 0 }
};

drt_glbrnd_methods dre_glbrnd_Rayint_method[] = {
   { "DoPrimSurf",	(DtMethodPtr) drr_glbrnd_prisrf_rayint },
   { "Rayintmshtri",	(DtMethodPtr) drr_glbrnd_rayintmshtri_rayint },
   { "Rayinttri",	(DtMethodPtr) drr_glbrnd_rayinttri_rayint },
   { "Rayintsph",	(DtMethodPtr) drr_glbrnd_rayintsph_rayint },
   { 0, 0 }
};

drt_glbrnd_methods dre_glbrnd_UsrDat_method[] = {
   { "DoPrimSurf",	(DtMethodPtr) drr_glbrnd_prisrf_userdata },
   { "Rayintmshtri",	(DtMethodPtr) drr_glbrnd_rayintmshtri_userdata },
   { "Rayinttri",	(DtMethodPtr) drr_glbrnd_rayinttri_userdata },
   { "Rayintsph",	(DtMethodPtr) drr_glbrnd_rayintsph_userdata },
   { 0, 0 }
};

drt_glbrnd_methods dre_glbrnd_Wcsloc_method[] = {
   { "DoPrimSurf",	(DtMethodPtr) drr_glbrnd_prisrf_wcsloc },
   { "Rayintmshtri",	(DtMethodPtr) drr_glbrnd_rayintmshtri_wcsloc },
   { "Rayinttri",	(DtMethodPtr) drr_glbrnd_rayinttri_wcsloc },
   { "Rayintsph",	(DtMethodPtr) drr_glbrnd_rayintsph_wcsloc },
   { 0, 0 }
};

drt_glbrnd_methods dre_glbrnd_Wcsnrm_method[] = {
   { "DoPrimSurf",	(DtMethodPtr) drr_glbrnd_prisrf_wcsnrm },
   { "Rayintmshtri",	(DtMethodPtr) drr_glbrnd_rayintmshtri_wcsnrm },
   { "Rayinttri",	(DtMethodPtr) drr_glbrnd_rayinttri_wcsnrm },
   { "Rayintsph",	(DtMethodPtr) drr_glbrnd_rayintsph_wcsnrm },
   { 0, 0 }
};

drt_glbrnd_methods dre_glbrnd_WldBnd_method[] = {
   { "DoPrimSurf",	(DtMethodPtr) drr_glbrnd_prisrf_world_boundary },
   { "Rayintmshtri",	(DtMethodPtr) drr_glbrnd_rayintmshtri_world_boundary },
   { "Rayinttri",	(DtMethodPtr) drr_glbrnd_rayinttri_world_boundary },
   { "Rayintsph",	(DtMethodPtr) drr_glbrnd_rayintsph_world_boundary },
   { 0, 0 }
};

/*
 ======================================================================
 */

static void 
null_routine (void) {}

/*
  NOTE: The glbrnd renderer uses a special rendering interface 
  call to create its methods.  This is for backwards compatibility 
  with old user code that may have used the constants DcMethodGlbrnd... 
  for access to glbrnd methods.
  The code contains comments that indicate how the methods would be 
  created using the standard rendering interface calls.
 */

DtInt 
drr_glbrnd_install_renderer (DtInt renderer_id)
{
    DtInt i;
    DtInt type;
   
    dre_glbrnd_data.current_device = NULL;
    dre_glbrnd_data.current_view = NULL;

    dre_glbrnd_data.remote_render_switch = DcOff;

    dre_glbrnd_curdevdat = NULL;
    dre_glbrnd_curviwdat = NULL;
    dre_glbrnd_curwindat = NULL;
    dre_glbrnd_curcamdat = NULL;
    dre_glbrnd_curlgtdat = NULL;

    dre_glbrnd_initialize_studio = DcTrue;

    dre_glbrnd_data.studio_method =
	    DDclass_ReplaceMethod("GlbrndStudio", 
				  DcMethodGlbrndIniRender,
				  DcMethodStdRenderStudio);
    /*
     * For Standard Rendering Interface method creation use:
     *   dre_glbrnd_data.studio_method =
     *	 DDclass_CopyMethod(DcMethodStdRenderStudio, 
     *				"GlbrndStudio");
     */   
    for (i=0; dre_glbrnd_studio_method[i].name != 0; i++) {
	if ((type = DsInqClassId(dre_glbrnd_studio_method[i].name)) != -1) {
	    DDclass_SetMethod(type, 
			      dre_glbrnd_data.studio_method,
			      dre_glbrnd_studio_method[i].routine);
	}
    }
    dre_glbrnd_data.display_method = 
	    DDclass_ReplaceMethod("GlbrndDisplay", 
				DcMethodGlbrndIniObjs,
				DcMethodStdRenderDisplay);
    /* 
     * For Standard Rendering Interface method creation use:
     *   dre_glbrnd_data.display_method = 
     *	 DDclass_CopyMethod(DcMethodStdRenderDisplay, 
     *				"GlbrndDisplay");
     */
    for (i=0; dre_glbrnd_display_method[i].name != 0; i++) {
	if ((type = DsInqClassId(dre_glbrnd_display_method[i].name)) != -1) {
	    DDclass_SetMethod(type, 
			       dre_glbrnd_data.display_method,
			       dre_glbrnd_display_method[i].routine);
	}
    }


    dre_glbrnd_data.SpcboxOvr_method =
	    DDclass_ReplaceMethod("GlbrndSpcBoxOvr", 
				DcMethodGlbrndSpcboxOvr, -1);
    /*
     * For Standard Rendering Interface method creation use:
     *   dre_glbrnd_data.SpcboxOvr_method =
     *	 DDclass_AddMethod("GlbrndSpcBoxOvr");
     */
    for (i=0; dre_glbrnd_SpcboxOvr_method[i].name != 0; i++) {
	if ((type = DsInqClassId(dre_glbrnd_SpcboxOvr_method[i].name)) != -1) {
	    DDclass_SetMethod(type, 
			       dre_glbrnd_data.SpcboxOvr_method,
			       dre_glbrnd_SpcboxOvr_method[i].routine);
	}
    }


    dre_glbrnd_data.Rayint_method =
	    DDclass_ReplaceMethod("GlbrndRayint", 
				DcMethodGlbrndRayint, -1);
    /*
     * For Standard Rendering Interface method creation use:
     *   dre_glbrnd_data.Rayint_method =
     *	 DDclass_AddMethod("GlbrndRayint");
     */
    for (i=0; dre_glbrnd_Rayint_method[i].name != 0; i++) {
	if ((type = DsInqClassId(dre_glbrnd_Rayint_method[i].name)) != -1) {
	    DDclass_SetMethod(type, 
			       dre_glbrnd_data.Rayint_method,
			       dre_glbrnd_Rayint_method[i].routine);
	}
    }


    dre_glbrnd_data.UsrDat_method =
	    DDclass_ReplaceMethod("GlbrndUsrdat", 
				DcMethodGlbrndUsrdat, -1);
    /*
     * For Standard Rendering Interface method creation use:
     *   dre_glbrnd_data.UsrDat_method =
     *	 DDclass_AddMethod("GlbrndUsrdat");
     */
    for (i=0; dre_glbrnd_UsrDat_method[i].name != 0; i++) {
	if ((type = DsInqClassId(dre_glbrnd_UsrDat_method[i].name)) != -1) {
	    DDclass_SetMethod(type, 
			       dre_glbrnd_data.UsrDat_method,
			       dre_glbrnd_UsrDat_method[i].routine);
	}
    }


    dre_glbrnd_data.Wcsloc_method =
	    DDclass_ReplaceMethod("GlbrndWcsloc", 
				DcMethodGlbrndWcsloc, -1);
    /*
     * For Standard Rendering Interface method creation use:
     *   dre_glbrnd_data.Wcsloc_method =
     *	 DDclass_AddMethod("GlbrndWcsloc");
     */
    for (i=0; dre_glbrnd_Wcsloc_method[i].name != 0; i++) {
	if ((type = DsInqClassId(dre_glbrnd_Wcsloc_method[i].name)) != -1) {
	    DDclass_SetMethod(type, 
			       dre_glbrnd_data.Wcsloc_method,
			       dre_glbrnd_Wcsloc_method[i].routine);
	}
    }


    dre_glbrnd_data.Wcsnrm_method =
	    DDclass_ReplaceMethod("GlbrndWcsnrm", 
				DcMethodGlbrndWcsnrm, -1);
    /*
     * For Standard Rendering Interface method creation use:
     *   dre_glbrnd_data.Wcsnrm_method =
     *	 DDclass_AddMethod("GlbrndWcsnrm");
     */
    for (i=0; dre_glbrnd_Wcsnrm_method[i].name != 0; i++) {
	if ((type = DsInqClassId(dre_glbrnd_Wcsnrm_method[i].name)) != -1) {
	    DDclass_SetMethod(type, 
			       dre_glbrnd_data.Wcsnrm_method,
			       dre_glbrnd_Wcsnrm_method[i].routine);
	}
    }


    dre_glbrnd_data.WldBnd_method =
	    DDclass_ReplaceMethod("GlbrndWldBnd", 
				DcMethodGlbrndWldBnd, -1);
    /*
     * For Standard Rendering Interface method creation use:
     *   dre_glbrnd_data.WldBnd_method =
     *	 DDclass_AddMethod("GlbrndWldBnd");
     */
    for (i=0; dre_glbrnd_WldBnd_method[i].name != 0; i++) {
	if ((type = DsInqClassId(dre_glbrnd_WldBnd_method[i].name)) != -1) {
	    DDclass_SetMethod(type, 
			       dre_glbrnd_data.WldBnd_method,
			       dre_glbrnd_WldBnd_method[i].routine);
	}
    }

    dre_glbrnd_data.device_index =
	    DDclass_AddObjectData
	    (   DsInqClassId ("DoDevice"),
		(DtMethodPtr) drr_glbrnd_create_device_data,
		(DtMethodPtr) null_routine,
		(DtMethodPtr) drr_glbrnd_delete_device_data
	    );
				 
    dre_glbrnd_data.view_index =
	    DDclass_AddObjectData
	    (   DsInqClassId ("DoView"),
		(DtMethodPtr) drr_glbrnd_create_view_data,
		(DtMethodPtr) null_routine,
		(DtMethodPtr) drr_glbrnd_delete_view_data
	    );

    drr_rayint_initialize_subsystem();

    drr_recrnd_initialize_subsystem();

    drr_glbshd_initialize_subsystem();

    return DDrender_AddRenderer
	   (   renderer_id, "Standard Production Renderer",
	       (DtMethodPtr) drr_glbrnd_render,
	       (DtMethodPtr) null_routine,
	       drr_glbrnd_new_class,
	       (DtMethodPtr) drr_glbrnd_get_wcstofcsmat
	   );
}
/*
 ======================================================================
 */

void drr_glbrnd_new_class (
    char  *class_name,
    DtInt  class_id)
{
    DtInt i;

    for (i=0; dre_glbrnd_studio_method[i].name != 0; i++) {
	if (!strcmp(dre_glbrnd_studio_method[i].name, class_name)) {
	    DDclass_SetMethod(class_id, 
			       dre_glbrnd_data.studio_method,
			       dre_glbrnd_studio_method[i].routine);
	}
    }
    for (i=0; dre_glbrnd_display_method[i].name != 0; i++) {
	if (!strcmp(dre_glbrnd_display_method[i].name, class_name)) {
	    DDclass_SetMethod(class_id, 
			       dre_glbrnd_data.display_method,
			       dre_glbrnd_display_method[i].routine);
	}
    }
    for (i=0; dre_glbrnd_SpcboxOvr_method[i].name != 0; i++) {
	if (!strcmp(dre_glbrnd_SpcboxOvr_method[i].name, class_name)) {
	    DDclass_SetMethod(class_id, 
			       dre_glbrnd_data.SpcboxOvr_method,
			       dre_glbrnd_SpcboxOvr_method[i].routine);
	}
    }
    for (i=0; dre_glbrnd_Rayint_method[i].name != 0; i++) {
	if (!strcmp(dre_glbrnd_Rayint_method[i].name, class_name)) {
	    DDclass_SetMethod(class_id, 
			       dre_glbrnd_data.Rayint_method,
			       dre_glbrnd_Rayint_method[i].routine);
	}
    }
    for (i=0; dre_glbrnd_UsrDat_method[i].name != 0; i++) {
	if (!strcmp(dre_glbrnd_UsrDat_method[i].name, class_name)) {
	    DDclass_SetMethod(class_id, 
			       dre_glbrnd_data.UsrDat_method,
			       dre_glbrnd_UsrDat_method[i].routine);
	}
    }
    for (i=0; dre_glbrnd_Wcsloc_method[i].name != 0; i++) {
	if (!strcmp(dre_glbrnd_Wcsloc_method[i].name, class_name)) {
	    DDclass_SetMethod(class_id, 
			       dre_glbrnd_data.Wcsloc_method,
			       dre_glbrnd_Wcsloc_method[i].routine);
	}
    }
    for (i=0; dre_glbrnd_Wcsnrm_method[i].name != 0; i++) {
	if (!strcmp(dre_glbrnd_Wcsnrm_method[i].name, class_name)) {
	    DDclass_SetMethod(class_id, 
			       dre_glbrnd_data.Wcsnrm_method,
			       dre_glbrnd_Wcsnrm_method[i].routine);
	}
    }
    for (i=0; dre_glbrnd_WldBnd_method[i].name != 0; i++) {
	if (!strcmp(dre_glbrnd_WldBnd_method[i].name, class_name)) {
	    DDclass_SetMethod(class_id, 
			       dre_glbrnd_data.WldBnd_method,
			       dre_glbrnd_WldBnd_method[i].routine);
	}
    }
}
