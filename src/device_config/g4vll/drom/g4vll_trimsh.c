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
	ddt_g4vll_drom_trimsh * ddr_g4vll_drom_trimsh_create (object)
	void ddr_g4vll_drom_trimsh_print (object, data)
	void ddr_g4vll_drom_trimsh_delete (object, data)
	void ddr_g4vll_drom_trimsh_render (object)
	dot_object *ddr_g4vll_drom_trimsh_create_alternate_tristrip(object) 

  ======================================================================
 */
#include "drom.h"
#include <dore/internal/system.h>
#include <dore/dore_develop/private/trimsh.h>
#include <dore/dore_develop/private/tristrip.h>

/* ====================================================================== */

static DtInt classid = -1;
static DtInt dataindex = -1;

/* this will be set by radiosity if necessary */
extern DtFlag dpe_radiosity_ok_to_use_tristrip_alternate_rep;

/* ====================================================================== */

ddt_g4vll_drom_trimsh *ddr_g4vll_drom_trimsh_create (
    DtObjectStructure *object)
{
    struct trimsh *trimsh;
    ddt_g4vll_drom_trimsh *drom_trimsh;
    int i;

    trimsh = (struct trimsh *)(object->data);

    drom_trimsh =  (ddt_g4vll_drom_trimsh *)
	    DDspace_Allocate(sizeof(ddt_g4vll_drom_trimsh));

    drom_trimsh->uvvertexlist = (DtInt *)0;

    if (trimsh->uv_count > 0) {
        drom_trimsh->bboxuv = (ddt_g4vll_drom_bboxuv *)DDspace_Allocate
                (trimsh->uv_count *sizeof(ddt_g4vll_drom_bboxuv));

        for (i=0; i< trimsh->uv_count; i++) {
            drom_trimsh->bboxuv[i].initialized = DcFalse;
        }
    } else {
        drom_trimsh->bboxuv = (ddt_g4vll_drom_bboxuv *)0;
    }

    if (trimsh->uvw_count > 0) {
        drom_trimsh->bboxuvw = (ddt_g4vll_drom_bboxuvw *)DDspace_Allocate
                (trimsh->uvw_count *sizeof(ddt_g4vll_drom_bboxuvw));

        for (i=0; i< trimsh->uvw_count; i++) {
            drom_trimsh->bboxuvw[i].initialized = DcFalse;
        }
    } else {
        drom_trimsh->bboxuvw = (ddt_g4vll_drom_bboxuvw *)0;
    }

    drom_trimsh->grid_check	 = DcFalse;

    drom_trimsh->altobj_tristrip = (DtObjectStructure *)0;

    return drom_trimsh;
}

/* ====================================================================== */

void ddr_g4vll_drom_trimsh_print (
    DtObject object,
    DtInt *data)
{
}

/* ====================================================================== */

void ddr_g4vll_drom_trimsh_delete (
    DtObject object,
    ddt_g4vll_drom_trimsh *data)
{
    if (data != (ddt_g4vll_drom_trimsh *)0) {
	if (data->uvvertexlist != (DtInt *)0) {
	    DDspace_Deallocate(data->uvvertexlist);
	}
	if (data->bboxuv != (ddt_g4vll_drom_bboxuv *)0) {
            DDspace_Deallocate(data->bboxuv);
        }
        if (data->bboxuvw != (ddt_g4vll_drom_bboxuvw *)0) {
            DDspace_Deallocate(data->bboxuvw);
        }
	if (data->altobj_tristrip != (DtObjectStructure *)0) {
	    DeDeleteObject(data->altobj_tristrip);
	}

	DDspace_Deallocate(data);
    }
}

/* ====================================================================== */

void ddr_g4vll_drom_trimsh_render (
    dot_object *object)
{
    extern DtInt *dde_g4vll_triangle_mesh_uvvertexlist;
    extern DtRealCouple *dde_g4vll_triangle_mesh_bboxuv;
    extern DtRealTriple *dde_g4vll_triangle_mesh_bboxuvw;
    ddt_g4vll_drom_bboxuv *bboxuv;
    ddt_g4vll_drom_bboxuvw *bboxuvw;
    struct trimsh *trimsh;
    DtIntTriple *uvvertexlist;
    ddt_g4vll_drom_trimsh *drom_trimsh;
    DtInt triangleno;
    DtInt i;
    DtInt idx;
    DtInt grid_dimensions[2];
    
    if (classid == -1) 
	classid = DsInqClassId("DoTriangleMesh");

    if (dataindex == -1) {
	for (i=0; dde_g4vll_drom_class_data[i].name != 0; i++) {
	    if (dde_g4vll_drom_class_data[i].id == classid) {
		dataindex = dde_g4vll_drom_class_data[i].index;
		break;
	    }
	}
    }

    if (!DDcondex_QueryRender(classid))
	    return;

    trimsh = (struct trimsh *)(object->data);

    drom_trimsh = (ddt_g4vll_drom_trimsh *)object->additional_data[dataindex];

    /*
     * Determine whether the mesh is, in fact, a gridmesh (check only once
     * per trimesh object).  If it is, construct a tristrip alternate object.  
     */
    if (!drom_trimsh->grid_check) {
	drom_trimsh->grid_check = DcTrue;

    	if (ddr_g4vll_drom_gridmesh_to_strip
	    (   trimsh->vertex_count, trimsh->triangle_count, 
		trimsh->triangle_vertexlist,
		&(drom_trimsh->tristrip_vertexcount), 
		&(drom_trimsh->tristrip_indices), grid_dimensions)) {

	    /*
	     * The trimesh's alternate object should be a triangle strip.
	     */

#           ifdef DEBUG
		printf ("TRIMSH: creating tristrip alternate object\n");
#           endif

	    drom_trimsh->altobj_tristrip = 
		    ddr_g4vll_drom_trimsh_create_alternate_tristrip(object); 
	}
    }

    if (drom_trimsh->altobj_tristrip != (DtObject)0 &&
        dpe_radiosity_ok_to_use_tristrip_alternate_rep) {
        /* if radiosity is used, we stick to the mesh representation */
        DDobject_InqCurrentMethod(drom_trimsh->altobj_tristrip)
            (drom_trimsh->altobj_tristrip);

    } else {
    	/*
     	 * Store the trimsh additional data globally for its output routine.
     	 */
	dde_g4vll_drom_trimsh = drom_trimsh;

	uvvertexlist = (DtIntTriple *) drom_trimsh->uvvertexlist;

	if (uvvertexlist == NULL && trimsh->uv_count > 0) {
	    uvvertexlist = (DtIntTriple *)DDspace_Allocate
		(trimsh->triangle_count*(sizeof (DtIntTriple)));

	    for (triangleno=0;triangleno<trimsh->triangle_count;triangleno++) {
	    	uvvertexlist[triangleno][0] =
		    trimsh->triangle_vertexlist[triangleno][0] * 2;
	    	uvvertexlist[triangleno][1] =
		    trimsh->triangle_vertexlist[triangleno][1] * 2;
	    	uvvertexlist[triangleno][2] =
		    trimsh->triangle_vertexlist[triangleno][2] * 2;
	    }
	    drom_trimsh->uvvertexlist = (DtInt*) uvvertexlist;
    	}

	if (trimsh->uv_count > 0 && dde_g4vll_drom_data.texture_2D_valid) {
	    idx = trimsh->uv_count > dde_g4vll_drom_data.map_diffuse_color_uv_index ?
		dde_g4vll_drom_data.map_diffuse_color_uv_index : 0;

	    bboxuv = &(drom_trimsh->bboxuv[idx]);

	    if (!bboxuv->initialized) {
	    	DtReal umin, umax, vmin, vmax;
	    	/*
	     	 * This bounding box has not been set yet so compute it.
	     	 */
	    	umin = umax = trimsh->uv_list[idx][0][0];
	    	vmin = vmax = trimsh->uv_list[idx][0][1];

/* CHANGE THIS CODE SO THAT LIST IS FROM ACTUAL VERTICES ONLY!!!!!! */

	    	for (i=1; i< trimsh->vertex_count; i++) {
		    umin = min(umin, trimsh->uv_list[idx][i][0]);
		    umax = max(umax, trimsh->uv_list[idx][i][0]);
		    vmin = min(vmin, trimsh->uv_list[idx][i][1]);
		    vmax = max(vmax, trimsh->uv_list[idx][i][1]);
	    	}

	    	bboxuv->bbox[0][0] = umin;
	    	bboxuv->bbox[0][1] = vmin;
	    
	    	bboxuv->bbox[1][0] = umin;
	    	bboxuv->bbox[1][1] = vmax;
	    
	    	bboxuv->bbox[2][0] = umax;
	    	bboxuv->bbox[2][1] = vmax;
	    
	    	bboxuv->bbox[3][0] = umax;
	    	bboxuv->bbox[3][1] = vmin;

	    	bboxuv->initialized = DcTrue;
	    	}
	    dde_g4vll_triangle_mesh_bboxuv = bboxuv->bbox;
	}

        if (trimsh->uvw_count > 0 && dde_g4vll_drom_data.texture_3D_valid) {
            idx = trimsh->uvw_count > dde_g4vll_drom_data.map_diffuse_color_uvw_index ?
                    dde_g4vll_drom_data.map_diffuse_color_uvw_index : 0;
    
            bboxuvw = &(drom_trimsh->bboxuvw[idx]);
    
            if (!bboxuvw->initialized) {
                DtReal umin, umax, vmin, vmax, wmin, wmax;
                /*
                 * this bounding box has not been set yet so compute bounding
                 * box
                 */
    
                umin = umax = trimsh->uvw_list[idx][0][0];
                vmin = vmax = trimsh->uvw_list[idx][0][1];
                wmin = wmax = trimsh->uvw_list[idx][0][2];
    
                for (i=1; i< trimsh->vertex_count; i++) {
                    umin = min(umin, trimsh->uvw_list[idx][i][0]);
                    umax = max(umax, trimsh->uvw_list[idx][i][0]);
                    vmin = min(vmin, trimsh->uvw_list[idx][i][1]);
                    vmax = max(vmax, trimsh->uvw_list[idx][i][1]);
                    wmin = min(wmin, trimsh->uvw_list[idx][i][2]);
                    wmax = max(wmax, trimsh->uvw_list[idx][i][2]);
                }
    
                bboxuvw->bbox[0][0] = umin;
                bboxuvw->bbox[0][1] = vmin;
                bboxuvw->bbox[0][2] = wmin;
    
                bboxuvw->bbox[1][0] = umin;
                bboxuvw->bbox[1][1] = vmax;
                bboxuvw->bbox[1][2] = wmin;
    
                bboxuvw->bbox[2][0] = umax;
                bboxuvw->bbox[2][1] = vmax;
                bboxuvw->bbox[2][2] = wmin;
    
                bboxuvw->bbox[3][0] = umax;
                bboxuvw->bbox[3][1] = vmin;
                bboxuvw->bbox[3][2] = wmin;
    
                bboxuvw->bbox[4][0] = umin;
                bboxuvw->bbox[4][1] = vmin;
                bboxuvw->bbox[4][2] = wmax;
    
                bboxuvw->bbox[5][0] = umin;
                bboxuvw->bbox[5][1] = vmax;
                bboxuvw->bbox[5][2] = wmax;
    
                bboxuvw->bbox[6][0] = umax;
                bboxuvw->bbox[6][1] = vmax;
                bboxuvw->bbox[6][2] = wmax;
    
                bboxuvw->bbox[7][0] = umax;
                bboxuvw->bbox[7][1] = vmin;
                bboxuvw->bbox[7][2] = wmax;

	    	bboxuvw->initialized = DcTrue;
            }
            dde_g4vll_triangle_mesh_bboxuvw = bboxuvw->bbox;
        }
    
    	if (trimsh->compiled_triangle_vertexlist == NULL) {
	    trimsh->compiled_triangle_vertexlist = (DtIntTriple *)DDspace_Allocate
		(trimsh->triangle_count*(sizeof (DtIntTriple)));

	    for (triangleno=0;triangleno<trimsh->triangle_count;triangleno++) {
	    	trimsh->compiled_triangle_vertexlist[triangleno][0] =
		    trimsh->triangle_vertexlist[triangleno][0] * 3;
	    	trimsh->compiled_triangle_vertexlist[triangleno][1] =
		    trimsh->triangle_vertexlist[triangleno][1] * 3;
	    	trimsh->compiled_triangle_vertexlist[triangleno][2] =
		    trimsh->triangle_vertexlist[triangleno][2] * 3;
	    }
    	}

        if (doe_system_timing_display_traverse_count == 1)
    	    doe_system_timing_mesh_triangles_drawn += trimsh->triangle_count;

    	dde_g4vll_triangle_mesh_uvvertexlist = (DtInt *)uvvertexlist;

    	ddr_g4vll_drom_render_triangle_mesh (DcRGB,
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

    	dde_g4vll_triangle_mesh_uvvertexlist = (DtInt *)0;
	dde_g4vll_drom_trimsh = (ddt_g4vll_drom_trimsh *)0;
    }
}

/* ====================================================================== */

dot_object *ddr_g4vll_drom_trimsh_create_alternate_tristrip (
    dot_object *object)
{
    struct trimsh *trimsh;
    dot_object *tristripobj;
    ddt_g4vll_drom_trimsh *drom_trimsh;
    DtVertexType vertextype;
    DtInt vertexsize;
    DtInt vertexno, index;
    DtUInt uv,uvw;
    DtReal *vertices, *vptr;

    trimsh = (struct trimsh *)(object->data);

    drom_trimsh = (ddt_g4vll_drom_trimsh *)object->additional_data[dataindex];

    vertextype = trimsh->vertextype;
    if (trimsh->smoothflag) {
            /* add normals to vertex type */
            vertextype |= DcLocNrm;
    }

    vertexsize = 3;
    if (vertextype & DcLocNrm)
        vertexsize += 3;
    if (vertextype & DcLocClr)
        vertexsize += 3;
    if (vertextype & DcLocAlpha)
        vertexsize += 3;

    vertexsize = vertexsize + trimsh->uv_count*2 + trimsh->uvw_count*3;

    vertices = (DtReal *)dor_space_allocate(
		drom_trimsh->tristrip_vertexcount*vertexsize*(sizeof(DtReal)));

    vptr = vertices;

    for (vertexno=0; vertexno<drom_trimsh->tristrip_vertexcount; vertexno++) {
    	/* 
     	 * The tristrip is defined by the tristrip_indices into the trimsh.
     	 */
	index = drom_trimsh->tristrip_indices[vertexno];
	*vptr++ = trimsh->vertex_location[index][0];
        *vptr++ = trimsh->vertex_location[index][1];
        *vptr++ = trimsh->vertex_location[index][2];

        if (vertextype & DcLocNrm) {
            *vptr++ = trimsh->vertex_normal[index][0];
            *vptr++ = trimsh->vertex_normal[index][1];
            *vptr++ = trimsh->vertex_normal[index][2];
        }
        if (vertextype & DcLocClr) {
            *vptr++ = trimsh->vertex_color[index][0];
            *vptr++ = trimsh->vertex_color[index][1];
            *vptr++ = trimsh->vertex_color[index][2];
        }
        if (vertextype & DcLocAlpha) {
            *vptr++ = trimsh->vertex_alpha[index];
        }
        for (uv=0; uv<trimsh->uv_count; uv++) {
            *vptr++ = trimsh->uv_list[uv][index][0];
            *vptr++ = trimsh->uv_list[uv][index][1];
        }
        for (uvw=0; uvw<trimsh->uvw_count; uvw++) {
            *vptr++ = trimsh->uvw_list[uvw][index][0];
            *vptr++ = trimsh->uvw_list[uvw][index][1];
            *vptr++ = trimsh->uvw_list[uvw][index][2];
        }
    }

    tristripobj = dor_tristrip_create(trimsh->colormodel,
	vertextype | dor_system_texture_uv_count(trimsh->uv_count) |
	dor_system_texture_uvw_count(trimsh->uvw_count),
	drom_trimsh->tristrip_vertexcount, vertices);

    dor_space_deallocate(vertices);
    return(tristripobj);
}
