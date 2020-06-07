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
        ddt_g4vll_drom_vartristrip * ddr_g4vll_drom_vartristrip_create (object)
        void ddr_g4vll_drom_vartristrip_print (object, data)
        void ddr_g4vll_drom_vartristrip_delete (object, data)
	void ddr_g4vll_drom_vartristrip_render (object)

  ======================================================================
 */
#include "drom.h"
#include <dore/internal/system.h>
#include <dore/dore_develop/private/vartrilst.h>

/*
 ======================================================================
 */

static DtInt classid = -1;
static DtInt dataindex = -1;

/*
  ======================================================================
 */
ddt_g4vll_drom_vartrilist *ddr_g4vll_drom_vartrilist_create (
    DtObjectStructure *object)
{
    struct vartrilist *vartrilist;
    ddt_g4vll_drom_vartrilist *drom_vartrilist;
    int i;

    vartrilist = (struct vartrilist *)(object->data);

    drom_vartrilist =  (ddt_g4vll_drom_vartrilist *)
            DDspace_Allocate(sizeof(ddt_g4vll_drom_vartrilist));

    drom_vartrilist->uvcount = vartrilist->uv_count;
    drom_vartrilist->uvwcount = vartrilist->uvw_count;

    if (vartrilist->uv_count > 0) {
        drom_vartrilist->bboxuv = (ddt_g4vll_drom_bboxuv *)DDspace_Allocate
                (vartrilist->uv_count *sizeof(ddt_g4vll_drom_bboxuv));

        for (i=0; i< vartrilist->uv_count; i++) {
            drom_vartrilist->bboxuv[i].initialized = DcFalse;
        }
    } else {
        drom_vartrilist->bboxuv = (ddt_g4vll_drom_bboxuv *)0;
    }

    if (vartrilist->uvw_count > 0) {
        drom_vartrilist->bboxuvw = (ddt_g4vll_drom_bboxuvw *)DDspace_Allocate
                (vartrilist->uvw_count *sizeof(ddt_g4vll_drom_bboxuvw));

        for (i=0; i< vartrilist->uvw_count; i++) {
            drom_vartrilist->bboxuvw[i].initialized = DcFalse;
        }
    } else {
        drom_vartrilist->bboxuvw = (ddt_g4vll_drom_bboxuvw *)0;
    }

    /*
     * We will update the drom_vartrilist information only when its
     * sequence number does not match that of the kernel.
     */
    drom_vartrilist->sequence_number = vartrilist->sequence_number;

    return drom_vartrilist;
}
/*
  ======================================================================
 */
void ddr_g4vll_drom_vartrilist_print (
    DtObject object,
    DtInt *data)
{
}
/*
  ======================================================================
 */
void ddr_g4vll_drom_vartrilist_delete (
    DtObject object,
    ddt_g4vll_drom_vartrilist *data)
{
    if (data != (ddt_g4vll_drom_vartrilist *)0) {
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
void ddr_g4vll_drom_vartrilist_render (
    dot_object *object)
{
    struct vartrilist *vartrilist;
    ddt_g4vll_drom_vartrilist *drom_vartrilist;
    ddt_g4vll_drom_bboxuv *bboxuv;
    ddt_g4vll_drom_bboxuvw *bboxuvw;
    dot_object *shader;
    DtInt i;
    DtInt index;

    if (classid == -1) 
	    classid = DsInqClassId("DoVarTriangleList");

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

    vartrilist = (struct vartrilist *)(object->data);

    drom_vartrilist = (ddt_g4vll_drom_vartrilist *)
		object->additional_data[dataindex];

    /*
     * Store the vartrilist object's handle globally for the trilist
     * output routine.
     */
    dde_g4vll_drom_vartrilist = drom_vartrilist;

    /*
     * If we're texture mapping and have UVs, construct the uv bounding box 
     * from the appropriate UV set.  Do this only if the kernel's index or
     * sequence number doesn't match that of the driver's.
     */
    if (vartrilist->uv_count > 0 && dde_g4vll_drom_data.texture_2D_valid) {
        index = vartrilist->uv_count > 
	    	dde_g4vll_drom_data.map_diffuse_color_uv_index ?
            	dde_g4vll_drom_data.map_diffuse_color_uv_index : 0;

        if ((index+1) > drom_vartrilist->uvcount) {
	    /* 
	     * New UV set(s) have been added since we were last through here.
	     * Add'em to the end of the driver's data.
	     */
            drom_vartrilist->bboxuv = (ddt_g4vll_drom_bboxuv *)
                    	DDspace_Reallocate(drom_vartrilist->bboxuv,
                        vartrilist->uv_count * sizeof(ddt_g4vll_drom_bboxuv));
            for (i = drom_vartrilist->uvcount; i < vartrilist->uv_count; i++) {
                drom_vartrilist->bboxuv[i].initialized = DcFalse;
            }
            drom_vartrilist->uvcount = vartrilist->uv_count;
            drom_vartrilist->sequence_number = vartrilist->sequence_number;

        } else if (drom_vartrilist->sequence_number != 
	    	   vartrilist->sequence_number) {
	    /*
	     * One or more of the existing UV set(s) have been changed.
	     * We don't know which one so "uninitialise" all of them.
	     */
            for (i = 0; i < vartrilist->uv_count; i++) {
                drom_vartrilist->bboxuv[i].initialized = DcFalse;
            }
            drom_vartrilist->uvcount = vartrilist->uv_count;
            drom_vartrilist->sequence_number = vartrilist->sequence_number;
	}

        bboxuv = &(drom_vartrilist->bboxuv[index]);

        if (!bboxuv->initialized) {
            float umin, umax, vmin, vmax;
            /*
             * This bounding box has not been set yet so compute it.
             */
            umin = umax = vartrilist->uv_list[index][0][0];
            vmin = vmax = vartrilist->uv_list[index][0][1];

            for (i=1; i< vartrilist->triangle_count * 3; i++) {
                umin = min(umin, vartrilist->uv_list[index][i][0]);
                umax = max(umax, vartrilist->uv_list[index][i][0]);
                vmin = min(vmin, vartrilist->uv_list[index][i][1]);
                vmax = max(vmax, vartrilist->uv_list[index][i][1]);
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
    }

    /*
     * If we're texture mapping and have UVWs, construct the uvw bounding box
     * from the appropriate UVW set.  Do this only if the kernel's index or
     * sequence number doesn't match that of the driver's.
     */
    if (vartrilist->uvw_count > 0 && dde_g4vll_drom_data.texture_3D_valid) {
        index = vartrilist->uvw_count > 
		dde_g4vll_drom_data.map_diffuse_color_uvw_index ?
                dde_g4vll_drom_data.map_diffuse_color_uvw_index : 0;

        if ((index+1) > drom_vartrilist->uvwcount) {
	    /*
	     * New UVW set(s) have been added since we were last through here.
             * Add'em to the end of the driver's data.
             */
            drom_vartrilist->bboxuvw = (ddt_g4vll_drom_bboxuvw *)
                    DDspace_Reallocate(drom_vartrilist->bboxuvw,
                        vartrilist->uvw_count * sizeof(ddt_g4vll_drom_bboxuvw));
            for (i=drom_vartrilist->uvwcount; i<vartrilist->uvw_count; i++) {
                drom_vartrilist->bboxuvw[i].initialized = DcFalse;
            }
            drom_vartrilist->uvwcount = vartrilist->uvw_count;
            drom_vartrilist->sequence_number = vartrilist->sequence_number;
        } else if (drom_vartrilist->sequence_number !=
                   vartrilist->sequence_number) {
            /*
             * One or more of the existing UVW set(s) have been changed.
	     * We don't know which one so "uninitialise" all of them.
             */
            for (i = 0; i < vartrilist->uvw_count; i++) {
                drom_vartrilist->bboxuvw[i].initialized = DcFalse;
            }
            drom_vartrilist->uvwcount = vartrilist->uvw_count;
            drom_vartrilist->sequence_number = vartrilist->sequence_number;
        }

        bboxuvw = &(drom_vartrilist->bboxuvw[index]);

        if (!bboxuvw->initialized) {
            float umin, umax, vmin, vmax, wmin, wmax;
            /*
             * This bounding box has not been set yet so compute it.
             */
            umin = umax = vartrilist->uvw_list[index][0][0];
            vmin = vmax = vartrilist->uvw_list[index][0][1];
            wmin = wmax = vartrilist->uvw_list[index][0][2];

            for (i=1; i< vartrilist->triangle_count * 3; i++) {
                umin = min(umin, vartrilist->uvw_list[index][i][0]);
                umax = max(umax, vartrilist->uvw_list[index][i][0]);
                vmin = min(vmin, vartrilist->uvw_list[index][i][1]);
                vmax = max(vmax, vartrilist->uvw_list[index][i][1]);
                wmin = min(wmin, vartrilist->uvw_list[index][i][2]);
                wmax = max(wmax, vartrilist->uvw_list[index][i][2]);
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

            bboxuv->initialized = DcTrue;
        }
    }

    DDclass_InqMethod(DcTypeAttSurfaceShader, DcMethodInqGlbAttVal)(&shader);
    if (shader != DcShaderConstant) {
    	if (vartrilist->needelemnrms) {
	    if (vartrilist->element_normal == NULL) {
	    	vartrilist->element_normal = (DtRealTriple *)DDspace_Allocate
			    (vartrilist->triangle_count*(sizeof (DtRealTriple)));
	    }
	    dor_vartrilist_calculate_triangle_normals(vartrilist);
	    vartrilist->needelemnrms = DcFalse;
	}
    }

    if (doe_system_timing_display_traverse_count == 1)
        doe_system_timing_triangles_drawn += vartrilist->triangle_count;

    ddr_g4vll_drom_render_triangle_list (DcRGB,
				       vartrilist->bounding_box_pts,
				       vartrilist->triangle_count,
				       vartrilist->element_normal,
				       vartrilist->vertex_location,
				       vartrilist->vertex_normal,
				       vartrilist->vertex_color,
				       vartrilist->uv_count,
				       vartrilist->uv_list, 
				       vartrilist->uvw_count,
				       vartrilist->uvw_list,
				       vartrilist->vertex_alpha);


    dde_g4vll_drom_vartrilist = (ddt_g4vll_drom_vartrilist *)0;

}
/*
  ======================================================================
 */
