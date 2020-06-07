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
	ddt_vll_drom_tristrip * ddr_vll_drom_tristrip_create (object)
	void ddr_vll_drom_tristrip_print (object, data)
	void ddr_vll_drom_tristrip_delete (object, data)
	void ddr_vll_drom_tristrip_render (object)

  ======================================================================
 */
#include "drom.h"
#include "dore/dore_develop/private/tristrip.h"

/*
 * Structure for additional data required by VLL.  bboxuv and bboxuvw are 
 * convex hulls enclosing all texture indices of the current texture, specified 
 * in uv and uvw space, respectively.
 */
typedef struct {
    ddt_vll_drom_bboxuv *	bboxuv;
    ddt_vll_drom_bboxuvw *	bboxuvw;
} ddt_vll_drom_tristrip;

/*
 ======================================================================
 */
static DtInt classid = -1;
static DtInt dataindex = -1;

/*
  ======================================================================
 */
ddt_vll_drom_tristrip *
ddr_vll_drom_tristrip_create(object)
DtObjectStructure *object;
{
    struct tristrip *tristrip;
    ddt_vll_drom_tristrip *drom_tristrip;
    int i;

    tristrip = (struct tristrip *)(object->data);

    drom_tristrip =  (ddt_vll_drom_tristrip *)
	    DDspace_Allocate(sizeof(ddt_vll_drom_tristrip));

    if (tristrip->uv_count > 0) {
	drom_tristrip->bboxuv = (ddt_vll_drom_bboxuv *)DDspace_Allocate
		(tristrip->uv_count *sizeof(ddt_vll_drom_bboxuv));

	for (i=0; i< tristrip->uv_count; i++) {
	    drom_tristrip->bboxuv[i].initialized = DcFalse;
	}
    } else {
	drom_tristrip->bboxuv = (ddt_vll_drom_bboxuv *)0;
    }

    if (tristrip->uvw_count > 0) {
        drom_tristrip->bboxuvw = (ddt_vll_drom_bboxuvw *)DDspace_Allocate
                (tristrip->uvw_count *sizeof(ddt_vll_drom_bboxuvw));

        for (i=0; i< tristrip->uvw_count; i++) {
            drom_tristrip->bboxuvw[i].initialized = DcFalse;
        }
    } else {
        drom_tristrip->bboxuvw = (ddt_vll_drom_bboxuvw *)0;
    }

    return drom_tristrip;
}
/*
  ======================================================================
 */
void
ddr_vll_drom_tristrip_print(object, data)
DtObject object;
DtInt *data;
{
}
/*
  ======================================================================
 */
void
ddr_vll_drom_tristrip_delete(object, data)
DtObject object;
ddt_vll_drom_tristrip *data;
{
    if (data != (ddt_vll_drom_tristrip *)0) {
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
ddr_vll_drom_tristrip_render(object)
dot_object *object;
{
    extern DtRealCouple *dde_vll_triangle_strip_bboxuv;
    extern DtRealTriple *dde_vll_triangle_strip_bboxuvw;
    struct tristrip *tristrip;
    DtInt i;
    DtInt triangleno;
    DtInt odd;
    DtInt basevtx;
    DtInt idx;
    ddt_vll_drom_bboxuv *bboxuv;
    ddt_vll_drom_bboxuvw *bboxuvw;
    ddt_vll_drom_tristrip *drom_tristrip;

    if (classid == -1) 
	    classid = DsInqClassId("DoTriangleStrip");

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

    tristrip = (struct tristrip *)(object->data);

    drom_tristrip = (ddt_vll_drom_tristrip *)object->additional_data[dataindex];

    if (tristrip->uv_count > 0 && dde_vll_drom_data.texture_2D_valid) {
	idx = tristrip->uv_count > dde_vll_drom_data.map_diffuse_color_uv_index ?
		dde_vll_drom_data.map_diffuse_color_uv_index : 0;

	bboxuv = &(drom_tristrip->bboxuv[idx]);

	if (!bboxuv->initialized) {
	    float umin, umax, vmin, vmax;
	    /*
	     * this bounding box has not been set yet so compute bounding
	     * box
	     */

	    umin = umax = tristrip->uv_list[idx][0][0];
	    vmin = vmax = tristrip->uv_list[idx][0][1];

	    for (i=1; i< tristrip->vertex_count; i++) {
		umin = min(umin, tristrip->uv_list[idx][i][0]);
		umax = max(umax, tristrip->uv_list[idx][i][0]);
		vmin = min(vmin, tristrip->uv_list[idx][i][1]);
		vmax = max(vmax, tristrip->uv_list[idx][i][1]);
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
	dde_vll_triangle_strip_bboxuv = bboxuv->bbox;
    }

    if (tristrip->uvw_count > 0 && dde_vll_drom_data.texture_3D_valid) {
        idx = tristrip->uvw_count > dde_vll_drom_data.map_diffuse_color_uvw_index ?
                dde_vll_drom_data.map_diffuse_color_uvw_index : 0;

        bboxuvw = &(drom_tristrip->bboxuvw[idx]);
/*
printf ("bboxuvw points to %d\n", bboxuvw);
*/

        if (!bboxuvw->initialized) {
            DtReal umin, umax, vmin, vmax, wmin, wmax;

            /*
             * this bounding box has not been set yet so compute bounding
             * box
             */

            umin = umax = tristrip->uvw_list[idx][0][0];
            vmin = vmax = tristrip->uvw_list[idx][0][1];
            wmin = wmax = tristrip->uvw_list[idx][0][2];

            for (i=1; i< tristrip->vertex_count; i++) {
                umin = min(umin, tristrip->uvw_list[idx][i][0]);
                umax = max(umax, tristrip->uvw_list[idx][i][0]);
                vmin = min(vmin, tristrip->uvw_list[idx][i][1]);
                vmax = max(vmax, tristrip->uvw_list[idx][i][1]);
                wmin = min(wmin, tristrip->uvw_list[idx][i][2]);
                wmax = max(wmax, tristrip->uvw_list[idx][i][2]);
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
        dde_vll_triangle_strip_bboxuvw = bboxuvw->bbox;
    }

    doe_system_timing_strip_triangles_drawn += tristrip->vertex_count - 2;

    ddr_vll_drom_render_triangle_strip (DcRGB,
				        tristrip->bounding_box_pts,
				        tristrip->vertex_count,
				        tristrip->vertex_location,
				        tristrip->vertex_normal,
				        tristrip->vertex_color,
				        tristrip->element_normal,
				        tristrip->uv_count,
				        tristrip->uv_list,
				        tristrip->uvw_count,
				        tristrip->uvw_list);

    dde_vll_triangle_strip_bboxuv = (DtRealCouple *)0;
    dde_vll_triangle_strip_bboxuvw = (DtRealTriple *)0;
}
/*
  ======================================================================
 */
