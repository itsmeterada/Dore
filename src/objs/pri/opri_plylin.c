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
	dor_plylin_initialize
	dor_plylin_create
	dor_plylin_destroy
	dor_plylin_print
	dor_plylin_bounding_box
	dor_plylin_compute_bounding_volume
	dor_plylin_pick

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/plylin.h>

/*
 ======================================================================
 */

DtInt doe_plylin_class_id;

static DtMethodEntry plylin_methods[] = {
    { DcMethodCmpBndVolume, (DtMethodPtr) dor_plylin_compute_bounding_volume },
    { DcMethodDestroy,      (DtMethodPtr) dor_plylin_destroy },
    { DcMethodPick,         (DtMethodPtr) dor_plylin_pick },
    { DcMethodPrint,        (DtMethodPtr) dor_plylin_print }
};

void dor_plylin_initialize (void)
{
    doe_plylin_class_id = dor_class_install (DcTypePolyline, "DoPolyline", 
					     4, plylin_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_plylin_create (
    DtColorModel colormodel,
    DtVertexType vertextype,
    DtInt vertexcount,
    DtReal *vertices)
{
    static DtFlag initialized = DcFalse;
    struct plylin *plylin;
    DtReal *vertexptr;
    DtInt vertexno;
    DtVertexType basevtxtype;
    DtInt uv, uvw;
    dot_object *plylinobject;
   
    if (!initialized) {
   	dor_plylin_initialize();
   	initialized = DcTrue;
    }

    plylin = (struct plylin *)dor_space_allocate(sizeof *plylin);

    plylin->colormodel = colormodel;

    basevtxtype = dod_system_inq_vertex_basetype(vertextype);
    plylin->vertextype = basevtxtype;

    plylin->vertex_count = vertexcount;

    /*  Vertices always have locations.  */

    plylin->vertex_location = (DtRealTriple *)dor_space_allocate
		(vertexcount*(sizeof (DtRealTriple)));

    /* texture coordinates */

    plylin->uv_list = (DtRealCouple **)dor_space_allocate
		(dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
    plylin->uvw_list = (DtRealTriple **)dor_space_allocate
		(dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *));

    for (uv = 0; uv < dod_system_max_vertex_uv_cnt; uv++) {
        plylin->uv_list[uv] = NULL;
    }
    for (uvw = 0; uvw < dod_system_max_vertex_uvw_cnt; uvw++) {
        plylin->uvw_list[uvw] = NULL;
    }

    plylin->uv_count = dod_system_inq_vertex_uv_cnt(vertextype);
    plylin->uvw_count = dod_system_inq_vertex_uvw_cnt(vertextype);
 
    for (uv=0; uv< plylin->uv_count; uv++) {
        plylin->uv_list[uv] = (DtRealCouple *)dor_space_allocate
		(vertexcount*(sizeof (DtRealCouple)));
    }

    for (uvw=0; uvw< plylin->uvw_count; uvw++) {
        plylin->uvw_list[uvw] = (DtRealTriple *)dor_space_allocate
		(vertexcount*(sizeof (DtRealTriple)));
    }

    /**/
   
    vertexptr = vertices;

    plylin->vertex_normal = (DtRealTriple *)NULL;
    plylin->vertex_color = (DtColorRGB *)NULL;
    plylin->vertex_alpha = (DtReal *)NULL;


    if (basevtxtype & DcLocNrm)
        plylin->vertex_normal = (DtRealTriple *)
            dor_space_allocate(vertexcount*(sizeof (DtRealTriple)));

    if (basevtxtype & DcLocClr)
        plylin->vertex_color = (DtColorRGB *)
            dor_space_allocate(vertexcount*(sizeof (DtColorRGB)));

    if (basevtxtype & DcLocAlpha)
        plylin->vertex_alpha = (DtReal *)
            dor_space_allocate(vertexcount*(sizeof (DtReal)));

PRAGMA(ASIS)
    for (vertexno=0; vertexno<vertexcount; vertexno++) {
        plylin->vertex_location[vertexno][0] = *vertexptr++;
        plylin->vertex_location[vertexno][1] = *vertexptr++;
        plylin->vertex_location[vertexno][2] = *vertexptr++;

        if (basevtxtype & DcLocNrm) {
            plylin->vertex_normal[vertexno][0] = *vertexptr++;
            plylin->vertex_normal[vertexno][1] = *vertexptr++;
            plylin->vertex_normal[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocClr) {
            plylin->vertex_color[vertexno][0] = *vertexptr++;
            plylin->vertex_color[vertexno][1] = *vertexptr++;
            plylin->vertex_color[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocAlpha) {
            plylin->vertex_alpha[vertexno] = *vertexptr++;
        }

        for (uv=0; uv < plylin->uv_count; uv++) {
            plylin->uv_list[uv][vertexno][0] = *vertexptr++;
            plylin->uv_list[uv][vertexno][1] = *vertexptr++;
        }

        for (uvw=0; uvw < plylin->uvw_count; uvw++) {
            plylin->uvw_list[uvw][vertexno][0] = *vertexptr++;
            plylin->uvw_list[uvw][vertexno][1] = *vertexptr++;
            plylin->uvw_list[uvw][vertexno][2] = *vertexptr++;
        }
    }

    dor_plylin_bounding_box(plylin);

    plylinobject = dor_object_create(doe_plylin_class_id,plylin);

    return(plylinobject);
}
/*
 ======================================================================
 */

void dor_plylin_destroy (struct object *plylinobject)
{
    int i;
    struct plylin *plylin;

    plylin = (struct plylin *)plylinobject->data;

    if (plylin->vertex_location != NULL){
	dor_space_deallocate(plylin->vertex_location);
    }

    if (plylin->vertex_normal != NULL){
	dor_space_deallocate(plylin->vertex_normal);
    }

    if (plylin->vertex_color != NULL){
	dor_space_deallocate(plylin->vertex_color);
    }

    if (plylin->vertex_alpha != NULL){
	dor_space_deallocate(plylin->vertex_alpha);
    }

    for (i=0; i< plylin->uv_count; i++) {
	dor_space_deallocate(plylin->uv_list[i]);
    }
    dor_space_deallocate(plylin->uv_list);

    for (i=0; i< plylin->uvw_count; i++) {
	dor_space_deallocate(plylin->uvw_list[i]);
    }
    dor_space_deallocate(plylin->uvw_list);

    dor_space_deallocate(plylin);
}
/*
 ======================================================================
 */

void dor_plylin_print (struct object *plylinobject)
{
    struct plylin *plylin;

    plylin = (struct plylin *)(plylinobject->data);

    if (plylin == NULL){
	dor_print_output("polyline is NULL");
	return;
    }

    switch (plylin->colormodel){
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;
 
    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }
 
    dor_utilpri_print_vertextype(plylin->vertextype);

    sprintf(dor_print_get_line(),"vertex count = %d",plylin->vertex_count);
    dor_print_output(dor_print_get_line());

    if (plylin->uv_count > 0) {
	sprintf (dor_print_get_line(), "uv's per vertex = %d", plylin->uv_count);
	dor_print_output(dor_print_get_line());
    }

    if (plylin->uvw_count > 0) {
	sprintf (dor_print_get_line(), "uvw's per vertex = %d", plylin->uvw_count);
	dor_print_output(dor_print_get_line());
    }

}

/*
 ======================================================================
 */

void dor_plylin_bounding_box (struct plylin *plylin)
{
    DtReal xmin, ymin, zmin, xmax, ymax, zmax;
    DtInt vertexno;
    DtInt vertexcount;

    if (plylin->vertex_count == 0){
	xmin = 0.;
	ymin = 0.;
	zmin = 0.;

	xmax = 0.;
	ymax = 0.;
	zmax = 0.;
    }
    else{
	xmin = plylin->vertex_location[0][0];
	xmax = plylin->vertex_location[0][0];

	ymin = plylin->vertex_location[0][1];
	ymax = plylin->vertex_location[0][1];

	zmin = plylin->vertex_location[0][2];
	zmax = plylin->vertex_location[0][2];

	vertexcount = plylin->vertex_count;

	for (vertexno=1;vertexno<vertexcount;vertexno++){
	    xmin = min(xmin,plylin->vertex_location[vertexno][0]);
	    xmax = max(xmax,plylin->vertex_location[vertexno][0]);

	    ymin = min(ymin,plylin->vertex_location[vertexno][1]);
	    ymax = max(ymax,plylin->vertex_location[vertexno][1]);

	    zmin = min(zmin,plylin->vertex_location[vertexno][2]);
	    zmax = max(zmax,plylin->vertex_location[vertexno][2]);
	}
    }

    plylin->bounding_box_pts[0][0] = xmin;
    plylin->bounding_box_pts[0][1] = ymin;
    plylin->bounding_box_pts[0][2] = zmin;

    plylin->bounding_box_pts[1][0] = xmin;
    plylin->bounding_box_pts[1][1] = ymax;
    plylin->bounding_box_pts[1][2] = zmin;

    plylin->bounding_box_pts[2][0] = xmax;
    plylin->bounding_box_pts[2][1] = ymax;
    plylin->bounding_box_pts[2][2] = zmin;

    plylin->bounding_box_pts[3][0] = xmax;
    plylin->bounding_box_pts[3][1] = ymin;
    plylin->bounding_box_pts[3][2] = zmin;

    plylin->bounding_box_pts[4][0] = xmin;
    plylin->bounding_box_pts[4][1] = ymin;
    plylin->bounding_box_pts[4][2] = zmax;

    plylin->bounding_box_pts[5][0] = xmin;
    plylin->bounding_box_pts[5][1] = ymax;
    plylin->bounding_box_pts[5][2] = zmax;

    plylin->bounding_box_pts[6][0] = xmax;
    plylin->bounding_box_pts[6][1] = ymax;
    plylin->bounding_box_pts[6][2] = zmax;

    plylin->bounding_box_pts[7][0] = xmax;
    plylin->bounding_box_pts[7][1] = ymin;
    plylin->bounding_box_pts[7][2] = zmax;
}
/*
 ======================================================================
 */

void dor_plylin_compute_bounding_volume (dot_object *object)
{
    struct plylin *plylin;

    plylin = (struct plylin *)(object->data);

    dor_system_report_bounding_points
	(plylin->vertex_count, (DtReal*)plylin->vertex_location);
}
/*
 ======================================================================
 */

void dor_plylin_pick (dot_object *object)
{
    struct plylin *plylin;

    if (!dor_condex_query_pick(doe_plylin_class_id))
	    return;

    if (!doe_system_alternate_object_execution_flag){
	dor_pick_initialize_object(object);
    }

    plylin = (struct plylin *)(object->data);

    dor_pick_connected_line_list(plylin->bounding_box_pts,plylin->vertex_count,
				 plylin->vertex_location);
}
