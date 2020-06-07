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
	dor_plymsh_initialize
	dor_plymsh_create
	dor_plymsh_destroy
	dor_plymsh_print
	dor_plymsh_update_alternate
	dor_plymsh_create_alternate_geometry_complex_triangle
	dor_plymsh_create_alternate_geom_lines
	dor_plymsh_create_alternate_geometry_simple_triangle
	dor_plymsh_create_alternate_geom_tris
	dor_plymsh_calculate_vertex_normals
	dor_plymsh_compute_bounding_volume
	dor_plymsh_enable_edges_complex
	dor_plymsh_check_complex_edge
	dor_plymsh_enable_edges_simple
	dor_plymsh_pick
	dor_plymsh_pick_lines
	dor_plymsh_pick_tris
	dor_plymsh_render_standard
	dor_plymsh_update_lines
	dor_plymsh_update_tris

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/gpolyd.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/plymsh.h>
#include <dore/dore_develop/private/linlst.h>
#include <dore/dore_develop/private/trilst.h>

#define MAXVTXS (27 + dod_system_max_vertex_uv_cnt*6 + \
                        dod_system_max_vertex_uvw_cnt*9)

#define MAXVTXS_LIN (18 + dod_system_max_vertex_uv_cnt*4 + \
                        dod_system_max_vertex_uvw_cnt*6)

/*
 ======================================================================
 */

DtInt doe_plymsh_class_id;

static DtMethodEntry plymsh_methods[] = {
    { DcMethodCmpBndVolume,  (DtMethodPtr) dor_plymsh_compute_bounding_volume },
    { DcMethodDestroy,  (DtMethodPtr) dor_plymsh_destroy },
    { DcMethodPick,  (DtMethodPtr) dor_plymsh_pick },
    { DcMethodPrint,  (DtMethodPtr) dor_plymsh_print },
    { DcMethodUpdStdAltObj,  (DtMethodPtr) dor_plymsh_update_alternate },
    { DcMethodUpdStdAltLineObj,  (DtMethodPtr) dor_plymsh_update_lines },
    { DcMethodStdRenderDisplay,  (DtMethodPtr) dor_plymsh_render_standard }
};

void dor_plymsh_initialize (void)
{
    doe_plymsh_class_id = dor_class_install (DcTypePolygonMesh, "DoPolygonMesh",
					     7, plymsh_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_plymsh_create (
    DtColorModel colormodel,
    DtVertexType vertextype,
    DtInt vertexcount,
    DtReal vertices[],
    DtInt polygoncount,
    DtInt polygons[],
    DtInt contours[],
    DtInt vertexlist[],
    DtShapeType shape,
    DtFlag smoothflag)
{
   static DtFlag initialized = DcFalse;
   struct plymsh *plymsh;
   DtInt polyvertexcount;
   DtInt contourcount;
   DtInt contourno;
   DtInt polygonno;
   DtReal *vertexptr;
   DtInt vertexno;
   DtVertexType basevtxtype;
   DtInt uv, uvw;
   dot_object *plymshobject;

   if (!initialized) {
      	dor_plymsh_initialize();
      	initialized = DcTrue;
   }

   plymsh = (struct plymsh *)dor_space_allocate(sizeof *plymsh);

   plymsh->colormodel = colormodel;

   basevtxtype = dod_system_inq_vertex_basetype(vertextype);
   plymsh->vertextype = basevtxtype;
   plymsh->fullvtxtype = vertextype;

   plymsh->polygon_count = polygoncount;

   plymsh->polygons = (DtInt *)dor_space_allocate((polygoncount)*sizeof(DtInt));

   contourcount = 0;

   contourno = 0;

   for (polygonno=0;polygonno<polygoncount;polygonno++) {
   	plymsh->polygons[polygonno] = polygons[polygonno];
   	contourcount += polygons[polygonno];
   }

   plymsh->contours = (DtInt *)dor_space_allocate((contourcount)*sizeof(DtInt));

   polyvertexcount = 0;

   for (contourno=0;contourno<contourcount;contourno++) {
   	plymsh->contours[contourno] = contours[contourno];
   	polyvertexcount += contours[contourno];
   }

   plymsh->vertexlist = (DtInt *) dor_space_allocate(
			(polyvertexcount)*sizeof(DtInt));

   for (vertexno=0;vertexno<polyvertexcount;vertexno++) {
   	plymsh->vertexlist[vertexno] = vertexlist[vertexno];
   }

   plymsh->vertex_count = vertexcount;

   plymsh->vertex_location = (DtRealTriple *)dor_space_allocate(
   	vertexcount*(sizeof (DtRealTriple)));

   plymsh->vertex_normal = (DtRealTriple *)NULL;

   plymsh->vertex_color = (DtColorRGB *)NULL;

   plymsh->vertex_alpha = (DtReal *)NULL;

   /* Texture coordinates */

   plymsh->uv_list = (DtRealCouple **)dor_space_allocate(
                dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
   plymsh->uvw_list = (DtRealTriple **)dor_space_allocate(
                dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *));

   for (uv=0; uv < dod_system_max_vertex_uv_cnt; uv++) {
        plymsh->uv_list[uv] = NULL;
   }
   for (uvw=0; uvw < dod_system_max_vertex_uvw_cnt; uvw++) {
        plymsh->uvw_list[uvw] = NULL;
   }

   plymsh->uv_count = dod_system_inq_vertex_uv_cnt(vertextype);
   plymsh->uvw_count = dod_system_inq_vertex_uvw_cnt(vertextype);

   for (uv=0; uv< plymsh->uv_count; uv++) {
        plymsh->uv_list[uv] = (DtRealCouple *)dor_space_allocate(
                                vertexcount*(sizeof (DtRealCouple)));
   }

   for (uvw=0; uvw< plymsh->uvw_count; uvw++) {
        plymsh->uvw_list[uvw] = (DtRealTriple *)dor_space_allocate(
                                vertexcount*(sizeof (DtRealTriple)));
   }

   /**/

   vertexptr = vertices;

   if (basevtxtype & DcLocNrm)
        plymsh->vertex_normal = (DtRealTriple *)
            dor_space_allocate(vertexcount*(sizeof (DtRealTriple)));

    if (basevtxtype & DcLocClr)
        plymsh->vertex_color = (DtColorRGB *)
            dor_space_allocate(vertexcount*(sizeof (DtColorRGB)));

    if (basevtxtype & DcLocAlpha)
        plymsh->vertex_alpha = (DtReal *)
            dor_space_allocate(vertexcount*(sizeof (DtReal)));

PRAGMA(ASIS)
   for (vertexno=0;vertexno<vertexcount;vertexno++) {
       plymsh->vertex_location[vertexno][0] = *vertexptr++;
       plymsh->vertex_location[vertexno][1] = *vertexptr++;
       plymsh->vertex_location[vertexno][2] = *vertexptr++;

       if (basevtxtype & DcLocNrm) {
           plymsh->vertex_normal[vertexno][0] = *vertexptr++;
           plymsh->vertex_normal[vertexno][1] = *vertexptr++;
           plymsh->vertex_normal[vertexno][2] = *vertexptr++;
       }

       if (basevtxtype & DcLocClr) {
           plymsh->vertex_color[vertexno][0] = *vertexptr++;
           plymsh->vertex_color[vertexno][1] = *vertexptr++;
           plymsh->vertex_color[vertexno][2] = *vertexptr++;
       }

       if (basevtxtype & DcLocAlpha) {
           plymsh->vertex_alpha[vertexno] = *vertexptr++;
       }

       for (uv=0; uv < plymsh->uv_count; uv++) {
           plymsh->uv_list[uv][vertexno][0] = *vertexptr++;
           plymsh->uv_list[uv][vertexno][1] = *vertexptr++;
       }

       for (uvw=0; uvw < plymsh->uvw_count; uvw++) {
           plymsh->uvw_list[uvw][vertexno][0] = *vertexptr++;
           plymsh->uvw_list[uvw][vertexno][1] = *vertexptr++;
           plymsh->uvw_list[uvw][vertexno][2] = *vertexptr++;
       }
   }

   plymsh->shape = shape;

   if (smoothflag) {
       if (!(basevtxtype & DcLocNrm)) {

            plymsh->vertex_normal = (DtRealTriple *)dor_space_allocate(
                       vertexcount*(sizeof (DtRealTriple)));

            dor_plymsh_calculate_vertex_normals(plymsh);

            plymsh->vertextype |= DcLocNrm;
            plymsh->fullvtxtype |= DcLocNrm;
       }
   }

   plymsh->smoothflag = smoothflag;

   plymsh->alt_object_lines = NULL;
   plymsh->alt_object_tris = NULL;

   plymshobject = dor_object_create(doe_plymsh_class_id,plymsh);

   return(plymshobject);
}
/*
 ======================================================================
 */

void dor_plymsh_destroy (dot_object *object)
{
    int i;
    struct plymsh *plymsh;

    plymsh = (struct plymsh *)(object->data);

    if (plymsh->vertex_location != NULL) {
	dor_space_deallocate(plymsh->vertex_location);
    }

    if (plymsh->vertex_normal != NULL) {
	dor_space_deallocate(plymsh->vertex_normal);
    }

    if (plymsh->vertex_color != NULL) {
	dor_space_deallocate(plymsh->vertex_color);
    }

    if (plymsh->vertex_alpha != NULL) {
	dor_space_deallocate(plymsh->vertex_alpha);
    }

    for (i=0; i< plymsh->uv_count; i++) {
	dor_space_deallocate(plymsh->uv_list[i]);
    }
    dor_space_deallocate(plymsh->uv_list);

    for (i=0; i< plymsh->uvw_count; i++) {
	dor_space_deallocate(plymsh->uvw_list[i]);
    }
    dor_space_deallocate(plymsh->uvw_list);

    if (plymsh->polygons != NULL) {
	dor_space_deallocate(plymsh->polygons);
    }

    if (plymsh->contours != NULL) {
	dor_space_deallocate(plymsh->contours);
    }

    if (plymsh->vertexlist != NULL) {
	dor_space_deallocate(plymsh->vertexlist);
    }

    if (plymsh->alt_object_lines != NULL) {
	dor_object_check_deletion(plymsh->alt_object_lines);
    }

    if (plymsh->alt_object_tris != NULL) {
	dor_object_check_deletion(plymsh->alt_object_tris);
    }

    dor_space_deallocate(plymsh);
}
/*
 ======================================================================
 */

void dor_plymsh_print (struct object *object)
{
    struct plymsh *plymsh;

    plymsh = (struct plymsh *)(object->data);

    if (plymsh == NULL){
	dor_print_output("polygon mesh is NULL");
	return;
    }

    switch (plymsh->colormodel){
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;

    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    dor_utilpri_print_vertextype(plymsh->vertextype);

    switch (plymsh->shape){
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
	    "smooth flag = %d",plymsh->smoothflag);
    dor_print_output(dor_print_get_line());
	
    sprintf(dor_print_get_line(),
	    "vertex count = %d",plymsh->vertex_count);
    dor_print_output(dor_print_get_line());
	
    sprintf(dor_print_get_line(),
	    "polygon count = %d",plymsh->polygon_count);
    dor_print_output(dor_print_get_line());

    if (plymsh->uv_count > 0) {
	sprintf (dor_print_get_line(), 
		 "uv's per vertex = %d", plymsh->uv_count);
	dor_print_output(dor_print_get_line());
    }

    if (plymsh->uvw_count > 0) {
	sprintf (dor_print_get_line(), 
		 "uvw's per vertex = %d", plymsh->uvw_count);
	dor_print_output(dor_print_get_line());
    }
}
/*
 ======================================================================
 */

dot_object *dor_plymsh_update_alternate(dot_object *object)
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
	alt = dor_plymsh_update_tris(object);
	break;

    case DcOutlines:
	alt = dor_plymsh_update_lines(object);
	break;
    default:
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		   "dor_plymsh_update_alternate",
		   "unknown reptype");
	alt = DcNullPtr;
	break;
    }
    return alt;
}
/*
 ======================================================================
 */

void dor_plymsh_create_alternate_geometry_complex_triangle (
    struct plymsh *plymsh,
    dot_gpolyd_interpolated_vertex *gpolydvtx1,
    dot_gpolyd_interpolated_vertex *gpolydvtx2,
    dot_gpolyd_interpolated_vertex *gpolydvtx3)
{
   DtVertexType vertextype;
   DtReal *vtxptr;
   DtReal vertices[MAXVTXS];
   DtInt vertex1, vertex2;
   DtReal weight1, weight2;
   DtInt uv, uvw;
   DtInt edge_enable;

   edge_enable = dor_plymsh_enable_edges_complex(plymsh, gpolydvtx1, 
                                                 gpolydvtx2, gpolydvtx3);

   vertextype = plymsh->vertextype;

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
           *vtxptr++ = weight1*plymsh->vertex_normal[vertex1][0] +
                       weight2*plymsh->vertex_normal[vertex2][0];
           *vtxptr++ = weight1*plymsh->vertex_normal[vertex1][1] +
                       weight2*plymsh->vertex_normal[vertex2][1];
           *vtxptr++ = weight1*plymsh->vertex_normal[vertex1][2] +
                       weight2*plymsh->vertex_normal[vertex2][2];
        }
       if (vertextype & DcLocClr) {
           *vtxptr++ = weight1*plymsh->vertex_color[vertex1][0] +
                       weight2*plymsh->vertex_color[vertex2][0];
           *vtxptr++ = weight1*plymsh->vertex_color[vertex1][1] +
                       weight2*plymsh->vertex_color[vertex2][1];
           *vtxptr++ = weight1*plymsh->vertex_color[vertex1][2] +
                       weight2*plymsh->vertex_color[vertex2][2];
       }
       if (vertextype & DcLocAlpha) {
           *vtxptr++ = weight1*plymsh->vertex_alpha[vertex1] +
                       weight2*plymsh->vertex_alpha[vertex2];
       }

       for (uv=0; uv < plymsh->uv_count; uv++) {
           *vtxptr++ = weight1*plymsh->uv_list[uv][vertex1][0] +
                       weight2*plymsh->uv_list[uv][vertex2][0];
           *vtxptr++ = weight1*plymsh->uv_list[uv][vertex1][1] +
                       weight2*plymsh->uv_list[uv][vertex2][1];
       }
       for (uvw=0; uvw < plymsh->uvw_count; uvw++) {
           *vtxptr++ = weight1*plymsh->uvw_list[uvw][vertex1][0] +
                       weight2*plymsh->uvw_list[uvw][vertex2][0];
           *vtxptr++ = weight1*plymsh->uvw_list[uvw][vertex1][1] +
                       weight2*plymsh->uvw_list[uvw][vertex2][1];
           *vtxptr++ = weight1*plymsh->uvw_list[uvw][vertex1][2] +
                       weight2*plymsh->uvw_list[uvw][vertex2][2];
       }

   } else {
       if (vertextype & DcLocNrm) {
           *vtxptr++ = plymsh->vertex_normal[vertex1][0];
           *vtxptr++ = plymsh->vertex_normal[vertex1][1];
           *vtxptr++ = plymsh->vertex_normal[vertex1][2];
       }
       if (vertextype & DcLocClr) {
           *vtxptr++ = plymsh->vertex_color[vertex1][0];
           *vtxptr++ = plymsh->vertex_color[vertex1][1];
           *vtxptr++ = plymsh->vertex_color[vertex1][2];
       }
       if (vertextype & DcLocAlpha) {
           *vtxptr++ = plymsh->vertex_alpha[vertex1];
       }

       for (uv=0; uv < plymsh->uv_count; uv++) {
           *vtxptr++ = plymsh->uv_list[uv][vertex1][0];
           *vtxptr++ = plymsh->uv_list[uv][vertex1][1];
       }
       for (uvw=0; uvw < plymsh->uvw_count; uvw++) {
            *vtxptr++ = plymsh->uvw_list[uvw][vertex1][0];
            *vtxptr++ = plymsh->uvw_list[uvw][vertex1][1];
       *vtxptr++ = plymsh->uvw_list[uvw][vertex1][2];
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
           *vtxptr++ = weight1*plymsh->vertex_normal[vertex1][0] +
                       weight2*plymsh->vertex_normal[vertex2][0];
           *vtxptr++ = weight1*plymsh->vertex_normal[vertex1][1] +
                       weight2*plymsh->vertex_normal[vertex2][1];
           *vtxptr++ = weight1*plymsh->vertex_normal[vertex1][2] +
                       weight2*plymsh->vertex_normal[vertex2][2];
        }
       if (vertextype & DcLocClr) {
           *vtxptr++ = weight1*plymsh->vertex_color[vertex1][0] +
                       weight2*plymsh->vertex_color[vertex2][0];
           *vtxptr++ = weight1*plymsh->vertex_color[vertex1][1] +
                       weight2*plymsh->vertex_color[vertex2][1];
           *vtxptr++ = weight1*plymsh->vertex_color[vertex1][2] +
                       weight2*plymsh->vertex_color[vertex2][2];
       }
       if (vertextype & DcLocAlpha) {
           *vtxptr++ = weight1*plymsh->vertex_alpha[vertex1] +
                       weight2*plymsh->vertex_alpha[vertex2];
       }
       for (uv=0; uv < plymsh->uv_count; uv++) {
           *vtxptr++ = weight1*plymsh->uv_list[uv][vertex1][0] +
                        weight2*plymsh->uv_list[uv][vertex2][0];
           *vtxptr++ = weight1*plymsh->uv_list[uv][vertex1][1] +
                        weight2*plymsh->uv_list[uv][vertex2][1];
       }
       for (uvw=0; uvw < plymsh->uvw_count; uvw++) {
           *vtxptr++ = weight1*plymsh->uvw_list[uvw][vertex1][0] +
                       weight2*plymsh->uvw_list[uvw][vertex2][0];
           *vtxptr++ = weight1*plymsh->uvw_list[uvw][vertex1][1] +
                       weight2*plymsh->uvw_list[uvw][vertex2][1];
           *vtxptr++ = weight1*plymsh->uvw_list[uvw][vertex1][2] +
                       weight2*plymsh->uvw_list[uvw][vertex2][2];
       }

   } else {
       if (vertextype & DcLocNrm) {
           *vtxptr++ = plymsh->vertex_normal[vertex1][0];
           *vtxptr++ = plymsh->vertex_normal[vertex1][1];
           *vtxptr++ = plymsh->vertex_normal[vertex1][2];
       }
       if (vertextype & DcLocClr) {
           *vtxptr++ = plymsh->vertex_color[vertex1][0];
           *vtxptr++ = plymsh->vertex_color[vertex1][1];
           *vtxptr++ = plymsh->vertex_color[vertex1][2];
       }
       if (vertextype & DcLocAlpha) {
           *vtxptr++ = plymsh->vertex_alpha[vertex1];
       }

       for (uv=0; uv < plymsh->uv_count; uv++) {
           *vtxptr++ = plymsh->uv_list[uv][vertex1][0];
           *vtxptr++ = plymsh->uv_list[uv][vertex1][1];
       }
       for (uvw=0; uvw < plymsh->uvw_count; uvw++) {
            *vtxptr++ = plymsh->uvw_list[uvw][vertex1][0];
            *vtxptr++ = plymsh->uvw_list[uvw][vertex1][1];
       *vtxptr++ = plymsh->uvw_list[uvw][vertex1][2];
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
           *vtxptr++ = weight1*plymsh->vertex_normal[vertex1][0] +
                       weight2*plymsh->vertex_normal[vertex2][0];
           *vtxptr++ = weight1*plymsh->vertex_normal[vertex1][1] +
                       weight2*plymsh->vertex_normal[vertex2][1];
           *vtxptr++ = weight1*plymsh->vertex_normal[vertex1][2] +
                       weight2*plymsh->vertex_normal[vertex2][2];
        }
       if (vertextype & DcLocClr) {
           *vtxptr++ = weight1*plymsh->vertex_color[vertex1][0] +
                       weight2*plymsh->vertex_color[vertex2][0];
           *vtxptr++ = weight1*plymsh->vertex_color[vertex1][1] +
                       weight2*plymsh->vertex_color[vertex2][1];
           *vtxptr++ = weight1*plymsh->vertex_color[vertex1][2] +
                       weight2*plymsh->vertex_color[vertex2][2];
       }
       if (vertextype & DcLocAlpha) {
           *vtxptr++ = weight1*plymsh->vertex_alpha[vertex1] +
                       weight2*plymsh->vertex_alpha[vertex2];
       }
       for (uv=0; uv < plymsh->uv_count; uv++) {
           *vtxptr++ = weight1*plymsh->uv_list[uv][vertex1][0] +
                        weight2*plymsh->uv_list[uv][vertex2][0];
           *vtxptr++ = weight1*plymsh->uv_list[uv][vertex1][1] +
                        weight2*plymsh->uv_list[uv][vertex2][1];
       }
       for (uvw=0; uvw < plymsh->uvw_count; uvw++) {
           *vtxptr++ = weight1*plymsh->uvw_list[uvw][vertex1][0] +
                       weight2*plymsh->uvw_list[uvw][vertex2][0];
           *vtxptr++ = weight1*plymsh->uvw_list[uvw][vertex1][1] +
                       weight2*plymsh->uvw_list[uvw][vertex2][1];
           *vtxptr++ = weight1*plymsh->uvw_list[uvw][vertex1][2] +
                       weight2*plymsh->uvw_list[uvw][vertex2][2];
       }

   } else {
       if (vertextype & DcLocNrm) {
           *vtxptr++ = plymsh->vertex_normal[vertex1][0];
           *vtxptr++ = plymsh->vertex_normal[vertex1][1];
           *vtxptr++ = plymsh->vertex_normal[vertex1][2];
       }
       if (vertextype & DcLocClr) {
           *vtxptr++ = plymsh->vertex_color[vertex1][0];
           *vtxptr++ = plymsh->vertex_color[vertex1][1];
           *vtxptr++ = plymsh->vertex_color[vertex1][2];
       }
       if (vertextype & DcLocAlpha) {
           *vtxptr++ = plymsh->vertex_alpha[vertex1];
       }

       for (uv=0; uv < plymsh->uv_count; uv++) {
           *vtxptr++ = plymsh->uv_list[uv][vertex1][0];
           *vtxptr++ = plymsh->uv_list[uv][vertex1][1];
       }
       for (uvw=0; uvw < plymsh->uvw_count; uvw++) {
            *vtxptr++ = plymsh->uvw_list[uvw][vertex1][0];
            *vtxptr++ = plymsh->uvw_list[uvw][vertex1][1];
       *vtxptr++ = plymsh->uvw_list[uvw][vertex1][2];
       }
   }

   dor_trilst_add_triangle_with_edges(plymsh->alt_object_tris,
                                      vertices, edge_enable);
}

/*
 ======================================================================
 */

dot_object *dor_plymsh_create_alternate_geom_lines (struct plymsh *plymsh)
{
    DtReal vtxs[MAXVTXS_LIN];
    DtUInt i;
    DtUInt uv, uvw;
    DtInt contourlistloc;
    DtInt vertexlistloc;
    DtInt polygonno;
    DtInt contourno;
    DtInt contourvertexcount;
    DtInt contourvertexno;
    DtInt lvtx0, lvtx1;
    DtInt firstvertex, lastvertex;
    dot_object *linlstobj; 
    DtInt j,sum;

    contourlistloc = 0;
    for (i=0,sum=0;i<plymsh->polygon_count;i++) {
	for (j=0; j<plymsh->polygons[i]; j++) {
	    sum += plymsh->contours[contourlistloc++];
	}
    }

    linlstobj = dor_linlst_open(plymsh->colormodel,plymsh->fullvtxtype,sum);
   
    plymsh->alt_object_lines = linlstobj;

    contourlistloc = 0;
    vertexlistloc = 0;

    for (polygonno=0;polygonno<plymsh->polygon_count;polygonno++) {

	for (contourno=0;
	     contourno<plymsh->polygons[polygonno];contourno++) {

	    contourvertexcount = plymsh->contours[contourlistloc++];
	    firstvertex = plymsh->vertexlist[vertexlistloc];
	    lastvertex = plymsh->vertexlist[vertexlistloc+contourvertexcount-1];

	    for (contourvertexno=0;
		 contourvertexno<contourvertexcount;
		 contourvertexno++) {

		if (contourvertexno == contourvertexcount-1) {
		    lvtx0 = lastvertex;
		    lvtx1 = firstvertex;
		} else {
		    lvtx0 = plymsh->vertexlist[vertexlistloc++];
		    lvtx1 = plymsh->vertexlist[vertexlistloc];
		}

		i = 0;

		/* One end of a line */

		vtxs[i++] = plymsh->vertex_location[lvtx0][0];
                vtxs[i++] = plymsh->vertex_location[lvtx0][1];
                vtxs[i++] = plymsh->vertex_location[lvtx0][2];

                if (plymsh->vertextype & DcLocNrm) {
		    vtxs[i++] = plymsh->vertex_normal[lvtx0][0];
		    vtxs[i++] = plymsh->vertex_normal[lvtx0][1];
		    vtxs[i++] = plymsh->vertex_normal[lvtx0][2];
                }

                if (plymsh->vertextype & DcLocClr) {
		    vtxs[i++] = plymsh->vertex_color[lvtx0][0];
		    vtxs[i++] = plymsh->vertex_color[lvtx0][1];
		    vtxs[i++] = plymsh->vertex_color[lvtx0][2];
                }

                if (plymsh->vertextype & DcLocAlpha) {
		    vtxs[i++] = plymsh->vertex_alpha[lvtx0];
                }

		for (uv=0; uv<plymsh->uv_count; uv++) {
		    vtxs[i++] = plymsh->uv_list[uv][lvtx0][0]; 
		    vtxs[i++] = plymsh->uv_list[uv][lvtx0][1]; 
                }

                for (uvw=0; uvw<plymsh->uvw_count; uvw++) {
		    vtxs[i++] = plymsh->uvw_list[uvw][lvtx0][0]; 
		    vtxs[i++] = plymsh->uvw_list[uvw][lvtx0][1]; 
		    vtxs[i++] = plymsh->uvw_list[uvw][lvtx0][2]; 
                }

		/* The other end of the line */

		vtxs[i++] = plymsh->vertex_location[lvtx1][0];
                vtxs[i++] = plymsh->vertex_location[lvtx1][1];
                vtxs[i++] = plymsh->vertex_location[lvtx1][2];
 
                if (plymsh->vertextype & DcLocNrm) {
		    vtxs[i++] = plymsh->vertex_normal[lvtx1][0];
		    vtxs[i++] = plymsh->vertex_normal[lvtx1][1];
		    vtxs[i++] = plymsh->vertex_normal[lvtx1][2];
                }
 
                if (plymsh->vertextype & DcLocClr) {
		    vtxs[i++] = plymsh->vertex_color[lvtx1][0];
		    vtxs[i++] = plymsh->vertex_color[lvtx1][1];
		    vtxs[i++] = plymsh->vertex_color[lvtx1][2];
                }

                if (plymsh->vertextype & DcLocAlpha) {
		    vtxs[i++] = plymsh->vertex_alpha[lvtx1];
                }

		for (uv=0; uv<plymsh->uv_count; uv++) {
		    vtxs[i++] = plymsh->uv_list[uv][lvtx1][0]; 
		    vtxs[i++] = plymsh->uv_list[uv][lvtx1][1]; 
                }
         
                for (uvw=0; uvw<plymsh->uvw_count; uvw++) {
		    vtxs[i++] = plymsh->uvw_list[uvw][lvtx1][0]; 
		    vtxs[i++] = plymsh->uvw_list[uvw][lvtx1][1]; 
		    vtxs[i++] = plymsh->uvw_list[uvw][lvtx1][2]; 
                }

		/* Add the line to the list */

		dor_linlst_add_line(linlstobj,vtxs);

	    }

	    vertexlistloc++; 

	}
    }

    dor_linlst_close(linlstobj);

    return(linlstobj);
}
/*
 ======================================================================
 */

void dor_plymsh_create_alternate_geometry_simple_triangle (
    struct plymsh *plymsh,
    DtInt vertex1,
    DtInt vertex2,
    DtInt vertex3)
{
   DtVertexType vertextype;
   DtReal *vtxptr;
   DtReal vertices[MAXVTXS];
   DtInt uv, uvw;
   DtInt edge_enable;

   edge_enable = dor_plymsh_enable_edges_simple(plymsh,vertex1,vertex2,vertex3);

   vertextype = plymsh->vertextype;

   vtxptr = vertices;

   /* First vertex */

   *vtxptr++ = plymsh->vertex_location[vertex1][0];
   *vtxptr++ = plymsh->vertex_location[vertex1][1];
   *vtxptr++ = plymsh->vertex_location[vertex1][2];

   if (vertextype & DcLocNrm) {
       *vtxptr++ = plymsh->vertex_normal[vertex1][0];
       *vtxptr++ = plymsh->vertex_normal[vertex1][1];
       *vtxptr++ = plymsh->vertex_normal[vertex1][2];
   }
   if (vertextype & DcLocClr) {
       *vtxptr++ = plymsh->vertex_color[vertex1][0];
       *vtxptr++ = plymsh->vertex_color[vertex1][1];
       *vtxptr++ = plymsh->vertex_color[vertex1][2];
   }
   if (vertextype & DcLocAlpha) {
       *vtxptr++ = plymsh->vertex_alpha[vertex1];
   }


   for (uv=0; uv<plymsh->uv_count; uv++) {
       *vtxptr++ = plymsh->uv_list[uv][vertex1][0]; 
       *vtxptr++ = plymsh->uv_list[uv][vertex1][1]; 
   }

   for (uvw=0; uvw<plymsh->uvw_count; uvw++) {
       *vtxptr++ = plymsh->uvw_list[uvw][vertex1][0]; 
       *vtxptr++ = plymsh->uvw_list[uvw][vertex1][1]; 
       *vtxptr++ = plymsh->uvw_list[uvw][vertex1][2]; 
   }

   /* Second vertex */

   *vtxptr++ = plymsh->vertex_location[vertex2][0];
   *vtxptr++ = plymsh->vertex_location[vertex2][1];
   *vtxptr++ = plymsh->vertex_location[vertex2][2];

   if (vertextype & DcLocNrm) {
       *vtxptr++ = plymsh->vertex_normal[vertex2][0];
       *vtxptr++ = plymsh->vertex_normal[vertex2][1];
       *vtxptr++ = plymsh->vertex_normal[vertex2][2];
   }
   if (vertextype & DcLocClr) {
       *vtxptr++ = plymsh->vertex_color[vertex2][0];
       *vtxptr++ = plymsh->vertex_color[vertex2][1];
       *vtxptr++ = plymsh->vertex_color[vertex2][2];
   }
   if (vertextype & DcLocAlpha) {
       *vtxptr++ = plymsh->vertex_alpha[vertex2];
   }


   for (uv=0; uv<plymsh->uv_count; uv++) {
       *vtxptr++ = plymsh->uv_list[uv][vertex2][0]; 
       *vtxptr++ = plymsh->uv_list[uv][vertex2][1]; 
   }

   for (uvw=0; uvw<plymsh->uvw_count; uvw++) {
       *vtxptr++ = plymsh->uvw_list[uvw][vertex2][0]; 
       *vtxptr++ = plymsh->uvw_list[uvw][vertex2][1]; 
       *vtxptr++ = plymsh->uvw_list[uvw][vertex2][2]; 
   }
    /* Third vertex */

   *vtxptr++ = plymsh->vertex_location[vertex3][0];
   *vtxptr++ = plymsh->vertex_location[vertex3][1];
   *vtxptr++ = plymsh->vertex_location[vertex3][2];

   if (vertextype & DcLocNrm) {
       *vtxptr++ = plymsh->vertex_normal[vertex3][0];
       *vtxptr++ = plymsh->vertex_normal[vertex3][1];
       *vtxptr++ = plymsh->vertex_normal[vertex3][2];
   }
   if (vertextype & DcLocClr) {
       *vtxptr++ = plymsh->vertex_color[vertex3][0];
       *vtxptr++ = plymsh->vertex_color[vertex3][1];
       *vtxptr++ = plymsh->vertex_color[vertex3][2];
   }
   if (vertextype & DcLocAlpha) {
       *vtxptr++ = plymsh->vertex_alpha[vertex3];
   }


   for (uv=0; uv<plymsh->uv_count; uv++) {
       *vtxptr++ = plymsh->uv_list[uv][vertex3][0]; 
       *vtxptr++ = plymsh->uv_list[uv][vertex3][1]; 
   }

   for (uvw=0; uvw<plymsh->uvw_count; uvw++) {
       *vtxptr++ = plymsh->uvw_list[uvw][vertex3][0]; 
       *vtxptr++ = plymsh->uvw_list[uvw][vertex3][1]; 
       *vtxptr++ = plymsh->uvw_list[uvw][vertex3][2]; 
   }

   /* Add triangle to list */

   dor_trilst_add_triangle_with_edges(plymsh->alt_object_tris,
                                      vertices,edge_enable);

}
/*
 ======================================================================
 */

DtInt doe_plymsh_polygonno;

dot_object *dor_plymsh_create_alternate_geom_tris(struct plymsh *plymsh)
{
    dot_object *trilstobj;
    DtInt contourlistloc;
    DtInt vertexlistloc;
    DtInt polygonno;
    DtInt contourno;
    DtInt vertexno;
    DtInt contourvertexcount;
    DtInt contourvertexno;
    DtInt i,j,sum;

    contourlistloc = 0;
    for (i=0,sum=0;i<plymsh->polygon_count;i++) {
	for (j=0; j<plymsh->polygons[i]; j++) {
	    sum += plymsh->contours[contourlistloc++];
	}
    }
    if (plymsh->shape == DcComplex) sum *= 2;

    trilstobj = dor_trilst_open(DcRGB,plymsh->fullvtxtype,sum);

    plymsh->alt_object_tris = trilstobj;

    contourlistloc = 0;
    vertexlistloc = 0;

    for (polygonno=0;polygonno<plymsh->polygon_count;polygonno++) {
   	dor_gpolyd_create_polygon();

   	for (contourno=0;contourno<plymsh->polygons[polygonno];contourno++) {
	    dor_gpolyd_create_contour();
   
	    contourvertexcount = plymsh->contours[contourlistloc++];

	    for (contourvertexno=0;
		 contourvertexno<contourvertexcount;
		 contourvertexno++) {
		vertexno = plymsh->vertexlist[vertexlistloc++];

		dor_gpolyd_create_contour_vertex(vertexno,
			plymsh->vertex_location[vertexno][0],
			plymsh->vertex_location[vertexno][1],
			plymsh->vertex_location[vertexno][2]);
	    }
   
	    dor_gpolyd_close_contour();
   	}

   	dor_gpolyd_close_polygon();

	doe_plymsh_polygonno = polygonno;
   	dor_gpolyd_triangulate_polygon(plymsh->shape, 
			dor_plymsh_create_alternate_geometry_simple_triangle,
			dor_plymsh_create_alternate_geometry_complex_triangle,
			plymsh);
    }

    dor_trilst_close(trilstobj);

    return(trilstobj);
}
/*
 ======================================================================
 */

void dor_plymsh_calculate_vertex_normals (struct plymsh *plymsh)
{
    DtInt *vertexrefcount;
    DtInt vertexno;
    DtInt polygonno;
    DtInt contourlistloc;
    DtInt vertexlistloc;
    DtInt vertex1, vertex2, vertex3;
    DtRealTriple polygonnormal;
    DtInt contourno;
    DtInt contourvertexno;
    DtInt contourvertexcount;

    /*  Allocate a table of polygon reference counts, one for each vertex.  */

    vertexrefcount = (DtInt *)dor_space_allocate
		(plymsh->vertex_count*(sizeof (DtInt)));

    /*  Zero the vertex normals and polygon reference counts for
	all vertices.  */

    for (vertexno=0;vertexno<plymsh->vertex_count;vertexno++){
	vertexrefcount[vertexno] = 0;

	plymsh->vertex_normal[vertexno][0] = 0.;
	plymsh->vertex_normal[vertexno][1] = 0.;
	plymsh->vertex_normal[vertexno][2] = 0.;
    }

    contourlistloc = 0;
    vertexlistloc = 0;

    for (polygonno=0;polygonno<plymsh->polygon_count;polygonno++){
	/*  Compute the polygon normal.  */

	vertex1 = plymsh->vertexlist[vertexlistloc+0];
	vertex2 = plymsh->vertexlist[vertexlistloc+1];
	vertex3 = plymsh->vertexlist[vertexlistloc+2];

	dor_math_fast_cross_cosines(
				    plymsh->vertex_location[vertex1],
				    plymsh->vertex_location[vertex2],
				    plymsh->vertex_location[vertex3],
				    polygonnormal);

	/*  For each of the polygon's vertices, update that vertex's
	    polygon reference count and accumulated polygon normal.  This
	    also bumps the vertexlistloc value to the first vertex of the
	    next polygon.  */

	for (contourno=0;contourno<plymsh->polygons[polygonno];contourno++){
	    contourvertexcount = plymsh->contours[contourlistloc++];

	    for (contourvertexno=0;
		 contourvertexno<contourvertexcount;
		 contourvertexno++){
		vertexno = plymsh->vertexlist[vertexlistloc++];

		vertexrefcount[vertexno]++;

		plymsh->vertex_normal[vertexno][0] += polygonnormal[0];
		plymsh->vertex_normal[vertexno][1] += polygonnormal[1];
		plymsh->vertex_normal[vertexno][2] += polygonnormal[2];
	    }
	}
    }

    /*  Rescale the vertex normals.  */

    for (vertexno=0;vertexno<plymsh->vertex_count;vertexno++){
	plymsh->vertex_normal[vertexno][0] /= vertexrefcount[vertexno];
	plymsh->vertex_normal[vertexno][1] /= vertexrefcount[vertexno];
	plymsh->vertex_normal[vertexno][2] /= vertexrefcount[vertexno];

	if (!dor_math_renormalize_vector(plymsh->vertex_normal[vertexno])){
	    DDerror (ERR_ZERO_TRI_NRM_SUM,
			"dor_plymsh_calculate_vertex_normals", 
			DcNullPtr);
	}
    }

    /*  Free the vertex polygon reference count table.  */

    dor_space_deallocate(vertexrefcount);
}
/*
 ======================================================================
 */

void dor_plymsh_compute_bounding_volume (dot_object *object)
{
    struct plymsh *plymsh;

    plymsh = (struct plymsh *)(object->data);

    if (plymsh->alt_object_tris != DcNullPtr) {
	dor_object_get_method(plymsh->alt_object_tris,
			      DcMethodCmpBndVolume)(plymsh->alt_object_tris);
    } else {
	dor_system_report_bounding_points(plymsh->vertex_count,
					  (DtReal*)plymsh->vertex_location);
    }
}
/*
 ======================================================================
 */

DtInt dor_plymsh_enable_edges_complex (
    struct plymsh *plymsh,
    dot_gpolyd_interpolated_vertex *gpolydvtx1,
    dot_gpolyd_interpolated_vertex *gpolydvtx2,
    dot_gpolyd_interpolated_vertex *gpolydvtx3)
{
    DtInt edge_enable;

    /* Enable triangle edges on the polygon outlines */

    edge_enable = dod_no_enabled_edges;
    if (dor_plymsh_check_complex_edge(plymsh, gpolydvtx1, gpolydvtx2))
	    edge_enable |= dod_edge1_enabled;
    if (dor_plymsh_check_complex_edge(plymsh, gpolydvtx2, gpolydvtx3))
	    edge_enable |= dod_edge2_enabled;
    if (dor_plymsh_check_complex_edge(plymsh, gpolydvtx3, gpolydvtx1))
	    edge_enable |= dod_edge3_enabled;
#ifdef DEBUG
    printf("edge_enable %d \n", edge_enable);
#endif
   
    return (edge_enable);
}


DtFlag dor_plymsh_check_complex_edge (
    struct plymsh *plymsh,
    dot_gpolyd_interpolated_vertex *vtx1,
    dot_gpolyd_interpolated_vertex *vtx2)
{
    DtInt contourcount;
    static DtInt vtx_space = 0;
    static DtInt *vtx1_no = NULL;
    static DtInt *vtx2_no = NULL;
    DtInt vtx1_no_count, vtx2_no_count;
    DtInt vertexlist_start;
    DtInt contours_start;
    DtInt vertexcount;
    DtInt mshvertex;
    DtInt i,j,k;

#ifdef DEBUG
    printf("vtx1->vertex1 %d  vtx2->vertex1 %d \n",vtx1->vertex1,  vtx2->vertex1);
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
	contourcount = plymsh->polygons[doe_plymsh_polygonno];
   	vertexlist_start = 0;
   	contours_start = 0;
   	k = 0;
   	for (i=0; i<doe_plymsh_polygonno; i++) {
	    for (j=0; j<plymsh->polygons[i]; j++) {
		vertexlist_start += plymsh->contours[k++];
	    }
	    contours_start += plymsh->polygons[i];
   	}
#ifdef DEBUG
        printf("contourcount %d  contours_start %d\n", contourcount,contours_start);
#endif

   	for (i=0; i<contourcount; i++) {
	    vertexcount = plymsh->contours[contours_start+i];
#ifdef DEBUG
	    printf("i %d vertexlist_start %d  vertexcount %d\n", i,vertexlist_start,vertexcount);
#endif
	    if (vertexcount > vtx_space) {
		vtx1_no = (DtInt *)dor_space_reallocate
				(vtx1_no,vertexcount*(sizeof(DtInt)));
		vtx2_no = (DtInt *)dor_space_reallocate
				(vtx2_no,vertexcount*(sizeof(DtInt)));
		vtx_space = vertexcount;
	    }

	    vtx1_no_count = 0;
	    vtx2_no_count = 0;
	    for (j=0; j<vertexcount; j++) {
		mshvertex = plymsh->vertexlist[vertexlist_start+j];
#ifdef DEBUG
		printf("mshvertex %d\n", mshvertex);
#endif
		if (vtx1->vertex1 == mshvertex) {
		    vtx1_no[vtx1_no_count++] = j;
		}
		if (vtx2->vertex1 == mshvertex) {
		    vtx2_no[vtx2_no_count++] = j;
		}
	    }
#ifdef DEBUG
	    printf("vtx1_no_count %d vtx2_no_count %d\n", vtx1_no_count, vtx2_no_count);
#endif

	    for (j=0; j<vtx1_no_count; j++) {
		for (k=0; k<vtx2_no_count; k++) {
		    if (vtx1_no[j] == ((vtx2_no[k]+1) % vertexcount) ||
			vtx2_no[k] == ((vtx1_no[j]+1) % vertexcount)) {
			return(DcTrue);
		    }
		}
	    }

	    vertexlist_start += vertexcount;
	}
    }
   
    return (DcFalse);
}
/*
 ======================================================================
 */

DtInt dor_plymsh_enable_edges_simple (
    struct plymsh *plymsh,
    DtInt vertex1,
    DtInt vertex2,
    DtInt vertex3)
{
    DtInt edge_enable;
    DtInt contourcount;
    static DtInt vtx_space = 0;
    static DtInt *vtx1_no = NULL;
    static DtInt *vtx2_no = NULL; 
    static DtInt *vtx3_no = NULL; 
    DtInt vtx1_no_count, vtx2_no_count, vtx3_no_count;
    DtInt vertexlist_start;
    DtInt contours_start;
    DtInt vertexcount;
    DtInt mshvertex;
    DtInt i,j,k;

    contourcount = plymsh->polygons[doe_plymsh_polygonno];
    vertexlist_start = 0;
    contours_start = 0;
    k = 0;
    for (i=0; i<doe_plymsh_polygonno; i++) {
	for (j=0; j<plymsh->polygons[i]; j++) {
	    vertexlist_start += plymsh->contours[k++];
	}
	contours_start += plymsh->polygons[i];
    }

    for (i=0; i<contourcount; i++) {
	vertexcount = plymsh->contours[contours_start+i];
	if (vertexcount > vtx_space) {
	    vtx1_no = (DtInt *)dor_space_reallocate
			(vtx1_no,vertexcount*(sizeof(DtInt)));
	    vtx2_no = (DtInt *)dor_space_reallocate
			(vtx2_no,vertexcount*(sizeof(DtInt)));
	    vtx3_no = (DtInt *)dor_space_reallocate
			(vtx3_no,vertexcount*(sizeof(DtInt)));	
	    vtx_space = vertexcount;
	}

	vtx1_no_count = 0;
	vtx2_no_count = 0;
	vtx3_no_count = 0;
	for (j=0; j<vertexcount; j++) {
	    mshvertex = plymsh->vertexlist[vertexlist_start+j];
	    if (vertex1 == mshvertex) {
		vtx1_no[vtx1_no_count++] = j;
	    }
	    if (vertex2 == mshvertex) {
		vtx2_no[vtx2_no_count++] = j;
	    }
	    if (vertex3 == mshvertex) {
		vtx3_no[vtx3_no_count++] = j;
	    }
	}

	edge_enable = dod_no_enabled_edges;

	for (j=0; j<vtx1_no_count; j++) {
	    for (k=0; k<vtx2_no_count; k++) {
		if (vtx1_no[j] == ((vtx2_no[k]+1) % vertexcount) ||
		    vtx2_no[k] == ((vtx1_no[j]+1) % vertexcount)) {
		    edge_enable |= dod_edge1_enabled;
		    break;
		} 
	    }
	    for (k=0; k<vtx3_no_count; k++) {
		if (vtx1_no[j] == ((vtx3_no[k]+1) % vertexcount) ||
		    vtx3_no[k] == ((vtx1_no[j]+1) % vertexcount)) {
		    edge_enable |= dod_edge3_enabled;
		    break;
		} 
	    }
	}
	for (j=0; j<vtx2_no_count; j++) {
	    for (k=0; k<vtx3_no_count; k++) {
		if (vtx2_no[j] == ((vtx3_no[k]+1) % vertexcount) ||
		    vtx3_no[k] == ((vtx2_no[j]+1) % vertexcount)) {
		    edge_enable |= dod_edge2_enabled;
		    break;
		} 
	    }
   	}

	vertexlist_start += vertexcount;
    }
   
    return (edge_enable);
}
/*
 ======================================================================
 */

void dor_plymsh_pick (dot_object *object)
{
    DtRepType reptype;

    if (!dor_condex_query_pick(doe_plymsh_class_id))
	    return;

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    case DcOutlines:
	dor_plymsh_pick_lines(object);
	break;

    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
    case DcWireframe:
    case DcPoints:
	dor_plymsh_pick_tris(object);
	break;
    }
}
/*
 ======================================================================
 */

void dor_plymsh_pick_lines (dot_object *object)
{
    struct plymsh *plymsh;

    plymsh = (struct plymsh *)(object->data);

    if (plymsh->alt_object_lines != DcNullPtr) {
	dor_object_get_method(plymsh->alt_object_lines,
			      DcMethodPick)(plymsh->alt_object_lines);

	return;
    } else {
	plymsh->alt_object_lines = 
		dor_plymsh_create_alternate_geom_lines(plymsh);

	if (plymsh->alt_object_lines == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_plymsh_create_alternate_geom_lines", "");

	    return;
	}

	dor_object_get_method(plymsh->alt_object_lines,
			      DcMethodPick)(plymsh->alt_object_lines);

	return;
    }
}
/*
 ======================================================================
 */

void dor_plymsh_pick_tris (dot_object *object)
{
    struct plymsh *plymsh;

    plymsh = (struct plymsh *)(object->data);

    if (plymsh->alt_object_tris != DcNullPtr) {
	dor_object_get_method(plymsh->alt_object_tris,
			      DcMethodPick)(plymsh->alt_object_tris);

	return;
    } else {
	plymsh->alt_object_tris = 
		dor_plymsh_create_alternate_geom_tris(plymsh);

	if (plymsh->alt_object_tris == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_plymsh_create_alternate_geom_tris", "");
	    return;
	}

	dor_object_get_method(plymsh->alt_object_tris,
			      DcMethodPick)(plymsh->alt_object_tris);

	return;
    }
}
/*
 ======================================================================
 */

void dor_plymsh_render_standard (dot_object *object)
{
    dot_object *alt;

    if (!dor_condex_query_render(doe_plymsh_class_id))
	    return;

    alt = dor_plymsh_update_alternate (object);
    if (alt != DcNullPtr) {
	dor_object_get_current_method(alt) (alt);
    }
}
/*
 ======================================================================
 */

dot_object *dor_plymsh_update_lines (dot_object *object)
{
    struct plymsh *plymsh;
    dot_object *alt;

    plymsh = (struct plymsh *)(object->data);

    if (plymsh->alt_object_lines != DcNullPtr) {
	alt = plymsh->alt_object_lines;

    } else {
	plymsh->alt_object_lines = 
		dor_plymsh_create_alternate_geom_lines(plymsh);
	alt = plymsh->alt_object_lines;

	if (plymsh->alt_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_plymsh_update_lines", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */

dot_object *dor_plymsh_update_tris (dot_object *object)
{
    struct plymsh *plymsh;
    dot_object *alt;

    plymsh = (struct plymsh *)(object->data);

    if (plymsh->alt_object_tris != DcNullPtr) {
	alt = plymsh->alt_object_tris;

    } else {
	plymsh->alt_object_tris = 
		dor_plymsh_create_alternate_geom_tris(plymsh);
	alt = plymsh->alt_object_tris;

	if (plymsh->alt_object_tris == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_plymsh_update_tris", "");
	}
    }
    return alt;
}
