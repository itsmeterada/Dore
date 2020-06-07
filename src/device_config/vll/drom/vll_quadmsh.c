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
	ddt_vll_drom_quadmsh * ddr_vll_drom_quadmsh_create (object)
	void ddr_vll_drom_quadmsh_print (object, data)
	void ddr_vll_drom_quadmsh_delete (object, data)
	void ddr_vll_drom_quadmsh_render (object)

  ======================================================================
 */
#include "drom.h"
#include "dore/dore_develop/private/quadmsh.h"

/* 
 * Structure for additional data required by VLL.  uvvertexlist is a
 * uv connectivity list; bboxuv and bboxuvw are convex hulls enclosing all 
 * texture indices of the current texture, specified in uv and uvw space,
 * respectively.
 */
typedef struct {
    DtInt *			uvvertexlist;
    ddt_vll_drom_bboxuv *	bboxuv;
    ddt_vll_drom_bboxuvw *	bboxuvw;
} ddt_vll_drom_quadmsh;

/*
 ======================================================================
 */
static DtInt classid = -1;
static DtInt dataindex = -1;

/*
  ======================================================================
 */
ddt_vll_drom_quadmsh *
ddr_vll_drom_quadmsh_create(object)
DtObjectStructure *object;
{
    struct quadmsh *quadmsh;
    ddt_vll_drom_quadmsh *drom_quadmsh;
    int i;

    quadmsh = (struct quadmsh *)(object->data);

    drom_quadmsh =  (ddt_vll_drom_quadmsh *)
	    DDspace_Allocate(sizeof(ddt_vll_drom_quadmsh));

    drom_quadmsh->uvvertexlist = (DtInt *)0;

    if (quadmsh->uv_count > 0) {
	drom_quadmsh->bboxuv = (ddt_vll_drom_bboxuv *)DDspace_Allocate
		(quadmsh->uv_count *sizeof(ddt_vll_drom_bboxuv));

	for (i=0; i< quadmsh->uv_count; i++) {
	    drom_quadmsh->bboxuv[i].initialized = DcFalse;
	}
    } else {
	drom_quadmsh->bboxuv = (ddt_vll_drom_bboxuv *)0;
    }

    if (quadmsh->uvw_count > 0) {
        drom_quadmsh->bboxuvw = (ddt_vll_drom_bboxuvw *)DDspace_Allocate
                (quadmsh->uvw_count *sizeof(ddt_vll_drom_bboxuvw));

        for (i=0; i< quadmsh->uvw_count; i++) {
            drom_quadmsh->bboxuvw[i].initialized = DcFalse;
        }
    } else {
        drom_quadmsh->bboxuvw = (ddt_vll_drom_bboxuvw *)0;
    }

    return drom_quadmsh;
}
/*
  ======================================================================
 */
void
ddr_vll_drom_quadmsh_print(object, data)
DtObject object;
DtInt *data;
{
}
/*
  ======================================================================
 */
void
ddr_vll_drom_quadmsh_delete(object, data)
DtObject object;
ddt_vll_drom_quadmsh *data;
{
    if (data != (ddt_vll_drom_quadmsh *)0) {
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
ddr_vll_drom_quadmsh_render(object)
dot_object *object;
{
    extern DtInt *dde_vll_quad_mesh_uvvertexlist;
    extern DtRealCouple *dde_vll_quad_mesh_bboxuv;
    extern DtRealTriple *dde_vll_quad_mesh_bboxuvw;
    struct quadmsh *quadmsh;
    DtInt i, j, vertexno;
    quadmsh_vertexlist *uvvertexlist;
    ddt_vll_drom_quadmsh *drom_quadmsh;
    ddt_vll_drom_bboxuv *bboxuv;
    ddt_vll_drom_bboxuvw *bboxuvw;
    DtInt quadno;
    DtInt idx;
    
    if (classid == -1) 
	    classid = DsInqClassId("DoQuadMesh");

    /* Determine index into quad mesh's additional data */
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

    quadmsh = (struct quadmsh *)(object->data);

    drom_quadmsh = (ddt_vll_drom_quadmsh *)object->additional_data[dataindex];

    uvvertexlist = (quadmsh_vertexlist *) drom_quadmsh->uvvertexlist;

    if (uvvertexlist == NULL && quadmsh->uv_count > 0) {
	uvvertexlist = (quadmsh_vertexlist *)DDspace_Allocate
		(quadmsh->quad_count*(sizeof (quadmsh_vertexlist)));

	for (quadno=0;quadno<quadmsh->quad_count;quadno++) {
	    uvvertexlist[quadno][0] =
		    quadmsh->quad_vertexlist[quadno][0] * 2;
	    uvvertexlist[quadno][1] =
		    quadmsh->quad_vertexlist[quadno][1] * 2;
	    uvvertexlist[quadno][2] =
		    quadmsh->quad_vertexlist[quadno][2] * 2;
	    uvvertexlist[quadno][3] =
		    quadmsh->quad_vertexlist[quadno][3] * 2;
	}
	drom_quadmsh->uvvertexlist = uvvertexlist;
    }

    if (quadmsh->uv_count > 0 && dde_vll_drom_data.texture_2D_valid) {
	idx = quadmsh->uv_count > dde_vll_drom_data.map_diffuse_color_uv_index ?
		dde_vll_drom_data.map_diffuse_color_uv_index : 0;

	bboxuv = &(drom_quadmsh->bboxuv[idx]);

	if (!bboxuv->initialized) {
	    DtReal umin, umax, vmin, vmax;
	    /*
	     * this bounding box has not been set yet so compute bounding
	     * box
	     */

            vertexno = quadmsh->quad_vertexlist[0][0];
	    umin = umax = quadmsh->uv_list[idx][vertexno][0];
	    vmin = vmax = quadmsh->uv_list[idx][vertexno][1];

            for (i=1; i<quadmsh->quad_count; i++) {
                for (j=0; j<4; j++) {
                    /* use only the vertices referenced in the mesh */
                    vertexno = quadmsh->quad_vertexlist[i][j];

                    umin = min(umin, quadmsh->uv_list[idx][vertexno][0]);
                    umax = max(umax, quadmsh->uv_list[idx][vertexno][0]);
                    vmin = min(vmin, quadmsh->uv_list[idx][vertexno][1]);
                    vmax = max(vmax, quadmsh->uv_list[idx][vertexno][1]);
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
	dde_vll_quad_mesh_bboxuv = bboxuv->bbox;
    }

    if (quadmsh->uvw_count > 0 && dde_vll_drom_data.texture_3D_valid) {
        idx = quadmsh->uvw_count > dde_vll_drom_data.map_diffuse_color_uvw_index ?
                dde_vll_drom_data.map_diffuse_color_uvw_index : 0;

        bboxuvw = &(drom_quadmsh->bboxuvw[idx]);

        if (!bboxuvw->initialized) {
            DtReal umin, umax, vmin, vmax, wmin, wmax;
            /*
             * this bounding box has not been set yet so compute bounding
             * box
             */

            vertexno = quadmsh->quad_vertexlist[0][0];
            umin = umax = quadmsh->uvw_list[idx][vertexno][0];
            vmin = vmax = quadmsh->uvw_list[idx][vertexno][1];
            wmin = wmax = quadmsh->uvw_list[idx][vertexno][2];

            for (i=1; i<quadmsh->quad_count; i++) {
                for (j=0; j<4; j++) {
                    /* use only the vertices referenced in the mesh */
                    vertexno = quadmsh->quad_vertexlist[i][j];

                    umin = min(umin, quadmsh->uvw_list[idx][vertexno][0]);
                    umax = max(umax, quadmsh->uvw_list[idx][vertexno][0]);
                    vmin = min(vmin, quadmsh->uvw_list[idx][vertexno][1]);
                    vmax = max(vmax, quadmsh->uvw_list[idx][vertexno][1]);
                    wmin = min(wmin, quadmsh->uvw_list[idx][vertexno][2]);
                    wmax = max(wmax, quadmsh->uvw_list[idx][vertexno][2]);
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
        dde_vll_quad_mesh_bboxuvw = bboxuvw->bbox;
    }

    if (quadmsh->compiled_quad_vertexlist == NULL) {
	quadmsh->compiled_quad_vertexlist = (quadmsh_vertexlist *)DDspace_Allocate
		(quadmsh->quad_count*(sizeof (quadmsh_vertexlist)));

	for (quadno=0;quadno<quadmsh->quad_count;quadno++) {
	    quadmsh->compiled_quad_vertexlist[quadno][0] =
		    quadmsh->quad_vertexlist[quadno][0] * 3;
	    quadmsh->compiled_quad_vertexlist[quadno][1] =
		    quadmsh->quad_vertexlist[quadno][1] * 3;
	    quadmsh->compiled_quad_vertexlist[quadno][2] =
		    quadmsh->quad_vertexlist[quadno][2] * 3;
	    quadmsh->compiled_quad_vertexlist[quadno][3] =
		    quadmsh->quad_vertexlist[quadno][3] * 3;
	}
    }

    doe_system_timing_mesh_quads_drawn += quadmsh->quad_count;

    dde_vll_quad_mesh_uvvertexlist = (DtInt *)uvvertexlist;

    ddr_vll_drom_render_quad_mesh (DcRGB,
				   quadmsh->bounding_box_pts,
				   quadmsh->quad_count,
				   quadmsh->element_normal,
				   quadmsh->vertex_count,
				   quadmsh->vertex_location,
				   quadmsh->vertex_normal,
				   quadmsh->vertex_color,
				   quadmsh->quad_vertexlist,
				   quadmsh->compiled_quad_vertexlist,
				   quadmsh->uv_count,
				   quadmsh->uv_list,
				   quadmsh->uvw_count,
				   quadmsh->uvw_list);

    dde_vll_quad_mesh_uvvertexlist = (DtInt *)0;
    dde_vll_quad_mesh_bboxuv = (DtRealCouple *)0;
    dde_vll_quad_mesh_bboxuvw = (DtRealTriple *)0;
}
/*
  ======================================================================
 */
