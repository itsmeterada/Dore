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
	dor_linlst_initialize
	dor_linlst_create
	dor_linlst_destroy
	dor_linlst_print
	dor_linlst_add_line
	dor_linlst_add_line_space
	dor_linlst_bounding_box
	dor_linlst_close
	dor_linlst_compute_bounding_volume
	dor_linlst_open
	dor_linlst_pick

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/linlst.h>

DtInt doe_linlst_class_id;
DtFlag doe_linlst_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry linlst_methods[] =
{   { DcMethodCmpBndVolume, (DtMethodPtr) dor_linlst_compute_bounding_volume },
    { DcMethodDestroy,      (DtMethodPtr) dor_linlst_destroy },
    { DcMethodPick,         (DtMethodPtr) dor_linlst_pick },
    { DcMethodPrint,        (DtMethodPtr) dor_linlst_print }
};

void dor_linlst_initialize (void)
{
    doe_linlst_class_id = dor_class_install (DcTypeLineList, "DoLineList", 4,
					     linlst_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_linlst_create (
    DtColorModel colormodel,
    DtVertexType vertextype,
    DtInt linecount,
    DtReal *vertices)
{
    struct linlst *linlst;
    DtInt vertexcount;
    DtReal *vertexptr;
    DtInt vertexno;
    DtVertexType basevtxtype;
    DtInt uv;
    DtInt uvw;
    dot_object *linlstobject;

    if (!doe_linlst_initialized) {
   	dor_linlst_initialize();
   	doe_linlst_initialized = DcTrue;
    }

    linlst = (struct linlst *)dor_space_allocate(sizeof *linlst);
   
    linlst->colormodel = colormodel;

    linlst->line_count = linecount;

    basevtxtype = dod_system_inq_vertex_basetype(vertextype);
    linlst->vertextype = basevtxtype;

    vertexcount = linecount * 2;

    /*  Vertices always have locations.  */

    linlst->vertex_location = (DtRealTriple *)
	    dor_space_allocate(vertexcount*(sizeof (DtRealTriple)));

    /* texture coordinate lists */

    linlst->uv_list = (DtRealCouple **)
	    dor_space_allocate(dod_system_max_vertex_uv_cnt*sizeof(DtRealCouple *));
    linlst->uvw_list = (DtRealTriple **)
	    dor_space_allocate(dod_system_max_vertex_uvw_cnt*sizeof(DtRealTriple *));

    for (uv = 0; uv < dod_system_max_vertex_uv_cnt; uv++) {
	linlst->uv_list[uv] = NULL;
    }
    for (uvw = 0; uvw < dod_system_max_vertex_uvw_cnt; uvw++) {
	linlst->uvw_list[uvw] = NULL;
    }

    linlst->uv_count = dod_system_inq_vertex_uv_cnt(vertextype);
    linlst->uvw_count = dod_system_inq_vertex_uvw_cnt(vertextype);

    for (uv=0; uv< linlst->uv_count; uv++) {
       	linlst->uv_list[uv] = (DtRealCouple *)
		dor_space_allocate(vertexcount*(sizeof (DtRealCouple)));
    }

    for (uvw=0; uvw< linlst->uvw_count; uvw++) {
     	linlst->uvw_list[uvw] = (DtRealTriple *)
		dor_space_allocate(vertexcount*(sizeof (DtRealTriple)));
    }


    vertexptr = vertices;

    linlst->vertex_normal = (DtRealTriple *)NULL;
    linlst->vertex_color = (DtColorRGB *)NULL;
    linlst->vertex_alpha = (DtReal *)NULL;

PRAGMA(ASIS)
    if (basevtxtype & DcLocNrm)
        linlst->vertex_normal = (DtRealTriple *)
            dor_space_allocate(vertexcount*(sizeof (DtRealTriple)));

    if (basevtxtype & DcLocClr)
        linlst->vertex_color = (DtRealTriple *)
            dor_space_allocate(vertexcount*(sizeof (DtRealTriple)));

    if (basevtxtype & DcLocAlpha)
        linlst->vertex_alpha = (DtReal *)
            dor_space_allocate(vertexcount*(sizeof (DtReal)));

    for (vertexno=0; vertexno<vertexcount; vertexno++) {
        linlst->vertex_location[vertexno][0] = *vertexptr++;
        linlst->vertex_location[vertexno][1] = *vertexptr++;
        linlst->vertex_location[vertexno][2] = *vertexptr++;

        if (basevtxtype & DcLocNrm) {
            linlst->vertex_normal[vertexno][0] = *vertexptr++;
            linlst->vertex_normal[vertexno][1] = *vertexptr++;
            linlst->vertex_normal[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocClr) {
            linlst->vertex_color[vertexno][0] = *vertexptr++;
            linlst->vertex_color[vertexno][1] = *vertexptr++;
            linlst->vertex_color[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocAlpha) {
            linlst->vertex_alpha[vertexno] = *vertexptr++;
        }

        for (uv=0; uv < linlst->uv_count; uv++) {
            linlst->uv_list[uv][vertexno][0] = *vertexptr++;
            linlst->uv_list[uv][vertexno][1] = *vertexptr++;
        }
                             	 
        for (uvw=0; uvw < linlst->uvw_count; uvw++){
            linlst->uvw_list[uvw][vertexno][0] = *vertexptr++;
            linlst->uvw_list[uvw][vertexno][1] = *vertexptr++;
            linlst->uvw_list[uvw][vertexno][2] = *vertexptr++;
        } 
    }

    dor_linlst_bounding_box(linlst);
   
    linlstobject = dor_object_create(doe_linlst_class_id,linlst);

    return(linlstobject);
}
/*
 ======================================================================
 */

void dor_linlst_destroy (dot_object *object)
{
    struct linlst *linlst;
    int i;

    linlst = (struct linlst *)(object->data);

    if (linlst->vertex_location != NULL) {
	dor_space_deallocate(linlst->vertex_location);
    }

    if (linlst->vertex_normal != NULL) {
	dor_space_deallocate(linlst->vertex_normal);
    }

    if (linlst->vertex_color != NULL) {
	dor_space_deallocate(linlst->vertex_color);
    }

    if (linlst->vertex_alpha != NULL) {
	dor_space_deallocate(linlst->vertex_alpha);
    }

    for (i=0; i< linlst->uv_count; i++) {
	dor_space_deallocate(linlst->uv_list[i]);
    }
    dor_space_deallocate(linlst->uv_list);

    for (i=0; i< linlst->uvw_count; i++) {
	dor_space_deallocate(linlst->uvw_list[i]);
    }
    dor_space_deallocate(linlst->uvw_list);

    dor_space_deallocate(linlst);
}
/*
 ======================================================================
 */

void dor_linlst_print (dot_object *object)
{
    struct linlst *linlst;
    /*
      DtInt totalspace;
      DtInt vertexcount;
      */

    linlst = (struct linlst *)(object->data);

    if (linlst == NULL){
	dor_print_output("bad values");
	return;
    }

    switch (linlst->colormodel){
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;

    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    dor_utilpri_print_vertextype(linlst->vertextype);

    sprintf(dor_print_get_line(),"linecount = %d",linlst->line_count);
    dor_print_output(dor_print_get_line());

    if (linlst->uv_count > 0) {
	sprintf (dor_print_get_line(), "uv's per vertex = %d", linlst->uv_count);
	dor_print_output(dor_print_get_line());
    }

    if (linlst->uvw_count > 0) {
	sprintf (dor_print_get_line(), "uvw's per vertex = %d", linlst->uvw_count);
	dor_print_output(dor_print_get_line());
    }

    /*
      vertexcount = linlst->line_count * 2;
      
      printf("vertex location space = %d bytes\n",
      vertexcount*(sizeof (DtRealTriple)));
      
      totalspace = vertexcount*(sizeof (DtRealTriple));
      
      if (linlst->vertex_normal != NULL){
      printf("vertex normal space = %d bytes\n",
      vertexcount*(sizeof (DtRealTriple)));
      
      totalspace += vertexcount*(sizeof (DtRealTriple));
      }
      
      if (linlst->vertex_color != NULL){
      printf("vertex color space = %d bytes\n",
      vertexcount*(sizeof (DtColorRGB)));
      
      totalspace += vertexcount*(sizeof (DtRealTriple));
      }
      
      if (linlst->vertex_alpha != NULL){
      printf("vertex alpha space = %d bytes\n",
      vertexcount*(sizeof (DtReal)));
      
      totalspace += vertexcount*(sizeof (DtRealTriple));
      }
      
      printf("total space = %d bytes\n",totalspace);
      */
}
/*
 ======================================================================
 */

void dor_linlst_add_line (
    dot_object *object,
    DtReal *vertices)
{
    struct linlst *linlst;
    DtInt firstvertex;
    DtInt vertexno;
    DtInt uv;
    DtInt uvw;

    linlst = (struct linlst *)(object->data);

    if (linlst->line_count >= linlst->line_space) {
	dor_linlst_add_line_space(linlst);
    }

    firstvertex = linlst->line_count * 2;

    for (vertexno=firstvertex;vertexno<firstvertex+2;vertexno++) {
        linlst->vertex_location[vertexno][0] = *vertices++;
        linlst->vertex_location[vertexno][1] = *vertices++;
        linlst->vertex_location[vertexno][2] = *vertices++;

        if (linlst->vertextype & DcLocNrm) {
            linlst->vertex_normal[vertexno][0] = *vertices++;
            linlst->vertex_normal[vertexno][1] = *vertices++;
            linlst->vertex_normal[vertexno][2] = *vertices++;
        }

        if (linlst->vertextype & DcLocClr) {
            linlst->vertex_color[vertexno][0] = *vertices++;
            linlst->vertex_color[vertexno][1] = *vertices++;
            linlst->vertex_color[vertexno][2] = *vertices++;
        }

        if (linlst->vertextype & DcLocAlpha) {
            linlst->vertex_alpha[vertexno] = *vertices++;
        }

        for (uv=0; uv < linlst->uv_count; uv++) {
            linlst->uv_list[uv][vertexno][0] = *vertices++;
            linlst->uv_list[uv][vertexno][1] = *vertices++;
        }

        for (uvw=0; uvw < linlst->uvw_count; uvw++){
            linlst->uvw_list[uvw][vertexno][0] = *vertices++;
            linlst->uvw_list[uvw][vertexno][1] = *vertices++;
            linlst->uvw_list[uvw][vertexno][2] = *vertices++;
        }
    }

    linlst->line_count++;
}
/*
 ======================================================================
 */

void dor_linlst_add_line_space (struct linlst *linlst)
{
    DtInt vertexcount;
    DtPtr newspace;
    DtInt uv, uvw;

#ifdef DEBUG
    printf ("dor_linlst_add_line_space: from %d to ",
	    linlst->line_space);
#endif
    if (linlst->line_space == 0) {
	linlst->line_space = linlst->space_estimate;
    } else {
	linlst->line_space += (.2 * linlst->space_estimate) + 1;
    }
#ifdef DEBUG
    printf ("%d\n", linlst->line_space);
#endif

    vertexcount = linlst->line_space * 2;

    newspace = dor_space_reallocate
		(linlst->vertex_location,vertexcount*(sizeof (DtRealTriple)));
    if (newspace != NULL) 
	    linlst->vertex_location = (DtRealTriple *)newspace;
    else 
	    DDerror (ERR_CANT_ALLOC_MEM, "dor_linlst_add_line_space", 
			  "vertex_location");


    if (linlst->vertextype & DcLocNrm) {
	newspace = dor_space_reallocate
		(linlst->vertex_normal,vertexcount*(sizeof (DtRealTriple)));
	if (newspace != NULL)
		linlst->vertex_normal = (DtRealTriple *)newspace;
	else 
		DDerror (ERR_CANT_ALLOC_MEM, 
			      "dor_linlst_add_line_space", 
			      "vertex_normal");
    } 

    if (linlst->vertextype & DcLocClr) {
	newspace = dor_space_reallocate
		(linlst->vertex_color,vertexcount*(sizeof (DtColorRGB)));
	if (newspace != NULL)
		linlst->vertex_color = (DtColorRGB *)newspace;
	else
		DDerror (ERR_CANT_ALLOC_MEM, 
			      "dor_linlst_add_line_space", 
			      "vertex_color");
    }

    if (linlst->vertextype & DcLocAlpha) {
	newspace = dor_space_reallocate
		(linlst->vertex_color,vertexcount*(sizeof (DtReal)));
	if (newspace != NULL)
		linlst->vertex_alpha = (DtReal *)newspace;
	else
		DDerror (ERR_CANT_ALLOC_MEM, 
			      "dor_linlst_add_line_space", 
			      "vertex_alpha");
    }

    for (uv=0; uv< linlst->uv_count; uv++) {
	newspace = dor_space_reallocate(linlst->uv_list[uv],
					vertexcount*(sizeof (DtRealCouple)));
	if (newspace != NULL) 
		linlst->uv_list[uv] = (DtRealCouple *)newspace;
	else 
		DDerror (ERR_CANT_ALLOC_MEM, 
			      "dor_linlst_add_line_space",
			      "uv_list[uv]");
    }

    for (uvw=0; uvw< linlst->uvw_count; uvw++) {
	newspace= dor_space_reallocate
		(linlst->uvw_list[uvw],vertexcount*(sizeof (DtRealTriple)));
	if (newspace != NULL) 
		linlst->uvw_list[uvw] = (DtRealTriple *)newspace;
	else 
		DDerror (ERR_CANT_ALLOC_MEM, 
			      "dor_linlst_add_line_space",
			      "uvw_list[uvw]");
    }
}
/*
 ======================================================================
 */

void dor_linlst_bounding_box (struct linlst *linlst)
{
    DtReal xmin, ymin, zmin, xmax, ymax, zmax;
    DtInt vertexno;
    DtInt vertexcount;

    if (linlst->line_count == 0){
	xmin = 0.;
	ymin = 0.;
	zmin = 0.;

	xmax = 0.;
	ymax = 0.;
	zmax = 0.;
    }
    else{
	xmin = linlst->vertex_location[0][0];
	xmax = linlst->vertex_location[0][0];

	ymin = linlst->vertex_location[0][1];
	ymax = linlst->vertex_location[0][1];

	zmin = linlst->vertex_location[0][2];
	zmax = linlst->vertex_location[0][2];

	vertexcount = 2 * linlst->line_count;

	for (vertexno=1;vertexno<vertexcount;vertexno++){
	    xmin = min(xmin,linlst->vertex_location[vertexno][0]);
	    xmax = max(xmax,linlst->vertex_location[vertexno][0]);

	    ymin = min(ymin,linlst->vertex_location[vertexno][1]);
	    ymax = max(ymax,linlst->vertex_location[vertexno][1]);

	    zmin = min(zmin,linlst->vertex_location[vertexno][2]);
	    zmax = max(zmax,linlst->vertex_location[vertexno][2]);
	}
    }

    linlst->bounding_box_pts[0][0] = xmin;
    linlst->bounding_box_pts[0][1] = ymin;
    linlst->bounding_box_pts[0][2] = zmin;

    linlst->bounding_box_pts[1][0] = xmin;
    linlst->bounding_box_pts[1][1] = ymax;
    linlst->bounding_box_pts[1][2] = zmin;

    linlst->bounding_box_pts[2][0] = xmax;
    linlst->bounding_box_pts[2][1] = ymax;
    linlst->bounding_box_pts[2][2] = zmin;

    linlst->bounding_box_pts[3][0] = xmax;
    linlst->bounding_box_pts[3][1] = ymin;
    linlst->bounding_box_pts[3][2] = zmin;

    linlst->bounding_box_pts[4][0] = xmin;
    linlst->bounding_box_pts[4][1] = ymin;
    linlst->bounding_box_pts[4][2] = zmax;

    linlst->bounding_box_pts[5][0] = xmin;
    linlst->bounding_box_pts[5][1] = ymax;
    linlst->bounding_box_pts[5][2] = zmax;

    linlst->bounding_box_pts[6][0] = xmax;
    linlst->bounding_box_pts[6][1] = ymax;
    linlst->bounding_box_pts[6][2] = zmax;

    linlst->bounding_box_pts[7][0] = xmax;
    linlst->bounding_box_pts[7][1] = ymin;
    linlst->bounding_box_pts[7][2] = zmax;
}
/*
 ======================================================================
 */

void dor_linlst_close (dot_object *object)
{
    struct linlst *linlst;
    DtInt vertexcount;
    DtInt uv, uvw;

    linlst = (struct linlst *)(object->data);

    /*  Recompress the space used to store the vertex data to be exactly
	big enough.  */

    vertexcount = linlst->line_count * 2;

    if (vertexcount != 0) {

   	linlst->vertex_location = (DtRealTriple *)dor_space_reallocate
		(linlst->vertex_location,vertexcount*(sizeof (DtRealTriple)));

   	if (linlst->vertextype & DcLocNrm) 
	    linlst->vertex_normal = (DtRealTriple *)dor_space_reallocate
		(linlst->vertex_normal,vertexcount*(sizeof (DtRealTriple)));

   	if (linlst->vertextype & DcLocClr)
	    linlst->vertex_color = (DtColorRGB *)dor_space_reallocate
		(linlst->vertex_color,vertexcount*(sizeof (DtColorRGB)));

   	if (linlst->vertextype & DcLocAlpha)
	    linlst->vertex_alpha = (DtReal *)dor_space_reallocate
		(linlst->vertex_alpha,vertexcount*(sizeof (DtReal)));

  	for (uv=0; uv < linlst->uv_count; uv++) {
	    linlst->uv_list[uv] = (DtRealCouple *)dor_space_reallocate
		(linlst->uv_list[uv], vertexcount*(sizeof (DtRealCouple)));
	}
  	for (uvw=0; uvw < linlst->uvw_count; uvw++) {
	    linlst->uvw_list[uvw] = (DtRealTriple *)dor_space_reallocate
		(linlst->uvw_list[uvw], vertexcount*(sizeof (DtRealTriple)));
	}
    }

    dor_linlst_bounding_box(linlst);
}
/*
 ======================================================================
 */

void dor_linlst_compute_bounding_volume (dot_object *object)
{
    struct linlst *linlst;

    linlst = (struct linlst *)(object->data);

    dor_system_report_bounding_points (8, (DtReal*)linlst->bounding_box_pts);
}
/*
 ======================================================================
 */

dot_object *dor_linlst_open (
    DtColorModel colormodel,
    DtVertexType vertextype,
    DtInt space_estimate)
{
    DtVertexType basevtxtype;
    struct linlst *linlst;
    dot_object *linlstobject;
    int i;

    if (!doe_linlst_initialized) {
	dor_linlst_initialize();
	doe_linlst_initialized = DcTrue;
    }

    linlst = (struct linlst *)dor_space_allocate(sizeof *linlst);

    linlst->colormodel = colormodel;

    linlst->space_estimate = space_estimate < 1 ? 1 : space_estimate;

    linlst->line_count = 0;

    basevtxtype = dod_system_inq_vertex_basetype(vertextype);
    linlst->vertextype = basevtxtype;

    /* texture coordinates */

    linlst->uv_list = (DtRealCouple **)dor_space_allocate
		(dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
    linlst->uvw_list = (DtRealTriple **)dor_space_allocate
		(dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *));

    for (i = 0; i < dod_system_max_vertex_uv_cnt; i++) {
        linlst->uv_list[i] = NULL;
    }
    for (i = 0; i < dod_system_max_vertex_uvw_cnt; i++) {
        linlst->uvw_list[i] = NULL;
    }

    linlst->uv_count = dod_system_inq_vertex_uv_cnt(vertextype);
    linlst->uvw_count = dod_system_inq_vertex_uvw_cnt(vertextype);

    /**/

    linlst->line_space = 0;
    linlst->vertex_location = (DtRealTriple *)NULL;
    linlst->vertex_normal = (DtColorRGB *)NULL;
    linlst->vertex_color = (DtRealTriple *)NULL;
    linlst->vertex_alpha = (DtReal *)NULL;

    linlstobject = dor_object_create(doe_linlst_class_id,linlst);

    return(linlstobject);
}
/*
 ======================================================================
 */

void dor_linlst_pick (dot_object *object)
{
    struct linlst *linlst;

    if (!dor_condex_query_pick(doe_linlst_class_id))
	    return;

    if (!doe_system_alternate_object_execution_flag){
	dor_pick_initialize_object(object);
    }

    linlst = (struct linlst *)(object->data);

    dor_pick_line_list(linlst->bounding_box_pts,linlst->line_count,
		       linlst->vertex_location);
}
