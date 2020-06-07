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
	ddt_vll_drom_vartrimsh * ddr_vll_drom_vartrimsh_create (object)
	void ddr_vll_drom_vartrimsh_print (object, data)
	void ddr_vll_drom_vartrimsh_delete (object, data)
	void ddr_vll_drom_vartrimsh_render (object)

  ======================================================================
 */
#include "drom.h"
#include "dore/dore_develop/private/vartrimsh.h"

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
} ddt_vll_drom_vartrimsh;

/*
 ======================================================================
 */
static DtInt classid = -1;
static DtInt dataindex = -1;

/*
  ======================================================================
 */
ddt_vll_drom_vartrimsh *
ddr_vll_drom_vartrimsh_create(object)
DtObjectStructure *object;
{
    struct vartrimsh *vartrimsh;
    ddt_vll_drom_vartrimsh *drom_vartrimsh;
    int i;

    vartrimsh = (struct vartrimsh *)(object->data);

    drom_vartrimsh =  (ddt_vll_drom_vartrimsh *)
	    DDspace_Allocate(sizeof(ddt_vll_drom_vartrimsh));

    drom_vartrimsh->uvvertexlist = (DtInt *)0;

    drom_vartrimsh->uvcount = vartrimsh->uv_count;
    drom_vartrimsh->uvwcount = vartrimsh->uvw_count;

    if (vartrimsh->uv_count > 0) {
	drom_vartrimsh->bboxuv = (ddt_vll_drom_bboxuv *)DDspace_Allocate
		(vartrimsh->uv_count *sizeof(ddt_vll_drom_bboxuv));

	for (i=0; i< vartrimsh->uv_count; i++) {
	    drom_vartrimsh->bboxuv[i].initialized = DcFalse;
	}
    } else {
	drom_vartrimsh->bboxuv = (ddt_vll_drom_bboxuv *)0;
    }

    if (vartrimsh->uvw_count > 0) {
        drom_vartrimsh->bboxuvw = (ddt_vll_drom_bboxuvw *)DDspace_Allocate
                (vartrimsh->uvw_count *sizeof(ddt_vll_drom_bboxuvw));

        for (i=0; i< vartrimsh->uvw_count; i++) {
            drom_vartrimsh->bboxuvw[i].initialized = DcFalse;
        }
    } else {
        drom_vartrimsh->bboxuvw = (ddt_vll_drom_bboxuvw *)0;
    }

    return drom_vartrimsh;
}
/*
  ======================================================================
 */
void
ddr_vll_drom_vartrimsh_print(object, data)
DtObject object;
DtInt *data;
{
}
/*
  ======================================================================
 */
void
ddr_vll_drom_vartrimsh_delete(object, data)
DtObject object;
ddt_vll_drom_vartrimsh *data;
{
    if (data != (ddt_vll_drom_vartrimsh *)0) {
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
ddr_vll_drom_vartrimsh_render(object)
dot_object *object;
{
    extern DtInt *dde_vll_triangle_mesh_uvvertexlist;
    extern DtRealCouple *dde_vll_triangle_mesh_bboxuv;
    extern DtRealTriple *dde_vll_triangle_mesh_bboxuvw;
    extern DtInt *dde_vll_triangle_mesh_edge_enable;
    struct vartrimsh *vartrimsh;
    DtInt i, j, vertexno;
    DtIntTriple *uvvertexlist;
    ddt_vll_drom_vartrimsh *drom_vartrimsh;
    ddt_vll_drom_bboxuv *bboxuv;
    ddt_vll_drom_bboxuvw *bboxuvw;
    DtInt triangleno;
    dot_pobject shader;
    DtInterpType interptype;
    static DtPFI reptype_query = (DtPFI)DcNullPtr;
    DtRepType reptype;
    DtInt idx;

    if (classid == -1) 
	    classid = DsInqClassId("DoVarTriangleMesh");

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

    vartrimsh = (struct vartrimsh *)(object->data);

    drom_vartrimsh = (ddt_vll_drom_vartrimsh *)object->additional_data[dataindex];

    uvvertexlist = (DtIntTriple *) drom_vartrimsh->uvvertexlist;

    if (uvvertexlist == NULL && vartrimsh->uv_count > 0) {
	uvvertexlist = (vartrimsh_vertexlist *)DDspace_Allocate
		(vartrimsh->triangle_count*(sizeof (vartrimsh_vertexlist)));

	for (triangleno=0;triangleno<vartrimsh->triangle_count;triangleno++) {
	    uvvertexlist[triangleno][0] =
		    vartrimsh->triangle_vertexlist[triangleno][0] * 2;
	    uvvertexlist[triangleno][1] =
		    vartrimsh->triangle_vertexlist[triangleno][1] * 2;
	    uvvertexlist[triangleno][2] =
		    vartrimsh->triangle_vertexlist[triangleno][2] * 2;
	}
	drom_vartrimsh->uvvertexlist = uvvertexlist;
    }

    if (vartrimsh->compiled_triangle_vertexlist == NULL) {
	vartrimsh->compiled_triangle_vertexlist = (vartrimsh_vertexlist *)
		DDspace_Allocate (vartrimsh->triangle_count*(
		sizeof (vartrimsh_vertexlist)));

	for (triangleno=0;triangleno<vartrimsh->triangle_count;triangleno++) {
	    vartrimsh->compiled_triangle_vertexlist[triangleno][0] =
		    vartrimsh->triangle_vertexlist[triangleno][0] * 3;
	    vartrimsh->compiled_triangle_vertexlist[triangleno][1] =
		    vartrimsh->triangle_vertexlist[triangleno][1] * 3;
	    vartrimsh->compiled_triangle_vertexlist[triangleno][2] =
		    vartrimsh->triangle_vertexlist[triangleno][2] * 3;
	}
    }

    if (vartrimsh->uv_count > 0 && dde_vll_drom_data.texture_2D_valid) {
	idx = vartrimsh->uv_count > dde_vll_drom_data.map_diffuse_color_uv_index ?
		dde_vll_drom_data.map_diffuse_color_uv_index : 0;

	if ((idx+1) > drom_vartrimsh->uvcount) {
	    drom_vartrimsh->bboxuv = (ddt_vll_drom_bboxuv *)
		    DDspace_Reallocate(drom_vartrimsh->bboxuv, vartrimsh->uv_count *
					 sizeof(ddt_vll_drom_bboxuv));
	    for (i=drom_vartrimsh->uvcount; i<vartrimsh->uv_count; i++) {
		drom_vartrimsh->bboxuv[i].initialized = DcFalse;
	    }
	    drom_vartrimsh->uvcount = vartrimsh->uv_count;
	}

	bboxuv = &(drom_vartrimsh->bboxuv[idx]);

	if (!bboxuv->initialized) {
	    float umin, umax, vmin, vmax;
	    /*
	     * this bounding box has not been set yet so compute bounding
	     * box
	     */

            vertexno = vartrimsh->triangle_vertexlist[0][0];
            umin = umax = vartrimsh->uv_list[idx][vertexno][0];
            vmin = vmax = vartrimsh->uv_list[idx][vertexno][1];

            for (i=1; i<vartrimsh->triangle_count; i++) {
                for (j=0; j<3; j++) {
                    /* use only the vertices referenced in the mesh */
                    vertexno = vartrimsh->triangle_vertexlist[i][j];

                    umin = min(umin, vartrimsh->uv_list[idx][vertexno][0]);
                    umax = max(umax, vartrimsh->uv_list[idx][vertexno][0]);
                    vmin = min(vmin, vartrimsh->uv_list[idx][vertexno][1]);
                    vmax = max(vmax, vartrimsh->uv_list[idx][vertexno][1]);
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

    if (vartrimsh->uvw_count > 0 && dde_vll_drom_data.texture_3D_valid) {
        idx = vartrimsh->uvw_count > dde_vll_drom_data.map_diffuse_color_uvw_index ?
                dde_vll_drom_data.map_diffuse_color_uvw_index : 0;

       if ((idx+1) > drom_vartrimsh->uvwcount) {
            drom_vartrimsh->bboxuvw = (ddt_vll_drom_bboxuvw *)
                    DDspace_Reallocate(drom_vartrimsh->bboxuvw, 
			vartrimsh->uvw_count * sizeof(ddt_vll_drom_bboxuvw));
            for (i=drom_vartrimsh->uvwcount; i<vartrimsh->uvw_count; i++) {
                drom_vartrimsh->bboxuvw[i].initialized = DcFalse;
            }
            drom_vartrimsh->uvwcount = vartrimsh->uvw_count;
        }

        bboxuvw = &(drom_vartrimsh->bboxuvw[idx]);

        if (!bboxuvw->initialized) {
            float umin, umax, vmin, vmax, wmin, wmax;
            /*
             * this bounding box has not been set yet so compute bounding
             * box
             */

            vertexno = vartrimsh->triangle_vertexlist[0][0];
            umin = umax = vartrimsh->uvw_list[idx][vertexno][0];
            vmin = vmax = vartrimsh->uvw_list[idx][vertexno][1];
            wmin = wmax = vartrimsh->uvw_list[idx][vertexno][2];

            for (i=1; i<vartrimsh->triangle_count; i++) {
                for (j=0; j<3; j++) {
                    /* use only the vertices referenced in the mesh */
                    vertexno = vartrimsh->triangle_vertexlist[i][j];

                    umin = min(umin, vartrimsh->uvw_list[idx][vertexno][0]);
                    umax = max(umax, vartrimsh->uvw_list[idx][vertexno][0]);
                    vmin = min(vmin, vartrimsh->uvw_list[idx][vertexno][1]);
                    vmax = max(vmax, vartrimsh->uvw_list[idx][vertexno][1]);
                    wmin = min(wmin, vartrimsh->uvw_list[idx][vertexno][2]);
                    wmax = max(wmax, vartrimsh->uvw_list[idx][vertexno][2]);
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

    DDclass_InqMethod(DcTypeAttSurfaceShader, DcMethodInqGlbAttVal)(&shader);
    if (shader != DcShaderConstant) {
	if (vartrimsh->needelemnrms) {
	    if (vartrimsh->element_normal == NULL) {
		vartrimsh->element_normal =
			(DtRealTriple *)DDspace_Allocate
				(vartrimsh->triangle_count*(sizeof (DtRealTriple)));
	    }
	    dor_vartrimsh_calculate_triangle_normals(vartrimsh);
	    vartrimsh->needelemnrms = DcFalse;
	}

	DDclass_InqMethod(DcTypeAttInterpType, DcMethodInqGlbAttVal)(&interptype);
	if ((interptype == DcVertexShade) && vartrimsh->needvertnrms) {
	    if (vartrimsh->vertex_normal == NULL) {
		vartrimsh->vertex_normal =
			(DtRealTriple *)DDspace_Allocate
				(vartrimsh->vertex_count*(sizeof (DtRealTriple)));
	    }
	    dor_vartrimsh_calculate_vertex_normals(vartrimsh);
	    vartrimsh->needvertnrms = DcFalse;
	}
    }

    doe_system_timing_mesh_triangles_drawn += vartrimsh->triangle_count;

    if (reptype_query == (DtPFI)DcNullPtr) {
        reptype_query = DDclass_InqMethod (DsInqClassId("AttRepType"),
					   DcMethodInqGlbAttVal);
    }
    (*reptype_query)(&reptype);

    if (reptype == DcSurfaceOutlineEdges && 
	vartrimsh->edge_enable != NULL) {
	dde_vll_triangle_mesh_edge_enable = vartrimsh->edge_enable;
    }

    dde_vll_triangle_mesh_uvvertexlist = (DtInt *)uvvertexlist;

    ddr_vll_drom_render_triangle_mesh (DcRGB,
				       vartrimsh->bounding_box_pts,
				       vartrimsh->triangle_count,
				       vartrimsh->element_normal,
				       vartrimsh->vertex_count,
				       vartrimsh->vertex_location,
				       vartrimsh->vertex_normal,
				       vartrimsh->vertex_color,
				       vartrimsh->triangle_vertexlist,
				       vartrimsh->compiled_triangle_vertexlist,
				       vartrimsh->uv_count,
				       vartrimsh->uv_list,
				       vartrimsh->uvw_count,
				       vartrimsh->uvw_list);

    dde_vll_triangle_mesh_uvvertexlist = (DtInt *)0;
    dde_vll_triangle_mesh_edge_enable = (DtInt *)0;
    dde_vll_triangle_mesh_bboxuv = (DtRealCouple *)0;
    dde_vll_triangle_mesh_bboxuvw = (DtRealTriple *)0;
}
/*
  ======================================================================
 */
