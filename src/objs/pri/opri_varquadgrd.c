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
	dor_varquadgrid_initialize 
	dor_varquadgrid_create
	dor_varquadgrid_destroy 
	dor_varquadgrid_print 
	dor_varquadgrid_render_standard 
	dor_varquadgrid_update_alternate 
	dor_varquadgrid_update_lines 
	dor_varquadgrid_update_tris 
	dor_varquadgrid_bounding_box 
	dor_varquadgrid_create_alternate_geom_lines
	dor_varquadgrid_transfer_vtx_info 
	dor_varquadgrid_calculate_quad_normals
	dor_varquadgrid_calculate_vertex_normals
	dor_varquadgrid_compute_bounding_volume 
	dor_varquadgrid_pick 
	dor_varquadgrid_pick_lines 
	dor_varquadgrid_pick_tris 
	dor_varquadgrid_create_alternate_geom_tris
	dor_varquadgrid_update 
	dor_varquadgrid_update_uv
	dor_varquadgrid_update_uvw

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/varquadgrd.h>
#include <dore/dore_develop/private/varlinlst.h>
#include <dore/dore_develop/private/vartrimsh.h>

/*
 ======================================================================
 */

DtInt doe_varquadgrid_class_id;

static DtMethodEntry varquadgrid_methods[] = {
    {DcMethodCmpBndVolume,(DtMethodPtr)dor_varquadgrid_compute_bounding_volume},
    {DcMethodDestroy,      (DtMethodPtr)dor_varquadgrid_destroy},
    {DcMethodPick,          (DtMethodPtr)dor_varquadgrid_pick},
    {DcMethodPrint,          (DtMethodPtr)dor_varquadgrid_print},
    {DcMethodUpdStdAltObj,    (DtMethodPtr)dor_varquadgrid_update_alternate},
    {DcMethodStdRenderDisplay, (DtMethodPtr)dor_varquadgrid_render_standard},
    {DcMethodUpdStdAltLineObj,  (DtMethodPtr)dor_varquadgrid_update_lines}
};

void dor_varquadgrid_initialize (void)
{
    doe_varquadgrid_class_id = dor_class_add ("DoVarQuadGrid", 7,
                               varquadgrid_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_varquadgrid_create (
    DtColorModel colormodel,
    DtInt width,
    DtInt height,
    DtReal *vertices,
    DtReal *vertnormal,
    DtReal *vertcolor,
    DtFlag smoothflag)
{
   static DtFlag initialized = DcFalse;
   struct varquadgrid *varquadgrid;
   DtInt i;
   dot_object *varquadgridobject;
   
   if (!initialized) {
      dor_varquadgrid_initialize();
      initialized = DcTrue;
   }

   /* we assume the vertices are given row by row, from top to bottom,
      for example if width=4 and height=3, we have:

          0    1    2    3
          x----x----x----x
          |    |    |    |
          4    5    6    7
          x----x----x----x
          |    |    |    |
          8    9   10   11
          x----x----x----x
    */


   varquadgrid = (struct varquadgrid *)dor_space_allocate(sizeof *varquadgrid);
   
   varquadgrid->colormodel = colormodel;
   varquadgrid->vertex_location = (DtRealTriple *)vertices;
   varquadgrid->vertex_normal = (DtRealTriple *)vertnormal;
   varquadgrid->vertex_color = (DtColorRGB *)vertcolor;
   varquadgrid->width = width;
   varquadgrid->height = height;
   varquadgrid->smoothflag = smoothflag;
   varquadgrid->needelemnrms = DcTrue;
   varquadgrid->element_normal = (DtRealTriple *)NULL;

   varquadgrid->alt_object_lines = NULL;
   varquadgrid->alt_linevtx = (DtRealTriple *)NULL;
   varquadgrid->alt_linenrm = (DtRealTriple *)NULL;
   varquadgrid->alt_lineclr = (DtColorRGB *)NULL;
   varquadgrid->decompose_lines = DcTrue;

   varquadgrid->alt_object_tris = NULL;
   varquadgrid->alt_trivtxlist = NULL;

   varquadgrid->uv_count = 0;
   varquadgrid->uv_list = (DtRealCouple **)dor_space_allocate(
                dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
   for (i=0; i<dod_system_max_vertex_uv_cnt; i++) {
	varquadgrid->uv_list[i] = NULL;
   }

   varquadgrid->uvw_count = 0;
   varquadgrid->uvw_list = (DtRealTriple **)dor_space_allocate(
                dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *));
   for (i=0; i<dod_system_max_vertex_uvw_cnt; i++) {
	varquadgrid->uvw_list[i] = NULL;
   }

   if (smoothflag) {
	if (vertnormal == NULL) {
		varquadgrid->needvertnrms = DcTrue;		
	} else {
		/* If we got vertex normal from the user never
		mess with them */
		varquadgrid->smoothflag = DcFalse;
		varquadgrid->needvertnrms = DcFalse;
	}
   } else {
	varquadgrid->needvertnrms = DcFalse;
   }

   varquadgrid->sequence_number = 1;

   /* Edge enable field not currently used */
   varquadgrid->edge_enable = NULL;

   dor_varquadgrid_bounding_box(varquadgrid);

   varquadgridobject = dor_object_create(doe_varquadgrid_class_id,varquadgrid);
   return(varquadgridobject);
}

/*
 ======================================================================
 */

void dor_varquadgrid_destroy (dot_object *object)
{
    struct varquadgrid *varquadgrid;

    varquadgrid = (struct varquadgrid *)(object->data);

    if (varquadgrid->smoothflag && (varquadgrid->vertex_normal != NULL)) {
	dor_space_deallocate(varquadgrid->vertex_normal);
    }

    if (varquadgrid->element_normal != NULL) {
	dor_space_deallocate(varquadgrid->element_normal);
    }

    if (varquadgrid->alt_object_lines != NULL) {
        dor_object_check_deletion(varquadgrid->alt_object_lines);
    }

    if (varquadgrid->alt_linevtx != NULL) {
        dor_space_deallocate(varquadgrid->alt_linevtx);
    }

    if (varquadgrid->alt_linenrm != NULL) {
        dor_space_deallocate(varquadgrid->alt_linenrm);
    }

    if (varquadgrid->alt_lineclr != NULL) {
        dor_space_deallocate(varquadgrid->alt_lineclr);
    }

    if (varquadgrid->alt_object_tris != NULL) {
        dor_object_check_deletion(varquadgrid->alt_object_tris);
    }

    if (varquadgrid->alt_trivtxlist != NULL) {
        dor_space_deallocate(varquadgrid->alt_trivtxlist);
    }

    if (varquadgrid->edge_enable != NULL) {
        dor_space_deallocate(varquadgrid->edge_enable);
    }

    dor_space_deallocate (varquadgrid->uv_list);
    dor_space_deallocate (varquadgrid->uvw_list);

    dor_space_deallocate(varquadgrid);
}
/*
 ======================================================================
 */

void dor_varquadgrid_print (dot_object *object)
{
    struct varquadgrid *varquadgrid;

    varquadgrid = (struct varquadgrid *)(object->data);

    if (varquadgrid == NULL){
	dor_print_output("variable triangle mesh is NULL");
	return;
    }
 
    switch (varquadgrid->colormodel){
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;
 
    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    sprintf(dor_print_get_line(),
	    "Grid width is %d grid height is %d", varquadgrid->width,
             varquadgrid->height);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex locations at [%lx]", varquadgrid->vertex_location);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex normals at [%lx]", varquadgrid->vertex_normal);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex colors at [%lx]", varquadgrid->vertex_color);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"smooth flag = %d",varquadgrid->smoothflag);
    dor_print_output(dor_print_get_line());

}
/*
 ======================================================================
 */

void dor_varquadgrid_render_standard (dot_object *object)
{
    dot_object *alt;

    if (!dor_condex_query_render(doe_varquadgrid_class_id))
            return;

    alt = dor_varquadgrid_update_alternate (object);
    if (alt != DcNullPtr) {
        dor_object_get_current_method(alt) (alt);
    }
}

/*
 ======================================================================
 */
dot_object *dor_varquadgrid_update_alternate (dot_object *object)
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
        alt = dor_varquadgrid_update_tris(object);
        break;

    case DcOutlines:
        alt = dor_varquadgrid_update_lines(object);
        break;
   default:
        DDerror(ERR_VALUE_OUT_OF_RANGE,
                   "dor_varquadgrid_update_alternate",
                   "unknown reptype");
        alt = DcNullPtr;
        break;
    }
    return alt;
}

/*
 ======================================================================
 */

dot_object *dor_varquadgrid_update_lines (dot_object *object)
{
    struct varquadgrid *varquadgrid;

    varquadgrid = (struct varquadgrid *)(object->data);

    if (varquadgrid->decompose_lines) {
	if (varquadgrid->alt_object_lines != DcNullPtr) {
	    dor_object_get_method(varquadgrid->alt_object_lines,
				  DcMethodDestroy)(varquadgrid->alt_object_lines);
	    varquadgrid->alt_object_lines = DcNullPtr;
	} 

	varquadgrid->alt_object_lines =
		dor_varquadgrid_create_alternate_geom_lines(varquadgrid);

	if (varquadgrid->alt_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_varquadgrid_update_lines", "");
	} else {
	    varquadgrid->decompose_lines = DcFalse;
	}
    }
    return varquadgrid->alt_object_lines;
}
/*
 ======================================================================
 */
dot_object *dor_varquadgrid_update_tris (dot_object *object)
{
    struct varquadgrid *varquadgrid;

    varquadgrid = (struct varquadgrid *)(object->data);

    if (varquadgrid->needvertnrms) {
        if (varquadgrid->vertex_normal == NULL) {

        /* need element normals to compute vertex normals */
            varquadgrid->element_normal =
                    (DtRealTriple *)dor_space_allocate(
                     (varquadgrid->width-1)*(varquadgrid->height-1) *
                     (sizeof(DtRealTriple)));
      
            varquadgrid->vertex_normal =
                    (DtRealTriple *)dor_space_allocate(
                     varquadgrid->width*varquadgrid->height *
                     (sizeof(DtRealTriple)));
       }
        dor_varquadgrid_calculate_quad_normals(varquadgrid);
        dor_varquadgrid_calculate_vertex_normals(varquadgrid);
        varquadgrid->needvertnrms = DcFalse;
    }

    if (varquadgrid->alt_object_tris == NULL) {

        varquadgrid->alt_object_tris =
                dor_varquadgrid_create_alternate_geom_tris(varquadgrid);

        if (varquadgrid->alt_object_tris == NULL) 
            DDerror(ERR_CANT_CREATE_ALTOBJ,
                       "dor_varquadgrid_update_tris", "");

    }

    return varquadgrid->alt_object_tris;
}

/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

void dor_varquadgrid_bounding_box (struct varquadgrid *varquadgrid)
{
    DtReal xmin, ymin, zmin, xmax, ymax, zmax;
    DtInt vertexno;
    DtInt vertexcount;

    xmin = varquadgrid->vertex_location[0][0];
    xmax = varquadgrid->vertex_location[0][0];

    ymin = varquadgrid->vertex_location[0][1];
    ymax = varquadgrid->vertex_location[0][1];

    zmin = varquadgrid->vertex_location[0][2];
    zmax = varquadgrid->vertex_location[0][2];

    vertexcount = varquadgrid->width*varquadgrid->height;

PRAGMA(IVDEP)
PRAGMA(VBEST)
            for (vertexno=1;vertexno<vertexcount;vertexno++){
		xmin = min(xmin,varquadgrid->vertex_location[vertexno][0]);
		xmax = max(xmax,varquadgrid->vertex_location[vertexno][0]);

		ymin = min(ymin,varquadgrid->vertex_location[vertexno][1]);
		ymax = max(ymax,varquadgrid->vertex_location[vertexno][1]);

		zmin = min(zmin,varquadgrid->vertex_location[vertexno][2]);
		zmax = max(zmax,varquadgrid->vertex_location[vertexno][2]);

		zmin = min(zmin,varquadgrid->vertex_location[vertexno][3]);
		zmax = max(zmax,varquadgrid->vertex_location[vertexno][3]);
	    }

    varquadgrid->bounding_box_pts[0][0] = xmin;
    varquadgrid->bounding_box_pts[0][1] = ymin;
    varquadgrid->bounding_box_pts[0][2] = zmin;

    varquadgrid->bounding_box_pts[1][0] = xmin;
    varquadgrid->bounding_box_pts[1][1] = ymax;
    varquadgrid->bounding_box_pts[1][2] = zmin;

    varquadgrid->bounding_box_pts[2][0] = xmax;
    varquadgrid->bounding_box_pts[2][1] = ymax;
    varquadgrid->bounding_box_pts[2][2] = zmin;

    varquadgrid->bounding_box_pts[3][0] = xmax;
    varquadgrid->bounding_box_pts[3][1] = ymin;
    varquadgrid->bounding_box_pts[3][2] = zmin;

    varquadgrid->bounding_box_pts[4][0] = xmin;
    varquadgrid->bounding_box_pts[4][1] = ymin;
    varquadgrid->bounding_box_pts[4][2] = zmax;

    varquadgrid->bounding_box_pts[5][0] = xmin;
    varquadgrid->bounding_box_pts[5][1] = ymax;
    varquadgrid->bounding_box_pts[5][2] = zmax;

    varquadgrid->bounding_box_pts[6][0] = xmax;
    varquadgrid->bounding_box_pts[6][1] = ymax;
    varquadgrid->bounding_box_pts[6][2] = zmax;

    varquadgrid->bounding_box_pts[7][0] = xmax;
    varquadgrid->bounding_box_pts[7][1] = ymin;
    varquadgrid->bounding_box_pts[7][2] = zmax;
}
/*
 ======================================================================
 */

dot_object *dor_varquadgrid_create_alternate_geom_lines (
    struct varquadgrid *varquadgrid)
{
    DtInt i,j, linevtxcnt;
    DtInt linevtx, quadvtx;
    dot_object *varlinlstobj;

    /* compute total number of lines in the grid * 2 = number
       of vertices needed for the line list  */
    linevtxcnt = 2*(varquadgrid->width *(varquadgrid->height-1) +
                   varquadgrid->height*(varquadgrid->width-1));

    if (varquadgrid->alt_linevtx == NULL) {

	varquadgrid->alt_linevtx = (DtRealTriple *)dor_space_allocate
		(linevtxcnt*(sizeof (DtRealTriple)));

	if (varquadgrid->vertex_normal != NULL) {
	    varquadgrid->alt_linenrm = 
		    (DtRealTriple *)dor_space_allocate
		(linevtxcnt*(sizeof (DtRealTriple)));
	}

	if (varquadgrid->vertex_color != NULL) {
	    varquadgrid->alt_lineclr = 
		    (DtRealTriple *)dor_space_allocate
			    (linevtxcnt*(sizeof (DtRealTriple)));
	}
    }

    linevtx = 0;

    /* this loop makes all horizontal lines, from top to bottom */
    for (j=0; j<varquadgrid->height; j++) {
        for (i=0; i<varquadgrid->width-1; i++) {

            quadvtx = j*varquadgrid->width+i;
            dor_varquadgrid_transfer_vtx_info(quadvtx, linevtx, varquadgrid);

            /* other end of first line (going "right") */
            dor_varquadgrid_transfer_vtx_info(quadvtx+1, linevtx+1, varquadgrid);
            linevtx += 2;
        }
    }

    /* this loop makes all vertical lines, from left to right */
    for (j=0; j<varquadgrid->width; j++) {
        for (i=0; i<varquadgrid->height-1; i++) {

            quadvtx = i*varquadgrid->width+j;
            dor_varquadgrid_transfer_vtx_info(quadvtx, linevtx, varquadgrid);

            /* other end of first line (going "down") */
            dor_varquadgrid_transfer_vtx_info(quadvtx+varquadgrid->width,
                                              linevtx+1, varquadgrid);
            linevtx += 2;
        }
    }

    varlinlstobj = dor_varlinlst_create(varquadgrid->colormodel, linevtx/2,
					(DtReal*)varquadgrid->alt_linevtx,
					(DtReal*)varquadgrid->alt_linenrm,
					(DtReal*)varquadgrid->alt_lineclr);
    return(varlinlstobj);
}
/*
 ======================================================================
 */

void dor_varquadgrid_transfer_vtx_info (
    DtInt quadvtx,
    DtInt linevtx,
    struct varquadgrid *varquadgrid)
{
    varquadgrid->alt_linevtx[linevtx][0] =
                    varquadgrid->vertex_location[quadvtx][0];
    varquadgrid->alt_linevtx[linevtx][1] =
                    varquadgrid->vertex_location[quadvtx][1];
    varquadgrid->alt_linevtx[linevtx][2] =
                    varquadgrid->vertex_location[quadvtx][2];

    if (varquadgrid->vertex_normal != NULL) {
        varquadgrid->alt_linenrm[linevtx][0] =
                        varquadgrid->vertex_normal[quadvtx][0];
        varquadgrid->alt_linenrm[linevtx][1] =
                        varquadgrid->vertex_normal[quadvtx][1];
        varquadgrid->alt_linenrm[linevtx][2] =
                        varquadgrid->vertex_normal[quadvtx][2];
    }

    if (varquadgrid->vertex_color != NULL) {
        varquadgrid->alt_lineclr[linevtx][0] =
                        varquadgrid->vertex_color[quadvtx][0];
        varquadgrid->alt_lineclr[linevtx][1] =
                        varquadgrid->vertex_color[quadvtx][1];
        varquadgrid->alt_lineclr[linevtx][2] =
                        varquadgrid->vertex_color[quadvtx][2];
    }

}
/*
 ======================================================================
 */

void dor_varquadgrid_calculate_quad_normals (struct varquadgrid *varquadgrid)
{
    DtInt quadno, i, j;
    DtInt vertex0, vertex1, vertex2;

    /* This code assumes the quad is planar and uses the first three
       vertices of the quad. */

    quadno = 0;
    for (j=0;j<varquadgrid->height-1;j++){
        for (i=0;i<varquadgrid->width-1;i++){

            vertex0 = j*varquadgrid->width + i;
            vertex1 = vertex0 + 1;
            vertex2 = vertex0 + varquadgrid->width + 1;

            dor_math_fast_cross_cosines(
                  varquadgrid->vertex_location[vertex0],
                  varquadgrid->vertex_location[vertex1],
                  varquadgrid->vertex_location[vertex2],
                  varquadgrid->element_normal[quadno]);

            dor_math_renormalize_fast(varquadgrid->element_normal[quadno]);

            quadno++;
        }
    }
}
/*
 ======================================================================
 */

void dor_varquadgrid_calculate_vertex_normals (struct varquadgrid *varquadgrid)
{
    DtInt vertexno, vno[4];
    DtInt quadno, vertexcount;
    DtInt i, j, k;

    vertexcount = varquadgrid->width*varquadgrid->height;

PRAGMA(ASIS)
    for (vertexno=0;vertexno<vertexcount;vertexno++) {
        varquadgrid->vertex_normal[vertexno][0] = 0.;
        varquadgrid->vertex_normal[vertexno][1] = 0.;
        varquadgrid->vertex_normal[vertexno][2] = 0.;
    }

    /*  For each quads' four vertices, accumulate quad normal. */

PRAGMA(ASIS)
    quadno = 0;
    for (j=0;j<varquadgrid->height-1;j++) {
        for (i=0;i<varquadgrid->width-1;i++) {
            vno[0] = j*varquadgrid->width + i;  /* upperleft corner of quad */
            vno[1] = vno[0] + 1;
            vno[2] = vno[0] + varquadgrid->width;
            vno[3] = vno[2] + 1;
            for (k=0; k<4; k++) {
                varquadgrid->vertex_normal[vno[k]][0] +=
                                varquadgrid->element_normal[quadno][0];
                varquadgrid->vertex_normal[vno[k]][1] +=
                                varquadgrid->element_normal[quadno][1];
                varquadgrid->vertex_normal[vno[k]][2] +=
                                varquadgrid->element_normal[quadno][2];
            }
        quadno++;
        }
    }

    /*  Rescale the vertex normals.  */

PRAGMA(ASIS)
    for (vertexno=0;vertexno<vertexcount;vertexno++) {
        if (!dor_math_renormalize_vector(varquadgrid->vertex_normal[vertexno])) {
            DDerror (ERR_ZERO_QUAD_NRM_SUM,
                    "dor_varquadgrid_calculate_vertex_normals",
                    DcNullPtr);
        }
    }
}
/*
 ======================================================================
 */

void dor_varquadgrid_compute_bounding_volume (dot_object *object)
{
    struct varquadgrid *varquadgrid;

    varquadgrid = (struct varquadgrid *)(object->data);

    dor_system_report_bounding_points
	(8, (DtReal*)varquadgrid->bounding_box_pts);
}

/*
 ======================================================================
 */

void dor_varquadgrid_pick (dot_object *object)
{
    DtRepType reptype;

    if (!dor_condex_query_pick(doe_varquadgrid_class_id))
	    return;

    dor_pick_initialize_object(object);

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    case DcOutlines:
        dor_varquadgrid_pick_lines(object);
        break;

    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
    case DcWireframe:
    case DcPoints:
        dor_varquadgrid_pick_tris(object);
        break;
    }
}
/*
 ======================================================================
 */

void dor_varquadgrid_pick_lines (dot_object *object)
{
    struct varquadgrid *varquadgrid;

    varquadgrid = (struct varquadgrid *)(object->data);

    if (varquadgrid->decompose_lines) {
        if (varquadgrid->alt_object_lines != DcNullPtr) {
            doe_system_alternate_object_execution_flag = DcTrue;

            dor_object_get_method(varquadgrid->alt_object_lines,
                                  DcMethodDestroy)(varquadgrid->alt_object_lines);

            doe_system_alternate_object_execution_flag = DcFalse;

            varquadgrid->alt_object_lines = NULL;
        }

        varquadgrid->alt_object_lines =
                dor_varquadgrid_create_alternate_geom_lines(varquadgrid);

        if (varquadgrid->alt_object_lines == NULL) {
            DDerror(ERR_CANT_CREATE_ALTOBJ,
                       "dor_varquadgrid_pick_lines", "");
            return;
        }

        varquadgrid->decompose_lines = DcFalse;
    }

    doe_system_alternate_object_execution_flag = DcTrue;

    dor_object_get_method(varquadgrid->alt_object_lines,
                          DcMethodPick)(varquadgrid->alt_object_lines);
    doe_system_alternate_object_execution_flag = DcFalse;

    return;
}

/*
 ======================================================================
 */

void dor_varquadgrid_pick_tris (dot_object *object)
{
    struct varquadgrid *varquadgrid;

    varquadgrid = (struct varquadgrid *)(object->data);

    if (varquadgrid->alt_object_tris == NULL) {
        varquadgrid->alt_object_tris =
              dor_varquadgrid_create_alternate_geom_tris(varquadgrid);
        if (varquadgrid->alt_object_tris == NULL) {
            DDerror(ERR_CANT_CREATE_ALTOBJ,
                       "dor_varquadgrid_pick_tris", "");
            return;
        }
    }

    doe_system_alternate_object_execution_flag = DcTrue;

    dor_object_get_method(varquadgrid->alt_object_tris,
                          DcMethodPick)(varquadgrid->alt_object_tris);

    doe_system_alternate_object_execution_flag = DcFalse;

    return;
}

/*
 ======================================================================
 */

dot_object *dor_varquadgrid_create_alternate_geom_tris (
    struct varquadgrid *varquadgrid)
{
    DtInt vertexno;
    DtInt trianglecount;
    DtInt vertexcount;
    DtUInt i, j;
    DtIntTriple *vconnectptr;
    dot_object *vartrimshobj;

    /* number of triangles is twice the number of quads */
    trianglecount = 2 * (varquadgrid->width-1)*(varquadgrid->height-1);

    if (varquadgrid->alt_trivtxlist == NULL) {
        varquadgrid->alt_trivtxlist = (DtIntTriple *)dor_space_allocate(
                                    trianglecount*(sizeof (DtIntTriple)));

        vconnectptr = varquadgrid->alt_trivtxlist;

        for (j=0; j<varquadgrid->height-1; j++) {
            for (i=0; i<varquadgrid->width-1; i++) {

                vertexno = j*varquadgrid->width + i;
                (*vconnectptr)[0] = vertexno;
                (*vconnectptr)[1] = vertexno + 1;
                (*vconnectptr)[2] = vertexno + 1 + varquadgrid->width;
                vconnectptr++;

                (*vconnectptr)[0] = vertexno;
                (*vconnectptr)[1] = vertexno + 1 + varquadgrid->width;
                (*vconnectptr)[2] = vertexno + varquadgrid->width;
                vconnectptr++;
            }
        }
    }

    vertexcount = varquadgrid->width*varquadgrid->height;

    vartrimshobj=dor_vartrimsh_create(varquadgrid->colormodel,
                                        vertexcount,
                                        (DtReal*)varquadgrid->vertex_location,
                                        (DtReal*)varquadgrid->vertex_normal,
                                        (DtReal*)varquadgrid->vertex_color,
                                        trianglecount,
                                        varquadgrid->alt_trivtxlist,
                                        varquadgrid->smoothflag);

    if (vartrimshobj != DcNullObject) {
        /* copy the varquadgrid's uv/uvws for the vartrimsh */
        for (i=0; i<varquadgrid->uv_count; i++) 
           dor_vartrimsh_update_uv(vartrimshobj, i, 
                                   varquadgrid->uv_list[i]);

        for (i=0; i<varquadgrid->uvw_count; i++) 
           dor_vartrimsh_update_uvw(vartrimshobj, i,
                                    varquadgrid->uvw_list[i]);
    }

    return(vartrimshobj);
}

/*
 ======================================================================
 */

void dor_varquadgrid_update (
    dot_object *object,
    DtReal *vlocs,
    DtReal *vnrms,
    DtReal *vclrs,
    DtFlag recompute_nrms)
{
    struct varquadgrid *varquadgrid;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_varquadgrid_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarQuadGrid",DcNullPtr);
	return;
    }   
#endif

    varquadgrid = (struct varquadgrid *)(object->data);

    varquadgrid->sequence_number++;
   
    if (vlocs != NULL) {
	varquadgrid->vertex_location = (DtRealTriple *)vlocs;

	dor_varquadgrid_bounding_box(varquadgrid);

	if (recompute_nrms) {
	    varquadgrid->needelemnrms = DcTrue;
	    if (varquadgrid->smoothflag) 
		    varquadgrid->needvertnrms = DcTrue;
	} else {
	    varquadgrid->needelemnrms = DcFalse;
	    varquadgrid->needvertnrms = DcFalse;
	}
	
    }

    /* If object was created without vertex normals then you cannot
       pass them in now */
	
    if (vnrms != NULL) {
	if ((varquadgrid->vertex_normal != NULL) && !varquadgrid->smoothflag) 
		varquadgrid->vertex_normal = (DtRealTriple *)vnrms;
	else
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "dor_varquadgrid_update", 
			    "vertex normals ignored");
    }	

    /* If object was created without colors then you cannot 
       pass them in now */

    if (vclrs != NULL) {
	if (varquadgrid->vertex_color != NULL)
		varquadgrid->vertex_color = (DtColorRGB *)vclrs;
	else 
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "dor_varquadgrid_update", 
			    "vertex colors ignored");
    }

    if (varquadgrid->alt_object_tris != NULL)
            dor_vartrimsh_update(varquadgrid->alt_object_tris,
                                 vlocs, vnrms, vclrs, DcFalse);

    /* can't use dor_varlinlst_update because line alt obj.
       does not share the same set of vertices. Will need to 
       remake alt. line obj. if needed */
    varquadgrid->decompose_lines = DcTrue;

}
/*
 ======================================================================
 */

void dor_varquadgrid_update_uv (
    dot_object *object,
    DtInt uvindex,
    DtRealCouple *uvlist)
{
    struct varquadgrid *varquadgrid;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_varquadgrid_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarQuadGridUV",DcNullPtr);
	return;
    }   
#endif

    varquadgrid = (struct varquadgrid *)(object->data);

#ifdef ERRCHK
    if (uvindex<0) {
          DDerror(ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarQuadGridUV",
                        "uv index < 0");
          return;
    } else if (uvindex > varquadgrid->uv_count) {
          DDerror(ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarQuadGridUV",
                        "uv index too large");
          return;
    }
#endif

    varquadgrid->sequence_number++;
    if (varquadgrid->uv_count < uvindex+1) {
	varquadgrid->uv_count = uvindex+1;
    }
    varquadgrid->uv_list[uvindex] = uvlist;

   /* update the uv's of the underlying vartrimsh if it exists */

    if (varquadgrid->alt_object_tris != NULL) 
        dor_vartrimsh_update_uv(varquadgrid->alt_object_tris,
                                uvindex, uvlist);

}
/*
 ======================================================================
 */

void dor_varquadgrid_update_uvw (
    dot_object *object,
    DtInt uvwindex,
    DtRealTriple *uvwlist)
{
    struct varquadgrid *varquadgrid;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_varquadgrid_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarQuadGridUVW",DcNullPtr);
	return;
    }   
#endif

    varquadgrid = (struct varquadgrid *)(object->data);

#ifdef ERRCHK
    if (uvwindex<0) {
          DDerror (ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarQuadGridUVW",
                        "uvw index < 0");

          return;
    } else if (uvwindex > varquadgrid->uvw_count) {
          DDerror (ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarQuadGridUVW",
                        "uvw index too large");
          return;
    }

#endif

    varquadgrid->sequence_number++;
    if (varquadgrid->uvw_count < uvwindex+1)
	    varquadgrid->uvw_count = uvwindex+1;
    varquadgrid->uvw_list[uvwindex] = uvwlist;   

   /* update the uvw's of the underlying vartrimsh if it exists */

    if (varquadgrid->alt_object_tris != NULL) 
        dor_vartrimsh_update_uvw(varquadgrid->alt_object_tris,
                                 uvwindex, uvwlist);
}
