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
	ddt_g4vll_drom_quadgrid * ddr_g4vll_drom_quadgrid_create (object)
	void ddr_g4vll_drom_quadgrid_print (object, data)
	void ddr_g4vll_drom_quadgrid_delete (object, data)
	void ddr_g4vll_drom_quadgrid_render (object)

  ======================================================================
 */
#include "drom.h"
#include <dore/internal/system.h>
#include <dore/dore_develop/private/quadgrid.h>

/*
 ======================================================================
 */
static DtInt classid = -1;
static DtInt dataindex = -1;

/*
  ======================================================================
 */
ddt_g4vll_drom_quadgrid *ddr_g4vll_drom_quadgrid_create (
    DtObjectStructure *object)
{
    struct quadgrid *quadgrid;
    ddt_g4vll_drom_quadgrid *drom_quadgrid;
    int i;

    quadgrid = (struct quadgrid *)(object->data);

    drom_quadgrid = (ddt_g4vll_drom_quadgrid *)
		DDspace_Allocate(sizeof(ddt_g4vll_drom_quadgrid));

    if (quadgrid->uv_count > 0) {
	drom_quadgrid->bboxuv = (ddt_g4vll_drom_bboxuv *)DDspace_Allocate
		(quadgrid->uv_count *sizeof(ddt_g4vll_drom_bboxuv));

	for (i=0; i < quadgrid->uv_count; i++) {
	    drom_quadgrid->bboxuv[i].initialized = DcFalse;
	}
    } else {
	drom_quadgrid->bboxuv = (ddt_g4vll_drom_bboxuv *)0;
    }
    if (quadgrid->uvw_count > 0) {
	drom_quadgrid->bboxuvw = (ddt_g4vll_drom_bboxuvw *)DDspace_Allocate
		(quadgrid->uvw_count *sizeof(ddt_g4vll_drom_bboxuvw));

	for (i=0; i < quadgrid->uvw_count; i++) {
	    drom_quadgrid->bboxuvw[i].initialized = DcFalse;
	}
    } else {
	drom_quadgrid->bboxuvw = (ddt_g4vll_drom_bboxuvw *)0;
    }

    return drom_quadgrid;
}
/*
  ======================================================================
 */
void ddr_g4vll_drom_quadgrid_print (
    DtObject object,
    DtInt *data)
{
}
/*
  ======================================================================
 */
void ddr_g4vll_drom_quadgrid_delete (
    DtObject object,
    ddt_g4vll_drom_quadgrid *data)
{
    if (data != (ddt_g4vll_drom_quadgrid *)0) {
        if (data->bboxuv != (ddt_g4vll_drom_bboxuv *)0) {
            DDspace_Deallocate(data->bboxuv);
        }
        if (data->bboxuvw != (ddt_g4vll_drom_bboxuvw *)0) {
            DDspace_Deallocate(data->bboxuvw);
        }
	DDspace_Deallocate(data);
    }
}
/*
 ======================================================================
 */
void ddr_g4vll_drom_quadgrid_render (
    dot_object *object)
{
    extern DtRealCouple *dde_g4vll_quad_grid_bboxuv;
    extern DtRealTriple *dde_g4vll_quad_grid_bboxuvw;
    struct quadgrid *quadgrid;
    ddt_g4vll_drom_quadgrid *drom_quadgrid;
    ddt_g4vll_drom_bboxuv  *bboxuv;
    ddt_g4vll_drom_bboxuvw *bboxuvw;
    DtInt i, idx;
    DtInt vertexcount;

    if (classid == -1) {
	classid = DsInqClassId("DoQuadGrid");
    }

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

    quadgrid = (struct quadgrid *)object->data;

    drom_quadgrid = (ddt_g4vll_drom_quadgrid *)
		object->additional_data[dataindex];

    /*
     * Store the object's handle globally for its output routine.
     */
    dde_g4vll_drom_quadgrid = drom_quadgrid;

    if (quadgrid->uv_count > 0 && dde_g4vll_drom_data.texture_2D_valid) {
	idx = quadgrid->uv_count > dde_g4vll_drom_data.map_diffuse_color_uv_index ?
		dde_g4vll_drom_data.map_diffuse_color_uv_index : 0;

	bboxuv = &(drom_quadgrid->bboxuv[idx]);

        vertexcount = quadgrid->width*quadgrid->height;
	if (!bboxuv->initialized) {
	    float umin, umax, vmin, vmax;
	    /*
	     * This bounding box has not been set yet so compute it.
	     */

	    umin = umax = quadgrid->uv_list[idx][0][0];
	    vmin = vmax = quadgrid->uv_list[idx][0][1];

	    for (i=1; i<vertexcount; i++) {
		umin = min(umin, quadgrid->uv_list[idx][i][0]);
		umax = max(umax, quadgrid->uv_list[idx][i][0]);
		vmin = min(vmin, quadgrid->uv_list[idx][i][1]);
		vmax = max(vmax, quadgrid->uv_list[idx][i][1]);
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
	dde_g4vll_quad_grid_bboxuv = bboxuv->bbox;
    }

    if (quadgrid->uvw_count > 0 && dde_g4vll_drom_data.texture_3D_valid) {
        idx = quadgrid->uvw_count > dde_g4vll_drom_data.map_diffuse_color_uvw_index ?
                dde_g4vll_drom_data.map_diffuse_color_uvw_index : 0;

        bboxuvw = &(drom_quadgrid->bboxuvw[idx]);

        vertexcount = quadgrid->width*quadgrid->height;

        if (!bboxuvw->initialized) {
            DtReal umin, umax, vmin, vmax, wmin, wmax;
            /*
             * This bounding box has not been set yet so compute it.
             */

            umin = umax = quadgrid->uvw_list[idx][0][0];
            vmin = vmax = quadgrid->uvw_list[idx][0][1];
            wmin = wmax = quadgrid->uvw_list[idx][0][2];

            for (i=1; i< vertexcount; i++) {
                umin = min(umin, quadgrid->uvw_list[idx][i][0]);
                umax = max(umax, quadgrid->uvw_list[idx][i][0]);
                vmin = min(vmin, quadgrid->uvw_list[idx][i][1]);
                vmax = max(vmax, quadgrid->uvw_list[idx][i][1]);
                wmin = min(wmin, quadgrid->uvw_list[idx][i][2]);
                wmax = max(wmax, quadgrid->uvw_list[idx][i][2]);
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
        dde_g4vll_quad_grid_bboxuvw = bboxuvw->bbox;
    }

    if (doe_system_timing_display_traverse_count == 1)
        doe_system_timing_mesh_quads_drawn += ((quadgrid->width - 1) *
		(quadgrid->height - 1));

    ddr_g4vll_drom_render_quad_grid (DcRGB,
				       quadgrid->bounding_box_pts,
				       quadgrid->width,
				       quadgrid->height,
				       quadgrid->element_normal,
				       quadgrid->vertex_location,
				       quadgrid->vertex_normal,
				       quadgrid->vertex_color,
				       quadgrid->uv_count,
				       quadgrid->uv_list,
				       quadgrid->uvw_count,
				       quadgrid->uvw_list,
				       quadgrid->vertex_alpha);

    dde_g4vll_quad_grid_bboxuv  = (DtRealCouple *)0;
    dde_g4vll_quad_grid_bboxuvw = (DtRealTriple *)0;
    dde_g4vll_drom_quadgrid = (ddt_g4vll_drom_quadgrid *)0;
}
/*
 ======================================================================
 */
