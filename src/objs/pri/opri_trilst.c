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
	dor_trilst_initialize
	dor_trilst_create
	dor_trilst_destroy
	dor_trilst_print
	dor_trilst_update_lines
	dor_trilst_open
	dor_trilst_close
	dor_trilst_bounding_box
	dor_trilst_create_alternate_lines
	dor_trilst_calculate_triangle_normals
	dor_trilst_compute_bounding_volume
	dor_trilst_pick
	dor_trilst_add_triangle
	dor_trilst_add_triangle_space
	dor_trilst_add_triangle_with_edges
	dor_trilst_add_triangle_with_edges_space

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/trilst.h>
#include <dore/dore_develop/private/linlst.h>

DtInt doe_trilst_class_id;
DtFlag doe_trilst_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry trilst_methods[] = {
    {DcMethodCmpBndVolume, (DtMethodPtr)dor_trilst_compute_bounding_volume},
    {DcMethodDestroy, (DtMethodPtr)dor_trilst_destroy},
    {DcMethodPick, (DtMethodPtr)dor_trilst_pick},
    {DcMethodPrint, (DtMethodPtr)dor_trilst_print},
    {DcMethodUpdStdAltLineObj, (DtMethodPtr)dor_trilst_update_lines}
};

void dor_trilst_initialize (void)
{
    doe_trilst_class_id = dor_class_install (DcTypeTriangleList, "DoTriangleList", 
					     5, trilst_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_trilst_create (
    DtColorModel colormodel,
    DtVertexType vertextype,
    DtInt trianglecount,
    DtReal vertices[3])
{
    struct trilst *trilst;
    DtInt vertexcount;
    DtReal *vertexptr;
    DtInt vertexno;
    DtVertexType basevtxtype;
    DtInt uv;
    DtInt uvw;
    dot_object *trilstobject;

    if (!doe_trilst_initialized) {
      	dor_trilst_initialize();
   	doe_trilst_initialized = DcTrue;
    }

    trilst = (struct trilst *)dor_space_allocate(sizeof *trilst);

    trilst->alt_object_lines = (DtObjectStructure *)0;

    trilst->colormodel = colormodel;

    trilst->triangle_count = trianglecount;

    basevtxtype = dod_system_inq_vertex_basetype(vertextype);
    trilst->vertextype = basevtxtype;

    vertexcount = trianglecount * 3;

    /*  Vertices always have locations.  */

    trilst->vertex_location = (DtRealTriple *)dor_space_allocate
				(vertexcount*(sizeof (DtRealTriple)));

    /* texture coordinates */

    trilst->uv_list = (DtRealCouple **)dor_space_allocate
			(dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
    trilst->uvw_list = (DtRealTriple **)dor_space_allocate
			(dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *));

    for (uv=0; uv < dod_system_max_vertex_uv_cnt; uv++) {
   	trilst->uv_list[uv] = NULL;
    }
    for (uvw=0; uvw < dod_system_max_vertex_uvw_cnt; uvw++) {
   	trilst->uvw_list[uvw] = NULL;
    }

    trilst->uv_count = dod_system_inq_vertex_uv_cnt(vertextype);
    trilst->uvw_count = dod_system_inq_vertex_uvw_cnt(vertextype);

    for (uv=0; uv< trilst->uv_count; uv++) {
   	trilst->uv_list[uv] = (DtRealCouple *)dor_space_allocate
			(vertexcount*(sizeof (DtRealCouple)));
    }

    for (uvw=0; uvw< trilst->uvw_count; uvw++) {
   	trilst->uvw_list[uvw] = (DtRealTriple *)dor_space_allocate
			(vertexcount*(sizeof (DtRealTriple)));
    }

    /**/

    trilst->vertex_normal = (DtRealTriple *)NULL;
    trilst->vertex_color = (DtColorRGB *)NULL;
    trilst->vertex_alpha = (DtReal *)NULL;

    vertexptr = vertices;

PRAGMA(ASIS)
    if (basevtxtype & DcLocNrm)
        trilst->vertex_normal = (DtRealTriple *)
            dor_space_allocate(vertexcount*(sizeof (DtRealTriple)));

    if (basevtxtype & DcLocClr)
        trilst->vertex_color = (DtColorRGB *)
            dor_space_allocate(vertexcount*(sizeof (DtColorRGB)));

    if (basevtxtype & DcLocAlpha)
        trilst->vertex_alpha = (DtReal *)
            dor_space_allocate(vertexcount*(sizeof (DtReal)));

    for (vertexno=0; vertexno<vertexcount; vertexno++) {
        trilst->vertex_location[vertexno][0] = *vertexptr++;
        trilst->vertex_location[vertexno][1] = *vertexptr++;
        trilst->vertex_location[vertexno][2] = *vertexptr++;

        if (basevtxtype & DcLocNrm) {
            trilst->vertex_normal[vertexno][0] = *vertexptr++;
            trilst->vertex_normal[vertexno][1] = *vertexptr++;
            trilst->vertex_normal[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocClr) {
            trilst->vertex_color[vertexno][0] = *vertexptr++;
            trilst->vertex_color[vertexno][1] = *vertexptr++;
            trilst->vertex_color[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocAlpha) {
            trilst->vertex_alpha[vertexno] = *vertexptr++;
        }

        for (uv=0; uv < trilst->uv_count; uv++) {
            trilst->uv_list[uv][vertexno][0] = *vertexptr++;
            trilst->uv_list[uv][vertexno][1] = *vertexptr++;
        }

        for (uvw=0; uvw < trilst->uvw_count; uvw++) {
            trilst->uvw_list[uvw][vertexno][0] = *vertexptr++;
            trilst->uvw_list[uvw][vertexno][1] = *vertexptr++;
            trilst->uvw_list[uvw][vertexno][2] = *vertexptr++;
        }
    }



    trilst->edge_enable = NULL;

    /*  Compute element normals for all triangles in the list.  */

    trilst->element_normal = (DtRealTriple *)dor_space_allocate
				(trianglecount*(sizeof (DtRealTriple)));

    dor_trilst_calculate_triangle_normals(trilst);

    dor_trilst_bounding_box(trilst);

    trilstobject = dor_object_create(doe_trilst_class_id,trilst);

    return(trilstobject);
}
/*
 ======================================================================
 */

void dor_trilst_destroy (dot_object *object)
{
    struct trilst *trilst;
    int i;

    trilst = (struct trilst *)(object->data);

    if (trilst->vertex_location != NULL) {
	dor_space_deallocate(trilst->vertex_location);
    }

    if (trilst->vertex_normal != NULL) {
	dor_space_deallocate(trilst->vertex_normal);
    }

    if (trilst->vertex_color != NULL) {
	dor_space_deallocate(trilst->vertex_color);
    }

    if (trilst->vertex_alpha != NULL) {
	dor_space_deallocate(trilst->vertex_alpha);
    }

    if (trilst->element_normal != NULL) {
	dor_space_deallocate(trilst->element_normal);
    }

    for (i=0; i< trilst->uv_count; i++) {
	dor_space_deallocate(trilst->uv_list[i]);
    }
    dor_space_deallocate(trilst->uv_list);

    for (i=0; i< trilst->uvw_count; i++) {
	dor_space_deallocate(trilst->uvw_list[i]);
    }
    dor_space_deallocate(trilst->uvw_list);

    if (trilst->edge_enable != NULL) {
	dor_space_deallocate(trilst->edge_enable);
    }

    dor_space_deallocate(trilst);
}
/*
 ======================================================================
 */

void dor_trilst_print (dot_object *object)
{
    struct trilst *trilst;

    trilst = (struct trilst *)(object->data);

    if (trilst == NULL){
	dor_print_output("triangle list is NULL");
	return;
    }
 
    switch (trilst->colormodel){
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;
 
    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    dor_utilpri_print_vertextype (trilst->vertextype);

    sprintf(dor_print_get_line(),"triangle count = %d",trilst->triangle_count);
    dor_print_output(dor_print_get_line());

    if (trilst->uv_count > 0) {
	sprintf (dor_print_get_line(), "uv's per vertex = %d", trilst->uv_count);
	dor_print_output(dor_print_get_line());
    }

    if (trilst->uvw_count > 0) {
	sprintf (dor_print_get_line(), "uvw's per vertex = %d", trilst->uvw_count);
	dor_print_output(dor_print_get_line());
    }
}
/*
 ======================================================================
 */

dot_object *dor_trilst_update_lines (dot_object *object)
{
    struct trilst *trilst;
    dot_object *alt;

    trilst = (struct trilst *)(object->data);

    if (trilst->alt_object_lines != DcNullPtr) {
	alt = trilst->alt_object_lines;

    } else {
	trilst->alt_object_lines = 
		dor_trilst_create_alternate_lines(trilst);
	alt = trilst->alt_object_lines;

	if (trilst->alt_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_trilst_update_lines", "");
	}
    }
    return alt;
}

/* ====================================================================== */

dot_object *dor_trilst_open (
    DtColorModel colormodel,
    DtVertexType vertextype,
    DtInt        space_estimate)
{
    register struct trilst *trilst;
    register dot_object    *trilstobject;
    register DtVertexType   basevtxtype;
    register DtInt          i;
    
    if (!doe_trilst_initialized) {
	dor_trilst_initialize ();
	doe_trilst_initialized = DcTrue;
    }

    /* Allocate the triangle list structure. */
    
    trilst = dor_space_allocate (sizeof *trilst);

    /* Initialize the triangle list fields. */
    
    trilst->alt_object_lines = DcNullPtr;
    trilst->colormodel       = colormodel;
    trilst->edge_enable      = NULL;
    trilst->space_estimate   = (space_estimate < 1) ? 1 : space_estimate;
    trilst->triangle_count   = 0;
    trilst->triangle_space   = 0;
    trilst->vertex_location  = NULL;
    trilst->vertex_normal    = NULL;
    trilst->vertex_color     = NULL;
    trilst->vertex_alpha     = NULL;

    basevtxtype = dod_system_inq_vertex_basetype (vertextype);
    trilst->vertextype = basevtxtype;

    /* Allocate and initialize the texture coordinate data. */

    trilst->uv_list = dor_space_allocate
	    (dod_system_max_vertex_uv_cnt * sizeof(DtRealCouple *));
    trilst->uvw_list = dor_space_allocate
	    (dod_system_max_vertex_uv_cnt * sizeof(DtRealTriple *));

    for (i = 0; i < dod_system_max_vertex_uv_cnt; i++) {
	trilst->uv_list[i] = DcNullPtr;
    }
    for (i = 0; i < dod_system_max_vertex_uvw_cnt; i++) {
	trilst->uvw_list[i] = DcNullPtr;
    }
    
    trilst->uv_count  = dod_system_inq_vertex_uv_cnt (vertextype);
    trilst->uvw_count = dod_system_inq_vertex_uvw_cnt (vertextype);
    
    trilstobject = dor_object_create (doe_trilst_class_id, trilst);
    
    return trilstobject;
}
/*
 ======================================================================
 */

void dor_trilst_close (dot_object *object)
{
    struct trilst *trilst;
    DtInt vertexcount;
    DtInt uv, uvw;

    trilst = (struct trilst *)(object->data);

    vertexcount = trilst->triangle_count * 3;

    /*  Recompress the space used to store the vertex data to be exactly
	big enough.  */

    if (trilst->edge_enable != NULL) {
	trilst->edge_enable = (DtInt *) dor_space_reallocate
		(trilst->edge_enable, trilst->triangle_count*(sizeof (DtInt)));
    }

    if (vertexcount != 0) {

   	trilst->vertex_location = (DtRealTriple *)dor_space_reallocate
		(trilst->vertex_location,vertexcount*(sizeof (DtRealTriple)));

	if (trilst->vertextype & DcLocNrm) 
	    trilst->vertex_normal = (DtRealTriple *)dor_space_reallocate
		(trilst->vertex_normal,vertexcount*(sizeof (DtRealTriple)));

	if (trilst->vertextype & DcLocClr) 
	    trilst->vertex_color = (DtColorRGB *)dor_space_reallocate
		(trilst->vertex_color,vertexcount*(sizeof (DtColorRGB)));

	if (trilst->vertextype & DcLocAlpha) 
	    trilst->vertex_alpha = (DtReal *)dor_space_reallocate
		(trilst->vertex_alpha,vertexcount*(sizeof (DtReal)));

        for (uv=0; uv < trilst->uv_count; uv++) { 
	    trilst->uv_list[uv] = (DtRealCouple *)dor_space_reallocate
		(trilst->uv_list[uv], vertexcount*(sizeof (DtRealCouple)));      
        } 
        for (uvw=0; uvw < trilst->uvw_count; uvw++) {
	    trilst->uvw_list[uvw] = (DtRealTriple *)dor_space_reallocate
		(trilst->uvw_list[uvw], vertexcount*(sizeof (DtRealTriple)));
        }
    }

    /*  Compute element normals for all triangles in the list.  */

    trilst->element_normal = (DtRealTriple *)dor_space_allocate
		(trilst->triangle_count*(sizeof (DtRealTriple)));

    dor_trilst_calculate_triangle_normals(trilst);

    dor_trilst_bounding_box(trilst);
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

void dor_trilst_bounding_box (struct trilst *trilst)
{
    DtReal xmin, ymin, zmin, xmax, ymax, zmax;
    DtInt vertexno;
    DtInt vertexcount;

    if (trilst->triangle_count == 0){
	xmin = 0.;
	ymin = 0.;
	zmin = 0.;

	xmax = 0.;
	ymax = 0.;
	zmax = 0.;
    }
    else{
	xmin = trilst->vertex_location[0][0];
	xmax = trilst->vertex_location[0][0];

	ymin = trilst->vertex_location[0][1];
	ymax = trilst->vertex_location[0][1];

	zmin = trilst->vertex_location[0][2];
	zmax = trilst->vertex_location[0][2];

	vertexcount = 3 * trilst->triangle_count;

	for (vertexno=1;vertexno<vertexcount;vertexno++){
	    xmin = min(xmin,trilst->vertex_location[vertexno][0]);
	    xmax = max(xmax,trilst->vertex_location[vertexno][0]);

	    ymin = min(ymin,trilst->vertex_location[vertexno][1]);
	    ymax = max(ymax,trilst->vertex_location[vertexno][1]);

	    zmin = min(zmin,trilst->vertex_location[vertexno][2]);
	    zmax = max(zmax,trilst->vertex_location[vertexno][2]);
	}
    }

    trilst->bounding_box_pts[0][0] = xmin;
    trilst->bounding_box_pts[0][1] = ymin;
    trilst->bounding_box_pts[0][2] = zmin;

    trilst->bounding_box_pts[1][0] = xmin;
    trilst->bounding_box_pts[1][1] = ymax;
    trilst->bounding_box_pts[1][2] = zmin;

    trilst->bounding_box_pts[2][0] = xmax;
    trilst->bounding_box_pts[2][1] = ymax;
    trilst->bounding_box_pts[2][2] = zmin;

    trilst->bounding_box_pts[3][0] = xmax;
    trilst->bounding_box_pts[3][1] = ymin;
    trilst->bounding_box_pts[3][2] = zmin;

    trilst->bounding_box_pts[4][0] = xmin;
    trilst->bounding_box_pts[4][1] = ymin;
    trilst->bounding_box_pts[4][2] = zmax;

    trilst->bounding_box_pts[5][0] = xmin;
    trilst->bounding_box_pts[5][1] = ymax;
    trilst->bounding_box_pts[5][2] = zmax;

    trilst->bounding_box_pts[6][0] = xmax;
    trilst->bounding_box_pts[6][1] = ymax;
    trilst->bounding_box_pts[6][2] = zmax;

    trilst->bounding_box_pts[7][0] = xmax;
    trilst->bounding_box_pts[7][1] = ymin;
    trilst->bounding_box_pts[7][2] = zmax;
}
/*
 ======================================================================
 */

#define MAXVTXS (18 + dod_system_max_vertex_uv_cnt*4 + \
			dod_system_max_vertex_uvw_cnt*6)

dot_object *dor_trilst_create_alternate_lines (struct trilst *trilst)
{
    DtReal vtxs[MAXVTXS];
    DtInt trino;
    DtInt lvtx0, lvtx1;
    DtUInt i;
    DtUInt uv, uvw;
    DtInt vertexno;
    dot_object *linlstobj;

    linlstobj = dor_linlst_open(trilst->colormodel, 
				trilst->vertextype |
				dor_system_texture_uv_count(trilst->uv_count) |
				dor_system_texture_uvw_count(trilst->uvw_count),
				3*trilst->triangle_count);

    for (trino=0; trino<trilst->triangle_count; trino++) {
	for (vertexno=0; vertexno<3; vertexno++){

	    lvtx0 = trino*3+vertexno;
	    if (vertexno==2) 
		    lvtx1 = trino*3;
	    else
		    lvtx1 = trino*3+vertexno+1;

	    i = 0;

	    /* One end of a line */

	    vtxs[i++] = trilst->vertex_location[lvtx0][0];
	    vtxs[i++] = trilst->vertex_location[lvtx0][1];
	    vtxs[i++] = trilst->vertex_location[lvtx0][2];

	    if (trilst->vertextype & DcLocNrm) {
		vtxs[i++] = trilst->vertex_normal[lvtx0][0];
		vtxs[i++] = trilst->vertex_normal[lvtx0][1];
		vtxs[i++] = trilst->vertex_normal[lvtx0][2];
	    }

	    if (trilst->vertextype & DcLocClr) {
		vtxs[i++] = trilst->vertex_color[lvtx0][0];
		vtxs[i++] = trilst->vertex_color[lvtx0][1];
		vtxs[i++] = trilst->vertex_color[lvtx0][2];
	    }
	
	    if (trilst->vertextype & DcLocAlpha) {
		vtxs[i++] = trilst->vertex_alpha[lvtx0];
	    }
	
	    for (uv=0; uv<trilst->uv_count; uv++) {
		vtxs[i++] = trilst->uv_list[uv][lvtx0][0]; 
		vtxs[i++] = trilst->uv_list[uv][lvtx0][1]; 
	    }
	
	    for (uvw=0; uvw<trilst->uvw_count; uvw++) {
		vtxs[i++] = trilst->uvw_list[uvw][lvtx0][0]; 
		vtxs[i++] = trilst->uvw_list[uvw][lvtx0][1]; 
		vtxs[i++] = trilst->uvw_list[uvw][lvtx0][2]; 
	    }
	
	    /* The other end of the line */
	
	    vtxs[i++] = trilst->vertex_location[lvtx1][0]; 
	    vtxs[i++] = trilst->vertex_location[lvtx1][1]; 
	    vtxs[i++] = trilst->vertex_location[lvtx1][2]; 
	 
	    if (trilst->vertextype & DcLocNrm) {
		vtxs[i++] = trilst->vertex_normal[lvtx1][0];
		vtxs[i++] = trilst->vertex_normal[lvtx1][1];
		vtxs[i++] = trilst->vertex_normal[lvtx1][2];
	    } 
	 
	    if (trilst->vertextype & DcLocClr) {
		vtxs[i++] = trilst->vertex_color[lvtx1][0];
		vtxs[i++] = trilst->vertex_color[lvtx1][1];
		vtxs[i++] = trilst->vertex_color[lvtx1][2];
	    }
	
	    if (trilst->vertextype & DcLocAlpha) {
		vtxs[i++] = trilst->vertex_alpha[lvtx1];
	    }
	
	    for (uv=0; uv<trilst->uv_count; uv++) {
		vtxs[i++] = trilst->uv_list[uv][lvtx1][0]; 
		vtxs[i++] = trilst->uv_list[uv][lvtx1][1]; 
	    }
	
	    for (uvw=0; uvw<trilst->uvw_count; uvw++) {
		vtxs[i++] = trilst->uvw_list[uvw][lvtx1][0]; 
		vtxs[i++] = trilst->uvw_list[uvw][lvtx1][1]; 
		vtxs[i++] = trilst->uvw_list[uvw][lvtx1][2]; 
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

void dor_trilst_calculate_triangle_normals (struct trilst *trilst)
{
    DtInt triangleno;

    for (triangleno=0;triangleno<trilst->triangle_count;triangleno++){
	dor_math_fast_cross_cosines(trilst->vertex_location[triangleno*3],
				    trilst->vertex_location[triangleno*3+1],
				    trilst->vertex_location[triangleno*3+2],
				    trilst->element_normal[triangleno]);

	dor_math_renormalize_fast(trilst->element_normal[triangleno]);
    }
}
/*
 ======================================================================
 */

void dor_trilst_compute_bounding_volume (dot_object *object)
{
    struct trilst *trilst;

    trilst = (struct trilst *)(object->data);

    dor_system_report_bounding_points (8, (DtReal*)trilst->bounding_box_pts);
}
/*
 ======================================================================
 */

void dor_trilst_pick (dot_object *object)
{
    struct trilst *trilst;

    if (!dor_condex_query_pick(doe_trilst_class_id))
	    return;

    if (!doe_system_alternate_object_execution_flag){
	dor_pick_initialize_object(object);
    }

    trilst = (struct trilst *)(object->data);

    dor_pick_triangle_list(trilst->bounding_box_pts,trilst->triangle_count,
			   trilst->vertex_location);
}
/*
 ======================================================================
 */

void dor_trilst_add_triangle (
    dot_object *object,
    DtReal *vertices)
{
    struct trilst *trilst;
    DtInt firstvertex;
    DtInt vertexno;
    DtInt uv;
    DtInt uvw;

    trilst = (struct trilst *)(object->data);

    if (trilst->triangle_count >= trilst->triangle_space) {
	dor_trilst_add_triangle_space(trilst);
    }

    firstvertex = trilst->triangle_count * 3;

    for (vertexno=firstvertex;vertexno<firstvertex+3;vertexno++) {
        trilst->vertex_location[vertexno][0] = *vertices++;
        trilst->vertex_location[vertexno][1] = *vertices++;
        trilst->vertex_location[vertexno][2] = *vertices++;

        if (trilst->vertextype & DcLocNrm) {    
            trilst->vertex_normal[vertexno][0] = *vertices++;
            trilst->vertex_normal[vertexno][1] = *vertices++;
            trilst->vertex_normal[vertexno][2] = *vertices++;
        }

        if (trilst->vertextype & DcLocClr) {    
            trilst->vertex_color[vertexno][0] = *vertices++;
            trilst->vertex_color[vertexno][1] = *vertices++;
            trilst->vertex_color[vertexno][2] = *vertices++;
        }

        if (trilst->vertextype & DcLocAlpha) {    
            trilst->vertex_alpha[vertexno] = *vertices++;
        }

        for (uv=0; uv < trilst->uv_count; uv++) {
            trilst->uv_list[uv][vertexno][0] = *vertices++;
            trilst->uv_list[uv][vertexno][1] = *vertices++;
        }

        for (uvw=0; uvw < trilst->uvw_count; uvw++) {
            trilst->uvw_list[uvw][vertexno][0] = *vertices++;
            trilst->uvw_list[uvw][vertexno][1] = *vertices++;
            trilst->uvw_list[uvw][vertexno][2] = *vertices++;
            }
    }

    trilst->triangle_count++;
}
/*
 ======================================================================
 */

void dor_trilst_add_triangle_space (struct trilst *trilst)
{
    DtInt vertexcount;
    DtInt uv_idx;
    DtInt uvw_idx;
    DtPtr newspace;

#ifdef DEBUG
    printf ("dor_trilst_add_triangle_space: from %d to ",
	    trilst->triangle_space);
#endif

    if (trilst->triangle_space == 0) {
	trilst->triangle_space = trilst->space_estimate;
    } else {
	trilst->triangle_space += (.2* trilst->space_estimate) + 1;
    }

#ifdef DEBUG
    printf ("%d\n", trilst->triangle_space);
#endif

    vertexcount = trilst->triangle_space * 3;

    newspace = dor_space_reallocate(trilst->vertex_location,
				    vertexcount*(sizeof (DtRealTriple)));
    if (newspace != NULL) 
	trilst->vertex_location = (DtRealTriple *)newspace;
    else 
	DDerror (ERR_CANT_ALLOC_MEM, 
		 "dor_trilst_add_triangle_space", "vertex_location");

    if (trilst->vertextype & DcLocNrm) {

   	newspace = dor_space_reallocate(trilst->vertex_normal,
					vertexcount*(sizeof (DtRealTriple)));
   	if (newspace != NULL) 
	    trilst->vertex_normal = (DtRealTriple *)newspace;
   	else 
	    DDerror (ERR_CANT_ALLOC_MEM, 
		     "dor_trilst_add_triangle_space", "vertex_normal");

    } 
    if (trilst->vertextype & DcLocClr) {

   	newspace = dor_space_reallocate(trilst->vertex_color,
					vertexcount*(sizeof (DtColorRGB)));
   	if (newspace != NULL) 
	    trilst->vertex_color = (DtColorRGB *)newspace;
   	else 
	    DDerror (ERR_CANT_ALLOC_MEM, 
		     "dor_trilst_add_triangle_space", "vertex_color");
    }
    if (trilst->vertextype & DcLocAlpha) {

   	newspace = dor_space_reallocate(trilst->vertex_alpha,
					vertexcount*(sizeof (DtReal)));
   	if (newspace != NULL) 
	    trilst->vertex_alpha = (DtReal *)newspace;
   	else 
	    DDerror (ERR_CANT_ALLOC_MEM, 
		     "dor_trilst_add_triangle_space", "vertex_alpha");
    }

    for (uv_idx=0; uv_idx< trilst->uv_count; uv_idx++) {
	newspace = dor_space_reallocate(trilst->uv_list[uv_idx],
					vertexcount*(sizeof (DtRealCouple)));
	if (newspace != NULL) 
	    trilst->uv_list[uv_idx] = (DtRealCouple *)newspace;
	else 
	    DDerror (ERR_CANT_ALLOC_MEM,
		     "dor_trilst_add_triangle_space", "uv_list[uv_idx]");
    }

    for (uvw_idx=0; uvw_idx< trilst->uvw_count; uvw_idx++) {
	newspace= dor_space_reallocate( trilst->uvw_list[uvw_idx],
				       vertexcount*(sizeof (DtRealTriple)));
	if (newspace != NULL) 
	    trilst->uvw_list[uvw_idx] = (DtRealTriple *)newspace;
	else 
	    DDerror (ERR_CANT_ALLOC_MEM, 
		     "dor_trilst_add_triangle_space", "uvw_list[uvw_idx]");
    }
}
/*
 ======================================================================
 */

void dor_trilst_add_triangle_with_edges (
    dot_object *object,
    DtReal *vertices,
    DtInt edge_enable)
{
    struct trilst *trilst;
    DtInt firstvertex;
    DtInt vertexno;
    DtInt uv;
    DtInt uvw;

    trilst = (struct trilst *)(object->data);

    if (trilst->triangle_count >= trilst->triangle_space) {
	dor_trilst_add_triangle_with_edges_space(trilst);
    }

    trilst->edge_enable[trilst->triangle_count] = edge_enable;

    firstvertex = trilst->triangle_count * 3;

    for (vertexno=firstvertex;vertexno<firstvertex+3;vertexno++) {
        trilst->vertex_location[vertexno][0] = *vertices++;
        trilst->vertex_location[vertexno][1] = *vertices++;
        trilst->vertex_location[vertexno][2] = *vertices++;

        if (trilst->vertextype & DcLocNrm) {
            trilst->vertex_normal[vertexno][0] = *vertices++;
            trilst->vertex_normal[vertexno][1] = *vertices++;
            trilst->vertex_normal[vertexno][2] = *vertices++;
        }
        if (trilst->vertextype & DcLocClr) {
            trilst->vertex_color[vertexno][0] = *vertices++;
            trilst->vertex_color[vertexno][1] = *vertices++;
            trilst->vertex_color[vertexno][2] = *vertices++;
        }
        if (trilst->vertextype & DcLocAlpha) {
            trilst->vertex_alpha[vertexno] = *vertices++;
        }

        for (uv=0; uv < trilst->uv_count; uv++) {
            trilst->uv_list[uv][vertexno][0] = *vertices++;
            trilst->uv_list[uv][vertexno][1] = *vertices++;
        }

        for (uvw=0; uvw < trilst->uvw_count; uvw++) {
            trilst->uvw_list[uvw][vertexno][0] = *vertices++;
            trilst->uvw_list[uvw][vertexno][1] = *vertices++;
            trilst->uvw_list[uvw][vertexno][2] = *vertices++;
        }
    }

    trilst->triangle_count++;
}
/*
 ======================================================================
 */

void dor_trilst_add_triangle_with_edges_space (struct trilst *trilst)
{
    DtInt vertexcount;
    DtInt uv_idx;
    DtInt uvw_idx;
    DtPtr newspace;

#ifdef DEBUG
    printf ("dor_trilst_add_triangle_space: from %d to ",
	    trilst->triangle_space);
#endif

    if (trilst->triangle_space == 0) {
	trilst->triangle_space = trilst->space_estimate;
    } else {
	trilst->triangle_space += (.2* trilst->space_estimate) + 1;
    }

#ifdef DEBUG
    printf ("%d\n", trilst->triangle_space);
#endif

    newspace = dor_space_reallocate(trilst->edge_enable,
				    trilst->triangle_space*(sizeof (DtInt)));
    if (newspace != NULL)
	trilst->edge_enable = (DtInt *)newspace;
    else
	DDerror (ERR_CANT_ALLOC_MEM,
		 "dor_trilst_add_triangle_with_edges_space", "edge_enable");

    vertexcount = trilst->triangle_space * 3;

    newspace = dor_space_reallocate(trilst->vertex_location,
				    vertexcount*(sizeof (DtRealTriple)));
    if (newspace != NULL) 
	    trilst->vertex_location = (DtRealTriple *)newspace;
    else 
	    DDerror (ERR_CANT_ALLOC_MEM, 
			  "dor_trilst_add_triangle_space", "vertex_location");

    if (trilst->vertextype & DcLocNrm) {

   	newspace = dor_space_reallocate(trilst->vertex_normal,
					vertexcount*(sizeof (DtRealTriple)));
   	if (newspace != NULL) 
	    trilst->vertex_normal = (DtRealTriple *)newspace;
   	else 
	    DDerror (ERR_CANT_ALLOC_MEM, 
			  "dor_trilst_add_triangle_space", "vertex_normal");

    } 
    if (trilst->vertextype & DcLocClr) {

   	newspace = dor_space_reallocate(trilst->vertex_color,
					vertexcount*(sizeof (DtColorRGB)));
   	if (newspace != NULL) 
	    trilst->vertex_color = (DtColorRGB *)newspace;
   	else 
	    DDerror (ERR_CANT_ALLOC_MEM, 
		      "dor_trilst_add_triangle_space", "vertex_color");
    }
    if (trilst->vertextype & DcLocAlpha) {

   	newspace = dor_space_reallocate(trilst->vertex_alpha,
					vertexcount*(sizeof (DtReal)));
   	if (newspace != NULL) 
	    trilst->vertex_alpha = (DtReal *)newspace;
   	else 
	    DDerror (ERR_CANT_ALLOC_MEM, 
		      "dor_trilst_add_triangle_space", "vertex_alpha");
    }

    for (uv_idx=0; uv_idx< trilst->uv_count; uv_idx++) {
	newspace = dor_space_reallocate(trilst->uv_list[uv_idx],
					vertexcount*(sizeof (DtRealCouple)));
	if (newspace != NULL) 
	    trilst->uv_list[uv_idx] = (DtRealCouple *)newspace;
	else 
	    DDerror (ERR_CANT_ALLOC_MEM, 
		      "dor_trilst_add_triangle_space", "uv_list[uv_idx]");
    }

    for (uvw_idx=0; uvw_idx< trilst->uvw_count; uvw_idx++) {
	newspace= dor_space_reallocate( trilst->uvw_list[uvw_idx],
				       vertexcount*(sizeof (DtRealTriple)));
	if (newspace != NULL) 
	    trilst->uvw_list[uvw_idx] = (DtRealTriple *)newspace;
	else 
	    DDerror (ERR_CANT_ALLOC_MEM, 
		      "dor_trilst_add_triangle_space", "uvw_list[uvw_idx]");
    }
}
