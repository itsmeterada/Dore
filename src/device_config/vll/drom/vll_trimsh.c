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
	ddt_vll_drom_trimsh * ddr_vll_drom_trimsh_create (object)
	void ddr_vll_drom_trimsh_print (object, data)
	void ddr_vll_drom_trimsh_delete (object, data)
	void ddr_vll_drom_trimsh_render (object)

  ======================================================================
 */
#include "drom.h"
#include "dore/dore_develop/private/trimsh.h"

/*
 * Structure for additional data required by VLL.  uvvertexlist is a
 * uv connectivity list; bboxuv and bboxuvw are convex hulls enclosing 
 * all texture indices of the current texture, specified in uv and uvw 
 * space, respectively.
 */

typedef struct {
    DtInt *			uvvertexlist;
    ddt_vll_drom_bboxuv *	bboxuv;
    ddt_vll_drom_bboxuvw *	bboxuvw;
} ddt_vll_drom_trimsh;

/*
 ======================================================================
 */
static DtInt classid = -1;
static DtInt dataindex = -1;

/*
  ======================================================================
 */
ddt_vll_drom_trimsh *
ddr_vll_drom_trimsh_create(object)
DtObjectStructure *object;
{
    struct trimsh *trimsh;
    ddt_vll_drom_trimsh *drom_trimsh;
    int i;

    trimsh = (struct trimsh *)(object->data);

    drom_trimsh =  (ddt_vll_drom_trimsh *)
	    DDspace_Allocate(sizeof(ddt_vll_drom_trimsh));

    drom_trimsh->uvvertexlist = (DtInt *)0;

    if (trimsh->uv_count > 0) {
	drom_trimsh->bboxuv = (ddt_vll_drom_bboxuv *)DDspace_Allocate
		(trimsh->uv_count *sizeof(ddt_vll_drom_bboxuv));

	for (i=0; i< trimsh->uv_count; i++) {
	    drom_trimsh->bboxuv[i].initialized = DcFalse;
	}
    } else {
	drom_trimsh->bboxuv = (ddt_vll_drom_bboxuv *)0;
    }

    if (trimsh->uvw_count > 0) {
	drom_trimsh->bboxuvw = (ddt_vll_drom_bboxuvw *)DDspace_Allocate
		(trimsh->uvw_count *sizeof(ddt_vll_drom_bboxuvw));

	for (i=0; i< trimsh->uvw_count; i++) {
	    drom_trimsh->bboxuvw[i].initialized = DcFalse;
	}
    } else {
	drom_trimsh->bboxuvw = (ddt_vll_drom_bboxuvw *)0;
    }

    return drom_trimsh;
}
/*
  ======================================================================
 */
void
ddr_vll_drom_trimsh_print(object, data)
DtObject object;
DtInt *data;
{
}
/*
  ======================================================================
 */
void
ddr_vll_drom_trimsh_delete(object, data)
DtObject object;
ddt_vll_drom_trimsh *data;
{
    if (data != (ddt_vll_drom_trimsh *)0) {
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
ddr_vll_drom_trimsh_render(object)
dot_object *object;
{
    extern DtInt *dde_vll_triangle_mesh_uvvertexlist;
    extern DtRealCouple *dde_vll_triangle_mesh_bboxuv;
    extern DtRealTriple *dde_vll_triangle_mesh_bboxuvw;
    struct trimsh *trimsh;
    DtInt i, j, vertexno;
    DtIntTriple *uvvertexlist;
    ddt_vll_drom_trimsh *drom_trimsh;
    ddt_vll_drom_bboxuv *bboxuv;
    ddt_vll_drom_bboxuvw *bboxuvw;
    DtInt triangleno;
    DtInt idx;
    
    if (classid == -1) 
	    classid = DsInqClassId("DoTriangleMesh");

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

    trimsh = (struct trimsh *)(object->data);

    drom_trimsh = (ddt_vll_drom_trimsh *)object->additional_data[dataindex];

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
	drom_trimsh->uvvertexlist = uvvertexlist;
    }

    if (trimsh->uv_count > 0 && dde_vll_drom_data.texture_2D_valid) {
	idx = trimsh->uv_count > dde_vll_drom_data.map_diffuse_color_uv_index ?
		dde_vll_drom_data.map_diffuse_color_uv_index : 0;

	bboxuv = &(drom_trimsh->bboxuv[idx]);

	if (!bboxuv->initialized) {
	    DtReal umin, umax, vmin, vmax;
	    /*
	     * this bounding box has not been set yet so compute bounding
	     * box
	     */

            vertexno = trimsh->triangle_vertexlist[0][0];
	    umin = umax = trimsh->uv_list[idx][vertexno][0];
	    vmin = vmax = trimsh->uv_list[idx][vertexno][1];

	    for (i=1; i<trimsh->triangle_count; i++) {
                for (j=0; j<3; j++) {
                    /* use only the vertices referenced in the mesh */
                    vertexno = trimsh->triangle_vertexlist[i][j];

		    umin = min(umin, trimsh->uv_list[idx][vertexno][0]);
		    umax = max(umax, trimsh->uv_list[idx][vertexno][0]);
		    vmin = min(vmin, trimsh->uv_list[idx][vertexno][1]);
		    vmax = max(vmax, trimsh->uv_list[idx][vertexno][1]);
                }
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

    if (trimsh->uvw_count > 0 && dde_vll_drom_data.texture_3D_valid) {
        idx = trimsh->uvw_count > dde_vll_drom_data.map_diffuse_color_uvw_index ?
                dde_vll_drom_data.map_diffuse_color_uvw_index : 0;

        bboxuvw = &(drom_trimsh->bboxuvw[idx]);

        if (!bboxuvw->initialized) {
            DtReal umin, umax, vmin, vmax, wmin, wmax;
            /*
             * this bounding box has not been set yet so compute bounding
             * box
             */

            vertexno = trimsh->triangle_vertexlist[0][0];
            umin = umax = trimsh->uvw_list[idx][vertexno][0];
            vmin = vmax = trimsh->uvw_list[idx][vertexno][1];
            wmin = wmax = trimsh->uvw_list[idx][vertexno][2];

	    for (i=1; i<trimsh->triangle_count; i++) {
                for (j=0; j<3; j++) {
                    /* use only the vertices referenced in the mesh */
                    vertexno = trimsh->triangle_vertexlist[i][j];

		    umin = min(umin, trimsh->uvw_list[idx][vertexno][0]);
		    umax = max(umax, trimsh->uvw_list[idx][vertexno][0]);
		    vmin = min(vmin, trimsh->uvw_list[idx][vertexno][1]);
		    vmax = max(vmax, trimsh->uvw_list[idx][vertexno][1]);
                    wmin = min(wmin, trimsh->uvw_list[idx][vertexno][2]);
                    wmax = max(wmax, trimsh->uvw_list[idx][vertexno][2]);
                }
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

    doe_system_timing_mesh_triangles_drawn += trimsh->triangle_count;

    dde_vll_triangle_mesh_uvvertexlist = (DtInt *)uvvertexlist;

    ddr_vll_drom_render_triangle_mesh (DcRGB,
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
				       trimsh->uvw_list);

    dde_vll_triangle_mesh_uvvertexlist = (DtInt *)0;
    dde_vll_triangle_mesh_bboxuv = (DtRealCouple *)0;
    dde_vll_triangle_mesh_bboxuvw = (DtRealTriple *)0;
}
/*
  ======================================================================
 */
