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
	dor_quadmsh_initialize
	dor_quadmsh_create
	dor_quadmsh_destroy
	dor_quadmsh_print
	dor_quadmsh_update_alternate
	dor_quadmsh_update_lines
	dor_quadmsh_update_tris
	dor_quadmsh_create_alternate_geom_lines
	dor_quadmsh_create_alternate_geom_tris
	dor_quadmsh_calculate_quad_normals
	dor_quadmsh_calculate_vertex_normals
	dor_quadmsh_bounding_box
	dor_quadmsh_compute_bounding_volume
	dor_quadmsh_pick
	dor_quadmsh_pick_lines
	dor_quadmsh_pick_tris
	dor_quadmsh_render_standard

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/quadmsh.h>
#include <dore/dore_develop/private/linlst.h>
#include <dore/dore_develop/private/trimsh.h>
/*
 ======================================================================
 */

DtInt doe_quadmsh_class_id;

static DtMethodEntry quadmsh_methods[] = {
    {DcMethodCmpBndVolume,    (DtMethodPtr)dor_quadmsh_compute_bounding_volume},
    {DcMethodDestroy,         (DtMethodPtr)dor_quadmsh_destroy},
    {DcMethodPick,            (DtMethodPtr)dor_quadmsh_pick},
    {DcMethodPrint,           (DtMethodPtr)dor_quadmsh_print},
    {DcMethodUpdStdAltObj,    (DtMethodPtr)dor_quadmsh_update_alternate},
    {DcMethodUpdStdAltLineObj,(DtMethodPtr)dor_quadmsh_update_lines},
    {DcMethodStdRenderDisplay,(DtMethodPtr)dor_quadmsh_render_standard}
};

void dor_quadmsh_initialize (void)
{
    doe_quadmsh_class_id = dor_class_add ("DoQuadMesh", 7, 
		quadmsh_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_quadmsh_create (
    DtColorModel colormodel,
    DtVertexType vertextype,
    DtInt vertexcount,
    DtReal vertices[3],
    DtInt quadcount,
    DtInt quadvtxlist[][4],
    DtFlag smoothflag)
{
   static DtFlag initialized = DcFalse;
   struct quadmsh *quadmsh;
   DtInt vertexno;
   DtReal *vertexptr;
   quadmsh_vertexlist *quadmshvtxlstptr, *quadvtxlstptr;
   DtInt vertex1, vertex2, vertex3, vertex4;
   DtInt quadno;
   dot_object *quadmshobject;
   DtVertexType basevtxtype;
   DtInt uv;
   DtInt uvw;

   if (!initialized) {
      	dor_quadmsh_initialize();
   	initialized = DcTrue;
   }

   quadmsh = (struct quadmsh *)dor_space_allocate(sizeof *quadmsh);

   quadmsh->alt_object_lines = (DtObjectStructure *)0;
   quadmsh->alt_object_tris = (DtObjectStructure *)0;

   quadmsh->colormodel = colormodel;

   basevtxtype = dod_system_inq_vertex_basetype(vertextype);
   quadmsh->vertextype = basevtxtype;

   quadmsh->vertex_count = vertexcount;
   
   quadmsh->vertex_location = (DtRealTriple *)dor_space_allocate(
			   vertexcount*(sizeof (DtRealTriple)));

   quadmsh->vertex_normal = (DtRealTriple *)NULL;

   quadmsh->vertex_color = (DtColorRGB *)NULL;

   quadmsh->vertex_alpha = (DtReal *)NULL;

   /* texture coordinates */

   quadmsh->uv_list = (DtRealCouple **)dor_space_allocate(
		dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
   quadmsh->uvw_list = (DtRealTriple **)dor_space_allocate(
		dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *));

   for (uv = 0; uv < dod_system_max_vertex_uv_cnt; uv++) {
   	quadmsh->uv_list[uv] = NULL;
   }
   for (uvw = 0; uvw < dod_system_max_vertex_uvw_cnt; uvw++) {
   	quadmsh->uvw_list[uvw] = NULL;
   }

   quadmsh->uv_count = dod_system_inq_vertex_uv_cnt(vertextype);
   quadmsh->uvw_count = dod_system_inq_vertex_uvw_cnt(vertextype);

   for (uv=0; uv< quadmsh->uv_count; uv++) {
   	quadmsh->uv_list[uv] = (DtRealCouple *)dor_space_allocate(
				vertexcount*(sizeof (DtRealCouple)));
   }

   for (uvw=0; uvw< quadmsh->uvw_count; uvw++) {
   	quadmsh->uvw_list[uvw] = (DtRealTriple *)dor_space_allocate(
				vertexcount*(sizeof (DtRealTriple)));
   }

   /**/

   vertexptr = vertices;

    if (basevtxtype & DcLocNrm)
        quadmsh->vertex_normal = (DtRealTriple *)
            dor_space_allocate(vertexcount*(sizeof (DtRealTriple)));

    if (basevtxtype & DcLocClr)
        quadmsh->vertex_color = (DtColorRGB *)
            dor_space_allocate(vertexcount*(sizeof (DtColorRGB)));

    if (basevtxtype & DcLocAlpha)
        quadmsh->vertex_alpha = (DtReal *)
            dor_space_allocate(vertexcount*(sizeof (DtReal)));

    for (vertexno=0;vertexno<vertexcount;vertexno++) {
        quadmsh->vertex_location[vertexno][0] = *vertexptr++;
        quadmsh->vertex_location[vertexno][1] = *vertexptr++;
        quadmsh->vertex_location[vertexno][2] = *vertexptr++;

        if (basevtxtype & DcLocNrm) {
            quadmsh->vertex_normal[vertexno][0] = *vertexptr++;
            quadmsh->vertex_normal[vertexno][1] = *vertexptr++;
            quadmsh->vertex_normal[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocClr) {
            quadmsh->vertex_color[vertexno][0] = *vertexptr++;
            quadmsh->vertex_color[vertexno][1] = *vertexptr++;
            quadmsh->vertex_color[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocAlpha) {
            quadmsh->vertex_alpha[vertexno] = *vertexptr++;
        }

        for (uv=0; uv < quadmsh->uv_count; uv++) {
            quadmsh->uv_list[uv][vertexno][0] = *vertexptr++;
            quadmsh->uv_list[uv][vertexno][1] = *vertexptr++;
        }

        for (uvw=0; uvw < quadmsh->uvw_count; uvw++) {
            quadmsh->uvw_list[uvw][vertexno][0] = *vertexptr++;
            quadmsh->uvw_list[uvw][vertexno][1] = *vertexptr++;
            quadmsh->uvw_list[uvw][vertexno][2] = *vertexptr++;
        }
    }

   quadmsh->quad_vertexlist = (quadmsh_vertexlist *)dor_space_allocate(
			quadcount*(sizeof (quadmsh_vertexlist)));

   quadvtxlstptr = quadvtxlist;
   quadmshvtxlstptr = quadmsh->quad_vertexlist;
   
   quadmsh->quad_count = 0;

   for (quadno=0;quadno<quadcount;quadno++) {
	vertex1 = (*quadvtxlstptr)[0];
	vertex2 = (*quadvtxlstptr)[1];
	vertex3 = (*quadvtxlstptr)[2];
	vertex4 = (*quadvtxlstptr)[3];

	quadvtxlstptr++;

	if ((vertex1 < 0) || (vertex1 >= vertexcount) ||
	    (vertex2 < 0) || (vertex2 >= vertexcount) ||
	    (vertex3 < 0) || (vertex3 >= vertexcount) ||
	    (vertex4 < 0) || (vertex4 >= vertexcount)) {
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "dor_quadmsh_create", 
			    "vertex index > vertexcount");
		continue;
	}

	/*  Add the quadrilateral to the primitive's list if it is not
	degenerate.  A degenerative quad is a point or a line.  */

	if (dor_quadmsh_check_degen_quad_vtx_values(
		&(quadmsh->vertex_location[vertex1][0]),
		&(quadmsh->vertex_location[vertex2][0]),
		&(quadmsh->vertex_location[vertex3][0]),
		&(quadmsh->vertex_location[vertex4][0]))) {

		(*quadmshvtxlstptr)[0] = vertex1;
		(*quadmshvtxlstptr)[1] = vertex2;
		(*quadmshvtxlstptr)[2] = vertex3;
		(*quadmshvtxlstptr)[3] = vertex4;

		quadmsh->quad_count++;

		quadmshvtxlstptr++;
	}
   }

   /*  Since we will use the element normals to compute the vertex
  	 normals, compute the element normals first.  */

   quadmsh->element_normal = (DtRealTriple *)dor_space_allocate(
			quadcount*(sizeof (DtRealTriple)));

   dor_quadmsh_calculate_quad_normals(quadmsh);

   if (smoothflag) {

	if (!(basevtxtype & DcLocNrm)) {

		quadmsh->vertex_normal = 
			(DtRealTriple *)dor_space_allocate(
				vertexcount*(sizeof (DtRealTriple)));

		dor_quadmsh_calculate_vertex_normals(quadmsh);
	}
   }

   quadmsh->smoothflag = smoothflag;

   dor_quadmsh_bounding_box(quadmsh);
   
   quadmsh->compiled_quad_vertexlist = NULL;

   quadmshobject = dor_object_create(doe_quadmsh_class_id,quadmsh);

   return(quadmshobject);
}
/*
 ======================================================================
 */

void dor_quadmsh_destroy (dot_object *object)
{
    struct quadmsh *quadmsh;
    int i;

    quadmsh = (struct quadmsh *)(object->data);

    if (quadmsh->vertex_location != NULL) {
	dor_space_deallocate(quadmsh->vertex_location);
    }

    if (quadmsh->vertex_normal != NULL) {
	dor_space_deallocate(quadmsh->vertex_normal);
    }

    if (quadmsh->vertex_color != NULL) {
	dor_space_deallocate(quadmsh->vertex_color);
    }

    if (quadmsh->vertex_alpha != NULL) {
	dor_space_deallocate(quadmsh->vertex_alpha);
    }

    if (quadmsh->element_normal != NULL) {
	dor_space_deallocate(quadmsh->element_normal);
    }

    for (i=0; i< quadmsh->uv_count; i++) {
	dor_space_deallocate(quadmsh->uv_list[i]);
    }
    dor_space_deallocate(quadmsh->uv_list);

    for (i=0; i< quadmsh->uvw_count; i++) {
	dor_space_deallocate(quadmsh->uvw_list[i]);
    }
    dor_space_deallocate(quadmsh->uvw_list);

    if (quadmsh->quad_vertexlist != NULL){
	dor_space_deallocate(quadmsh->quad_vertexlist);
    }

    if (quadmsh->compiled_quad_vertexlist != NULL){
	dor_space_deallocate(quadmsh->compiled_quad_vertexlist);
    }

    if (quadmsh->alt_object_lines != NULL) {
	dor_object_check_deletion(quadmsh->alt_object_lines);
    }

    if (quadmsh->alt_object_tris != NULL) {
	dor_object_check_deletion(quadmsh->alt_object_tris);
    }

    dor_space_deallocate(quadmsh);
}
/*
 ======================================================================
 */

void dor_quadmsh_print (dot_object *object)
{
    struct quadmsh *quadmsh;

    quadmsh = (struct quadmsh *)(object->data);

    if (quadmsh == NULL){
	dor_print_output("quad mesh is NULL");
	return;
    }
 
    switch (quadmsh->colormodel){
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;
 
    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    dor_utilpri_print_vertextype(quadmsh->vertextype);

    sprintf(dor_print_get_line(),"smooth flag = %d",quadmsh->smoothflag);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"vertex count = %d",quadmsh->vertex_count);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"quad count = %d",quadmsh->quad_count);
    dor_print_output(dor_print_get_line());

    if (quadmsh->uv_count > 0) {
	sprintf (dor_print_get_line(), "uv sets per vertex = %d", quadmsh->uv_count);
	dor_print_output(dor_print_get_line());
    }

    if (quadmsh->uvw_count > 0) {
	sprintf (dor_print_get_line(), "uvw sets per vertex = %d", quadmsh->uvw_count);
	dor_print_output(dor_print_get_line());
    }
}
/*
 ======================================================================
 */
dot_object *dor_quadmsh_update_alternate (dot_object *object)
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
            alt = dor_quadmsh_update_tris(object);
            break;

    	case DcOutlines:
            alt = dor_quadmsh_update_lines(object);
            break;
    	default:
            DDerror(ERR_VALUE_OUT_OF_RANGE,
                   "dor_quadmsh_update_alternate",
                   "unknown reptype");
            alt = DcNullPtr;
            break;
    }
    return alt;
}
/*
 ======================================================================
 */
dot_object *dor_quadmsh_update_lines (dot_object *object)
{
    struct quadmsh *quadmsh;
    dot_object *alt;

    quadmsh = (struct quadmsh *)(object->data);

    if (quadmsh->alt_object_lines != DcNullPtr) {
	alt = quadmsh->alt_object_lines;

    } else {
	quadmsh->alt_object_lines = 
		dor_quadmsh_create_alternate_geom_lines(quadmsh);
	alt = quadmsh->alt_object_lines;

	if (quadmsh->alt_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_quadmsh_update_lines", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */
dot_object *dor_quadmsh_update_tris (dot_object *object)
{
    struct quadmsh *quadmsh;
    dot_object *alt;

    quadmsh = (struct quadmsh *)(object->data);

    if (quadmsh->alt_object_tris != DcNullPtr) {
        alt = quadmsh->alt_object_tris;

    } else {
        quadmsh->alt_object_tris =
                dor_quadmsh_create_alternate_geom_tris(quadmsh);
        alt = quadmsh->alt_object_tris;

        if (quadmsh->alt_object_tris == DcNullPtr) {
            DDerror(ERR_CANT_CREATE_ALTOBJ,
                       "dor_quadmsh_update_tris", "");
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

dot_object *dor_quadmsh_create_alternate_geom_lines (struct quadmsh *quadmsh)
{
    DtReal vtxs[MAXVTXS_LIN];
    DtVertexType vertextype;
    DtInt quadno;
    DtInt lvtx0, lvtx1;
    DtUInt i;
    DtUInt uv, uvw;
    DtInt vertexno;
    dot_object *linlstobj;

    vertextype = quadmsh->vertextype;

    if (quadmsh->smoothflag) {
        /* add normals to vertextype */
        vertextype |= DcLocNrm;
    }

    linlstobj = dor_linlst_open(quadmsh->colormodel, vertextype | 
			dor_system_texture_uv_count(quadmsh->uv_count) |
			dor_system_texture_uvw_count(quadmsh->uvw_count),
			4*quadmsh->quad_count);

    for (quadno=0; quadno<quadmsh->quad_count; quadno++) {
	for (vertexno=0; vertexno<4; vertexno++){

	    lvtx0 = quadmsh->quad_vertexlist[quadno][vertexno];
	    if (vertexno==3) 
		    lvtx1 = quadmsh->quad_vertexlist[quadno][0];
	    else
		    lvtx1 = quadmsh->quad_vertexlist[quadno][vertexno+1];

	    i = 0;

	    /* One end of a line */

	    vtxs[i++] = quadmsh->vertex_location[lvtx0][0];
	    vtxs[i++] = quadmsh->vertex_location[lvtx0][1];
	    vtxs[i++] = quadmsh->vertex_location[lvtx0][2];

	    if (vertextype & DcLocNrm) {
		vtxs[i++] = quadmsh->vertex_normal[lvtx0][0];
		vtxs[i++] = quadmsh->vertex_normal[lvtx0][1];
		vtxs[i++] = quadmsh->vertex_normal[lvtx0][2];
	    }

	    if (vertextype & DcLocClr) {
		vtxs[i++] = quadmsh->vertex_color[lvtx0][0];
		vtxs[i++] = quadmsh->vertex_color[lvtx0][1];
		vtxs[i++] = quadmsh->vertex_color[lvtx0][2];
	    }
	
	    if (vertextype & DcLocAlpha) {
		vtxs[i++] = quadmsh->vertex_alpha[lvtx0];
	    }
	
	    for (uv=0; uv<quadmsh->uv_count; uv++) {
		vtxs[i++] = quadmsh->uv_list[uv][lvtx0][0]; 
		vtxs[i++] = quadmsh->uv_list[uv][lvtx0][1]; 
	    }
	
	    for (uvw=0; uvw<quadmsh->uvw_count; uvw++) {
		vtxs[i++] = quadmsh->uvw_list[uvw][lvtx0][0]; 
		vtxs[i++] = quadmsh->uvw_list[uvw][lvtx0][1]; 
		vtxs[i++] = quadmsh->uvw_list[uvw][lvtx0][2]; 
	    }
	
	    /* The other end of the line */
	
	    vtxs[i++] = quadmsh->vertex_location[lvtx1][0]; 
	    vtxs[i++] = quadmsh->vertex_location[lvtx1][1]; 
	    vtxs[i++] = quadmsh->vertex_location[lvtx1][2]; 
	 
	    if (vertextype & DcLocNrm) {
		vtxs[i++] = quadmsh->vertex_normal[lvtx1][0];
		vtxs[i++] = quadmsh->vertex_normal[lvtx1][1];
		vtxs[i++] = quadmsh->vertex_normal[lvtx1][2];
	    } 
	 
	    if (vertextype & DcLocClr) {
		vtxs[i++] = quadmsh->vertex_color[lvtx1][0];
		vtxs[i++] = quadmsh->vertex_color[lvtx1][1];
		vtxs[i++] = quadmsh->vertex_color[lvtx1][2];
	    }
	
	    if (vertextype & DcLocAlpha) {
		vtxs[i++] = quadmsh->vertex_alpha[lvtx1];
	    }
	
	    for (uv=0; uv<quadmsh->uv_count; uv++) {
		vtxs[i++] = quadmsh->uv_list[uv][lvtx1][0]; 
		vtxs[i++] = quadmsh->uv_list[uv][lvtx1][1]; 
	    }
	
	    for (uvw=0; uvw<quadmsh->uvw_count; uvw++) {
		vtxs[i++] = quadmsh->uvw_list[uvw][lvtx1][0]; 
		vtxs[i++] = quadmsh->uvw_list[uvw][lvtx1][1]; 
		vtxs[i++] = quadmsh->uvw_list[uvw][lvtx1][2]; 
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

dot_object *dor_quadmsh_create_alternate_geom_tris (struct quadmsh *quadmsh)
{
    DtReal *vertices, *vptr;
    DtVertexType vertextype;
    DtInt quadno;
    DtInt vertexno;
    DtInt trianglecount;
    DtUInt uv, uvw;
    dot_object *trimshobj;
    DtIntTriple *trivtxlst, *vconnectptr;
    DtInt vertexsize;

    /* 
     * In quad-to-triangle conversion, the vertices don't change but
     * the number of triangles is twice the number of quads.
     */
    vertextype = quadmsh->vertextype;
    if (quadmsh->smoothflag) {
        /* add normals to vertextype */
        vertextype |= DcLocNrm;
    }

    trianglecount = 2 * quadmsh->quad_count;

    trivtxlst = (DtIntTriple *)dor_space_allocate(
		trianglecount*(sizeof (DtIntTriple)));
    vconnectptr = trivtxlst;
    for (quadno=0; quadno<quadmsh->quad_count; quadno++) {
	(*vconnectptr)[0] = quadmsh->quad_vertexlist[quadno][0];
	(*vconnectptr)[1] = quadmsh->quad_vertexlist[quadno][1];
	(*vconnectptr)[2] = quadmsh->quad_vertexlist[quadno][2];
	vconnectptr++;

	(*vconnectptr)[0] = quadmsh->quad_vertexlist[quadno][0];
	(*vconnectptr)[1] = quadmsh->quad_vertexlist[quadno][2];
	(*vconnectptr)[2] = quadmsh->quad_vertexlist[quadno][3];
	vconnectptr++;
    }

    vertexsize = 3;
    if (vertextype & DcLocNrm)
        vertexsize += 3;
    if (vertextype & DcLocClr)
        vertexsize += 3;
    if (vertextype & DcLocAlpha)
        vertexsize += 3;

    vertexsize = vertexsize + quadmsh->uv_count*2 + quadmsh->uvw_count*3;

    vertices = (DtReal *)dor_space_allocate(
		quadmsh->vertex_count*vertexsize*(sizeof(DtReal)));

    vptr = vertices;
    for (vertexno=0; vertexno<quadmsh->vertex_count; vertexno++) {
	*vptr++ = quadmsh->vertex_location[vertexno][0];
	*vptr++ = quadmsh->vertex_location[vertexno][1];
	*vptr++ = quadmsh->vertex_location[vertexno][2];

	if (vertextype & DcLocNrm)  {
	    *vptr++ = quadmsh->vertex_normal[vertexno][0];
	    *vptr++ = quadmsh->vertex_normal[vertexno][1];
	    *vptr++ = quadmsh->vertex_normal[vertexno][2];
	}
	if (vertextype & DcLocClr)  {
	    *vptr++ = quadmsh->vertex_color[vertexno][0];
	    *vptr++ = quadmsh->vertex_color[vertexno][1];
	    *vptr++ = quadmsh->vertex_color[vertexno][2];
	}
	if (vertextype & DcLocAlpha)  {
	    *vptr++ = quadmsh->vertex_alpha[vertexno];
	}
	for (uv=0; uv<quadmsh->uv_count; uv++) {
            *vptr++ = quadmsh->uv_list[uv][vertexno][0];
            *vptr++ = quadmsh->uv_list[uv][vertexno][1];
        }
	for (uvw=0; uvw<quadmsh->uvw_count; uvw++) {
            *vptr++ = quadmsh->uvw_list[uvw][vertexno][0];
            *vptr++ = quadmsh->uvw_list[uvw][vertexno][1];
            *vptr++ = quadmsh->uvw_list[uvw][vertexno][2];
        }
    }

    trimshobj = dor_trimsh_create(quadmsh->colormodel, 
    		  vertextype | dor_system_texture_uv_count(quadmsh->uv_count) |
		  dor_system_texture_uvw_count(quadmsh->uvw_count) ,
			quadmsh->vertex_count, vertices,
			trianglecount, trivtxlst, quadmsh->smoothflag);

    dor_space_deallocate(trivtxlst);
    dor_space_deallocate(vertices);

    return(trimshobj);
}
/*
 ======================================================================
 */

void dor_quadmsh_calculate_quad_normals (struct quadmsh *quadmsh)
{
    DtInt quadno;
    DtInt vertex1, vertex2, vertex3;

    /* This code assumes the quad is planar and uses the first three
       vertices of the quad. */

    for (quadno=0;quadno<quadmsh->quad_count;quadno++){
	vertex1 = quadmsh->quad_vertexlist[quadno][0];
	vertex2 = quadmsh->quad_vertexlist[quadno][1];
	vertex3 = quadmsh->quad_vertexlist[quadno][2];

	dor_math_fast_cross_cosines(quadmsh->vertex_location[vertex1],
				    quadmsh->vertex_location[vertex2],
				    quadmsh->vertex_location[vertex3],
				    quadmsh->element_normal[quadno]);

	dor_math_renormalize_fast(quadmsh->element_normal[quadno]);
    }
}
/*
 ======================================================================
 */

void dor_quadmsh_calculate_vertex_normals (struct quadmsh *quadmsh)
{
    DtFlag *vertexrefflag;
    DtInt vertexno;
    DtInt quadno;
    DtInt quadvertexno;

    /*  Allocate a reference flag table for each vertex.  The reference
	flag tells us if the vertex belongs to any quad, and therefore
	whether its direction vector was computed and should be renormalized.  */

    vertexrefflag = (DtFlag *)dor_space_allocate
			(quadmsh->vertex_count*(sizeof (DtFlag)));

    /*  Zero the vertex normals and quad reference flags for
	all vertices.  */

PRAGMA(ASIS)
    for (vertexno=0;vertexno<quadmsh->vertex_count;vertexno++) {
   	vertexrefflag[vertexno] = DcFalse;

   	quadmsh->vertex_normal[vertexno][0] = 0.;
   	quadmsh->vertex_normal[vertexno][1] = 0.;
   	quadmsh->vertex_normal[vertexno][2] = 0.;
    }

    /*  For each quads' four vertices, update that vertex's
	quad reference flag and accumulated quad normal.  */

PRAGMA(ASIS)
    for (quadno=0;quadno<quadmsh->quad_count;quadno++) {

   	for (quadvertexno=0;quadvertexno<4;quadvertexno++) {

	    vertexno = quadmsh->quad_vertexlist[quadno][quadvertexno];

	    vertexrefflag[vertexno] = DcTrue;

	    quadmsh->vertex_normal[vertexno][0] +=
		    quadmsh->element_normal[quadno][0];
	    quadmsh->vertex_normal[vertexno][1] +=
		    quadmsh->element_normal[quadno][1];
	    quadmsh->vertex_normal[vertexno][2] +=
		    quadmsh->element_normal[quadno][2];
   	}
    }

    /*  Rescale the vertex normals.  */

PRAGMA(ASIS)
    for (vertexno=0;vertexno<quadmsh->vertex_count;vertexno++) {

   	if (vertexrefflag[vertexno]) {
	    if (!dor_math_renormalize_vector(quadmsh->vertex_normal[vertexno])) {
		DDerror (ERR_ZERO_QUAD_NRM_SUM,
			    "dor_quadmsh_calculate_vertex_normals", 
			    DcNullPtr);
	    }
   	}
    }

    /*  Free the vertex reference flags.  */

    dor_space_deallocate(vertexrefflag);
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

void dor_quadmsh_bounding_box (struct quadmsh *quadmsh)
{
    DtReal xmin, ymin, zmin, xmax, ymax, zmax;
    DtInt i, vertexno;
    DtInt quadno;

    if (quadmsh->quad_count == 0) {
        xmin = 0.;
        ymin = 0.;
        zmin = 0.;

        xmax = 0.;
        ymax = 0.;
        zmax = 0.;
    } else {

        vertexno = quadmsh->quad_vertexlist[0][0];

        xmin = quadmsh->vertex_location[vertexno][0];
        xmax = quadmsh->vertex_location[vertexno][0];

        ymin = quadmsh->vertex_location[vertexno][1];
        ymax = quadmsh->vertex_location[vertexno][1];

        zmin = quadmsh->vertex_location[vertexno][2];
        zmax = quadmsh->vertex_location[vertexno][2];

PRAGMA(IVDEP)
PRAGMA(VBEST)

        /* use only the vertices that are referenced in the mesh */

        for (quadno=0;quadno<quadmsh->quad_count;quadno++){
            for (i=0; i<4; i++) {
                vertexno = quadmsh->quad_vertexlist[quadno][i];

                xmin = min(xmin,quadmsh->vertex_location[vertexno][0]);
                xmax = max(xmax,quadmsh->vertex_location[vertexno][0]);

                ymin = min(ymin,quadmsh->vertex_location[vertexno][1]);
                ymax = max(ymax,quadmsh->vertex_location[vertexno][1]);

                zmin = min(zmin,quadmsh->vertex_location[vertexno][2]);
                zmax = max(zmax,quadmsh->vertex_location[vertexno][2]);
            }
        }
    }

    quadmsh->bounding_box_pts[0][0] = xmin;
    quadmsh->bounding_box_pts[0][1] = ymin;
    quadmsh->bounding_box_pts[0][2] = zmin;

    quadmsh->bounding_box_pts[1][0] = xmin;
    quadmsh->bounding_box_pts[1][1] = ymax;
    quadmsh->bounding_box_pts[1][2] = zmin;

    quadmsh->bounding_box_pts[2][0] = xmax;
    quadmsh->bounding_box_pts[2][1] = ymax;
    quadmsh->bounding_box_pts[2][2] = zmin;

    quadmsh->bounding_box_pts[3][0] = xmax;
    quadmsh->bounding_box_pts[3][1] = ymin;
    quadmsh->bounding_box_pts[3][2] = zmin;

    quadmsh->bounding_box_pts[4][0] = xmin;
    quadmsh->bounding_box_pts[4][1] = ymin;
    quadmsh->bounding_box_pts[4][2] = zmax;

    quadmsh->bounding_box_pts[5][0] = xmin;
    quadmsh->bounding_box_pts[5][1] = ymax;
    quadmsh->bounding_box_pts[5][2] = zmax;

    quadmsh->bounding_box_pts[6][0] = xmax;
    quadmsh->bounding_box_pts[6][1] = ymax;
    quadmsh->bounding_box_pts[6][2] = zmax;

    quadmsh->bounding_box_pts[7][0] = xmax;
    quadmsh->bounding_box_pts[7][1] = ymin;
    quadmsh->bounding_box_pts[7][2] = zmax;
}
/*
 ======================================================================
 */

void dor_quadmsh_compute_bounding_volume (dot_object *object)
{
    struct quadmsh *quadmsh;

    quadmsh = (struct quadmsh *)(object->data);

    dor_system_report_bounding_points (8, (DtReal*)quadmsh->bounding_box_pts);
}

/*
 ======================================================================
 */

void dor_quadmsh_pick (dot_object *object)
{
    DtRepType reptype;

    if (!dor_condex_query_pick(doe_quadmsh_class_id))
            return;

    dor_pick_initialize_object(object);

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    case DcOutlines:
        dor_quadmsh_pick_lines(object);
        break;

    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
    case DcWireframe:
    case DcPoints:
        dor_quadmsh_pick_tris(object);
        break;
    default:
        DDerror(ERR_VALUE_OUT_OF_RANGE,
                   "dor_quadmsh_pick",
                   "unknown reptype");
    }
}
/*
 ======================================================================
 */

void dor_quadmsh_pick_lines (dot_object *object)
{
    struct quadmsh *quadmsh;

    quadmsh = (struct quadmsh *)(object->data);

    if (quadmsh->alt_object_lines != DcNullPtr) {
        doe_system_alternate_object_execution_flag = DcTrue;

        dor_object_get_method(quadmsh->alt_object_lines,
                              DcMethodPick)(quadmsh->alt_object_lines);

        doe_system_alternate_object_execution_flag = DcFalse;

        return;
    } else {
        quadmsh->alt_object_lines =
                dor_quadmsh_create_alternate_geom_lines(quadmsh);

        if (quadmsh->alt_object_lines == NULL) {
            DDerror(ERR_CANT_CREATE_ALTOBJ,
                       "dor_quadmsh_pick_lines", "");
            return;
        }

        doe_system_alternate_object_execution_flag = DcTrue;

        dor_object_get_method(quadmsh->alt_object_lines,
                              DcMethodPick)(quadmsh->alt_object_lines);

        doe_system_alternate_object_execution_flag = DcFalse;

        return;
    }
}
/*
 ======================================================================
 */

void dor_quadmsh_pick_tris (dot_object *object)
{
    struct quadmsh *quadmsh;

    quadmsh = (struct quadmsh *)(object->data);

    if (quadmsh->alt_object_tris != DcNullPtr) {
        doe_system_alternate_object_execution_flag = DcTrue;

        dor_object_get_method(quadmsh->alt_object_tris,
                              DcMethodPick)(quadmsh->alt_object_tris);

        doe_system_alternate_object_execution_flag = DcFalse;

        return;
    } else {
        quadmsh->alt_object_tris =
                dor_quadmsh_create_alternate_geom_tris(quadmsh);

        if (quadmsh->alt_object_tris == NULL) {
            DDerror(ERR_CANT_CREATE_ALTOBJ,
                       "dor_quadmsh_pick_tris", "");
            return;
        }

        doe_system_alternate_object_execution_flag = DcTrue;

        dor_object_get_method(quadmsh->alt_object_tris,
                              DcMethodPick)(quadmsh->alt_object_tris);

        doe_system_alternate_object_execution_flag = DcFalse;

        return;
    }
}
/*
 ======================================================================
 */

void dor_quadmsh_render_standard (dot_object *object)
{
    dot_object *alt;

    if (!dor_condex_query_render(doe_quadmsh_class_id))
            return;

    alt = dor_quadmsh_update_alternate (object);
    if (alt != DcNullPtr) {
        dor_object_get_current_method(alt) (alt);
    }
}
