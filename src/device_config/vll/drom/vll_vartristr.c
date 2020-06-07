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
	ddt_vll_drom_vartristrip * ddr_vll_drom_vartristrip_create (object)
	void ddr_vll_drom_vartristrip_print (object, data)
	void ddr_vll_drom_vartristrip_delete (object, data)
	void ddr_vll_drom_vartristrip_render (object)

  ======================================================================
 */
#include "drom.h"
#include "dore/dore_develop/private/vartristrp.h"

/*
 * Structure for additional data required by VLL.  uvvertexlist is a
 * uv connectivity list; bboxuv and bboxuvw are convex hulls enclosing all
 * texture indices of the current texture, specified in uv and uvw space,
 * respectively.
 */

typedef struct {
    DtInt *			uvvertexlist;
    DtInt 			uvcount;
    DtInt 			uvwcount;
    ddt_vll_drom_bboxuv *	bboxuv;
    ddt_vll_drom_bboxuvw *	bboxuvw;
} ddt_vll_drom_vartristrip;
/*
 ======================================================================
 */
static DtInt classid = -1;
static DtInt dataindex = -1;

/*
  ======================================================================
 */
ddt_vll_drom_vartristrip *
ddr_vll_drom_vartristrip_create(object)
DtObjectStructure *object;
{
    struct vartristrip *vartristrip;
    ddt_vll_drom_vartristrip *drom_vartristrip;
    int i;

    vartristrip = (struct vartristrip *)(object->data);

    drom_vartristrip =  (ddt_vll_drom_vartristrip *)
	    DDspace_Allocate(sizeof(ddt_vll_drom_vartristrip));

    drom_vartristrip->uvvertexlist = (DtInt *)0;

    drom_vartristrip->uvcount = vartristrip->uv_count;
    drom_vartristrip->uvwcount = vartristrip->uvw_count;

    if (vartristrip->uv_count > 0) {
	drom_vartristrip->bboxuv = (ddt_vll_drom_bboxuv *)DDspace_Allocate
		(vartristrip->uv_count *sizeof(ddt_vll_drom_bboxuv));

	for (i=0; i< vartristrip->uv_count; i++) {
	    drom_vartristrip->bboxuv[i].initialized = DcFalse;
	}
    } else {
	drom_vartristrip->bboxuv = (ddt_vll_drom_bboxuv *)0;
    }

    if (vartristrip->uvw_count > 0) {
        drom_vartristrip->bboxuvw = (ddt_vll_drom_bboxuvw *)DDspace_Allocate
                (vartristrip->uvw_count *sizeof(ddt_vll_drom_bboxuvw));

        for (i=0; i< vartristrip->uvw_count; i++) {
            drom_vartristrip->bboxuvw[i].initialized = DcFalse;
        }
    } else {
        drom_vartristrip->bboxuvw = (ddt_vll_drom_bboxuvw *)0;
    }

    return drom_vartristrip;
}
/*
  ======================================================================
 */
void
ddr_vll_drom_vartristrip_print(object, data)
DtObject object;
DtInt *data;
{
}
/*
  ======================================================================
 */
void
ddr_vll_drom_vartristrip_delete(object, data)
DtObject object;
ddt_vll_drom_vartristrip *data;
{
    if (data != (ddt_vll_drom_vartristrip *)0) {
	if (data->uvvertexlist != (DtInt *)0) {
	    DDspace_Deallocate(data->uvvertexlist);
	}
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
ddr_vll_drom_vartristrip_render(object)
dot_object *object;
{
    extern DtInt *dde_vll_triangle_mesh_uvvertexlist;
    extern DtRealCouple *dde_vll_triangle_mesh_bboxuv;
    extern DtRealTriple *dde_vll_triangle_mesh_bboxuvw;
    struct vartristrip *vartristrip;
    ddt_vll_drom_vartristrip *drom_vartristrip;
    ddt_vll_drom_bboxuv *bboxuv;
    ddt_vll_drom_bboxuvw *bboxuvw;
    DtInt i;
    DtIntTriple *uvvertexlist;
    DtInt triangleno;
    DtInt odd;
    DtInt basevtx;
    DtInt idx;

    if (classid == -1) 
	    classid = DsInqClassId("DoVarTriangleStrip");

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

    vartristrip = (struct vartristrip *)(object->data);

    drom_vartristrip = (ddt_vll_drom_vartristrip *)object->additional_data[dataindex];

    uvvertexlist = (DtIntTriple *) drom_vartristrip->uvvertexlist;

    if (vartristrip->triangle_vertexlist == NULL) {
	vartristrip->triangle_vertexlist = (DtIntTriple *)
		DDspace_Allocate((vartristrip->vertex_count-2)*
				   (sizeof (DtIntTriple)));
	for (basevtx=0, odd=DcTrue; basevtx<vartristrip->vertex_count-2; basevtx++){
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
	 
    if (uvvertexlist == NULL && vartristrip->uv_count > 0) {
	uvvertexlist = (DtIntTriple *)DDspace_Allocate
		((vartristrip->vertex_count - 2)*(sizeof (DtIntTriple)));

	for (triangleno=0;triangleno<vartristrip->vertex_count - 2;triangleno++) {
	    uvvertexlist[triangleno][0] =
		    vartristrip->triangle_vertexlist[triangleno][0] * 2;
	    uvvertexlist[triangleno][1] =
		    vartristrip->triangle_vertexlist[triangleno][1] * 2;
	    uvvertexlist[triangleno][2] =
		    vartristrip->triangle_vertexlist[triangleno][2] * 2;
	}
	drom_vartristrip->uvvertexlist = uvvertexlist;
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

    if (vartristrip->uv_count > 0 && dde_vll_drom_data.texture_2D_valid) {
	idx = vartristrip->uv_count > dde_vll_drom_data.map_diffuse_color_uv_index ?
		dde_vll_drom_data.map_diffuse_color_uv_index : 0;

	if ((idx+1) > drom_vartristrip->uvcount) {
	    drom_vartristrip->bboxuv = (ddt_vll_drom_bboxuv *)
		    DDspace_Reallocate(drom_vartristrip->bboxuv, 
			vartristrip->uv_count * sizeof(ddt_vll_drom_bboxuv));
	    for (i=drom_vartristrip->uvcount; i<vartristrip->uv_count; i++) {
		drom_vartristrip->bboxuv[i].initialized = DcFalse;
	    }
	    drom_vartristrip->uvcount = vartristrip->uv_count;
	}

	bboxuv = &(drom_vartristrip->bboxuv[idx]);

	if (!bboxuv->initialized) {
	    float umin, umax, vmin, vmax;
	    /*
	     * this bounding box has not been set yet so compute bounding
	     * box
	     */

	    umin = umax = vartristrip->uv_list[idx][0][0];
	    vmin = vmax = vartristrip->uv_list[idx][0][1];

	    for (i=1; i< vartristrip->vertex_count; i++) {
		umin = min(umin, vartristrip->uv_list[idx][i][0]);
		umax = max(umax, vartristrip->uv_list[idx][i][0]);
		vmin = min(vmin, vartristrip->uv_list[idx][i][1]);
		vmax = max(vmax, vartristrip->uv_list[idx][i][1]);
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
	dde_vll_triangle_mesh_bboxuv = bboxuv->bbox;
    }

    if (vartristrip->uvw_count > 0 && dde_vll_drom_data.texture_3D_valid) {
        idx = vartristrip->uvw_count > dde_vll_drom_data.map_diffuse_color_uvw_index ?
                dde_vll_drom_data.map_diffuse_color_uvw_index : 0;

       if ((idx+1) > drom_vartristrip->uvwcount) {
            drom_vartristrip->bboxuvw = (ddt_vll_drom_bboxuvw *)
                    DDspace_Reallocate(drom_vartristrip->bboxuvw,
                        vartristrip->uvw_count * sizeof(ddt_vll_drom_bboxuvw));
            for (i=drom_vartristrip->uvwcount; i<vartristrip->uvw_count; i++) {
                drom_vartristrip->bboxuvw[i].initialized = DcFalse;
            }
            drom_vartristrip->uvwcount = vartristrip->uvw_count;
        }

        bboxuvw = &(drom_vartristrip->bboxuvw[idx]);

        if (!bboxuvw->initialized) {
            float umin, umax, vmin, vmax, wmin, wmax;
            /*
             * this bounding box has not been set yet so compute bounding
             * box
             */

            umin = umax = vartristrip->uvw_list[idx][0][0];
            vmin = vmax = vartristrip->uvw_list[idx][0][1];
            wmin = wmax = vartristrip->uvw_list[idx][0][2];

            for (i=1; i< vartristrip->vertex_count; i++) {
                umin = min(umin, vartristrip->uvw_list[idx][i][0]);
                umax = max(umax, vartristrip->uvw_list[idx][i][0]);
                vmin = min(vmin, vartristrip->uvw_list[idx][i][1]);
                vmax = max(vmax, vartristrip->uvw_list[idx][i][1]);
                wmin = min(wmin, vartristrip->uvw_list[idx][i][2]);
                wmax = max(wmax, vartristrip->uvw_list[idx][i][2]);
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
        dde_vll_triangle_mesh_bboxuvw = bboxuvw->bbox;
    }

    doe_system_timing_mesh_triangles_drawn += vartristrip->vertex_count - 2;

    dde_vll_triangle_mesh_uvvertexlist = (DtInt *)uvvertexlist;

    ddr_vll_drom_render_triangle_mesh (DcRGB,
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
				       vartristrip->uvw_list);

    dde_vll_triangle_mesh_uvvertexlist = (DtInt *)0;
    dde_vll_triangle_mesh_bboxuv = (DtRealCouple *)0;
    dde_vll_triangle_mesh_bboxuvw = (DtRealTriple *)0;
}
/*
  ======================================================================
 */
