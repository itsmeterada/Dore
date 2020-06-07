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
	dor_varquadmsh_initialize
	dor_varquadmsh_create
	dor_varquadmsh_destroy
	dor_varquadmsh_print
	dor_varquadmsh_update_alternate
	dor_varquadmsh_update_lines
	dor_varquadmsh_update_tris
	dor_varquadmsh_bounding_box
	dor_varquadmsh_create_alternate_geom_lines
	dor_varquadmsh_create_alternate_geom_tris
	dor_varquadmsh_calculate_quad_normals
	dor_varquadmsh_calculate_vertex_normals
	dor_varquadmsh_compute_bounding_volume
	dor_varquadmsh_pick
	dor_varquadmsh_pick_lines
	dor_varquadmsh_pick_tris 
	dor_varquadmsh_render_standard 
	dor_varquadmsh_update
	dor_varquadmsh_update_uv
	dor_varquadmsh_update_uvw 

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/internal/glbatt.h>
#include <dore/dore_develop/private/varquadmsh.h>
#include <dore/dore_develop/private/varlinlst.h>
#include <dore/dore_develop/private/vartrimsh.h>

/*
 ======================================================================
 */

DtInt doe_varquadmsh_class_id;

static DtMethodEntry varquadmsh_methods[] = {
    {DcMethodCmpBndVolume, (DtMethodPtr)dor_varquadmsh_compute_bounding_volume},
    {DcMethodDestroy,      (DtMethodPtr)dor_varquadmsh_destroy},
    {DcMethodPick,         (DtMethodPtr)dor_varquadmsh_pick},
    {DcMethodPrint,        (DtMethodPtr)dor_varquadmsh_print},
    {DcMethodUpdStdAltObj, (DtMethodPtr)dor_varquadmsh_update_alternate},
    {DcMethodUpdStdAltLineObj, (DtMethodPtr)dor_varquadmsh_update_lines},
    {DcMethodStdRenderDisplay, (DtMethodPtr)dor_varquadmsh_render_standard}
};

void dor_varquadmsh_initialize (void)
{
    doe_varquadmsh_class_id = dor_class_add ("DoVarQuadMesh", 7, 
		varquadmsh_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_varquadmsh_create (
    DtColorModel colormodel,
    DtInt vertexcount,
    DtReal *vertices,
    DtReal *vertnormal,
    DtReal *vertcolor,
    DtInt quadcount,
    DtInt quadvtxlst[][4],
    DtFlag smoothflag)
{
   static DtFlag initialized = DcFalse;
   struct varquadmsh *varquadmsh;
   varquadmsh_vertexlist *varquadmsh_vlstptr;
   varquadmsh_vertexlist *quad_vlstptr;
   DtInt vertex1, vertex2, vertex3, vertex4;
   DtInt quadno;
   DtInt i;
   dot_object *varquadmshobject;
   
   if (!initialized) {
      dor_varquadmsh_initialize();
      initialized = DcTrue;
   }

   varquadmsh = dor_space_allocate (sizeof *varquadmsh);
   
   varquadmsh->colormodel = colormodel;
   varquadmsh->vertex_count = vertexcount;
   varquadmsh->vertex_location = (DtRealTriple *)vertices;
   varquadmsh->vertex_normal = (DtRealTriple *)vertnormal;
   varquadmsh->vertex_color = (DtColorRGB *)vertcolor;
   varquadmsh->quad_count = quadcount;
   varquadmsh->smoothflag = smoothflag;
   varquadmsh->needelemnrms = DcTrue;
   varquadmsh->compiled_quad_vertexlist = NULL;
   varquadmsh->element_normal = (DtRealTriple *)NULL;

   varquadmsh->alt_object_tris = NULL;
   varquadmsh->alt_triangle_vertexlist = NULL;

   varquadmsh->alt_object_lines = NULL;
   varquadmsh->alt_linevtx = (DtRealTriple *)NULL;
   varquadmsh->alt_linenrm = (DtRealTriple *)NULL;
   varquadmsh->alt_lineclr = (DtColorRGB *)NULL;
   varquadmsh->decompose_lines = DcTrue;

   varquadmsh->uv_count = 0;
   varquadmsh->uv_list = dor_space_allocate(
                dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
   for (i=0; i<dod_system_max_vertex_uv_cnt; i++) {
	varquadmsh->uv_list[i] = NULL;
   }

   varquadmsh->uvw_count = 0;
   varquadmsh->uvw_list = dor_space_allocate(
                dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *));
   for (i=0; i<dod_system_max_vertex_uvw_cnt; i++) {
	varquadmsh->uvw_list[i] = NULL;
   }

   varquadmsh->quad_vertexlist = 
		dor_space_allocate(quadcount*(sizeof (varquadmsh_vertexlist)));

   quad_vlstptr = quadvtxlst;
   varquadmsh_vlstptr = varquadmsh->quad_vertexlist;

   for (quadno=0;quadno<quadcount;quadno++) {
	vertex1 = (*quad_vlstptr)[0];
	vertex2 = (*quad_vlstptr)[1];
	vertex3 = (*quad_vlstptr)[2];
	vertex4 = (*quad_vlstptr)[3];

	quad_vlstptr++;

	if ((vertex1 < 0) || (vertex1 >= vertexcount) ||
	    (vertex2 < 0) || (vertex2 >= vertexcount) ||
	    (vertex3 < 0) || (vertex3 >= vertexcount) ||
	    (vertex4 < 0) || (vertex4 >= vertexcount)) {
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "dor_varquadmsh_create", 
			    "vertex index > vertexcount or < zero");

		continue;
	}

	(*varquadmsh_vlstptr)[0] = vertex1;
	(*varquadmsh_vlstptr)[1] = vertex2;
	(*varquadmsh_vlstptr)[2] = vertex3;
	(*varquadmsh_vlstptr)[3] = vertex4;

	varquadmsh_vlstptr++;
   }

   if (smoothflag) {
	if (vertnormal == NULL) {
		varquadmsh->needvertnrms = DcTrue;		
	} else {
		/* 
		 * Never mess with the vertex normals if the user passed them in.
		 */
		varquadmsh->smoothflag = DcFalse;
		varquadmsh->needvertnrms = DcFalse;
	}
   } else {
	varquadmsh->needvertnrms = DcFalse;
   }

   varquadmsh->sequence_number = 1;

   varquadmsh->edge_enable = NULL;

   dor_varquadmsh_bounding_box(varquadmsh);

   varquadmshobject = dor_object_create(doe_varquadmsh_class_id,varquadmsh);
   return(varquadmshobject);
}

/*
 ======================================================================
 */
void dor_varquadmsh_destroy (
    dot_object *object)
{
    struct varquadmsh *varquadmsh;

    varquadmsh = (struct varquadmsh *)(object->data);

    /*
     * Delete only that space which the user didn't pass in for sharing.
     */
    if (varquadmsh->smoothflag && (varquadmsh->vertex_normal != NULL)) {
	dor_space_deallocate(varquadmsh->vertex_normal);
    }

    if (varquadmsh->element_normal != NULL) {
	dor_space_deallocate(varquadmsh->element_normal);
    }

    if (varquadmsh->quad_vertexlist != NULL) {
	dor_space_deallocate(varquadmsh->quad_vertexlist);
    }

    if (varquadmsh->compiled_quad_vertexlist != NULL) {
	dor_space_deallocate(varquadmsh->compiled_quad_vertexlist);
    }

    if (varquadmsh->alt_object_tris != NULL) {
	dor_object_check_deletion(varquadmsh->alt_object_tris);
    }

    if (varquadmsh->alt_triangle_vertexlist != NULL) {
	dor_space_deallocate(varquadmsh->alt_triangle_vertexlist);
    }

    if (varquadmsh->alt_object_lines != NULL) {
	dor_object_check_deletion(varquadmsh->alt_object_lines);
    }

    if (varquadmsh->alt_linevtx != NULL) {
	dor_space_deallocate(varquadmsh->alt_linevtx);
    }

    if (varquadmsh->alt_linenrm != NULL) {
	dor_space_deallocate(varquadmsh->alt_linenrm);
    }

    if (varquadmsh->alt_lineclr != NULL) {
	dor_space_deallocate(varquadmsh->alt_lineclr);
    }

    if (varquadmsh->alt_lineclr != NULL) {
	dor_space_deallocate(varquadmsh->alt_lineclr);
    }

    dor_space_deallocate (varquadmsh->uv_list);
    dor_space_deallocate (varquadmsh->uvw_list);

    dor_space_deallocate(varquadmsh);
}
/*
 ======================================================================
 */

void dor_varquadmsh_print (
    dot_object *object)
{
    struct varquadmsh *varquadmsh;

    varquadmsh = (struct varquadmsh *)(object->data);

    if (varquadmsh == NULL){
	dor_print_output("variable quadrilateral mesh is NULL");
	return;
    }
 
    switch (varquadmsh->colormodel){
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;
 
    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    sprintf(dor_print_get_line(),
	    "vertex locations at [%lx]", varquadmsh->vertex_location);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex normals at [%lx]", varquadmsh->vertex_normal);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex colors at [%lx]", varquadmsh->vertex_color);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"smooth flag = %d",varquadmsh->smoothflag);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"vertex count = %d",varquadmsh->vertex_count);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"quad count = %d",varquadmsh->quad_count);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */
void dor_varquadmsh_render_standard (
    dot_object *object)
{
    dot_object *alt;

    if (!dor_condex_query_render(doe_varquadmsh_class_id))
            return;

    alt = dor_varquadmsh_update_alternate (object);
    if (alt != DcNullPtr) {
        dor_object_get_current_method(alt) (alt);
    }
}
/*
 ======================================================================
 */
dot_object *dor_varquadmsh_update_alternate (
    dot_object *object)
{
    DtRepType reptype;
    dot_object *alt;

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
    case DcWireframe:
    case DcPoints:
        alt = dor_varquadmsh_update_tris(object);
        break;

    case DcOutlines:
        alt = dor_varquadmsh_update_lines(object);
        break;
   default:
        DDerror(ERR_VALUE_OUT_OF_RANGE,
                   "dor_varquadmsh_update_alternate",
                   "unknown reptype");
        alt = DcNullPtr;
        break;
    }
    return alt;
}
/*
 ======================================================================
 */

dot_object *dor_varquadmsh_update_lines (
    dot_object *object)
{
    struct varquadmsh *varquadmsh;

    varquadmsh = (struct varquadmsh *)(object->data);

    if (varquadmsh->decompose_lines) {
	if (varquadmsh->alt_object_lines != DcNullPtr) {
	    dor_object_get_method(varquadmsh->alt_object_lines,
			  DcMethodDestroy)(varquadmsh->alt_object_lines);
	    varquadmsh->alt_object_lines = DcNullPtr;
	} 

	varquadmsh->alt_object_lines =
		dor_varquadmsh_create_alternate_geom_lines(varquadmsh);

	if (varquadmsh->alt_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_varquadmsh_update_lines", "");
	} else {
	    varquadmsh->decompose_lines = DcFalse;
	}
    }
    return varquadmsh->alt_object_lines;
}
/*
 ======================================================================
 */
dot_object *dor_varquadmsh_update_tris (
    dot_object *object)
{
    struct varquadmsh *varquadmsh;
    int i;

    varquadmsh = (struct varquadmsh *)(object->data);

    if (varquadmsh->needvertnrms) {
        if (varquadmsh->vertex_normal == NULL) {

        /* need element normals to compute vertex normals */
            varquadmsh->element_normal =
                    dor_space_allocate
		    (varquadmsh->quad_count * (sizeof(DtRealTriple)));

            varquadmsh->vertex_normal =
                    dor_space_allocate
		    (varquadmsh->vertex_count * (sizeof(DtRealTriple)));
       }
        dor_varquadmsh_calculate_quad_normals(varquadmsh);
        dor_varquadmsh_calculate_vertex_normals(varquadmsh);
        varquadmsh->needvertnrms = DcFalse;
    }

    if (varquadmsh->alt_object_tris == NULL) {

        varquadmsh->alt_object_tris =
                dor_varquadmsh_create_alternate_geom_tris(varquadmsh);

        if (varquadmsh->alt_object_tris == NULL) {
            DDerror(ERR_CANT_CREATE_ALTOBJ,
                       "dor_varquadmsh_update_tris", "");
        } else {

            /* copy the varquadmsh's uv/uvws for the vartrimsh */
            for (i=0; i<varquadmsh->uv_count; i++) {
                dor_vartrimsh_update_uv(varquadmsh->alt_object_tris, i,
                                       varquadmsh->uv_list[i]);
            }

            for (i=0; i<varquadmsh->uvw_count; i++) {
                dor_vartrimsh_update_uvw(varquadmsh->alt_object_tris, i,
                                         varquadmsh->uvw_list[i]);
            }
        }
    }

    return varquadmsh->alt_object_tris;
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

void dor_varquadmsh_bounding_box(struct varquadmsh *varquadmsh)
{
    DtReal xmin, ymin, zmin, xmax, ymax, zmax;
    DtInt i, vertexno;
    DtInt quadno;


    if (varquadmsh->quad_count == 0) {
        xmin = 0.;
        ymin = 0.;
        zmin = 0.;

        xmax = 0.;
        ymax = 0.;
        zmax = 0.;
    } else {

	vertexno = varquadmsh->quad_vertexlist[0][0];

	xmin = varquadmsh->vertex_location[vertexno][0];
	xmax = varquadmsh->vertex_location[vertexno][0];

	ymin = varquadmsh->vertex_location[vertexno][1];
	ymax = varquadmsh->vertex_location[vertexno][1];

	zmin = varquadmsh->vertex_location[vertexno][2];
	zmax = varquadmsh->vertex_location[vertexno][2];

PRAGMA(IVDEP)
PRAGMA(VBEST)

	/*
	 * Use only the vertices that are referenced in the mesh
	 * to compute the bounding box.
	 */
	for (quadno=0;quadno<varquadmsh->quad_count;quadno++){
            for (i=0; i<4; i++) {

		vertexno = varquadmsh->quad_vertexlist[quadno][i];

		xmin = min(xmin,varquadmsh->vertex_location[vertexno][0]);
		xmax = max(xmax,varquadmsh->vertex_location[vertexno][0]);

		ymin = min(ymin,varquadmsh->vertex_location[vertexno][1]);
		ymax = max(ymax,varquadmsh->vertex_location[vertexno][1]);

		zmin = min(zmin,varquadmsh->vertex_location[vertexno][2]);
		zmax = max(zmax,varquadmsh->vertex_location[vertexno][2]);
	    }
	}
    }

    varquadmsh->bounding_box_pts[0][0] = xmin;
    varquadmsh->bounding_box_pts[0][1] = ymin;
    varquadmsh->bounding_box_pts[0][2] = zmin;

    varquadmsh->bounding_box_pts[1][0] = xmin;
    varquadmsh->bounding_box_pts[1][1] = ymax;
    varquadmsh->bounding_box_pts[1][2] = zmin;

    varquadmsh->bounding_box_pts[2][0] = xmax;
    varquadmsh->bounding_box_pts[2][1] = ymax;
    varquadmsh->bounding_box_pts[2][2] = zmin;

    varquadmsh->bounding_box_pts[3][0] = xmax;
    varquadmsh->bounding_box_pts[3][1] = ymin;
    varquadmsh->bounding_box_pts[3][2] = zmin;

    varquadmsh->bounding_box_pts[4][0] = xmin;
    varquadmsh->bounding_box_pts[4][1] = ymin;
    varquadmsh->bounding_box_pts[4][2] = zmax;

    varquadmsh->bounding_box_pts[5][0] = xmin;
    varquadmsh->bounding_box_pts[5][1] = ymax;
    varquadmsh->bounding_box_pts[5][2] = zmax;

    varquadmsh->bounding_box_pts[6][0] = xmax;
    varquadmsh->bounding_box_pts[6][1] = ymax;
    varquadmsh->bounding_box_pts[6][2] = zmax;

    varquadmsh->bounding_box_pts[7][0] = xmax;
    varquadmsh->bounding_box_pts[7][1] = ymin;
    varquadmsh->bounding_box_pts[7][2] = zmax;
}
/*
 ======================================================================
 */

dot_object *dor_varquadmsh_create_alternate_geom_lines (
    struct varquadmsh *varquadmsh)
{
    DtInt quadno;
    DtInt vtxno;
    DtInt vtxcnt;
    DtInt linevtxcnt;
    DtInt lvtx0, lvtx1;
    dot_object *varlinlstobj;

    vtxcnt = 0;

    if (varquadmsh->alt_linevtx == NULL) {

	/*
	 * The number of vertices in the line list = 2 * (quad_count * 4)
	 */
	linevtxcnt = 8 * varquadmsh->quad_count;

	varquadmsh->alt_linevtx = (DtRealTriple *)dor_space_allocate
		(linevtxcnt * (sizeof (DtRealTriple)));

	if (varquadmsh->vertex_normal != NULL) {
	    varquadmsh->alt_linenrm = 
		    (DtRealTriple *)dor_space_allocate
		(linevtxcnt * (sizeof (DtRealTriple)));
	}

	if (varquadmsh->vertex_color != NULL) {
	    varquadmsh->alt_lineclr = 
		    (DtRealTriple *)dor_space_allocate
			    (linevtxcnt * (sizeof (DtRealTriple)));
	}
    }

    for (quadno=0; quadno<varquadmsh->quad_count; quadno++){
	for (vtxno=0; vtxno<4; vtxno++) {

	    lvtx0 = varquadmsh->quad_vertexlist[quadno][vtxno];
	    if (vtxno==3)
		    lvtx1 = varquadmsh->quad_vertexlist[quadno][0];
	    else
		    lvtx1 = varquadmsh->quad_vertexlist[quadno][vtxno+1];

	    varquadmsh->alt_linevtx[vtxcnt][0] = 
		    varquadmsh->vertex_location[lvtx0][0];
	    varquadmsh->alt_linevtx[vtxcnt][1] = 
		    varquadmsh->vertex_location[lvtx0][1];
	    varquadmsh->alt_linevtx[vtxcnt][2] = 
		    varquadmsh->vertex_location[lvtx0][2];

	    if (varquadmsh->vertex_normal != NULL) {
		varquadmsh->alt_linenrm[vtxcnt][0] = 
			varquadmsh->vertex_normal[lvtx0][0];
		varquadmsh->alt_linenrm[vtxcnt][1] = 
			varquadmsh->vertex_normal[lvtx0][1];
		varquadmsh->alt_linenrm[vtxcnt][2] = 
			varquadmsh->vertex_normal[lvtx0][2];
	    }

	    if (varquadmsh->vertex_color != NULL) {
		varquadmsh->alt_lineclr[vtxcnt][0] = 
			varquadmsh->vertex_color[lvtx0][0];
		varquadmsh->alt_lineclr[vtxcnt][1] = 
			varquadmsh->vertex_color[lvtx0][1];
		varquadmsh->alt_lineclr[vtxcnt][2] = 
			varquadmsh->vertex_color[lvtx0][2];
	    }

	    vtxcnt++;

	    varquadmsh->alt_linevtx[vtxcnt][0] = 
		    varquadmsh->vertex_location[lvtx1][0];
	    varquadmsh->alt_linevtx[vtxcnt][1] = 
		    varquadmsh->vertex_location[lvtx1][1];
	    varquadmsh->alt_linevtx[vtxcnt][2] = 
		    varquadmsh->vertex_location[lvtx1][2];

	    if (varquadmsh->vertex_normal != NULL) {
		varquadmsh->alt_linenrm[vtxcnt][0] = 
			varquadmsh->vertex_normal[lvtx1][0];
		varquadmsh->alt_linenrm[vtxcnt][1] = 
			varquadmsh->vertex_normal[lvtx1][1];
		varquadmsh->alt_linenrm[vtxcnt][2] = 
			varquadmsh->vertex_normal[lvtx1][2];
	    }

	    if (varquadmsh->vertex_color != NULL) {
		varquadmsh->alt_lineclr[vtxcnt][0] = 
			varquadmsh->vertex_color[lvtx1][0];
		varquadmsh->alt_lineclr[vtxcnt][1] = 
			varquadmsh->vertex_color[lvtx1][1];
		varquadmsh->alt_lineclr[vtxcnt][2] = 
			varquadmsh->vertex_color[lvtx1][2];
	    }

	    vtxcnt++;
	}
    }

    
    varlinlstobj = dor_varlinlst_create(varquadmsh->colormodel, vtxcnt/2,
				(DtReal*)varquadmsh->alt_linevtx, 
				(DtReal*)varquadmsh->alt_linenrm,
				(DtReal*)varquadmsh->alt_lineclr);

    return(varlinlstobj);
}
/*
 ======================================================================
 */
dot_object *dor_varquadmsh_create_alternate_geom_tris (
    struct varquadmsh *varquadmsh)
{
    DtInt trianglecount;
    DtInt quadno;
    DtIntTriple *vconnectptr;
    dot_object *vartrimshobj;

    /* number of triangles is twice the number of quads */
    trianglecount = 2 * varquadmsh->quad_count;

    if (varquadmsh->alt_triangle_vertexlist == NULL) {
        varquadmsh->alt_triangle_vertexlist = (DtIntTriple *)dor_space_allocate(
                                    trianglecount*(sizeof (DtIntTriple)));

        vconnectptr = varquadmsh->alt_triangle_vertexlist;

	/*
	 * Construct connectivity list for triangle mesh.
	 */
	for (quadno=0; quadno<varquadmsh->quad_count; quadno++) {
            (*vconnectptr)[0] = varquadmsh->quad_vertexlist[quadno][0];
            (*vconnectptr)[1] = varquadmsh->quad_vertexlist[quadno][1];
            (*vconnectptr)[2] = varquadmsh->quad_vertexlist[quadno][2];
            vconnectptr++;

            (*vconnectptr)[0] = varquadmsh->quad_vertexlist[quadno][0];
            (*vconnectptr)[1] = varquadmsh->quad_vertexlist[quadno][2];
            (*vconnectptr)[2] = varquadmsh->quad_vertexlist[quadno][3];
            vconnectptr++;
	}
    }

    vartrimshobj = dor_vartrimsh_create(varquadmsh->colormodel,
                                        varquadmsh->vertex_count,
                                        (DtReal*)varquadmsh->vertex_location,
                                        (DtReal*)varquadmsh->vertex_normal,
                                        (DtReal*)varquadmsh->vertex_color,
                                        trianglecount,
                                        varquadmsh->alt_triangle_vertexlist,
                                        DcFalse);
    return(vartrimshobj);
}
/*
 ======================================================================
 */
void dor_varquadmsh_calculate_quad_normals (
    struct varquadmsh *varquadmsh)
{
    DtInt quadno;
    DtInt vertex1, vertex2, vertex3;

    /*
     * To compute a quad's normal, we require that the quad be planar and
     * use only its first three vertices.
     */
    for (quadno=0;quadno<varquadmsh->quad_count;quadno++) {
	vertex1 = varquadmsh->quad_vertexlist[quadno][0];
	vertex2 = varquadmsh->quad_vertexlist[quadno][1];
	vertex3 = varquadmsh->quad_vertexlist[quadno][2];

	dor_math_fast_cross_cosines
	(   varquadmsh->vertex_location[vertex1],
	    varquadmsh->vertex_location[vertex2],
	    varquadmsh->vertex_location[vertex3],
	    varquadmsh->element_normal[quadno]
	);

	dor_math_renormalize_fast (varquadmsh->element_normal[quadno]);
    }
}
/*
 ======================================================================
 */

void dor_varquadmsh_calculate_vertex_normals (
    struct varquadmsh *varquadmsh)
{
    DtFlag *vertexrefflag;
    DtInt vertexno;
    DtInt quadno;
    DtInt quadvertexno;

    /* 
     * Allocate a reference flag table for each vertex.  The reference
     * flag tells us if the vertex belongs to any quad, and therefore
     * whether its direction vector was computed and should be renormalized.  
     */
    vertexrefflag = (DtFlag *)dor_space_allocate
			(varquadmsh->vertex_count*(sizeof (DtFlag)));

    /*  
     * Zero the vertex normals and quad reference flags for all vertices.  
     */
PRAGMA(ASIS)
    for (vertexno=0;vertexno<varquadmsh->vertex_count;vertexno++) {
	vertexrefflag[vertexno] = DcFalse;

	varquadmsh->vertex_normal[vertexno][0] = 0.;
	varquadmsh->vertex_normal[vertexno][1] = 0.;
	varquadmsh->vertex_normal[vertexno][2] = 0.;
    }

    /*  
     * For each of the quad's four vertices, update that vertex's
     * quad reference count and accumulated quad normal.  The vertex
     * normal is the average of the element normals from the surfaces
     * that are adjacent to that vertex.
     */
PRAGMA(ASIS)
    for (quadno=0;quadno<varquadmsh->quad_count;quadno++) {
	for (quadvertexno=0;quadvertexno<4;quadvertexno++) {
	    vertexno = varquadmsh->quad_vertexlist[quadno][quadvertexno];

	    vertexrefflag[vertexno] = DcTrue;

	    varquadmsh->vertex_normal[vertexno][0] +=
		    varquadmsh->element_normal[quadno][0];
	    varquadmsh->vertex_normal[vertexno][1] +=
		    varquadmsh->element_normal[quadno][1];
	    varquadmsh->vertex_normal[vertexno][2] +=
		    varquadmsh->element_normal[quadno][2];
	    varquadmsh->vertex_normal[vertexno][3] +=
		    varquadmsh->element_normal[quadno][3];
	}
    }

    /*  
     * Rescale the vertex normals.  
     */
PRAGMA(ASIS)
    for (vertexno=0;vertexno<varquadmsh->vertex_count;vertexno++) {
	if (vertexrefflag[vertexno]) {
	    if (!dor_math_renormalize_vector(varquadmsh->vertex_normal[vertexno])) {
		DDerror (ERR_ZERO_QUAD_NRM_SUM,
			    "dor_varquadmsh_calculate_vertex_normals", DcNullPtr);
	    }
	}
    }

    /*  Free the vertex reference flags.  */

    dor_space_deallocate(vertexrefflag);
}
/*
 ======================================================================
 */

void dor_varquadmsh_compute_bounding_volume (
    dot_object *object)
{
    struct varquadmsh *varquadmsh;

    varquadmsh = (struct varquadmsh *)(object->data);

    dor_system_report_bounding_points(8,
    	 (DtReal*)varquadmsh->bounding_box_pts);
}

/*
 ======================================================================
 */

void dor_varquadmsh_pick (
    dot_object *object)
{
    DtRepType reptype;

    if (!dor_condex_query_pick(doe_varquadmsh_class_id))
	    return;

    dor_pick_initialize_object(object);

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    case DcOutlines:
        dor_varquadmsh_pick_lines(object);
        break;

    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
    case DcWireframe:
    case DcPoints:
        dor_varquadmsh_pick_tris(object);
        break;
    }
}
/*
 ======================================================================
 */
void dor_varquadmsh_pick_lines (
    dot_object *object)
{
    struct varquadmsh *varquadmsh;

    varquadmsh = (struct varquadmsh *)(object->data);

    if (varquadmsh->decompose_lines) {
        if (varquadmsh->alt_object_lines != DcNullPtr) {
            doe_system_alternate_object_execution_flag = DcTrue;

            dor_object_get_method(varquadmsh->alt_object_lines,
                                  DcMethodDestroy)(varquadmsh->alt_object_lines);

            doe_system_alternate_object_execution_flag = DcFalse;

            varquadmsh->alt_object_lines = NULL;
        }
        varquadmsh->alt_object_lines =
                dor_varquadmsh_create_alternate_geom_lines(varquadmsh);

        if (varquadmsh->alt_object_lines == NULL) {
            DDerror(ERR_CANT_CREATE_ALTOBJ,
                       "dor_varquadmsh_pick_lines", "");
            return;
        }

        varquadmsh->decompose_lines = DcFalse;
    }

    doe_system_alternate_object_execution_flag = DcTrue;

    dor_object_get_method(varquadmsh->alt_object_lines,
                          DcMethodPick)(varquadmsh->alt_object_lines);
    doe_system_alternate_object_execution_flag = DcFalse;

    return;
}
/*
 ======================================================================
 */
void dor_varquadmsh_pick_tris (
    dot_object *object)
{
    struct varquadmsh *varquadmsh;

    varquadmsh = (struct varquadmsh *)(object->data);

    if (varquadmsh->alt_object_tris == NULL) {
        varquadmsh->alt_object_tris =
              dor_varquadmsh_create_alternate_geom_tris(varquadmsh);
        if (varquadmsh->alt_object_tris == NULL) {
            DDerror(ERR_CANT_CREATE_ALTOBJ,
                       "dor_varquadmsh_pick_tris", "");
            return;
        }
    }

    doe_system_alternate_object_execution_flag = DcTrue;

    dor_object_get_method(varquadmsh->alt_object_tris,
                          DcMethodPick)(varquadmsh->alt_object_tris);

    doe_system_alternate_object_execution_flag = DcFalse;

    return;
}
/*
 ======================================================================
 */

void dor_varquadmsh_update (
    dot_object *object,
    DtReal *vlocs,
    DtReal *vnrms,
    DtReal *vclrs,
    DtFlag recompute_nrms)
{
    struct varquadmsh *varquadmsh;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_varquadmsh_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarQuadMesh",DcNullPtr);
	return;
    }   
#endif

    varquadmsh = (struct varquadmsh *)(object->data);

    varquadmsh->sequence_number++;
   
    if (vlocs != NULL) {
	varquadmsh->vertex_location = (DtRealTriple *)vlocs;

	dor_varquadmsh_bounding_box(varquadmsh);

	if (recompute_nrms) {
	    varquadmsh->needelemnrms = DcTrue;
	    if (varquadmsh->smoothflag) 
		    varquadmsh->needvertnrms = DcTrue;
	} else {
	    varquadmsh->needelemnrms = DcFalse;
	    varquadmsh->needvertnrms = DcFalse;
	}
	
    }

    /* 
     * If the object was created without vertex normals and/or colours,
     * then you cannot pass them in now.
     */
    if (vnrms != NULL) {
	if ((varquadmsh->vertex_normal != NULL) && !varquadmsh->smoothflag) 
		varquadmsh->vertex_normal = (DtRealTriple *)vnrms;
	else
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "dor_varquadmsh_update", "vertex normals ignored");
    }	
    if (vclrs != NULL) {
	if (varquadmsh->vertex_color != NULL)
		varquadmsh->vertex_color = (DtColorRGB *)vclrs;
	else 
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "dor_varquadmsh_update", "vertex colors ignored");
    }

    varquadmsh->decompose_lines = DcTrue;

}
/*
 ======================================================================
 */

void dor_varquadmsh_update_uv (
    dot_object *object,
    DtInt uvindex,
    DtRealCouple *uvlist)
{
    struct varquadmsh *varquadmsh;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_varquadmsh_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarQuadMeshUV",DcNullPtr);
	return;
    }   
#endif

    varquadmsh = (struct varquadmsh *)(object->data);

#ifdef ERRCHK
    if (uvindex<0) {
          DDerror(ERR_VALUE_OUT_OF_RANGE, 
			"DpUpdVarQuadMeshUV", "uv index < 0");
          return;
    } else if (uvindex > varquadmsh->uv_count) {
          DDerror(ERR_VALUE_OUT_OF_RANGE, 
			"DpUpdVarQuadMeshUV", "uv index too large");
          return;
    }
#endif

    varquadmsh->sequence_number++;
    if (varquadmsh->uv_count < uvindex+1) {
	varquadmsh->uv_count = uvindex+1;
    }
    varquadmsh->uv_list[uvindex] = uvlist;

    /*
     * We need to update the uv's of the alternate object (vartrimsh)
     * if it exists.
     */
    if (varquadmsh->alt_object_tris != NULL) {
	dor_vartrimsh_update_uv(varquadmsh->alt_object_tris, uvindex, uvlist);
    }
}
/*
 ======================================================================
 */

void dor_varquadmsh_update_uvw (
    dot_object *object,
    DtInt uvwindex,
    DtRealTriple *uvwlist)

{
    struct varquadmsh *varquadmsh;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_varquadmsh_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarQuadMeshUVW",DcNullPtr);
	return;
    }   
#endif

    varquadmsh = (struct varquadmsh *)(object->data);

#ifdef ERRCHK
    if (uvwindex<0) {
          DDerror (ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarQuadMeshUVW",
                        "uvw index < 0");

          return;
    } else if (uvwindex > varquadmsh->uvw_count) {
          DDerror (ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarQuadMeshUVW",
                        "uvw index too large");
          return;
    }

#endif

    varquadmsh->sequence_number++;
    if (varquadmsh->uvw_count < uvwindex+1)
	    varquadmsh->uvw_count = uvwindex+1;
    varquadmsh->uvw_list[uvwindex] = uvwlist;   

    /*
     * We need to update the uvw's of the alternate object (vartrimsh)
     * if it exists.
     */
    if (varquadmsh->alt_object_tris != NULL) {
	dor_vartrimsh_update_uvw(varquadmsh->alt_object_tris, uvwindex, uvwlist);
    }
}
