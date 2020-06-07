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
	dor_quadgrid_initialize
	dor_quadgrid_create
	dor_quadgrid_destroy
	dor_quadgrid_print
	dor_quadgrid_update_alternate
	dor_quadgrid_update_lines
	dor_quadgrid_update_tris
	dor_quadgrid_create_alternate_geom_lines
        dor_quadgrid_transfer_vtx_info
	dor_quadgrid_create_alternate_geom_tris
	dor_quadgrid_calculate_quad_normals
	dor_quadgrid_calculate_vertex_normals
	dor_quadgrid_bounding_box
	dor_quadgrid_compute_bounding_volume
	dor_quadgrid_pick
	dor_quadgrid_pick_lines
	dor_quadgrid_pick_tris
	dor_quadgrid_render_standard

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/internal/glbatt.h>
#include <dore/dore_develop/private/quadgrid.h>
#include <dore/dore_develop/private/trimsh.h>
#include <dore/dore_develop/private/linlst.h>
/*
 ======================================================================
 */

DtInt doe_quadgrid_class_id;

static DtMethodEntry quadgrid_methods[] = {
    { DcMethodCmpBndVolume, (DtMethodPtr)dor_quadgrid_compute_bounding_volume },
    { DcMethodDestroy,       (DtMethodPtr)dor_quadgrid_destroy },
    { DcMethodPick,           (DtMethodPtr)dor_quadgrid_pick },
    { DcMethodPrint,           (DtMethodPtr)dor_quadgrid_print },
    { DcMethodUpdStdAltObj,     (DtMethodPtr)dor_quadgrid_update_alternate },
    { DcMethodUpdStdAltLineObj,  (DtMethodPtr)dor_quadgrid_update_lines },
    { DcMethodStdRenderDisplay,   (DtMethodPtr)dor_quadgrid_render_standard }
};

void dor_quadgrid_initialize (void)
{
    doe_quadgrid_class_id = dor_class_add
				("DoQuadGrid", 7, quadgrid_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_quadgrid_create (
    DtColorModel colormodel,
    DtVertexType vertextype,
    DtInt width,
    DtInt height,
    DtReal vertices[3],
    DtFlag smoothflag)
{
   static DtFlag initialized = DcFalse;
   struct quadgrid *quadgrid;
   DtInt vertexno;
   DtInt vertexcount, quadcount;
   DtReal *vertexptr;
   dot_object *quadgridobject;
   DtVertexType basevtxtype;
   DtInt uv;
   DtInt uvw;

   if (!initialized) {
      	dor_quadgrid_initialize();
   	initialized = DcTrue;
   }

   quadgrid = (struct quadgrid *)dor_space_allocate(sizeof *quadgrid);

   quadgrid->alt_object_lines = (DtObjectStructure *)0;
   quadgrid->alt_object_tris = (DtObjectStructure *)0;

   quadgrid->colormodel = colormodel;

   basevtxtype = dod_system_inq_vertex_basetype(vertextype);
   quadgrid->vertextype = basevtxtype;

   quadgrid->width = width;
   quadgrid->height = height;

   vertexcount = width * height;
   
   quadgrid->vertex_location = (DtRealTriple *)dor_space_allocate(
			   vertexcount*(sizeof (DtRealTriple)));

   quadgrid->vertex_normal = (DtRealTriple *)NULL;

   quadgrid->vertex_color = (DtColorRGB *)NULL;

   quadgrid->vertex_alpha = (DtReal *)NULL;

   /* texture coordinates */

   quadgrid->uv_list = (DtRealCouple **)dor_space_allocate(
		dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
   quadgrid->uvw_list = (DtRealTriple **)dor_space_allocate(
		dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *));

   for (uv = 0; uv < dod_system_max_vertex_uv_cnt; uv++) {
   	quadgrid->uv_list[uv] = NULL;
   }
   for (uvw = 0; uvw < dod_system_max_vertex_uvw_cnt; uvw++) {
   	quadgrid->uvw_list[uvw] = NULL;
   }

   quadgrid->uv_count = dod_system_inq_vertex_uv_cnt(vertextype);
   quadgrid->uvw_count = dod_system_inq_vertex_uvw_cnt(vertextype);

   for (uv=0; uv< quadgrid->uv_count; uv++) {
   	quadgrid->uv_list[uv] = (DtRealCouple *)dor_space_allocate(
				vertexcount*(sizeof (DtRealCouple)));
   }

   for (uvw=0; uvw< quadgrid->uvw_count; uvw++) {
   	quadgrid->uvw_list[uvw] = (DtRealTriple *)dor_space_allocate(
				vertexcount*(sizeof (DtRealTriple)));
   }

   /**/

   vertexptr = vertices;

    if (basevtxtype & DcLocNrm)
        quadgrid->vertex_normal = (DtRealTriple *)
            dor_space_allocate(vertexcount*(sizeof (DtRealTriple)));

    if (basevtxtype & DcLocClr)
        quadgrid->vertex_color = (DtColorRGB *)
            dor_space_allocate(vertexcount*(sizeof (DtColorRGB)));

    if (basevtxtype & DcLocAlpha)
        quadgrid->vertex_alpha = (DtReal *)
            dor_space_allocate(vertexcount*(sizeof (DtReal)));

    for (vertexno=0;vertexno<vertexcount;vertexno++) {
        quadgrid->vertex_location[vertexno][0] = *vertexptr++;
        quadgrid->vertex_location[vertexno][1] = *vertexptr++;
        quadgrid->vertex_location[vertexno][2] = *vertexptr++;

        if (basevtxtype & DcLocNrm) {
            quadgrid->vertex_normal[vertexno][0] = *vertexptr++;
            quadgrid->vertex_normal[vertexno][1] = *vertexptr++;
            quadgrid->vertex_normal[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocClr) {
            quadgrid->vertex_color[vertexno][0] = *vertexptr++;
            quadgrid->vertex_color[vertexno][1] = *vertexptr++;
            quadgrid->vertex_color[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocAlpha) {
            quadgrid->vertex_alpha[vertexno] = *vertexptr++;
        }

        for (uv=0; uv < quadgrid->uv_count; uv++) {
            quadgrid->uv_list[uv][vertexno][0] = *vertexptr++;
            quadgrid->uv_list[uv][vertexno][1] = *vertexptr++;
        }

        for (uvw=0; uvw < quadgrid->uvw_count; uvw++) {
            quadgrid->uvw_list[uvw][vertexno][0] = *vertexptr++;
            quadgrid->uvw_list[uvw][vertexno][1] = *vertexptr++;
            quadgrid->uvw_list[uvw][vertexno][2] = *vertexptr++;
        }
    }

   /*  Since we will use the element normals to compute the vertex
  	 normals, compute the element normals first.  */

   quadcount = (width-1) * (height-1);
   quadgrid->element_normal = (DtRealTriple *)dor_space_allocate(
			      quadcount*(sizeof (DtRealTriple)));

   dor_quadgrid_calculate_quad_normals(quadgrid);

   if (smoothflag) {

	if (!(basevtxtype & DcLocNrm)) {

		quadgrid->vertex_normal = 
			(DtRealTriple *)dor_space_allocate(
				vertexcount*(sizeof (DtRealTriple)));

		dor_quadgrid_calculate_vertex_normals(quadgrid);
	}
   }

   quadgrid->smoothflag = smoothflag;

   dor_quadgrid_bounding_box(quadgrid);
   
   quadgridobject = dor_object_create(doe_quadgrid_class_id,quadgrid);

   return(quadgridobject);
}
/*
 ======================================================================
 */

void dor_quadgrid_destroy (dot_object *object)
{
    struct quadgrid *quadgrid;
    int i;

    quadgrid = (struct quadgrid *)(object->data);

    if (quadgrid->vertex_location != NULL) {
	dor_space_deallocate(quadgrid->vertex_location);
    }

    if (quadgrid->vertex_normal != NULL) {
	dor_space_deallocate(quadgrid->vertex_normal);
    }

    if (quadgrid->vertex_color != NULL) {
	dor_space_deallocate(quadgrid->vertex_color);
    }

    if (quadgrid->vertex_alpha != NULL) {
	dor_space_deallocate(quadgrid->vertex_alpha);
    }

    if (quadgrid->element_normal != NULL) {
	dor_space_deallocate(quadgrid->element_normal);
    }

    for (i=0; i< quadgrid->uv_count; i++) {
	dor_space_deallocate(quadgrid->uv_list[i]);
    }
    dor_space_deallocate(quadgrid->uv_list);

    for (i=0; i< quadgrid->uvw_count; i++) {
	dor_space_deallocate(quadgrid->uvw_list[i]);
    }
    dor_space_deallocate(quadgrid->uvw_list);

    dor_space_deallocate(quadgrid);
}
/*
 ======================================================================
 */

void dor_quadgrid_print (dot_object *object)
{
    struct quadgrid *quadgrid;

    quadgrid = (struct quadgrid *)(object->data);

    if (quadgrid == NULL){
	dor_print_output("quad grid is NULL");
	return;
    }
 
    switch (quadgrid->colormodel){
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;
 
    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    dor_utilpri_print_vertextype(quadgrid->vertextype);

    sprintf(dor_print_get_line(),"smooth flag = %d",quadgrid->smoothflag);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"grid width = %d",quadgrid->width);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"grid height = %d",quadgrid->height);
    dor_print_output(dor_print_get_line());

    if (quadgrid->uv_count > 0) {
	sprintf (dor_print_get_line(), "uv sets per vertex = %d", quadgrid->uv_count);
	dor_print_output(dor_print_get_line());
    }

    if (quadgrid->uvw_count > 0) {
	sprintf (dor_print_get_line(), "uvw sets per vertex = %d", quadgrid->uvw_count);
	dor_print_output(dor_print_get_line());
    }
}
/*
 ======================================================================
 */
dot_object *dor_quadgrid_update_alternate (dot_object *object)
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
            alt = dor_quadgrid_update_tris(object);
            break;

    	case DcOutlines:
            alt = dor_quadgrid_update_lines(object);
            break;
    	default:
            DDerror(ERR_VALUE_OUT_OF_RANGE,
                   "dor_quadgrid_update_alternate",
                   "unknown reptype");
            alt = DcNullPtr;
            break;
    }
    return alt;
}
/*
 ======================================================================
 */
dot_object *dor_quadgrid_update_lines (dot_object *object)
{
    struct quadgrid *quadgrid;
    dot_object *alt;

    quadgrid = (struct quadgrid *)(object->data);

    if (quadgrid->alt_object_lines != DcNullPtr) {
	alt = quadgrid->alt_object_lines;

    } else {
	quadgrid->alt_object_lines = 
		dor_quadgrid_create_alternate_geom_lines(quadgrid);
	alt = quadgrid->alt_object_lines;

	if (quadgrid->alt_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_quadgrid_update_lines", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */
dot_object *dor_quadgrid_update_tris (dot_object *object)
{
    struct quadgrid *quadgrid;
    dot_object *alt;

    quadgrid = (struct quadgrid *)(object->data);

    if (quadgrid->alt_object_tris != DcNullPtr) {
        alt = quadgrid->alt_object_tris;

    } else {
        quadgrid->alt_object_tris =
                dor_quadgrid_create_alternate_geom_tris(quadgrid);
        alt = quadgrid->alt_object_tris;

        if (quadgrid->alt_object_tris == DcNullPtr) {
            DDerror(ERR_CANT_CREATE_ALTOBJ,
                       "dor_quadgrid_update_tris", "");
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

dot_object *dor_quadgrid_create_alternate_geom_lines (struct quadgrid *quadgrid)
{
    DtReal vtxs[MAXVTXS_LIN];
    DtVertexType vertextype;
    DtUInt i, j;
    DtInt offset;
    DtInt vertexno, quadcount;
    dot_object *linlstobj;

    vertextype = quadgrid->vertextype;
    if (quadgrid->smoothflag) {
        /* add normals to vertextype */
        vertextype |= DcLocNrm;
    }

    quadcount = (quadgrid->width-1)*(quadgrid->height-1);

    linlstobj = dor_linlst_open(quadgrid->colormodel, vertextype | 
			dor_system_texture_uv_count(quadgrid->uv_count) |
			dor_system_texture_uvw_count(quadgrid->uvw_count),
			4*quadcount);

    /* compute how many reals it takes to store all vertex info */

    offset = 3;
    if (vertextype & DcLocNrm)
        offset += 3;
    if (vertextype & DcLocClr)
        offset += 3;
    if (vertextype & DcLocAlpha)
        offset += 3;

    offset += quadgrid->uv_count*2;
    offset += quadgrid->uvw_count*3;

    /* This loop draws two lines for each vertex except for the "rightmost"
       and "bottom" ones. For instance, for a 4 x 3 grid, after this
       loop we have:

          0    1    2    3
        0 x----x----x----x
          |    |    |    
          |    |    |    
        1 x----x----x----x
          |    |    |    
          |    |    |    
        2 x    x    x    x
 
    */

    for (j=0; j<quadgrid->height-1; j++) {
        for (i=0; i<quadgrid->width-1; i++) {

            vertexno = j*quadgrid->width+i;
	    /* Target vertex: first end of both lines */
            dor_quadgrid_transfer_vtx_info(vtxs, 0, quadgrid, vertexno);

	    /* other end of first line (going "right") */
            dor_quadgrid_transfer_vtx_info(vtxs, offset, quadgrid, vertexno+1);

            /* Add first line to the list */	
	    dor_linlst_add_line(linlstobj,vtxs);

	    /* other end of second line (going "down") */
            dor_quadgrid_transfer_vtx_info(vtxs, offset, quadgrid,
                                           vertexno+quadgrid->width);
	    /* Add second line to the list */
	    dor_linlst_add_line(linlstobj,vtxs);
	
   	}
    }

    /* now draw the bottom line segments */

    vertexno = quadgrid->width * (quadgrid->height - 1);
    for (i=0; i<quadgrid->width-1; i++) {

        dor_quadgrid_transfer_vtx_info(vtxs, 0, quadgrid, vertexno+i);
        dor_quadgrid_transfer_vtx_info(vtxs, offset, quadgrid, vertexno+i+1);

        /* Add line to the list */	
	dor_linlst_add_line(linlstobj,vtxs);
    }

    /* now draw the rightmost line segments */

    for (j=0; j<quadgrid->height-1; j++) {

        vertexno = (j+1) * quadgrid->width - 1;
        dor_quadgrid_transfer_vtx_info(vtxs, 0, quadgrid, vertexno);
        dor_quadgrid_transfer_vtx_info(vtxs, offset, quadgrid,
                                       vertexno+quadgrid->width);
        /* Add line to the list */	
	dor_linlst_add_line(linlstobj,vtxs);
    }

    dor_linlst_close(linlstobj);
    return(linlstobj);
}
/*
 ======================================================================
 */

/* copy information of one quadgrid vertex into vtxs array */

void dor_quadgrid_transfer_vtx_info (
    DtReal vtxs[],
    DtInt start,
    struct quadgrid *quadgrid,
    DtInt vertex_index)
{
    DtInt k, uv, uvw;

    k = start;
    vtxs[k++] = quadgrid->vertex_location[vertex_index][0];
    vtxs[k++] = quadgrid->vertex_location[vertex_index][1];
    vtxs[k++] = quadgrid->vertex_location[vertex_index][2];

    if ((quadgrid->vertextype & DcLocNrm) || quadgrid->smoothflag) {
        vtxs[k++] = quadgrid->vertex_normal[vertex_index][0];
        vtxs[k++] = quadgrid->vertex_normal[vertex_index][1];
        vtxs[k++] = quadgrid->vertex_normal[vertex_index][2];
    }

    if (quadgrid->vertextype & DcLocClr) {
        vtxs[k++] = quadgrid->vertex_color[vertex_index][0];
        vtxs[k++] = quadgrid->vertex_color[vertex_index][1];
        vtxs[k++] = quadgrid->vertex_color[vertex_index][2];
    }

    if (quadgrid->vertextype & DcLocAlpha) {
        vtxs[k++] = quadgrid->vertex_alpha[vertex_index];
    }

    for (uv=0; uv<quadgrid->uv_count; uv++) {
        vtxs[k++] = quadgrid->uv_list[uv][vertex_index][0];
        vtxs[k++] = quadgrid->uv_list[uv][vertex_index][1];
    }

    for (uvw=0; uvw<quadgrid->uvw_count; uvw++) {
        vtxs[k++] = quadgrid->uvw_list[uvw][vertex_index][0];
        vtxs[k++] = quadgrid->uvw_list[uvw][vertex_index][1];
        vtxs[k++] = quadgrid->uvw_list[uvw][vertex_index][2];
    }
}

/*
 ======================================================================
 */

dot_object *dor_quadgrid_create_alternate_geom_tris (struct quadgrid *quadgrid)
{
    DtReal *vertices, *vptr;
    DtVertexType vertextype;
    DtInt vertexno;
    DtInt trianglecount;
    DtInt vertexcount, vertexsize;
    DtUInt uv, uvw, i, j;
    dot_object *trimshobj;
    DtIntTriple *trivtxlst, *vconnectptr;

    /* 
     * In quad-to-triangle conversion, the vertices don't change but
     * the number of triangles is twice the number of quads.
     */
    vertextype = quadgrid->vertextype;
    if (quadgrid->smoothflag) {
       /* add normals to vertextype */
       vertextype |= DcLocNrm;
    }

    vertexsize = 3;
    if (vertextype & DcLocNrm)
        vertexsize += 3;
    if (vertextype & DcLocClr)
        vertexsize += 3;
    if (vertextype & DcLocAlpha)
        vertexsize += 3;

    vertexsize = vertexsize + quadgrid->uv_count*2 + quadgrid->uvw_count*3;

    trianglecount = 2 * (quadgrid->width-1)*(quadgrid->height-1);

    trivtxlst = (DtIntTriple *)dor_space_allocate(
		trianglecount*(sizeof (DtIntTriple)));
    vconnectptr = trivtxlst;

    for (j=0; j<quadgrid->height-1; j++) {
        for (i=0; i<quadgrid->width-1; i++) {
            
            vertexno = j*quadgrid->width + i;
            (*vconnectptr)[0] = vertexno;
	    (*vconnectptr)[1] = vertexno + 1;
	    (*vconnectptr)[2] = vertexno + 1 + quadgrid->width;
	    vconnectptr++;

	    (*vconnectptr)[0] = vertexno;
	    (*vconnectptr)[1] = vertexno + 1 + quadgrid->width;
	    (*vconnectptr)[2] = vertexno + quadgrid->width;
	    vconnectptr++;
        }
    }

    vertexcount = quadgrid->width*quadgrid->height;
    vertices = (DtReal *)dor_space_allocate(
	       vertexcount*vertexsize*(sizeof (DtReal)));

    vptr = vertices;
    for (vertexno=0; vertexno<vertexcount; vertexno++) {
	*vptr++ = quadgrid->vertex_location[vertexno][0];
	*vptr++ = quadgrid->vertex_location[vertexno][1];
	*vptr++ = quadgrid->vertex_location[vertexno][2];

	if (vertextype & DcLocNrm) {
	    *vptr++ = quadgrid->vertex_normal[vertexno][0];
	    *vptr++ = quadgrid->vertex_normal[vertexno][1];
	    *vptr++ = quadgrid->vertex_normal[vertexno][2];
	}
	if (vertextype & DcLocClr) {
	    *vptr++ = quadgrid->vertex_color[vertexno][0];
	    *vptr++ = quadgrid->vertex_color[vertexno][1];
	    *vptr++ = quadgrid->vertex_color[vertexno][2];
	}
	if (vertextype & DcLocAlpha) {
	    *vptr++ = quadgrid->vertex_alpha[vertexno];
	}
	for (uv=0; uv<quadgrid->uv_count; uv++) {
            *vptr++ = quadgrid->uv_list[uv][vertexno][0];
            *vptr++ = quadgrid->uv_list[uv][vertexno][1];
        }
	for (uvw=0; uvw<quadgrid->uvw_count; uvw++) {
            *vptr++ = quadgrid->uvw_list[uvw][vertexno][0];
            *vptr++ = quadgrid->uvw_list[uvw][vertexno][1];
            *vptr++ = quadgrid->uvw_list[uvw][vertexno][2];
        }
    }

    return(trimshobj = dor_trimsh_create(quadgrid->colormodel, 
    		  vertextype | dor_system_texture_uv_count(quadgrid->uv_count) |
		  dor_system_texture_uvw_count(quadgrid->uvw_count) ,
			vertexcount, vertices,
			trianglecount, trivtxlst, quadgrid->smoothflag));
}
/*
 ======================================================================
 */

void dor_quadgrid_calculate_quad_normals (struct quadgrid *quadgrid)
{
    DtInt quadno;
    DtInt i, j;
    DtInt vertex0, vertex1, vertex2;

    /* This code assumes the quad is planar and uses three
       of the vertices of the quad to calculate the normal */

    quadno = 0;
    for (j=0;j<quadgrid->height-1;j++){
        for (i=0;i<quadgrid->width-1;i++){

	    vertex0 = j*quadgrid->width + i + 1;
            vertex1 = vertex0 - 1;
            vertex2 = vertex0 + quadgrid->width;

	    dor_math_fast_cross_cosines(
                  quadgrid->vertex_location[vertex0],
		  quadgrid->vertex_location[vertex1],
		  quadgrid->vertex_location[vertex2],
		  quadgrid->element_normal[quadno]);

	    dor_math_renormalize_fast(quadgrid->element_normal[quadno]);

            quadno++;
        }
    }
}
/*
 ======================================================================
 */

void dor_quadgrid_calculate_vertex_normals (struct quadgrid *quadgrid)
{
    DtInt vertexno, vno[4];
    DtInt quadno, vertexcount;
    DtInt i, j, k;

    vertexcount = quadgrid->width*quadgrid->height;

PRAGMA(ASIS)
    for (vertexno=0;vertexno<vertexcount;vertexno++) {
   	quadgrid->vertex_normal[vertexno][0] = 0.;
   	quadgrid->vertex_normal[vertexno][1] = 0.;
   	quadgrid->vertex_normal[vertexno][2] = 0.;
    }

    /*  For each quads' four vertices, accumulate quad normal. */

PRAGMA(ASIS)
    quadno = 0;
    for (j=0;j<quadgrid->height-1;j++) {
        for (i=0;i<quadgrid->width-1;i++) {
            vno[0] = j*quadgrid->width + i;  /* upperleft corner of quad */
            vno[1] = vno[0] + 1;
            vno[2] = vno[0] + quadgrid->width;
            vno[3] = vno[2] + 1;
            for (k=0; k<4; k++) {
	        quadgrid->vertex_normal[vno[k]][0] +=
		                quadgrid->element_normal[quadno][0];
	        quadgrid->vertex_normal[vno[k]][1] +=
		                quadgrid->element_normal[quadno][1];
	        quadgrid->vertex_normal[vno[k]][2] +=
		                quadgrid->element_normal[quadno][2];
            }
        quadno++;
        }
    }

    /*  Rescale the vertex normals.  */

PRAGMA(ASIS)
    for (vertexno=0;vertexno<vertexcount;vertexno++) {
	if (!dor_math_renormalize_vector(quadgrid->vertex_normal[vertexno])) {
	    DDerror (ERR_ZERO_QUAD_NRM_SUM,
		    "dor_quadgrid_calculate_vertex_normals", 
		    DcNullPtr);
        }
    }

}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

void dor_quadgrid_bounding_box (struct quadgrid *quadgrid)
{
    DtReal xmin, ymin, zmin, xmax, ymax, zmax;
    DtInt vertexno;
    DtInt vertexcount;

    if (quadgrid->width == 0 || quadgrid->height == 0) {
        xmin = 0.;
        ymin = 0.;
        zmin = 0.;

        xmax = 0.;
        ymax = 0.;
        zmax = 0.;
    } else {

        xmin = quadgrid->vertex_location[0][0];
        xmax = quadgrid->vertex_location[0][0];

        ymin = quadgrid->vertex_location[0][1];
        ymax = quadgrid->vertex_location[0][1];

        zmin = quadgrid->vertex_location[0][2];
        zmax = quadgrid->vertex_location[0][2];

PRAGMA(IVDEP)
PRAGMA(VBEST)

        vertexcount = quadgrid->width * quadgrid->height;

        for (vertexno=0;vertexno<vertexcount;vertexno++){

            xmin = min(xmin,quadgrid->vertex_location[vertexno][0]);
            xmax = max(xmax,quadgrid->vertex_location[vertexno][0]);

            ymin = min(ymin,quadgrid->vertex_location[vertexno][1]);
            ymax = max(ymax,quadgrid->vertex_location[vertexno][1]);

            zmin = min(zmin,quadgrid->vertex_location[vertexno][2]);
            zmax = max(zmax,quadgrid->vertex_location[vertexno][2]);
        }
    }

    quadgrid->bounding_box_pts[0][0] = xmin;
    quadgrid->bounding_box_pts[0][1] = ymin;
    quadgrid->bounding_box_pts[0][2] = zmin;

    quadgrid->bounding_box_pts[1][0] = xmin;
    quadgrid->bounding_box_pts[1][1] = ymax;
    quadgrid->bounding_box_pts[1][2] = zmin;

    quadgrid->bounding_box_pts[2][0] = xmax;
    quadgrid->bounding_box_pts[2][1] = ymax;
    quadgrid->bounding_box_pts[2][2] = zmin;

    quadgrid->bounding_box_pts[3][0] = xmax;
    quadgrid->bounding_box_pts[3][1] = ymin;
    quadgrid->bounding_box_pts[3][2] = zmin;

    quadgrid->bounding_box_pts[4][0] = xmin;
    quadgrid->bounding_box_pts[4][1] = ymin;
    quadgrid->bounding_box_pts[4][2] = zmax;

    quadgrid->bounding_box_pts[5][0] = xmin;
    quadgrid->bounding_box_pts[5][1] = ymax;
    quadgrid->bounding_box_pts[5][2] = zmax;

    quadgrid->bounding_box_pts[6][0] = xmax;
    quadgrid->bounding_box_pts[6][1] = ymax;
    quadgrid->bounding_box_pts[6][2] = zmax;

    quadgrid->bounding_box_pts[7][0] = xmax;
    quadgrid->bounding_box_pts[7][1] = ymin;
    quadgrid->bounding_box_pts[7][2] = zmax;
}
/*
 ======================================================================
 */

void dor_quadgrid_compute_bounding_volume (dot_object *object)
{
    struct quadgrid *quadgrid;

    quadgrid = (struct quadgrid *)(object->data);

    dor_system_report_bounding_points (8, (DtReal*)quadgrid->bounding_box_pts);
}

/*
 ======================================================================
 */

void dor_quadgrid_pick (dot_object *object)
{
    DtRepType reptype;

    if (!dor_condex_query_pick(doe_quadgrid_class_id))
            return;

    dor_pick_initialize_object(object);

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    case DcOutlines:
        dor_quadgrid_pick_lines(object);
        break;

    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
    case DcWireframe:
    case DcPoints:
        dor_quadgrid_pick_tris(object);
        break;
    default:
        DDerror(ERR_VALUE_OUT_OF_RANGE,
                   "dor_quadgrid_pick",
                   "unknown reptype");
    }
}
/*
 ======================================================================
 */

void dor_quadgrid_pick_lines (dot_object *object)
{
    struct quadgrid *quadgrid;

    quadgrid = (struct quadgrid *)(object->data);

    if (quadgrid->alt_object_lines != DcNullPtr) {
        doe_system_alternate_object_execution_flag = DcTrue;

        dor_object_get_method(quadgrid->alt_object_lines,
                              DcMethodPick)(quadgrid->alt_object_lines);

        doe_system_alternate_object_execution_flag = DcFalse;

        return;
    } else {
        quadgrid->alt_object_lines =
                dor_quadgrid_create_alternate_geom_lines(quadgrid);

        if (quadgrid->alt_object_lines == NULL) {
            DDerror(ERR_CANT_CREATE_ALTOBJ,
                       "dor_quadgrid_pick_lines", "");
            return;
        }

        doe_system_alternate_object_execution_flag = DcTrue;

        dor_object_get_method(quadgrid->alt_object_lines,
                              DcMethodPick)(quadgrid->alt_object_lines);

        doe_system_alternate_object_execution_flag = DcFalse;

        return;
    }
}
/*
 ======================================================================
 */

void dor_quadgrid_pick_tris (dot_object *object)
{
    struct quadgrid *quadgrid;

    quadgrid = (struct quadgrid *)(object->data);

    if (quadgrid->alt_object_tris != DcNullPtr) {
        doe_system_alternate_object_execution_flag = DcTrue;

        dor_object_get_method(quadgrid->alt_object_tris,
                              DcMethodPick)(quadgrid->alt_object_tris);

        doe_system_alternate_object_execution_flag = DcFalse;

        return;
    } else {
        quadgrid->alt_object_tris =
                dor_quadgrid_create_alternate_geom_tris(quadgrid);

        if (quadgrid->alt_object_tris == NULL) {
            DDerror(ERR_CANT_CREATE_ALTOBJ,
                       "dor_quadgrid_pick_tris", "");
            return;
        }

        doe_system_alternate_object_execution_flag = DcTrue;

        dor_object_get_method(quadgrid->alt_object_tris,
                              DcMethodPick)(quadgrid->alt_object_tris);

        doe_system_alternate_object_execution_flag = DcFalse;

        return;
    }
}
/*
 ======================================================================
 */

void dor_quadgrid_render_standard (dot_object *object)
{
    dot_object *alt;

    if (!dor_condex_query_render(doe_quadgrid_class_id))
            return;

    alt = dor_quadgrid_update_alternate (object);
    if (alt != DcNullPtr) {
        dor_object_get_current_method(alt) (alt);
    }
}
