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
	ddt_vll_drom_quadlst * ddr_vll_drom_quadlst_create (object)
	void ddr_vll_drom_quadlst_print (object, data)
	void ddr_vll_drom_quadlst_delete (object, data)
	void ddr_vll_drom_quadlst_render (object)

  ======================================================================
 */
#include "drom.h"
#include "dore/dore_develop/private/quadlst.h"

/* 
 * Additional data required by VLL.  bboxuv and bboxuvw are the convex hulls
 * enclosing all texture indices of the current texture, specified in uv and
 * uvw space respectively.
 */
typedef struct {
    ddt_vll_drom_bboxuv *bboxuv;
    ddt_vll_drom_bboxuvw *bboxuvw;
} ddt_vll_drom_quadlst;

/*
 ======================================================================
 */
static DtInt classid = -1;
static DtInt dataindex = -1;

/*
  ======================================================================
 */
ddt_vll_drom_quadlst *
ddr_vll_drom_quadlst_create(object)
DtObjectStructure *object;
{
    struct quadlst *quadlst;
    ddt_vll_drom_quadlst *drom_quadlst;
    int i;

    quadlst = (struct quadlst *)(object->data);

    drom_quadlst =  (ddt_vll_drom_quadlst *)
            DDspace_Allocate(sizeof(ddt_vll_drom_quadlst));

    if (quadlst->uv_count > 0) {
	drom_quadlst->bboxuv = (ddt_vll_drom_bboxuv *)DDspace_Allocate
		(quadlst->uv_count *sizeof(ddt_vll_drom_bboxuv));

	for (i=0; i< quadlst->uv_count; i++) {
	    drom_quadlst->bboxuv[i].initialized = DcFalse;
	}
    } else {
	drom_quadlst->bboxuv = (ddt_vll_drom_bboxuv *)0;
    }

    if (quadlst->uvw_count > 0) {
	drom_quadlst->bboxuvw = (ddt_vll_drom_bboxuvw *)DDspace_Allocate
		(quadlst->uvw_count *sizeof(ddt_vll_drom_bboxuvw));

	for (i=0; i< quadlst->uvw_count; i++) {
	    drom_quadlst->bboxuvw[i].initialized = DcFalse;
	}
    } else {
	drom_quadlst->bboxuvw = (ddt_vll_drom_bboxuvw *)0;
    }

    return drom_quadlst;
}
/*
  ======================================================================
 */
void
ddr_vll_drom_quadlst_print(object, data)
DtObject object;
DtInt *data;
{
}
/*
  ======================================================================
 */
void
ddr_vll_drom_quadlst_delete(object, data)
DtObject object;
ddt_vll_drom_quadlst *data;
{
    if (data != (ddt_vll_drom_quadlst *)0) {
        if (data->bboxuv != (ddt_vll_drom_bboxuv *)0) {
            DDspace_Deallocate(data->bboxuv);
        }
        if (data->bboxuvw != (ddt_vll_drom_bboxuvw *)0) {
            DDspace_Deallocate(data->bboxuvw);
        }

	DDspace_Deallocate(data);
    }
}
/*
  ======================================================================
 */
void
ddr_vll_drom_quadlst_render(object)
dot_object *object;
{
    extern DtRealCouple *dde_vll_quad_list_bboxuv;
    extern DtRealTriple *dde_vll_quad_list_bboxuvw;
    struct quadlst *quadlst;
    DtInt i;
    ddt_vll_drom_quadlst *drom_quadlst;
    ddt_vll_drom_bboxuv *bboxuv;
    ddt_vll_drom_bboxuvw *bboxuvw;
    DtInt quadno;
    DtInt idx;
    
    if (classid == -1) 
	    classid = DsInqClassId("DoQuadList");

    /* Determine index into quad list's additional data */
    if (dataindex == -1) {
	for (i=0; dde_vll_drom_class_data[i].name != 0; i++) {
	    if (dde_vll_drom_class_data[i].id == classid) {
		dataindex = dde_vll_drom_class_data[i].index;
		break;
	    }
	}
    }

    if (!DDcondex_QueryRender(classid))
	    return;

    quadlst = (struct quadlst *)(object->data);

    drom_quadlst = (ddt_vll_drom_quadlst *)object->additional_data[dataindex];

    if (quadlst->uv_count > 0 && dde_vll_drom_data.texture_2D_valid) {
	idx = quadlst->uv_count > dde_vll_drom_data.map_diffuse_color_uv_index ?
		dde_vll_drom_data.map_diffuse_color_uv_index : 0;

	bboxuv = &(drom_quadlst->bboxuv[idx]);

	if (!bboxuv->initialized) {
	    DtReal umin, umax, vmin, vmax;
	    /*
	     * this bounding box has not been set yet so compute bounding
	     * box
	     */

	    umin = umax = quadlst->uv_list[idx][0][0];
	    vmin = vmax = quadlst->uv_list[idx][0][1];

	    for (i=1; i< quadlst->quad_count*4; i++) {
		umin = min(umin, quadlst->uv_list[idx][i][0]);
		umax = max(umax, quadlst->uv_list[idx][i][0]);
		vmin = min(vmin, quadlst->uv_list[idx][i][1]);
		vmax = max(vmax, quadlst->uv_list[idx][i][1]);
	    }

	    bboxuv->bbox[0][0] = umin;
	    bboxuv->bbox[0][1] = vmin;
	    
	    bboxuv->bbox[1][0] = umin;
	    bboxuv->bbox[1][1] = vmax;
	    
	    bboxuv->bbox[2][0] = umax;
	    bboxuv->bbox[2][1] = vmax;
	    
	    bboxuv->bbox[3][0] = umax;
	    bboxuv->bbox[3][1] = vmin;
	}
	dde_vll_quad_list_bboxuv = bboxuv->bbox;
    }

    if (quadlst->uvw_count > 0 && dde_vll_drom_data.texture_3D_valid) {
	idx = quadlst->uvw_count > dde_vll_drom_data.map_diffuse_color_uvw_index ?
		dde_vll_drom_data.map_diffuse_color_uvw_index : 0;

	bboxuvw = &(drom_quadlst->bboxuvw[idx]);

	if (!bboxuvw->initialized) {
	    DtReal umin, umax, vmin, vmax, wmin, wmax;
	    /*
	     * this bounding box has not been set yet so compute bounding
	     * box
	     */

	    umin = umax = quadlst->uvw_list[idx][0][0];
	    vmin = vmax = quadlst->uvw_list[idx][0][1];
	    wmin = wmax = quadlst->uvw_list[idx][0][2];

	    for (i=1; i< quadlst->quad_count*4; i++) {
		umin = min(umin, quadlst->uvw_list[idx][i][0]);
		umax = max(umax, quadlst->uvw_list[idx][i][0]);
		vmin = min(vmin, quadlst->uvw_list[idx][i][1]);
		vmax = max(vmax, quadlst->uvw_list[idx][i][1]);
		wmin = min(wmin, quadlst->uvw_list[idx][i][2]);
		wmax = max(wmax, quadlst->uvw_list[idx][i][2]);
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
	}
	dde_vll_quad_list_bboxuvw = bboxuvw->bbox;
    }

    doe_system_timing_quads_drawn += quadlst->quad_count;

    ddr_vll_drom_render_quad_list (DcRGB,
				   quadlst->bounding_box_pts,
				   quadlst->quad_count,
				   quadlst->element_normal,
				   quadlst->vertex_location,
				   quadlst->vertex_normal,
				   quadlst->vertex_color,
				   quadlst->uv_count,
				   quadlst->uv_list,
				   quadlst->uvw_count,
				   quadlst->uvw_list);

    dde_vll_quad_list_bboxuv = (DtRealCouple *)0;
    dde_vll_quad_list_bboxuvw = (DtRealTriple *)0;
}
/*
  ======================================================================
 */
