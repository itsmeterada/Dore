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
	dor_quadlst_initialize
	dor_quadlst_create
	dor_quadlst_destroy
	dor_quadlst_print
	dor_quadlst_update_alternate
	dor_quadlst_update_lines
	dor_quadlst_update_tris
	dor_quadlst_create_alternate_geom_lines
	dor_quadlst_create_alternate_geom_tris
	dor_quadlst_calculate_quad_normals
	dor_quadlst_bounding_box
	dor_quadlst_compute_bounding_volume
	dor_quadlst_pick
	dor_quadlst_pick_lines
	dor_quadlst_pick_tris
        dor_quadlst_render_standard


  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/internal/glbatt.h>
#include <dore/dore_develop/private/quadlst.h>
#include <dore/dore_develop/private/linlst.h>
#include <dore/dore_develop/private/trilst.h>
/*
 ======================================================================
 */

DtInt doe_quadlst_class_id;

static DtMethodEntry quadlst_methods[] = {
    {DcMethodCmpBndVolume,    (DtMethodPtr)dor_quadlst_compute_bounding_volume},
    {DcMethodDestroy,         (DtMethodPtr)dor_quadlst_destroy},
    {DcMethodPick,            (DtMethodPtr)dor_quadlst_pick},
    {DcMethodPrint,           (DtMethodPtr)dor_quadlst_print},
    {DcMethodUpdStdAltObj,    (DtMethodPtr)dor_quadlst_update_alternate},
    {DcMethodUpdStdAltLineObj,(DtMethodPtr)dor_quadlst_update_lines},
    {DcMethodStdRenderDisplay,(DtMethodPtr)dor_quadlst_render_standard}
};

void dor_quadlst_initialize (void)
{
    doe_quadlst_class_id = dor_class_add ("DoQuadList", 7, 
		quadlst_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_quadlst_create (
    DtColorModel colormodel,
    DtVertexType vertextype,
    DtInt quadcount,
    DtReal vertices[3])
{
   static DtFlag initialized = DcFalse;
   struct quadlst *quadlst;
   DtInt vertexcount;
   DtInt vertexno;
   DtReal *vertexptr;
   dot_object *quadlstobject;
   DtVertexType basevtxtype;
   DtInt uv;
   DtInt uvw;

   if (!initialized) {
      	dor_quadlst_initialize();
   	initialized = DcTrue;
   }

   quadlst = (struct quadlst *)dor_space_allocate(sizeof *quadlst);

   quadlst->alt_object_lines = (DtObjectStructure *)0;
   quadlst->alt_object_tris = (DtObjectStructure *)0;

   quadlst->colormodel = colormodel;

   quadlst->quad_count = quadcount;
   vertexcount = quadcount * 4;

   basevtxtype = dod_system_inq_vertex_basetype(vertextype);
   quadlst->vertextype = basevtxtype;

   quadlst->vertex_location = (DtRealTriple *)dor_space_allocate(
			   vertexcount*(sizeof (DtRealTriple)));

   /* texture coordinates */

   quadlst->uv_list = (DtRealCouple **)dor_space_allocate(
		dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
   quadlst->uvw_list = (DtRealTriple **)dor_space_allocate(
		dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *));

   for (uv = 0; uv < dod_system_max_vertex_uv_cnt; uv++) {
   	quadlst->uv_list[uv] = NULL;
   }
   for (uvw = 0; uvw < dod_system_max_vertex_uvw_cnt; uvw++) {
   	quadlst->uvw_list[uvw] = NULL;
   }

   quadlst->uv_count = dod_system_inq_vertex_uv_cnt(vertextype);
   quadlst->uvw_count = dod_system_inq_vertex_uvw_cnt(vertextype);

   for (uv=0; uv< quadlst->uv_count; uv++) {
   	quadlst->uv_list[uv] = (DtRealCouple *)dor_space_allocate(
				vertexcount*(sizeof (DtRealCouple)));
   }

   for (uvw=0; uvw< quadlst->uvw_count; uvw++) {
   	quadlst->uvw_list[uvw] = (DtRealTriple *)dor_space_allocate(
				vertexcount*(sizeof (DtRealTriple)));
   }

   /**/
   vertexptr = vertices;

    quadlst->vertex_normal = (DtRealTriple *) NULL;
    quadlst->vertex_color = (DtColorRGB *) NULL;
    quadlst->vertex_alpha = (DtReal *) NULL;

    if (basevtxtype & DcLocNrm)
        quadlst->vertex_normal = (DtRealTriple *)
            dor_space_allocate(vertexcount*(sizeof (DtRealTriple)));

    if (basevtxtype & DcLocClr)
        quadlst->vertex_color = (DtColorRGB *)
            dor_space_allocate(vertexcount*(sizeof (DtColorRGB)));

    if (basevtxtype & DcLocAlpha)
        quadlst->vertex_alpha = (DtReal *)
            dor_space_allocate(vertexcount*(sizeof (DtReal)));

    for (vertexno=0;vertexno<vertexcount;vertexno++) {

        quadlst->vertex_location[vertexno][0] = *vertexptr++;
        quadlst->vertex_location[vertexno][1] = *vertexptr++;
        quadlst->vertex_location[vertexno][2] = *vertexptr++;

        if (basevtxtype & DcLocNrm) {
            quadlst->vertex_normal[vertexno][0] = *vertexptr++;
            quadlst->vertex_normal[vertexno][1] = *vertexptr++;
            quadlst->vertex_normal[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocClr) {
            quadlst->vertex_color[vertexno][0] = *vertexptr++;
            quadlst->vertex_color[vertexno][1] = *vertexptr++;
            quadlst->vertex_color[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocAlpha) {
            quadlst->vertex_alpha[vertexno] = *vertexptr++;
        }

        for (uv=0; uv < quadlst->uv_count; uv++) {
            quadlst->uv_list[uv][vertexno][0] = *vertexptr++;
            quadlst->uv_list[uv][vertexno][1] = *vertexptr++;
        }

        for (uvw=0; uvw < quadlst->uvw_count; uvw++) {
            quadlst->uvw_list[uvw][vertexno][0] = *vertexptr++;
            quadlst->uvw_list[uvw][vertexno][1] = *vertexptr++;
            quadlst->uvw_list[uvw][vertexno][2] = *vertexptr++;
        }

    }

    /*  Compute element normals for all quads in the list.  */

   quadlst->element_normal = (DtRealTriple *)dor_space_allocate(
			quadcount*(sizeof (DtRealTriple)));

   dor_quadlst_calculate_quad_normals(quadlst);

   dor_quadlst_bounding_box(quadlst);
   
   quadlstobject = dor_object_create(doe_quadlst_class_id,quadlst);

   return(quadlstobject);
}
/*
 ======================================================================
 */

void dor_quadlst_destroy (dot_object *object)
{
    struct quadlst *quadlst;
    int i;

    quadlst = (struct quadlst *)(object->data);

    if (quadlst->vertex_location != NULL) {
	dor_space_deallocate(quadlst->vertex_location);
    }

    if (quadlst->vertex_normal != NULL) {
	dor_space_deallocate(quadlst->vertex_normal);
    }

    if (quadlst->vertex_color != NULL) {
	dor_space_deallocate(quadlst->vertex_color);
    }

    if (quadlst->vertex_alpha != NULL) {
	dor_space_deallocate(quadlst->vertex_alpha);
    }

    if (quadlst->element_normal != NULL) {
	dor_space_deallocate(quadlst->element_normal);
    }

    for (i=0; i< quadlst->uv_count; i++) {
	dor_space_deallocate(quadlst->uv_list[i]);
    }
    dor_space_deallocate(quadlst->uv_list);

    for (i=0; i< quadlst->uvw_count; i++) {
	dor_space_deallocate(quadlst->uvw_list[i]);
    }
    dor_space_deallocate(quadlst->uvw_list);

    dor_space_deallocate(quadlst);
}
/*
 ======================================================================
 */

void dor_quadlst_print (dot_object *object)
{
    struct quadlst *quadlst;

    quadlst = (struct quadlst *)(object->data);

    if (quadlst == NULL){
	dor_print_output("quad list is NULL");
	return;
    }
 
    switch (quadlst->colormodel){
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;
 
    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    dor_utilpri_print_vertextype(quadlst->vertextype);

    sprintf(dor_print_get_line(),"quad count = %d",quadlst->quad_count);
    dor_print_output(dor_print_get_line());

    if (quadlst->uv_count > 0) {
	sprintf (dor_print_get_line(), "uv's per vertex = %d", quadlst->uv_count);
	dor_print_output(dor_print_get_line());
    }

    if (quadlst->uvw_count > 0) {
	sprintf (dor_print_get_line(), "uvw's per vertex = %d", quadlst->uvw_count);
	dor_print_output(dor_print_get_line());
    }
}
/*
 ======================================================================
 */
dot_object *dor_quadlst_update_alternate (dot_object *object)
{
    DtRepType	reptype;
    dot_object	*alt;

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    	case DcSurface:
    	case DcSurfaceWireframeEdges:
    	case DcSurfaceOutlineEdges:
    	case DcWireframe:
    	case DcPoints:
            alt = dor_quadlst_update_tris(object);
            break;

    	case DcOutlines:
            alt = dor_quadlst_update_lines(object);
            break;
    	default:
            DDerror(ERR_VALUE_OUT_OF_RANGE,
                   "dor_quadlst_update_alternate",
                   "unknown reptype");
            alt = DcNullPtr;
            break;
    }
    return alt;
}
/*
 ======================================================================
 */
dot_object *dor_quadlst_update_lines (dot_object *object)
{
    struct quadlst *quadlst;
    dot_object *alt;

    quadlst = (struct quadlst *)(object->data);

    if (quadlst->alt_object_lines != DcNullPtr) {
	alt = quadlst->alt_object_lines;

    } else {
	quadlst->alt_object_lines = 
		dor_quadlst_create_alternate_geom_lines(quadlst);
	alt = quadlst->alt_object_lines;

	if (quadlst->alt_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_quadlst_update_lines", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */
dot_object *dor_quadlst_update_tris (dot_object *object)
{
    struct quadlst *quadlst;
    dot_object *alt;

    quadlst = (struct quadlst *)(object->data);

    if (quadlst->alt_object_tris != DcNullPtr) {
        alt = quadlst->alt_object_tris;

    } else {
        quadlst->alt_object_tris =
                dor_quadlst_create_alternate_geom_tris(quadlst);
        alt = quadlst->alt_object_tris;

        if (quadlst->alt_object_tris == DcNullPtr) {
            DDerror(ERR_CANT_CREATE_ALTOBJ,
                       "dor_quadlst_update_tris", "");
        }
    }
    return alt;
}
/*
 ======================================================================
 */

/* MAXVTXS_LIN is 2 per line * (3 locs + 3 nrms + 3 colrs + 
		2 uvs * max uv sets + 3 uvws * max uvw sets) */
#define MAXVTXS_LIN (18 + dod_system_max_vertex_uv_cnt*4 + \
			dod_system_max_vertex_uvw_cnt*6)

dot_object *dor_quadlst_create_alternate_geom_lines (struct quadlst *quadlst)
{
    DtReal vtxs[MAXVTXS_LIN];
    DtInt quadno;
    DtInt lvtx0, lvtx1;
    DtUInt i;
    DtUInt uv, uvw;
    DtInt vertexno;
    dot_object *linlstobj;

    linlstobj = dor_linlst_open(quadlst->colormodel, quadlst->vertextype | 
			dor_system_texture_uv_count(quadlst->uv_count) |
			dor_system_texture_uvw_count(quadlst->uvw_count),
			4*quadlst->quad_count);

    for (quadno=0; quadno<quadlst->quad_count; quadno++) {
	for (vertexno=0; vertexno<4; vertexno++){

	    lvtx0 = quadno * 4 + vertexno;
	    if (vertexno==3) 
		    lvtx1 = quadno * 4;
	    else
		    lvtx1 = quadno * 4 + vertexno+1;

	    i = 0;

	    /* One end of a line */

	    vtxs[i++] = quadlst->vertex_location[lvtx0][0];
	    vtxs[i++] = quadlst->vertex_location[lvtx0][1];
	    vtxs[i++] = quadlst->vertex_location[lvtx0][2];

	    if (quadlst->vertextype & DcLocNrm) {
		vtxs[i++] = quadlst->vertex_normal[lvtx0][0];
		vtxs[i++] = quadlst->vertex_normal[lvtx0][1];
		vtxs[i++] = quadlst->vertex_normal[lvtx0][2];
	    }

	    if (quadlst->vertextype & DcLocClr) {
		vtxs[i++] = quadlst->vertex_color[lvtx0][0];
		vtxs[i++] = quadlst->vertex_color[lvtx0][1];
		vtxs[i++] = quadlst->vertex_color[lvtx0][2];
	    }
	
	    if (quadlst->vertextype & DcLocAlpha) {
		vtxs[i++] = quadlst->vertex_alpha[lvtx0];
	    }
	
	    for (uv=0; uv<quadlst->uv_count; uv++) {
		vtxs[i++] = quadlst->uv_list[uv][lvtx0][0]; 
		vtxs[i++] = quadlst->uv_list[uv][lvtx0][1]; 
	    }
	
	    for (uvw=0; uvw<quadlst->uvw_count; uvw++) {
		vtxs[i++] = quadlst->uvw_list[uvw][lvtx0][0]; 
		vtxs[i++] = quadlst->uvw_list[uvw][lvtx0][1]; 
		vtxs[i++] = quadlst->uvw_list[uvw][lvtx0][2]; 
	    }
	
	    /* The other end of the line */
	
	    vtxs[i++] = quadlst->vertex_location[lvtx1][0]; 
	    vtxs[i++] = quadlst->vertex_location[lvtx1][1]; 
	    vtxs[i++] = quadlst->vertex_location[lvtx1][2]; 
	 
	    if (quadlst->vertextype & DcLocNrm) {
		vtxs[i++] = quadlst->vertex_normal[lvtx1][0];
		vtxs[i++] = quadlst->vertex_normal[lvtx1][1];
		vtxs[i++] = quadlst->vertex_normal[lvtx1][2];
	    } 
	 
	    if (quadlst->vertextype & DcLocClr) {
		vtxs[i++] = quadlst->vertex_color[lvtx1][0];
		vtxs[i++] = quadlst->vertex_color[lvtx1][1];
		vtxs[i++] = quadlst->vertex_color[lvtx1][2];
	    }
	
	    if (quadlst->vertextype & DcLocAlpha) {
		vtxs[i++] = quadlst->vertex_alpha[lvtx1];
	    }
	
	    for (uv=0; uv<quadlst->uv_count; uv++) {
		vtxs[i++] = quadlst->uv_list[uv][lvtx1][0]; 
		vtxs[i++] = quadlst->uv_list[uv][lvtx1][1]; 
	    }
	
	    for (uvw=0; uvw<quadlst->uvw_count; uvw++) {
		vtxs[i++] = quadlst->uvw_list[uvw][lvtx1][0]; 
		vtxs[i++] = quadlst->uvw_list[uvw][lvtx1][1]; 
		vtxs[i++] = quadlst->uvw_list[uvw][lvtx1][2]; 
	    }
	
	    /* Add the line to the list */
   		
	    dor_linlst_add_line(linlstobj,vtxs);
	
   	}

    }
    dor_linlst_close(linlstobj);
    return(linlstobj);
}
/*
 ======================================================================
 */

dot_object *dor_quadlst_create_alternate_geom_tris (struct quadlst *quadlst)
{
    DtReal *vertices, *vptr;
    DtVertexType vertextype;
    DtInt quadno, vertexno;
    DtInt index, i;
    DtInt trianglecount;
    DtInt vertexsize;
    DtUInt uv, uvw;
    dot_object *trilstobj;

    /* 
     * In quad-to-triangle conversion, the number of triangles is twice 
     * the number of quads and the number of vertices increases to be
     * quadlst->quad_count * (4 verts per quad + 2).
     */
    vertextype = quadlst->vertextype; 

    vertexsize = 3;
    if (vertextype & DcLocNrm)
        vertexsize += 3;
    if (vertextype & DcLocClr)
        vertexsize += 3;
    if (vertextype & DcLocAlpha)
        vertexsize += 3;

    vertexsize = vertexsize + quadlst->uv_count*2 + quadlst->uvw_count*3;

    trianglecount = 2 * quadlst->quad_count;

    vertices = (DtReal *) dor_space_allocate(
                            3*trianglecount*vertexsize*sizeof(DtReal));
    vptr = vertices;
    for (quadno=0; quadno<quadlst->quad_count; quadno++) {
	index = quadno * 4;

	/* first triangle */
	for (vertexno = 0; vertexno < 3; vertexno++) {
	    i = index + vertexno;
	    *vptr++ = quadlst->vertex_location[i][0];
	    *vptr++ = quadlst->vertex_location[i][1];
	    *vptr++ = quadlst->vertex_location[i][2];

	    if (vertextype & DcLocNrm) {
	        *vptr++ = quadlst->vertex_normal[i][0];
	        *vptr++ = quadlst->vertex_normal[i][1];
	        *vptr++ = quadlst->vertex_normal[i][2];
	    }
	    if (vertextype & DcLocClr) {
	        *vptr++ = quadlst->vertex_color[i][0];
	        *vptr++ = quadlst->vertex_color[i][1];
	        *vptr++ = quadlst->vertex_color[i][2];
	    }
	    if (vertextype & DcLocAlpha) {
	        *vptr++ = quadlst->vertex_alpha[i];
	    }
	    for (uv=0; uv<quadlst->uv_count; uv++) {
                *vptr++ = quadlst->uv_list[uv][i][0];
                *vptr++ = quadlst->uv_list[uv][i][1];
            }
	    for (uvw=0; uvw<quadlst->uvw_count; uvw++) {
                *vptr++ = quadlst->uvw_list[uvw][i][0];
                *vptr++ = quadlst->uvw_list[uvw][i][1];
                *vptr++ = quadlst->uvw_list[uvw][i][2];
            }
	}

	/* second triangle */
	for (vertexno = 0; vertexno < 3; vertexno++) {
	    i = index + ((vertexno+2) % 4);
	    *vptr++ = quadlst->vertex_location[i][0];
	    *vptr++ = quadlst->vertex_location[i][1];
	    *vptr++ = quadlst->vertex_location[i][2];

	    if (vertextype & DcLocNrm) {
	        *vptr++ = quadlst->vertex_normal[i][0];
	        *vptr++ = quadlst->vertex_normal[i][1];
	        *vptr++ = quadlst->vertex_normal[i][2];
	    }
	    if (vertextype & DcLocClr) {
	        *vptr++ = quadlst->vertex_color[i][0];
	        *vptr++ = quadlst->vertex_color[i][1];
	        *vptr++ = quadlst->vertex_color[i][2];
	    }
	    if (vertextype & DcLocAlpha) {
	        *vptr++ = quadlst->vertex_alpha[i];
	    }
	    for (uv=0; uv<quadlst->uv_count; uv++) {
                *vptr++ = quadlst->uv_list[uv][i][0];
                *vptr++ = quadlst->uv_list[uv][i][1];
            }
	    for (uvw=0; uvw<quadlst->uvw_count; uvw++) {
                *vptr++ = quadlst->uvw_list[uvw][i][0];
                *vptr++ = quadlst->uvw_list[uvw][i][1];
                *vptr++ = quadlst->uvw_list[uvw][i][2];
            }
	}
    }

    return(trilstobj = dor_trilst_create(quadlst->colormodel, 
		vertextype | dor_system_texture_uv_count(quadlst->uv_count) |
		dor_system_texture_uvw_count(quadlst->uvw_count),
		trianglecount, vertices));
}
/*
 ======================================================================
 */

void dor_quadlst_calculate_quad_normals (struct quadlst *quadlst)
{
    DtInt quadno;

    /* This code assumes the quad is planar and uses the first three
       vertices of the quad. */

    for (quadno=0;quadno<quadlst->quad_count;quadno++){
	dor_math_fast_cross_cosines(quadlst->vertex_location[quadno*4],
				    quadlst->vertex_location[quadno*4+1],
				    quadlst->vertex_location[quadno*4+2],
				    quadlst->element_normal[quadno]);

	dor_math_renormalize_fast(quadlst->element_normal[quadno]);
    }
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

void dor_quadlst_bounding_box (struct quadlst *quadlst)
{
    DtReal xmin, ymin, zmin, xmax, ymax, zmax;
    DtInt vertexno;
    DtInt vertexcount;

    if (quadlst->quad_count == 0) {
        xmin = 0.;
        ymin = 0.;
        zmin = 0.;

        xmax = 0.;
        ymax = 0.;
        zmax = 0.;
    }
    else{
    	xmin = quadlst->vertex_location[0][0];
    	xmax = quadlst->vertex_location[0][0];

    	ymin = quadlst->vertex_location[0][1];
    	ymax = quadlst->vertex_location[0][1];

    	zmin = quadlst->vertex_location[0][2];
    	zmax = quadlst->vertex_location[0][2];

    	vertexcount = 4 * quadlst->quad_count;

PRAGMA(IVDEP)
PRAGMA(VBEST)
	for (vertexno=1;vertexno<vertexcount;vertexno++){
	    xmin = min(xmin,quadlst->vertex_location[vertexno][0]);
	    xmax = max(xmax,quadlst->vertex_location[vertexno][0]);

	    ymin = min(ymin,quadlst->vertex_location[vertexno][1]);
	    ymax = max(ymax,quadlst->vertex_location[vertexno][1]);

	    zmin = min(zmin,quadlst->vertex_location[vertexno][2]);
	    zmax = max(zmax,quadlst->vertex_location[vertexno][2]);
	}
    }

    quadlst->bounding_box_pts[0][0] = xmin;
    quadlst->bounding_box_pts[0][1] = ymin;
    quadlst->bounding_box_pts[0][2] = zmin;

    quadlst->bounding_box_pts[1][0] = xmin;
    quadlst->bounding_box_pts[1][1] = ymax;
    quadlst->bounding_box_pts[1][2] = zmin;

    quadlst->bounding_box_pts[2][0] = xmax;
    quadlst->bounding_box_pts[2][1] = ymax;
    quadlst->bounding_box_pts[2][2] = zmin;

    quadlst->bounding_box_pts[3][0] = xmax;
    quadlst->bounding_box_pts[3][1] = ymin;
    quadlst->bounding_box_pts[3][2] = zmin;

    quadlst->bounding_box_pts[4][0] = xmin;
    quadlst->bounding_box_pts[4][1] = ymin;
    quadlst->bounding_box_pts[4][2] = zmax;

    quadlst->bounding_box_pts[5][0] = xmin;
    quadlst->bounding_box_pts[5][1] = ymax;
    quadlst->bounding_box_pts[5][2] = zmax;

    quadlst->bounding_box_pts[6][0] = xmax;
    quadlst->bounding_box_pts[6][1] = ymax;
    quadlst->bounding_box_pts[6][2] = zmax;

    quadlst->bounding_box_pts[7][0] = xmax;
    quadlst->bounding_box_pts[7][1] = ymin;
    quadlst->bounding_box_pts[7][2] = zmax;
}
/*
 ======================================================================
 */

void dor_quadlst_compute_bounding_volume (dot_object *object)
{
    struct quadlst *quadlst;

    quadlst = (struct quadlst *)(object->data);

    dor_system_report_bounding_points (8, (DtReal*)quadlst->bounding_box_pts);
}

/*
 ======================================================================
 */

void dor_quadlst_pick (dot_object *object)
{
    DtRepType reptype;

    if (!dor_condex_query_pick(doe_quadlst_class_id))
            return;

    dor_pick_initialize_object(object);

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    case DcOutlines:
        dor_quadlst_pick_lines(object);
        break;

    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
    case DcWireframe:
    case DcPoints:
        dor_quadlst_pick_tris(object);
        break;
    default:
        DDerror(ERR_VALUE_OUT_OF_RANGE,
                   "dor_quadlst_pick",
                   "unknown reptype");
    }
}
/*
 ======================================================================
 */

void dor_quadlst_pick_lines (dot_object *object)
{
    struct quadlst *quadlst;

    quadlst = (struct quadlst *)(object->data);

    if (quadlst->alt_object_lines != DcNullPtr) {
        doe_system_alternate_object_execution_flag = DcTrue;

        dor_object_get_method(quadlst->alt_object_lines,
                              DcMethodPick)(quadlst->alt_object_lines);

        doe_system_alternate_object_execution_flag = DcFalse;

        return;
    } else {
        quadlst->alt_object_lines =
                dor_quadlst_create_alternate_geom_lines(quadlst);

        if (quadlst->alt_object_lines == NULL) {
            DDerror(ERR_CANT_CREATE_ALTOBJ,
                       "dor_quadlst_pick_lines", "");
            return;
        }

        doe_system_alternate_object_execution_flag = DcTrue;

        dor_object_get_method(quadlst->alt_object_lines,
                              DcMethodPick)(quadlst->alt_object_lines);

        doe_system_alternate_object_execution_flag = DcFalse;

        return;
    }
}
/*
 ======================================================================
 */

void dor_quadlst_pick_tris (dot_object *object)
{
    struct quadlst *quadlst;

    quadlst = (struct quadlst *)(object->data);

    if (quadlst->alt_object_tris != DcNullPtr) {
        doe_system_alternate_object_execution_flag = DcTrue;

        dor_object_get_method(quadlst->alt_object_tris,
                              DcMethodPick)(quadlst->alt_object_tris);

        doe_system_alternate_object_execution_flag = DcFalse;

        return;
    } else {
        quadlst->alt_object_tris =
                dor_quadlst_create_alternate_geom_tris(quadlst);

        if (quadlst->alt_object_tris == NULL) {
            DDerror(ERR_CANT_CREATE_ALTOBJ,
                       "dor_quadlst_pick_tris", "");
            return;
        }

        doe_system_alternate_object_execution_flag = DcTrue;

        dor_object_get_method(quadlst->alt_object_tris,
                              DcMethodPick)(quadlst->alt_object_tris);

        doe_system_alternate_object_execution_flag = DcFalse;

        return;
    }
}
/*
 ======================================================================
 */

void dor_quadlst_render_standard (dot_object *object)
{
    dot_object *alt;

    if (!dor_condex_query_render(doe_quadlst_class_id))
            return;

    alt = dor_quadlst_update_alternate (object);
    if (alt != DcNullPtr) {
        dor_object_get_current_method(alt) (alt);
    }
}
