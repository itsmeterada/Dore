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
 * Association between class names and method routines
 * in arrays "samplernd_studio_method[]" and "samplernd_display_method[]"
 */

extern void drr_samplernd_light_render();
extern void drr_samplernd_camera_render();

extern void drr_samplernd_pntlst_render();
extern void drr_samplernd_varpntlst_render();
extern void drr_samplernd_linlst_render();
extern void drr_samplernd_varlinlst_render();
extern void drr_samplernd_plylin_render();
extern void drr_samplernd_trilst_render();
extern void drr_samplernd_trimsh_render();
extern void drr_samplernd_vartrimsh_render();
extern void drr_samplernd_tristrip_render();
extern void drr_samplernd_vartristrip_render();
extern void drr_samplernd_difint_render();
extern void drr_samplernd_difclr_render();
extern void drr_samplernd_difswi_render();
extern void drr_samplernd_reptyp_render();
extern void drr_samplernd_lcstowcsmat_render();

/*
 * Method routine pointers for studio traversal method
 */

drt_samplernd_methods dre_samplernd_studio_method[] = {
   { "DoLight",			drr_samplernd_light_render },
   { "DoCamera", 		drr_samplernd_camera_render },
   { 0,			      	0 }
};

/*
 * Method routine pointers for display traversal method
 */

drt_samplernd_methods dre_samplernd_display_method[] = {
   { "DoPointList",		drr_samplernd_pntlst_render },
   { "DoVarPointList",		drr_samplernd_varpntlst_render },
   { "DoLineList",		drr_samplernd_linlst_render },
   { "DoVarLineList",		drr_samplernd_varlinlst_render },
   { "DoPolyline", 		drr_samplernd_plylin_render },
   { "DoTriangleList", 		drr_samplernd_trilst_render },
   { "DoTriangleMesh", 		drr_samplernd_trimsh_render },
   { "DoVarTriangleMesh",	drr_samplernd_vartrimsh_render },
   { "DoTriangleStrip",		drr_samplernd_tristrip_render },
   { "DoVarTriangleStrip",	drr_samplernd_vartristrip_render },
   { "AttDiffuseIntens",	drr_samplernd_difint_render },
   { "AttDiffuseColor",		drr_samplernd_difclr_render },
   { "AttDiffuseSwitch",	drr_samplernd_difswi_render },
   { "AttRepType",		drr_samplernd_reptyp_render },
   { "AttLcstowcsmat",		drr_samplernd_lcstowcsmat_render },
   { 0,			      	0 }
};
