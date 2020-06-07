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
	dor_simply_initialize
	dor_simply_create
	dor_simply_destroy
	dor_simply_print
	dor_simply_update_alternate
	dor_simply_update_lines
	dor_simply_update_tris
	dor_simply_create_alternate_geometry_complex_triangle
	dor_simply_create_alternate_lines
	dor_simply_create_alternate_geometry_simple_triangle
	dor_simply_create_alternate_tris
	dor_simply_compute_bounding_volume
	dor_simply_enable_edges_complex
	dor_simply_check_complex_edge
	dor_simply_enable_edges_simple
	dor_simply_pick
	dor_simply_pick_lines
	dor_simply_pick_tris
	dor_simply_render_standard

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/gpolyd.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/simply.h>
#include <dore/dore_develop/private/linlst.h>
#include <dore/dore_develop/private/trilst.h>

/*
 ======================================================================
 */

DtInt doe_simply_class_id;

static DtMethodEntry simply_methods[] = {
    {DcMethodCmpBndVolume,     (DtMethodPtr)dor_simply_compute_bounding_volume},
    {DcMethodDestroy,          (DtMethodPtr)dor_simply_destroy},
    {DcMethodPick,             (DtMethodPtr)dor_simply_pick},
    {DcMethodPrint,            (DtMethodPtr)dor_simply_print},
    {DcMethodUpdStdAltObj,     (DtMethodPtr)dor_simply_update_alternate},
    {DcMethodUpdStdAltLineObj, (DtMethodPtr)dor_simply_update_lines},
    {DcMethodStdRenderDisplay, (DtMethodPtr)dor_simply_render_standard}
};

void dor_simply_initialize (void)
{
    doe_simply_class_id = dor_class_add ("DoSimplePolygon", 7,
					 simply_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_simply_create (
    DtColorModel colormodel,
    DtVertexType vertextype,
    DtInt vertexcount,
    DtReal *vertices,
    DtShapeType shape)
{
    static DtFlag initialized = DcFalse;
    struct simply *simply;
    DtInt vertexno;
    DtVertexType basevtxtype;
    DtInt uv, uvw;
    DtReal *vertexptr;
    dot_object *simplyobject;

    if (!initialized) {
      	dor_simply_initialize();
      	initialized = DcTrue;
    }

    simply = (struct simply *)dor_space_allocate(sizeof *simply);

    simply->colormodel = colormodel;

    basevtxtype = dod_system_inq_vertex_basetype(vertextype);
    simply->vertextype = basevtxtype;
    simply->fullvtxtype = vertextype;

    simply->vertex_count = vertexcount;

    simply->vertex_location = (DtRealTriple *)dor_space_allocate
			(vertexcount*(sizeof (DtRealTriple)));

    simply->vertex_normal = (DtRealTriple *)NULL;

    simply->vertex_color = (DtColorRGB *)NULL;

    simply->vertex_alpha = (DtReal *)NULL;

    /* texture coordinates */

    simply->uv_list = (DtRealCouple **)dor_space_allocate
		(dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
    simply->uvw_list = (DtRealTriple **)dor_space_allocate
		(dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *));

    for (uv = 0; uv < dod_system_max_vertex_uv_cnt; uv++) {
        simply->uv_list[uv] = NULL;
    }
    for (uvw = 0; uvw < dod_system_max_vertex_uvw_cnt; uvw++) {
        simply->uvw_list[uvw] = NULL;
    }

    simply->uv_count = dod_system_inq_vertex_uv_cnt(vertextype);
    simply->uvw_count = dod_system_inq_vertex_uvw_cnt(vertextype);

    for (uv=0; uv< simply->uv_count; uv++) {
        simply->uv_list[uv] = (DtRealCouple *)dor_space_allocate
		(vertexcount*(sizeof (DtRealCouple)));
    }

    for (uvw=0; uvw< simply->uvw_count; uvw++) {
        simply->uvw_list[uvw] = (DtRealTriple *)dor_space_allocate
		(vertexcount*(sizeof (DtRealTriple)));
    }

    /**/

    vertexptr = vertices;

    if (basevtxtype & DcLocNrm)
        simply->vertex_normal = (DtRealTriple *)
            dor_space_allocate(vertexcount*(sizeof (DtRealTriple)));

    if (basevtxtype & DcLocClr)
        simply->vertex_color = (DtColorRGB *)
            dor_space_allocate(vertexcount*(sizeof (DtColorRGB)));

    if (basevtxtype & DcLocAlpha)
        simply->vertex_alpha = (DtReal *)
            dor_space_allocate(vertexcount*(sizeof (DtReal)));

PRAGMA(ASIS)
    for (vertexno=0;vertexno<vertexcount;vertexno++) {
        simply->vertex_location[vertexno][0] = *vertexptr++;
        simply->vertex_location[vertexno][1] = *vertexptr++;
        simply->vertex_location[vertexno][2] = *vertexptr++;

        if (basevtxtype & DcLocNrm) {
            simply->vertex_normal[vertexno][0] = *vertexptr++;
            simply->vertex_normal[vertexno][1] = *vertexptr++;
            simply->vertex_normal[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocClr) {
            simply->vertex_color[vertexno][0] = *vertexptr++;
            simply->vertex_color[vertexno][1] = *vertexptr++;
            simply->vertex_color[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocAlpha) {
            simply->vertex_alpha[vertexno] = *vertexptr++;
        }

        for (uv=0; uv < simply->uv_count; uv++) {
            simply->uv_list[uv][vertexno][0] = *vertexptr++;
            simply->uv_list[uv][vertexno][1] = *vertexptr++;
        }

        for (uvw=0; uvw < simply->uvw_count; uvw++) {
            simply->uvw_list[uvw][vertexno][0] = *vertexptr++;
            simply->uvw_list[uvw][vertexno][1] = *vertexptr++;
            simply->uvw_list[uvw][vertexno][2] = *vertexptr++;
        }

    }

    simply->shape = shape;

    simply->alt_object_lines = NULL;
    simply->alt_object_tris = NULL;

    simplyobject = dor_object_create(doe_simply_class_id,simply);

    return(simplyobject);
}
/*
 ======================================================================
 */

void dor_simply_destroy (dot_object *object)
{
    int i;
    struct simply *simply;

    simply = (struct simply *)(object->data);

    if (simply->vertex_location != NULL) {
	dor_space_deallocate(simply->vertex_location);
    }

    if (simply->vertex_normal != NULL) {
	dor_space_deallocate(simply->vertex_normal);
    }

    if (simply->vertex_color != NULL) {
	dor_space_deallocate(simply->vertex_color);
    }

    if (simply->vertex_alpha != NULL) {
	dor_space_deallocate(simply->vertex_alpha);
    }

    for (i=0; i< simply->uv_count; i++) {
	dor_space_deallocate(simply->uv_list[i]);
    }
    dor_space_deallocate(simply->uv_list);

    for (i=0; i< simply->uvw_count; i++) {
	dor_space_deallocate(simply->uvw_list[i]);
    }
    dor_space_deallocate(simply->uvw_list);

    if (simply->alt_object_lines != NULL) {
	dor_object_check_deletion(simply->alt_object_lines);
    }

    if (simply->alt_object_tris != NULL) {
	dor_object_check_deletion(simply->alt_object_tris);
    }

    dor_space_deallocate(simply);
}
/*
 ======================================================================
 */

void dor_simply_print (struct object *object)
{
    struct simply *simply;

    simply = (struct simply *)(object->data);

    if (simply == NULL){
	dor_print_output("simple polygon is NULL");
	return;
    }

    switch (simply->colormodel){
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;
 
    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    dor_utilpri_print_vertextype(simply->vertextype);

    switch (simply->shape)
    {
     case DcConvex: 
	dor_print_output("shape = convex");
	break;

     case DcConcave: 
	dor_print_output("shape = concave");
	break;

     case DcComplex: 
	dor_print_output("shape = complex");
	break;
    }

    sprintf(dor_print_get_line(),
	    "vertex count = %d",simply->vertex_count);
    dor_print_output(dor_print_get_line());

    if (simply->uv_count > 0) {
	sprintf (dor_print_get_line(), 
		 "uv's per vertex = %d", simply->uv_count);
	dor_print_output(dor_print_get_line());
    }

    if (simply->uvw_count > 0) {
	sprintf (dor_print_get_line(), 
		 "uvw's per vertex = %d", simply->uvw_count);
	dor_print_output(dor_print_get_line());
    }
}
/*
 ======================================================================
 */

dot_object *dor_simply_update_alternate (dot_object *object)
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
	alt = dor_simply_update_tris(object);
	break;

    case DcOutlines:
	alt = dor_simply_update_lines(object);
	break;
    default:
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		   "dor_simply_update_alternate", 
		   "unknown reptype");
	alt = DcNullPtr;
	break;
    }
    return alt;
}
/*
 ======================================================================
 */

dot_object *dor_simply_update_lines (dot_object *object)
{
    struct simply *simply;
    dot_object *alt;

    simply = (struct simply *)(object->data);

    if (simply->alt_object_lines != DcNullPtr) {
	alt = simply->alt_object_lines;

    } else {
	simply->alt_object_lines = dor_simply_create_alternate_lines(simply);
	alt = simply->alt_object_lines;

	if (simply->alt_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_simply_update_lines", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */

dot_object *dor_simply_update_tris (dot_object *object)
{
    struct simply *simply;
    dot_object *alt;

    simply = (struct simply *)(object->data);

    if (simply->alt_object_tris != DcNullPtr) {
	alt = simply->alt_object_tris;

    } else {
	simply->alt_object_tris = dor_simply_create_alternate_tris(simply);
	alt = simply->alt_object_tris;

	if (simply->alt_object_tris == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_simply_update_tris", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */

#define MAXVTXS_CT (27 + dod_system_max_vertex_uv_cnt*6 + \
                        dod_system_max_vertex_uvw_cnt*9)

void dor_simply_create_alternate_geometry_complex_triangle (
    struct simply *simply,
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

   edge_enable = dor_simply_enable_edges_complex(simply, gpolydvtx1, 
						gpolydvtx2, gpolydvtx3);

   vertextype = simply->vertextype;

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
           *vtxptr++ = weight1*simply->vertex_normal[vertex1][0] +
                       weight2*simply->vertex_normal[vertex2][0];
           *vtxptr++ = weight1*simply->vertex_normal[vertex1][1] +
                       weight2*simply->vertex_normal[vertex2][1];
           *vtxptr++ = weight1*simply->vertex_normal[vertex1][2] +
                       weight2*simply->vertex_normal[vertex2][2];
       }

       if (vertextype & DcLocClr) {
           *vtxptr++ = weight1*simply->vertex_color[vertex1][0] +
                       weight2*simply->vertex_color[vertex2][0];
           *vtxptr++ = weight1*simply->vertex_color[vertex1][1] +
                       weight2*simply->vertex_color[vertex2][1];
           *vtxptr++ = weight1*simply->vertex_color[vertex1][2] +
                       weight2*simply->vertex_color[vertex2][2];
       }

       if (vertextype & DcLocAlpha) {
           *vtxptr++ = weight1*simply->vertex_alpha[vertex1] +
                       weight2*simply->vertex_alpha[vertex2];
       }

       for (uv=0; uv < simply->uv_count; uv++) {
            *vtxptr++ = weight1*simply->uv_list[uv][vertex1][0] +
                        weight2*simply->uv_list[uv][vertex2][0];
            *vtxptr++ = weight1*simply->uv_list[uv][vertex1][1] +
                        weight2*simply->uv_list[uv][vertex2][1];
       }
       for (uvw=0; uvw < simply->uvw_count; uvw++) {
            *vtxptr++ = weight1*simply->uvw_list[uvw][vertex1][0] +
                        weight2*simply->uvw_list[uvw][vertex2][0];
            *vtxptr++ = weight1*simply->uvw_list[uvw][vertex1][1] +
                        weight2*simply->uvw_list[uvw][vertex2][1];
            *vtxptr++ = weight1*simply->uvw_list[uvw][vertex1][2] +
                        weight2*simply->uvw_list[uvw][vertex2][2];
       }
    } else {
       if (vertextype & DcLocNrm) {
           *vtxptr++ = simply->vertex_normal[vertex1][0];
           *vtxptr++ = simply->vertex_normal[vertex1][1];
           *vtxptr++ = simply->vertex_normal[vertex1][2];
       }
       if (vertextype & DcLocClr) {
           *vtxptr++ = simply->vertex_color[vertex1][0];
           *vtxptr++ = simply->vertex_color[vertex1][1];
           *vtxptr++ = simply->vertex_color[vertex1][2];
       }
       if (vertextype & DcLocAlpha) {
           *vtxptr++ = simply->vertex_alpha[vertex1];
       }

       for (uv=0; uv < simply->uv_count; uv++) {
               *vtxptr++ = simply->uv_list[uv][vertex1][0];
               *vtxptr++ = simply->uv_list[uv][vertex1][1];
       }
       for (uvw=0; uvw < simply->uvw_count; uvw++) {
               *vtxptr++ = simply->uvw_list[uvw][vertex1][0];
               *vtxptr++ = simply->uvw_list[uvw][vertex1][1];
               *vtxptr++ = simply->uvw_list[uvw][vertex1][2];
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
           *vtxptr++ = weight1*simply->vertex_normal[vertex1][0] +
                       weight2*simply->vertex_normal[vertex2][0];
           *vtxptr++ = weight1*simply->vertex_normal[vertex1][1] +
                       weight2*simply->vertex_normal[vertex2][1];
           *vtxptr++ = weight1*simply->vertex_normal[vertex1][2] +
                       weight2*simply->vertex_normal[vertex2][2];
       }

       if (vertextype & DcLocClr) {
           *vtxptr++ = weight1*simply->vertex_color[vertex1][0] +
                       weight2*simply->vertex_color[vertex2][0];
           *vtxptr++ = weight1*simply->vertex_color[vertex1][1] +
                       weight2*simply->vertex_color[vertex2][1];
           *vtxptr++ = weight1*simply->vertex_color[vertex1][2] +
                       weight2*simply->vertex_color[vertex2][2];
       }

       if (vertextype & DcLocAlpha) {
           *vtxptr++ = weight1*simply->vertex_alpha[vertex1] +
                       weight2*simply->vertex_alpha[vertex2];
       }
        for (uv=0; uv < simply->uv_count; uv++) {
             *vtxptr++ = weight1*simply->uv_list[uv][vertex1][0] +
                        weight2*simply->uv_list[uv][vertex2][0];
             *vtxptr++ = weight1*simply->uv_list[uv][vertex1][1] +
                        weight2*simply->uv_list[uv][vertex2][1];
        }
        for (uvw=0; uvw < simply->uvw_count; uvw++) {
            *vtxptr++ = weight1*simply->uvw_list[uvw][vertex1][0] +
                        weight2*simply->uvw_list[uvw][vertex2][0];
            *vtxptr++ = weight1*simply->uvw_list[uvw][vertex1][1] +
                        weight2*simply->uvw_list[uvw][vertex2][1];
            *vtxptr++ = weight1*simply->uvw_list[uvw][vertex1][2] +
                        weight2*simply->uvw_list[uvw][vertex2][2];
        }
 
    } else {
       if (vertextype & DcLocNrm) {
           *vtxptr++ = simply->vertex_normal[vertex1][0];
           *vtxptr++ = simply->vertex_normal[vertex1][1];
           *vtxptr++ = simply->vertex_normal[vertex1][2];
       }
       if (vertextype & DcLocClr) {
           *vtxptr++ = simply->vertex_color[vertex1][0];
           *vtxptr++ = simply->vertex_color[vertex1][1];
           *vtxptr++ = simply->vertex_color[vertex1][2];
       }
       if (vertextype & DcLocAlpha) {
           *vtxptr++ = simply->vertex_alpha[vertex1];
       }
       for (uv=0; uv < simply->uv_count; uv++) {
           *vtxptr++ = simply->uv_list[uv][vertex1][0];
           *vtxptr++ = simply->uv_list[uv][vertex1][1];
       }
       for (uvw=0; uvw < simply->uvw_count; uvw++) {
           *vtxptr++ = simply->uvw_list[uvw][vertex1][0];
           *vtxptr++ = simply->uvw_list[uvw][vertex1][1];
           *vtxptr++ = simply->uvw_list[uvw][vertex1][2];
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
           *vtxptr++ = weight1*simply->vertex_normal[vertex1][0] +
                       weight2*simply->vertex_normal[vertex2][0];
           *vtxptr++ = weight1*simply->vertex_normal[vertex1][1] +
                       weight2*simply->vertex_normal[vertex2][1];
           *vtxptr++ = weight1*simply->vertex_normal[vertex1][2] +
                       weight2*simply->vertex_normal[vertex2][2];
       }

       if (vertextype & DcLocClr) {
           *vtxptr++ = weight1*simply->vertex_color[vertex1][0] +
                       weight2*simply->vertex_color[vertex2][0];
           *vtxptr++ = weight1*simply->vertex_color[vertex1][1] +
                       weight2*simply->vertex_color[vertex2][1];
           *vtxptr++ = weight1*simply->vertex_color[vertex1][2] +
                       weight2*simply->vertex_color[vertex2][2];
       }

       if (vertextype & DcLocAlpha) {
           *vtxptr++ = weight1*simply->vertex_alpha[vertex1] +
                       weight2*simply->vertex_alpha[vertex2];
       }

        for (uv=0; uv < simply->uv_count; uv++) {
            *vtxptr++ = weight1*simply->uv_list[uv][vertex1][0] +
                        weight2*simply->uv_list[uv][vertex2][0];
            *vtxptr++ = weight1*simply->uv_list[uv][vertex1][1] +
                        weight2*simply->uv_list[uv][vertex2][1];
        }
        for (uvw=0; uvw < simply->uvw_count; uvw++) {
            *vtxptr++ = weight1*simply->uvw_list[uvw][vertex1][0] +
                        weight2*simply->uvw_list[uvw][vertex2][0];
            *vtxptr++ = weight1*simply->uvw_list[uvw][vertex1][1] +
                        weight2*simply->uvw_list[uvw][vertex2][1];
            *vtxptr++ = weight1*simply->uvw_list[uvw][vertex1][2] +
                       weight2*simply->uvw_list[uvw][vertex2][2];
        }

    } else {
       if (vertextype & DcLocNrm) {
           *vtxptr++ = simply->vertex_normal[vertex1][0];
           *vtxptr++ = simply->vertex_normal[vertex1][1];
           *vtxptr++ = simply->vertex_normal[vertex1][2];
       }
       if (vertextype & DcLocClr) {
           *vtxptr++ = simply->vertex_color[vertex1][0];
           *vtxptr++ = simply->vertex_color[vertex1][1];
           *vtxptr++ = simply->vertex_color[vertex1][2];
       }
       if (vertextype & DcLocAlpha) {
           *vtxptr++ = simply->vertex_alpha[vertex1];
       }
       for (uv=0; uv < simply->uv_count; uv++) {
           *vtxptr++ = simply->uv_list[uv][vertex1][0];
           *vtxptr++ = simply->uv_list[uv][vertex1][1];
       }        
       for (uvw=0; uvw < simply->uvw_count; uvw++) {
           *vtxptr++ = simply->uvw_list[uvw][vertex1][0];
           *vtxptr++ = simply->uvw_list[uvw][vertex1][1];
           *vtxptr++ = simply->uvw_list[uvw][vertex1][2];
       }
   }

   dor_trilst_add_triangle_with_edges(simply->alt_object_tris,
                                      vertices, edge_enable);
}

/*
 ======================================================================
 */

#define MAXVTXS_L (18 + dod_system_max_vertex_uv_cnt*4 + \
			dod_system_max_vertex_uvw_cnt*6)

dot_object *dor_simply_create_alternate_lines (struct simply *simply)
{
    DtReal vtxs[MAXVTXS_L];
    DtInt lastvertex;
    DtInt lvtx0, lvtx1;
    DtUInt i;
    DtUInt uv, uvw;
    DtInt vertexno;
    dot_object *linlstobj;

    linlstobj = dor_linlst_open(simply->colormodel, simply->fullvtxtype,
				simply->vertex_count);

    lastvertex = simply->vertex_count-1;

    for (vertexno=0; vertexno<=lastvertex; vertexno++) {
   	if (vertexno == lastvertex) {
	    lvtx0 = lastvertex;
	    lvtx1 = 0;
   	} else {
	    lvtx0 = vertexno;
	    lvtx1 = vertexno+1;
   	}

   	i = 0;

	/* One end of a line */

   	vtxs[i++] = simply->vertex_location[lvtx0][0];
        vtxs[i++] = simply->vertex_location[lvtx0][1];
        vtxs[i++] = simply->vertex_location[lvtx0][2];

   	if (simply->vertextype & DcLocNrm) {
	    vtxs[i++] = simply->vertex_normal[lvtx0][0];
	    vtxs[i++] = simply->vertex_normal[lvtx0][1];
	    vtxs[i++] = simply->vertex_normal[lvtx0][2];
   	}

   	if (simply->vertextype & DcLocClr) {
	    vtxs[i++] = simply->vertex_color[lvtx0][0];
	    vtxs[i++] = simply->vertex_color[lvtx0][1];
	    vtxs[i++] = simply->vertex_color[lvtx0][2];
   	}

   	if (simply->vertextype & DcLocAlpha) {
	    vtxs[i++] = simply->vertex_alpha[lvtx0];
   	}

	for (uv=0; uv<simply->uv_count; uv++) {
	    vtxs[i++] = simply->uv_list[uv][lvtx0][0]; 
	    vtxs[i++] = simply->uv_list[uv][lvtx0][1]; 
	}

	for (uvw=0; uvw<simply->uvw_count; uvw++) {
	    vtxs[i++] = simply->uvw_list[uvw][lvtx0][0]; 
	    vtxs[i++] = simply->uvw_list[uvw][lvtx0][1]; 
	    vtxs[i++] = simply->uvw_list[uvw][lvtx0][2]; 
	}

	/* The other end of the line */

   	vtxs[i++] = simply->vertex_location[lvtx1][0]; 
        vtxs[i++] = simply->vertex_location[lvtx1][1]; 
        vtxs[i++] = simply->vertex_location[lvtx1][2]; 
 
        if (simply->vertextype & DcLocNrm) {
	    vtxs[i++] = simply->vertex_normal[lvtx1][0];
	    vtxs[i++] = simply->vertex_normal[lvtx1][1];
	    vtxs[i++] = simply->vertex_normal[lvtx1][2];
        } 
 
        if (simply->vertextype & DcLocClr) {
	    vtxs[i++] = simply->vertex_color[lvtx1][0];
	    vtxs[i++] = simply->vertex_color[lvtx1][1];
	    vtxs[i++] = simply->vertex_color[lvtx1][2];
        }

        if (simply->vertextype & DcLocAlpha) {
	    vtxs[i++] = simply->vertex_alpha[lvtx1];
        }

	for (uv=0; uv<simply->uv_count; uv++) {
	    vtxs[i++] = simply->uv_list[uv][lvtx1][0]; 
	    vtxs[i++] = simply->uv_list[uv][lvtx1][1]; 
	}

	for (uvw=0; uvw<simply->uvw_count; uvw++) {
	    vtxs[i++] = simply->uvw_list[uvw][lvtx1][0]; 
	    vtxs[i++] = simply->uvw_list[uvw][lvtx1][1]; 
	    vtxs[i++] = simply->uvw_list[uvw][lvtx1][2]; 
	}

	/* Add the line to the list */
   	
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

void dor_simply_create_alternate_geometry_simple_triangle (
    struct simply *simply,
    DtInt vertex1,
    DtInt vertex2,
    DtInt vertex3)
{
   DtVertexType vertextype;
   DtReal *vtxptr;
   DtReal vertices[MAXVTXS_ST];
   DtInt uv, uvw;
   DtInt edge_enable;

   edge_enable = dor_simply_enable_edges_simple(simply, vertex1, vertex2, vertex3);

   vertextype = simply->vertextype;

   vtxptr = vertices;

   /* First vertex */

   *vtxptr++ = simply->vertex_location[vertex1][0];
   *vtxptr++ = simply->vertex_location[vertex1][1];
   *vtxptr++ = simply->vertex_location[vertex1][2];

   if (vertextype & DcLocNrm) {
       *vtxptr++ = simply->vertex_normal[vertex1][0];
       *vtxptr++ = simply->vertex_normal[vertex1][1];
       *vtxptr++ = simply->vertex_normal[vertex1][2];
   }
   if (vertextype & DcLocClr) {
       *vtxptr++ = simply->vertex_color[vertex1][0];
       *vtxptr++ = simply->vertex_color[vertex1][1];
       *vtxptr++ = simply->vertex_color[vertex1][2];
   }
   if (vertextype & DcLocAlpha) {
       *vtxptr++ = simply->vertex_alpha[vertex1];
   }

   for (uv=0; uv<simply->uv_count; uv++) {
       *vtxptr++ = simply->uv_list[uv][vertex1][0]; 
       *vtxptr++ = simply->uv_list[uv][vertex1][1]; 
   }

   for (uvw=0; uvw<simply->uvw_count; uvw++) {
       *vtxptr++ = simply->uvw_list[uvw][vertex1][0]; 
       *vtxptr++ = simply->uvw_list[uvw][vertex1][1]; 
       *vtxptr++ = simply->uvw_list[uvw][vertex1][2]; 
   }

   /* Second vertex */

   *vtxptr++ = simply->vertex_location[vertex2][0];
   *vtxptr++ = simply->vertex_location[vertex2][1];
   *vtxptr++ = simply->vertex_location[vertex2][2];

   if (vertextype & DcLocNrm) {
       *vtxptr++ = simply->vertex_normal[vertex2][0];
       *vtxptr++ = simply->vertex_normal[vertex2][1];
       *vtxptr++ = simply->vertex_normal[vertex2][2];
   }
   if (vertextype & DcLocClr) {
       *vtxptr++ = simply->vertex_color[vertex2][0];
       *vtxptr++ = simply->vertex_color[vertex2][1];
       *vtxptr++ = simply->vertex_color[vertex2][2];
   }
   if (vertextype & DcLocAlpha) {
       *vtxptr++ = simply->vertex_alpha[vertex2];
   }

   for (uv=0; uv<simply->uv_count; uv++) {
       *vtxptr++ = simply->uv_list[uv][vertex2][0]; 
       *vtxptr++ = simply->uv_list[uv][vertex2][1]; 
   }

   for (uvw=0; uvw<simply->uvw_count; uvw++) {
       *vtxptr++ = simply->uvw_list[uvw][vertex2][0]; 
       *vtxptr++ = simply->uvw_list[uvw][vertex2][1]; 
       *vtxptr++ = simply->uvw_list[uvw][vertex2][2]; 
   }

   /* Third vertex */

   *vtxptr++ = simply->vertex_location[vertex3][0];
   *vtxptr++ = simply->vertex_location[vertex3][1];
   *vtxptr++ = simply->vertex_location[vertex3][2];

   if (vertextype & DcLocNrm) {
       *vtxptr++ = simply->vertex_normal[vertex3][0];
       *vtxptr++ = simply->vertex_normal[vertex3][1];
       *vtxptr++ = simply->vertex_normal[vertex3][2];
   }
   if (vertextype & DcLocClr) {
       *vtxptr++ = simply->vertex_color[vertex3][0];
       *vtxptr++ = simply->vertex_color[vertex3][1];
       *vtxptr++ = simply->vertex_color[vertex3][2];
   }
   if (vertextype & DcLocAlpha) {
       *vtxptr++ = simply->vertex_alpha[vertex3];
   }

   for (uv=0; uv<simply->uv_count; uv++) {
       *vtxptr++ = simply->uv_list[uv][vertex3][0]; 
       *vtxptr++ = simply->uv_list[uv][vertex3][1]; 
   }

   for (uvw=0; uvw<simply->uvw_count; uvw++) {
       *vtxptr++ = simply->uvw_list[uvw][vertex3][0]; 
       *vtxptr++ = simply->uvw_list[uvw][vertex3][1]; 
       *vtxptr++ = simply->uvw_list[uvw][vertex3][2]; 
   }

    /* Add triangle to list */

    dor_trilst_add_triangle_with_edges(simply->alt_object_tris,
                                       vertices, edge_enable);
}
/*
 ======================================================================
 */

dot_object *dor_simply_create_alternate_tris (struct simply *simply)
{
    dot_object *trilstobj;
    DtInt vertexno;

    trilstobj = dor_trilst_open(DcRGB,simply->fullvtxtype,
				simply->vertex_count);

    simply->alt_object_tris = trilstobj;

    dor_gpolyd_create_polygon();

    dor_gpolyd_create_contour();
	
    for (vertexno=0;vertexno<simply->vertex_count;vertexno++){
	dor_gpolyd_create_contour_vertex(vertexno,
					 simply->vertex_location[vertexno][0],
					 simply->vertex_location[vertexno][1],
					 simply->vertex_location[vertexno][2]);
    }
	
    dor_gpolyd_close_contour();

    dor_gpolyd_close_polygon();

    dor_gpolyd_triangulate_polygon
    (   simply->shape, 
	dor_simply_create_alternate_geometry_simple_triangle,
	dor_simply_create_alternate_geometry_complex_triangle,
	simply
    );

    dor_trilst_close(trilstobj);

    return(trilstobj);
}
/*
 ======================================================================
 */

void dor_simply_compute_bounding_volume (dot_object *object)
{
    struct simply *simply;

    simply = (struct simply *)object->data;

    dor_system_report_bounding_points
	(simply->vertex_count, (DtReal*) simply->vertex_location);
}
/*
 ======================================================================
 */

DtInt dor_simply_enable_edges_complex (
    struct simply *simply,
    dot_gpolyd_interpolated_vertex *gpolydvtx1,
    dot_gpolyd_interpolated_vertex *gpolydvtx2,
    dot_gpolyd_interpolated_vertex *gpolydvtx3)
{
    DtInt edge_enable;

#ifdef DEBUG
    printf("gpolyds %d %d %d\n", gpolydvtx1->vertex1, gpolydvtx2->vertex1, gpolydvtx3->vertex1);
#endif
    edge_enable = dod_no_enabled_edges;
    if (dor_simply_check_complex_edge(simply, gpolydvtx1, gpolydvtx2))
	    edge_enable |= dod_edge1_enabled;
    if (dor_simply_check_complex_edge(simply, gpolydvtx2, gpolydvtx3))
	    edge_enable |= dod_edge2_enabled;
    if (dor_simply_check_complex_edge(simply, gpolydvtx3, gpolydvtx1))
	    edge_enable |= dod_edge3_enabled;
#ifdef DEBUG
    printf("edge_enable %d \n", edge_enable);
#endif

    return (edge_enable);
}

DtFlag dor_simply_check_complex_edge (
    struct simply *simply,
    dot_gpolyd_interpolated_vertex *vtx1,
    dot_gpolyd_interpolated_vertex *vtx2)
{
    DtInt vertexcount;

#ifdef DEBUG
    printf("dor_simply_check_cmpxpoly_edge\n");
#endif

    vertexcount = simply->vertex_count;

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

    } else if (vtx1->vertex1 == ((vtx2->vertex1+1)%vertexcount) ||
	       vtx2->vertex1 == ((vtx1->vertex1+1)%vertexcount)) {
#ifdef DEBUG
        printf("vtx1->vertex1 %d, vtx2->vertex1 %d\n", vtx1->vertex1, vtx2->vertex1);
#endif
        return (DcTrue);
    }

    return (DcFalse);

}
/*
 ======================================================================
 */

DtInt dor_simply_enable_edges_simple (
    struct simply *simply,
    DtInt vertex1,
    DtInt vertex2,
    DtInt vertex3)
{
    DtInt vertexcount;  
    DtInt edge_enable;

#ifdef DEBUG
    printf ("v1 %d v2 %d v3 %d\n", vertex1, vertex2, vertex3);
#endif

    vertexcount = simply->vertex_count;
    edge_enable = dod_no_enabled_edges;
    if (vertex1 == ((vertex2+1)%vertexcount) ||
	vertex2 == ((vertex1+1)%vertexcount)) {
        edge_enable |= dod_edge1_enabled;
#ifdef DEBUG
        printf ("enable edge 1\n");
#endif
    }
    if (vertex2 == ((vertex3+1)%vertexcount) ||
	vertex3 == ((vertex2+1)%vertexcount)) {
        edge_enable |= dod_edge2_enabled;
#ifdef DEBUG
        printf ("enable edge 2\n");
#endif
    }
    if (vertex3 == ((vertex1+1)%vertexcount) ||
	vertex1 == ((vertex3+1)%vertexcount)) {
        edge_enable |= dod_edge3_enabled;
#ifdef DEBUG
        printf ("enable edge 3\n");
#endif
    }

#ifdef DEBUG
    printf ("edge_enable = %d\n", edge_enable);
#endif

    return(edge_enable);
}
/*
 ======================================================================
 */

void dor_simply_pick (dot_object *object)
{
    DtRepType reptype;

    if (!dor_condex_query_pick(doe_simply_class_id))
	    return;

    dor_pick_initialize_object(object);

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    case DcOutlines:
	dor_simply_pick_lines(object);
	break;

    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
    case DcWireframe:
    case DcPoints:
	dor_simply_pick_tris(object);
	break;
    default:
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		   "dor_simply_pick",
		   "unknown reptype");
    }
}
/*
 ======================================================================
 */

void dor_simply_pick_lines (dot_object *object)
{
    struct simply *simply;

    simply = (struct simply *)(object->data);

    if (simply->alt_object_lines != DcNullPtr) {
   	doe_system_alternate_object_execution_flag = DcTrue;

   	dor_object_get_method(simply->alt_object_lines,
			      DcMethodPick)(simply->alt_object_lines);

   	doe_system_alternate_object_execution_flag = DcFalse;

   	return;
    } else {
   	simply->alt_object_lines =
   		dor_simply_create_alternate_lines(simply);

   	if (simply->alt_object_lines == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_simply_pick_lines", "");
	    return;
   	}

   	doe_system_alternate_object_execution_flag = DcTrue;

   	dor_object_get_method(simply->alt_object_lines,
			      DcMethodPick)(simply->alt_object_lines);

   	doe_system_alternate_object_execution_flag = DcFalse;

   	return;
    }
}
/*
 ======================================================================
 */

void dor_simply_pick_tris (dot_object *object)
{
    struct simply *simply;

    simply = (struct simply *)(object->data);

    if (simply->alt_object_tris != DcNullPtr) {
   	doe_system_alternate_object_execution_flag = DcTrue;

   	dor_object_get_method(simply->alt_object_tris,
			      DcMethodPick)(simply->alt_object_tris);

   	doe_system_alternate_object_execution_flag = DcFalse;

   	return;
    } else {
   	simply->alt_object_tris =
   		dor_simply_create_alternate_tris(simply);

   	if (simply->alt_object_tris == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_simply_pick_tris", "");
	    return;
   	}

   	doe_system_alternate_object_execution_flag = DcTrue;

   	dor_object_get_method(simply->alt_object_tris,
			      DcMethodPick)(simply->alt_object_tris);

   	doe_system_alternate_object_execution_flag = DcFalse;

   	return;
    }
}
/*
 ======================================================================
 */

void dor_simply_render_standard (dot_object *object)
{
    dot_object *alt;

    if (!dor_condex_query_render(doe_simply_class_id))
	    return;

    alt = dor_simply_update_alternate (object);
    if (alt != DcNullPtr) {
	dor_object_get_current_method(alt) (alt);
    }
}
