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
	ddt_vll_drom_trilst * ddr_vll_drom_trilst_create (object)
	void ddr_vll_drom_trilst_print (object, data)
	void ddr_vll_drom_trilst_delete (object, data)
	void ddr_vll_drom_trilst_render (object)

  ======================================================================
 */
#include "drom.h"
#include "dore/dore_develop/private/trilst.h"

/*
 * Additional data required by VLL.  bboxuv and bboxuvw are the convex hulls
 * enclosing all texture indices of the current texture, specified in uv and
 * uvw space respectively.
 */
typedef struct {
    ddt_vll_drom_bboxuv *bboxuv;
    ddt_vll_drom_bboxuvw *bboxuvw;
} ddt_vll_drom_trilst;

/*
 ======================================================================
 */
static DtInt classid = -1;
static DtInt dataindex = -1;

/*
  ======================================================================
 */
ddt_vll_drom_trilst *
ddr_vll_drom_trilst_create(object)
DtObjectStructure *object;
{
    struct trilst *trilst;
    ddt_vll_drom_trilst *drom_trilst;
    int i;

    trilst = (struct trilst *)(object->data);

    drom_trilst =  (ddt_vll_drom_trilst *)
            DDspace_Allocate(sizeof(ddt_vll_drom_trilst));

    if (trilst->uv_count > 0) {
	drom_trilst->bboxuv = (ddt_vll_drom_bboxuv *)DDspace_Allocate
		(trilst->uv_count *sizeof(ddt_vll_drom_bboxuv));

	for (i=0; i< trilst->uv_count; i++) {
	    drom_trilst->bboxuv[i].initialized = DcFalse;
	}
    } else {
	drom_trilst->bboxuv = (ddt_vll_drom_bboxuv *)0;
    }

    if (trilst->uvw_count > 0) {
	drom_trilst->bboxuvw = (ddt_vll_drom_bboxuvw *)DDspace_Allocate
		(trilst->uvw_count *sizeof(ddt_vll_drom_bboxuvw));

	for (i=0; i< trilst->uvw_count; i++) {
	    drom_trilst->bboxuvw[i].initialized = DcFalse;
	}
    } else {
	drom_trilst->bboxuvw = (ddt_vll_drom_bboxuvw *)0;
    }

    return drom_trilst;
}
/*
  ======================================================================
 */
void
ddr_vll_drom_trilst_print(object, data)
DtObject object;
DtInt *data;
{
}
/*
  ======================================================================
 */
void
ddr_vll_drom_trilst_delete(object, data)
DtObject object;
ddt_vll_drom_trilst *data;
{
    if (data != (ddt_vll_drom_trilst *)0) {
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
ddr_vll_drom_trilst_render(object)
dot_object *object;
{
    extern DtInt *dde_vll_triangle_list_edge_enable;
    extern DtRealCouple *dde_vll_triangle_list_bboxuv;
    extern DtRealTriple *dde_vll_triangle_list_bboxuvw;
    struct trilst *trilst;
    ddt_vll_drom_trilst *drom_trilst;
    ddt_vll_drom_bboxuv *bboxuv;
    ddt_vll_drom_bboxuvw *bboxuvw;
    static DtPFI reptype_query = (DtPFI)DcNullPtr;
    DtRepType reptype;
    DtInt i, idx;

    if (classid == -1) {
	classid = DsInqClassId("DoTriangleList");
    }

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

    trilst = (struct trilst *)object->data;

    drom_trilst = (ddt_vll_drom_trilst *)object->additional_data[dataindex];

    if (trilst->uv_count > 0 && dde_vll_drom_data.texture_2D_valid) {
	idx = trilst->uv_count > dde_vll_drom_data.map_diffuse_color_uv_index ?
		dde_vll_drom_data.map_diffuse_color_uv_index : 0;

	bboxuv = &(drom_trilst->bboxuv[idx]);

	if (!bboxuv->initialized) {
	    float umin, umax, vmin, vmax;
	    /*
	     * this bounding box has not been set yet so compute bounding
	     * box
	     */

	    umin = umax = trilst->uv_list[idx][0][0];
	    vmin = vmax = trilst->uv_list[idx][0][1];

	    for (i=1; i< trilst->triangle_count*3; i++) {
		umin = min(umin, trilst->uv_list[idx][i][0]);
		umax = max(umax, trilst->uv_list[idx][i][0]);
		vmin = min(vmin, trilst->uv_list[idx][i][1]);
		vmax = max(vmax, trilst->uv_list[idx][i][1]);
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
	dde_vll_triangle_list_bboxuv = bboxuv->bbox;
    }

    if (trilst->uvw_count > 0 && dde_vll_drom_data.texture_3D_valid) {
        idx = trilst->uvw_count > dde_vll_drom_data.map_diffuse_color_uvw_index ?
                dde_vll_drom_data.map_diffuse_color_uvw_index : 0;

        bboxuvw = &(drom_trilst->bboxuvw[idx]);

        if (!bboxuvw->initialized) {
            DtReal umin, umax, vmin, vmax, wmin, wmax;
            /*
             * this bounding box has not been set yet so compute bounding
             * box
             */

            umin = umax = trilst->uvw_list[idx][0][0];
            vmin = vmax = trilst->uvw_list[idx][0][1];
            wmin = wmax = trilst->uvw_list[idx][0][2];

            for (i=1; i< trilst->triangle_count*3; i++) {
                umin = min(umin, trilst->uvw_list[idx][i][0]);
                umax = max(umax, trilst->uvw_list[idx][i][0]);
                vmin = min(vmin, trilst->uvw_list[idx][i][1]);
                vmax = max(vmax, trilst->uvw_list[idx][i][1]);
                wmin = min(wmin, trilst->uvw_list[idx][i][2]);
                wmax = max(wmax, trilst->uvw_list[idx][i][2]);
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
        dde_vll_triangle_list_bboxuvw = bboxuvw->bbox;
    }

    if (reptype_query == (DtPFI)DcNullPtr) {
 	reptype_query = DDclass_InqMethod (DsInqClassId("AttRepType"),
					   DcMethodInqGlbAttVal);
    }
    (*reptype_query)(&reptype);

    doe_system_timing_triangles_drawn += trilst->triangle_count;

    if (reptype == DcSurfaceOutlineEdges && 
	trilst->edge_enable != NULL) {
	dde_vll_triangle_list_edge_enable = trilst->edge_enable;
    }

    ddr_vll_drom_render_triangle_list (DcRGB,
				       trilst->bounding_box_pts,
				       trilst->triangle_count,
				       trilst->element_normal,
				       trilst->vertex_location,
				       trilst->vertex_normal,
				       trilst->vertex_color,
				       trilst->uv_count,
				       trilst->uv_list,
				       trilst->uvw_count,
				       trilst->uvw_list);

    dde_vll_triangle_list_edge_enable = (DtInt *)0;
    dde_vll_triangle_list_bboxuv = (DtRealCouple *)0;
    dde_vll_triangle_list_bboxuvw = (DtRealTriple *)0;
}
/*
 ======================================================================
 */
