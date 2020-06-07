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
	dor_ply_initialize
	dor_ply_create
	dor_ply_destroy
	dor_ply_print
	dor_ply_update_alternate
	dor_ply_create_alternate_geometry_complex_triangle
	dor_ply_create_alternate_geom_lines
	dor_ply_create_alternate_geometry_simple_triangle
	dor_ply_create_alternate_geom_tris
	dor_ply_compute_bounding_volume
	dor_ply_enable_edges_complex
	dor_ply_check_complex_edge
	dor_ply_enable_edges_simple
	dor_ply_pick
	dor_ply_pick_lines
	dor_ply_pick_tris
	dor_ply_render_standard
	dor_ply_update_lines
	dor_ply_update_tris

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/gpolyd.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/ply.h>
#include <dore/dore_develop/private/linlst.h>
#include <dore/dore_develop/private/trilst.h>

/*
 ======================================================================
 */

DtInt doe_ply_class_id;

static DtMethodEntry ply_methods[] = {
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_ply_compute_bounding_volume },
    { DcMethodDestroy,          (DtMethodPtr) dor_ply_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_ply_pick },
    { DcMethodPrint,            (DtMethodPtr) dor_ply_print },
    { DcMethodUpdStdAltObj,     (DtMethodPtr) dor_ply_update_alternate },
    { DcMethodUpdStdAltLineObj, (DtMethodPtr) dor_ply_update_lines },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_ply_render_standard }
};

void dor_ply_initialize (void)
{
    doe_ply_class_id = dor_class_install
		       (DcTypePolygon, "DoPolygon", 7, ply_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_ply_create (
    DtColorModel colormodel,
    DtVertexType vertextype,
    DtInt contourcount,
    DtInt *contours,
    DtReal *vertices,
    DtShapeType shape)
{
    static DtFlag initialized = DcFalse;
    struct ply *ply;
    DtInt vertexcount;
    DtInt contourno;
    DtReal *vertexptr;
    DtInt vertexno;
    DtVertexType basevtxtype;
    DtInt uv, uvw;
    dot_object *plyobject;

    if (!initialized) {
      	dor_ply_initialize();
      	initialized = DcTrue;
    }

    ply = dor_space_allocate (sizeof *ply);

    ply->colormodel = colormodel;

    basevtxtype = dod_system_inq_vertex_basetype(vertextype);
    ply->vertextype = basevtxtype;
    ply->fullvtxtype = vertextype;

    ply->contour_count = contourcount;

    ply->contours = dor_space_allocate ((contourcount) * sizeof(DtInt));

    /*  Total number of vertices is given by the contour length counts in
	contours.  */

    vertexcount = 0;

    for (contourno=0;contourno<contourcount;contourno++) {
   	ply->contours[contourno] = contours[contourno];
   	vertexcount += contours[contourno];
    }

    ply->vertex_count = vertexcount;

    ply->vertex_location =
	dor_space_allocate (vertexcount * (sizeof (DtRealTriple)));

    ply->vertex_normal = (DtRealTriple *)NULL;
    ply->vertex_color  = (DtColorRGB *)NULL;
    ply->vertex_alpha  = (DtReal *)NULL;

    /* texture coordinates */

    ply->uv_list = dor_space_allocate
		(dod_system_max_vertex_uv_cnt * sizeof (DtRealCouple *));
    ply->uvw_list = dor_space_allocate
		(dod_system_max_vertex_uvw_cnt * sizeof (DtRealTriple *));

    for (uv = 0; uv < dod_system_max_vertex_uv_cnt; uv++) {
        ply->uv_list[uv] = DcNullPtr;
    }
    for (uvw = 0; uvw < dod_system_max_vertex_uvw_cnt; uvw++) {
        ply->uvw_list[uvw] = DcNullPtr;
    }

    ply->uv_count  = dod_system_inq_vertex_uv_cnt  (vertextype);
    ply->uvw_count = dod_system_inq_vertex_uvw_cnt (vertextype);

    for (uv=0; uv < ply->uv_count; uv++) {
        ply->uv_list[uv] =
	    dor_space_allocate (vertexcount * (sizeof (DtRealCouple)));
    }

    for (uvw=0; uvw< ply->uvw_count; uvw++) {
        ply->uvw_list[uvw] =
	    dor_space_allocate (vertexcount * (sizeof (DtRealTriple)));
    }

    vertexptr = vertices;

    if (basevtxtype & DcLocNrm)
        ply->vertex_normal =
            dor_space_allocate (vertexcount * (sizeof (DtRealTriple)));

    if (basevtxtype & DcLocClr)
        ply->vertex_color =
            dor_space_allocate (vertexcount * (sizeof (DtRealTriple)));

    if (basevtxtype & DcLocAlpha)
        ply->vertex_alpha =
	    dor_space_allocate (vertexcount * (sizeof (DtReal)));

    for (vertexno=0; vertexno<vertexcount; vertexno++) {
        ply->vertex_location[vertexno][0] = *vertexptr++;
        ply->vertex_location[vertexno][1] = *vertexptr++;
        ply->vertex_location[vertexno][2] = *vertexptr++;

        if (basevtxtype & DcLocNrm) {
            ply->vertex_normal[vertexno][0] = *vertexptr++;
            ply->vertex_normal[vertexno][1] = *vertexptr++;
            ply->vertex_normal[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocClr) {
            ply->vertex_color[vertexno][0] = *vertexptr++;
            ply->vertex_color[vertexno][1] = *vertexptr++;
            ply->vertex_color[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocAlpha) {
            ply->vertex_alpha[vertexno] = *vertexptr++;
        }

        for (uv=0; uv < ply->uv_count; uv++) {
            ply->uv_list[uv][vertexno][0] = *vertexptr++;
            ply->uv_list[uv][vertexno][1] = *vertexptr++;
        }

        for (uvw=0; uvw < ply->uvw_count; uvw++){
            ply->uvw_list[uvw][vertexno][0] = *vertexptr++;
            ply->uvw_list[uvw][vertexno][1] = *vertexptr++;
            ply->uvw_list[uvw][vertexno][2] = *vertexptr++;
        }
    }

    ply->shape = shape;

    ply->alt_object_lines = DcNullPtr;
    ply->alt_object_tris  = DcNullPtr;

    plyobject = dor_object_create (doe_ply_class_id,ply);

    return plyobject;
}
/*
 ======================================================================
 */

void dor_ply_destroy (dot_object *object)
{
    register int i;
    register struct ply *ply;

    ply = (struct ply *)(object->data);

    if (ply->vertex_location) dor_space_deallocate (ply->vertex_location);
    if (ply->vertex_normal)   dor_space_deallocate (ply->vertex_normal);
    if (ply->vertex_color)    dor_space_deallocate (ply->vertex_color);
    if (ply->vertex_alpha)    dor_space_deallocate (ply->vertex_alpha);

    for (i=0; i< ply->uv_count; i++) {
	dor_space_deallocate (ply->uv_list[i]);
    }
    dor_space_deallocate (ply->uv_list);

    for (i=0; i< ply->uvw_count; i++) {
	dor_space_deallocate (ply->uvw_list[i]);
    }
    dor_space_deallocate (ply->uvw_list);

    if (ply->contours)
	dor_space_deallocate(ply->contours);

    if (ply->alt_object_lines)
	dor_object_check_deletion (ply->alt_object_lines);

    if (ply->alt_object_tris)
	dor_object_check_deletion (ply->alt_object_tris);

    dor_space_deallocate (ply);
}
/*
 ======================================================================
 */

void dor_ply_print (struct object *plyobject)
{
    struct ply *ply;

    ply = (struct ply *)(plyobject->data);

    if (!ply) {
	dor_print_output ("ply is NULL");
	return;
    }

    dor_print_output ((ply->colormodel == DcRGB) ? "colormodel = DcRGB"
						 : "colormodel = Unknown");

    dor_utilpri_print_vertextype(ply->vertextype);

    dor_print_output
    (   (ply->shape == DcConvex)  ? "shape = convex"  :
	(ply->shape == DcConcave) ? "shape = concave" :
	(ply->shape == DcComplex) ? "shape = complex" : "shape = Unknown"
    );

    sprintf(dor_print_get_line(),
	    "vertex count = %d",ply->vertex_count);
    dor_print_output(dor_print_get_line());

    if (ply->uv_count > 0) {
	sprintf (dor_print_get_line(), 
		 "uv's per vertex = %d", ply->uv_count);
	dor_print_output(dor_print_get_line());
    }

    if (ply->uvw_count > 0) {
	sprintf (dor_print_get_line(), 
		 "uvw's per vertex = %d", ply->uvw_count);
	dor_print_output(dor_print_get_line());
    }
}
/*
 ======================================================================
 */

dot_object *dor_ply_update_alternate (dot_object *object)
{
    dot_object *alt;
    DtRepType reptype;

    dor_global_reptyp_get_value(&reptype);
 
    switch(reptype) {
	case DcSurface:
	case DcSurfaceWireframeEdges:
	case DcSurfaceOutlineEdges:
	case DcWireframe:
	case DcPoints:
	    alt = dor_ply_update_tris(object);
	    break;

	case DcOutlines:
	    alt = dor_ply_update_lines(object);
	    break;

	default:
	    DDerror (ERR_VALUE_OUT_OF_RANGE,
		     "dor_ply_update_alternate", "unknown reptype");
	    alt = DcNullPtr;
	    break;
    }
    return alt;
}
/*
 ======================================================================
 */

#define MAXVTXS_CT (27 + dod_system_max_vertex_uv_cnt*6 + \
                        dod_system_max_vertex_uvw_cnt*9)

void dor_ply_create_alternate_geometry_complex_triangle (
    struct ply *ply,
    dot_gpolyd_interpolated_vertex *gpolydvtx1,
    dot_gpolyd_interpolated_vertex *gpolydvtx2,
    dot_gpolyd_interpolated_vertex *gpolydvtx3)
{
   DtVertexType vertextype;
   DtReal *vtxptr;
   DtReal vertices[MAXVTXS_CT];
   DtInt vertex1, vertex2;
   DtReal weight1, weight2;
   DtInt uv, uvw;
   DtInt edge_enable;

   edge_enable = dor_ply_enable_edges_complex(ply,gpolydvtx1,gpolydvtx2,gpolydvtx3);

   vertextype = ply->vertextype;

    vtxptr = vertices;

    *vtxptr++ = gpolydvtx1->coord[0];
    *vtxptr++ = gpolydvtx1->coord[1];
    *vtxptr++ = gpolydvtx1->coord[2];

    vertex1 = gpolydvtx1->vertex1;

    if (gpolydvtx1->interpolated) {
        vertex2 = gpolydvtx1->vertex2;

        weight1 = gpolydvtx1->weight1;
        weight2 = gpolydvtx1->weight2;

        if (vertextype & DcLocNrm) {
            *vtxptr++ = weight1*ply->vertex_normal[vertex1][0] +
                        weight2*ply->vertex_normal[vertex2][0];
            *vtxptr++ = weight1*ply->vertex_normal[vertex1][1] +
                        weight2*ply->vertex_normal[vertex2][1];
            *vtxptr++ = weight1*ply->vertex_normal[vertex1][2] +
                        weight2*ply->vertex_normal[vertex2][2];
        }

        if (vertextype & DcLocClr) {
            *vtxptr++ = weight1*ply->vertex_color[vertex1][0] +
                        weight2*ply->vertex_color[vertex2][0];
            *vtxptr++ = weight1*ply->vertex_color[vertex1][1] +
                        weight2*ply->vertex_color[vertex2][1];
            *vtxptr++ = weight1*ply->vertex_color[vertex1][2] +
                        weight2*ply->vertex_color[vertex2][2];
        }

        if (vertextype & DcLocAlpha) {
            *vtxptr++ = weight1*ply->vertex_alpha[vertex1] +
                        weight2*ply->vertex_alpha[vertex2];
        }

        for (uv=0; uv < ply->uv_count; uv++) {
            *vtxptr++ = weight1*ply->uv_list[uv][vertex1][0] +
                        weight2*ply->uv_list[uv][vertex2][0];
            *vtxptr++ = weight1*ply->uv_list[uv][vertex1][1] +
                        weight2*ply->uv_list[uv][vertex2][1];
        }

        for (uvw=0; uvw < ply->uvw_count; uvw++) {
            *vtxptr++ = weight1*ply->uvw_list[uvw][vertex1][0] +
                        weight2*ply->uvw_list[uvw][vertex2][0];
            *vtxptr++ = weight1*ply->uvw_list[uvw][vertex1][1] +
                        weight2*ply->uvw_list[uvw][vertex2][1];
            *vtxptr++ = weight1*ply->uvw_list[uvw][vertex1][2] +
                        weight2*ply->uvw_list[uvw][vertex2][2];
        }

    } else {

        if (vertextype & DcLocNrm) {
            *vtxptr++ = ply->vertex_normal[vertex1][0];
            *vtxptr++ = ply->vertex_normal[vertex1][1];
            *vtxptr++ = ply->vertex_normal[vertex1][2];
        }

        if (vertextype & DcLocClr) {
            *vtxptr++ = ply->vertex_color[vertex1][0];
            *vtxptr++ = ply->vertex_color[vertex1][1];
            *vtxptr++ = ply->vertex_color[vertex1][2];
        }

        if (vertextype & DcLocAlpha) {
            *vtxptr++ = ply->vertex_alpha[vertex1];
        }

        for (uv=0; uv < ply->uv_count; uv++) {
               *vtxptr++ = ply->uv_list[uv][vertex1][0];
               *vtxptr++ = ply->uv_list[uv][vertex1][1];
        }
        for (uvw=0; uvw < ply->uvw_count; uvw++) {
               *vtxptr++ = ply->uvw_list[uvw][vertex1][0];
               *vtxptr++ = ply->uvw_list[uvw][vertex1][1];
               *vtxptr++ = ply->uvw_list[uvw][vertex1][2];
        }
    }

    *vtxptr++ = gpolydvtx2->coord[0];
    *vtxptr++ = gpolydvtx2->coord[1];
    *vtxptr++ = gpolydvtx2->coord[2];

    vertex1 = gpolydvtx2->vertex1;

    if (gpolydvtx2->interpolated) {
        vertex2 = gpolydvtx2->vertex2;

        weight1 = gpolydvtx2->weight1;
        weight2 = gpolydvtx2->weight2;

        if (vertextype & DcLocNrm) {
            *vtxptr++ = weight1*ply->vertex_normal[vertex1][0] +
                        weight2*ply->vertex_normal[vertex2][0];
            *vtxptr++ = weight1*ply->vertex_normal[vertex1][1] +
                        weight2*ply->vertex_normal[vertex2][1];
       	    *vtxptr++ = weight1*ply->vertex_normal[vertex1][2] +
                        weight2*ply->vertex_normal[vertex2][2];
        }

        if (vertextype & DcLocClr) {
            *vtxptr++ = weight1*ply->vertex_color[vertex1][0] +
                        weight2*ply->vertex_color[vertex2][0];
            *vtxptr++ = weight1*ply->vertex_color[vertex1][1] +
                        weight2*ply->vertex_color[vertex2][1];
       	    *vtxptr++ = weight1*ply->vertex_color[vertex1][2] +
                        weight2*ply->vertex_color[vertex2][2];
        }

        if (vertextype & DcLocAlpha) {
            *vtxptr++ = weight1*ply->vertex_alpha[vertex1] +
                        weight2*ply->vertex_alpha[vertex2];
        }

        for (uv=0; uv < ply->uv_count; uv++) {
            *vtxptr++ = weight1*ply->uv_list[uv][vertex1][0] +
                        weight2*ply->uv_list[uv][vertex2][0];
            *vtxptr++ = weight1*ply->uv_list[uv][vertex1][1] +
                        weight2*ply->uv_list[uv][vertex2][1];
        }

        for (uvw=0; uvw < ply->uvw_count; uvw++) {
            *vtxptr++ = weight1*ply->uvw_list[uvw][vertex1][0] +
                        weight2*ply->uvw_list[uvw][vertex2][0];
            *vtxptr++ = weight1*ply->uvw_list[uvw][vertex1][1] +
                        weight2*ply->uvw_list[uvw][vertex2][1];
            *vtxptr++ = weight1*ply->uvw_list[uvw][vertex1][2] +
                        weight2*ply->uvw_list[uvw][vertex2][2];
        }

    } else {
        if (vertextype & DcLocNrm) { 
           *vtxptr++ = ply->vertex_normal[vertex1][0];
           *vtxptr++ = ply->vertex_normal[vertex1][1];
           *vtxptr++ = ply->vertex_normal[vertex1][2];
        }

        if (vertextype & DcLocClr) {
           *vtxptr++ = ply->vertex_color[vertex1][0];
           *vtxptr++ = ply->vertex_color[vertex1][1];
           *vtxptr++ = ply->vertex_color[vertex1][2];
        }

        if (vertextype & DcLocAlpha) {
           *vtxptr++ = ply->vertex_alpha[vertex1];
        }

        for (uv=0; uv < ply->uv_count; uv++) {
               *vtxptr++ = ply->uv_list[uv][vertex1][0];
               *vtxptr++ = ply->uv_list[uv][vertex1][1];
        }
        for (uvw=0; uvw < ply->uvw_count; uvw++) {
               *vtxptr++ = ply->uvw_list[uvw][vertex1][0];
               *vtxptr++ = ply->uvw_list[uvw][vertex1][1];
               *vtxptr++ = ply->uvw_list[uvw][vertex1][2];
        }
    }

    *vtxptr++ = gpolydvtx3->coord[0];
    *vtxptr++ = gpolydvtx3->coord[1];
    *vtxptr++ = gpolydvtx3->coord[2];

    vertex1 = gpolydvtx3->vertex1;

    if (gpolydvtx3->interpolated) {
        vertex2 = gpolydvtx3->vertex2;

        weight1 = gpolydvtx3->weight1;
        weight2 = gpolydvtx3->weight2;

        if (vertextype & DcLocNrm) {
            *vtxptr++ = weight1*ply->vertex_normal[vertex1][0] +
                        weight2*ply->vertex_normal[vertex2][0];
            *vtxptr++ = weight1*ply->vertex_normal[vertex1][1] +
                        weight2*ply->vertex_normal[vertex2][1];
            *vtxptr++ = weight1*ply->vertex_normal[vertex1][2] +
                        weight2*ply->vertex_normal[vertex2][2];
        }

        if (vertextype & DcLocClr) {
            *vtxptr++ = weight1*ply->vertex_color[vertex1][0] +
                        weight2*ply->vertex_color[vertex2][0];
            *vtxptr++ = weight1*ply->vertex_color[vertex1][1] +
                        weight2*ply->vertex_color[vertex2][1];
            *vtxptr++ = weight1*ply->vertex_color[vertex1][2] +
                        weight2*ply->vertex_color[vertex2][2];
        }

        if (vertextype & DcLocAlpha) {
            *vtxptr++ = weight1*ply->vertex_alpha[vertex1] +
                        weight2*ply->vertex_alpha[vertex2];
        }

        for (uv=0; uv < ply->uv_count; uv++) {
            *vtxptr++ = weight1*ply->uv_list[uv][vertex1][0] +
                       weight2*ply->uv_list[uv][vertex2][0];
            *vtxptr++ = weight1*ply->uv_list[uv][vertex1][1] +
                       weight2*ply->uv_list[uv][vertex2][1];
        }
        for (uvw=0; uvw < ply->uvw_count; uvw++) {
            *vtxptr++ = weight1*ply->uvw_list[uvw][vertex1][0] +
                       weight2*ply->uvw_list[uvw][vertex2][0];
            *vtxptr++ = weight1*ply->uvw_list[uvw][vertex1][1] +
                       weight2*ply->uvw_list[uvw][vertex2][1];
            *vtxptr++ = weight1*ply->uvw_list[uvw][vertex1][2] +
                       weight2*ply->uvw_list[uvw][vertex2][2];
        }

    } else {
        if (vertextype & DcLocNrm) {
           *vtxptr++ = ply->vertex_normal[vertex1][0];
           *vtxptr++ = ply->vertex_normal[vertex1][1];
           *vtxptr++ = ply->vertex_normal[vertex1][2];
        }

        if (vertextype & DcLocClr) {
           *vtxptr++ = ply->vertex_color[vertex1][0];
           *vtxptr++ = ply->vertex_color[vertex1][1];
           *vtxptr++ = ply->vertex_color[vertex1][2];
        }

        if (vertextype & DcLocAlpha) {
           *vtxptr++ = ply->vertex_alpha[vertex1];
        }

        for (uv=0; uv < ply->uv_count; uv++) {
               *vtxptr++ = ply->uv_list[uv][vertex1][0];
               *vtxptr++ = ply->uv_list[uv][vertex1][1];
        }

        for (uvw=0; uvw < ply->uvw_count; uvw++) {
               *vtxptr++ = ply->uvw_list[uvw][vertex1][0];
               *vtxptr++ = ply->uvw_list[uvw][vertex1][1];
               *vtxptr++ = ply->uvw_list[uvw][vertex1][2];
        }
    }

    dor_trilst_add_triangle_with_edges(ply->alt_object_tris,vertices,edge_enable);

}

/*
 ======================================================================
 */

#define MAXVTXS_GL (18 + dod_system_max_vertex_uv_cnt*4 + \
                        dod_system_max_vertex_uvw_cnt*6)

dot_object *dor_ply_create_alternate_geom_lines (struct ply *ply)
{
    DtReal vtxs[MAXVTXS_GL];
    DtInt firstvertex;
    DtInt lastvertex;
    DtUInt i;
    DtUInt uv, uvw;
    DtInt vtxno;
    DtInt lnvtx;
    DtInt contourno;
    DtInt contourvertexno;
    dot_object *linlstobj;
    DtInt sum;

    for (i=0,sum=0; i<ply->contour_count;i++) {
	sum += ply->contours[i];
    }

    linlstobj = dor_linlst_open(ply->colormodel, ply->vertextype |
	dor_system_texture_uv_count(ply->uv_count) |
	dor_system_texture_uvw_count(ply->uvw_count), sum);

    vtxno=0;
    for (contourno=0; contourno<ply->contour_count; contourno++) {

	firstvertex = vtxno;
	lastvertex = firstvertex + ply->contours[contourno] - 1;

	for (contourvertexno=0; 
	     contourvertexno<ply->contours[contourno]-1;
             contourvertexno++) {	

	    i = 0;
	    for (lnvtx=0; lnvtx<2; lnvtx++) {

		vtxs[i++] = ply->vertex_location[vtxno+lnvtx][0];
		vtxs[i++] = ply->vertex_location[vtxno+lnvtx][1];
		vtxs[i++] = ply->vertex_location[vtxno+lnvtx][2];

		if (ply->vertextype & DcLocNrm) {
		    vtxs[i++] = ply->vertex_normal[vtxno+lnvtx][0];
		    vtxs[i++] = ply->vertex_normal[vtxno+lnvtx][1];
		    vtxs[i++] = ply->vertex_normal[vtxno+lnvtx][2];
		}
                if (ply->vertextype & DcLocClr) {
		    vtxs[i++] = ply->vertex_color[vtxno+lnvtx][0];
		    vtxs[i++] = ply->vertex_color[vtxno+lnvtx][1];
		    vtxs[i++] = ply->vertex_color[vtxno+lnvtx][2];
		}

                if (ply->vertextype & DcLocAlpha) {
		    vtxs[i++] = ply->vertex_alpha[vtxno+lnvtx];
		}

		for (uv=0; uv<ply->uv_count; uv++) {
		    vtxs[i++] = ply->uv_list[uv][vtxno+lnvtx][0]; 
		    vtxs[i++] = ply->uv_list[uv][vtxno+lnvtx][1]; 
		}

		for (uvw=0; uvw<ply->uvw_count; uvw++) {
		    vtxs[i++] = ply->uvw_list[uvw][vtxno+lnvtx][0]; 
		    vtxs[i++] = ply->uvw_list[uvw][vtxno+lnvtx][1]; 
		    vtxs[i++] = ply->uvw_list[uvw][vtxno+lnvtx][2]; 
		}
	    }

	    vtxno++;
	    dor_linlst_add_line(linlstobj,vtxs);	
   	}
	vtxno++;

	/* close contour */
	i = 0;

	/* One end of last line */

	vtxs[i++] = ply->vertex_location[firstvertex][0];
	vtxs[i++] = ply->vertex_location[firstvertex][1];
	vtxs[i++] = ply->vertex_location[firstvertex][2];

	if (ply->vertextype & DcLocNrm) {
	    vtxs[i++] = ply->vertex_normal[firstvertex][0];
	    vtxs[i++] = ply->vertex_normal[firstvertex][1];
	    vtxs[i++] = ply->vertex_normal[firstvertex][2];
	}

	if (ply->vertextype & DcLocClr) {
	    vtxs[i++] = ply->vertex_color[firstvertex][0];
	    vtxs[i++] = ply->vertex_color[firstvertex][1];
	    vtxs[i++] = ply->vertex_color[firstvertex][2];
	}

	if (ply->vertextype & DcLocAlpha) {
	    vtxs[i++] = ply->vertex_alpha[firstvertex];
	}

	for (uv=0; uv<ply->uv_count; uv++) {
	    vtxs[i++] = ply->uv_list[uv][firstvertex][0]; 
	    vtxs[i++] = ply->uv_list[uv][firstvertex][1]; 
        }

        for (uvw=0; uvw<ply->uvw_count; uvw++) {
	    vtxs[i++] = ply->uvw_list[uvw][firstvertex][0]; 
	    vtxs[i++] = ply->uvw_list[uvw][firstvertex][1]; 
	    vtxs[i++] = ply->uvw_list[uvw][firstvertex][2]; 
        }

	/* Other end of last line */

	vtxs[i++] = ply->vertex_location[lastvertex][0];
	vtxs[i++] = ply->vertex_location[lastvertex][1];
	vtxs[i++] = ply->vertex_location[lastvertex][2];

	if (ply->vertextype & DcLocNrm) {
	    vtxs[i++] = ply->vertex_normal[lastvertex][0];
	    vtxs[i++] = ply->vertex_normal[lastvertex][1];
	    vtxs[i++] = ply->vertex_normal[lastvertex][2];
	}

	if (ply->vertextype & DcLocClr) {
	    vtxs[i++] = ply->vertex_color[lastvertex][0];
	    vtxs[i++] = ply->vertex_color[lastvertex][1];
	    vtxs[i++] = ply->vertex_color[lastvertex][2];
	}

	if (ply->vertextype & DcLocAlpha) {
	    vtxs[i++] = ply->vertex_alpha[lastvertex];
	}

	for (uv=0; uv<ply->uv_count; uv++) {
	    vtxs[i++] = ply->uv_list[uv][lastvertex][0]; 
	    vtxs[i++] = ply->uv_list[uv][lastvertex][1]; 
        }

        for (uvw=0; uvw<ply->uvw_count; uvw++) {
	    vtxs[i++] = ply->uvw_list[uvw][lastvertex][0]; 
	    vtxs[i++] = ply->uvw_list[uvw][lastvertex][1]; 
	    vtxs[i++] = ply->uvw_list[uvw][lastvertex][2]; 
        }

	/* Add last line to list */

	dor_linlst_add_line(linlstobj,vtxs);	
    }

    dor_linlst_close(linlstobj);
    return(linlstobj);
}
/*
 ======================================================================
 */

#define MAXVTXS_ST (27 + dod_system_max_vertex_uv_cnt*6 + \
			dod_system_max_vertex_uvw_cnt*9)

void dor_ply_create_alternate_geometry_simple_triangle (
    struct ply *ply,
    DtInt vertex1,
    DtInt vertex2,
    DtInt vertex3)
{
   DtVertexType vertextype;
   DtReal *vtxptr;
   DtReal vertices[MAXVTXS_ST];
   DtInt uv, uvw;
   DtInt edge_enable;

   edge_enable = dor_ply_enable_edges_simple(ply,vertex1,vertex2,vertex3);

   vertextype = ply->vertextype;

   vtxptr = vertices;

   /* First vertex */

   *vtxptr++ = ply->vertex_location[vertex1][0];
   *vtxptr++ = ply->vertex_location[vertex1][1];
   *vtxptr++ = ply->vertex_location[vertex1][2];

   if (vertextype & DcLocNrm) {
       *vtxptr++ = ply->vertex_normal[vertex1][0];
       *vtxptr++ = ply->vertex_normal[vertex1][1];
       *vtxptr++ = ply->vertex_normal[vertex1][2];
   }

   if (vertextype & DcLocClr) {
       *vtxptr++ = ply->vertex_color[vertex1][0];
       *vtxptr++ = ply->vertex_color[vertex1][1];
       *vtxptr++ = ply->vertex_color[vertex1][2];
   }

   if (vertextype & DcLocAlpha) {
       *vtxptr++ = ply->vertex_alpha[vertex1];
   }

   for (uv=0; uv<ply->uv_count; uv++) {
       *vtxptr++ = ply->uv_list[uv][vertex1][0]; 
       *vtxptr++ = ply->uv_list[uv][vertex1][1]; 
   }

   for (uvw=0; uvw<ply->uvw_count; uvw++) {
       *vtxptr++ = ply->uvw_list[uvw][vertex1][0]; 
       *vtxptr++ = ply->uvw_list[uvw][vertex1][1]; 
       *vtxptr++ = ply->uvw_list[uvw][vertex1][2]; 
   }

    /* Second vertex */

   *vtxptr++ = ply->vertex_location[vertex2][0];
   *vtxptr++ = ply->vertex_location[vertex2][1];
   *vtxptr++ = ply->vertex_location[vertex2][2];

   if (vertextype & DcLocNrm) {
       *vtxptr++ = ply->vertex_normal[vertex2][0];
       *vtxptr++ = ply->vertex_normal[vertex2][1];
       *vtxptr++ = ply->vertex_normal[vertex2][2];
   }

   if (vertextype & DcLocClr) {
       *vtxptr++ = ply->vertex_color[vertex2][0];
       *vtxptr++ = ply->vertex_color[vertex2][1];
       *vtxptr++ = ply->vertex_color[vertex2][2];
   }

   if (vertextype & DcLocAlpha) {
       *vtxptr++ = ply->vertex_alpha[vertex2];
   }

   for (uv=0; uv<ply->uv_count; uv++) {
        *vtxptr++ = ply->uv_list[uv][vertex2][0]; 
        *vtxptr++ = ply->uv_list[uv][vertex2][1]; 
   }

   for (uvw=0; uvw<ply->uvw_count; uvw++) {
       *vtxptr++ = ply->uvw_list[uvw][vertex2][0]; 
       *vtxptr++ = ply->uvw_list[uvw][vertex2][1]; 
       *vtxptr++ = ply->uvw_list[uvw][vertex2][2]; 
   }

    /* Third vertex */

   *vtxptr++ = ply->vertex_location[vertex3][0];
   *vtxptr++ = ply->vertex_location[vertex3][1];
   *vtxptr++ = ply->vertex_location[vertex3][2];

   if (vertextype & DcLocNrm) {
       *vtxptr++ = ply->vertex_normal[vertex3][0];
       *vtxptr++ = ply->vertex_normal[vertex3][1];
       *vtxptr++ = ply->vertex_normal[vertex3][2];
   }

   if (vertextype & DcLocClr) {
       *vtxptr++ = ply->vertex_color[vertex3][0];
       *vtxptr++ = ply->vertex_color[vertex3][1];
       *vtxptr++ = ply->vertex_color[vertex3][2];
   }

   if (vertextype & DcLocAlpha) {
       *vtxptr++ = ply->vertex_alpha[vertex3];
   }

   for (uv=0; uv<ply->uv_count; uv++) {
       *vtxptr++ = ply->uv_list[uv][vertex3][0]; 
       *vtxptr++ = ply->uv_list[uv][vertex3][1]; 
   }

   for (uvw=0; uvw<ply->uvw_count; uvw++) {
       *vtxptr++ = ply->uvw_list[uvw][vertex3][0]; 
       *vtxptr++ = ply->uvw_list[uvw][vertex3][1]; 
       *vtxptr++ = ply->uvw_list[uvw][vertex3][2]; 
   }

   /* Add triangle */

   dor_trilst_add_triangle_with_edges(ply->alt_object_tris,vertices, edge_enable);

}

/*
 ======================================================================
 */

dot_object *dor_ply_create_alternate_geom_tris (struct ply *ply)
{
    dot_object *trilstobj;
    DtInt vertexno;
    DtInt contourno;
    DtInt contourvertexno;
    DtInt i, sum;

    for (i=0,sum=0; i<ply->contour_count;i++) {
      	sum += ply->contours[i];
    }
    if (ply->shape == DcComplex) sum *= 2;

    trilstobj = dor_trilst_open(DcRGB,ply->fullvtxtype,sum);

    ply->alt_object_tris = trilstobj;

    dor_gpolyd_create_polygon();

    vertexno = 0;

    for (contourno=0;contourno<ply->contour_count;contourno++) {
   	dor_gpolyd_create_contour();
   
   	for (contourvertexno=0;
   	     contourvertexno<ply->contours[contourno];
   	     contourvertexno++) {
	    dor_gpolyd_create_contour_vertex(vertexno,
					     ply->vertex_location[vertexno][0],
					     ply->vertex_location[vertexno][1],
					     ply->vertex_location[vertexno][2]);

	    vertexno++;
   	}
   
   	dor_gpolyd_close_contour();
    }

    dor_gpolyd_close_polygon();

    dor_gpolyd_triangulate_polygon(ply->shape, 
				   dor_ply_create_alternate_geometry_simple_triangle,
				   dor_ply_create_alternate_geometry_complex_triangle,
				   ply);

    dor_trilst_close(trilstobj);

    return(trilstobj);
}
/*
 ======================================================================
 */

void dor_ply_compute_bounding_volume (dot_object *object)
{
    struct ply *ply;

    ply = (struct ply *)(object->data);

    dor_system_report_bounding_points
	(ply->vertex_count, (DtReal*)ply->vertex_location);
}
/*
 ======================================================================
 */

DtInt dor_ply_enable_edges_complex (
    struct ply *ply,
    dot_gpolyd_interpolated_vertex *gpolydvtx1,
    dot_gpolyd_interpolated_vertex *gpolydvtx2,
    dot_gpolyd_interpolated_vertex *gpolydvtx3)
{
    DtInt edge_enable;

    edge_enable = dod_no_enabled_edges;
    if (dor_ply_check_complex_edge(ply, gpolydvtx1, gpolydvtx2))
	    edge_enable |= dod_edge1_enabled;
    if (dor_ply_check_complex_edge(ply, gpolydvtx2, gpolydvtx3))
	    edge_enable |= dod_edge2_enabled;
    if (dor_ply_check_complex_edge(ply, gpolydvtx3, gpolydvtx1))
	    edge_enable |= dod_edge3_enabled;

#ifdef DEBUG
    printf("edge_enable %d\n", edge_enable);
#endif

    return (edge_enable);
}

DtFlag dor_ply_check_complex_edge (
    struct ply *ply,
    dot_gpolyd_interpolated_vertex *vtx1,
    dot_gpolyd_interpolated_vertex *vtx2)
{
    DtInt vertexcount;
    DtInt vtx1_contourno, vtx2_contourno;
    DtInt vtx1_no, vtx2_no;
    DtInt contours_vtxcnt;
    DtInt i;

#ifdef DEBUG
    printf("dor_ply_check_complex_edge\n");
    printf("vtx1 %d %d   vtx2 %d %d\n", vtx1->vertex1, vtx1->vertex2, vtx2->vertex1, vtx2->vertex2);
#endif
    if (vtx1->interpolated && vtx2->interpolated) {
#ifdef DEBUG
	printf("both interpolated\n");
#endif
        if ((vtx1->vertex1 == vtx2->vertex1 ||
             vtx1->vertex1 == vtx2->vertex2) &&
            (vtx1->vertex2 == vtx2->vertex1 ||
             vtx1->vertex2 == vtx2->vertex2))
		return (DcTrue);

    } else if (vtx1->interpolated) {
#ifdef DEBUG
	printf("vtx1 interpolated\n");
#endif
        if (vtx2->vertex1 == vtx1->vertex1 ||
            vtx2->vertex1 == vtx1->vertex2)
		return (DcTrue); 
  
    } else if (vtx2->interpolated) {
#ifdef DEBUG
	printf("vtx2 interpolated\n");
#endif
        if (vtx1->vertex1 == vtx2->vertex1 ||
            vtx1->vertex1 == vtx2->vertex2)
		return (DcTrue);

    } else {

	/* Figure out which contours the vertices belong to and where
	   they are in each contour */

   	vtx1_contourno = 0;
   	vtx1_no = vtx1->vertex1;
   	contours_vtxcnt = ply->contours[0];
   	i = 1;
   	while (vtx1->vertex1 >= contours_vtxcnt) {
	    vtx1_no = vtx1->vertex1 - contours_vtxcnt;
	    contours_vtxcnt += ply->contours[i++];
	    vtx1_contourno++;
   	}

   	vtx2_contourno = 0;
   	vtx2_no = vtx2->vertex1;
   	contours_vtxcnt = ply->contours[0];
   	i = 1;
   	while (vtx2->vertex1 >= contours_vtxcnt) {
	    vtx2_no = vtx2->vertex1 - contours_vtxcnt;
	    contours_vtxcnt += ply->contours[i++];
	    vtx2_contourno++;
   	}
#ifdef DEBUG 
 	printf("vtx1_contourno %d vtx1_no %d\n",vtx1_contourno,vtx1_no);
 	printf("vtx2_contourno %d vtx2_no %d\n",vtx2_contourno,vtx2_no);
#endif

  	if (vtx1_contourno == vtx2_contourno) {
	    vertexcount = ply->contours[vtx1_contourno];
	    if (vtx1_no == ((vtx2_no+1) % vertexcount) ||
		vtx2_no == ((vtx1_no+1) % vertexcount)) 
		    return (DcTrue);
	}
    }

    return (DcFalse);
	
}
/*
 ======================================================================
 */

DtInt dor_ply_enable_edges_simple (
    struct ply *ply,
    DtInt vertex1,
    DtInt vertex2,
    DtInt vertex3)
{
    DtInt edge_enable;
    DtInt vertexcount;
    DtInt vtx1_contourno, vtx2_contourno, vtx3_contourno;
    DtInt vtx1_no, vtx2_no, vtx3_no;
    DtInt contours_vtxcnt;
    DtInt i;

    /* Figure out which contours the vertices belong to and where
       they are in each contour */

    vtx1_contourno = 0;
    vtx1_no = vertex1;
    contours_vtxcnt = ply->contours[0];
    i = 1;
    while (vertex1 >= contours_vtxcnt) {
	vtx1_no = vertex1 - contours_vtxcnt;
	contours_vtxcnt += ply->contours[i++];
	vtx1_contourno++;
    }

    vtx2_contourno = 0;
    vtx2_no = vertex2;
    contours_vtxcnt = ply->contours[0];
    i = 1;
    while (vertex2 >= contours_vtxcnt) {
	vtx2_no = vertex2 - contours_vtxcnt;
	contours_vtxcnt += ply->contours[i++];
	vtx2_contourno++;
    }

    vtx3_contourno = 0;
    vtx3_no = vertex3;
    contours_vtxcnt = ply->contours[0];
    i = 1;
    while (vertex3 >= contours_vtxcnt) {
	vtx3_no = vertex3 - contours_vtxcnt;
	contours_vtxcnt += ply->contours[i++];
	vtx3_contourno++;
    }

    /* Enable appropriate edges */
    edge_enable = dod_no_enabled_edges;

    if (vtx1_contourno == vtx2_contourno) {
	vertexcount = ply->contours[vtx1_contourno];
	if (vtx1_no == ((vtx2_no+1) % vertexcount) ||
	    vtx2_no == ((vtx1_no+1) % vertexcount)) {
	    edge_enable |= dod_edge1_enabled;
	}
    }

    if (vtx2_contourno == vtx3_contourno) {
	vertexcount = ply->contours[vtx2_contourno];
	if (vtx2_no == ((vtx3_no+1) % vertexcount) ||
	    vtx3_no == ((vtx2_no+1) % vertexcount)) {
	    edge_enable |= dod_edge2_enabled;
	}
    }

    if (vtx3_contourno == vtx1_contourno) {
	vertexcount = ply->contours[vtx3_contourno];
	if (vtx3_no == ((vtx1_no+1) % vertexcount) ||
	    vtx1_no == ((vtx3_no+1) % vertexcount)) {
	    edge_enable |= dod_edge3_enabled;
	}
    }
  
    return (edge_enable);
}
/*
 ======================================================================
 */

void dor_ply_pick (dot_object *object)
{
    DtRepType reptype;

    if (!dor_condex_query_pick(doe_ply_class_id))
	    return;

    dor_pick_initialize_object(object);

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    case DcOutlines:
	dor_ply_pick_lines(object);
	break;

    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
    case DcWireframe:
    case DcPoints:
	dor_ply_pick_tris(object);
	break;
    }
}
/*
 ======================================================================
 */

void dor_ply_pick_lines (dot_object *object)
{
    struct ply *ply;

    ply = (struct ply *)(object->data);

    if (ply->alt_object_lines != DcNullPtr) {
   	doe_system_alternate_object_execution_flag = DcTrue;

   	dor_object_get_method(ply->alt_object_lines,
			      DcMethodPick)(ply->alt_object_lines);

   	doe_system_alternate_object_execution_flag = DcFalse;

   	return;
    } else {
   	ply->alt_object_lines = 
		dor_ply_create_alternate_geom_lines(ply);

   	if (ply->alt_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_ply_pick_lines", "");
	    return;
   	}

   	doe_system_alternate_object_execution_flag = DcTrue;

   	dor_object_get_method(ply->alt_object_lines,
			      DcMethodPick)(ply->alt_object_lines);

   	doe_system_alternate_object_execution_flag = DcFalse;

   	return;
    }
}
/*
 ======================================================================
 */

void dor_ply_pick_tris (dot_object *object)
{
    struct ply *ply;

    ply = (struct ply *)(object->data);

    if (ply->alt_object_tris != DcNullPtr) {
   	doe_system_alternate_object_execution_flag = DcTrue;

   	dor_object_get_method(ply->alt_object_tris,
			      DcMethodPick)(ply->alt_object_tris);

   	doe_system_alternate_object_execution_flag = DcFalse;

   	return;
    } else {
   	ply->alt_object_tris = 
		dor_ply_create_alternate_geom_tris(ply);

   	if (ply->alt_object_tris == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_ply_pick_tris", "");
	    return;
   	}

   	doe_system_alternate_object_execution_flag = DcTrue;

   	dor_object_get_method(ply->alt_object_tris,
			      DcMethodPick)(ply->alt_object_tris);

   	doe_system_alternate_object_execution_flag = DcFalse;

   	return;
    }
}
/*
 ======================================================================
 */

void dor_ply_render_standard (dot_object *object)
{
    dot_object *alt;

    if (!dor_condex_query_render(doe_ply_class_id))
	    return;

    alt = dor_ply_update_alternate (object);
    if (alt != DcNullPtr) {
	dor_object_get_current_method(alt) (alt);
    }
}
/*
 ======================================================================
 */

dot_object *dor_ply_update_lines (dot_object *object)
{
    struct ply *ply;
    dot_object *alt;

    ply = (struct ply *)(object->data);

    if (ply->alt_object_lines != DcNullPtr) {
	alt = ply->alt_object_lines;
    } else {
	ply->alt_object_lines = dor_ply_create_alternate_geom_lines(ply);
	alt = ply->alt_object_lines;

	if (ply->alt_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_ply_update_lines", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */

dot_object *dor_ply_update_tris (dot_object *object)
{
    struct ply *ply;
    dot_object *alt;

    ply = (struct ply *)(object->data);

    if (ply->alt_object_tris != DcNullPtr) {
	alt = ply->alt_object_tris;
    } else {
	ply->alt_object_tris = dor_ply_create_alternate_geom_tris(ply);
	alt = ply->alt_object_tris;

	if (ply->alt_object_tris == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_ply_update_tris", "");
	}
    }
    return alt;
}
