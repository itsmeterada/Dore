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
#include <dore/dore_develop/private/vartristrp.h>

/*
 ======================================================================
 */

static DtInt classid = -1;
static DtInt dataindex = -1;

/*
  ======================================================================
 */
ddt_g4vll_drom_vartristrip *ddr_g4vll_drom_vartristrip_create (
    DtObjectStructure *object)
{
    struct vartristrip *vartristrip;
    ddt_g4vll_drom_vartristrip *drom_vartristrip;
    int i;

    vartristrip = (struct vartristrip *)(object->data);

    drom_vartristrip =  (ddt_g4vll_drom_vartristrip *)
            DDspace_Allocate(sizeof(ddt_g4vll_drom_vartristrip));

    drom_vartristrip->uvcount = vartristrip->uv_count;
    drom_vartristrip->uvwcount = vartristrip->uvw_count;

    if (vartristrip->uv_count > 0) {
        drom_vartristrip->bboxuv = (ddt_g4vll_drom_bboxuv *)DDspace_Allocate
                (vartristrip->uv_count *sizeof(ddt_g4vll_drom_bboxuv));

        for (i=0; i< vartristrip->uv_count; i++) {
            drom_vartristrip->bboxuv[i].initialized = DcFalse;
        }
    } else {
        drom_vartristrip->bboxuv = (ddt_g4vll_drom_bboxuv *)0;
    }

    if (vartristrip->uvw_count > 0) {
        drom_vartristrip->bboxuvw = (ddt_g4vll_drom_bboxuvw *)DDspace_Allocate
                (vartristrip->uvw_count *sizeof(ddt_g4vll_drom_bboxuvw));

        for (i=0; i< vartristrip->uvw_count; i++) {
            drom_vartristrip->bboxuvw[i].initialized = DcFalse;
        }
    } else {
        drom_vartristrip->bboxuvw = (ddt_g4vll_drom_bboxuvw *)0;
    }

    /*
     * We will update the drom_vartristrip information only when its
     * sequence number does not match that of the kernel.
     */
    drom_vartristrip->sequence_number = vartristrip->sequence_number;

    return drom_vartristrip;
}
/*
  ======================================================================
 */
void ddr_g4vll_drom_vartristrip_print (
    DtObject object,
    DtInt *data)
{
}
/*
  ======================================================================
 */
void ddr_g4vll_drom_vartristrip_delete (
    DtObject object,
    ddt_g4vll_drom_vartristrip *data)
{
    if (data != (ddt_g4vll_drom_vartristrip *)0) {
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
void ddr_g4vll_drom_vartristrip_render (
    dot_object *object)
{
    struct vartristrip *vartristrip;
    ddt_g4vll_drom_vartristrip *drom_vartristrip;
    ddt_g4vll_drom_bboxuv *bboxuv;
    ddt_g4vll_drom_bboxuvw *bboxuvw;
    dot_object *shader;
    DtInt i;
    DtInt index;

    if (classid == -1) 
	    classid = DsInqClassId("DoVarTriangleStrip");

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

    vartristrip = (struct vartristrip *)(object->data);

    drom_vartristrip = (ddt_g4vll_drom_vartristrip *)
		object->additional_data[dataindex];

    /*
     * Store the vartristrip object's handle globally for the tristrip
     * output routine.
     */
    dde_g4vll_drom_vartristrip = drom_vartristrip;

    /*
     * If we're texture mapping and have UVs, construct the uv bounding box 
     * from the appropriate UV set.  Do this only if the kernel's index or
     * sequence number doesn't match that of the driver's.
     */
    if (vartristrip->uv_count > 0 && dde_g4vll_drom_data.texture_2D_valid) {
        index = vartristrip->uv_count > 
	    	dde_g4vll_drom_data.map_diffuse_color_uv_index ?
            	dde_g4vll_drom_data.map_diffuse_color_uv_index : 0;

        if ((index+1) > drom_vartristrip->uvcount) {
	    /* 
	     * New UV set(s) have been added since we were last through here.
	     * Add'em to the end of the driver's data.
	     */
            drom_vartristrip->bboxuv = (ddt_g4vll_drom_bboxuv *)
                    	DDspace_Reallocate(drom_vartristrip->bboxuv,
                        vartristrip->uv_count * sizeof(ddt_g4vll_drom_bboxuv));
            for (i = drom_vartristrip->uvcount; i < vartristrip->uv_count; i++) {
                drom_vartristrip->bboxuv[i].initialized = DcFalse;
            }
            drom_vartristrip->uvcount = vartristrip->uv_count;
            drom_vartristrip->sequence_number = vartristrip->sequence_number;

        } else if (drom_vartristrip->sequence_number != 
	    	   vartristrip->sequence_number) {
	    /*
	     * One or more of the existing UV set(s) have been changed.
	     * We don't know which one so "uninitialise" all of them.
	     */
            for (i = 0; i < vartristrip->uv_count; i++) {
                drom_vartristrip->bboxuv[i].initialized = DcFalse;
            }
            drom_vartristrip->uvcount = vartristrip->uv_count;
            drom_vartristrip->sequence_number = vartristrip->sequence_number;
	}

        bboxuv = &(drom_vartristrip->bboxuv[index]);

        if (!bboxuv->initialized) {
            float umin, umax, vmin, vmax;
            /*
             * This bounding box has not been set yet so compute it.
             */
            umin = umax = vartristrip->uv_list[index][0][0];
            vmin = vmax = vartristrip->uv_list[index][0][1];

            for (i=1; i< vartristrip->vertex_count; i++) {
                umin = min(umin, vartristrip->uv_list[index][i][0]);
                umax = max(umax, vartristrip->uv_list[index][i][0]);
                vmin = min(vmin, vartristrip->uv_list[index][i][1]);
                vmax = max(vmax, vartristrip->uv_list[index][i][1]);
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
    if (vartristrip->uvw_count > 0 && dde_g4vll_drom_data.texture_3D_valid) {
        index = vartristrip->uvw_count > 
		dde_g4vll_drom_data.map_diffuse_color_uvw_index ?
                dde_g4vll_drom_data.map_diffuse_color_uvw_index : 0;

        if ((index+1) > drom_vartristrip->uvwcount) {
	    /*
	     * New UVW set(s) have been added since we were last through here.
             * Add'em to the end of the driver's data.
             */
            drom_vartristrip->bboxuvw = (ddt_g4vll_drom_bboxuvw *)
                    DDspace_Reallocate(drom_vartristrip->bboxuvw,
                        vartristrip->uvw_count * sizeof(ddt_g4vll_drom_bboxuvw));
            for (i=drom_vartristrip->uvwcount; i<vartristrip->uvw_count; i++) {
                drom_vartristrip->bboxuvw[i].initialized = DcFalse;
            }
            drom_vartristrip->uvwcount = vartristrip->uvw_count;
            drom_vartristrip->sequence_number = vartristrip->sequence_number;
        } else if (drom_vartristrip->sequence_number !=
                   vartristrip->sequence_number) {
            /*
             * One or more of the existing UVW set(s) have been changed.
	     * We don't know which one so "uninitialise" all of them.
             */
            for (i = 0; i < vartristrip->uvw_count; i++) {
                drom_vartristrip->bboxuvw[i].initialized = DcFalse;
            }
            drom_vartristrip->uvwcount = vartristrip->uvw_count;
            drom_vartristrip->sequence_number = vartristrip->sequence_number;
        }

        bboxuvw = &(drom_vartristrip->bboxuvw[index]);

        if (!bboxuvw->initialized) {
            float umin, umax, vmin, vmax, wmin, wmax;
            /*
             * This bounding box has not been set yet so compute it.
             */
            umin = umax = vartristrip->uvw_list[index][0][0];
            vmin = vmax = vartristrip->uvw_list[index][0][1];
            wmin = wmax = vartristrip->uvw_list[index][0][2];

            for (i=1; i< vartristrip->vertex_count; i++) {
                umin = min(umin, vartristrip->uvw_list[index][i][0]);
                umax = max(umax, vartristrip->uvw_list[index][i][0]);
                vmin = min(vmin, vartristrip->uvw_list[index][i][1]);
                vmax = max(vmax, vartristrip->uvw_list[index][i][1]);
                wmin = min(wmin, vartristrip->uvw_list[index][i][2]);
                wmax = max(wmax, vartristrip->uvw_list[index][i][2]);
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
    }

    DDclass_InqMethod(DcTypeAttSurfaceShader, DcMethodInqGlbAttVal)(&shader);
    if (shader != DcShaderConstant) {
    	if (vartristrip->needelemnrms) {
	    if (vartristrip->element_normal == NULL) {
	    	vartristrip->element_normal = (DtRealTriple *)DDspace_Allocate
			    (vartristrip->vertex_count*(sizeof (DtRealTriple)));
	    }
	    dor_vartristrip_calculate_triangle_normals(vartristrip);
	    vartristrip->needelemnrms = DcFalse;
	}
    }

    if (doe_system_timing_display_traverse_count == 1)
        doe_system_timing_strip_triangles_drawn += vartristrip->vertex_count - 2;

    ddr_g4vll_drom_render_triangle_strip (DcRGB,
				       vartristrip->bounding_box_pts,
				       vartristrip->vertex_count,
				       vartristrip->vertex_location,
				       vartristrip->vertex_normal,
				       vartristrip->vertex_color,
				       vartristrip->element_normal,
				       vartristrip->uv_count,
				       vartristrip->uv_list, 
				       vartristrip->uvw_count,
				       vartristrip->uvw_list,
				       DcNullPtr);

    dde_g4vll_drom_vartristrip = (ddt_g4vll_drom_vartristrip *)0;

}
/*
  ======================================================================
 */
