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
	dor_pntlst_initialize
	dor_pntlst_create
	dor_pntlst_destroy
	dor_pntlst_print
	dor_pntlst_add_point
	dor_pntlst_add_point_space
	dor_pntlst_bounding_box
	dor_pntlst_close
	dor_pntlst_compute_bounding_volume
	dor_pntlst_open
	dor_pntlst_pick

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/pntlst.h>

DtInt doe_pntlst_class_id;
DtFlag doe_pntlst_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry pntlst_methods[] = {
    { DcMethodCmpBndVolume, (DtMethodPtr) dor_pntlst_compute_bounding_volume },
    { DcMethodDestroy,      (DtMethodPtr) dor_pntlst_destroy },
    { DcMethodPick,         (DtMethodPtr) dor_pntlst_pick },
    { DcMethodPrint,        (DtMethodPtr) dor_pntlst_print }
};

void dor_pntlst_initialize (void)
{
    doe_pntlst_class_id = dor_class_install (DcTypePointList, "DoPointList", 
					     4, pntlst_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_pntlst_create (
    DtColorModel colormodel,
    DtVertexType vertextype,
    DtInt pointcount,
    DtReal *vertices)
{
    struct pntlst *pntlst;
    DtInt vertexcount;
    DtReal *vertexptr;
    DtInt vertexno;
    DtVertexType basevtxtype;
    DtInt uv;
    DtInt uvw;
    dot_object *pntlstobject;

    if (!doe_pntlst_initialized) {
	dor_pntlst_initialize();
	doe_pntlst_initialized = DcTrue;
    }

    pntlst = (struct pntlst *)dor_space_allocate(sizeof *pntlst);

    pntlst->colormodel = colormodel;

    pntlst->point_count = pointcount;

    basevtxtype = dod_system_inq_vertex_basetype(vertextype);
    pntlst->vertextype = basevtxtype;

    vertexcount = pointcount;

    /*  Vertices always have locations.  */

    pntlst->vertex_location = (DtRealTriple *)dor_space_allocate
			(vertexcount*(sizeof (DtRealTriple)));

    /* Texture coordinates */

    pntlst->uv_list = (DtRealCouple **)dor_space_allocate
			(dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
    pntlst->uvw_list = (DtRealTriple **)dor_space_allocate
			(dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *));

    for (uv = 0; uv < dod_system_max_vertex_uv_cnt; uv++) {
	pntlst->uv_list[uv] = NULL;
    }
    for (uvw = 0; uvw < dod_system_max_vertex_uvw_cnt; uvw++) {
	pntlst->uvw_list[uvw] = NULL;
    }

    pntlst->uv_count = dod_system_inq_vertex_uv_cnt(vertextype);
    pntlst->uvw_count = dod_system_inq_vertex_uvw_cnt(vertextype);

    for (uv=0; uv< pntlst->uv_count; uv++) {
	pntlst->uv_list[uv] = (DtRealCouple *)dor_space_allocate
			(vertexcount*(sizeof (DtRealCouple)));
    }
 
    for (uvw=0; uvw< pntlst->uvw_count; uvw++) {
	pntlst->uvw_list[uvw] = (DtRealTriple *)dor_space_allocate
			(vertexcount*(sizeof (DtRealTriple)));
    }

    /**/

    vertexptr = vertices;

    pntlst->vertex_normal = (DtRealTriple *)NULL;
    pntlst->vertex_color = (DtColorRGB *)NULL;
    pntlst->vertex_alpha = (DtReal *)NULL;

    if (basevtxtype & DcLocNrm)
        pntlst->vertex_normal = (DtRealTriple *)
            dor_space_allocate(vertexcount*(sizeof (DtRealTriple)));

    if (basevtxtype & DcLocClr)
        pntlst->vertex_color = (DtColorRGB *)
            dor_space_allocate(vertexcount*(sizeof (DtColorRGB)));

    if (basevtxtype & DcLocAlpha)
        pntlst->vertex_alpha = (DtReal *)
            dor_space_allocate(vertexcount*(sizeof (DtReal)));

PRAGMA(ASIS)
    for (vertexno=0; vertexno<vertexcount; vertexno++) {
        pntlst->vertex_location[vertexno][0] = *vertexptr++;
        pntlst->vertex_location[vertexno][1] = *vertexptr++;
        pntlst->vertex_location[vertexno][2] = *vertexptr++;

        if (basevtxtype & DcLocNrm) {
            pntlst->vertex_normal[vertexno][0] = *vertexptr++;
            pntlst->vertex_normal[vertexno][1] = *vertexptr++;
            pntlst->vertex_normal[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocClr) {
            pntlst->vertex_color[vertexno][0] = *vertexptr++;
            pntlst->vertex_color[vertexno][1] = *vertexptr++;
            pntlst->vertex_color[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocAlpha) {
            pntlst->vertex_alpha[vertexno] = *vertexptr++;
        }

        for (uv=0; uv < pntlst->uv_count; uv++) {
            pntlst->uv_list[uv][vertexno][0] = *vertexptr++;
            pntlst->uv_list[uv][vertexno][1] = *vertexptr++;
        }

        for (uvw=0; uvw < pntlst->uvw_count; uvw++) {
            pntlst->uvw_list[uvw][vertexno][0] = *vertexptr++;
            pntlst->uvw_list[uvw][vertexno][1] = *vertexptr++;
            pntlst->uvw_list[uvw][vertexno][2] = *vertexptr++;
        }
    }

    dor_pntlst_bounding_box(pntlst);

    pntlstobject = dor_object_create(doe_pntlst_class_id,pntlst);

    return(pntlstobject);
}
/*
 ======================================================================
 */

void dor_pntlst_destroy (dot_object *object)
{
    int i;
    struct pntlst *pntlst;

    pntlst = (struct pntlst *)(object->data);

    if (pntlst->vertex_location != NULL) {
	dor_space_deallocate(pntlst->vertex_location);
    }

    if (pntlst->vertex_normal != NULL) {
	dor_space_deallocate(pntlst->vertex_normal);
    }

    if (pntlst->vertex_color != NULL) {
	dor_space_deallocate(pntlst->vertex_color);
    }

    if (pntlst->vertex_alpha != NULL) {
	dor_space_deallocate(pntlst->vertex_alpha);
    }

    for (i=0; i< pntlst->uv_count; i++) {
	dor_space_deallocate(pntlst->uv_list[i]);
    }
    dor_space_deallocate(pntlst->uv_list);

    for (i=0; i< pntlst->uvw_count; i++) {
	dor_space_deallocate(pntlst->uvw_list[i]);
    }
    dor_space_deallocate(pntlst->uvw_list);

    dor_space_deallocate(pntlst);
}
/*
 ======================================================================
 */

void dor_pntlst_print (dot_object *object)
{
    struct pntlst *pntlst;
    /*
      DtInt totalspace;
      DtInt vertexcount;
      */

    pntlst = (struct pntlst *)(object->data);

    if (pntlst == NULL){
	dor_print_output("bad values");
	return;
    }

    switch (pntlst->colormodel){
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;

    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    dor_utilpri_print_vertextype(pntlst->vertextype);

    sprintf(dor_print_get_line(),"pointcount = %d",pntlst->point_count);
    dor_print_output(dor_print_get_line());

    if (pntlst->uv_count > 0) {
	sprintf (dor_print_get_line(), 
		 "uv's per vertex = %d", pntlst->uv_count);
	dor_print_output(dor_print_get_line());
    }

    if (pntlst->uvw_count > 0) {
	sprintf (dor_print_get_line(), 
		 "uvw's per vertex = %d", pntlst->uvw_count);
	dor_print_output(dor_print_get_line());
    }

    /*
      vertexcount = pntlst->point_count;
      
      printf("vertex location space = %d bytes\n",
      vertexcount*(sizeof (DtRealTriple)));
      
      totalspace = vertexcount*(sizeof (DtRealTriple));
      
      if (pntlst->vertex_normal != NULL){
      printf("vertex normal space = %d bytes\n",
      vertexcount*(sizeof (DtRealTriple)));
      
      totalspace += vertexcount*(sizeof (DtRealTriple));
      }
      
      if (pntlst->vertex_color != NULL){
      printf("vertex color space = %d bytes\n",
      vertexcount*(sizeof (DtRealTriple)));
      
      totalspace += vertexcount*(sizeof (DtRealTriple));
      }
      
      printf("total space = %d bytes\n",totalspace);
      */
}
/*
 ======================================================================
 */

void dor_pntlst_add_point (
    dot_object *object,
    DtReal *vertices)
{
    struct pntlst *pntlst;
    DtInt firstvertex;
    DtInt vertexno;
    DtInt uv, uvw;

    pntlst = (struct pntlst *)(object->data);

    if (pntlst->point_count >= pntlst->point_space) {
   	dor_pntlst_add_point_space(pntlst);
    }

    firstvertex = pntlst->point_count;

    for (vertexno=firstvertex;vertexno<firstvertex+1;vertexno++) {
        pntlst->vertex_location[vertexno][0] = *vertices++;
        pntlst->vertex_location[vertexno][1] = *vertices++;
        pntlst->vertex_location[vertexno][2] = *vertices++;

        if (pntlst->vertextype & DcLocNrm) {
            pntlst->vertex_normal[vertexno][0] = *vertices++;
            pntlst->vertex_normal[vertexno][1] = *vertices++;
            pntlst->vertex_normal[vertexno][2] = *vertices++;
        }
        if (pntlst->vertextype & DcLocClr) {
            pntlst->vertex_color[vertexno][0] = *vertices++;
            pntlst->vertex_color[vertexno][1] = *vertices++;
            pntlst->vertex_color[vertexno][2] = *vertices++;
        }
        if (pntlst->vertextype & DcLocAlpha) {
            pntlst->vertex_alpha[vertexno] = *vertices++;
        }

        for (uv=0; uv < pntlst->uv_count; uv++) {
            pntlst->uv_list[uv][vertexno][0] = *vertices++;
            pntlst->uv_list[uv][vertexno][1] = *vertices++;
        }

        for (uvw=0; uvw < pntlst->uvw_count; uvw++){
            pntlst->uvw_list[uvw][vertexno][0] = *vertices++;
            pntlst->uvw_list[uvw][vertexno][1] = *vertices++;
            pntlst->uvw_list[uvw][vertexno][2] = *vertices++;
        }
    }

    pntlst->point_count++;
}
/*
 ======================================================================
 */

void dor_pntlst_add_point_space (struct pntlst *pntlst)
{
    DtInt vertexcount;
    DtInt uv, uvw;
    DtPtr newspace;

    pntlst->point_space += 2000;

    vertexcount = pntlst->point_space;

    newspace = dor_space_reallocate( pntlst->vertex_location,
				    vertexcount*(sizeof (DtRealTriple)));

    if (newspace != NULL)
	pntlst->vertex_location = (DtRealTriple *)newspace;
    else
	DDerror (ERR_CANT_ALLOC_MEM,
		 "dor_pntlst_add_point_space", "vertex_location");

    if (pntlst->vertextype & DcLocNrm) {
 
        newspace = dor_space_reallocate(pntlst->vertex_normal,
					vertexcount*(sizeof (DtRealTriple)));
        if (newspace != NULL)
	    pntlst->vertex_normal = (DtRealTriple *)newspace;
        else
	    DDerror (ERR_CANT_ALLOC_MEM,
		     "dor_pntlst_add_point_space", "vertex_normal");

    } 
    if (pntlst->vertextype & DcLocClr) {

        newspace = dor_space_reallocate(pntlst->vertex_color,
					vertexcount*(sizeof (DtColorRGB)));
        if (newspace != NULL)
	    pntlst->vertex_color = (DtColorRGB *)newspace;
        else
	    DDerror (ERR_CANT_ALLOC_MEM,
		     "dor_pntlst_add_point_space", "vertex_color");
    }
    if (pntlst->vertextype & DcLocAlpha) {

        newspace = dor_space_reallocate(pntlst->vertex_alpha,
					vertexcount*(sizeof (DtReal)));
        if (newspace != NULL)
	    pntlst->vertex_alpha = (DtReal *)newspace;
        else
	    DDerror (ERR_CANT_ALLOC_MEM,
		     "dor_pntlst_add_point_space", "vertex_alpha");
    }

    for (uv=0; uv< pntlst->uv_count; uv++) {
	newspace = dor_space_reallocate(pntlst->uv_list[uv],
					vertexcount*(sizeof (DtRealCouple)));
	if (newspace != NULL)  
	    pntlst->uv_list[uv] = (DtRealCouple *)newspace;
	else 
	    DDerror (ERR_CANT_ALLOC_MEM,
		     "dor_pntlst_add_point_space", "uv_list[uv]");
    }
 
    for (uvw=0; uvw< pntlst->uvw_count; uvw++) {
	newspace= dor_space_reallocate( pntlst->uvw_list[uvw],
				       vertexcount*(sizeof (DtRealTriple)));
	if (newspace != NULL)
	    pntlst->uvw_list[uvw] = (DtRealTriple *)newspace;
	else
	    DDerror (ERR_CANT_ALLOC_MEM,
		     "dor_pntlst_add_point_space", "uvw_list[uvw]");
    }
	
}
/*
 ======================================================================
 */

void dor_pntlst_bounding_box (struct pntlst *pntlst)
{
    DtReal xmin, ymin, zmin, xmax, ymax, zmax;
    DtInt vertexno;

    if (pntlst->point_count == 0){
	xmin = 0.;
	ymin = 0.;
	zmin = 0.;

	xmax = 0.;
	ymax = 0.;
	zmax = 0.;
    }
    else{
	xmin = pntlst->vertex_location[0][0];
	xmax = pntlst->vertex_location[0][0];

	ymin = pntlst->vertex_location[0][1];
	ymax = pntlst->vertex_location[0][1];

	zmin = pntlst->vertex_location[0][2];
	zmax = pntlst->vertex_location[0][2];

	for (vertexno=1;vertexno<pntlst->point_count;vertexno++){
	    xmin = min(xmin,pntlst->vertex_location[vertexno][0]);
	    xmax = max(xmax,pntlst->vertex_location[vertexno][0]);

	    ymin = min(ymin,pntlst->vertex_location[vertexno][1]);
	    ymax = max(ymax,pntlst->vertex_location[vertexno][1]);

	    zmin = min(zmin,pntlst->vertex_location[vertexno][2]);
	    zmax = max(zmax,pntlst->vertex_location[vertexno][2]);
	}
    }

    pntlst->bounding_box_pts[0][0] = xmin;
    pntlst->bounding_box_pts[0][1] = ymin;
    pntlst->bounding_box_pts[0][2] = zmin;

    pntlst->bounding_box_pts[1][0] = xmin;
    pntlst->bounding_box_pts[1][1] = ymax;
    pntlst->bounding_box_pts[1][2] = zmin;

    pntlst->bounding_box_pts[2][0] = xmax;
    pntlst->bounding_box_pts[2][1] = ymax;
    pntlst->bounding_box_pts[2][2] = zmin;

    pntlst->bounding_box_pts[3][0] = xmax;
    pntlst->bounding_box_pts[3][1] = ymin;
    pntlst->bounding_box_pts[3][2] = zmin;

    pntlst->bounding_box_pts[4][0] = xmin;
    pntlst->bounding_box_pts[4][1] = ymin;
    pntlst->bounding_box_pts[4][2] = zmax;

    pntlst->bounding_box_pts[5][0] = xmin;
    pntlst->bounding_box_pts[5][1] = ymax;
    pntlst->bounding_box_pts[5][2] = zmax;

    pntlst->bounding_box_pts[6][0] = xmax;
    pntlst->bounding_box_pts[6][1] = ymax;
    pntlst->bounding_box_pts[6][2] = zmax;

    pntlst->bounding_box_pts[7][0] = xmax;
    pntlst->bounding_box_pts[7][1] = ymin;
    pntlst->bounding_box_pts[7][2] = zmax;
}
/*
 ======================================================================
 */

void dor_pntlst_close (dot_object *object)
{
    struct pntlst *pntlst;
    DtInt vertexcount;
    DtInt uv, uvw;

    pntlst = (struct pntlst *)(object->data);

    /*  Recompress the space used to store the vertex data to be exactly
	big enough.  */

    vertexcount = pntlst->point_count;

    if (vertexcount != 0) {

   	pntlst->vertex_location = (DtRealTriple *)dor_space_reallocate
		(pntlst->vertex_location,vertexcount*(sizeof (DtRealTriple)));

   	if (pntlst->vertextype & DcLocNrm) 
	    pntlst->vertex_normal = (DtRealTriple *)dor_space_reallocate
		(pntlst->vertex_normal,vertexcount*(sizeof (DtRealTriple)));

   	if (pntlst->vertextype & DcLocClr) 
	    pntlst->vertex_color = (DtColorRGB *)dor_space_reallocate
		(pntlst->vertex_color,vertexcount*(sizeof (DtColorRGB)));

   	if (pntlst->vertextype & DcLocAlpha) 
	    pntlst->vertex_alpha = (DtReal *)dor_space_reallocate
		(pntlst->vertex_alpha,vertexcount*(sizeof (DtReal)));

	for (uv=0; uv < pntlst->uv_count; uv++) { 
	    pntlst->uv_list[uv] = (DtRealCouple *)dor_space_reallocate
		(pntlst->uv_list[uv], vertexcount*(sizeof (DtRealCouple)));      
        } 
        for (uvw=0; uvw < pntlst->uvw_count; uvw++) {
	    pntlst->uvw_list[uvw] = (DtRealTriple *)dor_space_reallocate
		(pntlst->uvw_list[uvw], vertexcount*(sizeof (DtRealTriple)));
        }
    }

    dor_pntlst_bounding_box(pntlst);
}
/*
 ======================================================================
 */

void dor_pntlst_compute_bounding_volume (dot_object *object)
{
    struct pntlst *pntlst;

    pntlst = (struct pntlst *)(object->data);

    dor_system_report_bounding_points (8, (DtReal*)pntlst->bounding_box_pts);
}
/*
 ======================================================================
 */

dot_object *dor_pntlst_open (
    DtColorModel colormodel,
    DtVertexType vertextype)
{
    DtVertexType basevtxtype;
    int i;
    struct pntlst *pntlst;
    dot_object *pntlstobject;

    if (!doe_pntlst_initialized) {
	dor_pntlst_initialize();
	doe_pntlst_initialized = DcTrue;
    }

    pntlst = (struct pntlst *)dor_space_allocate(sizeof *pntlst);

    pntlst->colormodel = colormodel;

    pntlst->point_count = 0;

    basevtxtype = dod_system_inq_vertex_basetype(vertextype);
    pntlst->vertextype = basevtxtype;

    /* texture coordinates */
 
    pntlst->uv_list = (DtRealCouple **)dor_space_allocate
			(dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
    pntlst->uvw_list = (DtRealTriple **)dor_space_allocate
			(dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *));
 
    for (i = 0; i < dod_system_max_vertex_uv_cnt; i++) {
        pntlst->uv_list[i] = NULL;
    }
    for (i = 0; i < dod_system_max_vertex_uvw_cnt; i++) {
        pntlst->uvw_list[i] = NULL;
    }
 
    pntlst->uv_count = dod_system_inq_vertex_uv_cnt(vertextype);
    pntlst->uvw_count = dod_system_inq_vertex_uvw_cnt(vertextype);
 
    /**/ 

    pntlst->point_space = 0;
    pntlst->vertex_location = NULL;
    pntlst->vertex_normal = NULL;
    pntlst->vertex_color = NULL;

    pntlstobject = dor_object_create(doe_pntlst_class_id,pntlst);

    return(pntlstobject);
}
/*
 ======================================================================
 */

void dor_pntlst_pick (dot_object *object)
{
    struct pntlst *pntlst;

    if (!dor_condex_query_pick(doe_pntlst_class_id))
	    return;

    if (!doe_system_alternate_object_execution_flag){
	dor_pick_initialize_object(object);
    }

    pntlst = (struct pntlst *)(object->data);

    dor_pick_point_list(pntlst->bounding_box_pts,pntlst->point_count,
			pntlst->vertex_location);
}
