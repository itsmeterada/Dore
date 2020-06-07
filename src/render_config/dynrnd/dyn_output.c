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
	void drr_dynrnd_pntlst (object)
	void drr_dynrnd_linlst (object)
	void drr_dynrnd_plylin (object)
	void drr_dynrnd_trilst (object)
	void drr_dynrnd_trimsh (object)
	void drr_dynrnd_tristrip (object)
	void drr_dynrnd_anntxt (object)
	void drr_dynrnd_plymkr (object)
	void drr_dynrnd_varpntlst (object)
	void drr_dynrnd_varlinlst (object)
	void drr_dynrnd_vartrimsh (object)
	void drr_dynrnd_vartristrip (object)

  ======================================================================
 */
#include "dynrnd.h"
#include <dore/internal/system.h>
#include <dore/dore_develop/private/anntxt.h>
#include <dore/dore_develop/private/linlst.h>
#include <dore/dore_develop/private/plylin.h>
#include <dore/dore_develop/private/plymkr.h>
#include <dore/dore_develop/private/pntlst.h>
#include <dore/dore_develop/private/trilst.h>
#include <dore/dore_develop/private/trimsh.h>
#include <dore/dore_develop/private/tristrip.h>
#include <dore/dore_develop/private/varlinlst.h>
#include <dore/dore_develop/private/varpntlst.h>
#include <dore/dore_develop/private/vartrimsh.h>
#include <dore/dore_develop/private/vartristrp.h>

/* This pointer may be used by a drom to set element colors for
   geometry. For example, this is done in some rendering routines
   of the vll drom (required by radiosity for hemicube drawing) */ 

DtRealTriple *dre_dynrnd_drom_element_colors = (void *)DcNullPtr;


/*
 ======================================================================
 */
void drr_dynrnd_pntlst (
    dot_object *object)
{
    struct pntlst *pntlst;
    static DtInt class_id = -1;

    if (class_id == -1) {
	class_id = DsInqClassId("DoPointList");
    }

    if (!DDcondex_QueryRender(class_id))
	    return;

    pntlst = (struct pntlst *)object->data;

    if (pntlst->point_count == 0)
	    return;

    doe_system_timing_points_drawn += pntlst->point_count;

    (*dre_dynrnd_DROM_fcns->render_point_list) (
				DcRGB,
				pntlst->bounding_box_pts,
				pntlst->point_count,
				pntlst->vertex_location,
				pntlst->vertex_normal,
				pntlst->vertex_color,
				pntlst->uv_count,
				pntlst->uv_list,
				pntlst->uvw_count,
				pntlst->uvw_list,
				pntlst->vertex_alpha);
}
/*
 ======================================================================
 */
void drr_dynrnd_linlst (
    dot_object *object)
{
    struct linlst *linlst;
    static DtInt class_id = -1;

    if (class_id == -1) {
	class_id = DsInqClassId("DoLineList");
    }

    if (!DDcondex_QueryRender(class_id))
	    return;

    linlst = (struct linlst *)object->data;

    if (linlst->line_count == 0) {
	return;
    }

    doe_system_timing_lines_drawn += linlst->line_count;

    (*dre_dynrnd_DROM_fcns->render_line_list)(
				DcRGB,
				linlst->bounding_box_pts,
				linlst->line_count,
				DcNullPtr,
				linlst->vertex_location,
				linlst->vertex_normal,
				linlst->vertex_color,
				linlst->uv_count,
				linlst->uv_list,
				linlst->uvw_count,
				linlst->uvw_list,
				linlst->vertex_alpha);
}
/*
 ======================================================================
 */
void drr_dynrnd_plylin (
    dot_object *object)
{
    struct plylin *plylin;
    static DtInt class_id = -1;

    if (class_id == -1) {
	class_id = DsInqClassId("DoPolyline");
    }

    if (!DDcondex_QueryRender(class_id))
	    return;

    plylin = (struct plylin *)object->data;

    if (plylin->vertex_count < 2) {
	return;
    }

    doe_system_timing_connected_lines_drawn += (plylin->vertex_count - 1);

    (*dre_dynrnd_DROM_fcns->render_connected_line_list) (
					DcRGB,
					plylin->bounding_box_pts,
					plylin->vertex_count,
					DcNullPtr,
					plylin->vertex_location,
					plylin->vertex_normal,
					plylin->vertex_color,
					plylin->uv_count,
					plylin->uv_list,
					plylin->uvw_count,
					plylin->uvw_list,
					plylin->vertex_alpha);
}
/*
 ======================================================================
 */
void drr_dynrnd_trilst (
    dot_object *object)
{
    struct trilst *trilst;
    static DtInt class_id = -1;
    DtMethodPtr routine;
    dot_object *alt;
    static DtMethodPtr reptype_query = (DtMethodPtr)0;
    DtRepType reptype;

    if (class_id == -1) {
	class_id = DsInqClassId("DoTriangleList");
    }

    if (!DDcondex_QueryRender(class_id))
	    return;

    trilst = (struct trilst *)object->data;

    if (dre_dynrnd_DROM_fcns->use_separate_wireframe) {
	if (reptype_query == (DtMethodPtr)0) {
	    reptype_query = DDclass_InqMethod (DsInqClassId("AttRepType"),
					       DcMethodInqGlbAttVal);
	}
	(*reptype_query)(&reptype);
	if (reptype == DcWireframe || reptype == DcOutlines) {
	    routine = DDobject_InqMethod (object, DcMethodUpdStdAltLineObj);
	    alt = (dot_object *) (*routine)(object);
	    if (alt != DcNullPtr) {
		DDobject_InqCurrentMethod(alt)(alt);
	    }
	    return;
	}
    }

    doe_system_timing_triangles_drawn += trilst->triangle_count;

    (*dre_dynrnd_DROM_fcns->render_triangle_list) (
					DcRGB,
					trilst->bounding_box_pts,
					trilst->triangle_count,
					trilst->element_normal,
					trilst->vertex_location,
					trilst->vertex_normal,
					trilst->vertex_color,
					trilst->uv_count,
					trilst->uv_list,
					trilst->uvw_count,
					trilst->uvw_list,
					trilst->vertex_alpha);
}
/*
 ======================================================================
 */
void drr_dynrnd_trimsh (
    dot_object *object)
{
    DtInt triangleno;
    struct trimsh *trimsh;
    static DtInt class_id = -1;
    DtMethodPtr routine;
    dot_object *alt;
    static DtMethodPtr reptype_query = (DtMethodPtr)0;
    DtRepType reptype;

    if (class_id == -1) {
	class_id = DsInqClassId("DoTriangleMesh");
    }

    if (!DDcondex_QueryRender(class_id))
	    return;

    trimsh = (struct trimsh *)object->data;

    if (dre_dynrnd_DROM_fcns->use_separate_wireframe) {
	if (reptype_query == (DtMethodPtr)0) {
	    reptype_query = 
		DDclass_InqMethod (DsInqClassId("AttRepType"),
				   DcMethodInqGlbAttVal);
	}
	(*reptype_query)(&reptype);
	if (reptype == DcWireframe || reptype == DcOutlines) {
	    routine = 
		DDobject_InqMethod (object, DcMethodUpdStdAltLineObj);
	    alt = (dot_object *) (*routine)(object);
	    if (alt != DcNullPtr) {
		DDobject_InqCurrentMethod(alt)(alt);
	    }
	    return;
	}
    }

    if (trimsh->compiled_triangle_vertexlist == NULL) {
	trimsh->compiled_triangle_vertexlist = 
			(trimsh_vertexlist *)DDspace_Allocate
		(trimsh->triangle_count*(sizeof (trimsh_vertexlist)));

	for (triangleno=0;triangleno<trimsh->triangle_count;triangleno++) {
	    trimsh->compiled_triangle_vertexlist[triangleno][0] =
		    trimsh->triangle_vertexlist[triangleno][0] * 3;
	    trimsh->compiled_triangle_vertexlist[triangleno][1] =
		    trimsh->triangle_vertexlist[triangleno][1] * 3;
	    trimsh->compiled_triangle_vertexlist[triangleno][2] =
		    trimsh->triangle_vertexlist[triangleno][2] * 3;
	}
    }

    doe_system_timing_mesh_triangles_drawn += trimsh->triangle_count;

    (*dre_dynrnd_DROM_fcns->render_triangle_mesh) (
				DcRGB,
				trimsh->bounding_box_pts,
				trimsh->triangle_count,
				trimsh->element_normal,
				trimsh->vertex_count,
				trimsh->vertex_location,
				trimsh->vertex_normal,
				trimsh->vertex_color,
				trimsh->triangle_vertexlist,
				trimsh->compiled_triangle_vertexlist,
				trimsh->uv_count,
				trimsh->uv_list,
				trimsh->uvw_count,
				trimsh->uvw_list,
				trimsh->vertex_alpha);
}
/*
 ======================================================================
 */
void drr_dynrnd_tristrip (
    dot_object *object)
{
    struct tristrip *tristrip;
    static DtInt class_id = -1;
    DtMethodPtr routine;
    dot_object *alt;
    static DtMethodPtr reptype_query = (DtMethodPtr)0;
    DtRepType reptype;
    DtInt odd;
    DtInt basevtx;

    if (class_id == -1) {
	class_id = DsInqClassId("DoTriangleStrip");
    }

    if (!DDcondex_QueryRender(class_id))
	    return;

    tristrip = (struct tristrip *)(object->data);

    if (dre_dynrnd_DROM_fcns->use_separate_wireframe) {
	if (reptype_query == (DtMethodPtr)0) {
	    reptype_query = 
			DDclass_InqMethod (DsInqClassId("AttRepType"),
					DcMethodInqGlbAttVal);
	}
	(*reptype_query)(&reptype);
	if (reptype == DcWireframe || reptype == DcOutlines) {
	    routine = 
		DDobject_InqMethod (object, DcMethodUpdStdAltLineObj);
	    alt = (dot_object *) (*routine)(object);
	    if (alt != DcNullPtr) {
		DDobject_InqCurrentMethod(alt)(alt);
	    }
	    return;
	}
    }

    if (tristrip->triangle_vertexlist == NULL) {
	tristrip->triangle_vertexlist = (DtIntTriple *)
		DDspace_Allocate((tristrip->vertex_count-2)*
				 (sizeof (DtIntTriple)));
	for (basevtx=0, odd=DcTrue; basevtx<tristrip->vertex_count-2; basevtx++){
	    if (odd) {
		odd = DcFalse;
		tristrip->triangle_vertexlist[basevtx][0] = basevtx;
		tristrip->triangle_vertexlist[basevtx][1] = basevtx +1;
		tristrip->triangle_vertexlist[basevtx][2] = basevtx +2;
	    } else {
		odd = DcTrue;
		tristrip->triangle_vertexlist[basevtx][0] = basevtx;
		tristrip->triangle_vertexlist[basevtx][1] = basevtx +2;
		tristrip->triangle_vertexlist[basevtx][2] = basevtx +1;
	    }
	}
    }

    if (tristrip->compiled_triangle_vertexlist == NULL) {
	tristrip->compiled_triangle_vertexlist =  (DtIntTriple *)
		DDspace_Allocate ((tristrip->vertex_count-2)*
				  (sizeof (DtIntTriple)));
	for (basevtx=0;basevtx<tristrip->vertex_count-2;basevtx++) {
	    tristrip->compiled_triangle_vertexlist[basevtx][0] =
		    tristrip->triangle_vertexlist[basevtx][0] * 3;
	    tristrip->compiled_triangle_vertexlist[basevtx][1] =
		    tristrip->triangle_vertexlist[basevtx][1] * 3;
	    tristrip->compiled_triangle_vertexlist[basevtx][2] =
		    tristrip->triangle_vertexlist[basevtx][2] * 3;
	}
    }
	 
    doe_system_timing_mesh_triangles_drawn += tristrip->vertex_count - 2;

    (*dre_dynrnd_DROM_fcns->render_triangle_mesh)(
				DcRGB,
				tristrip->bounding_box_pts,
				tristrip->vertex_count - 2,
				tristrip->element_normal,
				tristrip->vertex_count,
				tristrip->vertex_location,
				tristrip->vertex_normal,
				tristrip->vertex_color,
				tristrip->triangle_vertexlist,
				tristrip->compiled_triangle_vertexlist,
				tristrip->uv_count,
				tristrip->uv_list,
				tristrip->uvw_count,
				tristrip->uvw_list,
				tristrip->vertex_alpha);
}
/*
 ======================================================================
 */
void drr_dynrnd_anntxt (
    dot_object *object)
{
    struct anntxt *anntxt;
    static DtInt class_id = -1;
    dot_object *alt;
    DtMatrix4x4 tfmmatrix;
    DtMethodPtr routine;

    if (class_id == -1) {
	class_id = DsInqClassId("DoAnnoText");
    }

    if (!DDcondex_QueryRender(class_id))
	    return;

    anntxt = (struct anntxt *)object->data;

    if ((DtFlag)(*dre_dynrnd_DROM_fcns->transform_clip_z_point) 
	(anntxt->position,tfmmatrix)) {
	return;
    }

    (*dre_dynrnd_DROM_fcns->push_lcstofcsmat) (tfmmatrix);

    routine = DDobject_InqMethod(object, DcMethodUpdStdAltObj);
    alt = (dot_object *)(*routine)(object);

    if (alt != DcNullPtr) {
	routine = DDobject_InqCurrentMethod(alt);
	(*routine)(alt);
    }

    (*dre_dynrnd_DROM_fcns->pop_lcstofcsmat)();
}

/*
 ======================================================================
 */
void drr_dynrnd_plymkr (
    dot_object *object)
{
    struct plymkr *plymkr;
    static DtInt class_id = -1;
    DtInt markerno;
    dot_object *alt;
    DtMatrix4x4 tfmmatrix;
    DtMethodPtr routine;

    if (class_id == -1) {
	class_id = DsInqClassId("DoPolymarker");
    }

    if (!DDcondex_QueryRender(class_id))
	    return;

    plymkr = (struct plymkr *)object->data;

    routine = DDobject_InqMethod(object, DcMethodUpdStdAltObj);
    alt = (dot_object *)(*routine)(object);

    for (markerno=0;markerno<plymkr->count;markerno++) {
	if ((DtFlag)(*dre_dynrnd_DROM_fcns->transform_clip_z_point) 
	    (plymkr->position[markerno],tfmmatrix)) {
	    continue;
	}

	(*dre_dynrnd_DROM_fcns->push_lcstofcsmat) (tfmmatrix);

	if (alt != DcNullPtr) {
	    DDobject_InqCurrentMethod(alt) (alt);
	}

	(*dre_dynrnd_DROM_fcns->pop_lcstofcsmat)();
    }
}
/*
 ======================================================================
 */
void drr_dynrnd_varpntlst (
    dot_object *object)
{
    struct varpntlst *varpntlst;
    static DtInt class_id = -1;

    if (class_id == -1) {
	class_id = DsInqClassId("DoVarPointList");
    }

    if (!DDcondex_QueryRender(class_id))
	    return;

    varpntlst = (struct varpntlst *)object->data;

    if (varpntlst->point_count == 0)
	    return;

    doe_system_timing_points_drawn += varpntlst->point_count;

    (*dre_dynrnd_DROM_fcns->render_point_list) (
				DcRGB,
				varpntlst->bounding_box_pts,
				varpntlst->point_count,
				varpntlst->vertex_location,
				varpntlst->vertex_normal,
				varpntlst->vertex_color,
				0,
				DcNullPtr,
				0,
				DcNullPtr,
				DcNullPtr);
}
/*
 ======================================================================
 */
void drr_dynrnd_varlinlst (
    dot_object *object)
{
    struct varlinlst *varlinlst;
    static DtInt class_id = -1;

    if (class_id == -1) {
	class_id = DsInqClassId("DoVarLineList");
    }

    if (!DDcondex_QueryRender(class_id))
	    return;

    varlinlst = (struct varlinlst *)object->data;

    if (varlinlst->line_count == 0) {
	return;
    }

    doe_system_timing_lines_drawn += varlinlst->line_count;

    (*dre_dynrnd_DROM_fcns->render_line_list)(
				DcRGB,
				varlinlst->bounding_box_pts,
				varlinlst->line_count,
				DcNullPtr,
				varlinlst->vertex_location,
				varlinlst->vertex_normal,
				varlinlst->vertex_color,
				0,
				DcNullPtr,
				0,
				DcNullPtr,
				DcNullPtr);
}
/*
 ======================================================================
 */
void drr_dynrnd_vartrimsh (
    dot_object *object)
{
    DtInt triangleno;
    struct vartrimsh *vartrimsh;
    static DtInt class_id = -1;
    DtObject shader;
    DtInterpType interptype;
    DtMethodPtr routine;

    if (class_id == -1) {
	class_id = DsInqClassId("DoVarTriangleMesh");
    }

    if (!DDcondex_QueryRender(class_id))
	    return;

    vartrimsh = (struct vartrimsh *)object->data;

    if (vartrimsh->compiled_triangle_vertexlist == NULL) {
	vartrimsh->compiled_triangle_vertexlist =
		(vartrimsh_vertexlist *)DDspace_Allocate
			(vartrimsh->triangle_count*(sizeof (vartrimsh_vertexlist)));

	for (triangleno=0; triangleno<vartrimsh->triangle_count; 
	     triangleno++){
	    vartrimsh->compiled_triangle_vertexlist[triangleno][0] =
		    vartrimsh->triangle_vertexlist[triangleno][0] * 3;
	    vartrimsh->compiled_triangle_vertexlist[triangleno][1] =
		    vartrimsh->triangle_vertexlist[triangleno][1] * 3;
	    vartrimsh->compiled_triangle_vertexlist[triangleno][2] =
		    vartrimsh->triangle_vertexlist[triangleno][2] * 3;
	}
    }

    routine = DDclass_InqMethod(DcTypeAttSurfaceShader, DcMethodInqGlbAttVal);
    (*routine) (&shader);

    if (shader != DcShaderConstant) {
	if (vartrimsh->needelemnrms) {
	    if (vartrimsh->element_normal == NULL) {
		vartrimsh->element_normal = 
			(DtRealTriple *)DDspace_Allocate
				(vartrimsh->triangle_count*(sizeof (DtRealTriple)));
	    }
	    dor_vartrimsh_calculate_triangle_normals(vartrimsh);
	    vartrimsh->needelemnrms = DcFalse;
	}

	routine = DDclass_InqMethod(DcTypeAttInterpType, 
				    DcMethodInqGlbAttVal);
	(*routine)(&interptype);

	if ((interptype == DcVertexShade) && vartrimsh->needvertnrms) {
	    if (vartrimsh->vertex_normal == NULL) {
		vartrimsh->vertex_normal =
			(DtRealTriple *)DDspace_Allocate
				(vartrimsh->vertex_count*(sizeof (DtRealTriple)));
	    }
	    dor_vartrimsh_calculate_vertex_normals(vartrimsh);
	    vartrimsh->needvertnrms = DcFalse;
	}
    }

    doe_system_timing_mesh_triangles_drawn += vartrimsh->triangle_count;

    (*dre_dynrnd_DROM_fcns->render_triangle_mesh) (
				DcRGB,
				vartrimsh->bounding_box_pts,
			  	vartrimsh->triangle_count,
				vartrimsh->element_normal,
				vartrimsh->vertex_count,
				vartrimsh->vertex_location,
				vartrimsh->vertex_normal,
				vartrimsh->vertex_color,
				vartrimsh->triangle_vertexlist,
				vartrimsh->compiled_triangle_vertexlist,
				vartrimsh->uv_count,
				vartrimsh->uv_list,
				vartrimsh->uvw_count,
				vartrimsh->uvw_list,
				DcNullPtr);
}
/*
 ======================================================================
 */
void drr_dynrnd_vartristrip (
    dot_object *object)
{
    struct vartristrip *vartristrip;
    static DtInt class_id = -1;
    DtMethodPtr routine;
    dot_object *alt;
    static DtMethodPtr reptype_query = (DtMethodPtr)0;
    DtRepType reptype;
    DtInt odd;
    DtInt basevtx;

    if (class_id == -1) {
	class_id = DsInqClassId("DoVarTriangleStrip");
    }

    if (!DDcondex_QueryRender(class_id))
	    return;

    vartristrip = (struct vartristrip *)(object->data);

    if (dre_dynrnd_DROM_fcns->use_separate_wireframe) {
	if (reptype_query == (DtMethodPtr)0) {
	    reptype_query = 
		    DDclass_InqMethod (DsInqClassId("AttRepType"),
				       DcMethodInqGlbAttVal);
	}
	(*reptype_query)(&reptype);
	if (reptype == DcWireframe || reptype == DcOutlines) {
	    routine = DDobject_InqMethod (object, 
					  DcMethodUpdStdAltLineObj);
	    alt = (dot_object *) (*routine)(object);
	    if (alt != DcNullPtr) {
		DDobject_InqCurrentMethod(alt)(alt);
	    }
	    return;
	}
    }

    odd = DcTrue;

    if (vartristrip->triangle_vertexlist == NULL) {
	vartristrip->triangle_vertexlist = (DtIntTriple *)
		DDspace_Allocate((vartristrip->vertex_count-2)*
				 (sizeof (DtIntTriple)));
	for (basevtx=0; basevtx<vartristrip->vertex_count-2; basevtx++){
	    if (odd) {
		odd = DcFalse;
		vartristrip->triangle_vertexlist[basevtx][0] = basevtx;
		vartristrip->triangle_vertexlist[basevtx][1] = basevtx +1;
		vartristrip->triangle_vertexlist[basevtx][2] = basevtx +2;
	    } else {
		odd = DcTrue;
		vartristrip->triangle_vertexlist[basevtx][0] = basevtx;
		vartristrip->triangle_vertexlist[basevtx][1] = basevtx +2;
		vartristrip->triangle_vertexlist[basevtx][2] = basevtx +1;
	    }
	}
    }

    if (vartristrip->compiled_triangle_vertexlist == NULL) {
	vartristrip->compiled_triangle_vertexlist =  (DtIntTriple *)
		DDspace_Allocate ((vartristrip->vertex_count-2)*
				  (sizeof (DtIntTriple)));
	for (basevtx=0;basevtx<vartristrip->vertex_count-2;basevtx++) {
	    vartristrip->compiled_triangle_vertexlist[basevtx][0] =
		    vartristrip->triangle_vertexlist[basevtx][0] * 3;
	    vartristrip->compiled_triangle_vertexlist[basevtx][1] =
		    vartristrip->triangle_vertexlist[basevtx][1] * 3;
	    vartristrip->compiled_triangle_vertexlist[basevtx][2] =
		    vartristrip->triangle_vertexlist[basevtx][2] * 3;
	}
    }

    if (vartristrip->needelemnrms) {
	if (vartristrip->element_normal == NULL) {
	    vartristrip->element_normal = 
		    (DtRealTriple *)DDspace_Allocate
			    ((vartristrip->vertex_count-2)*(sizeof (DtRealTriple)));
	}
	dor_vartristrip_calculate_triangle_normals(vartristrip);
	vartristrip->needelemnrms = DcFalse;
    }


    doe_system_timing_mesh_triangles_drawn += vartristrip->vertex_count - 2;

    (*dre_dynrnd_DROM_fcns->render_triangle_mesh)(
				DcRGB,
				vartristrip->bounding_box_pts,
				vartristrip->vertex_count - 2,
				vartristrip->element_normal,
				vartristrip->vertex_count,
				vartristrip->vertex_location,
				vartristrip->vertex_normal,
				vartristrip->vertex_color,
				vartristrip->triangle_vertexlist,
				vartristrip->compiled_triangle_vertexlist,
				vartristrip->uv_count,
				vartristrip->uv_list,
				vartristrip->uvw_count,
				vartristrip->uvw_list,
				DcNullPtr);
}
/*
 ======================================================================
 */
