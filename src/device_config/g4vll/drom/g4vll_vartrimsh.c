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
	ddt_g4vll_drom_vartrimsh * ddr_g4vll_drom_vartrimsh_create (object)
	void ddr_g4vll_drom_vartrimsh_print (object, data)
	void ddr_g4vll_drom_vartrimsh_delete (object, data)
	void ddr_g4vll_drom_vartrimsh_render (object)

  ======================================================================
 */
#include "drom.h"
#include <dore/internal/system.h>
#include <dore/dore_develop/private/vartrimsh.h>
#include <dore/dore_develop/private/tristrip.h>

/*
 ======================================================================
 */

static DtInt classid = -1;
static DtInt dataindex = -1;

/*
  ======================================================================
 */
ddt_g4vll_drom_vartrimsh *ddr_g4vll_drom_vartrimsh_create (
    DtObjectStructure *object)
{
    struct vartrimsh *vartrimsh;
    ddt_g4vll_drom_vartrimsh *drom_vartrimsh;
    int i;

    vartrimsh = (struct vartrimsh *)(object->data);

    drom_vartrimsh =  (ddt_g4vll_drom_vartrimsh *)
	    DDspace_Allocate(sizeof(ddt_g4vll_drom_vartrimsh));

    drom_vartrimsh->uvvertexlist = (DtInt *)0;

    drom_vartrimsh->uvcount = vartrimsh->uv_count;
    drom_vartrimsh->uvwcount = vartrimsh->uvw_count;

    if (vartrimsh->uv_count > 0) {
        drom_vartrimsh->bboxuv = (ddt_g4vll_drom_bboxuv *)DDspace_Allocate
                (vartrimsh->uv_count *sizeof(ddt_g4vll_drom_bboxuv));

        for (i=0; i< vartrimsh->uv_count; i++) {
            drom_vartrimsh->bboxuv[i].initialized = DcFalse;
        }
    } else {
        drom_vartrimsh->bboxuv = (ddt_g4vll_drom_bboxuv *)0;
    }

    if (vartrimsh->uvw_count > 0) {
        drom_vartrimsh->bboxuvw = (ddt_g4vll_drom_bboxuvw *)DDspace_Allocate
                (vartrimsh->uvw_count *sizeof(ddt_g4vll_drom_bboxuvw));

        for (i=0; i< vartrimsh->uvw_count; i++) {
            drom_vartrimsh->bboxuvw[i].initialized = DcFalse;
        }
    } else {
        drom_vartrimsh->bboxuvw = (ddt_g4vll_drom_bboxuvw *)0;
    }

    drom_vartrimsh->grid_check = DcFalse;

    drom_vartrimsh->tristrip_vertexcount = 0;
    drom_vartrimsh->tristrip_indices = 0;
    drom_vartrimsh->altobj_tristrip = 0;

    /*
     * Update the drom_vartrimsh information only when its sequence number
     * does not match that of the kernel.
     */
    drom_vartrimsh->sequence_number = vartrimsh->sequence_number;

    return drom_vartrimsh;
}
/*
  ======================================================================
 */
void ddr_g4vll_drom_vartrimsh_print (
    DtObject object,
    DtInt *data)
{
}
/*
  ======================================================================
 */
void ddr_g4vll_drom_vartrimsh_delete (
    DtObject object,
    ddt_g4vll_drom_vartrimsh *data)
{
    if (data != (ddt_g4vll_drom_vartrimsh *)0) {
	if (data->uvvertexlist != (DtInt *)0) {
	    DDspace_Deallocate(data->uvvertexlist);
	}
        if (data->bboxuv != (ddt_g4vll_drom_bboxuv *)0) {
            DDspace_Deallocate(data->bboxuv);
        }
        if (data->bboxuvw != (ddt_g4vll_drom_bboxuvw *)0) {
            DDspace_Deallocate(data->bboxuvw);
        }
        if (data->altobj_tristrip != (DtObjectStructure *)0) {
            DDspace_Deallocate(data->altobj_tristrip);
        }

	DDspace_Deallocate(data);
    }
}
/*
  ======================================================================
 */
void ddr_g4vll_drom_vartrimsh_render (
    dot_object *object)
{
    extern DtInt *dde_g4vll_triangle_mesh_uvvertexlist;
    struct vartrimsh *vartrimsh;
    DtIntTriple *uvvertexlist;
    extern DtInt *dde_g4vll_triangle_mesh_edge_enable;
    ddt_g4vll_drom_vartrimsh *drom_vartrimsh;
    ddt_g4vll_drom_bboxuv *bboxuv;
    ddt_g4vll_drom_bboxuvw *bboxuvw;
    DtInt triangleno;
    dot_object *shader;
    DtInterpType interptype;
    static DtMethodPtr reptype_query  = DcNullPtr;
    static DtMethodPtr altobj_destroy = DcNullPtr;
    DtRepType reptype;
    DtInt i;
    DtInt index;
    DtInt grid_dimensions[2];

    if (classid == -1) 
	    classid = DsInqClassId("DoVarTriangleMesh");

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

    vartrimsh = (struct vartrimsh *)(object->data);

    drom_vartrimsh = (ddt_g4vll_drom_vartrimsh *)object->additional_data[dataindex];

    /*
     * Determine whether the varmesh is, in fact, a gridmesh.  The actual check
     * is done only once per vartrimesh object as the number of triangles in
     * the mesh and its connectivity cannot be modified.  If it is a mesh, 
     * construct a tristrip alternate object.
     */
    if (!drom_vartrimsh->grid_check) {
        drom_vartrimsh->grid_check = DcTrue;

        if (ddr_g4vll_drom_gridmesh_to_strip
	    (   vartrimsh->vertex_count, vartrimsh->triangle_count,
            vartrimsh->triangle_vertexlist,
	    &(drom_vartrimsh->tristrip_vertexcount),
            &(drom_vartrimsh->tristrip_indices), (DtInt*) grid_dimensions)) {

            /*
             * The vartrimesh's alternate object should be a triangle strip.
             */

#           ifdef DEBUG
		printf ("VARTRIMSH: creating tristrip alternate object\n");
#           endif
            drom_vartrimsh->altobj_tristrip =
                    ddr_g4vll_drom_vartrimsh_create_alternate_tristrip(object);
	    drom_vartrimsh->sequence_number = vartrimsh->sequence_number;
        }
    }

    if (drom_vartrimsh->altobj_tristrip != (DtObject)0) {
	if (drom_vartrimsh->sequence_number != vartrimsh->sequence_number) {
            /*
             * Update the tristrip alternate object if the driver and
             * kernel sequence numbers are out of sync.
             */
	    if (altobj_destroy == 0) {
		altobj_destroy = DDobject_InqMethod(
			drom_vartrimsh->altobj_tristrip, DcMethodDestroy);
	    }
	    (*altobj_destroy)(drom_vartrimsh->altobj_tristrip);
	    drom_vartrimsh->altobj_tristrip =
                    ddr_g4vll_drom_vartrimsh_create_alternate_tristrip(object);
	    drom_vartrimsh->sequence_number = vartrimsh->sequence_number;
	}
	DDobject_InqCurrentMethod(drom_vartrimsh->altobj_tristrip)
                (drom_vartrimsh->altobj_tristrip);
    } else {
        /*
         * Store the vartrimsh object's handle globally for the trimsh
	 * output routine.
         */
        dde_g4vll_drom_vartrimsh = drom_vartrimsh;

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
	    drom_vartrimsh->uvvertexlist = (DtInt*) uvvertexlist;
        }

        if (vartrimsh->compiled_triangle_vertexlist == NULL) {
	    vartrimsh->compiled_triangle_vertexlist = (vartrimsh_vertexlist *)
	    	DDspace_Allocate (vartrimsh->triangle_count*
	    	    (sizeof (vartrimsh_vertexlist)));

	    for (triangleno=0;triangleno<vartrimsh->triangle_count;triangleno++) {
	        vartrimsh->compiled_triangle_vertexlist[triangleno][0] =
		    vartrimsh->triangle_vertexlist[triangleno][0] * 3;
	        vartrimsh->compiled_triangle_vertexlist[triangleno][1] =
		    vartrimsh->triangle_vertexlist[triangleno][1] * 3;
	        vartrimsh->compiled_triangle_vertexlist[triangleno][2] =
		    vartrimsh->triangle_vertexlist[triangleno][2] * 3;
	    }
        }

        /*
         * If we're texture mapping and have UVs, construct the uv bounding
         * box from the appropriate UV set.  Do this only if the kernel's 
	 * index or sequence number doesn't match that of the driver's.
         */
        if (vartrimsh->uv_count > 0 && dde_g4vll_drom_data.texture_2D_valid) {
            index = vartrimsh->uv_count > 
			dde_g4vll_drom_data.map_diffuse_color_uv_index ?
                	dde_g4vll_drom_data.map_diffuse_color_uv_index : 0;

            if ((index+1) > drom_vartrimsh->uvcount) {
            	/*
             	 * New UV set(s) have been added since we were last through here.
             	 * Add'em to the end of the driver's data.
             	 */
                drom_vartrimsh->bboxuv = (ddt_g4vll_drom_bboxuv *)
                    	DDspace_Reallocate(drom_vartrimsh->bboxuv, 
			vartrimsh->uv_count * sizeof(ddt_g4vll_drom_bboxuv));
                for (i=drom_vartrimsh->uvcount; i<vartrimsh->uv_count; i++) {
                    drom_vartrimsh->bboxuv[i].initialized = DcFalse;
                }
                drom_vartrimsh->uvcount = vartrimsh->uv_count;
                drom_vartrimsh->sequence_number = vartrimsh->sequence_number;
            } else if (drom_vartrimsh->sequence_number !=
                   vartrimsh->sequence_number) {
                /*
                 * One or more of the existing UV set(s) have been changed.
                 * We don't know which one so "uninitialise" all of them to
		 * ensure that a bounding box is created with the right data.
                 */
                for (i = 0; i < vartrimsh->uv_count; i++) {
                    drom_vartrimsh->bboxuv[i].initialized = DcFalse;
                }
                drom_vartrimsh->uvcount = vartrimsh->uv_count;
                drom_vartrimsh->sequence_number = vartrimsh->sequence_number;
            }

            bboxuv = &(drom_vartrimsh->bboxuv[index]);

            if (!bboxuv->initialized) {
                float umin, umax, vmin, vmax;
                /*
                 * This bounding box has not been set yet so compute it.
                 */
                umin = umax = vartrimsh->uv_list[index][0][0];
                vmin = vmax = vartrimsh->uv_list[index][0][1];

                for (i=1; i< vartrimsh->vertex_count; i++) {
                    umin = min(umin, vartrimsh->uv_list[index][i][0]);
                    umax = max(umax, vartrimsh->uv_list[index][i][0]);
                    vmin = min(vmin, vartrimsh->uv_list[index][i][1]);
                    vmax = max(vmax, vartrimsh->uv_list[index][i][1]);
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
        if (vartrimsh->uvw_count > 0 && dde_g4vll_drom_data.texture_3D_valid) {
            index = vartrimsh->uvw_count > 
		dde_g4vll_drom_data.map_diffuse_color_uvw_index ?
                dde_g4vll_drom_data.map_diffuse_color_uvw_index : 0;

           if ((index+1) > drom_vartrimsh->uvwcount) {
		/*
		 * New UVW set(s) have been added since we were last through
                 * here.  Add'em to the end of the driver's data.
                 */
                drom_vartrimsh->bboxuvw = (ddt_g4vll_drom_bboxuvw *)
                    	DDspace_Reallocate(drom_vartrimsh->bboxuvw,
                        vartrimsh->uvw_count * sizeof(ddt_g4vll_drom_bboxuvw));
                for (i=drom_vartrimsh->uvwcount; i<vartrimsh->uvw_count; i++) {
                    drom_vartrimsh->bboxuvw[i].initialized = DcFalse;
                }
                drom_vartrimsh->uvwcount = vartrimsh->uvw_count;
                drom_vartrimsh->sequence_number = vartrimsh->sequence_number;

            } else if (drom_vartrimsh->sequence_number !=
                   vartrimsh->sequence_number) {
            	/*
             	 * One or more of the existing UVW set(s) have been changed.
             	 * We don't know which one so "uninitialise" all of them to
		 * ensure that a bounding box is created with the right data.
             	 */
            	for (i = 0; i < vartrimsh->uvw_count; i++) {
                    drom_vartrimsh->bboxuvw[i].initialized = DcFalse;
                }
                drom_vartrimsh->uvwcount = vartrimsh->uvw_count;
                drom_vartrimsh->sequence_number = vartrimsh->sequence_number;
            }

            bboxuvw = &(drom_vartrimsh->bboxuvw[index]);

            if (!bboxuvw->initialized) {
                float umin, umax, vmin, vmax, wmin, wmax;
                /*
                 * this bounding box has not been set yet so compute bounding
                 * box
                 */

                umin = umax = vartrimsh->uvw_list[index][0][0];
                vmin = vmax = vartrimsh->uvw_list[index][0][1];
                wmin = wmax = vartrimsh->uvw_list[index][0][2];

                for (i=1; i< vartrimsh->vertex_count; i++) {
                    umin = min(umin, vartrimsh->uvw_list[index][i][0]);
                    umax = max(umax, vartrimsh->uvw_list[index][i][0]);
                    vmin = min(vmin, vartrimsh->uvw_list[index][i][1]);
                    vmax = max(vmax, vartrimsh->uvw_list[index][i][1]);
                    wmin = min(wmin, vartrimsh->uvw_list[index][i][2]);
                    wmax = max(wmax, vartrimsh->uvw_list[index][i][2]);
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

        if (doe_system_timing_display_traverse_count == 1)
            doe_system_timing_mesh_triangles_drawn += vartrimsh->triangle_count;

        if (!reptype_query) {
            reptype_query = DDclass_InqMethod (DsInqClassId("AttRepType"),
					   DcMethodInqGlbAttVal);
        }
        (*reptype_query)(&reptype);

        if (reptype == DcSurfaceOutlineEdges && 
	    vartrimsh->edge_enable != NULL) {
	    dde_g4vll_triangle_mesh_edge_enable = vartrimsh->edge_enable;
        }

        dde_g4vll_triangle_mesh_uvvertexlist = (DtInt *)uvvertexlist;

        ddr_g4vll_drom_render_triangle_mesh (DcRGB,
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
				       vartrimsh->uvw_list,
				       DcNullPtr);

        dde_g4vll_triangle_mesh_uvvertexlist = (DtInt *)0;
        dde_g4vll_triangle_mesh_edge_enable = (DtInt *)0;
        dde_g4vll_drom_vartrimsh = (ddt_g4vll_drom_vartrimsh *)0;
    }
}
/*
  ======================================================================
 */
dot_object *ddr_g4vll_drom_vartrimsh_create_alternate_tristrip (
    dot_object *object)
{
    struct vartrimsh *vartrimsh;
    dot_object *tristripobj;
    ddt_g4vll_drom_vartrimsh *drom_vartrimsh;
    DtVertexType vertextype;
    DtInt vertexsize;
    DtInt vertexno, index;
    DtUInt uv,uvw;
    DtReal *vertices, *vptr;

    vartrimsh = (struct vartrimsh *)(object->data);

    drom_vartrimsh = (ddt_g4vll_drom_vartrimsh *)object->additional_data[dataindex];

    vertexsize = 3;
    vertextype = DcLoc;
    if (vartrimsh->vertex_normal) {
        vertexsize += 3;
        vertextype |= DcLocNrm;
    }
    if (vartrimsh->vertex_color) {
        vertexsize += 3;
        vertextype |= DcLocClr;
    }

    vertexsize = vertexsize + vartrimsh->uv_count*2 + vartrimsh->uvw_count*3;

    vertices = (DtReal *)dor_space_allocate(
		drom_vartrimsh->tristrip_vertexcount*vertexsize*(sizeof(DtReal)));

    vptr = vertices;

    for (vertexno=0; vertexno<drom_vartrimsh->tristrip_vertexcount; vertexno++) {
    	/* 
     	 * The tristrip is defined by the tristrip_indices into the vartrimsh.
     	 */
	index = drom_vartrimsh->tristrip_indices[vertexno];
	*vptr++ = vartrimsh->vertex_location[index][0];
        *vptr++ = vartrimsh->vertex_location[index][1];
        *vptr++ = vartrimsh->vertex_location[index][2];

	if (vartrimsh->vertex_normal) {
            *vptr++ = vartrimsh->vertex_normal[index][0];
            *vptr++ = vartrimsh->vertex_normal[index][1];
            *vptr++ = vartrimsh->vertex_normal[index][2];
        }
        if (vartrimsh->vertex_color) {
            *vptr++ = vartrimsh->vertex_color[index][0];
            *vptr++ = vartrimsh->vertex_color[index][1];
            *vptr++ = vartrimsh->vertex_color[index][2];
        }
        for (uv=0; uv<vartrimsh->uv_count; uv++) {
            *vptr++ = vartrimsh->uv_list[uv][index][0];
            *vptr++ = vartrimsh->uv_list[uv][index][1];
        }
        for (uvw=0; uvw<vartrimsh->uvw_count; uvw++) {
            *vptr++ = vartrimsh->uvw_list[uvw][index][0];
            *vptr++ = vartrimsh->uvw_list[uvw][index][1];
            *vptr++ = vartrimsh->uvw_list[uvw][index][2];
        }
    }

    tristripobj = dor_tristrip_create(vartrimsh->colormodel,
	vertextype | dor_system_texture_uv_count(vartrimsh->uv_count) |
	dor_system_texture_uvw_count(vartrimsh->uvw_count),
	drom_vartrimsh->tristrip_vertexcount, vertices);

    dor_space_deallocate(vertices);
    return(tristripobj);
}
/*
  ======================================================================
 */
